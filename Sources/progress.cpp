#include "progress.h"


Progress :: Progress ()
{
	this->startTick = 0;
	this->prevTick = 0;
	this->curTick = 0;
	this->updateRate = 1000;
	this->hDlg = 0;
	this->indent = 0;
	this->itemId = 0;
	this->maxIndent = 0;
}



// Процедура индикации прогресса копирования (см. MSDN CopyFileEx)
DWORD CALLBACK progressRoutine
	(LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason, 
	HANDLE hSrcFile,
	HANDLE hDstFile,
	LPVOID lpData)
{	
	TCHAR str [MAX_PATH];
	
	Progress* progr = (Progress*) lpData;

	progr->curTick = GetTickCount();
	// Проверка - первый вызов процедуры?
	if (dwCallbackReason == CALLBACK_STREAM_SWITCH) 
	{	// ... да
		progr->startTick = progr->curTick;
		progr->prevTick = progr->startTick;
		ShowWindow (progr->hDlg, SW_SHOW);
		
		getFileNameFromHandle (hSrcFile, str, SIZETC (str));
		TCHAR compactPath [50];
		PathCompactPathEx (compactPath, str, SIZETC (compactPath) - sizeof (L'\0'), 0);
		SendDlgItemMessage (progr->hDlg, IDC_COPY_SOURCE, WM_SETTEXT, 0, (LPARAM)compactPath);
	}
	
	
	//if ((progr->curTick - progr->prevTick) > progr->updateRate)
	//{
		// Проверить флаг активности операции копирования ...
		if (HANDLE (FALSE) == GetProp (progr->hDlg, L"running"))
		{
			// Проверить скопирован ли файл целиком ...
			if (TotalFileSize.QuadPart != TotalBytesTransferred.QuadPart)
			{ // ... файл скопирован не полностью, завершить и удалить
				return PROGRESS_CANCEL; 
			}
			else
			{// ... файл скопирован полностью, завершить и оставить
				return PROGRESS_STOP;
			}
		}
		
		// Скорость копирования
		DWORD copySpeed = 0;
		DWORD timeDeltaSec = (DWORD) ((progr->curTick - progr->startTick) / 1000);
		if (timeDeltaSec)
		{
			copySpeed = (DWORD) ((TotalBytesTransferred.QuadPart) / timeDeltaSec);
			StrFormatByteSize ((LONGLONG)copySpeed, str, SIZETC (str));
			lstrcat (str, L"/c ");
			SendDlgItemMessage (progr->hDlg, IDC_COPY_SPEED, WM_SETTEXT, 0, (LPARAM)str);
		}
		else
		{
			SendDlgItemMessage (progr->hDlg, IDC_COPY_SPEED, WM_SETTEXT, 0, (LPARAM)L"--");
		}
	
		str [0] = L'\0';	
	
		// Процент выполнения копирования
		if (TotalFileSize.QuadPart)
		{
			//_itow ((int)((TotalBytesTransferred.QuadPart * 100) / TotalFileSize.QuadPart), str, 10);
			//lstrcat (str, L"%");
			//SendDlgItemMessage (progr->hDlg, IDC_COPY_PERCENT, WM_SETTEXT, 0, (LPARAM)str);
			//str [0] = L'\0';
		
			// Прогресс-бар копирования
			DWORD progress = (DWORD) ((TotalBytesTransferred.QuadPart * 100) / TotalFileSize.QuadPart);
			SendDlgItemMessage (progr->hDlg, IDC_COPY_PROGRESS_BAR, PBM_SETPOS, (WPARAM)progress, 0);
		}
		else
		{
			SendDlgItemMessage (progr->hDlg, IDC_COPY_PROGRESS_BAR, PBM_SETPOS, 0, 0);
		}
		// Времени осталось
		LARGE_INTEGER bytesEstimated;
		bytesEstimated.QuadPart = TotalFileSize.QuadPart - TotalBytesTransferred.QuadPart;
		if (copySpeed)
		{
			DWORD timeEstimated = (DWORD) (bytesEstimated.QuadPart / copySpeed);
			wsprintf (str, L"%02u:%02u",  timeEstimated / 60, timeEstimated % 60);
			SendDlgItemMessage (progr->hDlg, IDC_COPY_TIME, WM_SETTEXT, 0, (LPARAM)str);
		}
		else
		{
			SendDlgItemMessage (progr->hDlg, IDC_COPY_TIME, WM_SETTEXT, 0, (LPARAM)L"--:--");
		}
			
		// Сохранить текущее время
		progr->prevTick = progr->curTick;
	//}
	return PROGRESS_CONTINUE;
}
