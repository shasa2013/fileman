#include "count.h"

BOOL countSelected (HWND hLv)
{
	// Проверить флаг "идёт подсчёт"
	// HWND hPanel = GetDlgItem (hDlg, panelId);
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	if (props->сountedItem != -1)
	{
		return FALSE;
	}
	
	TCHAR fileName [MAX_PATH];
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;	// Флаг - получаем LPARAM
	lvi.iItem = SendMessage (hLv, LVM_GETSELECTIONMARK, 0, 0);
	if (lvi.iItem == -1) return FALSE;
	
	// Сохранить индекс строки
	props->сountedItem = lvi.iItem;

	// Создать список начального пути для задания "Подсчёт"
	Task* countTask = new Task ();
	countTask->getProgress()->itemId = GetWindowLong (hLv, GWL_ID);
	HWND hDlg = (HWND) GetWindowLong (hLv, GWL_HWNDPARENT);
	countTask->getProgress()->hDlg = hDlg;
	
	// Имя файла
	SendMessage (hLv, LVM_GETITEM, 0, (LPARAM)&lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
	countTask->addFileName (findData->cFileName);

	// Папка источник
	SendMessage (props->hFullPath, WM_GETTEXT, (WPARAM) SIZETC (fileName), (LPARAM)fileName);
	countTask->setSrcFolder (fileName);
	
	// Cоздать рабочий поток выполнения задания "Подсчёт" в спящем состоянии
	HANDLE hCountThread = CreateThread (NULL, NULL, countWorker, countTask, CREATE_SUSPENDED, NULL);
	if (hCountThread == NULL)
	{
		delete countTask;
		return FALSE;
	}
	
	// Запретить смену диска
	EnableWindow (props->hFullPath, FALSE);

	// Сохранить дескриптор потока операции
	setOperationThread (hLv, hCountThread, TRUE); 	

	// Запустить выполнение задания "Подсчёт"
	resumeOperationThread (hLv);
	return TRUE;
}


DWORD WINAPI countWorker (VOID* data)
{
	Task* task = (Task*) data;
	WIN32_FIND_DATA findData;
	DWORD result = 0;
	TCHAR tmpSrc [FM_MAX_PATH];
	UINT64 bytesCount = 0;
	while (task->nextPath()) 
	{
		lstrcpy (tmpSrc, task->getCurSrcPath());
		result = recurseOperation (&findData, tmpSrc, NULL,
			NULL, (void*)task->getProgress(), 0, FM_OPERATION_COUNT, &bytesCount);
	}
	SendMessage (task->getProgress()->hDlg, FM_COUNT_FINISHED, (WPARAM)tmpSrc, (LPARAM) task);
	return result;
}



INT_PTR onCountFinished (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	Task* task = (Task*) lParam;
	HWND hPanel = GetDlgItem (hDlg, task->getProgress()->itemId);
	FmPanelProps* panelProps = (FmPanelProps*) GetProp (hPanel, L"Props");
	LVITEM lvi;
	lvi.iSubItem = FM_COLUMN_SIZE;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = panelProps->сountedItem;
	SendMessage (hPanel, LVM_GETITEM, 0, (LPARAM) &lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
	TCHAR pszText [64];
	DWORDLONG largeSize = DWORDLONG (findData->nFileSizeLow) |
						 (DWORDLONG (findData->nFileSizeHigh) << 32);
	StrFormatByteSize (largeSize, pszText, SIZETC (pszText));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = pszText;
	SendMessage (hPanel, LVM_SETITEM, 0, (LPARAM)&lvi);

	lvi.mask = LVIF_STATE;
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
	lvi.iSubItem = 0;
	SendMessage (hPanel, LVM_SETITEM, 0, (LPARAM)&lvi);

	removeOperationThread (hPanel);
	panelProps->сountedItem = -1;
	EnableWindow (panelProps->hFullPath, TRUE);	

	delete task;
	return NULL;
}




INT_PTR onCountProgress (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	WaitForSingleObject (dlgMutex, INFINITE);
	
	HWND hPanel = GetDlgItem (hDlg, wParam);
	LVITEM lvi;
	FmPanelProps* panelProps = (FmPanelProps*) GetProp (hPanel, L"Props");
	if (panelProps == NULL)
	{
		printfd ("onCountProgress:panelProps is NULL\n");
		return NULL;
	}
	lvi.iItem = panelProps->сountedItem;
	if (lvi.iItem == -1)
	{
		printfd ("onCountProgress:panelProps->сountedItem:%d\n", lvi.iItem);
		return NULL;
	}

	// Взять структуру WIN32_FIND_DATA
	lvi.mask = LVIF_PARAM;
	SendMessage (hPanel, LVM_GETITEM, 0, (LPARAM)&lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
	if (findData == NULL)
	{
		printfd ("onCountProgress: нет WIN32_FIND_DATA* для %d сторки\n", lvi.iItem);
		return NULL;
	}
		
	// Занести размер файла
	DWORDLONG largeSize = * (DWORDLONG*) lParam;
	findData->nFileSizeHigh = (DWORD) (largeSize >> 32);
	findData->nFileSizeLow = (DWORD) (largeSize & 0xffffffff);
	
	ReleaseMutex (dlgMutex);
	return NULL;
}
