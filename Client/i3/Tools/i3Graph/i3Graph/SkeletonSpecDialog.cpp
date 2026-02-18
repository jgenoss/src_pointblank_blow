// SkeletonSpecDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "SkeletonSpecDialog.h"
#include ".\skeletonspecdialog.h"

#include "ShapeSetSpecDialog.h"
#include "JointD6SpecDialog.h"


// CSkeletonSpecDialog dialog

IMPLEMENT_DYNAMIC(CSkeletonSpecDialog, CDialog)
CSkeletonSpecDialog::CSkeletonSpecDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSkeletonSpecDialog::IDD, pParent)
{
}

CSkeletonSpecDialog::~CSkeletonSpecDialog()
{
}

void CSkeletonSpecDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKEL_LIST, m_SkelListCtrl);
	DDX_Control(pDX, IDC_BONE_TREE, m_BoneTreeCtrl);
	DDX_Control(pDX, IDC_ED_M00, m_M00Ctrl);
	DDX_Control(pDX, IDC_ED_M01, m_M01Ctrl);
	DDX_Control(pDX, IDC_ED_M02, m_M02Ctrl);
	DDX_Control(pDX, IDC_ED_M03, m_M03Ctrl);
	DDX_Control(pDX, IDC_ED_M10, m_M10Ctrl);
	DDX_Control(pDX, IDC_ED_M11, m_M11Ctrl);
	DDX_Control(pDX, IDC_ED_M12, m_M12Ctrl);
	DDX_Control(pDX, IDC_ED_M13, m_M13Ctrl);
	DDX_Control(pDX, IDC_ED_M20, m_M20Ctrl);
	DDX_Control(pDX, IDC_ED_M21, m_M21Ctrl);
	DDX_Control(pDX, IDC_ED_M22, m_M22Ctrl);
	DDX_Control(pDX, IDC_ED_M23, m_M23Ctrl);
	DDX_Control(pDX, IDC_ED_M30, m_M30Ctrl);
	DDX_Control(pDX, IDC_ED_M31, m_M31Ctrl);
	DDX_Control(pDX, IDC_ED_M32, m_M32Ctrl);
	DDX_Control(pDX, IDC_ED_M33, m_M33Ctrl);
	DDX_Control(pDX, IDC_ED_IM00, m_IM00Ctrl);
	DDX_Control(pDX, IDC_ED_IM01, m_IM01Ctrl);
	DDX_Control(pDX, IDC_ED_IM02, m_IM02Ctrl);
	DDX_Control(pDX, IDC_ED_IM03, m_IM03Ctrl);
	DDX_Control(pDX, IDC_ED_IM10, m_IM10Ctrl);
	DDX_Control(pDX, IDC_ED_IM11, m_IM11Ctrl);
	DDX_Control(pDX, IDC_ED_IM12, m_IM12Ctrl);
	DDX_Control(pDX, IDC_ED_IM13, m_IM13Ctrl);
	DDX_Control(pDX, IDC_ED_IM20, m_IM20Ctrl);
	DDX_Control(pDX, IDC_ED_IM21, m_IM21Ctrl);
	DDX_Control(pDX, IDC_ED_IM22, m_IM22Ctrl);
	DDX_Control(pDX, IDC_ED_IM23, m_IM23Ctrl);
	DDX_Control(pDX, IDC_ED_IM30, m_IM30Ctrl);
	DDX_Control(pDX, IDC_ED_IM31, m_IM31Ctrl);
	DDX_Control(pDX, IDC_ED_IM32, m_IM32Ctrl);
	DDX_Control(pDX, IDC_ED_IM33, m_IM33Ctrl);
	DDX_Control(pDX, IDC_ED_SHAPESET, m_ShapeSetNameCtrl);
	DDX_Control(pDX, IDC_ED_JOINT, m_JointInfoCtrl);
	DDX_Control(pDX, IDC_BTN_SHAPESETCHANGEVALUE, m_BtnShapeSetChangeCtrl );
	DDX_Control(pDX, IDC_BTN_JOINTCHANGEVALUE, m_BtnJointChangeCtrl );

}

BOOL CSkeletonSpecDialog::Execute( i3LOD * pLOD)
{
	m_pLOD = pLOD;

	if( DoModal() == IDCANCEL)
		return FALSE;

	return TRUE;
}

void CSkeletonSpecDialog::UpdateSkeleton( INT32 idx, i3Skeleton * pSkel)
{
	char conv[128];

	m_SkelListCtrl.SetItemData( idx, (DWORD_PTR) pSkel);

	// Bone Count
	sprintf( conv, "%d", pSkel->getBoneCount());
	m_SkelListCtrl.SetItemText( idx, 1, conv);
}

void CSkeletonSpecDialog::AssignSkeleton(void)
{
	INT32 idx;
	i3Skeleton * pSkel;
	char conv[256];

	//for( i = 0; i < (INT32) m_pLOD->getSkeletonCount(); i++)
	{
		pSkel = m_pLOD->getSkeleton();

		if( pSkel != NULL)
		{
			sprintf( conv, "Skeleton\n");
			idx = m_SkelListCtrl.InsertItem( m_SkelListCtrl.GetItemCount(), conv);

			UpdateSkeleton( idx, pSkel);
		}
	}
}

BEGIN_MESSAGE_MAP(CSkeletonSpecDialog, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SKEL_LIST, OnNMClickSkelList)
	ON_NOTIFY(NM_CLICK, IDC_BONE_TREE, OnNMClickBoneTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BONE_TREE, OnTvnSelchangedBoneTree)
	ON_BN_CLICKED(IDC_BTN_SHAPESETCHANGEVALUE, OnBnClickedBtnShapesetchangevalue)
	ON_BN_CLICKED(IDC_BTN_JOINTCHANGEVALUE, OnBnClickedBtnJointchangevalue)
END_MESSAGE_MAP()


// CSkeletonSpecDialog message handlers

BOOL CSkeletonSpecDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_SkelListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_SkelListCtrl.InsertColumn( 0, "Skeleton", LVCFMT_LEFT, 150);
		m_SkelListCtrl.InsertColumn( 1, "# Bones", LVCFMT_LEFT, 80);
	}

	AssignSkeleton();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

INT32 CSkeletonSpecDialog::GetSelectedSkeleton(void)
{
	POSITION pos = m_SkelListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
		return m_SkelListCtrl.GetNextSelectedItem( pos);

	return -1;
}

void CSkeletonSpecDialog::OnNMClickSkelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx = GetSelectedSkeleton();

	if( idx == -1)
	{
		SelectSkeleton( NULL);
		return;
	}

	i3Skeleton * pSkel = (i3Skeleton *) m_SkelListCtrl.GetItemData( idx);
	SelectSkeleton( pSkel);

	*pResult = 0;
}

void CSkeletonSpecDialog::SelectSkeleton( i3Skeleton * pSkel)
{
	m_pSkeleton = pSkel;

	m_BoneTreeCtrl.DeleteAllItems();

	if( m_pSkeleton != NULL)
		_Rec_SetBoneTree( TVI_ROOT, 0);
}

void CSkeletonSpecDialog::_Rec_SetBoneTree( HTREEITEM hParent, INT32 idx)
{
	INT32 i, j, prevDepth = 0;
	HTREEITEM hItem = NULL;
	i3Bone * pBone;

	for( i = 0; i < m_pSkeleton->getBoneCount(); i++)
	{
		pBone = m_pSkeleton->getBone( i);

		if( pBone->getDepth() < (UINT32) prevDepth)
		{
			for( j = 0; j < (INT32) (prevDepth - pBone->getDepth()); j++)
			{
				hParent = m_BoneTreeCtrl.GetParentItem( hParent);
				if( hParent == NULL)
					hParent = TVI_ROOT;
			}
		}
		else if( pBone->getDepth() > (UINT32) prevDepth)
		{
			hParent = hItem;
		}

		prevDepth = pBone->getDepth();

		hItem = m_BoneTreeCtrl.InsertItem( pBone->getName(), 0, 0, hParent);

		m_BoneTreeCtrl.SetItemData( hItem, (DWORD_PTR) pBone);
	}
}

void CSkeletonSpecDialog::OnNMClickBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CSkeletonSpecDialog::OnTvnSelchangedBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();
	char conv[256];

	if( hSelItem == NULL)
	{
		// Bone Matrix
		m_M00Ctrl.SetWindowText( "");
		m_M01Ctrl.SetWindowText( "");
		m_M02Ctrl.SetWindowText( "");
		m_M03Ctrl.SetWindowText( "");

		m_M10Ctrl.SetWindowText( "");
		m_M11Ctrl.SetWindowText( "");
		m_M12Ctrl.SetWindowText( "");
		m_M13Ctrl.SetWindowText( "");

		m_M20Ctrl.SetWindowText( "");
		m_M21Ctrl.SetWindowText( "");
		m_M22Ctrl.SetWindowText( "");
		m_M23Ctrl.SetWindowText( "");

		m_M30Ctrl.SetWindowText( "");
		m_M31Ctrl.SetWindowText( "");
		m_M32Ctrl.SetWindowText( "");
		m_M33Ctrl.SetWindowText( "");

		// Inverse Bone Matrix
		m_IM00Ctrl.SetWindowText( "");
		m_IM01Ctrl.SetWindowText( "");
		m_IM02Ctrl.SetWindowText( "");
		m_IM03Ctrl.SetWindowText( "");

		m_IM10Ctrl.SetWindowText( "");
		m_IM11Ctrl.SetWindowText( "");
		m_IM12Ctrl.SetWindowText( "");
		m_IM13Ctrl.SetWindowText( "");

		m_IM20Ctrl.SetWindowText( "");
		m_IM21Ctrl.SetWindowText( "");
		m_IM22Ctrl.SetWindowText( "");
		m_IM23Ctrl.SetWindowText( "");

		m_IM30Ctrl.SetWindowText( "");
		m_IM31Ctrl.SetWindowText( "");
		m_IM32Ctrl.SetWindowText( "");
		m_IM33Ctrl.SetWindowText( "");

		m_ShapeSetNameCtrl.SetWindowText( "(none)");
		m_JointInfoCtrl.SetWindowText( "(none)");

		m_BtnShapeSetChangeCtrl.EnableWindow( FALSE );
		m_BtnJointChangeCtrl.EnableWindow( FALSE );
	}
	else
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		// Bone Matrix
		i3String::ftoa( i3Matrix::Get11( pBone->getMatrix()), conv);
		m_M00Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get12( pBone->getMatrix()), conv);
		m_M01Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get13( pBone->getMatrix()), conv);
		m_M02Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get14( pBone->getMatrix()), conv);
		m_M03Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3String::ftoa( i3Matrix::Get21( pBone->getMatrix()), conv);
		m_M10Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get22( pBone->getMatrix()), conv);
		m_M11Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get23( pBone->getMatrix()), conv);
		m_M12Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get24( pBone->getMatrix()), conv);
		m_M13Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3String::ftoa( i3Matrix::Get31( pBone->getMatrix()), conv);
		m_M20Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get32( pBone->getMatrix()), conv);
		m_M21Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get33( pBone->getMatrix()), conv);
		m_M22Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get34( pBone->getMatrix()), conv);
		m_M23Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3String::ftoa( i3Matrix::Get41( pBone->getMatrix()), conv);
		m_M30Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get42( pBone->getMatrix()), conv);
		m_M31Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get43( pBone->getMatrix()), conv);
		m_M32Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3String::ftoa( i3Matrix::Get44( pBone->getMatrix()), conv);
		m_M33Ctrl.SetWindowText( conv);		conv[0] = 0;

		INT32 idx = m_pSkeleton->FindBoneByName( pBone->getName());
		I3ASSERT( idx != -1);

		if( m_pSkeleton->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkeleton->getShapeSet( idx);
			char conv[256];

			sprintf( conv, "%s", pShapeSet->GetName());

			m_ShapeSetNameCtrl.SetWindowText( conv);

			m_BtnShapeSetChangeCtrl.EnableWindow();

			if( pShapeSet->getJointInfo() != NULL)
			{
				i3PhysixJointInfo * pJointInfo = pShapeSet->getJointInfo();

				sprintf( conv, "%p", pJointInfo);

				m_JointInfoCtrl.SetWindowText( conv);
				m_BtnJointChangeCtrl.EnableWindow();
			}
			else
			{
				m_JointInfoCtrl.SetWindowText( "(none)");
				m_BtnJointChangeCtrl.EnableWindow( FALSE );
			}
		}
		else
		{
			m_ShapeSetNameCtrl.SetWindowText( "(none)");
			m_JointInfoCtrl.SetWindowText( "(none)");

			m_BtnShapeSetChangeCtrl.EnableWindow( FALSE );
			m_BtnJointChangeCtrl.EnableWindow( FALSE );
		}
	}

	*pResult = 0;
}

void CSkeletonSpecDialog::OnBnClickedBtnShapesetchangevalue()
{
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();

	if( hSelItem != NULL )
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		INT32 idx = m_pSkeleton->FindBoneByName( pBone->getName());
		I3ASSERT( idx != -1);

		if( m_pSkeleton->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkeleton->getShapeSet( idx);

			CShapeSetSpecDialog dlg;
			dlg.Execute( pShapeSet);
		}
	}
}

void CSkeletonSpecDialog::OnBnClickedBtnJointchangevalue()
{
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();

	if( hSelItem != NULL )
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		INT32 idx = m_pSkeleton->FindBoneByName( pBone->getName());
		I3ASSERT( idx != -1);

		if( m_pSkeleton->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkeleton->getShapeSet( idx);

			CJointD6SpecDialog dlg;
			dlg.Execute( pShapeSet);
		}
	}
}
