#include "copy.h"

BOOL copyDropped (HWND hDlg, Task* copyTask)
{
	Progress* progress = copyTask->getProgress();

	// Cоздать поток копирования в спящем состоянии
	HANDLE hCopyThread = CreateThread (NULL, NULL, copyFileWorker, copyTask, CREATE_SUSPENDED, NULL);
	if (hCopyThread == NULL) return FALSE;
	
	SetThreadPriority (hCopyThread, THREAD_PRIORITY_IDLE);
	
	// Создать диалог копирования
	LONG hInst = GetWindowLong (hDlg, GWL_HINSTANCE);
	progress->hDlg = CreateDialogParam ((HINSTANCE)hInst, MAKEINTRESOURCE(IDD_COPY_PROGRESS), hDlg, copyDlgProc, 0);
	if (progress->hDlg == NULL) return FALSE;
	
	TCHAR dstFolder [FM_MAX_PATH];
	PathCompactPathEx (dstFolder, copyTask->getDstFolder(), 50, 0);
	SendDlgItemMessage (progress->hDlg, IDC_COPY_DEST, WM_SETTEXT, 0, (LPARAM)dstFolder);
		
	getIniFileName (dstFolder, SIZETC (dstFolder));
	loadDlgItemProperties (progress->hDlg, IDD_COPY_PROGRESS, dstFolder);

	SendDlgItemMessage (progress->hDlg, IDC_COPY_PROGRESS_BAR, PBM_SETPOS, 100, 0);
	
	// Сохранить дескрипторы диалога копирования и потока копирования
	setOperationThread (progress->hDlg, hCopyThread, TRUE);
	
	// Установить флаг активности операции копирования
	SetProp (progress->hDlg, L"running", (HANDLE) TRUE);
	
	// Запустить поток копирования
	resumeOperationThread (progress->hDlg);

	return TRUE;
}

// Процедура обработки сообщений диалога копирования
INT CALLBACK copyDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
    {
        case WM_COMMAND:
			switch (LOWORD(wParam))
            {
                case IDCANCEL:
					onCopyDialogCancel (hDlg, wParam, lParam);
					return (INT) TRUE;
				case IDC_PAUSE_TOGGLE:
					onCopyPauseToggle (hDlg, wParam, lParam);
					return (INT) TRUE;
			}
			break;
		case WM_INITDIALOG:
			onCopyDlgInit (hDlg, wParam, lParam);
			break;
		case WM_NOTIFY:
			break;
		case FM_COPY_FINISHED:
			onCopyDialogFinished (hDlg, wParam, lParam);
			return (INT) TRUE;
		case WM_DESTROY:
			onCopyDlgDestroy (hDlg, wParam, lParam);
			break;
	}
	return (INT) FALSE;
}


INT CALLBACK progrBarWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_PAINT:
		case WM_NCPAINT:
			onProgrBarPaint (hWnd, wParam, lParam);
			break;
		case WM_SETTEXT:
			onProgrBarSetText (hWnd, wParam, lParam);
			break;
	}
	WNDPROC defWndProc = (WNDPROC) GetWindowLong (hWnd, GWL_USERDATA);
	return (INT) CallWindowProc (defWndProc, hWnd, msg, wParam, lParam);
}

INT_PTR onProgrBarSetText (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//ProgrBarUserData* pbud = (ProgrBarUserData*) GetWindowLong (hWnd, GWL_USERDATA);
	//lstrcpy (pbud->text, (TCHAR*)lParam);
	return (INT) 0;
}


INT_PTR onCopyDlgDestroy (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HFONT hFont = (HFONT) SendDlgItemMessage (hDlg, IDC_COPY_SPEED, WM_GETFONT, 0, 0);
	if (hFont) DeleteObject (hFont);
	return (INT) 0;
}


INT_PTR onCopyDlgInit (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	// Подключить свой обработчик сообщений Прогрессбара
	// 1. получить дескриптор Прогрессбара
	HWND hProgr = GetDlgItem (hDlg, IDC_COPY_PROGRESS_BAR);
	// 2. добавить свою процедуру обработки сообщений Прогрессбара
	WNDPROC defProc = (WNDPROC)SetWindowLong (hProgr, GWL_WNDPROC, (LONG)progrBarWndProc);
	// 3. сохранить встроенную процедуру обработки сообщений Прогрессбара в USERDATA
	SetWindowLong (hProgr, GWL_USERDATA, (LONG)defProc); // Свой обработчик сообщений Прогрессбара подключен
	// Шрифт для элементов диалога
	RECT rect;
	HWND hWnd = GetDlgItem (hDlg, IDC_COPY_SPEED);
	GetClientRect (hWnd, &rect);
	int hight = rect.bottom - rect.top;
	HFONT hFont = CreateFont (hight, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	SendDlgItemMessage (hDlg, IDC_COPY_SPEED, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendDlgItemMessage (hDlg, IDC_COPY_TIME, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	return (INT) 0;
}


INT_PTR onProgrBarPaint (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint  (hWnd, &ps);
	RECT rect;
	int pos = SendMessage (hWnd, PBM_GETPOS, 0, 0);
	TCHAR str [32];
	wsprintf (str, L"%d %%", pos);
	GetClientRect (hWnd, &rect);
	++ rect.left; -- rect.right;
	++ rect.top; -- rect.bottom;
	int hight = rect.bottom - rect.top;
	int width = rect.right - rect.left;
	HFONT hFont = CreateFont (hight, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FW_BOLD, NULL);
	RECT fillRect;
	CopyMemory (&fillRect, &rect, sizeof (fillRect));
	fillRect.right = (pos * width) / 100;
	FillRect (hdc, &fillRect, (HBRUSH) (COLOR_HIGHLIGHTTEXT));
	RECT fillRect2;
	fillRect2.top = fillRect.top;
	fillRect2.bottom = fillRect.bottom;
	fillRect2.left = fillRect.right;
	fillRect2.right = rect.right;
	FillRect (hdc, &fillRect2, (HBRUSH) (COLOR_3DFACE));
	SetBkMode (hdc, TRANSPARENT);
	SelectObject (hdc, hFont);
	
	SetTextColor (hdc, GetSysColor (COLOR_WINDOW));
	DrawText (hdc, str, -1, &rect, DT_CENTER);
	
	ExcludeClipRect (hdc, fillRect.left, rect.top, fillRect.right, rect.bottom);
	SetTextColor (hdc, RGB (0, 0, 0));
	DrawText (hdc, str, -1, &rect, DT_CENTER);
	
	DeleteObject (hFont);
	
	EndPaint (hWnd, &ps);
	return (INT) FALSE;
}


INT_PTR onCopyPauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam)
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


BOOL copySelected (HWND hDlg, UINT fullPathIdSrc, UINT panelIdSrc, UINT fullPathIdDst, UINT panelIdDst, INT opCode)
{
	TCHAR fileName [MAX_PATH];
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;
	TCHAR* dlgCaption = FM_UNCKNOWN_DLG_CAPTION;
	
	
	// Создать задание для копирования
	Task* copyTask = new Task ();
	copyTask->setOpCode (opCode);
	if (opCode == FM_OPERATION_COPY)
	{
		copyTask->setFlags (COPY_FILE_FAIL_IF_EXISTS);
		dlgCaption = FM_COPY_DLG_CAPTION;
	}
	if (opCode == FM_OPERATION_MOVE)
	{
		copyTask->setFlags (MOVEFILE_WRITE_THROUGH | MOVEFILE_COPY_ALLOWED);
		dlgCaption = FM_MOVE_DLG_CAPTION;
	}
	
	Progress* progress = copyTask->getProgress();

	// Создать список имён файлов из выделенных
	lvi.iItem = (int) SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
	while (lvi.iItem != -1)
	{
		SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETITEM, 0, (LPARAM)&lvi);
		copyTask->addFileName (((WIN32_FIND_DATA*)lvi.lParam)->cFileName);
		lvi.iItem = (int) SendDlgItemMessage (hDlg, panelIdSrc, LVM_GETNEXTITEM, lvi.iItem, (LPARAM)LVNI_SELECTED);
	}

	if (copyTask->getFileNamesCount() == 0)
	{
		wprintfd (L"copySelected:copyTask: спиcок файлов пуст\n");
		return FALSE;
	}
	
	// Папка источник
	SendDlgItemMessage (hDlg, fullPathIdSrc, WM_GETTEXT, SIZETC(fileName), (LPARAM)fileName);
	copyTask->setSrcFolder (fileName);
	
	// Папка назначение
	SendDlgItemMessage (hDlg, fullPathIdDst, WM_GETTEXT, SIZETC(fileName), (LPARAM)fileName);
	copyTask->setDstFolder (fileName);

	// Cоздать поток копирования в спящем состоянии
	HANDLE hCopyThread = CreateThread (NULL, NULL, copyFileWorker, copyTask, CREATE_SUSPENDED, NULL);
	if (hCopyThread == NULL) return FALSE;
	
	// Приоритет потока копирования
	SetThreadPriority (hCopyThread, THREAD_PRIORITY_IDLE);
	
	// Создать диалог копирования
	LONG hInst = GetWindowLong (hDlg, GWL_HINSTANCE);
	progress->hDlg = CreateDialogParam ((HINSTANCE)hInst, MAKEINTRESOURCE(IDD_COPY_PROGRESS), hDlg, copyDlgProc, 0);
	if (progress->hDlg == NULL) return FALSE;
	
	SendMessage (progress->hDlg, WM_SETTEXT, 0, (LPARAM)dlgCaption); 
	
	// Короткое название для папки
	TCHAR dstFolder [FM_MAX_PATH];
	PathCompactPathEx (dstFolder, copyTask->getDstFolder(), 50, 0);
	SendDlgItemMessage (progress->hDlg, IDC_COPY_DEST, WM_SETTEXT, 0, (LPARAM)dstFolder);
	
	// Положение диалога копирования	
	getIniFileName (fileName, SIZETC (fileName));
	loadDlgItemProperties (progress->hDlg, IDD_COPY_PROGRESS, fileName);

	SendDlgItemMessage (progress->hDlg, IDC_COPY_PROGRESS_BAR, PBM_SETPOS, 100, 0);
	
	// Сохранить дескрипторы диалога копирования и потока копирования
	setOperationThread (progress->hDlg, hCopyThread, TRUE);
	
	// Установить флаг активности операции копирования
	SetProp (progress->hDlg, L"running", (HANDLE) TRUE);
	
	// Запустить поток копирования
	resumeOperationThread (progress->hDlg);
	return TRUE;
}


INT_PTR onCopyDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	TCHAR iniFileName [FM_MAX_PATH];
	getIniFileName (iniFileName, sizeof (iniFileName));
	saveDlgItemProperties (hDlg, IDD_COPY_PROGRESS, iniFileName);
	removeOperationThread (hDlg);
	DestroyWindow (hDlg);
	delete (Task*) lParam;
	return 0;
}


// Обработчик принудительного завершения диалога копирования
// HWND hDlg - дескриптор диалога копирования
INT_PTR onCopyDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
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



// Процедура потока. Копирует файлы 
DWORD WINAPI copyFileWorker (VOID* data)
{
	Task* task = (Task*) data;
	INT result = 0;
	WIN32_FIND_DATA findData;
	TCHAR* tmpDst = (TCHAR*) fmAlloc (FM_MAX_PATH);
	TCHAR* tmpSrc = (TCHAR*) fmAlloc (FM_MAX_PATH);
	while (task->nextPath()) 
	{
		lstrcpy (tmpDst, task->getDstFolder());  // Прередаём копии строк, т.к. recurseOperation
		lstrcpy (tmpSrc, task->getCurSrcPath()); // изменяет переданные ей аргументы
		// Проверка источник-назначение, на наличие рекурсии
		/* ... */
		result = recurseOperation (&findData, tmpSrc, tmpDst, progressRoutine,
			(VOID*)task->getProgress(), task->getFlags(), task->getOpCode(), NULL);
	}
	SendMessage (task->getProgress()->hDlg, FM_COPY_FINISHED, 0, (LPARAM) data);
	fmFree ((HLOCAL*)&tmpSrc);
	fmFree ((HLOCAL*)&tmpDst);
	return result;
}

