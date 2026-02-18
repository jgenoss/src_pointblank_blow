// SpecObject.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "SpecObject.h"
#include ".\specobject.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecObject dialog

IMPLEMENT_DYNAMIC(CSpecObject, i3TDKDialogBase)
CSpecObject::CSpecObject(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecObject::IDD, pParent)
{
	m_pObj = NULL;
}

CSpecObject::~CSpecObject()
{
}

void CSpecObject::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_NameCtrl);
	DDX_Control(pDX, IDC_ED_HP, m_HPCtrl);
}

void	CSpecObject::SetObject( i3ElementBase * pObj)
{
	m_pObj = (CObjectTemplate *) pObj;

	if( m_pObj != NULL)
	{
		// Name
		if( m_pObj->hasName())
		{
			m_NameCtrl.SetWindowText( m_pObj->GetName());
		}
		else
		{
			m_NameCtrl.SetWindowText( "");
		}

		// HP
		char conv[128];
		i3::ftoa( m_pObj->getMaxHP(), conv);

		m_HPCtrl.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CSpecObject, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_ED_NAME, OnEnKillfocusEdName)
	ON_EN_KILLFOCUS(IDC_ED_HP, OnEnKillfocusEdHp)
END_MESSAGE_MAP()


// CSpecObject message handlers

BOOL CSpecObject::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecObject::OnOK()
{
	if( m_NameCtrl.GetModify())
	{
		OnEnKillfocusEdName();
	}

	if( m_HPCtrl.GetModify())
	{
		OnEnKillfocusEdHp();
	}
}

void CSpecObject::OnCancel()
{
}

void CSpecObject::OnEnKillfocusEdName()
{
	char conv[128];

	if( m_pObj == NULL)
		return;

	m_NameCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pObj->SetName( conv);

	m_NameCtrl.SetModify( FALSE);
}

void CSpecObject::OnEnKillfocusEdHp()
{
	char conv[128];

	if( m_pObj == NULL)
		return;

	m_HPCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pObj->setMaxHP( (REAL32) atof( conv));

	m_HPCtrl.SetModify( FALSE);
}
