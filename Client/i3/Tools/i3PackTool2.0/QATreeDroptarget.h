// TreeDropTarget.h
// For OLE Drag and Drop between tree controls
// Designed and developed by Vinayak Tadas
// vinayakt@aditi.com
// 

#if !defined(AFX_QATREEDROPTARGET_H__246241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_)
#define AFX_QATREEDROPTARGET_H__246241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeDropTarget.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeDropTarget

#include "TreeDroptarget.h"

class CDragDropTreeCtrl;
class CQATreeDropTarget :public CTreeDropTarget	
{
	// Overrides
public:
	virtual DROPEFFECT	OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT	OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual void		OnDragLeave(CWnd* pWnd);
	virtual BOOL		OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};
/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QATREEDROPTARGET_H__246241C3_897C_11D3_A59E_00A02411D21E__INCLUDED_)
