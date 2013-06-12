// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.

#ifndef _LTPD_H_
#define _LTPD_H_


#include "build.h"

#include "consts.h"
#include "fmmemory.h"

/** Класс для хранения задания "Пролистать директорию в панель" */
class LTPD
{
public:
	HWND hDlg;						/*< Дескриптор главного диалога		*/
	HWND hLv;						/*< Дескриптор панели				*/
	TCHAR* path;					/*< Путь к директории				*/
	LTPD (TCHAR* path);				/*< Конструктор						*/
	LTPD (VOID);					/*< Конструктор						*/
	SIZE_T getPathAllocSize (VOID);
	~ LTPD ();
};

#endif // _LTPD_H_