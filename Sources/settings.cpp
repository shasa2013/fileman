#include "settings.h"

/*DLGTEMPLATE* WINAPI doLockDlgRes (TCHAR lpszResName) 
{ 
  //  HRSRC hrsrc = FindResource (NULL, lpszResName, RT_DIALOG); 
   // HGLOBAL hglb = LoadResource (g_hinst, hrsrc); 
    //return (DLGTEMPLATE*) LockResource (hglb); 
	
}*/


INT_PTR	onSettings (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	static BOOL isCalled = FALSE;
	if (isCalled) return NULL;
	isCalled = TRUE;
	HINSTANCE hInst = (HINSTANCE) GetWindowLong (hDlg, GWLP_HINSTANCE);
	DialogBox (hInst, (LPCTSTR) MAKEINTRESOURCE (ID_DLG_SETTINGS), NULL, settingsDlgProc);
	isCalled = FALSE;
	return NULL;
}

INT CALLBACK settingsDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
    {
        case WM_COMMAND:
			switch (LOWORD(wParam))
            {
				case IDCANCEL:
					onSettingsDlgCancel (hDlg, wParam, lParam);
					break;
			}
		case WM_INITDIALOG:
            onSettingsDlgInit (hDlg, wParam, lParam);
			return TRUE;
	}
	return NULL;
}

INT CALLBACK settingsPageProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
    {
        case WM_COMMAND:
			switch (LOWORD(wParam))
            {
				case IDCANCEL:
					onSettingsPageCancel (hDlg, wParam, lParam);
					break;
			}
		case WM_INITDIALOG:
            onSettingsPageInit (hDlg, wParam, lParam);
			return TRUE;
	}
	return NULL;
}

INT_PTR onSettingsDlgInit (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst = (HINSTANCE) GetWindowLong (hDlg, GWLP_HINSTANCE);
	DialogBox (hInst, (LPCTSTR) MAKEINTRESOURCE (ID_DLG_COLORS), hDlg, settingsPageProc);
	return NULL;
}

INT_PTR onSettingsDlgCancel	(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	EndDialog (hDlg, NULL);
	return NULL;
}


INT_PTR onSettingsPageInit (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return NULL;
}

INT_PTR onSettingsPageCancel (HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	EndDialog (hDlg, 0);
	return NULL;
}