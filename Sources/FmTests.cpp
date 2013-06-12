
#include "fmTests.h"

/*
int main (int argc, TCHAR* argv[])
{
	_wsetlocale (LC_ALL, L"rus");
	CopyTask copyTask;
	copyTask.setDstFolder (L"e:\\temp\\1\\");
	copyTask.setSrcFolder (L"c:\\prg\\qwerti\\");
	for (int i = 0; i < 10; i ++)
	{
		TCHAR fileName[MAX_PATH];
		wsprintf(fileName, L"%s_%u", L"3óáàðà3÷¨çÊà_áóðàô÷åã_ÇîÁÀÁ¨ÐÍÀÃÀ", i);
		wprintf (L"%s\n", fileName);
		copyTask.addFileName (fileName);
	}
	while(copyTask.nextPath())
	{
		wprintf (L"%s\n", copyTask.getCurSrcPath());
		wprintf (L"%s\n", copyTask.getCurDstPath());
		wprintf (L" ---- \n");
	}
	copyTask.clear ();
	return 0;
}
*/

/*
BOOL WINAPI EnumerateFunc(NETRESOURCE* lpnr)
{
 
  DWORD dwResult, dwResultEnum;
  HANDLE hEnum = NULL;
  DWORD cbBuffer = 16384;      // 16K is a good size
  DWORD cEntries = -1;         // enumerate all possible entries
  LPNETRESOURCE lpnrLocal = NULL;     // pointer to enumerated structures
  DWORD i;
  //
  // Call the WNetOpenEnum function to begin the enumeration.
  //
  dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
                          RESOURCETYPE_ANY,   // all resources
                          0,        // enumerate all resources
                          lpnr,     // NULL first time the function is called
                          &hEnum);  // handle to the resource

  if (dwResult != NO_ERROR)
  {  
    //
    // Process errors with an application-defined error handler.
    //
    //NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
	printf("*** net error ***\n");
    return FALSE;
  }
  //
  // Call the GlobalAlloc function to allocate resources.
  //
  lpnrLocal = (LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);
  if (lpnrLocal == NULL) 
      return FALSE;
  
  do
  {  
    //
    // Initialize the buffer.
    //
    ZeroMemory(lpnrLocal, cbBuffer);
    //
    // Call the WNetEnumResource function to continue
    //  the enumeration.
    //
    dwResultEnum = WNetEnumResource(hEnum,      // resource handle
                                    &cEntries,  // defined locally as -1
                                    lpnrLocal,  // LPNETRESOURCE
                                    &cbBuffer); // buffer size
    //
    // If the call succeeds, loop through the structures.
    //
    if (dwResultEnum == NO_ERROR)
    {
      for(i = 0; i < cEntries; i++)
      {
        // Call an application-defined function to
        //  display the contents of the NETRESOURCE structures.
        //
        //DisplayStruct(hdc, &lpnrLocal[i]);
        wprintf (L"%s\n", lpnrLocal[i].lpRemoteName);

        // If the NETRESOURCE structure represents a container resource, 
        //  call the EnumerateFunc function recursively.

        if(RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage
                                       & RESOURCEUSAGE_CONTAINER))
          if(!EnumerateFunc(&lpnrLocal[i]))
            wprintf (L"EnumerateFunc returned FALSE.\n");
      }
    }
    // Process errors.
    //
    else if (dwResultEnum != ERROR_NO_MORE_ITEMS)
    {
      //NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
      printf("*** net error ***\n");
      break;
    }
  }
  //
  // End do.
  //
  while(dwResultEnum != ERROR_NO_MORE_ITEMS);
  //
  // Call the GlobalFree function to free the memory.
  //
  GlobalFree((HGLOBAL)lpnrLocal);
  //
  // Call WNetCloseEnum to end the enumeration.
  //
  dwResult = WNetCloseEnum(hEnum);
  
  if(dwResult != NO_ERROR)
  { 
    //
    // Process errors.
    //
    //NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetCloseEnum");
	printf("*** net error ***\n");
    return FALSE;
  }

  return TRUE;
}
*/

/*
int main (int argc, TCHAR* argv[])
{
	setlocale (LC_ALL, "rus");
	NETRESOURCE nr;
	EnumerateFunc (NULL);
	return 0;
}
*/


/*int main (int argc, TCHAR* argv[])*/
 