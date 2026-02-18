// TreeAddNode.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "TreeAddNode.h"
#include ".\treeaddnode.h"


// CTreeAddNode dialog

IMPLEMENT_DYNAMIC(CTreeAddNode, CDialog)
CTreeAddNode::CTreeAddNode(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeAddNode::IDD, pParent)
	, m_SelectedNode(_T(""))
{
}

CTreeAddNode::~CTreeAddNode()
{
}

void CTreeAddNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ListContorl);
	DDX_CBString(pDX, IDC_COMBO1, m_SelectedNode);
}

BOOL CTreeAddNode::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	{
		i3List tempList;
		INT32 i;
		i3ClassMeta * pMeta;

		m_ListContorl.AddString( "i3Node");

		pMeta = i3Node::GetClassMeta();

		pMeta->GetAllDerivedClasses( &tempList);

		for( i = 0; i < tempList.GetCount(); i++)
		{
			pMeta = (i3ClassMeta *) tempList.Items[i];

			m_ListContorl.AddString( pMeta->GetClassName());
		}
	}
	return TRUE; 
}

BEGIN_MESSAGE_MAP(CTreeAddNode, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CTreeAddNode message handlers

void CTreeAddNode::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CTreeAddNode::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
