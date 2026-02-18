// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecChangeCollisionGroup.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKOptSpecChangeCollisionGroup dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecChangeCollisionGroup, i3TDKOptSpecBase)

void i3TDKOptSpecChangeCollisionGroup::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_EDIT_COLLISIONGROUPIDX, m_Ed_CollisionGroupIdx);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecChangeCollisionGroup, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKOptSpecChangeCollisionGroup message handlers

BOOL i3TDKOptSpecChangeCollisionGroup::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	m_nCollisionGroup = 0;
	m_Ed_CollisionGroupIdx.SetWindowText( _T("0") );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecChangeCollisionGroup::OnOK()
{
	TCHAR conv[ 256];
	m_Ed_CollisionGroupIdx.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);

	m_nCollisionGroup = (UINT16) i3::atoi( conv);

	i3OptChangeCollisionGroup * pOpt = (i3OptChangeCollisionGroup*) m_pOpt;
	pOpt->SetCollisionGroup( m_nCollisionGroup);

	i3TDKOptSpecBase::OnOK();
}
