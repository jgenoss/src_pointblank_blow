// DialogStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "DialogStatistics.h"
#include ".\dialogstatistics.h"


// CDialogStatistics dialog

IMPLEMENT_DYNAMIC(CDialogStatistics, CDialog)
CDialogStatistics::CDialogStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogStatistics::IDD, pParent)
{
}

CDialogStatistics::~CDialogStatistics()
{
}

void CDialogStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NODECOUNT, m_ED_NodeCount);
	DDX_Control(pDX, IDC_ED_ATTRCOUNT, m_ED_AttrCount);
	DDX_Control(pDX, IDC_ED_SHAPESETCOUNT, m_ED_ShapeSetCount);
	DDX_Control(pDX, IDC_ED_CONVEXCOUNT, m_ED_ConvexCount);
}

bool CDialogStatistics::Execute( i3Node * pNode)
{
	m_pRoot = pNode;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDialogStatistics, CDialog)
END_MESSAGE_MAP()


// CDialogStatistics message handlers

I3SG_TRAVERSAL_RESULT CDialogStatistics::_ScanProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack )
{
	CDialogStatistics * pDlg = (CDialogStatistics *) pUserData;

	pDlg->m_NodeCount++;

	if( pNode->IsTypeOf( i3AttrSet::GetClassMeta()))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		pDlg->m_AttrCount += pAttrSet->GetAttrCount();
	}
	else if( pNode->IsTypeOf( i3Geometry::GetClassMeta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;

		pDlg->m_AttrCount += pGeo->GetGeometryAttrCount();
	}
	else if( pNode->IsTypeOf( i3Transform2::GetClassMeta()))
	{
		i3Transform2 * pTrans2 = (i3Transform2 *) pNode;
		i3PhysixShapeSet * pShapeSet = pTrans2->getShapeSet();

		if( pShapeSet != NULL)
		{
			pDlg->m_ShapeSetCount++;

			pDlg->m_ConvexCount += pShapeSet->getNxShapeCount();
		}
	}
	else if( pNode->IsTypeOf( i3ShapeSetContainer::GetClassMeta()))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;
		i3PhysixShapeSet * pShapeSet = pContainer->getShapeSet();

		if( pShapeSet != NULL)
		{
			pDlg->m_ShapeSetCount++;

			pDlg->m_ConvexCount += pShapeSet->getNxShapeCount();
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

BOOL CDialogStatistics::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_NodeCount = 0;
		m_AttrCount = 0;
		m_ShapeSetCount = 0;
		m_ConvexCount = 0;

		//i3Scene::Traverse( m_pRoot, _ScanProc, this);

		{
			NxScene * pScene = g_pPhysixCtx->getScene();

			m_ConvexCount = pScene->getNbStaticShapes() + pScene->getNbDynamicShapes();
		}

		char conv[256];

		sprintf( conv, "%d", m_NodeCount);
		m_ED_NodeCount.SetWindowText( conv);

		sprintf( conv, "%d", m_AttrCount);
		m_ED_AttrCount.SetWindowText( conv);

		sprintf( conv, "%d", m_ShapeSetCount);
		m_ED_ShapeSetCount.SetWindowText( conv);

		sprintf( conv, "%d", m_ConvexCount);
		m_ED_ConvexCount.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
