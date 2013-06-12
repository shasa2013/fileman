// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.

#ifndef _USER_H_
#define _USER_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>

#include "consts.h"

/** �������� ������ */
struct FmPanelProps
{
	COLORREF bkColors [2];		/*< ����� ���� ������, ��� ������ � �������� �����				*/
	COLORREF customColors [16];	/*< ������ ����� ������ � ������� ������ �����					*/
	FLOAT colsRelWidths [4];	/*< ������������� ������ �������								*/
	int sortColumnIndex;		/*< ������ �������, �� ������� ��������� ����������				*/
	int steckSels [FM_MAX_INDENT];/*< ���� �������� ����� ������, � ������� ��� ��������� ����	*/
	int steckInd;				/*< ��������� ����� �������� ����� ������						*/
	BOOL sortDir;				/*< ����������� ����������										*/
	int countedItem;
    WNDPROC defProc;			/*< ��������� �� �������� ��������� ����						*/
	HWND hFullPath;				/*< ���������� Combo-box � ������ ����							*/
	HWND hSelSize;				/*< ���������� Static � �������� ���������� ������				*/
	HWND hDlg;					/*< ���������� �������� ����������� ����						*/
	TCHAR* curDir;	/*< */
};


/** ��������� �������� �������� �������. ������������� ���������.
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ��������
TCHAR* fileName - ��� ����� */
BOOL saveDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName);

/** ��������� �������� �������� �������. ������������� ���������.
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ��������
TCHAR* fileName - ��� ����� */
BOOL loadDlgItemProperties (HWND hDlg, UINT itemId, TCHAR* fileName);

/** ���������� ������ � ������ ���� � ini ����� ���������
TCHAR* modulePath - ���� � ini ����� ���������.
DWORD size - ������ ������, �� ������� ��������� modulePath � ������ */
BOOL getIniFileName (TCHAR* modulePath, DWORD size);

/** ������������� ������ ������� ListVeiw
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ListVeiw
DWORD count - ���������� ������� */
BOOL setListVeiwColumnsWidth (HWND hLv, DWORD count, DWORD colWidths []);

/** ���������� ������ �������
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ListVeiw
DWORD count - ���������� �������
DWORD colWidths [] - ������ ������� */
BOOL getListVeiwColumnsWidth (HWND hDlg,  UINT itemId, DWORD count, DWORD colWidths []);

/** ���������� ������ �������
HWND hDlg - ���������� ListVeiw
DWORD count - ���������� �������
DWORD colWidths [] - ������ ������� */
BOOL getListVeiwColumnsWidth (HWND hLv,  DWORD count, DWORD colWidths []);

/** ���������� ��������� �������� �������
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ��������
RECT* pRect - �������������, ���������� ��������� */
HWND getDlgItemPos (HWND hDlg, UINT itemId, RECT* pRect);

/** �������������� �������
hDlg - ���������� �������
DWORD itemId - ������������� List View
DWORD columns - ���������� �������
TCHAR* headers [] - ��������� ������� */
BOOL initListViewColumns (HWND hDlg, UINT itemId, DWORD columns, TCHAR* headers [], DWORD colWidths []);



#endif // _USER_H_

