// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.

#ifndef _TASK_H_
#define _TASK_H_

#include "build.h"

#include <commctrl.h>
#include <vector>
#include "helper.h"
#include "resource.h"
#include "browse.h"
#include "user.h"
#include "progress.h"

#define TPM_DEFAULT		1						/*< Режим списка файлов, при котором путь файлов источников формируется из пути папки источника, и имён фалов */
#define TPM_DROP		2						/*< Режим списка файлов, при котором список файлов источников содержит полные пути файлов */

/** Класс для хранения задания Копирование/Удаление/Перемещение */
class Task
{
	std::vector <TCHAR*> fileNames;				/*< Список имён в папке источнике						*/
	TCHAR* dstFolder;							/*< Папка назначение									*/
	TCHAR* srcFolder;							/*< Папка источник										*/
	DWORD updateRate;							/*< Скорость обновления данных о прогрессе копирования	*/
	DWORD index;								/*< Индекс текущего имени в списке fileNames			*/
	TCHAR* curSrcPath;							/*< Текущий полный путь источник						*/
	TCHAR* curDstPath;							/*< Текущий полный путь назначение						*/
	Progress progress;							/*< Данные прогресса копирования						*/
	DWORD pathMode;								/*< Режим списка файлов источников						*/
	DWORD flags;								/*< Флаги специфичые для операции						*/
	DWORD opCode;								/*< Код операции										*/
	
public:
	Task (DWORD pathMode = TPM_DEFAULT);		/*< Конструктор											*/
	VOID setDstFolder (TCHAR* path);			/*< Устанавливает путь папки назначения					*/
	VOID setSrcFolder (TCHAR* path);			/*< Устанавливает путь папки источника					*/
	TCHAR* getCurSrcPath ();					/*< Возвращает следующий полный путь файла источника	*/
	TCHAR* getCurDstPath (VOID);				/*< Возвращает следующий полный путь файла назначения	*/
	TCHAR* getDstFolder (VOID);					/*< Возвращает путь папки назначения					*/
	TCHAR* getSrcFolder (VOID);
	DWORD getFileNamesCount (VOID);				/*< Возвращает количество имён файлов					*/
	Progress* getProgress (VOID);				/*< Возвращает указатель на данные прогресса операции	*/
	VOID addFileName (TCHAR* path);				/*< Добавляет имя файла									*/
	VOID clear (VOID);							/*< Очищает задание										*/
	BOOL nextPath (VOID);						/*< Выставляет следующие пути							*/
	VOID resetPath (VOID);						/*< Сбрасывает текущие пути								*/
	INT getOpCode (VOID);						/*< Возвращает код операции								*/
	VOID setOpCode (INT opCode);				/*< Устанавливает код операции							*/
	INT getFlags (VOID);						/*< Возвращает флаги операции							*/
	VOID setFlags (INT);						/*< Устанавливает флаги операции						*/
	~ Task ();									/*< Деструктор											*/
};


#endif // _TASK_H_