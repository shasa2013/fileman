// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.

#ifndef _MOVE_H_
#define _MOVE_H_

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
INT CALLBACK moveDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI moveFileWorker (VOID* data);
/*DWORD CALLBACK moveProgressRoutine (LARGE_INTEGER TotalFileSize,
									LARGE_INTEGER TotalBytesTransferred,
									LARGE_INTEGER StreamSize,
									LARGE_INTEGER StreamBytesTransferred,
									DWORD dwStreamNumber,
									DWORD dwCallbackReason,
									HANDLE hSrcFile,
									HANDLE hDstFile,
									LPVOID lpData);
*/
// Процедуры обработки сообщений окна

INT_PTR onMoveDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onMoveDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onMovePauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam);


BOOL moveSelected (HWND hDlg, UINT fullPathIdSrc, UINT panelIdSrc, UINT fullPathIdDst, UINT panelIdDst);





#endif // _MOVE_H_


