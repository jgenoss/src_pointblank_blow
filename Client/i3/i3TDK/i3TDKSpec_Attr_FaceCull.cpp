// i3TDKSpec_Attr_FaceCull.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_FaceCull.h"
#include ".\i3tdkspec_attr_facecull.h"


// i3TDKSpec_Attr_FaceCull dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_FaceCull, i3TDKDialogBase)

i3TDKSpec_Attr_FaceCull::~i3TDKSpec_Attr_FaceCull()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_FaceCull::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_CULL, m_CB_Cull);
}

void i3TDKSpec_Attr_FaceCull::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FaceCullModeAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		INT32 idx = -1;

		switch( m_pAttr->Get())
		{
			case I3G_FACE_CULL_NONE :	idx = 0; break;
			case I3G_FACE_CULL_BACK :	idx = 1; break;
			case I3G_FACE_CULL_FRONT :	idx = 2; break;
		}

		m_CB_Cull.SetCurSel( idx);
	}
	else
	{
		m_CB_Cull.SetCurSel( -1);
	}
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_FaceCull, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_CULL, OnCbnSelchangeTdkCbCull)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_FaceCull message handlers

BOOL i3TDKSpec_Attr_FaceCull::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_FaceCull::OnOK()
{
}

void i3TDKSpec_Attr_FaceCull::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_FaceCull::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_FaceCull::OnCbnSelchangeTdkCbCull()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Cull.GetCurSel())
	{
		case 0 :		m_pAttr->Set( I3G_FACE_CULL_NONE);	break;
		case 1 :		m_pAttr->Set( I3G_FACE_CULL_BACK);	break;
		case 2 :		m_pAttr->Set( I3G_FACE_CULL_FRONT);	break;
	}
}
