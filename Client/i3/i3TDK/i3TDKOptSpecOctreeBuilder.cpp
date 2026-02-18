// i3TDKOptSpecOctreeBuilder.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecOctreeBuilder.h"
#include ".\i3tdkoptspecoctreebuilder.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"
// i3TDKOptSpecOctreeBuilder dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecOctreeBuilder, i3TDKOptSpecBase)

void i3TDKOptSpecOctreeBuilder::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_CENTERMETHOD, m_CB_CenterMethod);
	DDX_Control(pDX, IDC_TDK_CHK_SPLITMESH, m_CHK_Split);
	DDX_Control(pDX, IDC_TDK_CHK_LIMITDEPTH, m_CHK_Depth);
	DDX_Control(pDX, IDC_TDK_ED_MAXDEPTH, m_ED_Depth);
	DDX_Control(pDX, IDC_TDK_CHK_MINPOLY, m_CHK_MinPoly);
	DDX_Control(pDX, IDC_TDK_ED_MINPOLY, m_ED_MinPoly);
	DDX_Control(pDX, IDC_TDK_ED_MINPOLY2, m_ED_CellSize);
	DDX_Control(pDX, IDC_TDK_ED_MINPOLY3, m_ED_EPS);
	DDX_Control(pDX, IDC_TDK_ED_EXTENT, m_ED_K);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecOctreeBuilder, i3TDKOptSpecBase)
	ON_BN_CLICKED(IDC_TDK_CHK_LIMITDEPTH, OnBnClickedTdkChkLimitdepth)
	ON_BN_CLICKED(IDC_TDK_CHK_MINPOLY, OnBnClickedTdkChkMinpoly)
END_MESSAGE_MAP()


// i3TDKOptSpecOctreeBuilder message handlers

BOOL i3TDKOptSpecOctreeBuilder::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		TCHAR conv[256];
		i3OptBuildOcTree * pOpt = (i3OptBuildOcTree *) m_pOpt;

		// Method of Center
		switch( pOpt->getMethodOfCenter())
		{
			case i3OptBuildOcTree::CFM_MAX_SPAN :	m_CB_CenterMethod.SetCurSel( 0);	break;
			default :								m_CB_CenterMethod.SetCurSel( 1);	break;
		}

		// Split mesh
		if( pOpt->getSplitState())
			m_CHK_Split.SetCheck( BST_CHECKED);
		else
			m_CHK_Split.SetCheck( BST_UNCHECKED);

		// Limit Depth
		if( pOpt->getLimitDepthState())
			m_CHK_Depth.SetCheck( BST_CHECKED);
		else
		{
			m_CHK_Depth.SetCheck( BST_UNCHECKED);

			m_ED_Depth.EnableWindow( FALSE);
		}

		// Maximum depth
		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%d"), pOpt->getMaxDepth());
		m_ED_Depth.SetWindowText( conv);

		// Min Polygon
		if( pOpt->getLimitTriangleCountState())
			m_CHK_MinPoly.SetCheck( BST_CHECKED);
		else
		{
			m_CHK_MinPoly.SetCheck( BST_UNCHECKED);

			m_ED_MinPoly.EnableWindow( FALSE);
		}

		i3::snprintf( conv, sizeof(conv)/sizeof(TCHAR), _T("%d"), pOpt->getMinTriangleCount());
		m_ED_MinPoly.SetWindowText( conv);

		// Cell size
		i3::ftoa( pOpt->getCellSize(), conv);
		m_ED_CellSize.SetWindowText( conv);

		i3::ftoa( pOpt->getExtent(), conv);
		m_ED_K.SetWindowText( conv);

		// EPS
		i3::ftoa( pOpt->getEPS(), conv);
		m_ED_EPS.SetWindowText( conv);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecOctreeBuilder::OnOK()
{
	TCHAR conv[256];
	i3OptBuildOcTree * pOpt = (i3OptBuildOcTree *) m_pOpt;

	// Method of Center
	switch( m_CB_CenterMethod.GetCurSel())
	{
		case 0 :	pOpt->setMethodOfCenter( i3OptBuildOcTree::CFM_MAX_SPAN);	break;
		default :	pOpt->setMethodOfCenter( i3OptBuildOcTree::CFM_FIXED);	break;
	}

	// Split Mesh
	if( m_CHK_Split.GetCheck() == BST_CHECKED)
		pOpt->setSplitState( true);
	else
		pOpt->setSplitState( false);

	// Limit Depth
	if( m_CHK_Depth.GetCheck() == BST_CHECKED)
	{
		pOpt->setLimitDepthState( true);

		m_ED_Depth.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) );
		pOpt->setMaxDepth( i3::atoi( conv));
	}
	else
		pOpt->setLimitDepthState( false);

	// Min Polygon
	if( m_CHK_MinPoly.GetCheck() == BST_CHECKED)
	{
		pOpt->setLimitTriangleCountState( true);

		m_ED_MinPoly.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) );
		pOpt->setMinTriangleCount( i3::atoi( conv));
	}
	else
		pOpt->setLimitTriangleCountState( false);

	// Cell size
	m_ED_CellSize.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) );
	pOpt->setCellSize( (REAL32) i3::atof( conv));

	// Extent
	m_ED_K.GetWindowText( conv, sizeof( conv)/sizeof(TCHAR) );
	pOpt->setExtent( (REAL32) i3::atof( conv));

	// EPS
	m_ED_EPS.GetWindowText( conv, sizeof( conv)/sizeof(TCHAR) );
	pOpt->setEPS( (REAL32) i3::atof( conv));

	i3TDKOptSpecBase::OnOK();
}

void i3TDKOptSpecOctreeBuilder::OnBnClickedTdkChkLimitdepth()
{
	m_ED_Depth.EnableWindow( ( m_CHK_Depth.GetCheck() == BST_CHECKED));
}

void i3TDKOptSpecOctreeBuilder::OnBnClickedTdkChkMinpoly()
{
	m_ED_MinPoly.EnableWindow( m_CHK_MinPoly.GetCheck() == BST_CHECKED);
}
