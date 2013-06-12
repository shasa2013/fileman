// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог
// Заголовки процедур для листания папок

#ifndef _BROWSE_H_
#define _BROWSE_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "fmmemory.h"
#include "user.h"
#include "resource.h"
#include "ltpd.h"
#include "consts.h"
#include "operation.h"
#include "abstract.h"
#include "watcher.h"


/** Данные для сортировки строк в ListView */
struct FmSort
{
	DWORD column;	/*< Индекс колонки по которой сортируются строки							*/
	BOOL dir;		/*< Напрвление сортировки: TRUE - по возрастанию, FALSE - по убыванию		*/
	BOOL separate;	/*< Сортировка директорий и файлов: TRUE - отдельно, FALSE - вместе			*/
	HWND hListView;	/*< Дескриптор ListView														*/
	DWORD flags;	/*< Флаги сравнения. Определены в MSDN вторым аргументом CompareString()		*/ 
	DWORD locale;	/*< Локаль сравнения. Определены в MSDN первым аргументом CompareString()	*/
};



/** Процедура обратного вызова для отработки сообщения LVM_SORTLIST (см. MSDN) */
INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


/** Возвращает путь выбранного диска
HWND hDlg - дескриптор главного окна.
UINT itemId - идентификатор ComboBox со списком дисков.
TCHAR* path - путь выбранного диска. */
BOOL getPathOfSelectedDisk (HWND hDlg, UINT itemId, TCHAR* path);


/** Вызывается при выборе диска из списка */
INT_PTR onDiskList (HWND hDlg, WPARAM wParam, LPARAM lParam);


///** Добавляет выделенную папку в понели к текущему пути
//HWND hDlg - дескриптор диалога
//UINT fullPathId - идентификатор полного пути
//UINT panelId - идентификатор панели */
//DWORD appendCurrentPath	(HWND hLv);


/** Заполняет список дисков
HWND hDlg - дескриптор главного окна
UINT itemId - идентификатор элемента */
VOID fillDiskList (HWND hDlg, UINT itemId);


/** Вызывается при нажатии на заголовок ListView
HWND hDlg - дескриптор диалога
WPARAM wParam - дескриптор ListView
LPARAM lParam - указатель на структуру NMLISTVIEW */
BOOL onColumnClick (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** Выводит в ListView список фалов и папок в директории
HWND hLv - дескриптор ListView */
BOOL listDirectoryToPanel (HWND hLv);


/** Процедура потока. Заполняет List View элементами директории */
DWORD WINAPI listDirectoryToPanelWorker (VOID* data);


/** Добавляет файл в панель
HWND hDlg - дескриптор главного диалога
TCHAR* path - папка в которой находится файл
WIN32_FIND_DATA* findData - содержит информацию о добавляемом файле */
DWORD addFileToListView (HWND hLv, TCHAR* path, WIN32_FIND_DATA* findData);


/** Заполняет иконками 
HWND hLv - дескриптор панели
TCHAR* path - путь к папке */
VOID addFilesIconsToListView (HINSTANCE hInst, HWND hLv, TCHAR* path);


/** Вызывается после завершения операции "Листание директории"
HWND hDlg - дескриптор главного диалога */
INT_PTR onListFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** Передаёт оболочке выбранный файл или папку на выполнение
HWND hLv - дескриптор панель */
BOOL executeSelected (HWND hLv);


/** Устанавливает текущий путь
HWND hDlg - дескриптор диалога */
//VOID setCurrentPath	(HWND hDlg, UINT diskListId, UINT fullPathId, UINT panelId);


#endif // _BROWSE_H_