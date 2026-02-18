// i3TDKOptSpecSpatialPartition.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecSpatialPartition.h"
#include "resource.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"
#include "i3Base/string/ext/atoi.h"
// i3TDKOptSpecSpatialPartition dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecSpatialPartition, i3TDKOptSpecBase)

void i3TDKOptSpecSpatialPartition::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_X, m_ED_SizeX);
	DDX_Control(pDX, IDC_TDK_ED_Y, m_ED_SizeY);
	DDX_Control(pDX, IDC_TDK_ED_Z, m_ED_SizeZ);
	DDX_Control(pDX, IDC_TDK_ED_TRICOUNT, m_ED_TriCount);
	DDX_Control(pDX, IDC_TDK_CHK_SPLIT, m_CHK_Split);
	DDX_Control(pDX, IDC_TDK_CHK_CREATEBOUNDBOX, m_CHK_CreateBoundBox);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecSpatialPartition, i3TDKOptSpecBase)
	ON_BN_CLICKED(IDC_TDK_CHK_SPLIT, OnBnClickedChkSplit)
END_MESSAGE_MAP()


// i3TDKOptSpecSpatialPartition message handlers

BOOL i3TDKOptSpecSpatialPartition::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];

		i3OptSpatialPartition * pOpt = (i3OptSpatialPartition *) m_pOpt;

		if( pOpt->getSplitEnable())
			m_CHK_Split.SetCheck( BST_CHECKED);
		else
			m_CHK_Split.SetCheck( BST_UNCHECKED);

		i3::ftoa( pOpt->getLimitSize()->x, conv);
		m_ED_SizeX.SetWindowText( conv);
		m_ED_SizeX.EnableWindow( pOpt->getSplitEnable() == true);

		i3::ftoa( pOpt->getLimitSize()->y, conv);
		m_ED_SizeY.SetWindowText( conv);
		m_ED_SizeY.EnableWindow( pOpt->getSplitEnable() == true);

		i3::ftoa( pOpt->getLimitSize()->z, conv);
		m_ED_SizeZ.SetWindowText( conv);
		m_ED_SizeZ.EnableWindow( pOpt->getSplitEnable() == true);

		i3::snprintf( conv, 256, _T("%d"), pOpt->getLimitTriangleCount());
		m_ED_TriCount.SetWindowText( conv);
		m_ED_TriCount.EnableWindow( pOpt->getSplitEnable() == true);

		if( pOpt->getCreateBoundBoxState())
			m_CHK_CreateBoundBox.SetCheck( BST_CHECKED);
		else
			m_CHK_CreateBoundBox.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecSpatialPartition::OnOK()
{
	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];
		VEC3D sz;

		i3OptSpatialPartition * pOpt = (i3OptSpatialPartition *) m_pOpt;

		if( m_CHK_Split.GetCheck() == BST_CHECKED)
			pOpt->setSplitEnable( true);
		else
			pOpt->setSplitEnable( false);

		m_ED_SizeX.GetWindowText( conv, (sizeof(conv) - 1)/sizeof(TCHAR) );
		sz.x = (REAL32) i3::atof( conv);

		m_ED_SizeY.GetWindowText( conv, (sizeof(conv) - 1)/sizeof(TCHAR) );
		sz.y = (REAL32) i3::atof( conv);

		m_ED_SizeZ.GetWindowText( conv, (sizeof(conv) - 1)/sizeof(TCHAR) );
		sz.z = (REAL32) i3::atof( conv);

		pOpt->setLimitSize( &sz);

		if( m_CHK_CreateBoundBox.GetCheck() == BST_CHECKED)
			pOpt->setCreateBoundBoxState( true);
		else
			pOpt->setCreateBoundBoxState( false);

		m_ED_TriCount.GetWindowText( conv, (sizeof(conv) -1)/sizeof(TCHAR) );
		pOpt->setLimitTriangleCount( i3::atoi( conv));
	}

	i3TDKOptSpecBase::OnOK();
}

void i3TDKOptSpecSpatialPartition::OnBnClickedChkSplit()
{
	BOOL bFlag;

	bFlag = m_CHK_Split.GetCheck() == BST_CHECKED;

	m_ED_SizeX.EnableWindow( bFlag);
	m_ED_SizeY.EnableWindow( bFlag);
	m_ED_SizeZ.EnableWindow( bFlag);
	m_ED_TriCount.EnableWindow( bFlag);
}
