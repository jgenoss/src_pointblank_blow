// i3TDKSpec_Attr_Color.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Color.h"

// i3TDKSpec_Attr_Color dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Color, i3TDKDialogBase)

i3TDKSpec_Attr_Color::~i3TDKSpec_Attr_Color()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Color::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ST_COLOR_PH, m_ST_ColorPH);
}

void i3TDKSpec_Attr_Color::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3ColorAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		m_ColorCtrl.setColor( m_pAttr->Get());
	}
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Color, i3TDKDialogBase)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Color message handlers

BOOL i3TDKSpec_Attr_Color::OnInitDialog()
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

void i3TDKSpec_Attr_Color::OnOK()
{
}

void i3TDKSpec_Attr_Color::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_Color::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
