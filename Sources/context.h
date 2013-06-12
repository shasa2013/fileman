// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур для контекстного меню

#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "browse.h"

#include <shlwapi.h>
#include <shlobj.h>

BOOL showContextMenu (HWND hDlg, TCHAR* fullPath, WNDPROC menuProc);
LRESULT CALLBACK contextMenuProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR onContextMenu (HWND hDlg, WPARAM wParam, LPARAM lParam);
VOID displayContextMenu (HWND hwnd, POINT pt);
BOOL onRightClick (HWND hDlg, LPARAM lParam);

#endif // _CONTEXT_H_