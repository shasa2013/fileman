// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур для операции "удаление"

#ifndef _DELETE_H_
#define _DELETE_H_

#include "build.h"

#include <commctrl.h>

#include "helper.h"
#include "resource.h"
#include "browse.h"
#include "user.h"
#include "task.h"
#include "progress.h"
#include "operation.h"


// Стандартные процедуры
INT CALLBACK deleteDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI deleteWorker  (VOID* data);
DWORD CALLBACK deleteProgressRoutine
	(LARGE_INTEGER notUsed1, LARGE_INTEGER notUsed2, LARGE_INTEGER notUsed3,
	LARGE_INTEGER notUsed4, DWORD notUsed5, DWORD notUsed6, HANDLE notUsed7,
	HANDLE hDlg, LPVOID lpData);

/** Процедура обработки сообщений окна */
INT_PTR onDeleteDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Процедура обработки сообщений окна */
INT_PTR onDeleteDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Процедура обработки сообщений окна */
INT_PTR onDeletePauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Процедура обработки сообщений окна */
INT_PTR onDeleteDialogInit (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** Запускает поток удаления выбранных элементов
HWND hDlg - дескриптор главного диалога
UINT fullPathId - идентификатор полного пути
UINT panelId - идентификатор панели */
BOOL deleteSelected (HWND hDlg, UINT fullPathId, UINT panelId);


#endif // _DELETE_H_