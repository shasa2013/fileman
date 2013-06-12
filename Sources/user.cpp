#include "user.h"



BOOL saveDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName)
{

	TCHAR itemName [32];
	TCHAR propName [32];
	TCHAR propValue [MAX_PATH];
	BOOL bResult = FALSE;
	int iResult = 0;


	// Проверяем, что это диалог ...
	HWND hItem = GetDlgItem (hDlg, itemId);
	if ( hItem == NULL)
	{// ... диалог
		WINDOWPLACEMENT lpwndpl = {0};
		lpwndpl.length = sizeof (WINDOWPLACEMENT);
		bResult &= GetWindowPlacement (hDlg, &lpwndpl);
		wsprintf (itemName, L"%u", itemId);
		lstrcpy (propName, L"WINDOWPLACEMENT");
		bResult &= WritePrivateProfileStruct (itemName, propName, &lpwndpl, sizeof(lpwndpl), fileName);
		return bResult;
	}
	
	iResult = GetClassName(GetDlgItem(hDlg, itemId), itemName, sizeof(itemName) / sizeof(itemName[0]));
	if (iResult == 0)
	{
		return FALSE;	
	}
	
	if (lstrcmp (itemName, L"ComboBox") == 0)
	{
		LRESULT res = SendDlgItemMessage(hDlg, itemId, WM_GETTEXT, sizeof(propValue) / sizeof(propValue[0]), (LPARAM)propValue);
		if (res == -1) lstrcpy (propValue, L"C:\\");
		wsprintf (itemName, L"%s %u", L"Edit", itemId);
		lstrcpy (propName, L"TEXT");
		return WritePrivateProfileString (itemName, propName, propValue, fileName);
	}
	
	if (lstrcmp (itemName, L"SysListView32") == 0)
	{
		FmPanelProps* panelProps = (FmPanelProps*) GetProp (hItem, L"Props");
		if (panelProps == NULL) return FALSE;

		wsprintf (itemName, L"%s %u", L"SysListView32", itemId);
		
		// Цвет чётных строк панели
		lstrcpy (propName, L"bkEven");
		wsprintf (propValue, L"%u", (DWORD)panelProps->bkColors[0]);
		bResult = WritePrivateProfileString (itemName, propName, propValue, fileName);
		
		// Цвет нечётных строк панели
		lstrcpy (propName, L"bkOdd");
		wsprintf (propValue, L"%u", (DWORD)panelProps->bkColors[1]);
		bResult &= WritePrivateProfileString (itemName, propName, propValue, fileName);
		
		// Структура окна "Выбор цвета"
		lstrcpy (propName, L"customColors");
		bResult &= WritePrivateProfileStruct (itemName, propName, panelProps->customColors, sizeof (panelProps->customColors), fileName);
		
		// Размеры и положение панели
		WINDOWPLACEMENT wpl = {0};
		GetWindowPlacement (hDlg, &wpl);		
		DWORD halfWidth = ((wpl.rcNormalPosition.right - wpl.rcNormalPosition.left) >> 1) - 20;
		DWORD colWidths [FM_COLUMN_COUNT];
		DWORD colCount = sizeof (colWidths) / sizeof(colWidths[0]);
		getListVeiwColumnsWidth (hDlg, itemId, colCount, colWidths);
		for (DWORD i = 0; i < colCount; ++ i)
		{
			panelProps->colsRelWidths [i] = colWidths [i] / FLOAT (halfWidth);
		}
		
		// Относительные ширины колонок панели
		lstrcpy (propName, L"colsRelWidths");
		bResult &= WritePrivateProfileStruct (itemName, propName, &panelProps->colsRelWidths, sizeof (panelProps->colsRelWidths), fileName);

		// Номер выделенной строки
		lstrcpy (propName, L"selectedLine");
		INT selMark = (INT) SendMessage (hItem, LVM_GETSELECTIONMARK, 0, 0);
		if (selMark == -1) selMark = 0;
		wsprintf (propValue, L"%u", selMark);
		bResult &= WritePrivateProfileString (itemName, propName, propValue, fileName);
	
		// Номер колонки сортировки
		lstrcpy (propName, L"sortColumn");
		wsprintf (propValue, L"%u", panelProps->sortColumnIndex);
		bResult &= WritePrivateProfileString (itemName, propName, propValue, fileName);

		// Направление сортировки
		lstrcpy (propName, L"sortDir");
		wsprintf (propValue, L"%u", panelProps->sortDir);
		bResult &= WritePrivateProfileString (itemName, propName, propValue, fileName);
		
		return bResult;
	}
	return FALSE;
}


BOOL loadDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName)
{
	TCHAR itemName [32];
	TCHAR propName [32];
	TCHAR propValue [MAX_PATH];
	BOOL bResult = TRUE;
	int iResult = TRUE;
	
	// Проверяем, что это главный диалог
	HWND hItem = GetDlgItem (hDlg, itemId);
	if (hItem == NULL)
	{
		WINDOWPLACEMENT wndpl = {0};
		wndpl.length = sizeof (wndpl);
		wsprintf (itemName, L"%u", itemId);
		wsprintf (propName, L"%s", L"WINDOWPLACEMENT");
		bResult &= GetPrivateProfileStruct (itemName, propName, &wndpl, sizeof(wndpl), fileName); 
		if (bResult)
		{
			bResult &= SetWindowPlacement(hDlg, &wndpl);
		}
		return bResult;
	}
	
	iResult = GetClassName (GetDlgItem(hDlg, itemId), itemName, SIZETC (itemName));
	if (iResult == 0)
	{
		return FALSE;	
	}
	
	if (lstrcmp (itemName, L"ComboBox") == 0)
	{
		LRESULT res = SendDlgItemMessage(hDlg, itemId, WM_GETTEXT, SIZETC(propValue), (LPARAM)propValue);
		if (res == -1) lstrcpy (propValue, L"C:\\");
		wsprintf (itemName, L"%s %u", L"Edit", itemId);
		wsprintf (propName, L"%s", L"TEXT");
		bResult &= 0 != GetPrivateProfileString (itemName, propName, L"", propValue, SIZETC(propValue), fileName);
		if (!PathIsDirectory (propValue))
		{
			lstrcpy (propValue, L"C:\\");
		}
		bResult &= CB_ERR == SendDlgItemMessage (hDlg, itemId, WM_SETTEXT, (WPARAM)0, (LPARAM)propValue);
		return bResult;
	}

	if (lstrcmp (itemName, L"SysListView32") == 0)
	{
		// Адрес стрктуры со свойствами панели
		FmPanelProps* panelsProps = (FmPanelProps*) GetProp (hItem, L"Props");
		wsprintf (itemName, L"%s %u", L"SysListView32", itemId);
		
		// Цвет фона чётных строк
		wsprintf (propName, L"%s", L"bkEven");
		bResult &= 0 != GetPrivateProfileString (itemName, propName, L"2147483647", propValue, SIZETC(propValue), fileName);
		if (bResult)
		{
			panelsProps->bkColors [0] = _wtol (propValue);
		}
		else
		{
			panelsProps->bkColors[0] = RGB (255, 255, 255);
		}
		
		// Цвет фона нечётных строк
		wsprintf (propName, L"%s", L"bkOdd");
		bResult &= 0 != GetPrivateProfileString (itemName, propName, L"16250871", propValue, SIZETC(propValue), fileName);
		if (bResult)
		{
			panelsProps->bkColors [1] = _wtol (propValue);
		}
		else
		{
			panelsProps->bkColors [1] = RGB (240, 240, 240);
		}
				
		// Массив своих цветов диалога выбора цвета
		wsprintf (propName, L"%s", L"customColors");
		bResult &= GetPrivateProfileStruct (itemName, propName, panelsProps->customColors, sizeof (panelsProps->customColors), fileName);
		
		// Относительные ширины колонок 0 ... 3
		wsprintf (propName, L"%s", L"colsRelWidths");
		bResult &= 0 != GetPrivateProfileStruct (itemName, propName, &panelsProps->colsRelWidths, sizeof(panelsProps->colsRelWidths), fileName);
					
		// Если структура с ширинами не была загружена, то задаём по умолчанию
		if (bResult == FALSE) 
		{
			panelsProps->colsRelWidths [FM_COLUMN_NAME] = 0.4f;
			panelsProps->colsRelWidths [FM_COLUMN_TYPE] = 0.2f;
			panelsProps->colsRelWidths [FM_COLUMN_DATE] = 0.2f;
			panelsProps->colsRelWidths [FM_COLUMN_SIZE] = 0.2f;
		}
		DWORD colWidths [4];
		WINDOWPLACEMENT wpl = {0};
		GetWindowPlacement (hDlg, &wpl);
		DWORD panelWidth = ((wpl.rcNormalPosition.right - wpl.rcNormalPosition.left) >> 1) - 20;
		colWidths [FM_COLUMN_NAME] = DWORD (panelWidth * panelsProps->colsRelWidths [FM_COLUMN_NAME]);
		colWidths [FM_COLUMN_TYPE] = DWORD (panelWidth * panelsProps->colsRelWidths [FM_COLUMN_TYPE]);
		colWidths [FM_COLUMN_DATE] = DWORD (panelWidth * panelsProps->colsRelWidths [FM_COLUMN_DATE]);
		colWidths [FM_COLUMN_SIZE] = DWORD (panelWidth * panelsProps->colsRelWidths [FM_COLUMN_SIZE]);
		TCHAR* heders [] = {L"Имя", L"Тип", L"Дата", L"Размер"};
		initListViewColumns (hDlg, itemId, SIZETC(heders), heders, colWidths);

		// Номер выделенной строки
		lstrcpy (propName, L"selectedLine");
		bResult &= GetPrivateProfileString (itemName, propName, L"0", propValue, SIZETC(propValue), fileName);
		//panelsProps->curSel = _wtoi (propValue);
		panelsProps->steckSels [0] = _wtoi (propValue);
		panelsProps->steckInd = 0;

		// Номер колонки сортировки
		lstrcpy (propName, L"sortColumn");
		bResult &= GetPrivateProfileString (itemName, propName, L"0", propValue, SIZETC(propValue), fileName);
		panelsProps->sortColumnIndex = _wtoi (propValue);

		// Направление сортировки
		lstrcpy (propName, L"sortDir");
		bResult &= GetPrivateProfileString (itemName, propName, L"0", propValue, SIZETC(propValue), fileName);
		panelsProps->sortDir = _wtoi (propValue);
		
	}
	return bResult;
}


// Возвращает строку с полным путём к файлу конфигурации
BOOL getIniFileName (TCHAR* modulePath, DWORD size)
{
	GetModuleFileName (NULL, modulePath, size);
	lcutrchr (modulePath, L'\\');
	lstrcat (modulePath, L"\\fileman.ini");
	return TRUE;
}



BOOL setListVeiwColumnsWidth (HWND hLv, DWORD count, DWORD colWidths [])
{
	LVCOLUMN lvc;
 	lvc.mask = LVCF_WIDTH;
	for (lvc.iOrder = 0; lvc.iOrder < int (count); ++ lvc.iOrder) 
	{ 
		lvc.cx = colWidths [lvc.iOrder];
		SendMessage (hLv, LVM_SETCOLUMN, lvc.iOrder, (LPARAM)&lvc);
    }
	return TRUE; 
}



BOOL getListVeiwColumnsWidth (HWND hDlg, UINT itemId, DWORD count, DWORD colWidths [])
{
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;
	BOOL result = TRUE;
	for (DWORD i = 0; i < count; ++ i)
	{
		result &= (BOOL) SendDlgItemMessage (hDlg, itemId, LVM_GETCOLUMN, i, LPARAM (&lvc));
		if (result) colWidths [i] = lvc.cx;
		else colWidths [i] = 100;
	}
	return result;
}



BOOL getListVeiwColumnsWidth (HWND hLv, DWORD count, DWORD colWidths [])
{
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_WIDTH | LVCF_SUBITEM;
	BOOL result = TRUE;
	for (DWORD i = 0; i < count; ++ i)
	{
		result &= (BOOL) SendMessage (hLv, LVM_GETCOLUMN, i, LPARAM (&lvc));
		wprintfd (L"");
		if (result) colWidths [i] = lvc.cx;
		else colWidths [i] = 100;
	}
	return result;
}



HWND getDlgItemPos (HWND hDlg, UINT itemId, RECT* pRect)
{
	HWND hItem = GetDlgItem (hDlg, itemId);
	if (hItem)
	{
		GetWindowRect (hItem, pRect);
	}
	return hItem;
}


BOOL initListViewColumns (HWND hDlg, UINT itemId, DWORD columns, TCHAR* headers [], DWORD colWidths []) 
{ 
	LVCOLUMN lvc = {0};
	
	// The mask specifies that the format, width, text, and subitem members of the structure are valid. 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			  
    for (DWORD iCol = 0; iCol < columns; ++ iCol) 
	{ 
		lvc.pszText = headers[iCol];
		lvc.cx = colWidths [iCol];
		switch (iCol)
		{
			case 1:
			case 2:
				lvc.fmt = LVCFMT_CENTER;
				break;
			case 3:
				lvc.fmt = LVCFMT_RIGHT;
				break;
			case 0:
			default:
				lvc.fmt = LVCFMT_LEFT;
				break;
		}
		SendDlgItemMessage(hDlg, itemId, LVM_INSERTCOLUMN, iCol, (LPARAM)&lvc);
	 }
	SendDlgItemMessage (hDlg, itemId, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)LVS_EX_FULLROWSELECT, (LPARAM)LVS_EX_FULLROWSELECT);
	return TRUE; 
}
