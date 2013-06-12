#include "helper.h"

int multiByteToWideChar (TCHAR* wcStr, char* mbStr, UINT codePage)
{
	int len = lstrlenA (mbStr) + sizeof ('\0');
	return MultiByteToWideChar (codePage, MB_PRECOMPOSED, mbStr, len, wcStr, len);
}

DWORD normalizePath (TCHAR* path)
{
	
	if ((path + lstrlen(path) - 1) == lstrrchr (path, L'\\')) 
	{
		*lstrrchr (path, L'\\') = L'\0';
	}
	
	DWORD fileAttr = GetFileAttributes(path);
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
	{
		TCHAR* tmpChar = lstrrchr (path, L'\\');
		if (tmpChar != NULL) *tmpChar = L'\0';
		return 0;
	}
	
	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		lstrcat (path, L"\\");
	}
	return fileAttr;
}


VOID showErrorMessage (HWND hDlg, DWORD err, TCHAR* caption)
{
	HLOCAL hlocal = NULL;
	DWORD iconFlag = 0;
	if (err == 0)
	{
		iconFlag = MB_ICONINFORMATION;
	}
	else
	{
		iconFlag = MB_ICONERROR;
	}
	FormatMessage  (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
					NULL,
					err,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					(LPTSTR) &hlocal,
					0,
					NULL);
	if (hlocal != NULL)
	{
		MessageBox (hDlg, (TCHAR*)LocalLock(hlocal), caption, MB_OK | iconFlag);
		LocalFree (hlocal);
	}
	else
	{
		MessageBox (hDlg, L"Произошла не известная ошибка", caption, MB_OK | MB_ICONERROR);
	}
}


VOID wprintfd (TCHAR* format, ...)
{
	DWORD er = GetLastError();
	TCHAR* buf = (TCHAR*) LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, FM_MAX_PATH);
	va_list args;
	va_start (args, format);
	_vsnwprintf (buf, LocalSize ((HLOCAL)buf), format, args);
	va_end (args);
	TCHAR* str = (TCHAR*) LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, 32);
	wsprintf (str, L" [%u]\n", er);
	if (er != 0)
	{
		HLOCAL hlocal = NULL;
		FormatMessage  (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
					NULL,
					er,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					(LPTSTR) &hlocal,
					0,
					NULL);
		if (hlocal != NULL)
		{
			SetLastError (0);
			OutputDebugString ((TCHAR*)hlocal);
			LocalFree (hlocal);
		}
		else
		{
			OutputDebugString (L"Не известная ошибка");
		}
	}
	lstrcat (buf, str);
	lstrcat (buf, L"\n");
	OutputDebugString (buf);
	LocalFree(str);
	LocalFree(buf);
	SetLastError (NO_ERROR);
}


VOID printfd (char* format, ...)
{
	char* buf = (char*) LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, FM_MAX_PATH);
	va_list args;
	va_start (args, format);
	_vsnprintf (buf, LocalSize ((HLOCAL)buf), format, args);
	va_end (args);
	char* str = (char*) LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, 32);
	DWORD er = GetLastError();
	
	sprintf (str, " [%d]\n", er);
	if (er != 0)
	{
		HLOCAL hlocal = NULL;
		FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
					NULL,
					er,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					(char*) &hlocal,
					0,
					NULL);
		if (hlocal != NULL)
		{
			SetLastError (0);
			OutputDebugStringA ((char*)hlocal);
			LocalFree (hlocal);
		}
		else
		{
			OutputDebugStringA ("Не известная ошибка");
		}
	}
	strcat (buf, str);
	strcat (buf, "\n");
	OutputDebugStringA (buf);
	LocalFree (str);
	LocalFree (buf);
	SetLastError (NO_ERROR);
}



TCHAR* lstrrchr (TCHAR* str, TCHAR rchar)
{
	wchar_t* end = NULL;
	if (str)
	{
		if (*str)
		{
			end = str + lstrlen (str);
			while (*end != rchar)
			{
				if (end == str)
				{
					end = NULL;
					break;
				}
				-- end;
			}
		}
	}
	return end;
}


TCHAR* lcutrchr (TCHAR* str, TCHAR ch)
{
	TCHAR* cutCh = lstrrchr (str, ch);
	if (cutCh != NULL) *cutCh = L'\0';
	return cutCh;
}



BOOL loadStringFromResource (HWND hDlg, UINT strId, TCHAR* destStr, SIZE_T strLen)
{
	LONG hInst = GetWindowLong(hDlg, GWL_HINSTANCE);
	BOOL rc = TRUE;
	if (LoadString((HINSTANCE)hInst, strId, destStr, strLen) == 0)
	{
		wprintfd (L"loadStringFromResource(): Невозможно загрузить ресурс id %u", strId); 
		rc = FALSE;
		lstrcpy (destStr, L"");
	}
	return rc;
}


HLOCAL fmLocalFree (HLOCAL hMem)
{
	if (hMem == NULL)
	{
		return NULL;
	}
	return LocalFree (hMem);
}



HLOCAL fmLocalAlloc (SIZE_T len)
{
	return LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, len);
}

SIZE_T fmLocalSize (HLOCAL hMem)
{
	return LocalSize (hMem);
}


BOOL getFileNameFromHandle (HANDLE hFile, TCHAR* pszFilename, SIZE_T fileNameLen) 
{
	pszFilename [0] = L'\0';

	// Создать объект отображения файла
	HANDLE hFileMap = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 1, NULL);
	if (hFileMap == NULL) return FALSE; 
	
	// Создать отображение файла для получения имени файла
	void* pMem = MapViewOfFile (hFileMap, FILE_MAP_READ, 0, 0, 1);
	if (pMem == NULL)
	{
		CloseHandle (hFileMap);
		return FALSE;
	}
	
	if (GetMappedFileName (GetCurrentProcess(), pMem, pszFilename, fileNameLen) == NULL) return FALSE;
	// Translate path with device name to drive letters.
	TCHAR szTemp[MAX_PATH];
	szTemp[0] = L'\0';
	if (GetLogicalDriveStrings(MAX_PATH, szTemp) == NULL)
	{
		UnmapViewOfFile (pMem);
		CloseHandle (hFileMap);
		return FALSE;
	}
	TCHAR szName[MAX_PATH];
	TCHAR szDrive[3] = L" :";
	BOOL bFound = FALSE;
	TCHAR* p = szTemp;

	do 
	{
		// Copy the drive letter to the template string
		*szDrive = *p;
		// Look up each device name
		if (QueryDosDevice(szDrive, szName, sizeof(szName)))
		{
			UINT uNameLen = lstrlen (szName);
			if (uNameLen < MAX_PATH) 
			{
				bFound = _wcsnicmp (pszFilename, szName, uNameLen) == 0;
				if (bFound) 
				{
					wsprintf (szTemp, L"%s%s", szDrive, pszFilename + uNameLen);
					lstrcpy (pszFilename, szTemp);
				}
			}
		}

		// Go to the next NULL character.
		while (*p++);
	} while (!bFound && *p); // end of string

	UnmapViewOfFile (pMem);
	CloseHandle (hFileMap);
	return TRUE;
}



BOOL addSlashToPath (TCHAR* path)
{
	TCHAR* tc = lstrrchr (path, L'*');
	if (tc) *tc = L'\0';
	tc = lstrrchr (path, L'\\');
	TCHAR* lc = path + lstrlen (path) - 1;
	if (lc != tc)
	{
		lstrcat (path, L"\\");
		return TRUE;
	}
	return FALSE;
}


BOOL delSlashFromPath (TCHAR* path)
{
	TCHAR* tc = lstrrchr (path, L'\\');
	if (tc)
	{
		*tc = L'\0';
		return TRUE;
	}
	return FALSE;
}


BOOL cutPath (TCHAR* path)
{
	TCHAR* tchar = lstrrchr (path, L'*');
	if (tchar) *tchar = '\0';
	
	tchar = lstrrchr (path, L'\\');
	TCHAR* last = path + lstrlen (path) - 1;
	if (last == tchar) *tchar = L'\0';
		
	tchar = lstrrchr (path, L'\\');
	if (tchar) *tchar = L'\0';
	
	return tchar != NULL;
}

DWORD addPath (TCHAR* curPath, TCHAR* addPath)
{
	// Если доб. путь текущий - ничего не делаем
	if (lstrcmp (addPath, L".") == NULL) return FM_ACP_CURDIR;
	
	// Если доб. путь уровень выше - обрезаем
	if (lstrcmp (addPath, L"..") == NULL)
	{
		cutPath (curPath);
		return FM_ACP_DIR_UP;
	}

	// Если это есть имя, то - добавляем его
	// Есть ли в конце символ "\\"? ...
	TCHAR* last1 = lstrrchr (curPath, L'\\');
	TCHAR* last2 = lstrlen (curPath) + curPath - 1;
	// ... нет - дописываем
	if (last1 != last2) lstrcat (curPath, L"\\");
	// ... здесь в любом случае есть, дописываем доб. путь
	lstrcat (curPath, addPath);

	return FM_ACP_DIR_DAWN;
}



DWORD getHash (TCHAR *str)
{
	DWORD hash = 5381;
	DWORD c;
	do
	{
		c = *str++;
		hash = ((hash << 5) + hash) + c;
	}
	while (c);
	
	return hash;
}


BOOL fmGetDiskFreeSpace (TCHAR* pszDrive, ULARGE_INTEGER* i64FreeBytes)
{
	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	return GetDiskFreeSpaceEx
		(pszDrive, &i64FreeBytesToCaller, &i64TotalBytes, i64FreeBytes);
}
