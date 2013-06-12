// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.
// ��������� �������� ��� �������� "������� ����� �����"

#ifndef _FOLDER_H_
#define _FOLDER_H_

#include "build.h"

#include <commctrl.h>
#include "resource.h"
#include "consts.h"
#include "helper.h"

INT CALLBACK newFolderDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR onNewFolderOk (HWND hDlg, WPARAM wParam, LPARAM lParam);
INT_PTR onNewFolderCancel (HWND hDlg, WPARAM wParam, LPARAM lParam);

/** ������ ������ ��� ����� ��������� �����
HWND hDlg - ���������� �������� �������
UINT fullPathId - ���������� ������� ���� */
BOOL createFolder (HWND hDlg, HWND hfullPath);

#endif //_FOLDER_H_