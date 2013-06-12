#include "count.h"

BOOL countSelected (HWND hLv)
{
	// ��������� ���� "��� �������"
	// HWND hPanel = GetDlgItem (hDlg, panelId);
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	if (props->�ountedItem != -1)
	{
		return FALSE;
	}
	
	TCHAR fileName [MAX_PATH];
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;	// ���� - �������� LPARAM
	lvi.iItem = SendMessage (hLv, LVM_GETSELECTIONMARK, 0, 0);
	if (lvi.iItem == -1) return FALSE;
	
	// ��������� ������ ������
	props->�ountedItem = lvi.iItem;

	// ������� ������ ���������� ���� ��� ������� "�������"
	Task* countTask = new Task ();
	countTask->getProgress()->itemId = GetWindowLong (hLv, GWL_ID);
	HWND hDlg = (HWND) GetWindowLong (hLv, GWL_HWNDPARENT);
	countTask->getProgress()->hDlg = hDlg;
	
	// ��� �����
	SendMessage (hLv, LVM_GETITEM, 0, (LPARAM)&lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
	countTask->addFileName (findData->cFileName);

	// ����� ��������
	SendMessage (props->hFullPath, WM_GETTEXT, (WPARAM) SIZETC (fileName), (LPARAM)fileName);
	countTask->setSrcFolder (fileName);
	
	// C������ ������� ����� ���������� ������� "�������" � ������ ���������
	HANDLE hCountThread = CreateThread (NULL, NULL, countWorker, countTask, CREATE_SUSPENDED, NULL);
	if (hCountThread == NULL)
	{
		delete countTask;
		return FALSE;
	}
	
	// ��������� ����� �����
	EnableWindow (props->hFullPath, FALSE);

	// ��������� ���������� ������ ��������
	setOperationThread (hLv, hCountThread, TRUE); 	

	// ��������� ���������� ������� "�������"
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
	lvi.iItem = panelProps->�ountedItem;
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
	panelProps->�ountedItem = -1;
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
	lvi.iItem = panelProps->�ountedItem;
	if (lvi.iItem == -1)
	{
		printfd ("onCountProgress:panelProps->�ountedItem:%d\n", lvi.iItem);
		return NULL;
	}

	// ����� ��������� WIN32_FIND_DATA
	lvi.mask = LVIF_PARAM;
	SendMessage (hPanel, LVM_GETITEM, 0, (LPARAM)&lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
	if (findData == NULL)
	{
		printfd ("onCountProgress: ��� WIN32_FIND_DATA* ��� %d ������\n", lvi.iItem);
		return NULL;
	}
		
	// ������� ������ �����
	DWORDLONG largeSize = * (DWORDLONG*) lParam;
	findData->nFileSizeHigh = (DWORD) (largeSize >> 32);
	findData->nFileSizeLow = (DWORD) (largeSize & 0xffffffff);
	
	ReleaseMutex (dlgMutex);
	return NULL;
}
