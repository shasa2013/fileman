#include "context.h"


BOOL showContextMenu (HWND hDlg, TCHAR* FName, WNDPROC menuProc)
{
	TCHAR* FileName = PathFindFileName (FName);
	TCHAR FilePath [MAX_PATH];
	lstrcpy (FilePath, FName);
	*PathFindFileName (FilePath) = L'\0';
	
	LPSHELLFOLDER DesktopFolder;
	if (NOERROR != SHGetDesktopFolder (&DesktopFolder))
	{
		return FALSE;
	}
	
	LPITEMIDLIST ParentPidl;
	ULONG Eaten;
	if (S_OK != DesktopFolder->ParseDisplayName (hDlg, 0, FilePath, &Eaten, &ParentPidl, 0))
	{
		return FALSE;
	}
	
	LPSHELLFOLDER ParentFolder;
	if (S_OK != DesktopFolder->BindToObject (ParentPidl, 0, IID_IShellFolder, (void**)&ParentFolder))
	{
		return FALSE;
	}
	
	LPITEMIDLIST Pidl;
	if (S_OK != ParentFolder->ParseDisplayName (hDlg, 0, FileName, &Eaten, &Pidl, 0))
	{
		return FALSE;
	}	

	LPCONTEXTMENU CM;
	if (S_OK != ParentFolder->GetUIObjectOf (hDlg, 1, (LPCITEMIDLIST*)&Pidl, IID_IContextMenu, 0, (void**)&CM))
	{
		return FALSE;
	}
	
	HMENU hMenu = CreatePopupMenu ();
	if (hMenu == NULL) 
	{
		return FALSE;
	}
	
	CM->QueryContextMenu (hMenu, 0, 1, 0x7FFF, CMF_EXTENDEDVERBS | CMF_EXPLORE);
	
	WNDPROC defWndProc = (WNDPROC) SetWindowLong (hDlg, GWL_WNDPROC, (LONG)menuProc);
	SetProp (hDlg, L"defWndProc", (HANDLE) defWndProc);
	
	POINT pt;
	GetCursorPos (&pt);
	int Cmd = TrackPopupMenu (hMenu,
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
		pt.x, pt.y, 0, hDlg, 0);


	SetWindowLong (hDlg, GWL_WNDPROC, (LONG) RemoveProp (hDlg, L"defWndProc"));
		
	if (Cmd)
	{
		// Set up a CMINVOKECOMMANDINFO structure.
		CMINVOKECOMMANDINFO CI;
		ZeroMemory (&CI, sizeof(CMINVOKECOMMANDINFO));
		CI.cbSize = sizeof (CMINVOKECOMMANDINFO);
		CI.hwnd = hDlg;
		CI.lpVerb = (LPCSTR) MAKEINTRESOURCE(Cmd - 1);
		CI.lpParameters = "";
		CI.lpDirectory = "";
		CI.nShow = SW_SHOWNORMAL;
		CM->InvokeCommand (&CI);
	}
	
	return DestroyMenu (hMenu);
}


// Процедура обработки сообщений контекстного меню
LRESULT CALLBACK contextMenuProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ 
		case WM_MENUCHAR:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_INITMENUPOPUP:
			break;
	}
	return CallWindowProc ((WNDPROC) GetProp (hWnd, L"defMenuProc"), hWnd, message, wParam, lParam);
}


INT_PTR onContextMenu (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	DWORD x = LOWORD (lParam);
	DWORD y = HIWORD (lParam);
	RECT rc;                  // client area of window 
	POINT pt = {x, y};        // location of mouse click 
	
	// Get the bounding rectangle of the client area. 
	GetClientRect(hDlg, &rc); 
	
	// Convert the mouse position to client coordinates. 
	ScreenToClient (hDlg, &pt); 
	
	// If the position is in the client area, display a shortcut menu. 
	if (PtInRect (&rc, pt))
	{ 
		ClientToScreen (hDlg, &pt); 
		displayContextMenu (hDlg, pt); 
		return TRUE; 
	} 

	// Return FALSE if no menu is displayed. 
	return FALSE;
}


VOID displayContextMenu (HWND hwnd, POINT pt)  
{ 
	
	HMENU hmenu;            // top-level menu 
	HMENU hmenuTrackPopup;  // shortcut menu 

	// Load the menu resource.
	LONG hInst = GetWindowLong (hwnd, GWL_HINSTANCE);
	hmenu = LoadMenu ((HINSTANCE)hInst, MAKEINTRESOURCE (IDC_CONTEXT_MENU));
	if (hmenu == NULL) 
	{
		return; 
	}

	// TrackPopupMenu cannot display the menu bar so get the handle
	// of the first shortcut menu
	hmenuTrackPopup = GetSubMenu (hmenu, 0); 

	// Display the shortcut menu. Track the right mouse button. 
	TrackPopupMenu (hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL); 

	// Destroy the menu. 
	BOOL result = DestroyMenu (hmenuTrackPopup);
	result = DestroyMenu (hmenu); 
	
}


BOOL onRightClick (HWND hDlg, LPARAM lParam)
{
	NMHDR* nmhdr = (NMHDR*) lParam;
	switch (nmhdr->idFrom)
	{
		case IDC_LEFTPANEL:
		case IDC_RIGHTPANEL:
		{
			// Получить папку файла
			TCHAR fullPath [FM_MAX_PATH];
			fullPath [0] = L'\0';
			if (nmhdr->idFrom == IDC_LEFTPANEL)
			{
				SendDlgItemMessage (hDlg, IDC_DISK_LIST_LEFT, WM_GETTEXT, SIZETC(fullPath), (LPARAM) fullPath);
			}
			else
			{
				SendDlgItemMessage (hDlg, IDC_DISK_LIST_RIGHT, WM_GETTEXT, SIZETC(fullPath), (LPARAM) fullPath);
			}
			TCHAR pszText [MAX_PATH];
			pszText [0] = L'\0';
			LVITEM lvi = {0};
			lvi.mask = LVIF_PARAM; 
			lvi.cchTextMax = SIZETC (pszText);
			lvi.pszText = pszText;
			
			// Получить имя файла			
			lvi.iItem = SendDlgItemMessage (hDlg, nmhdr->idFrom, LVM_GETSELECTIONMARK, 0, 0);
			if (lvi.iItem <= 0) break;
			SendDlgItemMessage (hDlg, nmhdr->idFrom, LVM_GETITEM, 0, (LPARAM)&lvi);
			WIN32_FIND_DATA* findData = (WIN32_FIND_DATA*) lvi.lParam;
			lstrcat (fullPath, findData->cFileName);
			return showContextMenu (hDlg, fullPath, contextMenuProc);
		}
	}
	return FALSE;
}