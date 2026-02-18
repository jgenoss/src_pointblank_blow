// i3TDKSpec_Attr_RenderMode.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_RenderMode.h"
#include ".\i3tdkspec_attr_rendermode.h"


// i3TDKSpec_Attr_RenderMode dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_RenderMode, i3TDKDialogBase)

i3TDKSpec_Attr_RenderMode::~i3TDKSpec_Attr_RenderMode()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_RenderMode::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_RENDERMODE, m_CB_Mode);
}

void i3TDKSpec_Attr_RenderMode::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3RenderModeAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		switch( m_pAttr->Get())
		{
			case I3G_RENDER_POINT	:	m_CB_Mode.SetCurSel( 0);	break;
			case I3G_RENDER_LINE	:	m_CB_Mode.SetCurSel( 1);	break;
			case I3G_RENDER_POLYGON	:	m_CB_Mode.SetCurSel( 2);	break;
		}
	}
	else
	{
		m_CB_Mode.SetCurSel( -1);
	}

	m_CB_Mode.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_RenderMode, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_RENDERMODE, OnCbnSelchangeTdkCbRendermode)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_RenderMode message handlers

BOOL i3TDKSpec_Attr_RenderMode::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_RenderMode::OnOK()
{
}

void i3TDKSpec_Attr_RenderMode::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_RenderMode::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_RenderMode::OnCbnSelchangeTdkCbRendermode()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Mode.GetCurSel())
	{
		case 0 :		m_pAttr->Set( I3G_RENDER_POINT);	break;
		case 1 :		m_pAttr->Set( I3G_RENDER_LINE);	break;
		case 2 :		m_pAttr->Set( I3G_RENDER_POLYGON);	break;
	}
}
