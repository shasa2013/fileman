#ifndef _progress_
#define _progress_

#include "build.h"

#include "helper.h"
#include "resource.h"

class Progress
{	
public:
	DWORD indent;
	DWORD maxIndent;
	DWORD startTick;
	DWORD prevTick;
	DWORD curTick;
	DWORD updateRate;
	HWND hDlg;
	UINT itemId;
	TCHAR* path;
	Progress ();
};

DWORD CALLBACK progressRoutine (LARGE_INTEGER TotalFileSize,
									LARGE_INTEGER TotalBytesTransferred,
									LARGE_INTEGER StreamSize,
									LARGE_INTEGER StreamBytesTransferred,
									DWORD dwStreamNumber,
									DWORD dwCallbackReason,
									HANDLE hSrcFile,
									HANDLE hDstFile,
									VOID* lpData);



#endif // _progress_