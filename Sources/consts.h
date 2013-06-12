/** ����� ���� Fileman.
������� ��������� ���������, 2009 �, ��������.
���������  */

#pragma once

#define FM_DIRTYPE_STUB				L"[DIR]"				/*< �������� ��� ���� �������� "�����"		*/

#define FM_CAPTION					TEXT("Fileman")				/*< ��������� ���������						*/
#define FM_COPY_DLG_CAPTION			TEXT("�����������")			/*< ��������� ������� �����������			*/
#define FM_MOVE_DLG_CAPTION			TEXT("�����������")			/*< ��������� ������� �����������			*/
#define FM_UNCKNOWN_DLG_CAPTION		TEXT("����������� ��������")	/*< ��������� ������� ����������� ��������	*/

//#define FM_PN_


#define FM_ITEM_COUNTING	1					/*< ������ ����� �����������		*/
#define FM_ITEM_COUNTED		2					/*< ������ ����� ��������			*/


/** ��������� ������� */
#define FM_COLUMN_NAME		0					/*< �������� ����� ��� ����������	*/
#define FM_COLUMN_TYPE		1					/*< ��� ����� �� ����������			*/
#define FM_COLUMN_DATE		2					/*< ���� �����						*/
#define FM_COLUMN_SIZE		3					/*< ������ �����					*/
#define FM_COLUMN_COUNT		4					/*< ���������� �������				*/


/** �������� ������������ ���������� appendCurrentPath () */
#define FM_ACP_NOTDIR		0					/*< ������� �� �����				*/
#define FM_ACP_COUNT		3					/*< ��� ������� ������� �����		*/
#define FM_ACP_NOSEL		4					/*< ������� �� ������				*/
#define FM_ACP_ERR			(0xffffffff)		/*< �������� ������					*/


/** �������� ������������ ���������� addFileToListView () */
#define FM_AFTLV_OK			0					/*< ����/����������� �������� � ������		*/
#define FM_AFTLV_NO			1					/*< ����/����������� �� �������� � ������	*/
#define FM_AFTLV_ERR		(0xffffffff)		/*< �������� ������							*/


/** ��������� ������������ � ��������� recurseOperation(...)  */

/** ���� �������� */
#define FM_OPERATION_COPY			1			/*< �������� �����������	*/
#define FM_OPERATION_DELETE			2			/*< �������� ��������		*/
#define FM_OPERATION_MOVE			3			/*< �������� �����������	*/
#define FM_OPERATION_COUNT			4			/*< �������� ��������		*/
#define FM_OPERATION_SERCH			5			/*< �������� ������			*/

/** ���� ���������  */
#define	FM_COPY_FINISHED		(WM_USER + 1)	/*< �������� ����������� ���������	*/
#define	FM_DELETE_FINISHED		(WM_USER + 2)	/*< �������� �������� ���������		*/
#define	FM_MOVE_FINISHED		(WM_USER + 3)	/*< �������� ����������� ���������	*/
#define FM_COUNT_FINISHED		(WM_USER + 4)	/*< �������� �������� ���������		*/
#define FM_COUNT_PROGRESS		(WM_USER + 5)	/*< �������� �������� ������������	*/
#define FM_LIST_FINISHED		(WM_USER + 6)	/*< �������� �������� ���������. WPARAM - �� ������������. LPARAM - �� ������������ */
#define FM_LIST_BEGIN		    (WM_USER + 7)	/*< �������� �������� ������		*/
#define WM_USER_MEDIACHANGED	(WM_USER + 88)	/*< */

#define FM_MAX_PATH					32767		/*< ������������ ������ � ������ ������� ����� ����� � UNICODE-������ �������� MSDN */
#define FM_MAX_INDENT				7708		/*< ������������� ������� ����������� ����� */

#define FM_COUNT_UPDATE_RATE		500			/*< ����� ���������� ������� ����� � ��			*/
#define FM_UPDATE_TIMER				133			/*< ������ ������������ ������� ����������, ��	*/
#define FM_DIRCHANGE_TIMER			550

/** ���� ����������� */
#define FM_IC_FIRST				1				/*< */
#define	FM_IC_UNCKNOWN			1				/*< ����������					*/
#define	FM_IC_DISK				2				/*< Ƹ����� ����				*/
#define	FM_IC_CDA				3				/*< Audio CD					*/
#define	FM_IC_NET				4				/*< ���� Microsft Windows		*/
#define	FM_IC_NFS				5				/*< ���� NFS Linux				*/
#define	FM_IC_FTP				6				/*< FTP							*/
#define	FM_IC_SHELL				7				/*< ������� ��������			*/
#define FM_IC_LAST				7				/*< */
#define	FM_IC_ANY				(0xffffffff)	/*< ����� ���					*/

/** ����������� ������ �������  */
static int minColumnWidths [FM_COLUMN_COUNT] = {100, 40, 100, 50};


volatile static ULONG m_ulSHChangeNotifyRegister = NULL;
