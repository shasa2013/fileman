#include "Fileman.h"



INT_PTR	onListViewEndTrack (HWND hLv, WPARAM wParam, LPARAM lParam)
{
	NMHEADER* nmLvHdr = (NMHEADER*) lParam;
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	WINDOWPLACEMENT wpl;
	GetWindowPlacement (hLv, &wpl);		
	DWORD width = (wpl.rcNormalPosition.right - wpl.rcNormalPosition.left) - 20;
	if (nmLvHdr->pitem->cxy < 40) nmLvHdr->pitem->cxy = 40;
	SendMessage (hLv, LVM_SETCOLUMNWIDTH, nmLvHdr->pitem->iOrder, (LPARAM)MAKELPARAM (nmLvHdr->pitem->cxy, 0));
	props->colsRelWidths [nmLvHdr->iItem] = FLOAT (nmLvHdr->pitem->cxy) / FLOAT (width);
	return NULL;
}


INT_PTR	onListViewTrack (HWND hLv, WPARAM wParam, LPARAM lParam)
{
	return NULL;
}


INT CALLBACK subClassLvProc (HWND hLv, UINT msg, WPARAM wParam, LONG lParam)
{
	//static BOOL shiftIsDown = FALSE;
	FmPanelProps* panel = (FmPanelProps*) GetProp (hLv, L"Props");
	//printfd ("subClassLvProc:panel:%08x\n", panel);
	switch (msg) 
    {
		case WM_TIMER:
		{
			if (wParam == FM_DIRCHANGE_TIMER)
			{
				KillTimer (hLv, FM_DIRCHANGE_TIMER);
				wprintfd (L"[%u] subClassLvProc:WM_TIMER %s\n", GetTickCount(), panel->curDir);
				listDirectoryToPanelWorker ((void*)hLv);
			}
			break;
		}
		case WM_NOTIFY:
		{
			NMHDR* pNmhdr = (NMHDR*)lParam;
			switch (pNmhdr->code)
			{
				case HDN_ENDTRACK:
					onListViewEndTrack (hLv, wParam, lParam);
					break;
				case HDN_TRACK:
					onListViewTrack (hLv, wParam, lParam);
					break;
			}
			break;
		}
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_DROPFILES:
			onDropFiles	(hLv, wParam, lParam);
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_F11:
				{
					TCHAR path [FM_MAX_PATH];
					SendMessage (panel->hFullPath, WM_GETTEXT, SIZETC (path), (LPARAM) path);
					SetCurrentDirectory (path);
					ShellExecute (NULL, L"open", L"cmd", NULL, NULL, SW_SHOWDEFAULT);
					break;
				}
				case VK_RETURN:
					onIdOk (hLv, 0, 0);
					break;
				case VK_SPACE: // Команда "Подсчитать"
					countSelected (hLv);
					break;
			}
			break;
		case FM_LIST_FINISHED:
			onListFinished (hLv, wParam, lParam);
			break;
	}
 	return CallWindowProc (panel->defProc, hLv, msg, wParam, lParam);
}



INT_PTR	onDropFiles	(HWND hLv, WPARAM wParam, LPARAM lParam)
{
	TCHAR path [FM_MAX_PATH];
	lParam = (LPARAM) DragQueryFile ((HDROP)wParam, 0xFFFFFFFF, path, SIZETC (path));
	FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	Task* copyTask = new Task (TPM_DROP);
	for (DWORD i = 0; i < DWORD (lParam); ++ i)
	{
		DragQueryFile ((HDROP)wParam, i, path, SIZETC (path));
		copyTask->addFileName (path);
	}
	DragFinish ((HDROP)wParam);
	SendMessage (props->hFullPath, WM_GETTEXT, SIZETC(path), (LPARAM)path);
	copyTask->setDstFolder (path);
	copyDropped (hLv, copyTask);
	return NULL;
}


INT APIENTRY wWinMain (HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR pCmdLine, INT nCmdShow)
{
	INITCOMMONCONTROLSEX initComCtls;
	OleInitialize (NULL);
	initComCtls.dwICC = ICC_WIN95_CLASSES;
	initComCtls.dwSize = sizeof (INITCOMMONCONTROLSEX);
	InitCommonControlsEx (&initComCtls);
	SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
	DialogBox (hInst, (LPCTSTR) MAKEINTRESOURCE (IDD_MAIN), NULL, mainDlgProc);
	OleUninitialize ();
	return 0;
}



INT CALLBACK mainDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
    {
        case WM_COMMAND:
			switch (LOWORD(wParam))
            {
                case IDCANCEL:
					onDialogCancel (hDlg, wParam, lParam);
					break;
				case IDC_DISK_LIST_LEFT:
				case IDC_DISK_LIST_RIGHT:
					onDiskList (hDlg, wParam, lParam);
					break;
/*				case IDOK:
					onIdOk (hDlg, wParam, lParam); 
					break; */
				case FM_SELBKCOLOR:
					onSetBkColor (hDlg, wParam, lParam);
					break;
				case ID_M_SETTINGS:
					onSettings (hDlg, wParam, lParam);
					break;
			}
			break;
		case WM_DROPFILES:
			onDropFiles (hDlg, wParam, lParam);
			break;
		case WM_TIMER:
			onTimer (hDlg, wParam, lParam);
			break;
		case WM_INITDIALOG:
            onDialogInit (hDlg, wParam, lParam);
			break;
		case WM_SIZE:
			onDialogSize (hDlg, wParam, lParam);
			break;
		case WM_NOTIFY:
			/* ! Важен код возвращаемый onDialogNotify ! */
			return onDialogNotify (hDlg, wParam, lParam);
		case WM_CONTEXTMENU:
			onContextMenu (hDlg, wParam, lParam);
			break;
		case FM_COUNT_FINISHED:
			onCountFinished (hDlg, wParam, lParam);
			break;
		case FM_COUNT_PROGRESS:
			onCountProgress (hDlg, wParam, lParam);
			break;
		case WM_USER_MEDIACHANGED:
			onMediaChanged (hDlg, wParam, lParam);
			break;
		case WM_DEVICECHANGE:
			onDeviceChange (hDlg, wParam, lParam);
			break;
	}
	return (INT_PTR) FALSE;
}


INT_PTR	onMediaChanged (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SHNOTIFYSTRUCT* shns = (SHNOTIFYSTRUCT*) wParam;
	TCHAR strPath [FM_MAX_PATH];
	strPath [0] = '\0';
	SHGetPathFromIDList ((struct _ITEMIDLIST*)shns->dwItem1, strPath);
	switch (lParam)
	{
		case SHCNE_MEDIAINSERTED:
			printfd ("onMediaChanged:Media inserted into %s\n", strPath);
			break;			
		case SHCNE_MEDIAREMOVED:
			printfd ("onMediaChanged:Media removed from %s\n", strPath);
			break;
	}
	return NULL;
}

INT_PTR	onDeviceChange (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	printfd ("onDeviceChange wParam:%u lParam:%u\n", wParam, lParam);
	return NULL;
}

INT_PTR onItemChanged (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	NMLISTVIEW* nmlv = (NMLISTVIEW*) lParam;
	NMHDR* nmhdr = &nmlv->hdr;
	if (nmlv->uChanged == LVIF_STATE)
	{
		DWORD selCount = SendMessage (nmhdr->hwndFrom, LVM_GETSELECTEDCOUNT, 0, 0);
		DWORD itemCount = SendMessage (nmhdr->hwndFrom, LVM_GETITEMCOUNT, 0, 0);
		DWORDLONG largeSize = 0;
		LVITEM lvi;
		lvi.iItem = (int) SendMessage (nmhdr->hwndFrom, LVM_GETNEXTITEM, (WPARAM)-1, (LPARAM)LVNI_SELECTED);
		TCHAR str1 [32];
		TCHAR str2 [256];
		while (lvi.iItem != -1)
		{
			lvi.mask = LVIF_PARAM;
			SendMessage(nmhdr->hwndFrom, LVM_GETITEM, 0, (LPARAM)&lvi);
			WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
			largeSize += (DWORDLONG (findData->nFileSizeHigh) << 32) | findData->nFileSizeLow;
			lvi.iItem = (int) SendMessage (nmhdr->hwndFrom, LVM_GETNEXTITEM, (WPARAM)lvi.iItem, (LPARAM)LVNI_SELECTED);
		}
		StrFormatByteSize (largeSize, str1, SIZETC (str1));
		wsprintf (str2, L"Выделено %u файлов из %u (%s)", selCount, itemCount, str1);
		UINT itemId = (UINT)GetProp (nmhdr->hwndFrom, L"Selection size Id");
		SendDlgItemMessage (hDlg, itemId, WM_SETTEXT, 0, (LPARAM)str2);
	}
	return NULL;
}

INT_PTR onTimer (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == FM_UPDATE_TIMER)
	{
/*		WaitForSingleObject (dlgMutex, INFINITE);
		ULARGE_INTEGER freeBytesAvailable = {0};
		ULARGE_INTEGER totalNumberOfBytes = {0};
		ULARGE_INTEGER totalNumberOfFreeBytes = {0};
		TCHAR str1 [1024];
		TCHAR str2 [32];
		TCHAR str3 [32];
		TCHAR path [4];
		BOOL rc = FALSE;
		str1 [0] = str2 [0] = str3 [0] = L'\0';
		getPathOfSelectedDisk (hDlg, IDC_DISK_LIST_LEFT, path);
		if (lstrcmp(path, L"A:\\"))
		{
			rc = GetDiskFreeSpaceEx (path, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
		}
		if (rc)
		{
			StrFormatByteSize (LONGLONG(freeBytesAvailable.QuadPart), str2, SIZETC (str2));
			StrFormatByteSize (LONGLONG(totalNumberOfBytes.QuadPart), str3, SIZETC (str3));
			wsprintf (str1, L"Свободно %s из %s", str2, str3);
			SetDlgItemText (hDlg, IDC_FREE_LEFT, str1);
		}
		else
		{
			SetDlgItemText(hDlg, IDC_FREE_LEFT, str1);
		}
		str1 [0] = str2 [0] = str3 [0] = L'\0';
		getPathOfSelectedDisk (hDlg, IDC_DISK_LIST_RIGHT, path);
	
		rc = FALSE;
		if (lstrcmp(path, L"A:\\"))
		{
			rc = GetDiskFreeSpaceEx (path, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
		}
		if (rc)
		{
			StrFormatByteSize (LONGLONG(freeBytesAvailable.QuadPart), str2, SIZETC (str2));
			StrFormatByteSize (LONGLONG(totalNumberOfBytes.QuadPart), str3, SIZETC (str3));
			wsprintf (str1, L"Свободно %s из %s", str2, str3);
			SendDlgItemMessage (hDlg, IDC_FREE_RIGHT, WM_SETTEXT, 0, (LPARAM)str1);
		}
		else
		{
			SendDlgItemMessage (hDlg, IDC_FREE_RIGHT, WM_SETTEXT, 0, (LPARAM)str1);
		}
		
		static TCHAR animate [] = {L'+', L' '};
		LVITEM lvi;
		HWND hPanel = GetDlgItem (hDlg, IDC_LEFTPANEL);
		FmPanelProps* panelProps = (FmPanelProps*)GetProp (hPanel, L"Props");
		lvi.iItem = panelProps->сountedItem;
		if (lvi.iItem != -1)
		{
			// Взять структуру WIN32_FIND_DATA
			lvi.mask = LVIF_PARAM;
			SendMessage (hPanel, LVM_GETITEM, 0, (LPARAM)&lvi);
			WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
			
			// Занести размер файла
			DWORDLONG largeSize = (DWORDLONG (findData->nFileSizeHigh) << 32) | findData->nFileSizeLow;
			
			// Занести строчку о размере 
			StrFormatByteSize (largeSize, str2, SIZETC (str2));
			wsprintf (str3, L"%C %s", animate [(GetTickCount() / FM_COUNT_UPDATE_RATE) % (SIZETC(animate))], str2);
			lvi.iSubItem = FM_COLUMN_SIZE;		// Колонка с размерами файлов
			lvi.pszText = str3;
			lvi.mask = LVIF_TEXT;
			SendMessage (hPanel, LVM_SETITEM, 0, (LPARAM)&lvi);
		}
		hPanel = GetDlgItem (hDlg, IDC_RIGHTPANEL);
		panelProps = (FmPanelProps*)GetProp (hPanel, L"Props");
		lvi.iItem = panelProps->сountedItem;
		if (lvi.iItem != -1)
		{
			// Взять структуру WIN32_FIND_DATA
			lvi.mask = LVIF_PARAM;
			SendMessage (hPanel, LVM_GETITEM, 0, (LPARAM)&lvi);
			WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
			
			// Занести размер файла
			DWORDLONG largeSize = (DWORDLONG (findData->nFileSizeHigh) << 32) | findData->nFileSizeLow;
			
			// Занести строчку о размере 
			StrFormatByteSize (largeSize, str2, SIZETC (str2));
			wsprintf (str3, L"%C %s", animate [(GetTickCount() / FM_COUNT_UPDATE_RATE) % SIZETC(animate)], str2);
			lvi.iSubItem = FM_COLUMN_SIZE;		// Колонка с размерами файлов
			lvi.pszText = str3;
			lvi.mask = LVIF_TEXT;	
			SendMessage (hPanel, LVM_SETITEM, 0, (LPARAM)&lvi);
		}
		ReleaseMutex (dlgMutex);
	*/
	}
	/* Вызывается когда список дисков закрывается
	wParam - идентификатор списка дисков и таймера */
	else if ((LOWORD (wParam) == IDC_DISK_LIST_LEFT) || (LOWORD (wParam) == IDC_DISK_LIST_RIGHT))
	{
		TCHAR path [4];
		KillTimer (hDlg, wParam);
		getPathOfSelectedDisk (hDlg, wParam, path);
		SendDlgItemMessage (hDlg, wParam, WM_SETTEXT, 0, (LPARAM)path);
		EnableWindow ((HWND)lParam, FALSE);
		HWND hLv = NULL;
		
		if (LOWORD (wParam) == IDC_DISK_LIST_LEFT)		
		{
			hLv = GetDlgItem (hDlg, IDC_LEFTPANEL);
		}
		else if (LOWORD (wParam) == IDC_DISK_LIST_RIGHT)
		{
			hLv = GetDlgItem (hDlg, IDC_RIGHTPANEL);
		}
		else
		{
			printfd ("fileman.cpp:onTimer:ошибка: Dlg Item ID: %u\n", LOWORD(wParam));
			return NULL;
		}
		FmPanelProps* props = (FmPanelProps*)GetProp (hLv, L"Props");
		lstrcpy (props->curDir, path);
		listDirectoryToPanel (hLv);
	}
	return NULL;
}




INT_PTR onSetBkColor (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	CHOOSECOLOR cc;						// common dialog box structure 
	HWND hItem = GetFocus ();
	FmPanelProps* panelsProps = (FmPanelProps*) GetProp (hItem, L"Props");
	if (panelsProps == NULL) return 0;
	DWORD selIndex = (DWORD) SendMessage (hItem, LVM_GETSELECTIONMARK, 0, 0) & 1;
		
	// Initialize CHOOSECOLOR 
	ZeroMemory (&cc, sizeof (cc));
	cc.lStructSize = sizeof (cc);
	cc.hwndOwner = hDlg;
	cc.lpCustColors = (DWORD*) panelsProps->customColors;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.rgbResult = panelsProps->bkColors [selIndex];
	if (ChooseColor (&cc) == TRUE)
	{
		panelsProps->bkColors [selIndex] = cc.rgbResult;
		CopyMemory (panelsProps->customColors, cc.lpCustColors, sizeof (cc.lpCustColors));
	}
	InvalidateRect (hItem, NULL, TRUE);
	return 0;
}


INT_PTR onIdOk (HWND hLv, WPARAM wParam, LPARAM lParam)
{
	executeSelected (hLv);
	return 0;
}



INT_PTR onDialogNotify (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	NMHDR* pNmhdr = NULL;
	switch (wParam)
	{
		case IDC_LEFTPANEL:
		case IDC_RIGHTPANEL:
		{
			if (lParam == NULL) break;
			pNmhdr = (NMHDR*) lParam;
			switch (pNmhdr->code)
			{
				case NM_DBLCLK:
				{
					HWND hLv = GetDlgItem (hDlg, pNmhdr->idFrom);
					executeSelected (hLv);
					break;
				}
				case NM_RCLICK:
				{
					switch (pNmhdr->idFrom)
					{
						case IDC_LEFTPANEL:
						case IDC_RIGHTPANEL:
						NMITEMACTIVATE* itemAct = (NMITEMACTIVATE*) lParam;
						FmPanelProps* props = (FmPanelProps*) GetProp (pNmhdr->hwndFrom, L"Props");
						props->steckSels[props->steckInd] = itemAct->iItem;
					}
					onRightClick (hDlg, lParam);
					break;
				}
				case LVN_KEYDOWN:
					if (lParam != NULL)
					{
						NMLVKEYDOWN* pNmkd = (NMLVKEYDOWN*) lParam;
						switch (pNmkd->wVKey)
						{
							case VK_F5: // Команда "Копировать"
								if (pNmkd->hdr.idFrom == IDC_LEFTPANEL)
								{
									copySelected (hDlg, IDC_DISK_LIST_LEFT, pNmkd->hdr.idFrom, IDC_DISK_LIST_RIGHT, IDC_RIGHTPANEL, FM_OPERATION_COPY);
								}
								else if (pNmkd->hdr.idFrom == IDC_RIGHTPANEL)
								{
									copySelected (hDlg, IDC_DISK_LIST_RIGHT, pNmkd->hdr.idFrom, IDC_DISK_LIST_LEFT, IDC_LEFTPANEL, FM_OPERATION_COPY);
								}
								break;
							case VK_DELETE: // Команда "Удалить"
								if (pNmkd->hdr.idFrom == IDC_LEFTPANEL)
								{
									deleteSelected (hDlg, IDC_DISK_LIST_LEFT, pNmkd->hdr.idFrom);
								}
								else if (pNmkd->hdr.idFrom == IDC_RIGHTPANEL)
								{
									deleteSelected (hDlg, IDC_DISK_LIST_RIGHT, pNmkd->hdr.idFrom);
								}
								break;
							case VK_F6: // Команда "Переместить"
								if (pNmkd->hdr.idFrom == IDC_LEFTPANEL)
								{
									copySelected (hDlg, IDC_DISK_LIST_LEFT, pNmkd->hdr.idFrom, IDC_DISK_LIST_RIGHT, IDC_RIGHTPANEL, FM_OPERATION_MOVE);
								}
								else if (pNmkd->hdr.idFrom == IDC_RIGHTPANEL)
								{
									copySelected(hDlg, IDC_DISK_LIST_RIGHT, pNmkd->hdr.idFrom, IDC_DISK_LIST_LEFT, IDC_LEFTPANEL, FM_OPERATION_MOVE);
								}
								break;
							case VK_F7: // Команда "Создать папку"
							{
								if (pNmkd->hdr.idFrom == IDC_LEFTPANEL)
								{
									createFolder (hDlg, GetDlgItem (hDlg, IDC_DISK_LIST_LEFT));
								}
								else if (pNmkd->hdr.idFrom == IDC_RIGHTPANEL)
								{
									createFolder (hDlg, GetDlgItem (hDlg, IDC_DISK_LIST_RIGHT));
								}
								else
								{
									printfd ("onDialogNotify:VK_F7:Неизвестный pNmkd->hdr.idFrom:%u\n", pNmkd->hdr.idFrom);
								}
								break;
							}
							case VK_BACK:
								printfd ("VK_BACK\n");
								break;
						}
					}
					break;
				case LVN_ITEMCHANGED:
					onItemChanged (hDlg, wParam, lParam);
					break;
				case NM_CUSTOMDRAW:
					switch (pNmhdr->idFrom)
					{
						case IDC_LEFTPANEL:
						case IDC_RIGHTPANEL:
						{
							LONG ln = onCustomDrawPanel (hDlg, lParam);
							SetWindowLong (hDlg, DWL_MSGRESULT, ln);
							return TRUE;
						}
					}
					break;
				case LVN_COLUMNCLICK:
					FmPanelProps* props = (FmPanelProps*) GetProp (pNmhdr->hwndFrom, L"Props");
					props->sortDir = !props->sortDir;
					onColumnClick (hDlg, (WPARAM)pNmhdr->hwndFrom, lParam);
					return TRUE;
			}
		}
		break;
	}
	return 0;
}


LONG onCustomDrawPanel (HWND hDlg, LPARAM lParam)
{
	NMLVCUSTOMDRAW* lvCustomDraw = (NMLVCUSTOMDRAW*) lParam;
	FmPanelProps* panelProps = (FmPanelProps*) GetProp (((NMHDR*)lParam)->hwndFrom, L"Props");
	if (panelProps == NULL) return 0;
	switch (lvCustomDraw->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			return CDRF_NOTIFYITEMDRAW;
		case CDDS_ITEMPREPAINT:
		{
			lvCustomDraw->clrTextBk = panelProps->bkColors [lvCustomDraw->nmcd.dwItemSpec & 1];
			return CDRF_NOTIFYSUBITEMDRAW;
		}
		case CDDS_ITEM | CDDS_SUBITEM | CDDS_PREPAINT:
			COLORREF clr = panelProps->bkColors [lvCustomDraw->nmcd.dwItemSpec & 1];
			if (lvCustomDraw->iSubItem != panelProps->sortColumnIndex)
			{
				lvCustomDraw->clrTextBk = (COLORREF) RGB (GetRValue(clr) - 20, GetGValue(clr) - 20, GetBValue(clr) - 20);
			}
			else
			{
				lvCustomDraw->clrTextBk = (COLORREF) RGB (GetRValue(clr), GetGValue(clr), GetBValue(clr));
			}
			switch (lvCustomDraw->iSubItem)
			{
				case 0:
					lvCustomDraw->clrText = RGB (0, 0, 0);
					break;
				case 1:
					lvCustomDraw->clrText = RGB (80, 100, 80);
					break;
				case 2:
					lvCustomDraw->clrText = RGB (20, 20, 255);
					break;
				case 3:
					lvCustomDraw->clrText = RGB (255, 20, 255);
					break;
				default:
					lvCustomDraw->clrText = RGB (0, 0, 0);
					break;
			}
			return CDRF_NEWFONT;
	}
   	return 0;
}







INT_PTR onDialogInit (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	wprintfd (L"Создан главный диалог %08xh\n", hDlg);
	
	SetFocus (hDlg);	

	// Заполнить список доступных дисков
	fillDiskList (hDlg, IDC_DISK_LIST_LEFT);
	fillDiskList (hDlg, IDC_DISK_LIST_RIGHT);
	
	// Список для иконок панелей
	HIMAGELIST hIL = ImageList_Create (16, 16, ILC_COLORDDB, 0, 1);
	SendDlgItemMessage (hDlg, IDC_LEFTPANEL, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hIL);
	hIL = ImageList_Create (16, 16, ILC_COLORDDB, 0, 1);
	SendDlgItemMessage (hDlg, IDC_RIGHTPANEL, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hIL);

	HWND hLv;
	
	// Выделить память для наблюдателя
	Watcher* watcher = new Watcher;

	// Выделить память для сохранения структуры с параметрами левой панели
	FmPanelProps* panelProps = (FmPanelProps*) fmAlloc (sizeof (FmPanelProps));
	
	// Заполнить параметры структуры с параметрами левой панели 	
	panelProps->сountedItem = -1;
	//panelProps->curSel = 0;
	panelProps->steckSels [0] = 0;
	panelProps->steckInd = 0;

	panelProps->hDlg = hDlg;
	panelProps->hFullPath = GetDlgItem (hDlg, IDC_DISK_LIST_LEFT);
	panelProps->curDir = watcher->getFolderL();
	hLv = GetDlgItem (hDlg, IDC_LEFTPANEL);
	panelProps->defProc = (WNDPROC) SetWindowLong (hLv, GWL_WNDPROC, (LONG)subClassLvProc);
	SetProp (hLv, L"Props", (HANDLE) panelProps);
	SetWindowLong (hLv, GWL_USERDATA, (LONG)panelProps);
	
	// Выделить память для сохранения структуры с параметрами правой панели
	panelProps = (FmPanelProps*) fmAlloc (sizeof (FmPanelProps));
	
	// Заполнить параметры структуры с параметрами правой панели
	panelProps->сountedItem = -1;
	//panelProps->curSel = 0;
	panelProps->steckSels [0] = 0;
	panelProps->steckInd = 0;
	panelProps->hDlg = hDlg;
	panelProps->hFullPath = GetDlgItem (hDlg, IDC_DISK_LIST_RIGHT);
	panelProps->curDir = watcher->getFolderR();
	hLv = GetDlgItem (hDlg, IDC_RIGHTPANEL);
	panelProps->defProc = (WNDPROC) SetWindowLong (hLv, GWL_WNDPROC, (LONG)subClassLvProc);
	SetProp (hLv, L"Props", (HANDLE) panelProps);
	SetWindowLong (hLv, GWL_USERDATA, (LONG)panelProps);
	
	// Загрузить свойства элементов диалога
	TCHAR iniFileName [FM_MAX_PATH];
	getIniFileName (iniFileName, SIZETC(iniFileName));
	loadDlgItemProperties (hDlg, IDD_MAIN, iniFileName);
	loadDlgItemProperties (hDlg, IDC_DISK_LIST_LEFT, iniFileName);
	loadDlgItemProperties (hDlg, IDC_DISK_LIST_RIGHT, iniFileName);
	loadDlgItemProperties (hDlg, IDC_LEFTPANEL, iniFileName);
	loadDlgItemProperties (hDlg, IDC_RIGHTPANEL, iniFileName);

	// Настройка Наблюдателя 
	LTPD ltpd;
	ltpd.hDlg = hDlg;
	watcher->setDirChangeHandler (onDirChangeHandler);
	SIZE_T pathSize = ltpd.getPathAllocSize ();
	
	SendDlgItemMessage (hDlg, IDC_DISK_LIST_RIGHT, WM_GETTEXT, pathSize, (LPARAM)ltpd.path);
	ltpd.hLv = GetDlgItem (hDlg, IDC_RIGHTPANEL);
	watcher->setLtpdR (&ltpd);
	
	SendDlgItemMessage (hDlg, IDC_DISK_LIST_LEFT, WM_GETTEXT, pathSize, (LPARAM)ltpd.path);
	ltpd.hLv = GetDlgItem (hDlg, IDC_LEFTPANEL);
	watcher->setLtpdL (&ltpd);
	SetProp (hDlg, L"Watcher", watcher);

	SetProp (GetDlgItem(hDlg, IDC_LEFTPANEL),  L"diskListId", (HANDLE) IDC_DISK_LIST_LEFT);
	SetProp (GetDlgItem(hDlg, IDC_RIGHTPANEL), L"diskListId", (HANDLE) IDC_DISK_LIST_RIGHT);	
	SetProp (GetDlgItem(hDlg, IDC_LEFTPANEL),  L"Selection size Id", (HANDLE) IDC_BYTES_SELECTED_LEFT);
	SetProp (GetDlgItem(hDlg, IDC_RIGHTPANEL), L"Selection size Id", (HANDLE) IDC_BYTES_SELECTED_RIGHT);

	SetTimer (hDlg, FM_UPDATE_TIMER, FM_UPDATE_TIMER, (TIMERPROC) NULL); 
	dlgMutex = CreateMutex (NULL, FALSE, NULL);

	// Приём drug and drop файлов
	DragAcceptFiles (hDlg, TRUE);

	SetProp (GetDlgItem(hDlg, IDC_LEFTPANEL), L"HFULLPATH", (HANDLE)GetDlgItem(hDlg, IDC_DISK_LIST_LEFT));
	SetProp (GetDlgItem(hDlg, IDC_RIGHTPANEL), L"HFULLPATH", (HANDLE)GetDlgItem(hDlg, IDC_DISK_LIST_RIGHT));

	listDirectoryToPanel (GetDlgItem (hDlg, IDC_LEFTPANEL));
	listDirectoryToPanel (GetDlgItem (hDlg, IDC_RIGHTPANEL));

	// Сообщения оболчки об добавлении или удалении носителя
    // Request notification from shell of media insertion -
    // allows us to detect removable media or a multimedia card
	ITEMIDLIST* ppidl;
	if (S_OK == SHGetSpecialFolderLocation (hDlg, CSIDL_DESKTOP, &ppidl))
	{
		SHChangeNotifyEntry shCNE;
		shCNE.pidl = ppidl;
		shCNE.fRecursive = TRUE;
	        
		// Returns a positive integer registration identifier (ID).
        // Returns zero if out of memory or in response to invalid parameters.
		m_ulSHChangeNotifyRegister =
			SHChangeNotifyRegister (hDlg,			 // Hwnd to receive notification
            SHCNE_DISKEVENTS,						 // Event types of interest (sources)                         
            SHCNE_MEDIAINSERTED | SHCNE_MEDIAREMOVED,// Events of interest - use          
													// SHCNE_ALLEVENTS for all events
			WM_USER_MEDIACHANGED,					// Notification message to be sent 
													// upon the event
			1,                                      // Number of entries in the pfsne array
			&shCNE);								// Array of SHChangeNotifyEntry structures that 
													// contain the notifications. This array should 
													// always be set to one when calling SHChnageNotifyRegister
													// or SHChangeNotifyDeregister will not work properly.


	}
	
	watcher->start ();
	
	return 0;
}


BOOL onDirChangeHandler (HWND hLv)
{
	KillTimer (hLv, FM_DIRCHANGE_TIMER);
	SetTimer (hLv, FM_DIRCHANGE_TIMER, FM_DIRCHANGE_TIMER, NULL);
	listDirectoryToPanelWorker (hLv);
	return TRUE;
}


INT_PTR onDialogSize (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	const DWORD upBarHight = 21;	
	const DWORD downBarHight = 21;
	const DWORD upBarHight2 = upBarHight << 1;

	DWORD halfDlgWidth = LOWORD (lParam) >> 1;
	DWORD dlgHigh = HIWORD (lParam);
	
	DWORD panelHight = dlgHigh - upBarHight2 - downBarHight;

	SendMessage (hDlg, WM_SETREDRAW, (WPARAM) FALSE, 0);
	setDlgItemPos (hDlg, IDC_DISK_LIST_LEFT, 0, 0, halfDlgWidth, upBarHight);
	setDlgItemPos (hDlg, IDC_FREE_LEFT, 0, upBarHight, halfDlgWidth, upBarHight);
	setDlgItemPos (hDlg, IDC_LEFTPANEL, 0, upBarHight2, halfDlgWidth, panelHight);
	setDlgItemPos (hDlg, IDC_DISK_LIST_RIGHT, halfDlgWidth, 0, halfDlgWidth, upBarHight);
	setDlgItemPos (hDlg, IDC_FREE_RIGHT, halfDlgWidth, upBarHight, halfDlgWidth, upBarHight);
	setDlgItemPos (hDlg, IDC_RIGHTPANEL, halfDlgWidth, upBarHight2, halfDlgWidth, panelHight);
	
	DWORD panelWidth = halfDlgWidth - 20;
	HWND hLv = GetDlgItem (hDlg, IDC_LEFTPANEL);
	setColumnsWidths (hLv, panelWidth);
	
	hLv = GetDlgItem (hDlg, IDC_RIGHTPANEL);
	setColumnsWidths (hLv, panelWidth);

	setDlgItemPos (hDlg, IDC_BYTES_SELECTED_LEFT, 0, dlgHigh - downBarHight, halfDlgWidth, downBarHight);
	setDlgItemPos (hDlg, IDC_BYTES_SELECTED_RIGHT, halfDlgWidth, dlgHigh - downBarHight, halfDlgWidth, downBarHight);
	
	SendMessage (hDlg, WM_SETREDRAW, (WPARAM) TRUE, 0);
	
	InvalidateRect (hDlg, NULL, FALSE);
	return 0;
}


VOID setColumnsWidths (HWND hLv, UINT width)
{
	DWORD widths [4];
	//FmPanelProps* props = (FmPanelProps*) GetProp (hLv, L"Props");
	
	widths [FM_COLUMN_SIZE] = 80;	// Колонка размера имеет фиксированную ширину
	widths [FM_COLUMN_DATE] = 100;	// Колонка даты имеет фиксированную ширину
	widths [FM_COLUMN_TYPE] = 50;	// Колонка даты имеет фиксированную ширину
	
	DWORD fixWidth = widths [FM_COLUMN_DATE]
				   + widths [FM_COLUMN_SIZE]
				   + widths [FM_COLUMN_TYPE];
	
	if (width > fixWidth)
	{
		widths [0] = DWORD (width - fixWidth);
	}
	else
	{
		widths [0] = 50;
	}
						
	setListVeiwColumnsWidth (hLv, SIZETC (widths), widths);
}


BOOL setDlgItemPos(HWND hDlg, UINT itemId, int X, int Y, int cx, int cy)
{
	return SetWindowPos (GetDlgItem(hDlg, itemId), HWND_TOP, X, Y, cx, cy, SWP_SHOWWINDOW);
}


INT_PTR onDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR iniFileName [FM_MAX_PATH];
	
	KillTimer (hDlg, FM_UPDATE_TIMER);
	CloseHandle (dlgMutex);
	
	delete (Watcher*) RemoveProp (hDlg, L"Watcher");
	
	getIniFileName (iniFileName, sizeof (iniFileName));
	saveDlgItemProperties (hDlg, IDD_MAIN, iniFileName);
	saveDlgItemProperties (hDlg, IDC_DISK_LIST_LEFT, iniFileName);
	saveDlgItemProperties (hDlg, IDC_DISK_LIST_RIGHT, iniFileName);
	saveDlgItemProperties (hDlg, IDC_LEFTPANEL, iniFileName);
	saveDlgItemProperties (hDlg, IDC_RIGHTPANEL, iniFileName);
	RemoveProp (GetDlgItem(hDlg, IDC_LEFTPANEL), L"diskListId");
	RemoveProp (GetDlgItem(hDlg, IDC_RIGHTPANEL), L"diskListId");
	SHChangeNotifyDeregister (m_ulSHChangeNotifyRegister);
	EndDialog (hDlg, 0);
	wprintfd (L"Закрыт главный диалог %08xh\n", hDlg);
	return 0;
}
