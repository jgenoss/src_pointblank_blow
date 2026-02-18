#include "StdAfx.h"
#include "ListAIDriver.h"
#include "AIDriverManager.h"

IMPLEMENT_DYNAMIC(cListAIDriver, CColoredListCtrl)

BEGIN_MESSAGE_MAP(cListAIDriver, CColoredListCtrl)
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_SIZE()
END_MESSAGE_MAP()

cListAIDriver::cListAIDriver()
{

}

void cListAIDriver::DoDataExchange(CDataExchange* pDX)
{
	CListCtrl::DoDataExchange(pDX);
}

void cListAIDriver::Init(const std::string& strColumName)
{
	//클릭 이벤트의 처리를 위해 이름을 저장해두자
	m_strColumName = strColumName;

	InsertColumn (0,_T(strColumName.c_str()),LVCFMT_CENTER);
	//InsertColumn (1,_T("WeaponList"),LVCFMT_CENTER);
	//InsertColumn (2,_T("TableList"),LVCFMT_CENTER);
	
	CRect r;
	GetClientRect(r);
	SetColumnWidth (0,r.right);
}

void cListAIDriver::UpdateAllRes()
{

}

void cListAIDriver::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);
}

void cListAIDriver::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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

BOOL cListAIDriver::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CColoredListCtrl::OnEraseBkgnd(pDC);

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

void cListAIDriver::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

// 	CRect r;
// 	GetClientRect(r);
// 	SetColumnWidth (0,r.right/3);
// 	SetColumnWidth (1,r.right/3);
// 	SetColumnWidth (1,r.right/3);
}

void cListAIDriver::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	Invalidate();

	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE) pNMHDR;  
	int m_nItem = pNMITEM->iItem;

	//cDlgAIDriver::OnInitDialog에 지정된 이름

	if (m_strColumName == "AIDriver List")
	{
		::SendMessage(g_HwndDlgAIDriver, WM_TDK_UPDATE, (WPARAM) 0, (LPARAM)m_nItem);
	}

	if (m_strColumName == "Weapon List")
	{
		::SendMessage(g_HwndDlgAIDriver, WM_TDK_UPDATE, (WPARAM) 1, (LPARAM)m_nItem);
	}

	if (m_strColumName == "Table List")
	{
		::SendMessage(g_HwndDlgAIDriver, WM_TDK_UPDATE, (WPARAM) 2, (LPARAM)m_nItem);
	}

	if (m_strColumName == "AI Enum")
	{
		::SendMessage(g_HwndDlgAIEnumList, WM_TDK_UPDATE, (WPARAM) 0, (LPARAM)m_nItem);
	}
}