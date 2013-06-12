#include "browse.h"


BOOL executeSelected (HWND hLv)
{
	TCHAR path [FM_MAX_PATH];
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");

    if  (props->countedItem != -1) return FALSE;

    // Папка файла
	SendMessage (props->hFullPath, WM_GETTEXT, (WPARAM)SIZETC(path), (LPARAM)path);

	// Имя файла
	LVITEM lvi;
	lvi.iItem = SendMessage (hLv, LVM_GETSELECTIONMARK, 0, 0);
	if (lvi.iItem == -1) return FALSE;

	props->steckSels [props->steckInd] = lvi.iItem;

	lvi.mask = LVIF_PARAM;
	SendMessage (hLv, LVM_GETITEM, 0, (LPARAM)&lvi);
	WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;

	if (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		Watcher* watcher = (Watcher*) GetProp (props->hDlg, L"Watcher");
		watcher->stop ();

		DWORD aprc = addPath (path, findData->cFileName);

		if (aprc == HAP_DIR_DOWN)
		{
			++ props->steckInd;
			props->steckSels [props->steckInd] = 0;
		}
		if (aprc == HAP_DIR_UP)
		{
			-- props->steckInd;
		}
		if (props->steckInd < 0) props->steckInd = 0;
		props->steckInd %= SIZETC (props->steckSels);

		addSlashToPath (path);
		lstrcpy (props->curDir, path);
		SendMessage (props->hFullPath, WM_SETTEXT, 0, (LPARAM)path);

		listDirectoryToPanel (hLv);

		watcher->start ();
		return TRUE;
	}

	addPath (path, findData->cFileName);
	HINSTANCE hInst = ShellExecute (props->hDlg, L"open", path, NULL, NULL, SW_SHOWDEFAULT);
	if (INT(hInst) <= 32) // Согласно MSDN
	{
		DWORD rc = GetLastError();
		//showErrorMessage (props->hDlg, rc, (char*)FM_CAPTION);
		printfd ("executeSelected (%08x): Ошибка %u: %s\n", hLv, rc, path);
		return FALSE;
	}
	return TRUE;
}


BOOL getPathOfSelectedDisk (HWND hDlg, UINT itemId, TCHAR* path)
{
	LRESULT itemIndex = SendDlgItemMessage (hDlg, itemId, CB_GETCURSEL, 0, 0);
	if (itemIndex == -1) itemIndex = 0;
	path [0] = (TCHAR) SendDlgItemMessage (hDlg, itemId, CB_GETITEMDATA, (WPARAM)itemIndex, 0);
	path [1] = L'\0';
	lstrcat (path, L":\\");
	return TRUE;
}

INT_PTR onDiskList (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == CBN_CLOSEUP)
	{
		SetTimer (hDlg, LOWORD (wParam), 100, NULL);
	}
	return 0;
}



void fillDiskList (HWND hDlg, UINT itemId)
{
	TCHAR driveName [5];
	TCHAR str [MAX_PATH];
	TCHAR driveDiscr [MAX_PATH];
	SendDlgItemMessage(hDlg, itemId, CB_RESETCONTENT, 0, 0);
	lstrcpy (driveName, L" :\\");
    for (driveName[0] = L'C'; driveName[0] <= L'Z'; ++ driveName[0])
    {
        DWORD driveType = GetDriveType (driveName);
        if (driveType == DRIVE_NO_ROOT_DIR) continue;
		loadStringFromResource(hDlg, IDS_DRIVE_UNKNOWN + driveType, driveDiscr, SIZETC(driveDiscr));

		TCHAR driveLabel [MAX_PATH];
		TCHAR fsType [MAX_PATH];
		DWORD volSerNum = 0;
		DWORD maxComLen = 0;
		DWORD fsFlags = 0;
		driveLabel[0] = L'\0';
		fsType[0] = L'\0';
		if (GetVolumeInformation (driveName, driveLabel, sizeof (driveLabel), &volSerNum, &maxComLen, &fsFlags, fsType, sizeof(fsType)))
		{
			if (lstrlen(driveLabel) > 0)
			{
				wsprintf (str, L"%s %s %s (%s)", driveDiscr, driveLabel, fsType, driveName);
			}
			else
			{
				wsprintf (str, L"%s %s (%s)", driveDiscr, fsType, driveName);
			}
		}
		else
		{
			wsprintf (str, L"%s %s", driveDiscr, driveName);
		}
		LRESULT itemIndex = SendDlgItemMessage(hDlg, itemId, CB_ADDSTRING, 0, (LPARAM) str);
		SendDlgItemMessage (hDlg, itemId, CB_SETITEMDATA, (WPARAM)itemIndex, (LPARAM) driveName[0]);
    }
}


BOOL onColumnClick (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (lParam == NULL)	return FALSE;

	NMLISTVIEW* nmlv = (NMLISTVIEW*) lParam;
	FmSort sort;

	sort.hListView = (HWND)wParam;

	FmPanelProps* props = (FmPanelProps*) GetProp (sort.hListView, L"Props");

	// Сохранить индекс сортирующей колонки и направление сортировки
	props->sortColumnIndex = nmlv->iSubItem;

	sort.column = nmlv->iSubItem;
	sort.dir = props->sortDir;
	sort.separate = TRUE;
	sort.flags = NORM_IGNORECASE | NORM_IGNOREWIDTH;
	sort.locale = LOCALE_USER_DEFAULT;

	// Сортировать
	SendMessage (sort.hListView, LVM_SORTITEMSEX, (WPARAM)&sort, (LPARAM) sortListView);
	return TRUE;
}


INT_PTR onListFinished (HWND hLv, WPARAM wParam, LPARAM lParam)
{
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	EnableWindow (props->hFullPath, TRUE);

	int items = (int) SendMessage (hLv, LVM_GETITEMCOUNT, 0, 0);
	if (props->steckSels[props->steckInd] >= items)
	{
		props->steckSels[props->steckInd] = items - 1;
	}

	NMLISTVIEW nmlv = {0};
	nmlv.iSubItem = props->sortColumnIndex;
	onColumnClick (props->hDlg, (WPARAM)hLv, (LPARAM) &nmlv);

	removeOperationThread (hLv);

	// Выбрать строку панели
	LVITEM lvi;
	lvi.mask = LVIF_STATE;							// Работаем с полем LVITEM::state
	lvi.state = LVIS_SELECTED | LVIS_FOCUSED;		// Устанавливаем флаг LVIS_SELECTED
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;	// Устанавливаем только флаг LVIS_SELECTED
	lvi.iItem = props->steckSels[props->steckInd];	// Индекс выбираемой строки
	lvi.iSubItem = 0;
	SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);

	//SendMessage (hLv, LVM_SETSELECTIONMARK, 0, (LPARAM)props->curSel);
	SendMessage (hLv, LVM_SETSELECTIONMARK, 0, (LPARAM)props->steckSels[props->steckInd]);

	//SendMessage (hLv, LVM_ENSUREVISIBLE, props->curSel, (LPARAM)TRUE);
	SendMessage (hLv, LVM_ENSUREVISIBLE, props->steckSels[props->steckInd], (LPARAM)TRUE);

	// Разрешить перерисовку панели
	SendMessage (hLv, WM_SETREDRAW, TRUE, 0);

	// Перерисовать панель
	InvalidateRect (hLv, NULL, NULL);
	return 0;
}


BOOL listDirectoryToPanel (HWND hLv)
{
	if (isOperationThreadExist (hLv)) return FALSE;

	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	if (props == NULL) return FALSE;
	//if (props->сountedItem > 0) return FALSE;
    if (props->countedItem > 0) return FALSE;
	HANDLE hThread = CreateThread (NULL, NULL, listDirectoryToPanelWorker, hLv, CREATE_SUSPENDED, NULL);
	if (hThread == NULL)
	{
		DWORD rc = GetLastError();
		printfd ("ошибка: listDirectoryToPanel:CreateThread");
		printdErrorStr (rc);
		return FALSE;
	}

	// Запретить перерисовку
	SendMessage (hLv, WM_SETREDRAW, FALSE, 0);

	// Запретить смену диска
	EnableWindow (props->hFullPath, FALSE);

	// Сохранить дескриптор потока
	setOperationThread (hLv, hThread, TRUE);

	// Запустить поток
	return resumeOperationThread (hLv);
}



DWORD WINAPI listDirectoryToPanelWorker (void* args)
{
	HWND hLv = (HWND) args;
	printfd ("listDirectoryToPanelWorker:args:%08x\n", args);
	WIN32_FIND_DATA findData;
	TCHAR tmpPath [FM_MAX_PATH];
	LVITEM lvi;
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	HINSTANCE hInst = (HINSTANCE) GetWindowLong (props->hDlg, GWL_HINSTANCE);
	HIMAGELIST hIL = (HIMAGELIST) SendMessage (hLv, LVM_GETIMAGELIST, LVSIL_SMALL, 0);
	tmpPath [0] = '\0';
	// Очистить панель от своих данных и Image List
	lvi.mask = LVIF_PARAM;
	// Очистить панель от всех элементов
	// Очистка производится удалением нулевого элемента
	lvi.iItem = 0;
	while ((BOOL)SendMessage (hLv, LVM_GETITEM, 0, (LPARAM)&lvi))
	{
		fmFree ((HLOCAL*)&lvi.lParam);
		ImageList_Remove (hIL, 0);
		SendMessage (hLv, LVM_DELETEITEM, (WPARAM)0, 0);
	};

	// Фильтр названий файлов
	lstrcpy (tmpPath, props->curDir);
	addPath (tmpPath, L"*");
	Find find;
	findData.cFileName [0] = '\0';
	HANDLE hObj = find.first (tmpPath, &findData);
	if (hObj != INVALID_HANDLE_VALUE)
	{
		do
		{
			addFileToListView (hLv, props->curDir, &findData);
			findData.cFileName [0] = '\0';
		}
		while (find.next (hObj, &findData));
		find.close (hObj);
	}
	else // Возникла ошибка
	{
		// Показать List View
		printfd ("Возникла ошибка при Find::first('%s', ...)\n", tmpPath);
		SendMessage (hLv, FM_LIST_FINISHED, 0, (LPARAM)0);// ? Внимание! требуется LPARAM!
		return 1;
	}

	// Занести иконки
	addFilesIconsToListView (hInst, hLv, props->curDir);
	SendMessage (hLv, FM_LIST_FINISHED, 0, (LPARAM)0); // ? Внимание! требуется LPARAM!
	return 0;
}




DWORD addFileToListView (HWND hLv, TCHAR* path, WIN32_FIND_DATA* findData)
{
	SYSTEMTIME stUTC;
	SYSTEMTIME stLocal;
	TCHAR pszText [MAX_PATH];
	LVITEM lvi;
	lvi.pszText = pszText;
	lvi.cchTextMax = SIZETC (pszText);

	if (lstrcmp(findData->cFileName, L".") == 0)
	{
		return FM_AFTLV_NO;
	}

	// Создать новую строку
	lvi.iItem = SendMessage(hLv, LVM_GETITEMCOUNT, 0, 0);
	lvi.iSubItem = FM_COLUMN_NAME;
	lvi.mask = LVIF_PARAM;
	lvi.lParam = (LPARAM) fmAlloc (sizeof(WIN32_FIND_DATA));
	CopyMemory ((VOID*)lvi.lParam, (const VOID*) findData, sizeof (WIN32_FIND_DATA));
	INT itemIndex = (INT) SendMessage (hLv, LVM_INSERTITEM, 0, (LPARAM)&lvi);
	if (itemIndex == -1)
	{
		return FM_AFTLV_ERR;
	}
	lvi.mask = LVIF_TEXT;

	// Файл или папка ...
	if (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{// ... если папка

		lvi.iSubItem = FM_COLUMN_NAME;
		lstrcpy (lvi.pszText, findData->cFileName);
		SendMessage(hLv, LVM_SETITEM, 0, (LPARAM)&lvi);

		lvi.iSubItem = FM_COLUMN_TYPE;
		lstrcpy (lvi.pszText, FM_DIRTYPE_STUB);
		SendMessage(hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
	}
	else
	{// ... если файл ...
		// ... тип файла
		lvi.iSubItem = FM_COLUMN_TYPE;
		lvi.pszText = lstrrchr (findData->cFileName, L'.');
		if (lvi.pszText)
		{
			++ lvi.pszText;
			SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
		}

		// ... имя файла
		lvi.iSubItem = FM_COLUMN_NAME;
		lvi.pszText = pszText;
		lstrcpy (lvi.pszText, findData->cFileName);
		lcutrchr (lvi.pszText, L'.');
		SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);

		// ... размер файла
		lvi.iSubItem = FM_COLUMN_SIZE;
		UINT64 fs = findData->nFileSizeLow | (UINT64 (findData->nFileSizeHigh) << 32);
		StrFormatByteSize (LONGLONG(fs), pszText, lvi.cchTextMax);
		lvi.pszText = pszText;
		SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
	}

	// Дата файла или папки
	lvi.iSubItem = FM_COLUMN_DATE;
	FileTimeToSystemTime (&findData->ftLastWriteTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	wsprintf (pszText, L"%02u-%02u-%02u %02u:%02u", stLocal.wYear, stLocal.wMonth,
		stLocal.wDay, stLocal.wHour, stLocal.wMinute);
	lvi.pszText = pszText;
	SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
	return FM_AFTLV_OK;
}


VOID addFilesIconsToListView (HINSTANCE hInst, HWND hLv, TCHAR* path)
{
	SHFILEINFO shfi;
	LVITEM lvi;
	TCHAR tmpPath [FM_MAX_PATH];
	HIMAGELIST hIL = (HIMAGELIST) SendMessage (hLv, LVM_GETIMAGELIST, LVSIL_SMALL, 0);
	INT itemsCount = (INT) SendMessage (hLv, LVM_GETITEMCOUNT, 0, 0);
	lvi.iSubItem = 0;
	WIN32_FIND_DATA* findData;
	for (lvi.iItem = 0; lvi.iItem < itemsCount; ++ lvi.iItem)
	{
		shfi.hIcon = NULL;
		lvi.mask = LVIF_PARAM;
		SendMessage (hLv, LVM_GETITEM, 0, (LPARAM)&lvi);
		findData = (WIN32_FIND_DATA*) lvi.lParam;
		if (findData == NULL) continue;
		lstrcpy (tmpPath, path);
		addPath (tmpPath, findData->cFileName);
		if (lstrcmp (findData->cFileName, L"..") == 0)
		{
			shfi.hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON1));
			if (shfi.hIcon != NULL)
			{
				lvi.iImage = ImageList_ReplaceIcon (hIL, -1, shfi.hIcon);
				lvi.mask = LVIF_IMAGE;
				SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
				DestroyIcon (shfi.hIcon);
			}
		}
		else
		{
			lvi.mask = LVIF_IMAGE;
			SHGetFileInfo (tmpPath, findData->dwFileAttributes, &shfi,
				sizeof (SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_ADDOVERLAYS | SHGFI_USEFILEATTRIBUTES);
			DWORD rc = GetLastError ();
			if (shfi.hIcon == NULL)
			{
				printfd ("Не найдена иконка для файла: \"%s\"\n", tmpPath);
				printdErrorStr (rc);
				shfi.hIcon = LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON3));
				lvi.iImage = ImageList_ReplaceIcon (hIL, -1, shfi.hIcon);
				lvi.mask = LVIF_IMAGE;
				SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
				DestroyIcon (shfi.hIcon);
			}
			else
			{
				lvi.iImage = ImageList_ReplaceIcon (hIL, -1, shfi.hIcon);
				SendMessage (hLv, LVM_SETITEM, 0, (LPARAM)&lvi);
			}
			if (shfi.hIcon != NULL)
			{
				DestroyIcon (shfi.hIcon);
			}
		}
	}
	return;
}

INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	INT rc;
	FmSort* sort;
	WIN32_FIND_DATA* findData1;
	WIN32_FIND_DATA* findData2;
	LVITEM lvi1;
	LVITEM lvi2;

	rc = 0;
	sort = (FmSort*) lParamSort;

	lvi1.mask = LVIF_PARAM;
	lvi1.iItem = INT (lParam1);
	lvi1.iSubItem = sort->column;
	SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi1);
	findData1 = (WIN32_FIND_DATA*) lvi1.lParam;

	lvi2.mask = LVIF_PARAM;
	lvi2.iItem = INT (lParam2);
	lvi2.iSubItem = sort->column;
	SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi2);
	findData2 = (WIN32_FIND_DATA*) lvi2.lParam;

	if (lstrcmp(findData1->cFileName, L"..") == 0)
	{
		return -1;
	}
	else if ((findData1->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		&& ((findData2->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
	{
		rc = -1;
		return rc;
	}

	if (lstrcmp(findData2->cFileName, L"..") == 0)
	{
		return 1;
	}
	else if ((findData2->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		&& ((findData1->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
	{
		rc = 1;
		return rc;
	}

	switch (sort->column)
	{
		case FM_COLUMN_DATE:
		{
			ULONGLONG time1;
			ULONGLONG time2;
			time1 = ((ULONGLONG) findData1->ftLastWriteTime.dwHighDateTime << 32)|((ULONGLONG)findData1->ftLastWriteTime.dwLowDateTime);
			time2 = ((ULONGLONG) findData2->ftLastWriteTime.dwHighDateTime << 32)|((ULONGLONG)findData1->ftLastWriteTime.dwLowDateTime);
			if (time1 < time2) rc = -1;
			else if (time1 >= time2) rc = 1;
			break;
		}
		case FM_COLUMN_NAME:
		{
			rc = CompareString (sort->locale, sort->flags, findData1->cFileName, lstrlen (findData1->cFileName), findData2->cFileName, lstrlen (findData2->cFileName));
			rc -= 2;
			break;
		}
		case FM_COLUMN_SIZE:
		{
			ULONGLONG size1;
			ULONGLONG size2;
			size1 = ((ULONGLONG) findData1->nFileSizeHigh << 32)|((ULONGLONG)findData1->nFileSizeLow);
			size2 = ((ULONGLONG) findData2->nFileSizeHigh << 32)|((ULONGLONG)findData2->nFileSizeLow);
			if (size1 < size2) rc = -1;
			else if (size1 > size2) rc = 1;
			break;
		}
		case FM_COLUMN_TYPE:
		{
			TCHAR* ext1;
			TCHAR* ext2;
			ext1 = PathFindExtension (findData1->cFileName);
			ext2 = PathFindExtension (findData2->cFileName);
			rc = CompareString (sort->locale, sort->flags, ext1, lstrlen (ext1), ext2, lstrlen (ext2));
			rc -= 2;
			break;
		}
	}
	if (!sort->dir) rc *= -1;
	return rc;
}
