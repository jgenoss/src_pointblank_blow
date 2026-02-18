// i3TDKSpec_AttrSet.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_AttrSet.h"
#include ".\i3tdkspec_attrset.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKSpec_AttrSet dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_AttrSet, i3TDKDialogBase)

i3TDKSpec_AttrSet::~i3TDKSpec_AttrSet()
{
	I3_SAFE_RELEASE( m_pAttrSet);
}

void i3TDKSpec_AttrSet::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_PRIO, m_ED_Prio);
}

void i3TDKSpec_AttrSet::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttrSet, ((i3AttrSet *) pObj));

	if( m_pAttrSet != nullptr)
	{
		i3::snprintf( conv, _countof(conv), _T("%d"), m_pAttrSet->GetPriority());

		m_ED_Prio.SetWindowText( conv);
	}
	else
	{
		m_ED_Prio.SetWindowText( _T("") );
	}

	m_ED_Prio.SetModify( FALSE);
	m_ED_Prio.EnableWindow( m_pAttrSet != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_AttrSet, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_TDK_ED_PRIO, OnEnKillfocusTdkEdPrio)
END_MESSAGE_MAP()


// i3TDKSpec_AttrSet message handlers

BOOL i3TDKSpec_AttrSet::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_AttrSet::OnOK()
{
}

void i3TDKSpec_AttrSet::OnCancel()
{
}

LRESULT i3TDKSpec_AttrSet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_AttrSet::OnEnKillfocusTdkEdPrio()
{
	if( m_pAttrSet == nullptr)
		return;

	if( m_ED_Prio.GetModify())
	{
		TCHAR conv[256];

		m_ED_Prio.GetWindowText( conv, _countof(conv));
		m_pAttrSet->SetPriority( i3::atoi( conv));

		m_ED_Prio.SetModify( FALSE);
	}
}
