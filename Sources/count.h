// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.
// ��������� �������� ��� �������� "�������"

#ifndef _COUNT_H_
#define _COUNT_H_

#include "build.h"
#include <shlwapi.h>

#include "task.h"
#include "operation.h"
#include "globals.h"

/** ��������� ����� �������� ���������� ������� ������ */
BOOL countSelected (HWND hDlg);


/** ��������� �������� ������. ������������ ��������� ������ ������ */
DWORD WINAPI countWorker (VOID* data);


/** ���������� ����� ������� ������� ������ ��������
HWND hDlg - ���������� �������� �������
PARAM wParam - �� ������������
LPARAM lParam - ��������� �� ����� Task */
INT_PTR onCountFinished (HWND hDlg, WPARAM wParam, LPARAM lParam);


/** ���������� �� ����� �������� ���������� �������
������ ��� ����������� ��������� ����������
HWND hDlg - ���������� �������� �������
WPARAM wParam - ������������� ������
LPARAM lParam - ��������� �� ���������� ���� DWORDLONG
� ������� ��������� �������� ������ */
INT_PTR onCountProgress (HWND hDlg, WPARAM wParam, LPARAM lParam);


#endif // _COUNT_H_