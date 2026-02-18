// i3TDKOptSpecBSPBuilder.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpecBSPBuilder.h"
#include ".\i3tdkoptspecbspbuilder.h"
#include "i3Base/string/ext/atoi.h"


// i3TDKOptSpecBSPBuilder dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecBSPBuilder, i3TDKOptSpecBase)

void i3TDKOptSpecBSPBuilder::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_COND_CONVEX, m_CHK_Cond_Convex);
	DDX_Control(pDX, IDC_CHK_COND_DEPTH, m_CHK_Cond_Depth);
	DDX_Control(pDX, IDC_CHK_COND_POLYCOUNT, m_CHK_Cond_PolyCount);
	DDX_Control(pDX, IDC_ED_DEPTHLIMIT, m_ED_DepthLimit);
	DDX_Control(pDX, IDC_ED_POLYCOUNTLIMIT, m_ED_MinPolyCount);
	DDX_Control(pDX, IDC_CB_PLANETYPE, m_CB_PlaneType);
	DDX_Control(pDX, IDC_CHK_SPLIT_POLY, m_CHK_SplitPoly);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpecBSPBuilder, i3TDKOptSpecBase)
	ON_BN_CLICKED(IDC_CHK_COND_DEPTH, OnBnClickedChkCondDepth)
	ON_BN_CLICKED(IDC_CHK_COND_POLYCOUNT, OnBnClickedChkCondPolycount)
END_MESSAGE_MAP()


// i3TDKOptSpecBSPBuilder message handlers

BOOL i3TDKOptSpecBSPBuilder::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		i3OptBSPBuilder * pOpt = (i3OptBSPBuilder *) m_pOpt;
		TCHAR conv[256];

		// Conditions
		{
			// Convex Geometry
			if( pOpt->isConditionFlag( SPLIT_COND_CONVEX))
				m_CHK_Cond_Convex.SetCheck( BST_CHECKED);
			else
				m_CHK_Cond_Convex.SetCheck( BST_UNCHECKED);

			// Depth
			if( pOpt->isConditionFlag( SPLIT_COND_DEPTH))
			{
				m_CHK_Cond_Depth.SetCheck( BST_CHECKED);
				m_ED_DepthLimit.EnableWindow( TRUE);
			}
			else
			{
				m_CHK_Cond_Depth.SetCheck( BST_UNCHECKED);
				m_ED_DepthLimit.EnableWindow( FALSE);
			}

			// Depth Limit
			i3::snprintf( conv, 256, _T("%d"), pOpt->getDepthLimit());
			m_ED_DepthLimit.SetWindowText( conv);

			// Polygon Count
			if( pOpt->isConditionFlag( SPLIT_COND_POLYCOUNT))
			{
				m_CHK_Cond_PolyCount.SetCheck( BST_CHECKED);
				m_ED_MinPolyCount.EnableWindow( TRUE);
			}
			else
			{
				m_CHK_Cond_PolyCount.SetCheck( BST_UNCHECKED);
				m_ED_MinPolyCount.EnableWindow( FALSE);
			}

			// Minimum Polygon Count
			i3::snprintf(conv, 256, _T("%d"), pOpt->getMinPolygonCount());
			m_ED_MinPolyCount.SetWindowText( conv);
		}

		// Split Mode
		switch( pOpt->getSplitMode())
		{
			case SPLIT_MODE_HYPERPLANE :		m_CB_PlaneType.SetCurSel( 0);	break;
			default :							m_CB_PlaneType.SetCurSel( 1);	break;
		}

		// Split spanning triangles
		if( pOpt->getSplitPolygonEnable())
			m_CHK_SplitPoly.SetCheck( BST_CHECKED);
		else
			m_CHK_SplitPoly.SetCheck( BST_UNCHECKED);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecBSPBuilder::OnOK()
{
	{
		i3OptBSPBuilder * pOpt = (i3OptBSPBuilder *) m_pOpt;
		TCHAR conv[256];

		// Conditions
		{
			// Convex Geometry
			if( m_CHK_Cond_Convex.GetCheck() == BST_CHECKED)
				pOpt->addConditionFlag( SPLIT_COND_CONVEX);
			else
				pOpt->removeConditionFlag( SPLIT_COND_CONVEX);

			// Depth
			if( m_CHK_Cond_Depth.GetCheck() == BST_CHECKED)
			{
				pOpt->addConditionFlag( SPLIT_COND_DEPTH);

				m_ED_DepthLimit.GetWindowText( conv, sizeof(conv)/sizeof(TCHAR) - 1);
				pOpt->setDepthLimit( i3::atoi( conv));
			}
			else
			{
				pOpt->removeConditionFlag( SPLIT_COND_DEPTH);
			}

			// Polygon Count
			if( m_CHK_Cond_PolyCount.GetCheck() == BST_CHECKED)
			{
				pOpt->addConditionFlag( SPLIT_COND_POLYCOUNT);
				
				m_ED_MinPolyCount.GetWindowText( conv, sizeof(conv) - 1);
				pOpt->setMinPolygonCount( i3::atoi( conv));
			}
			else
			{
				pOpt->removeConditionFlag( SPLIT_COND_POLYCOUNT);
			}
		}

		// Split Mode
		switch( m_CB_PlaneType.GetCurSel())
		{
			case 0 :		pOpt->setSplitMode( SPLIT_MODE_HYPERPLANE);	break;
			default :		pOpt->setSplitMode( SPLIT_MODE_HALFPLANE);	break;
		}

		// Split spanning triangles
		if( m_CHK_SplitPoly.GetCheck() == BST_CHECKED)
			pOpt->setSplitPolygonEnable( true);
		else
			pOpt->setSplitPolygonEnable( false);
	}

	i3TDKOptSpecBase::OnOK();
}

void i3TDKOptSpecBSPBuilder::OnBnClickedChkCondDepth()
{
	if( m_CHK_Cond_Depth.GetCheck() == BST_CHECKED)
	{
		m_ED_DepthLimit.EnableWindow( TRUE);
	}
	else
	{
		m_ED_DepthLimit.EnableWindow( FALSE);
	}
}

void i3TDKOptSpecBSPBuilder::OnBnClickedChkCondPolycount()
{
	if( m_CHK_Cond_PolyCount.GetCheck() == BST_CHECKED)
	{
		m_ED_MinPolyCount.EnableWindow( TRUE);
	}
	else
	{
		m_ED_MinPolyCount.EnableWindow( FALSE);
	}
}
