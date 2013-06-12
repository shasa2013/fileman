#include "browse.h"


BOOL onColumnClick (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (lParam == NULL)	return FALSE;

	NMLISTVIEW* nmlv = (NMLISTVIEW*) lParam;
	
	FmSort sort;
	sort.column = nmlv->iSubItem;
	sort.dir = TRUE;
	sort.separate = TRUE;
	sort.hListView = GetDlgItem (hDlg, wParam);
	sort.flags = NORM_IGNORECASE | NORM_IGNOREWIDTH;
	sort.locale = LOCALE_USER_DEFAULT;
	sort.separate = TRUE;
	
	// Сортировать
	SendMessage (sort.hListView, LVM_SORTITEMSEX, (WPARAM) &sort, (LPARAM) sortListView);
	
	// Сохранить индекс колонки по которой выполнена сортировка
	FmPanelProps* props = (FmPanelProps*) GetProp (sort.hListView, L"Props");
	props->sortColumnIndex = sort.column;
	
	return TRUE;
}


INT_PTR onListFinished (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	LTPD* ltpd = (LTPD*) lParam;
	HWND hPanel = GetDlgItem (hDlg, ltpd->itemId);
	UINT diskListId = (UINT) GetProp (hPanel, L"diskListId");
	EnableWindow (GetDlgItem(hDlg, diskListId), TRUE);
	SendMessage (hPanel, WM_SETREDRAW, (WPARAM) TRUE, 0);
	InvalidateRect (hPanel, NULL, NULL);
	removeOperationThread (hPanel);
	FmPanelProps* props = (FmPanelProps*) GetProp (hPanel, L"Props");
	NMLISTVIEW nmlv = {0};
	nmlv.iSubItem = props->sortColumnIndex;
	onColumnClick (hDlg, ltpd->itemId, (LPARAM) &nmlv);
	delete ltpd;	
	return 0;
}



BOOL listDirectoryToPanel (HWND hDlg, UINT panelId, UINT fullPathId)
{
	// Проверяем что не идёт подсчёт размера файлов
	HWND hPanel = GetDlgItem (hDlg, panelId);
	
	if (isOperationThreadExist (hPanel)) return FALSE;

	INT countedItem = (INT) GetProp (hPanel, L"Counted item");
	if (countedItem > 0)
	{
		return FALSE;
	}
	
	LTPD* ltpd = new LTPD();
	SendDlgItemMessage (hDlg, fullPathId, WM_GETTEXT, ltpd->getPathAllocSize(), (LPARAM)ltpd->path);
	ltpd->hDlg = hDlg;
	ltpd->itemId = panelId;
	
	HANDLE hThread = CreateThread (NULL, NULL, listDirectoryToPanelWorker, ltpd, CREATE_SUSPENDED, NULL);
	if (hThread == NULL)
	{
		delete ltpd;
		return FALSE;
	}
	
	// Запретить перерисовку 
	SendDlgItemMessage(ltpd->hDlg, ltpd->itemId, WM_SETREDRAW, (WPARAM) FALSE, 0);
	
	// Сохранить дескриптор потока
	setOperationThread (hPanel, hThread, TRUE);

	// Запустить поток
	return resumeOperationThread (hPanel);
}




DWORD WINAPI listDirectoryToPanelWorker (void* args)
{
	BOOL isUpDirPresent;		// Флаг, обозначает то что переход на дирректорию выше найден
	LTPD* ltpd = (LTPD*) args;
	TCHAR str [FM_MAX_PATH];
	str [0] = L'\0';
	HIMAGELIST hIL = NULL;
		
	LVITEM lvI = {0};
	lvI.pszText = str;
	lvI.cchTextMax = sizeof(str) / sizeof (str[0]);
	
	// Очистить панель от своих данных
	lvI.mask = LVIF_PARAM; 
	BOOL rc = TRUE;
	while (rc)
	{
		rc = SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_GETITEM, 0, (LPARAM)&lvI);
		if (rc)
		{
			if (lvI.lParam != NULL)
			{
				fmLocalFree ((HLOCAL)lvI.lParam);
			}
			lvI.lParam = NULL;
			++ lvI.iItem;
		}
	}
	
	lvI.mask = LVIF_TEXT | LVIF_IMAGE;
	
	lvI.iItem = 0;
	lvI.lParam = 0;
		
	SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_DELETEALLITEMS, 0, 0);
	
	hIL = (HIMAGELIST) SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_GETIMAGELIST, (WPARAM)LVSIL_SMALL, 0);
	if (hIL == NULL)
	{
		hIL = ImageList_Create (16, 16, ILC_COLORDDB, 255, 1);
		SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM) hIL);
	}
	else
	{
		ImageList_RemoveAll (hIL);
	}
	
	WIN32_FIND_DATA findData = {0};

	wsprintf (str, L"%s%s", ltpd->path, L"\\*");
	HANDLE hFFile = FindFirstFile (str, &findData);
	if (hFFile != INVALID_HANDLE_VALUE)
	{	
		isUpDirPresent = findData.cFileName[0] == L'.';
		isUpDirPresent &= findData.cFileName[1] == L'.';
		isUpDirPresent &= findData.cFileName[2] == L'\0';

		if (FM_AFTLV_OK == addFileToListView (ltpd->hDlg, &findData, ltpd->itemId, &lvI))
		{
			setFileIcon (ltpd->hDlg, ltpd->path, &findData, &lvI, hIL);
			lvI.mask = LVIF_IMAGE;
			SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_SETITEM, 0, (LPARAM)&lvI); 
			// Перейти к следующей строке
			++ lvI.iItem;
		}
		while (FindNextFile (hFFile, &findData))
		{
			if (!isUpDirPresent)
			{
				if (lstrcmp (findData.cFileName, L"..") == 0)
				{
					isUpDirPresent = TRUE;
				}
			}
			if (FM_AFTLV_OK == addFileToListView (ltpd->hDlg, &findData, ltpd->itemId, &lvI))
			{	
				setFileIcon (ltpd->hDlg, ltpd->path, &findData, &lvI, hIL);
				lvI.mask = LVIF_IMAGE;
				SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_SETITEM, 0, (LPARAM)&lvI); 
				++ lvI.iItem;
			}
		}
		FindClose (hFFile);
	}
	else // Возникла ошибка
	{
		// Показать List View
		wprintfd (L"Возникла ошибка при FindFirstFile('%s', ...)", str);
		SendMessage (ltpd->hDlg, FM_LIST_FINISHED, ltpd->itemId, (LPARAM)ltpd);
		return 1;
	}
		
	// Символ L".." не был найден, значит его надо вставить
	if (!isUpDirPresent) 
	{	
		// Имя файла L".." и соответствующая иконка
		HINSTANCE hInst = (HINSTANCE) GetWindowLong (ltpd->hDlg, GWL_HINSTANCE);
		HICON hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON1));
		lvI.iItem = 0; // L".." всегда в первой строке
		lvI.iImage = ImageList_ReplaceIcon (hIL, lvI.iItem, hIcon);
		lvI.iSubItem = FM_COLUMN_NAME;	// Колонка названия файлов
		lvI.pszText = L"..";
		lvI.lParam = NULL;
		SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&lvI);
		DestroyIcon (hIcon);
	}
	
	
	// Сортировать панель
	FmPanelProps* props = (FmPanelProps*)GetProp (GetDlgItem(ltpd->hDlg, ltpd->itemId), L"Props");
	NMLISTVIEW nmlv = {0};
	nmlv.iSubItem = props->sortColumnIndex;

	lvI.mask = LVIF_IMAGE;
	DWORD cnt = SendDlgItemMessage(ltpd->hDlg, ltpd->itemId, LVM_GETITEMCOUNT, 0, 0);
	for (lvI.iItem = 0; lvI.iItem < cnt; ++ lvI.iItem)
	{
		SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_GETITEM,0, (LPARAM) &lvI);
		wprintfd(L"item:%u image:%u", lvI.iItem, lvI.iImage);
	}
	SendMessage (ltpd->hDlg, FM_LIST_FINISHED, ltpd->itemId, (LPARAM)ltpd);
	return 0;
}




DWORD addFileToListView (HWND hDlg, WIN32_FIND_DATA* findData, UINT itemId, LVITEM* lvI)
{
	SYSTEMTIME stUTC; 
	SYSTEMTIME stLocal;
	TCHAR str1 [MAX_PATH];
	TCHAR* tmpStr1;
	
	if (lstrcmp(findData->cFileName, L".") == 0)
	{
		return FM_AFTLV_NO;
	}

	// Занести данные из findData
	lvI->lParam = (LPARAM) fmLocalAlloc (sizeof(WIN32_FIND_DATA));
	CopyMemory ((VOID*)lvI->lParam, (const VOID*) findData, sizeof (WIN32_FIND_DATA));
	
	// Создать новую строку
	lvI->iSubItem = 0;
	lvI->pszText[0] = L'\0';
	lvI->mask = LVIF_PARAM;
	SendDlgItemMessage (hDlg, itemId, LVM_INSERTITEM, (WPARAM)0, (LPARAM)lvI);
	
	
	lvI->mask = LVIF_TEXT;
	// Файл или папка
	if (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{	
		// если папка
		lvI->iSubItem = FM_COLUMN_NAME;
		lstrcpy (lvI->pszText, findData->cFileName);
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEM, (WPARAM)0, (LPARAM)lvI);
		
		lvI->iSubItem = FM_COLUMN_TYPE;
		lstrcpy (lvI->pszText, FM_DIRTYPE_STUB);
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEM, (WPARAM)0, (LPARAM)lvI);
	}
	else 
	{	
		// Если файл, то тип файла
		lvI->iSubItem = FM_COLUMN_TYPE;
		tmpStr1 = lstrrchr (findData->cFileName, L'.');
		if (tmpStr1 != NULL)
		{
			++ tmpStr1;
			lstrcpy (lvI->pszText, tmpStr1);
			SendDlgItemMessage (hDlg, itemId, LVM_SETITEM, (WPARAM)0, (LPARAM)lvI);
		}
		
		// имя файла
		lvI->iSubItem = FM_COLUMN_NAME;
		lstrcpy (lvI->pszText, findData->cFileName);
		lcutrchr (lvI->pszText, L'.');
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEM, (WPARAM)0, (LPARAM)lvI);

		// размер файла
		UINT64 fs = findData->nFileSizeLow | (UINT64(findData->nFileSizeHigh) << 32);
		StrFormatByteSize (LONGLONG(fs), str1, sizeof (str1) / sizeof (str1[0]));
		lvI->iSubItem = FM_COLUMN_SIZE;
		lstrcpy (lvI->pszText, str1);
		SendDlgItemMessage (hDlg, itemId, LVM_SETITEM, (WPARAM)lvI->iItem, (LPARAM)lvI);
	}

	// Дата файла или папки
	lvI->iSubItem = FM_COLUMN_DATE;
	FileTimeToSystemTime(&findData->ftLastWriteTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	wsprintf(str1, L"%02u-%02u-%02u %02u:%02u", stLocal.wYear, stLocal.wMonth,
		stLocal.wDay, stLocal.wHour, stLocal.wMinute);
	lstrcpy (lvI->pszText, str1);
	SendDlgItemMessage (hDlg, itemId, LVM_SETITEM, (WPARAM)0, (LPARAM)lvI);
	
	return FM_AFTLV_OK;
}




VOID setFileIcon (HWND hDlg, TCHAR* fileDir, WIN32_FIND_DATA* findData, LVITEM* lvI, HIMAGELIST hIL)
{
	SHFILEINFO shfi;
	if (lstrcmp (findData->cFileName, L"..") == 0)
	{
		HINSTANCE hInst = (HINSTANCE) GetWindowLong (hDlg, GWL_HINSTANCE);
		shfi.hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON1));
		lvI->iImage = ImageList_AddIcon (hIL, shfi.hIcon);
	}
	else
	{
		TCHAR fullPath [FM_MAX_PATH];
		lstrcpy (fullPath, fileDir);
		lstrcat (fullPath, findData->cFileName);
		SHGetFileInfo (fullPath, findData->dwFileAttributes, &shfi, sizeof(SHFILEINFO),
			SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES | SHGFI_ADDOVERLAYS);
		if (shfi.hIcon == NULL)
		{
			wprintfd (L"Не найдена иконка для файла %s", fullPath);
			return;
		}
		lvI->iImage = ImageList_ReplaceIcon (hIL, -1, shfi.hIcon);
		if (lvI->iImage == -1)
		{
			lvI->iImage = 0;
		}
	}
	DestroyIcon (shfi.hIcon);
	return;
}

