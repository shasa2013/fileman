#include "move.h"




INT CALLBACK moveDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
    {
        case WM_COMMAND:
			switch(LOWORD(wParam))
            {
                case IDCANCEL:
					onMoveDialogCancel (hDlg, wParam, lParam);
					return (INT) TRUE;
				case IDC_PAUSE_TOGGLE:
					onMovePauseToggle (hDlg, wParam, lParam);
					return (INT) TRUE;
			}
			break;
		case FM_MOVE_FINISHED:
			onMoveDialogFinished (hDlg, wParam, lParam);
			return (INT) TRUE;
	}
	return (INT) FALSE;	
}



DWORD WINAPI moveFileWorker (VOID* data)
{
	Task* task = (Task*) data;
	int result = 0;
	WIN32_FIND_DATA findData;
	TCHAR* tmpDst = (TCHAR*) fmAlloc (FM_MAX_PATH);
	TCHAR* tmpSrc = (TCHAR*) fmAlloc (FM_MAX_PATH);
	while (task->nextPath())
	{
		lstrcpy (tmpDst, task->getDstFolder());
		lstrcpy (tmpSrc, task->getCurSrcPath());
		result = recurseOperation
			(&findData,
			tmpSrc,
			tmpDst,
			progressRoutine,
			(void*)task->getProgress(),
			MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING,
			FM_OPERATION_MOVE, NULL);
	}
	SendMessage (task->getProgress()->hDlg, FM_MOVE_FINISHED, 0, (LPARAM) data);
	fmFree ((HLOCAL*)&tmpSrc);
	fmFree ((HLOCAL*)&tmpDst);
	return result;
}



/*DWORD CALLBACK moveProgressRoutine (LARGE_INTEGER TotalFileSize,
									LARGE_INTEGER TotalBytesTransferred,
									LARGE_INTEGER StreamSize,
									LARGE_INTEGER StreamBytesTransferred,
									DWORD dwStreamNumber,
									DWORD dwCallbackReason,
									HANDLE hSrcFile,
									HANDLE hDstFile,
									LPVOID lpData)
{
	return PROGRESS_CONTINUE;	
}
*/


INT_PTR onMoveDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR str1 [512];
	TCHAR str2 [512];
	LONG hInst = GetWindowLong (hDlg, GWL_HINSTANCE);
	LoadString ((HINSTANCE)hInst, IDS_SHURE_CANCEL_COPY, str1, sizeof(str1));
	LoadString ((HINSTANCE)hInst, IDS_CAPTION, str2, sizeof (str2));
	if (MB_OK == MessageBox (hDlg, str1, str2, MB_OK | MB_ICONQUESTION))
	{
		// Сбросить флаг активности операции копирования
		SetProp (hDlg, L"running", (HANDLE) FALSE);
	}
	return 0;
}



INT_PTR onMoveDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR iniFileName [FM_MAX_PATH];
	getIniFileName (iniFileName, sizeof (iniFileName));
	saveDlgItemProperties (hDlg, IDD_COPY_PROGRESS, iniFileName);
	DestroyWindow (hDlg);
	removeOperationThread (hDlg);
	Task* copyTask = (Task*) lParam;
	delete copyTask;
	return 0;
}



INT_PTR onMovePauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	UINT buttonStrId = 0;
	if (isOperationThreadSuspended (hDlg))
	{
		resumeOperationThread (hDlg);
		buttonStrId = IDS_COPY_PAUSE;
	}
	else
	{	
		suspendOperationThread (hDlg);
		buttonStrId = IDS_COPY_RESUME;
	}
	TCHAR* str = (TCHAR*) fmAlloc (MAX_PATH);
	loadStringFromResource (hDlg, buttonStrId, str, MAX_PATH);
	SendDlgItemMessage (hDlg, IDC_PAUSE_TOGGLE, WM_SETTEXT, 0, (LPARAM) str);
	fmFree ((HLOCAL*)&str);
	return 0;
}


BOOL moveSelected (HWND hDlg, UINT fullPathIdSrc, UINT panelIdSrc, UINT fullPathIdDst, UINT panelIdDst)
{
	TCHAR fileName [MAX_PATH];
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;
	
	// Создать задание для копирования
	Task* copyTask = new Task ();
	
	Progress* progress = copyTask->getProgress();

	// Создать список имён файлов из выделенных 
	lvi.iItem = (int) SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETNEXTITEM, (WPARAM)0, (LPARAM)LVNI_SELECTED);
	if (lvi.iItem == 0) return FALSE;
	while (lvi.iItem != -1)
	{
		SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETITEM, 0, (LPARAM)&lvi);
		copyTask->addFileName (((WIN32_FIND_DATA*)lvi.lParam)->cFileName);
		lvi.iItem = (int) SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETNEXTITEM, (WPARAM)lvi.iItem, (LPARAM)LVNI_SELECTED);
		fileName[0] = L'\0';
	}
	
	// Папка источник
	SendDlgItemMessage (hDlg, fullPathIdSrc, WM_GETTEXT, sizeof(fileName), (LPARAM)fileName);
	copyTask->setSrcFolder (fileName);
	
	// Папка назначение
	SendDlgItemMessage (hDlg, fullPathIdDst, WM_GETTEXT, sizeof(fileName), (LPARAM)fileName);
	copyTask->setDstFolder (fileName);

	// Cоздать поток копирования в спящем состоянии
	HANDLE hCopyThread = CreateThread (NULL, NULL, moveFileWorker, copyTask, CREATE_SUSPENDED, NULL);
	if (hCopyThread == NULL) return FALSE;
	
	SetThreadPriority (hCopyThread, THREAD_PRIORITY_IDLE);
	

	// Создать диалог копирования
	LONG hInst = GetWindowLong (hDlg, GWL_HINSTANCE);
	progress->hDlg = CreateDialogParam ((HINSTANCE)hInst, MAKEINTRESOURCE(IDD_COPY_PROGRESS), hDlg, moveDlgProc, 0);
	if (progress->hDlg == NULL) return FALSE;

	SendMessage (progress->hDlg, WM_SETTEXT, 0, (LPARAM) L"Перемещение");
	
	SendDlgItemMessage (progress->hDlg, IDC_COPY_DEST, WM_SETTEXT, 0, (LPARAM)copyTask->getDstFolder());
		
	getIniFileName (fileName, sizeof (fileName));
	loadDlgItemProperties (progress->hDlg, IDD_COPY_PROGRESS, fileName);
	
	// Сохранить дескрипторы диалога копирования и потока копирования
	setOperationThread (progress->hDlg, hCopyThread, TRUE);

	// Установить флаг активности операции перемещения
	SetProp (progress->hDlg, L"running", (HANDLE) TRUE);
		
	// Запустить поток копирования
	resumeOperationThread (progress->hDlg);

	return 0;
}