/* ����� ���� Fileman.
������� ��������� ���������, 2009 �, ��������.
��������� �������� ��� �������� "�����������" */

#ifndef _COPY_H_
#define _COPY_H_

#include "build.h"

#include <commctrl.h>

#include "helper.h"
#include "resource.h"
#include "browse.h"
#include "user.h"
#include "task.h"
#include "progress.h"
#include "operation.h"

typedef struct _ProgrBarUserData
{
	WNDPROC defWndProc;
	TCHAR* text;
	HFONT hFont;
} ProgrBarUserData;


// ����������� ���������
INT CALLBACK copyDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT CALLBACK progrBarWndProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI copyFileWorker (VOID* data);

// ��������� ��������� ��������� ����
INT_PTR onCopyDialogCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ����������� ���������� ������� �����������
HWND hDlg - ���������� ������� �����������
LPARAM lParam - ��������� �� ��������� ������ CopyFileData */
INT_PTR onCopyDialogFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ���������� ������� ������ ����� ������� ����������� */
INT_PTR onCopyPauseToggle (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** */
INT_PTR onCopyDlgInit (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ������ ����������� � ���������� � ����������� ����� */
INT_PTR onProgrBarPaint (HWND hDlg, WPARAM wParam, LPARAM lParam);

/**  */
INT_PTR onProgrBarSetText (HWND hDlg, WPARAM wParam, LPARAM lParam);

/**  */
INT_PTR onCopyDlgDestroy (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** �������� ���������� ����� */
BOOL copySelected (HWND hDlg, UINT fullPathIdSrc, UINT panelIdSrc, UINT fullPathIdDst, UINT panelIdDst, INT opCode);

/** �������� Drug and Drop ����� */
BOOL copyDropped (HWND hDlg, Task* task);



#endif // _COPY_H_
