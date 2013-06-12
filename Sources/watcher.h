// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.

#ifndef _watcher_h_
#define _watcher_h_

#include "build.h"

#include "helper.h"
#include "browse.h"


/** Следит за изменениями в папке, вызывая процедуры
пользователя при возникновении соответствующего изменения */
class Watcher
{	
	HANDLE hWorker;									/*< Дескриптор рабочего потока */
	HANDLE hStopWatch;								/*< Дескриптор события "Останов наблюдения"	*/
	BOOL (*dirChangeHandler) (HWND hLv);			/*< Указатель на процедуру обработки события "Содержимое папки изменилось" */
	static DWORD WINAPI watchWorker (VOID* arg);	/*< Процедура рабочего потока */
	LTPD ltpdL;										/*< Аргумент, передаваемый в обработчик события "Содержимое папки правой панели изменилось" */
	LTPD ltpdR;										/*< Аргумент, передаваемый в обработчик события "Содержимое папки левой панели изменилось" */
	DWORD waitTime;									/*< Продолжительность паузы после обработки события */
	static BOOL stubHandler (HWND hLv);				/*< Метод-заглушка, для неиспользованных событий */
	
public:
	
	// Устанавливает данные для листания в панель
	VOID setLtpdR (LTPD* ltpd);
	
	// Возвращает указатель на строку с путём к папке
	TCHAR* getFolderR (VOID);
	BOOL setFolderR (TCHAR* folder);

	// Устанавливает данные для листания в панель
	VOID setLtpdL (LTPD* ltpd);
	
	/** Возвращает указатель на строку с путём к папке */
	TCHAR* getFolderL (VOID);
	BOOL setFolderL (TCHAR* folder);

	/** Запускает наблюдение за папкой */ 
	BOOL start (VOID);

	/** Останавливает наблюдение за папкой, удаляя все дескрипторы.
	После вызова, возобновление наблюдения не возможно */
	BOOL stop (VOID);

	/** Переводит режим наблюдения в состояние "Пауза" или "Работа"
	BOOL isPaused - необходимое состояние наблюдения:
	TRUE переводит в сотояние "Пауза", FALSE переводит в сотояние "Работа" */
	BOOL setPause (BOOL isPaused);

	/** Устанавливает процедуру обработки события "Содержимое папки изменилось"
	BOOL (*dirChangeHandler) (VOID* arg) - указатель на процедуру
	VOID* arg - аргумент, передаваемый в процедуру */
	VOID setDirChangeHandler (BOOL (*dirChangeHandler) (HWND ltpd));
	
	/** Конструктор */
	Watcher (DWORD waitTime = 1000);
	
	/** Деструктор */
	~ Watcher (VOID);
};


#endif // _watcher_h_