// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Заголовки вспомогательных процедур

#ifndef _helper_h_
#define _helper_h_

#include "build.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <memory.h>
#include <psapi.h>
#include <string.h>
#include <wchar.h>
#include <tchar.h>

#include "consts.h"


#define SIZETC(arg) (sizeof(arg)/sizeof(arg[0]))


int multiByteToWideChar (TCHAR* wcStr, char* mbStr, UINT codePage = CP_ACP);


/** Возвращает указтель на первый справо символ в строке
TCHAR* str - указатель на строку
TCHAR rchar - символ */
TCHAR* lstrrchr (TCHAR* str, TCHAR rchar);


/** Укорачивает строку справо до первого заданного символа
TCHAR* str - укорачиваемая строка
TCHAR ch - заданный символ
Возвращает указатель на певый заданный символ в строке,
если такой был найден, или NULL если такого символа нет в строке */
TCHAR* lcutrchr (TCHAR* str, TCHAR rchar);


/** Добавляет символ косой черты к концу строки с путём,
если такого символа нет в конце строки
TCHAR* path - строка с путём
Возвращает TRUE если строка path была изменена,
FALSE если не была изменена */
BOOL addSlashToPath (TCHAR* path);


/** Удаляет символ косой черты в конце строки с путём
TCHAR* path - строка с путём
Возвращает TRUE если строка path была изменена, FALSE если не была изменина */
BOOL delSlashFromPath (TCHAR* path);


/** Выводит форматированную строку в Windows Debug output */
VOID wprintfd (TCHAR* format, ...);	


/** Выводит форматированную строку в Windows Debug output */
VOID printfd (char* format, ...);


/** Показывает сообщение системы об ошибке
HWND hDlg - дескриптор диалога
DWORD err - код ошибки 
TCHAR* caption - заголовок сообщения */
void showErrorMessage (HWND hDlg, DWORD err, TCHAR* caption);


/** Загружает строку из ресурсов
HWND hDlg - дескриптор диалога
UINT strId - идентификатор строки
TCHAR* destStr - выходной буфер
size_t strLen - длинна входного буфера */
BOOL loadStringFromResource (HWND hDlg, UINT strId, TCHAR* destStr, SIZE_T strLen);


HLOCAL fmLocalFree (HLOCAL hMem);
HLOCAL fmLocalAlloc (SIZE_T len);
SIZE_T fmLocalSize (HLOCAL hMem);


/** Возвращает имя файла по его дескриптору
HANDLE hFile - дескриптор файла
TCHAR* pszFilename - указатель на буфер, в который будет помещено имя файла
SIZE_T fileNameLen - длинна буфера pszFilename */
BOOL getFileNameFromHandle (HANDLE hFile, TCHAR* pszFilename, SIZE_T fileNameLen);


/** Приводит путь к нормальному виду. Если строка путь к директории,
то добавляет символ L'\\' в конец строки, Если строка путь к файлу,
то убирает символ L'\\' в конце строки, если символ L'\\' есть
TCHAR* path - Строка с путём */
DWORD normalizePath	(TCHAR* path);


/** Обрезает путь на один уровень выше (комманда "cd ..")
TCHAR* path - путь.
Возвращает TRUE, если путь был обрезан, FALSE если путь не был обрезан */
BOOL cutPath (TCHAR* path);

/** Добавляет путь на один уровень вниз
TCHAR* curPath - текущий путь.
TCHAR* addPath - добавляемый путь.
Возвращает TRUE, если путь был добавлен, FALSE если путь не был добавлен */
DWORD addPath (TCHAR* curPath, TCHAR* addPath);

/** Возвращает хэш-код строки
TCHAR* str - указатель на строку */
DWORD getHash (TCHAR* str);



BOOL fmGetDiskFreeSpace (TCHAR* pszDrive, ULARGE_INTEGER* i64FreeBytes);


#endif // _helper_h_