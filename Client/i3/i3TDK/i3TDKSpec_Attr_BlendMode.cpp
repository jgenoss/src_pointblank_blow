// i3TDKSpec_Attr_BlendMode.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_BlendMode.h"
#include ".\i3tdkspec_attr_blendmode.h"


// i3TDKSpec_Attr_BlendMode dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_BlendMode, i3TDKDialogBase)

i3TDKSpec_Attr_BlendMode::~i3TDKSpec_Attr_BlendMode()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_BlendMode::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_SRC, m_CB_Src);
	DDX_Control(pDX, IDC_TDK_CB_DEST, m_CB_Dest);
	DDX_Control(pDX, IDC_TDK_CB_OP, m_CB_OP);
}

void i3TDKSpec_Attr_BlendMode::SetObject( i3ElementBase * pObj)
{
	INT32 idx;

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3BlendModeAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		// Source
		idx = -1;
		switch( m_pAttr->GetSource())
		{
			case I3G_BLEND_ZERO			:	idx = 0;	break;
			case I3G_BLEND_ONE			:	idx = 1;	break;
			case I3G_BLEND_SRCCOLOR		:	idx = 2;	break;
			case I3G_BLEND_INVSRCCOLOR	:	idx = 3;	break;
			case I3G_BLEND_SRCALPHA		:	idx = 4;	break;
			case I3G_BLEND_INVSRCALPHA	:	idx = 5;	break;
			case I3G_BLEND_DESTALPHA	:	idx = 6;	break;
			case I3G_BLEND_INVDESTALPHA	:	idx = 7;	break;
			case I3G_BLEND_DESTCOLOR	:	idx = 8;	break;
			case I3G_BLEND_INVDESTCOLOR	:	idx = 9;	break;
			case I3G_BLEND_SRCALPHASAT	:	idx = 10;	break;
			case I3G_BLEND_FACTOR		:	idx = 11;	break;
			case I3G_BLEND_INVFACTOR	:	idx = 12;	break;
		}

		m_CB_Src.SetCurSel( idx);

		// Destination
		idx = -1;
		switch( m_pAttr->GetDestination())
		{
			case I3G_BLEND_ZERO			:	idx = 0;	break;
			case I3G_BLEND_ONE			:	idx = 1;	break;
			case I3G_BLEND_SRCCOLOR		:	idx = 2;	break;
			case I3G_BLEND_INVSRCCOLOR	:	idx = 3;	break;
			case I3G_BLEND_SRCALPHA		:	idx = 4;	break;
			case I3G_BLEND_INVSRCALPHA	:	idx = 5;	break;
			case I3G_BLEND_DESTALPHA	:	idx = 6;	break;
			case I3G_BLEND_INVDESTALPHA	:	idx = 7;	break;
			case I3G_BLEND_DESTCOLOR	:	idx = 8;	break;
			case I3G_BLEND_INVDESTCOLOR	:	idx = 9;	break;
			case I3G_BLEND_SRCALPHASAT	:	idx = 10;	break;
			case I3G_BLEND_FACTOR		:	idx = 11;	break;
			case I3G_BLEND_INVFACTOR	:	idx = 12;	break;
		}
		m_CB_Dest.SetCurSel( idx);

		// Operation
		idx = -1;
		switch( m_pAttr->GetBlendOperation())
		{
			case I3G_BLEND_OP_NONE			: idx = 0;	break;
			case I3G_BLEND_OP_ADD			: idx = 1;	break;
			case I3G_BLEND_OP_SUBTRACT		: idx = 2;	break;
			case I3G_BLEND_OP_REVSUBTRACT	: idx = 3;	break;
			case I3G_BLEND_OP_MIN			: idx = 4;	break;
			case I3G_BLEND_OP_MAX			: idx = 5;	break;
		}
		m_CB_OP.SetCurSel( idx);
	}
	else
	{
		m_CB_Src.SetCurSel( -1);
		m_CB_Dest.SetCurSel( -1);
		m_CB_OP.SetCurSel( -1);
	}

	m_CB_Src.EnableWindow( m_pAttr != nullptr);
	m_CB_Dest.EnableWindow( m_pAttr != nullptr);
	m_CB_OP.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_BlendMode, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_DEST, OnCbnSelchangeTdkCbDest)
	ON_CBN_SELCHANGE(IDC_TDK_CB_SRC, OnCbnSelchangeTdkCbSrc)
	ON_CBN_SELCHANGE(IDC_TDK_CB_OP, OnCbnSelchangeTdkCbOp)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_BlendMode message handlers

BOOL i3TDKSpec_Attr_BlendMode::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_BlendMode::OnOK()
{
}

void i3TDKSpec_Attr_BlendMode::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_BlendMode::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_BlendMode::OnCbnSelchangeTdkCbDest()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Dest.GetCurSel())
	{
		case 0	:	m_pAttr->SetDestination( I3G_BLEND_ZERO);	break;
		case 1	:	m_pAttr->SetDestination( I3G_BLEND_ONE);	break;
		case 2	:	m_pAttr->SetDestination( I3G_BLEND_SRCCOLOR);	break;
		case 3	:	m_pAttr->SetDestination( I3G_BLEND_INVSRCCOLOR);	break;
		case 4	:	m_pAttr->SetDestination( I3G_BLEND_SRCALPHA);	break;
		case 5	:	m_pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);	break;
		case 6	:	m_pAttr->SetDestination( I3G_BLEND_DESTALPHA);	break;
		case 7	:	m_pAttr->SetDestination( I3G_BLEND_INVDESTALPHA);	break;
		case 8	:	m_pAttr->SetDestination( I3G_BLEND_DESTCOLOR);	break;
		case 9	:	m_pAttr->SetDestination( I3G_BLEND_INVDESTCOLOR);	break;
		case 10	:	m_pAttr->SetDestination( I3G_BLEND_SRCALPHASAT);	break;
		case 11	:	m_pAttr->SetDestination( I3G_BLEND_FACTOR);	break;
		case 12	:	m_pAttr->SetDestination( I3G_BLEND_INVFACTOR);	break;
	}
}

void i3TDKSpec_Attr_BlendMode::OnCbnSelchangeTdkCbSrc()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Src.GetCurSel())
	{
		case 0	:	m_pAttr->SetSource( I3G_BLEND_ZERO);	break;
		case 1	:	m_pAttr->SetSource( I3G_BLEND_ONE);	break;
		case 2	:	m_pAttr->SetSource( I3G_BLEND_SRCCOLOR);	break;
		case 3	:	m_pAttr->SetSource( I3G_BLEND_INVSRCCOLOR);	break;
		case 4	:	m_pAttr->SetSource( I3G_BLEND_SRCALPHA);	break;
		case 5	:	m_pAttr->SetSource( I3G_BLEND_INVSRCALPHA);	break;
		case 6	:	m_pAttr->SetSource( I3G_BLEND_DESTALPHA);	break;
		case 7	:	m_pAttr->SetSource( I3G_BLEND_INVDESTALPHA);	break;
		case 8	:	m_pAttr->SetSource( I3G_BLEND_DESTCOLOR);	break;
		case 9	:	m_pAttr->SetSource( I3G_BLEND_INVDESTCOLOR);	break;
		case 10	:	m_pAttr->SetSource( I3G_BLEND_SRCALPHASAT);	break;
		case 11	:	m_pAttr->SetSource( I3G_BLEND_FACTOR);	break;
		case 12	:	m_pAttr->SetSource( I3G_BLEND_INVFACTOR);	break;
	}
}

void i3TDKSpec_Attr_BlendMode::OnCbnSelchangeTdkCbOp()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_OP.GetCurSel())
	{
		case 0	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_NONE);	break;
		case 1	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_ADD);	break;
		case 2	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_SUBTRACT);	break;
		case 3	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_REVSUBTRACT);	break;
		case 4	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_MIN);	break;
		case 5	: m_pAttr->SetBlendOperation( I3G_BLEND_OP_MAX);	break;
	}
}
