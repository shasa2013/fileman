#include "ltpd.h"



LTPD :: LTPD (TCHAR* path)
{
	this->path = (TCHAR*) fmAlloc (lstrlen(path) * sizeof(TCHAR) + sizeof (L'\0'));
	lstrcpy (this->path, path);
}



LTPD :: LTPD (VOID)
{
	this->path = (TCHAR*) fmAlloc (FM_MAX_PATH);
}



LTPD :: ~ LTPD ()
{
	if (this->path) fmFree ((HLOCAL*)&this->path);
}



SIZE_T LTPD :: getPathAllocSize (VOID)
{
	return fmSize ((HLOCAL)this->path);
}
