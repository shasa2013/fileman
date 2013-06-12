#include "delete.h"



INT CALLBACK deleteDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
    {
        case WM_COMMAND:
			switch(LOWORD(wParam))
            {
                case IDCANCEL:
					onDeleteDialogCancel (hDlg, wParam, lParam);
					return (INT) TRUE;
				case IDC_PAUSE_TOGGLE:
					onDeletePauseToggle (hDlg, wParam, lParam);
					return (INT) TRUE;
			}
			break;
		case FM_DELETE_FINISHED:
			onDeleteDialogFinished (hDlg, wParam, lParam);
			return (INT) TRUE;
		case WM_INITDIALOG:
			onDeleteDialogInit (hDlg, wParam, lParam);
			return (INT) TRUE;
	}
	return (INT) FALSE;
}



DWORD WINAPI deleteWorker (VOID* data)
{
	Task* task = (Task*) data;
	INT result = 0;
	WIN32_FIND_DATA findData;
	TCHAR tmpSrc [FM_MAX_PATH];
	task->getProgress()->path = tmpSrc;
	while (task->nextPath()) 
	{
		lstrcpy (tmpSrc, task->getCurSrcPath());
		result = recurseOperation (&findData, tmpSrc, NULL, deleteProgressRoutine,
			(void*)task->getProgress(), COPY_FILE_RESTARTABLE, FM_OPERATION_DELETE, NULL);
	}
	SendMessage (task->getProgress()->hDlg, FM_DELETE_FINISHED, 0, (LPARAM) data);
	return result;
}



INT_PTR onDeleteDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}



INT_PTR onDeleteDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR iniFileName [FM_MAX_PATH];
	getIniFileName (iniFileName, sizeof (iniFileName));
	saveDlgItemProperties (hDlg, IDD_DELETE_PROGRESS, iniFileName);
	DestroyWindow (hDlg);
	removeOperationThread (hDlg);
	delete (Task*) lParam;
	return 0;
}


// Вызывается при нажатии кнопки "Пауза" в диалоге удаления
INT_PTR onDeletePauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


// Удаляет выбранные файлы
BOOL deleteSelected (HWND hDlg, UINT fullPathId, UINT panelId)
{
	TCHAR fileName [MAX_PATH];
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;

	// Создать список начальных путей для задания "Удаление"
	Task* deleteTask = new Task ();
	Progress* progress = deleteTask->getProgress();
	// Создать список имён файлов из выделенных 
	lvi.iItem = -1;
	do 
	{
		lvi.iItem = (INT) SendDlgItemMessage (hDlg, panelId, LVM_GETNEXTITEM, (WPARAM)lvi.iItem, (LPARAM)LVNI_SELECTED);
		SendDlgItemMessage (hDlg, panelId, LVM_GETITEM, 0, (LPARAM)&lvi);
		WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
		if (lstrcmp (findData->cFileName, L"..") == 0) continue;
		deleteTask->addFileName (findData->cFileName);
	} while (lvi.iItem != -1);


	// Папка источник
	fileName [0] = L'\0';
	SendDlgItemMessage (hDlg, fullPathId, WM_GETTEXT, (WPARAM)sizeof(fileName), (LPARAM)fileName);
	deleteTask->setSrcFolder (fileName);
	
	// Cоздать рабочий поток выполнения задания "Удаление" в спящем состоянии
	HANDLE hDeleteThread = CreateThread (NULL, NULL, deleteWorker, deleteTask, CREATE_SUSPENDED, NULL);
	if (hDeleteThread == NULL) return FALSE;
	
	SetThreadPriority (hDeleteThread, THREAD_PRIORITY_IDLE);
	
	// Создать диалог отображающий выполнение задания "Удаление"
	LONG hInst = GetWindowLong (hDlg, GWL_HINSTANCE);
	progress->hDlg = CreateDialogParam ((HINSTANCE)hInst, MAKEINTRESOURCE(IDD_DELETE_PROGRESS), hDlg, deleteDlgProc, 0);
	if (progress->hDlg == NULL) return FALSE;
	
	SendDlgItemMessage (progress->hDlg, IDC_COPY_DEST, WM_SETTEXT, 0, (LPARAM)deleteTask->getDstFolder());
		
	getIniFileName (fileName, sizeof (fileName));
	loadDlgItemProperties (progress->hDlg, IDD_DELETE_PROGRESS, fileName);
	
	// Сохранить дескрипторы диалога копирования и потока копирования
	setOperationThread (progress->hDlg, hDeleteThread, TRUE);
		
	// Запустить рабочий поток удаления
	resumeOperationThread (progress->hDlg);
	return TRUE;
}


// Процедура индикации прогресса удаления
DWORD CALLBACK deleteProgressRoutine
	(LARGE_INTEGER notUsed1, LARGE_INTEGER notUsed2, LARGE_INTEGER notUsed3,
	LARGE_INTEGER notUsed4, DWORD notUsed5, DWORD rate, HANDLE notUsed7,
	HANDLE hDlg, LPVOID lpData)
{
	Progress* progr = (Progress*) lpData;
	TCHAR compactPath [50];
	PathCompactPathEx (compactPath, (TCHAR*)progr->path, sizeof (compactPath) / sizeof (compactPath[0]) - sizeof (L'\0'), 0);
	SendDlgItemMessage ((HWND)hDlg, IDC_CUR_DEL_FILENAME, WM_SETTEXT, 0, (LPARAM)compactPath);
	SendDlgItemMessage ((HWND)hDlg, IDC_DEL_PROGRESS, PBM_SETRANGE, 0, (LPARAM) MAKELPARAM (0, progr->maxIndent));
	SendDlgItemMessage ((HWND)hDlg, IDC_DEL_PROGRESS, PBM_SETPOS, (WPARAM)(progr->maxIndent - progr->indent), 0);
	return 0;
}


INT_PTR onDeleteDialogInit (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	/** HWND hProgr = GetDlgItem (hDlg, IDC_DEL_PROGRESS);
	SetWindowLong (hProgr, GWL_EXSTYLE, PBS_MARQUEE);
	SendMessage (hProgr, PBM_SETMARQUEE, (WPARAM)TRUE, (LPARAM)50); */
	return NULL; 
}

