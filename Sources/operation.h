#ifndef _OPERATION_H_
#define _OPERATION_H_


#include "build.h"

#include <stdlib.h>
#include "progress.h"
#include "consts.h"


/** ¬озвращает TRUE если рабочий поток операции дл€ заданного
диалога существует
HWND hDlg - дескриптор диалога операции */
BOOL isOperationThreadExist (HWND hDlg);


/** —охран€ет дескриптор потока копировани€
HWND hDlg - дескриптор диалога прогесса операции
UINT itemId - идентификатор диалога копировани€
HANDLE hOperThread - дескриптор потока копировани€ */
BOOL setOperationThread (HWND hDlg, HANDLE hThread, BOOL isSuspended);


/** ¬озвращает TRUE, если дл€ диалога копировани€ существует
HWND hDlg - дескриптор диалога прогесса операции,
UINT itemId - идентификатор диалога операции */
BOOL isThreadExist (HWND hDlg);


/** ”дал€ет дескриптор потока копировани€
HWND hDlg - дескриптор диалога прогесса операции
¬озвращает - TRUE если дл€ указанного диалга, поток копировани€ был удалЄн,
FALSE - если дл€ указанного диалга, нет потока копировани€ */
BOOL removeOperationThread (HWND hDlg);


/** ¬озвращает состо€ние диалога копировани€
HWND hOperDlg - дескриптор диалога поргресса операции
UINT itemId - идентификатор диалога поргресса операции */
BOOL isOperationThreadSuspended (HWND hDlg);


/** ¬озобновл€ет поток операции
HWND hDlg - дескриптор диалога поргресса операции,
UINT itemId - идентификатор диалога операции */
BOOL resumeOperationThread (HWND hDlg);


/** ѕриостанавливает поток операции
HWND hDlg - дескриптор диалога поргресса операции,
UINT itemId - идентификатор диалога операции */
BOOL suspendOperationThread (HWND hDlg);


/** ¬ыполн€ет рекурсивную операцию над пут€ми
WIN32_FIND_DATA* findData - указатель на структуру WIN32_FIND_DATA
TCHAR* beginFolder - начальный путь источник
TCHAR* dstFolder - начальна€ папка пути назначени€ (дл€ некоторых операций может быть NULL)
LPPROGRESS_ROUTINE routine - указатель на процедуру отображени€ хода операции
VOID* arg - аргумент передаваемый в процедуру отображени€ хода операции
DWORD flags - флаги дл€ рекурсивной операции
DWORD operation - выполн€ема€ операци€, определена в consts.h
UINT64* bytesCount - суммарный размер файлов. »спользуетс€ с FM_OPERATION_COUNT */
INT recurseOperation (WIN32_FIND_DATA* findData, TCHAR* beginPath, TCHAR* dstFolder,
	LPPROGRESS_ROUTINE routine, VOID* arg, DWORD flags, DWORD operation, UINT64* bytesCount);



#endif // _OPERATION_H_
