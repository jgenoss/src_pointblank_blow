// SpecChara.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecChara.h"
#include "i3Base/string/ext/ftoa.h"
// CSpecChara dialog

IMPLEMENT_DYNAMIC(CSpecChara, i3TDKDialogBase)
CSpecChara::CSpecChara(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecChara::IDD, pParent)
{
}

CSpecChara::~CSpecChara()
{
}

void CSpecChara::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_NameCtrl);
	DDX_Control(pDX, IDC_ED_SLOPE, m_SlopeCtrl);
	DDX_Control(pDX, IDC_ED_CONST, m_ConstCtrl);
}

void CSpecChara::SetObject( i3ElementBase * pObj)
{
	CChara * pChara = (CChara *) pObj;
	char conv[256];

	I3ASSERT( i3::kind_of<CChara*>(pObj)); //->IsTypeOf( CChara::static_meta()));
	I3_REF_CHANGE(m_pObject, pObj);

	if( pChara != NULL)
	{
		// Name
		if( pChara->hasName())
		{
			m_NameCtrl.SetWindowText( pChara->GetName());
		}
		else
		{
			m_NameCtrl.SetWindowText( "");
		}

		// Slope
		i3::ftoa( pChara->getSlope(), conv);
		m_SlopeCtrl.SetWindowText( conv);

		// Const
		i3::ftoa( pChara->getConst(), conv);
		m_ConstCtrl.SetWindowText( conv);
	}
	else
	{
		m_NameCtrl.SetWindowText( "");
		m_SlopeCtrl.SetWindowText( "");
		m_ConstCtrl.SetWindowText( "");
	}

	m_NameCtrl.SetModify( FALSE);
	m_SlopeCtrl.SetModify( FALSE);
	m_ConstCtrl.SetModify( FALSE);
}


BEGIN_MESSAGE_MAP(CSpecChara, i3TDKDialogBase)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_ED_CONST, OnEnKillfocusEdConst)
	ON_EN_KILLFOCUS(IDC_ED_NAME, OnEnKillfocusEdName)
	ON_EN_KILLFOCUS(IDC_ED_SLOPE, OnEnKillfocusEdSlope)
END_MESSAGE_MAP()


// CSpecChara message handlers

BOOL CSpecChara::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecChara::OnCancel()
{
}

void CSpecChara::OnOK()
{
	char conv[256];
	CChara * pChara = (CChara *) m_pObject;
	bool bModify = false;

	if( m_NameCtrl.GetModify())
	{
		m_NameCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_NameCtrl.SetModify( FALSE);

		pChara->SetName( conv);
		bModify = true;
	}

	if( m_SlopeCtrl.GetModify())
	{
		m_SlopeCtrl.GetWindowText( conv, sizeof( conv) - 1);
		m_SlopeCtrl.SetModify( FALSE);

		pChara->setSlope( (REAL32) atof( conv));
		bModify = true;
	}

	if( m_ConstCtrl.GetModify())
	{
		m_ConstCtrl.GetWindowText( conv, sizeof(conv) - 1);
		m_ConstCtrl.SetModify( FALSE);

		pChara->setConst( (REAL32) atof( conv));
		bModify = true;
	}

	if( bModify)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pChara);
	}
}

void CSpecChara::OnKillFocus(CWnd* pNewWnd)
{
	i3TDKDialogBase::OnKillFocus(pNewWnd);

	OnOK();
}

void CSpecChara::OnEnKillfocusEdConst()
{
	OnOK();
}

void CSpecChara::OnEnKillfocusEdName()
{
	OnOK();
}

void CSpecChara::OnEnKillfocusEdSlope()
{
	OnOK();
}
