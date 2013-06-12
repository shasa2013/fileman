#include "aspi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CASPI::CheckASPI ()
{
	HINSTANCE hDll;

	// check if aspi is available
	hDll = LoadLibrary ("WNASPI32.DLL");
	GetASPI32SupportInfo = (DWORD(*)(void))GetProcAddress (hDll, "GetASPI32SupportInfo");
	SendASPI32Command = (DWORD(*)(LPSRB))GetProcAddress (hDll, "SendASPI32Command");

	// make sure that we've got both function addresses
	if (GetASPI32SupportInfo && SendASPI32Command)
	{
		return TRUE;
	}

	return FALSE;
}





//-------------------------------------------------------------------------
BOOL CASPI::GetTOC_ASPI(int disknr, CdInfo& CD)
{
  BYTE command[16];
  BOOL retVal;
  TOC toc;

  toc.firstTrack = 0;
  toc.lastTrack  = 0;
  toc.tocLen     = 0;


  if( ASPI_DriveInfo() == FALSE )
    return FALSE;



  ZeroMemory( command, sizeof(command) );
  command[0] = 0x43;              // read TOC command
  command[1] = 0x02;              // MSF mode 
  command[7] = sizeof(TOC) / 256; // high-order byte of buffer len
  command[8] = sizeof(TOC) % 256; // low-order byte of buffer len
  
  retVal = ASPI_Command( disknr, command, (BYTE*)&toc, sizeof(TOC) );

  if ( retVal == TRUE )
  {
    if( toc.firstTrack != 1 )
      retVal = FALSE;
    else
      GetTracksFromTOC( CD, &toc );
  }

  return retVal;
}



//-------------------------------------------------------------------------
BOOL CASPI::GetTOC_SPTI(char* drivename, CdInfo& CD)
{
	BYTE command[16];
	BOOL retVal;
	TOC  toc;

	ZeroMemory( command, sizeof(command) );
	command[0] = 0x43;              // read TOC command
	command[1] = 0x02;              // MSF mode 
	command[7] = sizeof(TOC) >> 8; // high-order byte of buffer len
	command[8] = sizeof(TOC) & 0xff; // low-order byte of buffer len

	retVal = SPTI_Command (drivename, command, (BYTE*)&toc, sizeof (TOC));

	if (retVal == TRUE)
	{
		if (toc.firstTrack != 1)
			retVal = FALSE;
		else
			GetTracksFromTOC( CD, &toc );
	}
	return retVal;
}


// -------------------------------------------------------------------------
BOOL CASPI::GetCDTEXT_ASPI(int disknr, CdInfo& CD)
{
  BYTE  command[16];
  BOOL  retVal;

  ZeroMemory( command, sizeof(command) );
  command[0]   = 0x43;   // read TOC command
  command[1]   = 0x02;   // MSF mode 
  command[2]   = 0x05;   // CD text
  command[7] = sizeof(bigbuffer) >> 8; // high-order byte of buffer len
  command[8] = sizeof(bigbuffer) & 0xff; // low-order byte of buffer len

  ZeroMemory( bigbuffer, sizeof(bigbuffer) );
  retVal = ASPI_Command( disknr, command, bigbuffer, sizeof(bigbuffer) );

  if( retVal == TRUE )
    ReadCDTEXT( CD );
  return retVal;
}



//-------------------------------------------------------------------------
BOOL CASPI::GetCDTEXT_SPTI (char* drivename, CdInfo& CD)
{
	BYTE  command[16];
	BOOL  retVal;

	ZeroMemory (command, sizeof(command));
	command[0]   = 0x43;   // read TOC command
	command[1]   = 0x02;   // MSF mode 
	command[2]   = 0x05;   // CD text
	command[7] = sizeof (bigbuffer) >> 8;  // high-order byte of buffer len
	command[8] = sizeof (bigbuffer) & 0xff;// low-order byte of buffer len

	ZeroMemory (bigbuffer, sizeof (bigbuffer));
	retVal = SPTI_Command (drivename, command, bigbuffer, sizeof(bigbuffer));

	if (retVal == TRUE)
	ReadCDTEXT (CD);
	return retVal;
}



// -------------------------------------------------------------------------
BOOL CASPI::ASPI_DriveInfo()
{
	SRB_HAInquiry sh;
	SRB_GDEVBlock sd;
	BYTE numAdapters, maxTgt;
	BYTE i, j, k;
	int idx = 0;
	DWORD d;
	BYTE bCount, bStatus;

	// initialize the drive list;
	ZeroMemory (&driveList, sizeof (driveList));

	d = GetASPI32SupportInfo();
	bCount = LOBYTE(LOWORD(d));
	bStatus = HIBYTE(LOWORD(d));

	if ( bStatus != SS_COMP && bStatus != SS_NO_ADAPTERS )
	return FALSE;



	numAdapters = (BYTE) bCount;
	if (numAdapters == 0) 
	{
		return FALSE;
	}

	for(i = 0; i < numAdapters; ++ i)
	{
		ZeroMemory (&sh, sizeof(sh));
		sh.SRB_Cmd = SC_HA_INQUIRY;
		sh.SRB_HaID = i;
		SendASPI32Command( (LPSRB)&sh );

		// in case of error, skip to next adapter
		if ( sh.SRB_Status != SS_COMP )
		  continue;

		// determine the max target number for the adapter from offset 3
		// if it's zero, then assume the max is 8
		maxTgt = sh.HA_Unique[3];
		if (maxTgt == 0) maxTgt = 8;

		for( j = 0; j < maxTgt; ++ j)
		{
			// try all 8 values for LUN
			for (k = 0; k < 8; ++ k)
			{
				ZeroMemory (&sd, sizeof(sd));
				sd.SRB_Cmd = SC_GET_DEV_TYPE;
				sd.SRB_HaID = i;
				sd.SRB_Target = j;
				sd.SRB_Lun = k;
				SendASPI32Command ((LPSRB)&sd);
				if (sd.SRB_Status == SS_COMP)
				{
					if ( sd.SRB_DeviceType == DTYPE_CDROM && driveList.num <= MAX_DRIVE_LIST )
					{
						idx = driveList.num ++;
						driveList.drive[idx].a.ha = i;
						driveList.drive[idx].a.tgt = j;
						driveList.drive[idx].a.lun = k;
						wsprintf( driveList.drive[idx].a.desc, "ASPI[%d:%d:%d]", i, j, k );
					}
				}
			}
		}
	}
	return TRUE;
}




// -------------------------------------------------------------------------
void CASPI::GetTracksFromTOC (CdInfo& cdInfo, TOC* ptoc)
{
	int i;
	TrackRecord TR;

	if (cdInfo.m_NrOfTracks == ptoc->lastTrack)
	{
		return;
	}

	for (i = cdInfo.m_NrOfTracks; i < ptoc->lastTrack; ++ i)
	{
		DWORD startpos = TOC2DWORD (ptoc->tracks[i].addr);
		DWORD nextpos  = TOC2DWORD (ptoc->tracks[i + 1].addr);
		DWORD len      = nextpos - startpos;

		TR.startpos = DWORD2CMsf (startpos);
		TR.length   = DWORD2CMsf (len);
		TR.nr       = i + 1;
		TR.isAudio  = ((ptoc->tracks[i].ADR & 0x04) == 0);

		cdInfo.tracks.push_back (TR);
	}

	cdInfo.m_NrOfTracks = ptoc->lastTrack;
	cdInfo.m_EndPos = (DWORD) TOC2DWORD (ptoc->tracks[ptoc->lastTrack].addr);
}







BOOL  CASPI::ASPI_Command (int disknr, BYTE* indata, BYTE* outdata, int outdatalen)
{
	HANDLE hEvent;
	SRB_ExecSCSICmd s;
	DWORD dwStatus;
	ASPIDRV driveinfo;

	driveinfo = driveList.drive[disknr].a;

	hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);

	ZeroMemory (&s, sizeof(s));
	s.SRB_Cmd        = SC_EXEC_SCSI_CMD;
	s.SRB_HaID       = driveinfo.ha;
	s.SRB_Target     = driveinfo.tgt;
	s.SRB_Lun        = driveinfo.lun;
	s.SRB_Flags      = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	s.SRB_BufLen     = outdatalen;
	s.SRB_BufPointer = (BYTE FAR *) outdata;
	s.SRB_SenseLen   = 0x0E;
	s.SRB_CDBLen     = 0x0A;
	s.SRB_PostProc   = (LPVOID) hEvent;
	//memcpy (s.CDBByte, indata, 10);
	CopyMemory (s.CDBByte, indata, 10);

	ResetEvent (hEvent);
	dwStatus = SendASPI32Command ((LPSRB) &s);

	if (dwStatus == SS_PENDING)
	{
		WaitForSingleObject (hEvent, 10000);  // wait up to 10 secs
	}
	
	CloseHandle (hEvent);

	if (s.SRB_Status != SS_COMP)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL  CASPI::SPTI_Command (char* drivename, BYTE* indata, BYTE* outdata, int outdatalen)
{
	HANDLE hDrive;
	ULONG length;
	char buf[32];
	int retVal = TRUE;
	DWORD dwReturned;
	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER swb;
	DWORD dwFlags = GENERIC_READ;

	if (getOsVersion () >= OS_WIN2K)
	{
		dwFlags |= GENERIC_WRITE;
	}

	wsprintf (buf, "\\\\.\\%s", drivename);
	hDrive = CreateFile (buf, dwFlags, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hDrive == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	dwReturned = 0;
	length = sizeof (SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	ZeroMemory (&swb, sizeof(swb));
	swb.spt.Length              = sizeof(SCSI_PASS_THROUGH);
	swb.spt.CdbLength           = 10;
	swb.spt.DataIn              = SCSI_IOCTL_DATA_IN;
	swb.spt.DataTransferLength  = outdatalen;
	//swb.spt.SenseInfoLength   = 24;
	swb.spt.TimeOutValue        = 10;
	swb.spt.DataBuffer          = outdata;
	swb.spt.SenseInfoOffset = offsetof (SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
	CopyMemory (swb.spt.Cdb, indata, 10);
	retVal = DeviceIoControl(hDrive,
						   IOCTL_SCSI_PASS_THROUGH_DIRECT,
						   &swb,
						   length,
						   &swb,
						   length,
						   &dwReturned,
						   NULL);
	//  if( retVal == FALSE )
	//    Output( "%02X, %s", indata[0], GetErrorString() );
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



CMsf DWORD2CMsf( DWORD org )
{
	static CMsf result;

	int frm = org % 75;
	int sec = (org / 75) % 60;
	int min = (org / (75*60));

	result = CMsf (min, sec, frm);

	return result;
}




DWORD CMsf2DWORD (CMsf org )
{
	static DWORD result;
	
	result  = org.GetMinute() * 60 * 75;
	result += org.GetSecond() * 75;
	result += org.GetFrame();
	
	return result;
}



DWORD CTmsf2DWORD (CTmsf org)
{
	static DWORD result;

	result  = org.GetMinute() * 60 * 75;
	result += org.GetSecond() * 75;
	result += org.GetFrame();

	return result;
}

