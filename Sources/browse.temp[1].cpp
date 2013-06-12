#include "browse.h"


// Выводит в List View список фалов и папок в директории
// HWND hDlg - дескриптор диалога
// DWORD itemId - идентификатор List View
// TCHAR* path - путь к директории 
BOOL listDirectoryToPanel (HWND hDlg, UINT panelId, UINT fullPathId)
{
	ListToPanelData* ltpd = new ListToPanelData();
	SendDlgItemMessage (hDlg, fullPathId, WM_GETTEXT, LocalSize (ltpd->path), (LPARAM)ltpd->path);
	ltpd->hDlg = hDlg;
	ltpd->itemId = panelId;
	return NULL != CreateThread (NULL, NULL, listDirectoryToPanelWorker, ltpd, NULL, NULL);
}

// Процедура потока. Заполняет List View элементами директории
DWORD WINAPI listDirectoryToPanelWorker (void* args)
{
	BOOL isUpDirPresent = FALSE; // Флаг, обозначает то что переход на дирректорию выше найден
	ListToPanelData* ltpd = (ListToPanelData*) args;
	
	HIMAGELIST hIL = ImageList_Create (16, 16, ILC_COLORDDB, 1, 1);
	SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM) hIL);
	
	// Спрятать List View
	SendDlgItemMessage(ltpd->hDlg, ltpd->itemId, WM_SETREDRAW, (WPARAM) FALSE, 0);
	
	LVITEM lvI = {0};
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
	lvI.state = 0; 
	lvI.stateMask = 0; 

	SendDlgItemMessage(ltpd->hDlg, ltpd->itemId, LVM_DELETEALLITEMS, 0, 0);
	
	WIN32_FIND_DATA findData = {0};
	TCHAR str [FM_MAX_PATH];
	wsprintf (str, L"%s%s", ltpd->path, L"\\*");
	HANDLE hFFile = FindFirstFile (str, &findData);
	if (hFFile != INVALID_HANDLE_VALUE)
	{	
		if (lstrcmp(findData.cFileName, L"..") == 0)
		{
			isUpDirPresent = TRUE;
		}
		lvI.pszText = findData.cFileName;
		setIconForFile (ltpd->hDlg, ltpd->path, findData.cFileName, ltpd->itemId, &lvI, hIL);
		addFileToListView (ltpd->hDlg, &findData, ltpd->itemId, &lvI);
		while (FindNextFile(hFFile, &findData))
		{
			if (!isUpDirPresent)
			{
				if (lstrcmp(findData.cFileName, L"..") == 0)
				{
					isUpDirPresent = TRUE;
				}
			}
			lvI.pszText = findData.cFileName;
			setIconForFile (ltpd->hDlg, ltpd->path, findData.cFileName, ltpd->itemId, &lvI, hIL);
			addFileToListView (ltpd->hDlg, &findData, ltpd->itemId, &lvI);
		}
		FindClose (hFFile);
	}
		
	
	if (!isUpDirPresent) 
	{	// Символ L".." не был найден, значит его надо вставить
		lvI.iSubItem = 0;				// Колонка названия файлов
		DWORD tmpItemIndex = lvI.iItem;
		lvI.iItem = 0;					// Вставляем в начало
		lstrcpy (lvI.pszText, L"..");	// Символ верхней директории
		SendDlgItemMessage (ltpd->hDlg, ltpd->itemId, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&lvI);
		lvI.iItem = tmpItemIndex;
	}
	
	// Показать List View
	SendDlgItemMessage(ltpd->hDlg, ltpd->itemId, WM_SETREDRAW, (WPARAM) TRUE, 0);
	InvalidateRect (GetDlgItem(ltpd->hDlg, ltpd->itemId), NULL, NULL);
	SendMessage (ltpd->hDlg, FM_LIST_FINISHED, 0, (LPARAM)ltpd);
	return 0;
}


// Добавляет файл в ListView
void addFileToListView (HWND hDlg, WIN32_FIND_DATA* findData, UINT itemId, LVITEM* lvI)
{
	SYSTEMTIME stUTC = {0};
	SYSTEMTIME stLocal = {0};
		
	if (lstrcmp(lvI->pszText, L".") == 0)  return; // Символ текущей директории, пропускаем
	
	// Создать новую строчку
	lvI->iSubItem = 0;
	lvI->pszText = 0;
	SendDlgItemMessage (hDlg, itemId, LVM_INSERTITEM, (WPARAM)0, (LPARAM)lvI);
	
	// Файл или папка
	if (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{	// если папка
		lvI->iSubItem = 0;
		lvI->pszText = findData->cFileName;
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);
		lvI->iSubItem = 1;
		lvI->pszText = L"<DIR>";
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);
	}
	else 
	{	// если файл
	
		// тип файла
		lvI->iSubItem = 1;
		lvI->pszText = lstrrchr (findData->cFileName, L'.');
		if (lvI->pszText != NULL)
		{
			lvI->pszText ++;
			SendDlgItemMessage (hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);
		}
		
		// имя файла
		lvI->iSubItem = 0;
		lvI->pszText = lstrrchr (findData->cFileName, L'.');
		if (lvI->pszText != NULL)
		{
			*lvI->pszText = L'\0';
		}
		lvI->pszText = findData->cFileName;
		
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);

		// размер файла
		wsprintf (findData->cFileName, L"%lu", findData->nFileSizeLow | (findData->nFileSizeHigh << 32));
		lvI->iSubItem = 3;
		SendDlgItemMessage(hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);
	}

	// Дата файла или или папки
	lvI->iSubItem = 2;
	FileTimeToSystemTime(&findData->ftLastWriteTime, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
	wsprintf(findData->cFileName, L"%02u-%02u-%02u %02u:%02u", stLocal.wYear, stLocal.wMonth,
		stLocal.wDay, stLocal.wHour, stLocal.wMinute);
	lvI->pszText = findData->cFileName;
	SendDlgItemMessage (hDlg, itemId, LVM_SETITEMTEXT, (WPARAM)lvI->iItem, (LPARAM)lvI);
	
	
	
	// Перейти к следующей строке
	++ lvI->iItem;
	lvI->pszText[0] = L'\0';
}

ListToPanelData :: ListToPanelData (TCHAR* path)
{
	this->path = (TCHAR*) fmLocalAlloc (lstrlen(path) * sizeof(TCHAR) + sizeof (L'\0'));
	lstrcpy (this->path, path);
}

ListToPanelData :: ListToPanelData (VOID)
{
	this->path = (TCHAR*) fmLocalAlloc (FM_MAX_PATH);
}


ListToPanelData :: ~ ListToPanelData ()
{
	if (this->path) fmLocalFree (this->path);
}


void setIconForFile (HWND hDlg, TCHAR* folder, TCHAR* fileName, UINT itemId, LVITEM* lvI, HIMAGELIST hIL)
{
	if ((lstrcmp (fileName, L".") == 0) || (lstrcmp (fileName, L"..") == 0)) return;
	TCHAR* fullPath = (TCHAR*) fmLocalAlloc ((lstrlen (folder) + lstrlen (fileName))
									* sizeof (fileName[0])
									+ sizeof (L'\0')
									+ sizeof (L'\\'));
	wsprintf (fullPath, L"%s%s", folder, fileName);
	SHFILEINFO shfi = {0};
	SHGetFileInfo (fullPath, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof (shfi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
	lvI->iImage = ImageList_GetImageCount (hIL);
	ImageList_AddIcon (hIL, shfi.hIcon);
}