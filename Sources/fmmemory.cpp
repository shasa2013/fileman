#include "fmmemory.h"

HLOCAL fmFree (HLOCAL* hMem)
{
	if (*hMem == NULL)
	{
		printfd ("%s: fmFree: NULL-указатель\n", __FILE__);
		return NULL;
	}
	HLOCAL hLocal = LocalFree (*hMem);
	*hMem = NULL;
	return hLocal;
}


HLOCAL fmAlloc (SIZE_T len)
{
	HLOCAL hl = LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, len);
	if (hl == NULL)
	{
		DWORD rc = GetLastError ();
		printfd ("fmAlloc:");
		printdErrorStr (rc);
		exit (rc);
	}
	return hl;
}


SIZE_T fmSize (HLOCAL hMem)
{
	return LocalSize (hMem);
}
