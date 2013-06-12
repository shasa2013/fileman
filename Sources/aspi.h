#if !defined(ASPI_H_INCLUDED_)
#define ASPI_H_INCLUDED_


#include <windows.h>
#include "ASPI_struc.h"
#include "cdinfo.h"
#include "msf.h"


DWORD Msf2DWORD (Msf org);
DWORD Tmsf2DWORD (Tmsf org);
Msf  DWORD2CMsf (DWORD org);
DWORD TOC2DWORD (BYTE* ptr);


#endif

