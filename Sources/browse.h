// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������
// ��������� �������� ��� �������� �����

#ifndef _BROWSE_H_
#define _BROWSE_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "fmmemory.h"
#include "user.h"
#include "resource.h"
#include "ltpd.h"
#include "consts.h"
#include "operation.h"
#include "abstract.h"
#include "watcher.h"


/** ������ ��� ���������� ����� � ListView */
struct FmSort
{
	DWORD column;	/*< ������ ������� �� ������� ����������� ������							*/
	BOOL dir;		/*< ���������� ����������: TRUE - �� �����������, FALSE - �� ��������		*/
	BOOL separate;	/*< ���������� ���������� � ������: TRUE - ��������, FALSE - ������			*/
	HWND hListView;	/*< ���������� ListView														*/
	DWORD flags;	/*< ����� ���������. ���������� � MSDN ������ ���������� CompareString()		*/ 
	DWORD locale;	/*< ������ ���������. ���������� � MSDN ������ ���������� CompareString()	*/
};



/** ��������� ��������� ������ ��� ��������� ��������� LVM_SORTLIST (��. MSDN) */
INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);


/** ���������� ���� ���������� �����
HWND hDlg - ���������� �������� ����.
UINT itemId - ������������� ComboBox �� ������� ������.
TCHAR* path - ���� ���������� �����. */
BOOL getPathOfSelectedDisk (HWND hDlg, UINT itemId, TCHAR* path);


/** ���������� ��� ������ ����� �� ������ */
INT_PTR onDiskList (HWND hDlg, WPARAM wParam, LPARAM lParam);


///** ��������� ���������� ����� � ������ � �������� ����
//HWND hDlg - ���������� �������
//UINT fullPathId - ������������� ������� ����
//UINT panelId - ������������� ������ */
//DWORD appendCurrentPath	(HWND hLv);


/** ��������� ������ ������
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� �������� */
VOID fillDiskList (HWND hDlg, UINT itemId);


/** ���������� ��� ������� �� ��������� ListView
HWND hDlg - ���������� �������
WPARAM wParam - ���������� ListView
LPARAM lParam - ��������� �� ��������� NMLISTVIEW */
BOOL onColumnClick (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** ������� � ListView ������ ����� � ����� � ����������
HWND hLv - ���������� ListView */
BOOL listDirectoryToPanel (HWND hLv);


/** ��������� ������. ��������� List View ���������� ���������� */
DWORD WINAPI listDirectoryToPanelWorker (VOID* data);


/** ��������� ���� � ������
HWND hDlg - ���������� �������� �������
TCHAR* path - ����� � ������� ��������� ����
WIN32_FIND_DATA* findData - �������� ���������� � ����������� ����� */
DWORD addFileToListView (HWND hLv, TCHAR* path, WIN32_FIND_DATA* findData);


/** ��������� �������� 
HWND hLv - ���������� ������
TCHAR* path - ���� � ����� */
VOID addFilesIconsToListView (HINSTANCE hInst, HWND hLv, TCHAR* path);


/** ���������� ����� ���������� �������� "�������� ����������"
HWND hDlg - ���������� �������� ������� */
INT_PTR onListFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** ������� �������� ��������� ���� ��� ����� �� ����������
HWND hLv - ���������� ������ */
BOOL executeSelected (HWND hLv);


/** ������������� ������� ����
HWND hDlg - ���������� ������� */
//VOID setCurrentPath	(HWND hDlg, UINT diskListId, UINT fullPathId, UINT panelId);


#endif // _BROWSE_H_