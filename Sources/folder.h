// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур для операции "создать новую папку"

#ifndef _FOLDER_H_
#define _FOLDER_H_

#include "build.h"

#include <commctrl.h>
#include "resource.h"
#include "consts.h"
#include "helper.h"

INT CALLBACK newFolderDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR onNewFolderOk (HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onNewFolderCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Создаёт диалог для ввода назавания папки
HWND hDlg - дескриптор главного диалога
UINT fullPathId - дескриптор полного пути */
BOOL createFolder (HWND hDlg, HWND hfullPath);

#endif //_FOLDER_H_