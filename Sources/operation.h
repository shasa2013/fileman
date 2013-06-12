#ifndef _OPERATION_H_
#define _OPERATION_H_


#include "build.h"

#include <stdlib.h>
#include "progress.h"
#include "consts.h"


/** ���������� TRUE ���� ������� ����� �������� ��� ���������
������� ����������
HWND hDlg - ���������� ������� �������� */
BOOL isOperationThreadExist (HWND hDlg);


/** ��������� ���������� ������ �����������
HWND hDlg - ���������� ������� �������� ��������
UINT itemId - ������������� ������� �����������
HANDLE hOperThread - ���������� ������ ����������� */
BOOL setOperationThread (HWND hDlg, HANDLE hThread, BOOL isSuspended);


/** ���������� TRUE, ���� ��� ������� ����������� ����������
HWND hDlg - ���������� ������� �������� ��������,
UINT itemId - ������������� ������� �������� */
BOOL isThreadExist (HWND hDlg);


/** ������� ���������� ������ �����������
HWND hDlg - ���������� ������� �������� ��������
���������� - TRUE ���� ��� ���������� ������, ����� ����������� ��� �����,
FALSE - ���� ��� ���������� ������, ��� ������ ����������� */
BOOL removeOperationThread (HWND hDlg);


/** ���������� ��������� ������� �����������
HWND hOperDlg - ���������� ������� ��������� ��������
UINT itemId - ������������� ������� ��������� �������� */
BOOL isOperationThreadSuspended (HWND hDlg);


/** ������������ ����� ��������
HWND hDlg - ���������� ������� ��������� ��������,
UINT itemId - ������������� ������� �������� */
BOOL resumeOperationThread (HWND hDlg);


/** ���������������� ����� ��������
HWND hDlg - ���������� ������� ��������� ��������,
UINT itemId - ������������� ������� �������� */
BOOL suspendOperationThread (HWND hDlg);


/** ��������� ����������� �������� ��� ������
WIN32_FIND_DATA* findData - ��������� �� ��������� WIN32_FIND_DATA
TCHAR* beginFolder - ��������� ���� ��������
TCHAR* dstFolder - ��������� ����� ���� ���������� (��� ��������� �������� ����� ���� NULL)
LPPROGRESS_ROUTINE routine - ��������� �� ��������� ����������� ���� ��������
VOID* arg - �������� ������������ � ��������� ����������� ���� ��������
DWORD flags - ����� ��� ����������� ��������
DWORD operation - ����������� ��������, ���������� � consts.h
UINT64* bytesCount - ��������� ������ ������. ������������ � FM_OPERATION_COUNT */
INT recurseOperation (WIN32_FIND_DATA* findData, TCHAR* beginPath, TCHAR* dstFolder,
	LPPROGRESS_ROUTINE routine, VOID* arg, DWORD flags, DWORD operation, UINT64* bytesCount);



#endif // _OPERATION_H_
