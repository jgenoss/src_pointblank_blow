// UITimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "PaneTemplateView.h"
#include "MainFrm.h"


// CPaneTemplateView

IMPLEMENT_DYNAMIC(CPaneTemplateView, CDockablePane)

CPaneTemplateView::CPaneTemplateView()
{
	m_iCurrentTemplate = -1;
	m_pClippedTemplate = NULL;
}

CPaneTemplateView::~CPaneTemplateView()
{	
}

void CPaneTemplateView::AddTemplate( void)
{
	i3GICShapeTemplate * pTemplate = i3GICShapeTemplate::NewObjectRef();
	pTemplate->Create( I3STR("New Template"));

	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	m_iCurrentTemplate = pMainFrame->getDiagram()->getTemplateCount();

	pMainFrame->getDiagram()->addTemplate( pTemplate);
	pMainFrame->getDesignView()->removeAllShape();

	pTemplate->EditMode( true);

	UpdateListCtrl();
}

void CPaneTemplateView::RemoveTemplate( INT32 idx)
{
	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	i3GICShapeTemplate * pTemplate = pMainFrame->getDiagram()->getTemplate( idx);
	pMainFrame->getDiagram()->removeTemplate( pTemplate);

	if( m_iCurrentTemplate == idx)
	{
		m_iCurrentTemplate = -1;

		// DesignView를 초기화해야 한다.
		pMainFrame->getDesignView()->removeAllShape();
	}

	UpdateListCtrl();
}

void CPaneTemplateView::RemoveAllTemplate( void)
{
	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	pMainFrame->getDiagram()->removeAllTemplate();

	m_iCurrentTemplate = -1;
	pMainFrame->getDesignView()->removeAllShape();

	UpdateListCtrl();
}

void CPaneTemplateView::PasteTemplate( void)
{
	if( m_pClippedTemplate == NULL)
		return;

	i3GICShapeTemplate * pTemplate = i3GICShapeTemplate::NewObjectRef();
	pTemplate->Create( m_pClippedTemplate->GetName());

	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	m_iCurrentTemplate = pMainFrame->getDiagram()->getTemplateCount();

	pMainFrame->getDiagram()->addTemplate( pTemplate);
	pMainFrame->getDesignView()->removeAllShape();

	m_pClippedTemplate->getRoot()->CopyTo( pTemplate->getRoot(), I3_COPY_INSTANCE);

	pTemplate->EditMode( true);

	INT32 i;
	for( i = 0; i < pTemplate->getRoot()->getChildCount(); i++)
	{
		pMainFrame->getDesignView()->addShape( (i3GICShape*)pTemplate->getRoot()->getChild( i));
	}

	UpdateListCtrl();

}

void CPaneTemplateView::AttachShapeToTemplate( i3GICShape * pShape)
{
	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	i3GICShapeTemplate * pTemplate = pMainFrame->getDiagram()->getTemplate( m_iCurrentTemplate);

	if( pTemplate != NULL)
	{
		pTemplate->getRoot()->AddChild( pShape);
	}
}

void CPaneTemplateView::DetachShapeFromTemplate( i3GICShape * pShape)
{
	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	i3GICShapeTemplate * pTemplate = pMainFrame->getDiagram()->getTemplate( m_iCurrentTemplate);

	if( pTemplate != NULL)
	{
		pTemplate->getRoot()->RemoveChild( pShape);
	}
}

void CPaneTemplateView::UpdateListCtrl( void)
{
	INT32 i;

	m_TemplateListCtrl.DeleteAllItems();

	CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

	for( i = 0; i < pMainFrame->getDiagram()->getTemplateCount(); i++)
	{
		i3GICShapeTemplate * pTemplate = pMainFrame->getDiagram()->getTemplate( i);

		if( i == m_iCurrentTemplate)
			m_TemplateListCtrl.InsertItem( i, pTemplate->GetName(), I3TDK_IMAGELIST_ANIM);
		else
			m_TemplateListCtrl.InsertItem( i, pTemplate->GetName(), I3TDK_IMAGELIST_BLANK);
	}
}

void CPaneTemplateView::ProcessRButtonUp( UINT nFlags, CPoint point)
{
	CMenu menu, *pPopup;
	CPoint pt;
	UINT32 cmd;

	INT32 idx = -1;
	POSITION pos = m_TemplateListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
		idx = m_TemplateListCtrl.GetNextSelectedItem( pos);

	menu.LoadMenu( IDR_POPUP_TEMPLATE);

	pPopup = menu.GetSubMenu( 0);

	if( m_pClippedTemplate == NULL)
	{
		menu.EnableMenuItem( ID_TEMPLATE_PASTE, MF_BYCOMMAND | MF_GRAYED);
	}

	if( idx == -1)
	{
		menu.EnableMenuItem( ID_TEMPLATE_COPY, MF_BYCOMMAND | MF_GRAYED);
		menu.EnableMenuItem( ID_TEMPLATE_DELETE, MF_BYCOMMAND | MF_GRAYED);
	}

	GetCursorPos( &pt);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	if( cmd != 0)
	{
		switch( cmd)
		{
			case ID_TEMPLATE_NEW :		AddTemplate();		break;
			case ID_TEMPLATE_COPY :
				{	
					CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();
					m_pClippedTemplate =  pMainFrame->getDiagram()->getTemplate( idx);
				}
				break;
			case ID_TEMPLATE_PASTE :	PasteTemplate();	break;
			case ID_TEMPLATE_DELETE :
				if( idx != -1)	RemoveTemplate( idx);
				break;
		}
	}
}

BEGIN_MESSAGE_MAP(CPaneTemplateView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_NOTIFY( NM_DBLCLK, IDC_LIST_TEMPLATE, OnNMDBClickTemplateList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TEMPLATE, OnNMRClickTemplateList)
	ON_NOTIFY( LVN_ENDLABELEDIT, IDC_LIST_TEMPLATE, OnEndLabelEdit)
END_MESSAGE_MAP()


// CUIPaneTimeLine message handlers
int CPaneTemplateView::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	int rv = CDockablePane::OnCreate( lpCreateStruct);

	RECT rect;

	GetClientRect( &rect);

	if( m_TemplateListCtrl.Create( WS_CHILD | WS_VSCROLL | WS_VISIBLE | LVS_LIST | LVS_EDITLABELS, rect, this, IDC_LIST_TEMPLATE) == FALSE)
	{
		I3FATAL( "Create Template List fail.");
		return -1;
	}

	m_TemplateListCtrl.SetExtendedStyle( m_TemplateListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if( g_pTDKGlobalRes != NULL)
		m_TemplateListCtrl.SetImageList( g_pTDKGlobalRes->m_pTDKImageList, LVSIL_SMALL);


	return rv;
}

void CPaneTemplateView::OnPaint()
{	
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);
	
	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);

	UpdateListCtrl();
}

void CPaneTemplateView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu() != NULL)
	{
		return;
	}

	// default Popup 창을 사용하지 않도록 합니다.
}

void CPaneTemplateView::OnLButtonUp( UINT nFlags, CPoint point)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

	pMainWnd->LButtonUp( nFlags, point, false);

	CDockablePane::OnLButtonUp( nFlags, point);
}

void CPaneTemplateView::OnNMDBClickTemplateList(NMHDR *pNMHDR, LRESULT *pResult)
{
	SetFocus();
	INT32 idx = -1;
	POSITION pos = m_TemplateListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		idx = m_TemplateListCtrl.GetNextSelectedItem( pos);
		I3ASSERT( idx != -1);

		if( m_iCurrentTemplate != idx)
		{	// 기존에 DesignView에 있는 Shape들을 Diagram에서 삭제
			// 선택된 Template의 Shape으로 Diagram에 설정한다.
			CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();

			pMainFrame->getDesignView()->removeAllShape();

			INT32 i;

			i3GICShapeTemplate * pTemplate = pMainFrame->getDiagram()->getTemplate( idx);

			for( i = 0; i < pTemplate->getRoot()->getChildCount(); i++)
			{
				pMainFrame->getDiagram()->AddShape( NULL, (i3GICShape*) pTemplate->getRoot()->getChild( i));
			}

			pMainFrame->getDiagram()->SortByDepth();
		}

		m_iCurrentTemplate = idx;
		UpdateListCtrl();
	}
}


void CPaneTemplateView::OnNMRClickTemplateList(NMHDR * pNMHDR, LRESULT * pResult)
{
	CPoint pt;
	GetCursorPos( &pt);
	ProcessRButtonUp( 0, pt);
}

void CPaneTemplateView::OnEndLabelEdit(NMHDR * pNMHDR, LRESULT * pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM *plvItem = &plvDispInfo->item;
	if (plvItem->pszText != NULL)
	{
		m_TemplateListCtrl.SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);

		CMainFrame * pMainFrame = (CMainFrame*) ::AfxGetMainWnd();
		
		pMainFrame->getDiagram()->getTemplate( plvItem->iItem)->SetName( plvItem->pszText);
	}
}

//void CPaneTemplateView::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
//{
//	SCROLLINFO info;
//
//	info.cbSize = sizeof(SCROLLINFO);
//	info.fMask = SIF_PAGE | SIF_RANGE;
//	info.nMax = Factor;
//	info.nMin = 0;
//	info.nPage = m_WndSize.cy;
//
//	SetScrollInfo( SB_VERT, &info);
//
//	CDockablePane::OnVScroll( nSBCode, nPos, pScrollBar);
//}





