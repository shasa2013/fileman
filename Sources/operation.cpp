#include "operation.h"



BOOL setOperationThread (HWND hDlg, HANDLE hThread, BOOL isSuspended)
{
	if (!SetProp (hDlg, L"hThread", hThread)) return FALSE;
	if (!SetProp (hDlg, L"isSuspended", (HANDLE) isSuspended)) return FALSE;
	return TRUE;
}


BOOL removeOperationThread (HWND hDlg)
{
	BOOL retCode = FALSE;
	HANDLE hThread = RemoveProp (hDlg, L"hThread");
	if (hThread)
	{
		retCode = CloseHandle (hThread);
	}
	RemoveProp (hDlg, L"isSuspended");
	return retCode;
}


BOOL isOperationThreadSuspended (HWND hDlg)
{
	BOOL retCode = (BOOL) GetProp (hDlg, L"isSuspended");
	return retCode;
}


BOOL suspendOperationThread (HWND hDlg)
{
	HANDLE hThread = GetProp (hDlg, L"hThread");
	BOOL retCode = TRUE;
	if (hThread)
	{
		retCode = SuspendThread (hThread) != (DWORD) - 1;
		retCode &= SetProp (hDlg, L"isSuspended", (HANDLE) TRUE);
	}
	return retCode;
}



BOOL resumeOperationThread (HWND hDlg)
{
	HANDLE hThread = GetProp (hDlg, L"hThread");
	BOOL retCode = TRUE;
	if (hThread)
	{
		retCode = ResumeThread (hThread) != (DWORD) - 1;
		retCode &= SetProp (hDlg, L"isSuspended", (HANDLE) FALSE);
	}
	return retCode;
}


BOOL isOperationThreadExist (HWND hDlg)
{
	return GetProp (hDlg, L"hThread") != NULL;
}


INT recurseOperation
	(WIN32_FIND_DATA* findData, TCHAR* beginFolder, TCHAR* dstFolder,
	LPPROGRESS_ROUTINE routine, VOID* arg, DWORD flags, DWORD operation, UINT64* bytesCount)
{
	static Progress* progr = (Progress*) arg;
	static LARGE_INTEGER stub = {0};
	static DWORD rc = 0;
	static BOOL result = TRUE;
	static WIN32_FILE_ATTRIBUTE_DATA fileAttrs;
	++ progr->indent;
	if (progr->maxIndent < progr->indent)
	{
		progr->maxIndent = progr->indent;
	}
	if (dstFolder != NULL)
	{
		delSlashFromPath (dstFolder);
		lstrcat (dstFolder, lstrrchr (beginFolder, L'\\'));
	}
	if (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes (beginFolder))
	{
		if ((operation == FM_OPERATION_COPY) || (operation == FM_OPERATION_MOVE))
		{
			if (dstFolder)
			if (!CreateDirectory (dstFolder, NULL))
			{
				rc = GetLastError();
				wprintfd (L"Директория не создана:\n\t\"%s\"\n", dstFolder);
				printdErrorStr (rc);
				return progr->indent;
			}
			else
			{
				wprintfd (L"\nrecurseOperation:dstFolder:NULL - ошибка\n", dstFolder);
			}
		}

		TCHAR* tchar = lstrrchr (beginFolder, L'*');
		if (tchar == NULL) lstrcat (beginFolder, L"\\*");
		
		HANDLE hFile = FindFirstFile (beginFolder, findData);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			rc = GetLastError ();
			if (rc != 0)
			{
				wprintfd (L"Ошибка %u FindFirstFile(%s), вложенность %u\n", rc, beginFolder, progr->indent);
				printdErrorStr (rc);
				return progr->indent;
			}
		}
		BOOL next = TRUE;
		while (next)
		{
			if ((lstrcmp (findData->cFileName, L".") == 0)
				|| (lstrcmp (findData->cFileName, L"..") == 0))
			{
				rc = 0;
				next = FindNextFile (hFile, findData);
				if (next == FALSE)
				{
					rc = GetLastError();
				}
				switch (rc)
				{
					case ERROR_NO_MORE_FILES:
						printdErrorStr (rc);
					case ERROR_SHARING_VIOLATION:
					case ERROR_NOT_ENOUGH_MEMORY:
					case ERROR_ACCESS_DENIED:
						SetLastError (NO_ERROR);
					case NO_ERROR:
						continue;
					default:
						showFormatMessage (progr->hDlg, MB_OK, L"Ошибка %u FindNextFile\nвложенность %u, hFile %08x\n %s", rc, progr->indent, hFile);
						wprintfd (L"Ошибка %u при FindNextFile, вложенность %u\n", rc, progr->indent);
						printdErrorStr (rc);
						FindClose (hFile);
						return progr->indent;
				}
			}
			
			addSlashToPath (beginFolder);
			lstrcat (beginFolder, findData->cFileName);
		
			if (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((operation == FM_OPERATION_COPY) || (operation == FM_OPERATION_MOVE))
				{
						addSlashToPath (dstFolder);
						lstrcat (dstFolder, findData->cFileName);
				}
				recurseOperation (findData, beginFolder, dstFolder, routine, arg, flags, operation, bytesCount);
			}
			else
			{
				switch (operation)
				{
					case FM_OPERATION_COPY:
					{
						addPath (dstFolder, findData->cFileName);
						wprintfd (L"Копирую файл:\n\tиз \"%s\"\n\tв \"%s\"\n", beginFolder, dstFolder);
						result = CopyFileEx (beginFolder, dstFolder, routine, arg, FALSE, flags);
						if (!result)
						{
							rc = GetLastError ();
							wprintfd (L"Файл не cкопирован (ошибка %u):\n\t\"%s\"\n", rc, beginFolder);
							printdErrorStr (rc);
						}
						cutPath (dstFolder);
						break;
					}
					case FM_OPERATION_DELETE:
					{
						result = GetFileAttributesEx (beginFolder, GetFileExInfoStandard, (VOID*)&fileAttrs);
						if (result)
						{
							SetFileAttributes (beginFolder, fileAttrs.dwFileAttributes & (~FILE_ATTRIBUTE_READONLY));
						}
						else
						{
							rc = GetLastError();
							wprintfd (L"Не удаётся получить атрибуты файла (ошибка %u):\n\t\"%s\"\n", beginFolder);
							printdErrorStr (rc);
						}
						wprintfd (L"Удаляю файл (1):\n\t\"%s\"\n", beginFolder);
						result = DeleteFile (beginFolder);
						if (!result)
						{
							rc = GetLastError ();
							wprintfd(L"Файл не удалён (ошибка %u):\n\t\"%s\"\n", beginFolder, rc);
							printdErrorStr (rc);
						}
						routine (stub, stub, stub, stub, 0, progr->updateRate, 0, (HANDLE)progr->hDlg, progr);
						break;
					}
					case FM_OPERATION_MOVE:
					{
						addPath (dstFolder, findData->cFileName);
						result = GetFileAttributesEx (beginFolder, GetFileExInfoStandard, (VOID*)&fileAttrs);
						if (result)
						{
							SetFileAttributes (beginFolder, fileAttrs.dwFileAttributes & (~FILE_ATTRIBUTE_READONLY));
						}
						wprintfd (L"Перемещаю файл:\tиз \"%s\"\n\tв \"%s\"\n", beginFolder, dstFolder);
						result = MoveFileWithProgress (beginFolder, dstFolder, routine, arg, flags);
						if (!result)
						{
							rc = GetLastError ();
							wprintfd (L"Файл не перемещён:\n\t\"%s\"\n", beginFolder);
							printdErrorStr (rc);
						}
						cutPath (dstFolder);
						break;
					}
					case FM_OPERATION_COUNT:
					{
						*bytesCount += (DWORDLONG(findData->nFileSizeHigh) << 32) | findData->nFileSizeLow;
						SendMessage (progr->hDlg, FM_COUNT_PROGRESS, progr->itemId, (LPARAM)bytesCount);
						break;
					}
				}
				cutPath (beginFolder);
			}
			rc = NO_ERROR;
			next = FindNextFile (hFile, findData);
			if (next == FALSE)
			{
				rc = GetLastError ();
			}
			switch (rc)
			{
				case ERROR_NO_MORE_FILES:
					printdErrorStr (rc);
				case ERROR_SHARING_VIOLATION:
				case ERROR_NOT_ENOUGH_MEMORY:
				case ERROR_ACCESS_DENIED:
					SetLastError (NO_ERROR);
				case NO_ERROR:
					continue;
				default:
					showFormatMessage (progr->hDlg, MB_OK, L"Ошибка %u FindNextFile\nвложенность %u, hFile %08x\n %s", rc, progr->indent, hFile);
					wprintfd (L"Ошибка %u FindNextFile, вложенность %u\n", rc, progr->indent);
					printdErrorStr (rc);
					FindClose (hFile);
					return progr->indent;
			}
		} // <-- while (next)
		FindClose (hFile);
		if (operation == FM_OPERATION_DELETE)
		{
			lcutrchr (beginFolder, L'*');
			BOOL result = GetFileAttributesEx (beginFolder, GetFileExInfoStandard, (VOID*)&fileAttrs);
			if (result)
			{
				SetFileAttributes (beginFolder, fileAttrs.dwFileAttributes & (~FILE_ATTRIBUTE_READONLY));
			}
			wprintfd (L"Удаляю папку:\n\t\"%s\"\n", beginFolder);
			result = RemoveDirectory (beginFolder);
			if (!result)
			{
				rc = GetLastError ();
				wprintfd (L"Папка не удалена:\n\t\"%s\"\n", beginFolder);
				printdErrorStr (rc);
			}
			routine (stub, stub, stub, stub, 0, progr->updateRate, 0, progr->hDlg, progr);
		}
		else
		{
			cutPath (dstFolder);
		}
		cutPath (beginFolder);
	} // <-- if (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes (beginFolder))
	else 
	{
		switch (operation)
		{
			case FM_OPERATION_COPY:
			{
				wprintfd (L"Копирую файл:\n\tиз \"%s\"\n\tв \"%s\"\n", beginFolder, dstFolder);
				result = CopyFileEx (beginFolder, dstFolder, routine, arg, FALSE, flags);
				if (!result)
				{
					rc = GetLastError ();
					wprintfd (L"Файл не копируется: \"%s\"\n", beginFolder);
					printErrorStr (rc);
				}
				break;
			}
			
			case FM_OPERATION_DELETE:
			{
				result = GetFileAttributesEx (beginFolder, GetFileExInfoStandard, (VOID*)&fileAttrs);
				if (result)
				{
					SetFileAttributes (beginFolder, fileAttrs.dwFileAttributes & (~FILE_ATTRIBUTE_READONLY));
				}
				else
				{
					rc = GetLastError ();
					wprintfd (L"Не удётся получить атрибуты файла:\n\t%s\n", beginFolder);
					printErrorStr (rc);
				}
				wprintfd (L"Удаляю файл (2):\n\t\"%s\"\n", beginFolder);
				result = DeleteFile (beginFolder);
				if (!result)
				{	
					rc = GetLastError ();
					wprintfd (L"Файл не удалён: \"%s\"\n", beginFolder);
					printdErrorStr (rc);
				}
				routine (stub, stub, stub, stub, 0, progr->updateRate, 0, progr->hDlg, progr);
				cutPath (beginFolder);
				break;
			}
			case FM_OPERATION_MOVE:
			{
				result = GetFileAttributesEx (beginFolder, GetFileExInfoStandard, (VOID*)&fileAttrs);
				if (result)
				{
					SetFileAttributes (beginFolder, fileAttrs.dwFileAttributes & (~FILE_ATTRIBUTE_READONLY));
				}
				wprintfd (L"Перемещаю файл:\n\tиз \"%s\"\n\tв \"%s\"\n", beginFolder, dstFolder);
				result = MoveFileWithProgress (beginFolder, dstFolder, routine, arg, flags);
				if (!result)
				{
					rc = GetLastError ();
					wprintfd (L"Файл не перемещён: \"%s\"\n", beginFolder);
					printdErrorStr (rc);
				}
				break;				
			}
			case FM_OPERATION_COUNT:
			{
				HANDLE hFile = CreateFile (beginFolder, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				GetFileSizeEx (hFile, (PLARGE_INTEGER) bytesCount);
				SendMessage (progr->hDlg, FM_COUNT_PROGRESS, progr->itemId, (LPARAM)bytesCount);
				CloseHandle (hFile);
				break;
			}
		} // <-- switch (operation)
		cutPath (beginFolder);
	}
	return  -- progr->indent;
}
