// ����� ���� Fileman.
// ������� ��������� ���������, 2009 �, ��������.

#ifndef _LTPD_H_
#define _LTPD_H_


#include "build.h"

#include "consts.h"
#include "fmmemory.h"

/** ����� ��� �������� ������� "���������� ���������� � ������" */
class LTPD
{
public:
	HWND hDlg;						/*< ���������� �������� �������		*/
	HWND hLv;						/*< ���������� ������				*/
	TCHAR* path;					/*< ���� � ����������				*/
	LTPD (TCHAR* path);				/*< �����������						*/
	LTPD (VOID);					/*< �����������						*/
	SIZE_T getPathAllocSize (VOID);
	~ LTPD ();
};

#endif // _LTPD_H_