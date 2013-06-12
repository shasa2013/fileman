/** Часть кода Fileman.
Горшков Александр Сергеевич, 2009 г, Таганрог.
Константы  */

#pragma once

#define FM_DIRTYPE_STUB				L"[DIR]"				/*< Заглушка для типа элемента "папка"		*/

#define FM_CAPTION					TEXT("Fileman")				/*< Заголовок программы						*/
#define FM_COPY_DLG_CAPTION			TEXT("Копирование")			/*< Заголовок диалога копирования			*/
#define FM_MOVE_DLG_CAPTION			TEXT("Перемещение")			/*< Заголовок диалога перемещения			*/
#define FM_UNCKNOWN_DLG_CAPTION		TEXT("Неизвестная операция")	/*< Заголовок диалога Неизвестной операции	*/

//#define FM_PN_


#define FM_ITEM_COUNTING	1					/*< Размер папки вычисляется		*/
#define FM_ITEM_COUNTED		2					/*< Размер папки вычислен			*/


/** Нумирация колонок */
#define FM_COLUMN_NAME		0					/*< Название файла без расширения	*/
#define FM_COLUMN_TYPE		1					/*< Тип файла по расширению			*/
#define FM_COLUMN_DATE		2					/*< Дата файла						*/
#define FM_COLUMN_SIZE		3					/*< Размер файла					*/
#define FM_COLUMN_COUNT		4					/*< Количество колонок				*/


/** Значения возвращаемые процедурой appendCurrentPath () */
#define FM_ACP_NOTDIR		0					/*< Элемент не папка				*/
#define FM_ACP_COUNT		3					/*< Идёт подсчёт размера папки		*/
#define FM_ACP_NOSEL		4					/*< Элемент не выбран				*/
#define FM_ACP_ERR			(0xffffffff)		/*< Возникла ошибка					*/


/** Значения возвращаемые процедурой addFileToListView () */
#define FM_AFTLV_OK			0					/*< Файл/дирректория добавлен в панель		*/
#define FM_AFTLV_NO			1					/*< Файл/дирректория не добавлен в панель	*/
#define FM_AFTLV_ERR		(0xffffffff)		/*< Возникла ошибка							*/


/** Константы используемые в процедуре recurseOperation(...)  */

/** Коды операций */
#define FM_OPERATION_COPY			1			/*< Операция копирования	*/
#define FM_OPERATION_DELETE			2			/*< Операция удаления		*/
#define FM_OPERATION_MOVE			3			/*< Операция перемещения	*/
#define FM_OPERATION_COUNT			4			/*< Операция подсчёта		*/
#define FM_OPERATION_SERCH			5			/*< Операция поиска			*/

/** Коды сообщений  */
#define	FM_COPY_FINISHED		(WM_USER + 1)	/*< Операция копирования завершена	*/
#define	FM_DELETE_FINISHED		(WM_USER + 2)	/*< Операция удаления завершена		*/
#define	FM_MOVE_FINISHED		(WM_USER + 3)	/*< Операция перемещения завершена	*/
#define FM_COUNT_FINISHED		(WM_USER + 4)	/*< Операция подсчёта завершена		*/
#define FM_COUNT_PROGRESS		(WM_USER + 5)	/*< Операция подсчёта продолжается	*/
#define FM_LIST_FINISHED		(WM_USER + 6)	/*< Операция листания завершена. WPARAM - не используется. LPARAM - не используется */
#define FM_LIST_BEGIN		    (WM_USER + 7)	/*< Операция листания начата		*/
#define WM_USER_MEDIACHANGED	(WM_USER + 88)	/*< */

#define FM_MAX_PATH					32767		/*< Максимальный размер в байтах полного имени файла в UNICODE-строке согласно MSDN */
#define FM_MAX_INDENT				7708		/*< Максимальнная глубина вложенности папок */

#define FM_COUNT_UPDATE_RATE		500			/*< Время обновления размера папки в мс			*/
#define FM_UPDATE_TIMER				133			/*< Период срабатывания таймера обновления, мс	*/
#define FM_DIRCHANGE_TIMER			550

/** Коды интерфейсов */
#define FM_IC_FIRST				1				/*< */
#define	FM_IC_UNCKNOWN			1				/*< Неизвестен					*/
#define	FM_IC_DISK				2				/*< Жёсткий диск				*/
#define	FM_IC_CDA				3				/*< Audio CD					*/
#define	FM_IC_NET				4				/*< Сеть Microsft Windows		*/
#define	FM_IC_NFS				5				/*< Сеть NFS Linux				*/
#define	FM_IC_FTP				6				/*< FTP							*/
#define	FM_IC_SHELL				7				/*< Объекты оболочки			*/
#define FM_IC_LAST				7				/*< */
#define	FM_IC_ANY				(0xffffffff)	/*< Любой тип					*/

/** Минимальные ширины колонок  */
static int minColumnWidths [FM_COLUMN_COUNT] = {100, 40, 100, 50};


volatile static ULONG m_ulSHChangeNotifyRegister = NULL;
