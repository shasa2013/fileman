/* Часть кода Fileman.
Горшков Александр Сергеевич, 2009 г, Таганрог.
Заголовки процедур для операции "копирование" */

#ifndef _COPY_H_
#define _COPY_H_

#include "build.h"

#include <commctrl.h>

#include "helper.h"
#include "resource.h"
#include "browse.h"
#include "user.h"
#include "task.h"
#include "progress.h"
#include "operation.h"

typedef struct _ProgrBarUserData
{
	WNDPROC defWndProc;
	TCHAR* text;
	HFONT hFont;
} ProgrBarUserData;


// Стандартные процедуры
INT CALLBACK copyDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT CALLBACK progrBarWndProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI copyFileWorker (VOID* data);

// Процедуры обработки сообщений окна
INT_PTR onCopyDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик нормального завершения диалога копирования
HWND hDlg - дескриптор диалога копирования
LPARAM lParam - указатель на экземпляр класса CopyFileData */
INT_PTR onCopyDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Обработчик нажатия кнопки Пауза диалога копирования */
INT_PTR onCopyPauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** */
INT_PTR onCopyDlgInit (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** Рисует прогрессбар с процентами в двухцветной схеме */
INT_PTR onProgrBarPaint (HWND hDlg, WPARAM wParam, LPARAM lParam);

/**  */
INT_PTR onProgrBarSetText (HWND hDlg, WPARAM wParam, LPARAM lParam);

/**  */
INT_PTR onCopyDlgDestroy (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** Копирует выделенные файлы */
BOOL copySelected (HWND hDlg, UINT fullPathIdSrc, UINT panelIdSrc, UINT fullPathIdDst, UINT panelIdDst, INT opCode);

/** Копирует Drug and Drop файлы */
BOOL copyDropped (HWND hDlg, Task* task);



#endif // _COPY_H_
