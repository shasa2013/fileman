#ifndef _panels_h_
#define _panels_h_

#define PANEL_COLUMN_NAME		1
#define PANEL_COLUMN_TYPE		2
#define PANEL_COLUMN_SIZE		3
#define PANEL_COLUMN_DATE		4
#define PANEL_COLUMN_OTHER		5
#define PANEL_COLUMN_MAX_COUNT  5


typedef struct _PanelColumns
{
	ColumnsOrder [PANEL_COLUMN_MAX_COUNT];
} PanelColumns;


#endif
