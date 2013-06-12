// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.
// ��������� ��������������� ��������

#ifndef _helper_h_
#define _helper_h_

#include "build.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <memory.h>
#include <psapi.h>
#include <string.h>
#include <wchar.h>
#include <tchar.h>

#include "consts.h"


#define SIZETC(arg) (sizeof(arg)/sizeof(arg[0]))


int multiByteToWideChar (TCHAR* wcStr, char* mbStr, UINT codePage = CP_ACP);


/** ���������� �������� �� ������ ������ ������ � ������
TCHAR* str - ��������� �� ������
TCHAR rchar - ������ */
TCHAR* lstrrchr (TCHAR* str, TCHAR rchar);


/** ����������� ������ ������ �� ������� ��������� �������
TCHAR* str - ������������� ������
TCHAR ch - �������� ������
���������� ��������� �� ����� �������� ������ � ������,
���� ����� ��� ������, ��� NULL ���� ������ ������� ��� � ������ */
TCHAR* lcutrchr (TCHAR* str, TCHAR rchar);


/** ��������� ������ ����� ����� � ����� ������ � ����,
���� ������ ������� ��� � ����� ������
TCHAR* path - ������ � ����
���������� TRUE ���� ������ path ���� ��������,
FALSE ���� �� ���� �������� */
BOOL addSlashToPath (TCHAR* path);


/** ������� ������ ����� ����� � ����� ������ � ����
TCHAR* path - ������ � ����
���������� TRUE ���� ������ path ���� ��������, FALSE ���� �� ���� �������� */
BOOL delSlashFromPath (TCHAR* path);


/** ������� ��������������� ������ � Windows Debug output */
VOID wprintfd (TCHAR* format, ...);	


/** ������� ��������������� ������ � Windows Debug output */
VOID printfd (char* format, ...);


/** ���������� ��������� ������� �� ������
HWND hDlg - ���������� �������
DWORD err - ��� ������ 
TCHAR* caption - ��������� ��������� */
void showErrorMessage (HWND hDlg, DWORD err, TCHAR* caption);


/** ��������� ������ �� ��������
HWND hDlg - ���������� �������
UINT strId - ������������� ������
TCHAR* destStr - �������� �����
size_t strLen - ������ �������� ������ */
BOOL loadStringFromResource (HWND hDlg, UINT strId, TCHAR* destStr, SIZE_T strLen);


HLOCAL fmLocalFree (HLOCAL hMem);
HLOCAL fmLocalAlloc (SIZE_T len);
SIZE_T fmLocalSize (HLOCAL hMem);


/** ���������� ��� ����� �� ��� �����������
HANDLE hFile - ���������� �����
TCHAR* pszFilename - ��������� �� �����, � ������� ����� �������� ��� �����
SIZE_T fileNameLen - ������ ������ pszFilename */
BOOL getFileNameFromHandle (HANDLE hFile, TCHAR* pszFilename, SIZE_T fileNameLen);


/** �������� ���� � ����������� ����. ���� ������ ���� � ����������,
�� ��������� ������ L'\\' � ����� ������, ���� ������ ���� � �����,
�� ������� ������ L'\\' � ����� ������, ���� ������ L'\\' ����
TCHAR* path - ������ � ���� */
DWORD normalizePath	(TCHAR* path);


/** �������� ���� �� ���� ������� ���� (�������� "cd ..")
TCHAR* path - ����.
���������� TRUE, ���� ���� ��� �������, FALSE ���� ���� �� ��� ������� */
BOOL cutPath (TCHAR* path);

/** ��������� ���� �� ���� ������� ����
TCHAR* curPath - ������� ����.
TCHAR* addPath - ����������� ����.
���������� TRUE, ���� ���� ��� ��������, FALSE ���� ���� �� ��� �������� */
DWORD addPath (TCHAR* curPath, TCHAR* addPath);

/** ���������� ���-��� ������
TCHAR* str - ��������� �� ������ */
DWORD getHash (TCHAR* str);



BOOL fmGetDiskFreeSpace (TCHAR* pszDrive, ULARGE_INTEGER* i64FreeBytes);


#endif // _helper_h_