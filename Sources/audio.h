// Часть кода Fileman.
// Горшков Александр Сергеевич, 2009 г, Таганрог
// Заголовки процедур для AudioCD

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "build.h"

#include <stddef.h>
#include <vector>
#include <string>
#include <mmsystem.h>
#include <winioctl.h>

#include "ASPI_struc.h"
#include "consts.h"

#define OS_UNKNOWN  (-1)
#define OS_WIN95    0
#define OS_WIN98    1
#define OS_WINNT35  2
#define OS_WINNT4   3
#define OS_WIN2K    4

using namespace std;

typedef struct _TOCTRACK
{
	BYTE      rsvd;
	BYTE      ADR;
	BYTE      trackNumber;
	BYTE      rsvd2;
	BYTE      addr[4];
} TOCTRACK;

typedef struct _TOC
{
	WORD      tocLen;
	BYTE      firstTrack;
	BYTE      lastTrack;
	TOCTRACK tracks[100];
} TOC, *PTOC, FAR *LPTOC;


class CdText_data
{
	public:
		BYTE  type;
		BYTE  track;
		BYTE  seq;
		BYTE  characterPosition:4;  // character position
		BYTE  block:3;		        // block number 0..7
		BYTE  bDBC:1;		        // double byte character
		char  text[12];
		BYTE  crc[2];
};

typedef struct _CdTextData
{
	BYTE  type;
	BYTE  track;
	BYTE  seq;
	BYTE  characterPosition:4;  // character position
	BYTE  block:3;		        // block number 0..7
	BYTE  bDBC:1;		        // double byte character
	char  text[12];
	BYTE  crc[2];
} CdTextData;

class Msf
{
public:
	Msf () { this->msf = 0; }

	Msf (DWORD msf) { this->msf = msf; }

	Msf (BYTE minute, BYTE second, BYTE frame)
	{ this->msf = MCI_MAKE_MSF (minute, second, frame); }

	operator DWORD() const  { return msf; }

	BYTE GetMinute () const { return MCI_MSF_MINUTE (this->msf); }
	BYTE GetSecond () const { return MCI_MSF_SECOND (this->msf); }
	BYTE GetFrame  () const { return MCI_MSF_FRAME  (this->msf); }

protected:
	DWORD msf;
};



class Tmsf
{
public:
	Tmsf () { this->tmsf = 0; }

	Tmsf (DWORD tmsf) { this->tmsf = tmsf; }

	Tmsf (BYTE track, BYTE minute, BYTE second, BYTE frame)
	{ this->tmsf = MCI_MAKE_TMSF(track, minute, second, frame); }

	operator DWORD () const { return this->tmsf; }

	BYTE GetTrack  () const { return MCI_TMSF_TRACK  (this->tmsf); }
	BYTE GetMinute () const { return MCI_TMSF_MINUTE (this->tmsf); }
	BYTE GetSecond () const { return MCI_TMSF_SECOND (this->tmsf); }
	BYTE GetFrame  () const { return MCI_TMSF_FRAME  (this->tmsf); }

protected:
	DWORD tmsf;
};

class TrackRecord
{
public:
	TrackRecord ();
	TrackRecord (const TrackRecord& track);
	TrackRecord& operator = (const TrackRecord& track);
	int nr;
	char name [256];
	Msf length;
	Msf startPos;
	BOOL isAudio;
};

class CdInfo
{
public:
	void Clear();
	CdInfo& operator = (const CdInfo& cdInfo);
	vector <TrackRecord> tracks;
	DWORD endPos;
	DWORD nrOfTracks;					//*< Number of tracks
	char albumArtist [256];				//*< Album Artist
	char albumTitle [256];				//*< Album Title
	char extendedData [4096];           //*< The extended data for this CD
	CdInfo (void);
	BYTE bigbuffer [4096];
	DWORD lastFind;						//*< Индекс последнего запрошенного трека
};

BOOL SPTI_Command (TCHAR* path, BYTE* indata, BYTE* outdata, int outdatalen);
BOOL GetCDTEXT_SPTI (TCHAR* drivename, CdInfo* cdInfo);
BOOL GetTOC_SPTI (TCHAR* path, CdInfo* CD);
VOID GetTracksFromTOC (CdInfo* cdInfo, TOC* pToc);


int getOsVersion (VOID);

DWORD TOC2DWORD (BYTE* ptr);
Msf DWORD2Msf (DWORD org);
DWORD Msf2DWORD (Msf org );
DWORD Tmsf2DWORD (Tmsf org);
void readCdText (CdInfo* cdInfo);



#endif // _AUDIO_H_
