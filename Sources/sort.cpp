#include "sort.h"

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
	else if (findData1->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		rc = -1;
		if (!sort->dir) rc *= -1;
		return rc;
	}

	if (lstrcmp(findData2->cFileName, L"..") == 0)
	{
		return 1; 
	}
	else if (findData2->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		rc = 1;
		if (!sort->dir) rc *= -1;
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
			else if (time1 > time2) rc = 1;
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


// Сравнивает два элемента ListView
/*INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR str [MAX_PATH];				// Строка для LVITEM
	TCHAR cmpStr1 [MAX_PATH * 4];		// Перавая строка для сравнения
	TCHAR cmpStr2 [MAX_PATH * 4];		// Вторая строка для сравнения
	LVITEM lvi = {0};					// Струтура для чтения элементов ListView
	DWORD tmpCol;						// Для временного хранения индекса колонки
	
	
	FmSort* sort = (FmSort*) lParamSort;// Указатель на свою структуру FmSort
	cmpStr1 [0] = L'\0';
	cmpStr2 [0] = L'\0';
	
	lvi.mask = LVIF_TEXT;	
	lvi.pszText = str;
	lvi.cchTextMax = sizeof (str);
	
	tmpCol = sort->column;
	
	// Создать первую строку для сравнения
	lvi.iItem = lParam1;				
	lvi.iSubItem = sort->column;		// Первое слово - строка из кликнутой колонки
	SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
	lstrcat (cmpStr1, lvi.pszText);
	if (tmpCol != FM_COLUMN_TYPE)
	{
		lvi.iSubItem = FM_COLUMN_TYPE;				
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		if (lstrcmpi (lvi.pszText, FM_DIRTYPE_STUB) == 0)
		{
			return -1;
		}
	}
	if (tmpCol != FM_COLUMN_NAME)
	{
		lvi.iSubItem = FM_COLUMN_NAME;				// Второе слово - имя файла
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr1, lvi.pszText);
	}
	if (tmpCol != FM_COLUMN_DATE)
	{
		lvi.iSubItem = FM_COLUMN_DATE;				// Четвёртое слово - дата
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr1, lvi.pszText);
	}
	if (tmpCol != 3)
	{
		lvi.iSubItem = 3;				// Пятое слово - размер
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr1, lvi.pszText);
	}
		
	
	// Создать вторую строку для сравнения
	lvi.iItem = lParam2;				
	lvi.iSubItem = sort->column;		// Первое слово - строка из кликнутой колонки
	SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
	lstrcat (cmpStr2, lvi.pszText);
	if ((tmpCol != 1) && (sort->separate))
	{
		lvi.iSubItem = 1;				
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		if (lstrcmpi (lvi.pszText, FM_DIRTYPE_STUB) == 0)
		{
			return 1;
		}
	}
	if (tmpCol != 0)
	{
		lvi.iSubItem = 0;				// Второе слово - имя файла
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr2, lvi.pszText);
	}
	if (tmpCol != 2)
	{
		lvi.iSubItem = 2;				// Четвёртое слово - дата
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr2, lvi.pszText);
	}
	if (tmpCol != 3)
	{
		lvi.iSubItem = 3;				// Пятое слово - размер
		SendMessage (sort->hListView, LVM_GETITEM, 0, (LPARAM) &lvi);
		lstrcat (cmpStr2, lvi.pszText);
	}
		
	
	INT result = CompareString (sort->locale, sort->flags,
		cmpStr1, lstrlen (cmpStr1), cmpStr2, lstrlen (cmpStr2)); 		
	if (result == CSTR_LESS_THAN) return -1;
	if (result == CSTR_GREATER_THAN) return 1;
	return 0;
}*/