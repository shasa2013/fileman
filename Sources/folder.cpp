#include "folder.h"


INT CALLBACK newFolderDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
    {
        case WM_COMMAND:
			switch (LOWORD(wParam))
            {
                case IDCANCEL:
					onNewFolderCancel (hDlg, wParam, lParam);
					return (INT_PTR) FALSE;
				case IDC_NEW_FOLDER_OK:
					onNewFolderOk (hDlg, wParam, lParam);
					return (INT_PTR) FALSE;
			}
			break;
		case WM_INITDIALOG:
			SetProp (hDlg, L"hFullPath", (HANDLE)lParam);
			SendDlgItemMessage (hDlg, IDC_NEW_FOLDER_NAME, WM_SETTEXT, 0, (LPARAM)L"Новая папка");
			return (INT_PTR) TRUE;
	}
	return (INT_PTR) FALSE;
}



BOOL createFolder (HWND hDlg, HWND hFullPath)
{	
	HINSTANCE hInst = (HINSTANCE) GetWindowLong (hDlg, GWL_HINSTANCE);
	DialogBoxParam (hInst, MAKEINTRESOURCE (IDD_NEW_FOLDER), hDlg, newFolderDlgProc, (LPARAM)hFullPath);
	return NULL;
}


INT_PTR onNewFolderOk (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR path [FM_MAX_PATH];
	TCHAR folder [MAX_PATH];
	
	SendDlgItemMessage (hDlg, IDC_NEW_FOLDER_NAME, WM_GETTEXT, SIZETC(folder), (LPARAM)folder);
	
	HWND hFullPath = (HWND) GetProp (hDlg, L"hFullPath");
	SendMessage (hFullPath, WM_GETTEXT, SIZETC(path), (LPARAM)path);
		
	lstrcat (path, folder);
	
	if (!CreateDirectory (path, NULL))
	{
		DWORD err = GetLastError();
		loadStringFromResource (hDlg, IDS_EXECUTE_ERR_MSG_TITLE, path, SIZETC(path));
		showErrorMessage (hDlg, err, path);
	}
	
	EndDialog (hDlg, 0);
	return NULL;
}


INT_PTR onNewFolderCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	EndDialog (hDlg, 0);
	return NULL;
}