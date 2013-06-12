// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.

#ifndef _USER_H_
#define _USER_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>

#include "consts.h"

/** Свойства панели */
struct FmPanelProps
{
	COLORREF bkColors [2];		/*< Цвета фона строки, для чётных и нечётных строк				*/
	COLORREF customColors [16];	/*< Массив своих цветов в диалоге выбора цвета					*/
	FLOAT colsRelWidths [4];	/*< Относительные ширины колонок								*/
	int sortColumnIndex;		/*< Индекс колонки, по которой выполнена сортировка				*/
	int steckSels [FM_MAX_INDENT];/*< Стек индексов строк панели, в которые был произведён вход	*/
	int steckInd;				/*< Указатель стека индексов строк панели						*/
	BOOL sortDir;				/*< Направление сортировки										*/
	int countedItem;
    WNDPROC defProc;			/*< Указатель на исходную процедуру окна						*/
	HWND hFullPath;				/*< Дескриптор Combo-box с полным путём							*/
	HWND hSelSize;				/*< Дескриптор Static с размером выделенных файлов				*/
	HWND hDlg;					/*< Дескриптор главного диалогового окна						*/
	TCHAR* curDir;	/*< */
};


/** Сохраняет свойства элемента диалога. Эвристическая процедура.
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор элемента
TCHAR* fileName - имя файла */
BOOL saveDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName);

/** Загружает свойства элемента диалога. Эвристическая процедура.
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор элемента
TCHAR* fileName - имя файла */
BOOL loadDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName);

/** Возвращает строку с полным путём к ini файлу программы
TCHAR* modulePath - путём к ini файлу программы.
DWORD size - размер буфера, на который указывает modulePath в байтах */
BOOL getIniFileName (TCHAR* modulePath, DWORD size);

/** Устанавливает ширины колонок ListVeiw
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор ListVeiw
DWORD count - количество колонок */
BOOL setListVeiwColumnsWidth (HWND hLv, DWORD count, DWORD colWidths []);

/** Возвращает ширины колонок
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор ListVeiw
DWORD count - количество колонок
DWORD colWidths [] - ширины колонок */
BOOL getListVeiwColumnsWidth (HWND hDlg,  UINT itemId, DWORD count, DWORD colWidths []);

/** Возвращает ширины колонок
HWND hDlg - дескриптор ListVeiw
DWORD count - количество колонок
DWORD colWidths [] - ширины колонок */
BOOL getListVeiwColumnsWidth (HWND hLv,  DWORD count, DWORD colWidths []);

/** Возвращает положение элемента диалога
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор элемента
RECT* pRect - прямоугольник, занимаемый элементом */
HWND getDlgItemPos (HWND hDlg, UINT itemId, RECT* pRect);

/** Инициализирует колонки
hDlg - дескриптор диалога
DWORD itemId - идентификатор List View
DWORD columns - количество колонок
TCHAR* headers [] - заголовки колонок */
BOOL initListViewColumns (HWND hDlg, UINT itemId, DWORD columns, TCHAR* headers [], DWORD colWidths []);



#endif // _USER_H_

