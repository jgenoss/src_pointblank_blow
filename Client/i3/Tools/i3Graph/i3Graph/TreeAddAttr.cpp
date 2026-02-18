// TreeAddAttr.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "TreeAddAttr.h"
#include ".\treeaddattr.h"


// CTreeAddAttr dialog

IMPLEMENT_DYNAMIC(CTreeAddAttr, CDialog)
CTreeAddAttr::CTreeAddAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeAddAttr::IDD, pParent)
	, m_SelectedAttr(_T(""))
{
}

CTreeAddAttr::~CTreeAddAttr()
{
}

void CTreeAddAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ListControl);
	DDX_CBString(pDX, IDC_COMBO1, m_SelectedAttr);
}

BOOL CTreeAddAttr::OnInitDialog(void)
{
	CDialog::OnInitDialog(); 

	i3List tempList;
	INT32 i;
	i3ClassMeta * pMeta;

	pMeta = i3RenderAttr::GetClassMeta();
	pMeta->GetAllDerivedClasses( &tempList);

	for( i = 0; i < tempList.GetCount(); i++)
	{
		pMeta = (i3ClassMeta *) tempList.Items[i];
		m_ListControl.AddString( pMeta->GetClassName());				
	}

	tempList.Clear(); 
	pMeta = i3AbstractBoolAttr::GetClassMeta();
	pMeta->GetAllDerivedClasses( &tempList);
	for( i = 0; i < tempList.GetCount(); i++)
	{
		pMeta = (i3ClassMeta *) tempList.Items[i];
		m_ListControl.AddString( pMeta->GetClassName());				
	}
/*
	//이거는 나중에 삭제해야합니다. 
	m_ListControl.AddString("i3FogEnableAttr");
	//*/
	return TRUE; 
}

BEGIN_MESSAGE_MAP(CTreeAddAttr, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CTreeAddAttr message handlers

void CTreeAddAttr::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CTreeAddAttr::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CTreeAddAttr::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
}
