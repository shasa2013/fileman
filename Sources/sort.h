#ifndef _FM_SORT_H_
#define _FM_SORT_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>
#include "consts.h"

// Данные для сортировки строк в ListView
struct FmSort
{
	DWORD column;		// Индекс колонки по которой сортируются строки
	BOOL dir;			// Напрвление сортировки: TRUE - по возрастанию, FALSE - по убыванию
	BOOL separate;		// Сортировка директорий и файлов: TRUE - отдельно, FALSE - вместе
	HWND hListView;		// Дескриптор ListView
	DWORD flags;		// Флаги сравнения. Определены в MSDN вторым аргументом CompareString() 
	DWORD locale;		// Локаль сравнения. Определены в MSDN первым аргументом CompareString()
};

/* Процедура обратного вызова для отработки сообщения LVM_SORTLIST (см. MSDN)*/
INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#endif // _FM_SORT_H_