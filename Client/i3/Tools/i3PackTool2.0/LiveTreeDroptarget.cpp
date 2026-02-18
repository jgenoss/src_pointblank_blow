#include "stdafx.h"
#include "LiveTreeDroptarget.h"
#include "DragDropTreeCtrl.h"
#include "PTFile.h"
#include "PTTreeworkLive.h"
#include "PTTreeworkQA.h"
#include "PTStorage.h"
#include "PTTreeworkMediator.h"

#define RECT_BORDER	10

namespace
{
	bool ValidateFileTransferQAToLive(const char* destpath, CTreeCtrl* liveTree, CTreeCtrl* qaTree)
	{
		PTFile* qaFileSelected = 
			reinterpret_cast<PTFile*>(qaTree->GetItemData(qaTree->GetSelectedItem()));

		PTFile* qaRootFile 
			= reinterpret_cast<PTFile*>(qaTree->GetItemData(qaTree->GetRootItem()));
		const char* workdirQA = qaRootFile->GetFullpath();

		PTFile* liveRootFile 
			= reinterpret_cast<PTFile*>(liveTree->GetItemData(liveTree->GetRootItem()));
		const char* workdirLive = liveRootFile->GetFullpath();

		i3::string target(destpath);
		target += "\\";
		target += qaFileSelected->GetFileName();

		i3::string result(workdirQA);
		result += (TreeUtil::PathExceptWorkdir(target.c_str(), workdirLive)).c_str();

		return (TheMediator().GetTreeworkQA()->GetStorage()->Find(result.c_str()) ? true : false);
	}
}

DROPEFFECT CLiveTreeDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CTreeDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CLiveTreeDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropeffectRet = DROPEFFECT_COPY;
	if ((dwKeyState & MK_SHIFT) == MK_SHIFT)
		dropeffectRet = DROPEFFECT_MOVE;

	// Expand and highlight the item under the mouse and 
	m_pDestTreeCtrl = (CDragDropTreeCtrl*)pWnd;
	m_pSourceTreeCtrl = (CDragDropTreeCtrl*)CWnd::FromHandlePermanent(m_shWndTreeCtrl);

	if (m_pDestTreeCtrl == m_pSourceTreeCtrl) 
		return dropeffectRet; // QA 트리 내 파일 이동은 없다. 지우근.

	HTREEITEM hTItem = m_pDestTreeCtrl->HitTest(point);
	if (hTItem != NULL) 
	{
		m_pDestTreeCtrl->SelectDropTarget(hTItem);
		dropeffectRet = DROPEFFECT_COPY;
	}	
	
	// Scroll Tree control depending on mouse position
	CRect rectClient;
	pWnd->GetClientRect(&rectClient);
	pWnd->ClientToScreen(rectClient);
	pWnd->ClientToScreen(&point);
	int nScrollDir = -1;
	if (point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ((point.y <= rectClient.top + RECT_BORDER))
		nScrollDir = SB_LINEUP;

	if (nScrollDir != -1) 
	{
		int nScrollPos = pWnd->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pWnd->SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if (point.x <= rectClient.left + RECT_BORDER)
		nScrollDir = SB_LINELEFT;
	else
	if (point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if (nScrollDir != -1)
	{
		int nScrollPos = pWnd->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pWnd->SendMessage(WM_HSCROLL, wParam);
	}
	
	return dropeffectRet;
}

void CLiveTreeDropTarget::OnDragLeave(CWnd* pWnd )
{	
	CTreeDropTarget::OnDragLeave(pWnd);
}

BOOL CLiveTreeDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	//Get the selected item from Source and destination Tree controls 
	m_pSourceTreeCtrl = (CDragDropTreeCtrl*)CWnd::FromHandlePermanent(m_shWndTreeCtrl);
	m_pDestTreeCtrl = (CDragDropTreeCtrl*)pWnd;

	if (m_pDestTreeCtrl == m_pSourceTreeCtrl) 
		return TRUE;

	HTREEITEM hSelectedItem = m_pSourceTreeCtrl->GetSelectedItem();
	HTREEITEM hTDropItem = m_pDestTreeCtrl->GetDropHilightItem();
	
	// Get the image indexes from ImageList, if one exists
	int nImage = 0;
	int nSelImage = 0;
	GetItemImages(hSelectedItem, nImage, nSelImage);
	
	// Add the items to destination tree control

	// 우선 파일 하나씩 추가할 수 있게만 하자. 폴더 추가는 나중에.
	PTFile* fileSelected = reinterpret_cast<PTFile*>(m_pSourceTreeCtrl->GetItemData(hSelectedItem));
	m_pDestTreeCtrl->SelectItem(hTDropItem);
	m_hDestItem = m_pDestTreeCtrl->AddItem(hTDropItem, fileSelected, TVI_LAST, nImage, nSelImage, 0);
	//if (m_hDestItem)
	//	m_pDestTreeCtrl->AddItem(m_pSourceTreeCtrl->GetChildItem(hSelectedItem),m_hDestItem);
	
	//Remove highlighting
	m_pDestTreeCtrl->SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE, 0);
	return TRUE;
}