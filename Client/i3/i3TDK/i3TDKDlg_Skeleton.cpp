// i3TDKDlg_Skeleton.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlg_Skeleton.h"
#include ".\i3tdkdlg_skeleton.h"
#include "i3TDKSpec_ShapeSet.h"
#include "i3TDKSpec_Joint.h"
#include "i3Base/string/ext/ftoa.h"
// i3TDKDlg_Skeleton dialog

IMPLEMENT_DYNAMIC(i3TDKDlg_Skeleton, CDialog)


void i3TDKDlg_Skeleton::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_SKEL, m_BoneTreeCtrl);
	DDX_Control(pDX, IDC_TDK_ED_M00, m_M00Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M01, m_M01Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M02, m_M02Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M03, m_M03Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M10, m_M10Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M11, m_M11Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M12, m_M12Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M13, m_M13Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M20, m_M20Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M21, m_M21Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M22, m_M22Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M23, m_M23Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M30, m_M30Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M31, m_M31Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M32, m_M32Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_M33, m_M33Ctrl);
	DDX_Control(pDX, IDC_TDK_ED_SHAPESET, m_ShapeSetNameCtrl);
	DDX_Control(pDX, IDC_TDK_ED_JOINT, m_JointInfoCtrl);
	DDX_Control(pDX, IDC_TDK_BTN_SHAPESET, m_BtnShapeSetChangeCtrl );
	DDX_Control(pDX, IDC_TDK_BTN_JOINT, m_BtnJointChangeCtrl );
	DDX_Control(pDX, IDC_TDK_ED_NAME, m_ED_Name);
}

bool i3TDKDlg_Skeleton::Execute( i3Skeleton * pSkel)
{
	m_pSkel = pSkel;

	i3TDK::SetResInstance();

	INT32 rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
	{
		return false;
	}

	return true;
}

void i3TDKDlg_Skeleton::UpdateSkeleton(void)
{
	m_BoneTreeCtrl.DeleteAllItems();

	SelectBone( nullptr);

	if( m_pSkel != nullptr)
		_Rec_SetBoneTree( TVI_ROOT, 0);
}

void i3TDKDlg_Skeleton::_Rec_SetBoneTree( HTREEITEM hParent, INT32 idx)
{
	INT32 i, j, prevDepth = 0;
	HTREEITEM hItem = nullptr;
	i3Bone * pBone;

	for( i = 0; i < m_pSkel->getBoneCount(); i++)
	{
		pBone = m_pSkel->getBone( i);

		if( pBone->getDepth() < (UINT32) prevDepth)
		{
			for( j = 0; j < (INT32) (prevDepth - pBone->getDepth()); j++)
			{
				hParent = m_BoneTreeCtrl.GetParentItem( hParent);
				if( hParent == nullptr)
					hParent = TVI_ROOT;
			}
		}
		else if( pBone->getDepth() > (UINT32) prevDepth)
		{
			hParent = hItem;
		}

		prevDepth = pBone->getDepth();

#ifdef _UNICODE
		i3::stack_wstring wstrBoneName;	i3::mb_to_utf16(pBone->getName(), wstrBoneName);
		const wchar_t* szBoneName = wstrBoneName.c_str();
#else
		const char* szBoneName = pBone->getName();
#endif
		hItem = m_BoneTreeCtrl.InsertItem( szBoneName, 0, 0, hParent);

		m_BoneTreeCtrl.SetItemData( hItem, (DWORD_PTR) pBone);
	}
}

BEGIN_MESSAGE_MAP(i3TDKDlg_Skeleton, CDialog)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M00, OnEnKillfocusTdkEdM00)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M01, OnEnKillfocusTdkEdM01)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M02, OnEnKillfocusTdkEdM02)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M03, OnEnKillfocusTdkEdM03)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M10, OnEnKillfocusTdkEdM10)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M11, OnEnKillfocusTdkEdM11)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M12, OnEnKillfocusTdkEdM12)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M13, OnEnKillfocusTdkEdM13)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M20, OnEnKillfocusTdkEdM20)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M21, OnEnKillfocusTdkEdM21)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M22, OnEnKillfocusTdkEdM22)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M23, OnEnKillfocusTdkEdM23)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M30, OnEnKillfocusTdkEdM30)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M31, OnEnKillfocusTdkEdM31)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M32, OnEnKillfocusTdkEdM32)
	ON_EN_KILLFOCUS(IDC_TDK_ED_M33, OnEnKillfocusTdkEdM33)
	ON_BN_CLICKED(IDC_TDK_BTN_SHAPESET, OnBnClickedTdkBtnShapeset)
	ON_BN_CLICKED(IDC_TDK_BTN_JOINT, OnBnClickedTdkBtnJoint)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TDK_SKEL, OnTvnSelchangedTdkSkel)
	ON_NOTIFY( NM_RCLICK, IDC_TDK_SKEL, OnRclickTree)
END_MESSAGE_MAP()


// i3TDKDlg_Skeleton message handlers

BOOL i3TDKDlg_Skeleton::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		UpdateSkeleton();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlg_Skeleton::OnOK()
{
	CDialog::OnOK();
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM00()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM01()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM02()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM03()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM10()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM11()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM12()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM13()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM20()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM21()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM22()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM23()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM30()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM31()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM32()
{
}

void i3TDKDlg_Skeleton::OnEnKillfocusTdkEdM33()
{
}

void i3TDKDlg_Skeleton::OnBnClickedTdkBtnShapeset()
{
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();

	if( hSelItem != nullptr )
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		INT32 idx = m_pSkel->FindBoneByName( pBone->getName());
		I3ASSERT_RETURN( idx != -1);

		if( m_pSkel->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkel->getShapeSet( idx);

			i3TDKSpec_ShapeSet dlg;
			dlg.Execute( pShapeSet);
		}
	}
}

void i3TDKDlg_Skeleton::OnBnClickedTdkBtnJoint()
{
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();

	if( hSelItem != nullptr )
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		INT32 idx = m_pSkel->FindBoneByName( pBone->getName());
		I3ASSERT_RETURN( idx != -1);

		if( m_pSkel->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkel->getShapeSet( idx);

			i3TDKSpec_Joint dlg;
			dlg.Execute( pShapeSet);
		}
	}
}

void i3TDKDlg_Skeleton::SelectBone( i3Bone * pBone)
{
	TCHAR conv[256];

	if( pBone == nullptr)
	{
		// Name
		m_ED_Name.SetWindowText( _T("(none)") );

		// Bone Matrix
		m_M00Ctrl.SetWindowText( _T("") );
		m_M01Ctrl.SetWindowText( _T("") );
		m_M02Ctrl.SetWindowText( _T("") );
		m_M03Ctrl.SetWindowText( _T("") );

		m_M10Ctrl.SetWindowText( _T("") );
		m_M11Ctrl.SetWindowText( _T("") );
		m_M12Ctrl.SetWindowText( _T("") );
		m_M13Ctrl.SetWindowText( _T("") );

		m_M20Ctrl.SetWindowText( _T("") );
		m_M21Ctrl.SetWindowText( _T("") );
		m_M22Ctrl.SetWindowText( _T("") );
		m_M23Ctrl.SetWindowText( _T("") );

		m_M30Ctrl.SetWindowText( _T("") );
		m_M31Ctrl.SetWindowText( _T("") );
		m_M32Ctrl.SetWindowText( _T("") );
		m_M33Ctrl.SetWindowText( _T("") );

		m_ShapeSetNameCtrl.SetWindowText( _T("(none)") );
		m_JointInfoCtrl.SetWindowText( _T("(none)") );

		m_BtnShapeSetChangeCtrl.EnableWindow( FALSE );
		m_BtnJointChangeCtrl.EnableWindow( FALSE );
	}
	else
	{
		// Name
#ifdef _UNICODE
		i3::stack_wstring wstrBoneName;		i3::mb_to_utf16(pBone->getName(), wstrBoneName);
		const wchar_t* szBoneName = wstrBoneName.c_str();
#else
		const char* szBoneName = pBone->getName();
#endif
		m_ED_Name.SetWindowText( szBoneName );

		// Bone Matrix
		i3::ftoa( i3Matrix::Get11( pBone->getMatrix()), conv);
		m_M00Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get12( pBone->getMatrix()), conv);
		m_M01Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get13( pBone->getMatrix()), conv);
		m_M02Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get14( pBone->getMatrix()), conv);
		m_M03Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3::ftoa( i3Matrix::Get21( pBone->getMatrix()), conv);
		m_M10Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get22( pBone->getMatrix()), conv);
		m_M11Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get23( pBone->getMatrix()), conv);
		m_M12Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get24( pBone->getMatrix()), conv);
		m_M13Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3::ftoa( i3Matrix::Get31( pBone->getMatrix()), conv);
		m_M20Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get32( pBone->getMatrix()), conv);
		m_M21Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get33( pBone->getMatrix()), conv);
		m_M22Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get34( pBone->getMatrix()), conv);
		m_M23Ctrl.SetWindowText( conv);		conv[0] = 0;

		i3::ftoa( i3Matrix::Get41( pBone->getMatrix()), conv);
		m_M30Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get42( pBone->getMatrix()), conv);
		m_M31Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get43( pBone->getMatrix()), conv);
		m_M32Ctrl.SetWindowText( conv);		conv[0] = 0;
		i3::ftoa( i3Matrix::Get44( pBone->getMatrix()), conv);
		m_M33Ctrl.SetWindowText( conv);		conv[0] = 0;

		INT32 idx = m_pSkel->FindBoneByName( pBone->getName());
		I3ASSERT_RETURN( idx != -1);

		if( m_pSkel->isShapeSet( idx))
		{
			i3PhysixShapeSet * pShapeSet = m_pSkel->getShapeSet( idx);
		
			i3::snprintf( conv, _countof(conv), _T("%s"), pShapeSet->GetName());

			m_ShapeSetNameCtrl.SetWindowText( conv);

			m_BtnShapeSetChangeCtrl.EnableWindow();

			if( pShapeSet->getJointInfo() != nullptr)
			{
				i3PhysixJointInfo * pJointInfo = pShapeSet->getJointInfo();

				i3::snprintf( conv, _countof(conv), _T("%p"), pJointInfo);

				m_JointInfoCtrl.SetWindowText( conv);
				m_BtnJointChangeCtrl.EnableWindow();
			}
			else
			{
				m_JointInfoCtrl.SetWindowText( _T("(none)") );
				m_BtnJointChangeCtrl.EnableWindow( FALSE );
			}
		}
		else
		{
			m_ShapeSetNameCtrl.SetWindowText( _T("(none)") );
			m_JointInfoCtrl.SetWindowText( _T("(none)") );

			m_BtnShapeSetChangeCtrl.EnableWindow( FALSE );
			m_BtnJointChangeCtrl.EnableWindow( FALSE );
		}
	}
}

void i3TDKDlg_Skeleton::OnTvnSelchangedTdkSkel(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hSelItem = m_BoneTreeCtrl.GetSelectedItem();
	
	if( hSelItem == nullptr)
	{
		SelectBone( nullptr);
	}
	else
	{
		i3Bone * pBone = (i3Bone *) m_BoneTreeCtrl.GetItemData( hSelItem);

		SelectBone( pBone);
	}

	*pResult = 0;
}
static BOOL _ExpandProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	if( pData == nullptr)
		pCtrl->Expand( hItem, TVE_EXPAND);
	else
		pCtrl->Expand( hItem, TVE_COLLAPSE);

	return TRUE;
}
void i3TDKDlg_Skeleton :: OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = nullptr;
	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_BoneTreeCtrl.ScreenToClient( &cpt);

		hSel = m_BoneTreeCtrl.HitTest( cpt, &flag);

		if ((hSel != nullptr) && (TVHT_ONITEM & flag))
		{
			m_BoneTreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle( g_hInstTDK);

		Menu.LoadMenu( IDR_POPUP_SKELETON);

		AfxSetResourceHandle( hOld);
	}

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == nullptr)
	{
		pPopup->EnableMenuItem( ID_SKELETON_EXPAND, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SKELETON_COLLAPSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
	}



	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd *) this, nullptr);

	

	switch( cmd)
	{
	case ID_SKELETON_EXPAND :					Expand( hSel);	break;
	case ID_SKELETON_COLLAPSE :					Collapse( hSel);	break;

	default :
		break;
	}


	*pResult = 1;
}
void i3TDKDlg_Skeleton::Expand( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _ExpandProc, nullptr);
}

void i3TDKDlg_Skeleton::Collapse( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_BoneTreeCtrl, _ExpandProc, (void *) 1);
}