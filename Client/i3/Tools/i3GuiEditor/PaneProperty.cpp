// PaneProperty.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "PaneProperty.h"
#include ".\paneproperty.h"

// CPaneProperty dialog

IMPLEMENT_DYNAMIC(CPaneProperty, i3TDKDialogBase)
CPaneProperty::CPaneProperty(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneProperty::IDD, pParent)
{
}

CPaneProperty::~CPaneProperty()
{
}

void CPaneProperty::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneProperty, i3TDKDialogBase)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneProperty message handlers

BOOL CPaneProperty::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText( "속성");
	CRect rect;
	GetClientRect(&rect);

	m_WndFoldCtrl.Create("Property", WS_CHILD | WS_VISIBLE, rect, this, 100);

	m_PropCommon.Create( MAKEINTRESOURCE( IDD_PROPERTY_COMMON), &m_WndFoldCtrl);
	m_WndFoldCtrl.AddPane( &m_PropCommon, "Common");

	i3TDK::RegisterUpdate( m_hWnd, i3GuiObjBase::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_REMOVE);
	i3TDK::RegisterUpdate( m_hWnd, i3GuiImage::static_meta(), I3_TDK_UPDATE_SELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneProperty::_SetInfo(i3GuiObjBase * pObj)
{
	//
	m_PropCommon.SetCommonInfo(pObj);

	if( i3::kind_of<i3GuiWindow*>(pObj)) //->IsTypeOf( i3GuiWindow::static_meta()))
	{
		//set window info
	}
	else if( i3::kind_of<i3GuiControl*>(pObj)) //->IsTypeOf( i3GuiControl::static_meta()))
	{
		//set control info
		
	}
}

void CPaneProperty::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	//property는 출처와 목적지에 상관없습니다.
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pObject != NULL )
		{
			if( i3::kind_of<i3GuiObjBase*>(pInfo->m_pObject)) //->IsTypeOf( i3GuiObjBase::static_meta()) )
			{
				m_PropCommon.SetCommonInfo((i3GuiObjBase*)pInfo->m_pObject);
			}
			else if ( i3::kind_of<i3GuiImage*>(pInfo->m_pObject)) //->IsTypeOf( i3GuiImage::static_meta()))
			{
				m_PropCommon.SetShapeInfo( (i3GuiImage*)pInfo->m_pObject);
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		m_PropCommon.UpdateInfo();
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		m_PropCommon.SetCommonInfo( NULL);
	}
}

LRESULT CPaneProperty::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPaneProperty::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	// TODO: Add your message handler code here and/or call default
	i3TDKDialogBase::OnLButtonDown(nFlags, point);
}

void CPaneProperty::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	RECT rt;
	::GetClientRect( this->m_hWnd, &rt);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_PropCommon))
	{
		m_PropCommon.SetWindowPos( NULL, rt.left , rt.top , rt.right, rt.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
