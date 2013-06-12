#ifndef _FM_SORT_H_
#define _FM_SORT_H_

#include "build.h"

#include <commctrl.h>
#include <shlwapi.h>
#include "consts.h"

// ������ ��� ���������� ����� � ListView
struct FmSort
{
	DWORD column;		// ������ ������� �� ������� ����������� ������
	BOOL dir;			// ���������� ����������: TRUE - �� �����������, FALSE - �� ��������
	BOOL separate;		// ���������� ���������� � ������: TRUE - ��������, FALSE - ������
	HWND hListView;		// ���������� ListView
	DWORD flags;		// ����� ���������. ���������� � MSDN ������ ���������� CompareString() 
	DWORD locale;		// ������ ���������. ���������� � MSDN ������ ���������� CompareString()
};

/* ��������� ��������� ������ ��� ��������� ��������� LVM_SORTLIST (��. MSDN)*/
INT CALLBACK sortListView (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#endif // _FM_SORT_H_