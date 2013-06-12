// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог
// Абстракция процедур доступа к объектам

#include "audio.h"
#include "helper.h"




/** Структура с указателями процедур перечислений */
struct Find
{
	HANDLE first (TCHAR* path, WIN32_FIND_DATA* pFindData);
	BOOL (*next) (HANDLE hObj, WIN32_FIND_DATA* pFindData);
	BOOL (*close) (HANDLE hObj);
};

/** Структура с указателями процедур операций */
struct Operation
{
	BOOL (*del) (TCHAR* srcPath, TCHAR* dstPath);
	BOOL (*copy) (TCHAR* srcPath, TCHAR* dstPath);
	BOOL (*move) (TCHAR* srcPath, TCHAR* dstPath);
};

/** Актуатор */
DWORD getInterfaceCode (TCHAR* path);

/** Перечислители Жёсткий диск */
HANDLE diskFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL diskFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL diskFindClose (HANDLE hObj);

/** Перечислители Audio CD */
HANDLE cdaFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL cdaFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL cdaFindClose (HANDLE hObj);

/** Перечислители Сеть Windows */
HANDLE netFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL netFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL netFindClose (HANDLE hObj);

/** Перечислители Сеть NFS */
HANDLE nfsFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL nfsFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL nfsFindClose (HANDLE hObj);

/** Перечислители FTP */
HANDLE ftpFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL ftpFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL ftpFindClose (HANDLE hObj);

/** Перечислители Shell */
HANDLE shellFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData);
BOOL shellFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData);
BOOL shellFindClose (HANDLE hObj);

/** Операции Жёсткий диск */
BOOL diskCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL diskDel (TCHAR* path);
BOOL diskMove (TCHAR* srcPath, TCHAR* dstPath);

/** Операции Audio CD */
BOOL cdaCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL cdaDel (TCHAR* path);
BOOL cdaMove (TCHAR* srcPath, TCHAR* dstPath);

/** Операции Сеть Windows */
BOOL netCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL netDel (TCHAR* path);
BOOL netMove (TCHAR* srcPath, TCHAR* dstPath);

/** Операции Сеть NFS */
BOOL nfsCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL nfsDel (TCHAR* path);
BOOL nfsMove (TCHAR* srcPath, TCHAR* dstPath);

/** Операции FTP */
BOOL ftpCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL ftpDel (TCHAR* path);
BOOL ftpMove (TCHAR* srcPath, TCHAR* dstPath);

/** Операции Shell */
BOOL shellCopy (TCHAR* srcPath, TCHAR* dstPath);
BOOL shellDel (TCHAR* path);
BOOL shellMove (TCHAR* srcPath, TCHAR* dstPath);
