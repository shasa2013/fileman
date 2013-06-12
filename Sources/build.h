// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог.
// Содержит определения используемые включаемыми файлами из Windows SDK.

#ifndef _FM_BUILD_H_
#define _FM_BUILD_H_
#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "Psapi.lib")
#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "helperW.lib")

#define ISOLATION_AWARE_ENABLED 1 // Защита от пикания в Vista (один из пунктов)



#define WINVER			(0x0501)
#define _WIN32_WINNT	(0x0501)
#define _WIN32_IE		(0x0600)


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE


#define UNICODE


#include <windows.h>

#include <helper.h>

/* --> Для совместимости с MSVC6 */
#ifndef SHGFI_ADDOVERLAYS
	#define SHGFI_ADDOVERLAYS      0x000000020     // apply the appropriate overlays
#endif

#ifndef LVM_SORTITEMSEX
	#define LVM_SORTITEMSEX        (LVM_FIRST + 81)
#endif

#ifndef CMF_EXTENDEDVERBS
	#define CMF_EXTENDEDVERBS      0x00000100      // rarely used verbs
#endif

#ifndef INVALID_FILE_ATTRIBUTES
	#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

WINBASEAPI BOOL WINAPI GetFileSizeEx (HANDLE hFile, PLARGE_INTEGER lpFileSize);
/* <-- Для совместимости с MSVC6 */



#endif // _FM_BUILD_H_