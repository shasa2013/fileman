// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки процедур для операции "подсчёт"

#ifndef _COUNT_H_
#define _COUNT_H_

#include "build.h"
#include <shlwapi.h>

#include "task.h"
#include "operation.h"
#include "globals.h"

/** Запускает поток подсчёта суммарного размера файлов */
BOOL countSelected (HWND hDlg);


/** Процедура рабочего потока. Подсчитывает суммарный размер файлов */
DWORD WINAPI countWorker (VOID* data);


/** Вызывается когда подсчёт размера файлов завершён
HWND hDlg - дескриптор главного диалога
PARAM wParam - не используется
LPARAM lParam - указатель на класс Task */
INT_PTR onCountFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** Вызывается во время подсчёта суммарного размера
файлов для отображения прогресса выполнения
HWND hDlg - дескриптор главного диалога
WPARAM wParam - идентификатор панели
LPARAM lParam - указатель на переменную типа DWORDLONG
с текущим суммарным размером файлов */
INT_PTR onCountProgress (HWND hDlg, WPARAM wParam, LPARAM lParam);


#endif // _COUNT_H_