#include "abstract.h"

DWORD getInterfaceCode (TCHAR* path)
{
	// Диск ?
	if ((path[1] == L':') && (path[2] == L'\\')) return FM_IC_DISK;
	if		(strstart (path, L"ftp://")) return FM_IC_FTP;
	else if (strstart (path, L"nfs://")) return FM_IC_NFS;
	else if (strstart (path, L"\\\\")) return FM_IC_NET;
	printfd ("getInterfaceCode: не известный протокол\n");
	return FM_IC_UNCKNOWN;
}

HANDLE Find :: first (TCHAR* path, WIN32_FIND_DATA* pFindData)
{
	memset (pFindData, 0, sizeof(WIN32_FIND_DATA));
	DWORD ic = getInterfaceCode (path);
	switch (ic)
	{
		case FM_IC_DISK:
		{
			CdInfo* cdInfo = new CdInfo;
			if (GetTOC_SPTI (path, cdInfo))
			{
				if (GetCDTEXT_SPTI (path, cdInfo))
				{
					if (cdInfo->tracks.size() > 0)
					{
						printfd ("Album artist: %s ", cdInfo->albumArtist);
						printfd ("Album title: %s ", cdInfo->albumTitle);
						printfd ("Extended data: %s ", cdInfo->extendedData);
						multiByteToWideChar (pFindData->cFileName, cdInfo->tracks[0].name);
						if (cdInfo->lastFind < cdInfo->nrOfTracks)
						{
							++ cdInfo->lastFind;
						}
						this->next = cdaFindNext;
						this->close = cdaFindClose;
						return HANDLE (cdInfo);
					}
				}
			}
			else
			{
				delete cdInfo;
			}
		}
	}
	this->next = diskFindNext;
	this->close = diskFindClose;
	return FindFirstFile (path, pFindData);
}


HANDLE diskFindFirst (TCHAR* path, WIN32_FIND_DATA* pFindData)
{
	return FindFirstFile (path, pFindData);
}


BOOL diskFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData)
{
	return FindNextFile (hObj, pFindData);
}


BOOL diskFindClose (HANDLE hObj)
{
	return FindClose (hObj);
}


BOOL cdaFindNext (HANDLE hObj, WIN32_FIND_DATA* pFindData)
{
	CdInfo* cdInfo = (CdInfo*) hObj;
	if (cdInfo->lastFind < cdInfo->tracks.size())
	{
		multiByteToWideChar (pFindData->cFileName, cdInfo->tracks[cdInfo->lastFind].name);
		++ cdInfo->lastFind;
		return TRUE;
	}
	return FALSE;
}

BOOL cdaFindClose (HANDLE hObj)
{
	delete (CdInfo*) hObj;
	return TRUE;
}


