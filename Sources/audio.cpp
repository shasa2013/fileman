#include "audio.h"

void CdInfo :: Clear ()
{
	this->tracks.clear();
	this->endPos = 0;
	this->nrOfTracks = 0;

	this->albumArtist[0] = '\0';
	this->albumTitle[0] = '\0';
	this->extendedData[0] = '\0';
}


CdInfo :: CdInfo (void)
{
	this->albumArtist[0] = '\0';
	this->albumTitle[0] = '\0';
	this->extendedData[0] = '\0';
	this->lastFind = 0;
}


TrackRecord :: TrackRecord ()
{
	this->nr = 0;
	this->name[0] = '\0';
	this->length = 0;
	this->startPos = 0;
	this->isAudio = FALSE;
}


TrackRecord :: TrackRecord (const TrackRecord& track)
{
	*this = track;
}


TrackRecord& TrackRecord :: operator = (const TrackRecord& track)
{
	this->nr = track.nr;

	lstrcpyA (this->name, track.name);

	this->length = track.length;
	this->startPos = track.startPos;
	this->isAudio = track.isAudio;

	return *this;
}


BOOL GetCDTEXT_SPTI (TCHAR* drivename, CdInfo* cdInfo)
{
	BYTE command[16];
	BOOL retVal;

	memset (command, 0, sizeof(command));
	command[0] = 0x43;   // read TOC command
	command[1] = 0x02;   // MSF mode
	command[2] = 0x05;   // CD text
	command[7] = sizeof (cdInfo->bigbuffer) >> 8;  // high-order byte of buffer len
	command[8] = sizeof (cdInfo->bigbuffer) & 0xff;// low-order byte of buffer len

	memset (cdInfo->bigbuffer, 0, sizeof (cdInfo->bigbuffer));
	retVal = SPTI_Command (drivename, command, cdInfo->bigbuffer, sizeof(cdInfo->bigbuffer));

	if (retVal == TRUE)

	readCdText (cdInfo);

	return retVal;
}

VOID GetTracksFromTOC (CdInfo* cdInfo, TOC* ptoc)
{
	int i;
	TrackRecord TR;

	for (i = ptoc->firstTrack; i < ptoc->lastTrack; ++ i)
	{
		DWORD startpos = TOC2DWORD (ptoc->tracks[i].addr);
		DWORD nextpos  = TOC2DWORD (ptoc->tracks[i + 1].addr);
		DWORD len      = nextpos - startpos;

		TR.startPos = DWORD2Msf (startpos);
		TR.length   = DWORD2Msf (len);
		TR.nr       = i + 1;
		TR.isAudio  = ((ptoc->tracks[i].ADR & 0x04) == 0);

		cdInfo->tracks.push_back (TR);
	}

	cdInfo->nrOfTracks = ptoc->lastTrack;

	cdInfo->endPos = (DWORD) TOC2DWORD (ptoc->tracks[ptoc->lastTrack].addr);
}


BOOL GetTOC_SPTI (TCHAR* drivename, CdInfo* cdInfo)
{
	BYTE command[16];
	BOOL retVal;
	TOC  toc;

	ZeroMemory (command, sizeof (command));
	command[0] = 0x43;              // read TOC command
	command[1] = 0x02;              // MSF mode
	command[7] = sizeof(TOC) >> 8;	// high-order byte of buffer len
	command[8] = sizeof(TOC) & 0xff;// low-order byte of buffer len

	retVal = SPTI_Command (drivename, command, (BYTE*)&toc, sizeof (TOC));

	if (retVal == TRUE)
	{
		if (toc.firstTrack != 1)
			retVal = FALSE;
		else
			GetTracksFromTOC (cdInfo, &toc);
	}
	return retVal;
}



int getOsVersion (void)
{
	OSVERSIONINFO os;
	ZeroMemory (&os, sizeof(os));
	os.dwOSVersionInfoSize = sizeof(os);
	GetVersionEx (&os);

	if ( os.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		if (os.dwMajorVersion == 3 && os.dwMinorVersion >= 50)
		{
			return OS_WINNT35;
		}
		else if (os.dwMajorVersion == 4)
		{
			return OS_WINNT4;
		}
		else if (os.dwMajorVersion == 5)
		{
			return OS_WIN2K;
		}
	}
	else if ( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		if (os.dwMinorVersion == 0)
		{
			return OS_WIN95;
		}
		else
		{
			return OS_WIN98;
		}
	}
	return OS_UNKNOWN;
}


BOOL SPTI_Command (TCHAR* drivename, BYTE* indata, BYTE* outdata, int outdatalen)
{
	return FALSE; // Ўибко тормазит код далее.

	HANDLE hDrive;
	ULONG length;
	TCHAR buf[65535];
	int retVal = TRUE;
	DWORD dwReturned;
	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER swb;
	DWORD dwFlags = GENERIC_READ;

	if (getOsVersion () >= 4)
	{
		dwFlags |= GENERIC_WRITE;
	}

	wsprintf (buf, L"\\\\.\\%s", drivename);
	buf[6] = L'\0';
	printfd ("SPTI_Command: %s\n", buf);
	hDrive = CreateFile (buf, dwFlags, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hDrive == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	dwReturned = 0;
	length = sizeof (SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	ZeroMemory (&swb, sizeof(swb));
	swb.spt.Length = sizeof(SCSI_PASS_THROUGH);
	swb.spt.CdbLength = 10;
	swb.spt.DataIn = SCSI_IOCTL_DATA_IN;
	swb.spt.DataTransferLength = outdatalen;
	swb.spt.TimeOutValue = 10;
	swb.spt.DataBuffer = outdata;
	swb.spt.SenseInfoOffset = offsetof (SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
	CopyMemory (swb.spt.Cdb, indata, 10);
	retVal = DeviceIoControl (hDrive, IOCTL_SCSI_PASS_THROUGH_DIRECT, &swb,
						   length, &swb, length, &dwReturned, NULL);
	CloseHandle (hDrive);
	return retVal;
}

DWORD  TOC2DWORD (BYTE* ptr)
{
	static DWORD result;
	int frm = (BYTE) ptr[3];
	int sec = (BYTE) ptr[2];
	int min = (BYTE) ptr[1];
	result = (min * 60 * 75) + (sec * 75) + frm;
	return result;
}

Msf DWORD2Msf (DWORD org)
{
	static Msf result;
	int frm = org % 75;
	int sec = (org / 75) % 60;
	int min = (org / (75*60));
	result = Msf (min, sec, frm);
	return result;
}


DWORD Msf2DWORD (Msf org )
{
	static DWORD result;
	result  = org.GetMinute() * 60 * 75;
	result += org.GetSecond() * 75;
	result += org.GetFrame();
	return result;
}


DWORD Tmsf2DWORD (Tmsf org)
{
	DWORD result;
	result  = org.GetMinute() * 60 * 75;
	result += org.GetSecond() * 75;
	result += org.GetFrame();
	return result;
}


void readCdText (CdInfo* cdInfo)
{
	CdTextData*  pdata;
	int i;
	int j;
	char buffer[256];

	vector <basic_string <char> > artists;
	vector <basic_string <char> > titles;

	char tmpStr [1024];
	int idx;
	int track;

	memset (buffer, 0, sizeof(buffer));
	idx = 0;

	pdata = (CdTextData*) (&cdInfo->bigbuffer[4]);
	for (i = 0; i < 0xFF; ++ i)
	{
		if (pdata->seq != i)
		{
			break;
		}

		if ((pdata->type >= 0x80) && (pdata->type <= 0x85) && (pdata->block == 0))
		{
			track = pdata->track;
			for (j = 0; j < 12; ++ j)
			{
				if (pdata->text[j] == 0x00)
				{
					switch (pdata->type)
					{
						case 0x80:  // title
							if (track == 0)
							{
								strcpy (cdInfo->albumTitle, buffer);
							}
							else
							{
								titles.push_back (string(buffer));
							}
							++ track;
							idx = 0;
							break;
						case 0x81:  // artist
							if (track == 0)
								strcpy (cdInfo->albumArtist, buffer);
							else
								artists.push_back(string(buffer));
							++ track;
							idx = 0;
							break;
						case 0x82:  // songwriter
						case 0x83:  // composer
						case 0x84:  // arranger
							break;
						case 0x85:  // Message
							//CD.m_ExtendedData += buffer;
							strcat (cdInfo->extendedData, buffer);
							++ track;
							idx = 0;
							break;
					}
				}
				else
				{
					buffer[idx] = pdata->text[j];
					++ idx;
				}
				buffer[idx] = 0x00;
			}
		}
		++ pdata;
	}

	for (i = 0; i < int (cdInfo->nrOfTracks - 1); ++ i)
	{
		sprintf (cdInfo->tracks[i].name, "%s - %s", artists[i].c_str(), titles[i].c_str());
	}
}
