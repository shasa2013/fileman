// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур для окна "Настройки"

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "build.h"

#include <commctrl.h>

#include "resource.h"

INT CALLBACK settingsDlgProc	(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR	onSettings				(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onSettingsDlgInit		(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onSettingsDlgCancel		(HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Вызывается при инициализации одной из страниц настроек */
INT CALLBACK settingsPageProc	(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR onSettingsPageInit		(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onSettingsPageCancel	(HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Loads and locks a dialog box template resource. 
Returns the address of the locked resource. 
lpszResName - name of the resource */


#endif // _SETTINGS_H_