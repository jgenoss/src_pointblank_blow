#include "StdAfx.h"
#include "WeaponAnimation.h"
#include "ListResourceFile.h"
#include "DlgResourceControl.h"

IMPLEMENT_DYNAMIC(cListResourceFile, CColoredListCtrl)

BEGIN_MESSAGE_MAP(cListResourceFile, CColoredListCtrl)
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &cDlgResourceControl::OnHdnItemdblclickListFiles)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_SIZE()
END_MESSAGE_MAP()

cListResourceFile::cListResourceFile()
{

}

void cListResourceFile::DoDataExchange(CDataExchange* pDX)
{
	CListCtrl::DoDataExchange(pDX);
}

void cListResourceFile::Init()
{
	InsertColumn (0,_T("File"),LVCFMT_CENTER);
	//InsertColumn (1,_T("Date"),LVCFMT_LEFT);
	CRect r;
	GetClientRect(r);
	SetColumnWidth (0,r.right);
}

void cListResourceFile::UpdateAllRes()
{

}

void cListResourceFile::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);
}

void cListResourceFile::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = lplvcd->nmcd.dwItemSpec;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT :
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}

		// Modify item text and or background
	case CDDS_ITEMPREPAINT:
		{
			lplvcd->clrText = RGB(0,0,0);
			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}

		// Modify sub item text and/or background
	case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{

			if(iRow %2){
				lplvcd->clrTextBk = m_colRow2;
			}
			else{
				lplvcd->clrTextBk = m_colRow1;
			}


			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

BOOL cListResourceFile::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	CColoredListCtrl::GetClientRect(rect);

	POINT mypoint;  

	CBrush brush0(m_colRow1);
	CBrush brush1(m_colRow2);

	int chunk_height=GetCountPerPage();
	pDC->FillRect(&rect,&brush1);

	for (int i=0;i<=chunk_height;i++)
	{
		GetItemPosition(i,&mypoint);
		rect.top=mypoint.y ;
		GetItemPosition(i+1,&mypoint);
		rect.bottom =mypoint.y;
		pDC->FillRect(&rect,i %2 ? &brush1 : &brush0);
	}

	brush0.DeleteObject();
	brush1.DeleteObject();

	return FALSE;
}

void cListResourceFile::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	CRect r;
	GetClientRect(r);
	SetColumnWidth (0,r.right);
	//SetColumnWidth (1,r.right /2);
}

void cListResourceFile::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	Invalidate();  

// 	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE) pNMHDR;  
// 	int m_nItem = pNMITEM->iItem;  
// 	int m_nSubItem = pNMITEM->iSubItem;  
// 
// 	i3AI* pCurrAI		= g_pChara->getAI(0)->getAI();
// 	i3AIState* pState	= pCurrAI->getAIState(m_nItem);
// 
// 	AI* pAI = g_pChara->getAI(0);
// 	if (pAI)
// 	{
// 		pAI->getAIContext()->removeGNodeState( I3_GAMENODE_STATE_LOCKED);
// 		pAI->getAIContext()->setCurrentAIState( pState);
// 		pAI->getAIContext()->addGNodeState( I3_GAMENODE_STATE_LOCKED);
// 		pAI->OnSelect( true);
// 	}
// 
// 	::SendMessage(g_HwndWeaponProp, WM_TDK_UPDATE, (WPARAM) m_nItem, (LPARAM)m_nSubItem);
// 
// 	I3_TDK_UPDATE_INFO Data;
// 	Data.m_Event = I3_TDK_UPDATE_ALL;
// 	::SendMessage(g_HwndDlgTimeTrack, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)m_nItem);

}