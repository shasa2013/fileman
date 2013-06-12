// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.

#ifndef _watcher_h_
#define _watcher_h_

#include "build.h"

#include "helper.h"
#include "browse.h"


/** ������ �� ����������� � �����, ������� ���������
������������ ��� ������������� ���������������� ��������� */
class Watcher
{	
	HANDLE hWorker;									/*< ���������� �������� ������ */
	HANDLE hStopWatch;								/*< ���������� ������� "������� ����������"	*/
	BOOL (*dirChangeHandler) (HWND hLv);			/*< ��������� �� ��������� ��������� ������� "���������� ����� ����������" */
	static DWORD WINAPI watchWorker (VOID* arg);	/*< ��������� �������� ������ */
	LTPD ltpdL;										/*< ��������, ������������ � ���������� ������� "���������� ����� ������ ������ ����������" */
	LTPD ltpdR;										/*< ��������, ������������ � ���������� ������� "���������� ����� ����� ������ ����������" */
	DWORD waitTime;									/*< ����������������� ����� ����� ��������� ������� */
	static BOOL stubHandler (HWND hLv);				/*< �����-��������, ��� ���������������� ������� */
	
public:
	
	// ������������� ������ ��� �������� � ������
	VOID setLtpdR (LTPD* ltpd);
	
	// ���������� ��������� �� ������ � ���� � �����
	TCHAR* getFolderR (VOID);
	BOOL setFolderR (TCHAR* folder);

	// ������������� ������ ��� �������� � ������
	VOID setLtpdL (LTPD* ltpd);
	
	/** ���������� ��������� �� ������ � ���� � ����� */
	TCHAR* getFolderL (VOID);
	BOOL setFolderL (TCHAR* folder);

	/** ��������� ���������� �� ������ */ 
	BOOL start (VOID);

	/** ������������� ���������� �� ������, ������ ��� �����������.
	����� ������, ������������� ���������� �� �������� */
	BOOL stop (VOID);

	/** ��������� ����� ���������� � ��������� "�����" ��� "������"
	BOOL isPaused - ����������� ��������� ����������:
	TRUE ��������� � �������� "�����", FALSE ��������� � �������� "������" */
	BOOL setPause (BOOL isPaused);

	/** ������������� ��������� ��������� ������� "���������� ����� ����������"
	BOOL (*dirChangeHandler) (VOID* arg) - ��������� �� ���������
	VOID* arg - ��������, ������������ � ��������� */
	VOID setDirChangeHandler (BOOL (*dirChangeHandler) (HWND ltpd));
	
	/** ����������� */
	Watcher (DWORD waitTime = 1000);
	
	/** ���������� */
	~ Watcher (VOID);
};


#endif // _watcher_h_