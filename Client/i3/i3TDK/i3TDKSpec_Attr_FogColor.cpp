// i3TDKSpec_Attr_FogColor.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_FogColor.h"
#include ".\i3tdkspec_attr_fogcolor.h"


// i3TDKSpec_Attr_FogColor dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_FogColor, i3TDKDialogBase)

i3TDKSpec_Attr_FogColor::~i3TDKSpec_Attr_FogColor()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_FogColor::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ST_COLOR_PH, m_ST_ColorPH);
}

void i3TDKSpec_Attr_FogColor::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FogColorAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		m_ColorCtrl.setColor( m_pAttr->Get());
	}
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_FogColor, i3TDKDialogBase)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_FogColor message handlers

BOOL i3TDKSpec_Attr_FogColor::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_ST_ColorPH.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ColorCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_FogColor::OnOK()
{
}

void i3TDKSpec_Attr_FogColor::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_FogColor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_TDK_SETCOLOR :
			if( m_pAttr != nullptr)
			{
				COLOR col;

				i3Color::Set( &col, m_ColorCtrl.getColor());

				m_pAttr->Set( &col);
			}
			break;
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}
