// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.

#ifndef _TASK_H_
#define _TASK_H_

#include "build.h"

#include <commctrl.h>
#include <vector>
#include "helper.h"
#include "resource.h"
#include "browse.h"
#include "user.h"
#include "progress.h"

#define TPM_DEFAULT		1						/*< ����� ������ ������, ��� ������� ���� ������ ���������� ����������� �� ���� ����� ���������, � ��� ����� */
#define TPM_DROP		2						/*< ����� ������ ������, ��� ������� ������ ������ ���������� �������� ������ ���� ������ */

/** ����� ��� �������� ������� �����������/��������/����������� */
class Task
{
	std::vector <TCHAR*> fileNames;				/*< ������ ��� � ����� ���������						*/
	TCHAR* dstFolder;							/*< ����� ����������									*/
	TCHAR* srcFolder;							/*< ����� ��������										*/
	DWORD updateRate;							/*< �������� ���������� ������ � ��������� �����������	*/
	DWORD index;								/*< ������ �������� ����� � ������ fileNames			*/
	TCHAR* curSrcPath;							/*< ������� ������ ���� ��������						*/
	TCHAR* curDstPath;							/*< ������� ������ ���� ����������						*/
	Progress progress;							/*< ������ ��������� �����������						*/
	DWORD pathMode;								/*< ����� ������ ������ ����������						*/
	DWORD flags;								/*< ����� ���������� ��� ��������						*/
	DWORD opCode;								/*< ��� ��������										*/
	
public:
	Task (DWORD pathMode = TPM_DEFAULT);		/*< �����������											*/
	VOID setDstFolder (TCHAR* path);			/*< ������������� ���� ����� ����������					*/
	VOID setSrcFolder (TCHAR* path);			/*< ������������� ���� ����� ���������					*/
	TCHAR* getCurSrcPath ();					/*< ���������� ��������� ������ ���� ����� ���������	*/
	TCHAR* getCurDstPath (VOID);				/*< ���������� ��������� ������ ���� ����� ����������	*/
	TCHAR* getDstFolder (VOID);					/*< ���������� ���� ����� ����������					*/
	TCHAR* getSrcFolder (VOID);
	DWORD getFileNamesCount (VOID);				/*< ���������� ���������� ��� ������					*/
	Progress* getProgress (VOID);				/*< ���������� ��������� �� ������ ��������� ��������	*/
	VOID addFileName (TCHAR* path);				/*< ��������� ��� �����									*/
	VOID clear (VOID);							/*< ������� �������										*/
	BOOL nextPath (VOID);						/*< ���������� ��������� ����							*/
	VOID resetPath (VOID);						/*< ���������� ������� ����								*/
	INT getOpCode (VOID);						/*< ���������� ��� ��������								*/
	VOID setOpCode (INT opCode);				/*< ������������� ��� ��������							*/
	INT getFlags (VOID);						/*< ���������� ����� ��������							*/
	VOID setFlags (INT);						/*< ������������� ����� ��������						*/
	~ Task ();									/*< ����������											*/
};


#endif // _TASK_H_