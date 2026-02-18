#ifndef _CTV_H_
#define _CTV_H_

//출처
// http://hebhy.cublip.com/216
// http://www.devpia.com/MAEUL/Contents/Detail.aspx?BoardID=51&MAEULNO=20&no=4261

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

#define ID_TIMER_AUTOEXPAND     1000        // 오토 익스팬드를 위한 타이머 ID이자 시간
#define ID_TIMER_AUTOSCROLL     50            // 오토 스크롤을 위한 타이머 ID이자 시간


typedef class CTV {

public:
	static HTREEITEM    DragItem(CTreeCtrl* pTree, HTREEITEM hDragItem, CPoint point);
	static HTREEITEM    CopyItem(CTreeCtrl* pTree, HTREEITEM hSrc, HTREEITEM hDestParent = TVI_ROOT, HTREEITEM hCopyAfter = TVI_LAST);
	static void			DeleteItem(CTreeCtrl* pTree, HTREEITEM hItem);
} *PCTV, **LPCTV;

#endif