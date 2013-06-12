#include "watcher.h"


TCHAR* Watcher :: getFolderR()
{
	return this->ltpdR.path;
}

BOOL Watcher :: setFolderR (TCHAR* folder)
{
	lstrcpy (this->ltpdR.path, folder);
	return TRUE;
}



TCHAR* Watcher :: getFolderL()
{
	return this->ltpdL.path;
}

BOOL Watcher :: setFolderL (TCHAR* folder)
{
	lstrcpy (this->ltpdL.path, folder);
	return TRUE;
}


BOOL Watcher :: stubHandler (HWND arg)
{
	UNREFERENCED_PARAMETER (arg);
	return TRUE;
}


BOOL Watcher :: start (VOID)
{
	if (this->hWorker != INVALID_HANDLE_VALUE)
	{
		printfd ("Watcher(%08x)::start: рабочий поток (%08x) уже запущен\n", this, this->hWorker);
		return FALSE;
	}
	
	this->hStopWatch = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (this->hStopWatch == INVALID_HANDLE_VALUE)
	{
		DWORD rc = GetLastError ();
		printfd ("Watcher::start: ");
		printdErrorStr (rc);
		return FALSE;
	}
	
	this->hWorker = CreateThread (NULL, 0, this->watchWorker, this, 0, NULL);
	return this->hWorker != INVALID_HANDLE_VALUE;	
}


BOOL Watcher :: stop (VOID)
{
	BOOL result = SetEvent (this->hStopWatch);
	WaitForSingleObject (this->hWorker, INFINITE);
	
	result &=  CloseHandle (this->hStopWatch);
	this->hStopWatch = INVALID_HANDLE_VALUE;
	
	result &= CloseHandle (this->hWorker);
	this->hWorker = INVALID_HANDLE_VALUE;
	
	return result;
}


BOOL Watcher :: setPause (BOOL isPaused)
{
	DWORD result = 0;
	if (isPaused)
	{
		result = SuspendThread (this->hWorker);
	}
	else
	{
		result = ResumeThread (this->hWorker);
	}
	return (result != (DWORD) - 1);
}


VOID Watcher :: setDirChangeHandler (BOOL (*dirChangeHandler) (HWND ltpd))
{
	this->dirChangeHandler = dirChangeHandler;
}


VOID Watcher :: setLtpdR (LTPD* ltpd)
{
	this->ltpdR.hDlg = ltpd->hDlg;
	this->ltpdR.hLv = ltpd->hLv;
	if (this->ltpdR.getPathAllocSize () > lstrlen (ltpd->path))
	{
		lstrcpy (this->ltpdR.path, ltpd->path);
	}
	else
	{
		printfd ("Watcher:setLtpdR: неверная длина строки\n");
	}
}


VOID Watcher :: setLtpdL (LTPD* ltpd)
{
	this->ltpdL.hDlg = ltpd->hDlg;
	this->ltpdL.hLv = ltpd->hLv;
	if (this->ltpdL.getPathAllocSize () > lstrlen (ltpd->path))
	{
		lstrcpy (this->ltpdL.path, ltpd->path);
	}
	else
	{
		printfd ("Watcher:setLtpdL: неверная длина строки\n");
	}
}


Watcher :: Watcher (DWORD waitTime)
{
	this->waitTime = waitTime;
	this->dirChangeHandler = this->stubHandler;
	this->hStopWatch = INVALID_HANDLE_VALUE;
	this->hWorker = INVALID_HANDLE_VALUE;
}


Watcher :: ~ Watcher (VOID)
{
	this->stop ();
}


DWORD WINAPI Watcher :: watchWorker (VOID* arg)
{
	Watcher* watcher = (Watcher*) arg; 
	HANDLE hChange [3];		// Дескрипторы событий
	DWORD waitStatus = 0;	// Состояние ожидания

	DWORD watchFlags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES;

	// Дескриптор события "Содержимое папки правой панели изменилось"
	hChange[0] = FindFirstChangeNotification (watcher->ltpdR.path, FALSE, watchFlags);
	//wprintfd (L"watchWorker:rightpath: \"%s\"\n", watcher->ltpdR.path);
	if (hChange [0] == INVALID_HANDLE_VALUE)
	{
		waitStatus = WAIT_FAILED;
	}

	// Дескриптор события "Останов наблюдения" 
	hChange [1] = watcher->hStopWatch;
	if (hChange [1] == INVALID_HANDLE_VALUE)
	{
		waitStatus = WAIT_FAILED;
	}
	
	// Дескриптор события "Содержимое папки левой панели изменилось"
	hChange [2] = FindFirstChangeNotification (watcher->ltpdL.path, FALSE, watchFlags);
	//wprintfd (L"watchWorker:leftpath: \"%s\"\n", watcher->ltpdL.path);
	if (hChange [2] == INVALID_HANDLE_VALUE)
	{
		waitStatus = WAIT_FAILED;
	}

	while (waitStatus != WAIT_FAILED)
	{
		waitStatus = WaitForMultipleObjects (sizeof (hChange) / sizeof	(HANDLE), hChange, FALSE, INFINITE);
		switch (waitStatus)
		{
			case WAIT_OBJECT_0 + 0:		// Событие "Директория правой панели изменилась"
				watcher->dirChangeHandler (watcher->ltpdR.hLv);
				wprintfd (L"Директория правой панели изменилась\n");
				break;
			case WAIT_OBJECT_0 + 2:		// Событие "Директория левой панели изменилась"
				watcher->dirChangeHandler (watcher->ltpdL.hLv);
				wprintfd (L"Директория левой панели изменилась\n");
				break;
			default:
				waitStatus = WAIT_FAILED;
				break;
		}
		if (waitStatus != WAIT_FAILED)
		{
			FindNextChangeNotification (hChange [waitStatus]);
		}
	}
	BOOL result = FindCloseChangeNotification (hChange[0]);
	result &= FindCloseChangeNotification (hChange[2]);
	return DWORD (!result);
}

