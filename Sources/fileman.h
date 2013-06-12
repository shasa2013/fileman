// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.
// ��������� ��������

#ifndef _fileMan_h_
#define _fileMan_h_

#include "build.h"

#include <commctrl.h>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include <Commdlg.h>

#include "helper.h"
#include "resource.h"
#include "copy.h"
#include "user.h"
#include "browse.h"
#include "task.h"
#include "watcher.h"
#include "delete.h"
#include "move.h"
#include "count.h"
#include "context.h"
#include "consts.h"
#include "folder.h"
#include "settings.h"


typedef struct _ColRGB
{
	unsigned char a;
	unsigned char b;
	unsigned char g;
	unsigned char r;
} ColRGB;


typedef struct
{
    DWORD dwItem1;    // dwItem1 contains the previous PIDL or name of the folder. 
    DWORD dwItem2;    // dwItem2 contains the new PIDL or name of the folder. 
} SHNOTIFYSTRUCT;



/** ����������� ��������� ���� � ����������� ��������� - ������ � ����
���������� ����� ������������ ��������� ��������������� ���������
��������, ����������� � resource.h */

/** */
VOID setColumnsWidths (HWND hLv, UINT width);


/** ���� ��������� �������� ���� */
INT CALLBACK mainDlgProc (HWND hDlg, UINT msg,  WPARAM wParam, LPARAM lParam);

/** ���� ��������� ������ */
INT CALLBACK subClassLvProc (HWND hLv, UINT Message, WPARAM wParam, LONG lParam);

/** ���������� ��������� ���� "������������� �������"
HWND hDlg - ���������� �������� ���� */
INT_PTR onDialogInit (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR onDialogNotify (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� "��������� ��������"
HWND hDlg - ���������� �������� �������
WPARAM wParam, LPARAM lParam - ��������� ��������� WM_SIZE */
INT_PTR onDialogSize (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ���������� ������� ����������� ��������� �����������
HWND hDlg - ���������� �������� ���� */
INT_PTR onDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR onIdOk (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR onSetBkColor (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR onTimer (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR	onItemChanged (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR	onNewFolder (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR	onDropFiles (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR	onListViewEndTrack (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� */
INT_PTR	onListViewTrack (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ������ ��������� ���� WM_USER_MEDIACHANGED
LPARAM lParam - contains the event SHCNE_xxxx
WPARAM wParam - supplies the SHNOTIFYSTRUCT */
INT_PTR	onMediaChanged (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ��������� ���� WM_DEVICECHANGED
LPARAM lParam - contains the event SHCNE_xxxx
WPARAM wParam - supplies the SHNOTIFYSTRUCT */
INT_PTR	onDeviceChange (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ������ ��������� */

/** ������������� ����� ��������� ����
HWND hDlg - ���������� �������� ����
UINT itemId - ������������� ��������
int X, int Y - ����� ��������� X, Y
int cx, int cy - ������ X, Y */
BOOL setDlgItemPos (HWND hDlg, UINT itemId, int X, int Y, int cx, int cy);

LONG onCustomDrawPanel (HWND hDlg, LPARAM lParam);

/** ���������� ������� ����� ���������� */
BOOL onDirChangeHandler (HWND hLv);

HLOCAL fmLocalFree (HLOCAL hMem);
HLOCAL fmLocalAlloc (SIZE_T len);
SIZE_T fmLocalSize (HLOCAL hMem);



#endif // _fileMan_h_
