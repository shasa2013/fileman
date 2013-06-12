// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур

#ifndef _fileMan_h_
#define _fileMan_h_

#include "build.h"

#include <commctrl.h>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include <Commdlg.h>

#include "helper.h"
#include "resource.h"
#include "copy.h"
#include "user.h"
#include "browse.h"
#include "task.h"
#include "watcher.h"
#include "delete.h"
#include "move.h"
#include "count.h"
#include "context.h"
#include "consts.h"
#include "folder.h"
#include "settings.h"


typedef struct _ColRGB
{
	unsigned char a;
	unsigned char b;
	unsigned char g;
	unsigned char r;
} ColRGB;


typedef struct
{
    DWORD dwItem1;    // dwItem1 contains the previous PIDL or name of the folder. 
    DWORD dwItem2;    // dwItem2 contains the new PIDL or name of the folder. 
} SHNOTIFYSTRUCT;



/** Обработчики сообщений окна и Стандартные процедуры - только в этих
процедурах можно использовать константы идентификаторов элементов
диалогов, определённых в resource.h */

/** */
VOID setColumnsWidths (HWND hLv, UINT width);


/** Приём сообщений главного окна */
INT CALLBACK mainDlgProc (HWND hDlg, UINT msg,  WPARAM wParam, LPARAM lParam);

/** Приём сообщений панели */
INT CALLBACK subClassLvProc (HWND hLv, UINT Message, WPARAM wParam, LONG lParam);

/** Обработчик сообщения окна "инициализация диалога"
HWND hDlg - дескриптор главного окна */
INT_PTR onDialogInit (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR onDialogNotify (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна "изменение размеров"
HWND hDlg - дескриптор главного диалога
WPARAM wParam, LPARAM lParam - аргументы сообщения WM_SIZE */
INT_PTR onDialogSize (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик завершения диалога отображения прогресса копирования
HWND hDlg - дескриптор главного окна */
INT_PTR onDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR onIdOk (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR onSetBkColor (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR onTimer (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR	onItemChanged (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR	onNewFolder (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR	onDropFiles (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR	onListViewEndTrack (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна */
INT_PTR	onListViewTrack (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик своего сообщения окна WM_USER_MEDIACHANGED
LPARAM lParam - contains the event SHCNE_xxxx
WPARAM wParam - supplies the SHNOTIFYSTRUCT */
INT_PTR	onMediaChanged (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик сообщения окна WM_DEVICECHANGED
LPARAM lParam - contains the event SHCNE_xxxx
WPARAM wParam - supplies the SHNOTIFYSTRUCT */
INT_PTR	onDeviceChange (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Особые процедуры */

/** Устанавливает новое положение окна
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор элемента
int X, int Y - новое положение X, Y
int cx, int cy - длинна X, Y */
BOOL setDlgItemPos (HWND hDlg, UINT itemId, int X, int Y, int cx, int cy);

LONG onCustomDrawPanel (HWND hDlg, LPARAM lParam);

/** Обработчик события Папка изменилась */
BOOL onDirChangeHandler (HWND hLv);

HLOCAL fmLocalFree (HLOCAL hMem);
HLOCAL fmLocalAlloc (SIZE_T len);
SIZE_T fmLocalSize (HLOCAL hMem);



#endif // _fileMan_h_
