#include "stdafx.h"
#include "QATreeDroptarget.h"
#include "DragDropTreeCtrl.h"

DROPEFFECT CQATreeDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CTreeDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}
		
DROPEFFECT CQATreeDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropeffectRet = DROPEFFECT_COPY;
	if ((dwKeyState & MK_SHIFT) == MK_SHIFT)
		dropeffectRet = DROPEFFECT_MOVE;

	m_pDestTreeCtrl = (CDragDropTreeCtrl *)pWnd;
	m_pSourceTreeCtrl = (CDragDropTreeCtrl *)CWnd::FromHandlePermanent(m_shWndTreeCtrl);

	if (m_pDestTreeCtrl != m_pSourceTreeCtrl) 
		dropeffectRet = DROPEFFECT_NONE;
	
	return dropeffectRet;
}

void CQATreeDropTarget::OnDragLeave(CWnd* pWnd)
{	
	CTreeDropTarget::OnDragLeave(pWnd);
}

BOOL CQATreeDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	return TRUE;
}