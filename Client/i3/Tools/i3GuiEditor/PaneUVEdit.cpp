// PaneUVEdit.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "PaneUVEdit.h"
#include "GlobalVariable.h"

// PaneUVEdit dialog

IMPLEMENT_DYNAMIC(CPaneUVEdit, i3TDKDialogBase)
CPaneUVEdit::CPaneUVEdit(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneUVEdit::IDD, pParent)
{
}

CPaneUVEdit::~CPaneUVEdit()
{
}

void CPaneUVEdit::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneUVEdit, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// PaneUVEdit message handlers

void CPaneUVEdit::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_UVEditCtrl.m_hWnd ))
	{
		m_UVEditCtrl.SetWindowPos( NULL, 0, 3, cx , cy -3, SWP_NOACTIVATE | SWP_NOZORDER );
	}
}

BOOL CPaneUVEdit::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText( "UVEdit");

	m_UVEditCtrl.Create( WS_CHILD | WS_VISIBLE, this, 0);

	i3TDK::RegisterUpdate( m_hWnd, i3Texture::static_meta(), I3_TDK_UPDATE_SELECT );
	i3TDK::RegisterUpdate( m_hWnd, i3GuiImage::static_meta(), I3_TDK_UPDATE_SET );
	i3TDK::RegisterUpdate( m_hWnd, i3GuiObjBase::static_meta(), I3_TDK_UPDATE_REMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneUVEdit::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( m_hWnd == pInfo->m_hwndFrom)
		return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT )
	{
		if( pInfo->m_Event & I3_TDK_USRWND_RESLIST)
		{
			if( pInfo->m_pObject != NULL)
			{
				if( i3::kind_of<i3Texture*>(pInfo->m_pObject)) //->IsTypeOf( i3Texture::static_meta()))
				{
					m_UVEditCtrl.SetImage( (i3Texture*)pInfo->m_pObject);
					m_UVEditCtrl.SetEditUVEnable( FALSE );
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_CTRLSHAPE)
		{
			if( pInfo->m_pObject != NULL)
			{
				if( i3::kind_of<i3GuiImage*>(pInfo->m_pObject)) //->IsTypeOf( i3GuiImage::static_meta()))
				{
					i3GuiImage * pImage = (i3GuiImage*)pInfo->m_pObject;
					m_UVEditCtrl.SetImageForEditUV( pImage );
					m_UVEditCtrl.SetEditUVEnable( TRUE );
				}
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_CTRLSHAPE)
		{
			m_UVEditCtrl.SetImage( NULL);
			m_UVEditCtrl.SetEditUVEnable( FALSE );
		}
	}
}

LRESULT CPaneUVEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL CPaneUVEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}
