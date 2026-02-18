// SelectBoneDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "SelectBoneDialog.h"
#include ".\selectbonedialog.h"


// CSelectBoneDialog dialog

IMPLEMENT_DYNAMIC(CSelectBoneDialog, CDialog)
CSelectBoneDialog::CSelectBoneDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectBoneDialog::IDD, pParent)
	, m_bAttach(FALSE)
{
	m_pRoot = NULL;
	m_pBone = NULL;
}

CSelectBoneDialog::~CSelectBoneDialog()
{
}

void CSelectBoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BONELIST, m_BoneListCtrl);
	DDX_Check(pDX, IDC_CHK_ATTACHBONE, m_bAttach);
}

BOOL CSelectBoneDialog::Execute( i3Node * pRoot)
{
	m_pRoot = pRoot;

	if( DoModal() == IDCANCEL)
	{
		m_pBone = NULL;
		return FALSE;
	}

	return TRUE;
}

void CSelectBoneDialog::AssignValue(void)
{
	i3::vector<i3Node*> BoneList;
	INT32  idx;
	i3Transform * pBone;

	i3Scene::FindNodeByType( m_pRoot, i3Transform::static_meta(), BoneList);

	for(size_t i = 0; i < BoneList.size(); i++)
	{
		pBone = (i3Transform *) BoneList[i];

		if( pBone->hasName())
		{
			idx = m_BoneListCtrl.InsertItem( m_BoneListCtrl.GetItemCount(), pBone->GetName(), 0);

			m_BoneListCtrl.SetItemData( idx, (DWORD_PTR) pBone);
		}
	}
}

BEGIN_MESSAGE_MAP(CSelectBoneDialog, CDialog)
END_MESSAGE_MAP()


// CSelectBoneDialog message handlers

BOOL CSelectBoneDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_BoneListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 250);
		
		m_BoneListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	AssignValue();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectBoneDialog::OnOK()
{
	POSITION pos;

	UpdateData( TRUE);

	pos = m_BoneListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		INT32 idx = m_BoneListCtrl.GetNextSelectedItem( pos);

		m_pBone = (i3Transform *) m_BoneListCtrl.GetItemData( idx);
	}
	else
	{
		m_pBone = NULL;
	}

	CDialog::OnOK();
}
