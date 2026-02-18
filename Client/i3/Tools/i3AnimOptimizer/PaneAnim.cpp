// PaneAnim.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "PaneAnim.h"
#include ".\paneanim.h"
#include "GlobVar.h"

// CPaneAnim dialog

IMPLEMENT_DYNAMIC(CPaneAnim, CDialog)
CPaneAnim::CPaneAnim(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneAnim::IDD, pParent)
{
}

CPaneAnim::~CPaneAnim()
{
}

void CPaneAnim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANIMLIST, m_AnimListCtrl);
}


BEGIN_MESSAGE_MAP(CPaneAnim, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_ANIMLIST, OnNMDblclkAnimlist)
END_MESSAGE_MAP()


// CPaneAnim message handlers

BOOL CPaneAnim::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_AnimListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 160);
		m_AnimListCtrl.InsertColumn( 1, "# Bone", LVCFMT_LEFT, 70);
		m_AnimListCtrl.InsertColumn( 2, "Duration", LVCFMT_LEFT, 70);

		m_AnimListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	RegisterUpdate( m_hWnd, i3Animation::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneAnim::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_AnimListCtrl.m_hWnd))
	{
		CRect rt;

		m_AnimListCtrl.GetWindowRect( &rt);
		ScreenToClient( &rt);

		m_AnimListCtrl.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, cy - rt.top - 3, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CPaneAnim::UpdateAnim( INT32 idx, i3Animation * pAnim)
{
	char conv[128];

	m_AnimListCtrl.SetItemData( idx, (DWORD_PTR) pAnim);

	// Name
	
	// Bone Count
	sprintf( conv, "%d\n", pAnim->GetTrackCount());
	m_AnimListCtrl.SetItemText( idx, 1, conv);

	// Duration
	i3String::ftoa( pAnim->GetDuration(), conv);
	m_AnimListCtrl.SetItemText( idx, 2, conv);
}

void CPaneAnim::UpdateAllAnim(void)
{
	INT32 i, idx;
	i3Animation * pAnim;

	m_AnimListCtrl.DeleteAllItems();

	for( i = 0; i < g_Project.GetAnimCount(); i++)
	{
		pAnim = g_Project.GetAnim( i);

		idx = m_AnimListCtrl.InsertItem( 0, pAnim->GetName(), 0);

		UpdateAnim( idx, pAnim);
	}
}

INT32 CPaneAnim::FindAnim( i3Animation * pAnim)
{
	INT32 i;
	i3Animation * pTemp;

	for( i = 0; i < m_AnimListCtrl.GetItemCount(); i++)
	{
		pTemp = (i3Animation *) m_AnimListCtrl.GetItemData( i);

		if( pTemp != NULL)
			return i;
	}

	return -1;
}

void CPaneAnim::OnAddAnim( i3Animation * pAnim)
{
	INT32 idx;

	idx = m_AnimListCtrl.InsertItem( 0, pAnim->GetName(), 0);

	UpdateAnim( idx, pAnim);
}

void CPaneAnim::OnRemoveAnim( i3Animation * pAnim)
{
	if( pAnim != NULL)
	{
		INT32 idx = FindAnim( pAnim);

		m_AnimListCtrl.DeleteItem( idx);
	}
	else
	{
		UpdateAllAnim();
	}
}

void CPaneAnim::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		I3ASSERT( pInfo->m_pObject != NULL);

		OnAddAnim( (i3Animation *) pInfo->m_pObject);
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		OnRemoveAnim( (i3Animation *) pInfo->m_pObject);
	}
}

LRESULT CPaneAnim::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO * ) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPaneAnim::OnNMDblclkAnimlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_AnimListCtrl.GetFirstSelectedItemPosition();
	INT32 idx;

	if( pos == NULL)
		return;

	idx = m_AnimListCtrl.GetNextSelectedItem( pos);

	i3Animation * pAnim = (i3Animation *) m_AnimListCtrl.GetItemData( idx);

	g_Project.PlayAnim( pAnim);
	*pResult = 0;
}
