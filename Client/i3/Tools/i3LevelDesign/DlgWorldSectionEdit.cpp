// DlgWorldSectionEdit.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgWorldSectionEdit.h"
#include "DlgWorldSectionInput.h"
#include ".\dlgworldsectionedit.h"


// CDlgWorldSectionEdit dialog

IMPLEMENT_DYNAMIC(CDlgWorldSectionEdit, CDialog)
CDlgWorldSectionEdit::CDlgWorldSectionEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWorldSectionEdit::IDD, pParent)
{
	m_pTable = NULL;

	m_pWorld	= NULL;
}

CDlgWorldSectionEdit::~CDlgWorldSectionEdit()
{
}

void CDlgWorldSectionEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SECTION_LIST, m_SectionListCtrl);
	DDX_Control(pDX, IDC_SHADOW_MASK, m_ShadowMaskListCtrl);
	DDX_Control(pDX, IDC_VISIBILITY_MASK, m_VisibleMaskListCtrl);
}

bool CDlgWorldSectionEdit::Execute( i3WorldSectionTable * pTable)
{
	m_pTable = pTable;

	m_bEditted = FALSE;
	if( DoModal() == IDOK)
		return true;

	return false;
}

void CDlgWorldSectionEdit::_RemoveAllSection(void)
{
	INT32 i;
	i3::pack::WORLD_SECTION_INFO * pInfo;

	for( i = 0; i < m_SectionListCtrl.GetItemCount(); i++)
	{
		pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( i);

		delete pInfo;
	}

	m_SectionListCtrl.DeleteAllItems();
}

void CDlgWorldSectionEdit::GetSelectedItems( i3::vector<i3::pack::WORLD_SECTION_INFO *>& List)
{
	POSITION pos;
	i3::pack::WORLD_SECTION_INFO * pInfo;

	pos = m_SectionListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		INT32 idx = m_SectionListCtrl.GetNextSelectedItem( pos);

		pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( idx);

		List.push_back( pInfo);
	}
}

void CDlgWorldSectionEdit::_SelectSection(void)
{
	i3::pack::WORLD_SECTION_INFO * pInfo;
	UINT64	shadow = 0, visible = 0, mask, shadowValid, visibleValid;
	i3::vector<i3::pack::WORLD_SECTION_INFO *> list;

	GetSelectedItems( list);

	shadowValid = 0xFFFFFFFFFFFFFFFFL;
	visibleValid = 0xFFFFFFFFFFFFFFFFL;

	for(size_t i = 0; i < list.size(); i++)
	{
		pInfo = list[i];

		if( i == 0 )
		{
			shadow = pInfo->m_ShadowMask;
			visible = pInfo->m_VisibilityMask;
		}
		else
		{
			UINT64 diff;

			diff = shadow ^ pInfo->m_ShadowMask; // XOR
			shadowValid = shadowValid & (~diff);

			diff = visible ^ pInfo->m_VisibilityMask; // XOR
			visibleValid = visibleValid & (~diff);

			shadow = shadow & pInfo->m_ShadowMask;
			visible = visible & pInfo->m_VisibilityMask;
		}
	}

	mask = 1;

	for(INT32 i = 0; i < m_SectionListCtrl.GetItemCount(); i++)
	{
		if( shadowValid & mask)
		{
			if( shadow & mask)
				SetShadowCheck( i, CHECKED);
			else
				SetShadowCheck( i, UNCHECKED);
		}
		else
		{
			SetShadowCheck( i, NA);
		}

		if( visibleValid & mask)
		{
			if( visible & mask)
				SetVisibilityCheck( i, CHECKED);
			else
				SetVisibilityCheck( i, UNCHECKED);
		}
		else
		{
			SetVisibilityCheck( i, NA);
		}

		mask = mask << 1;
	}
}

void CDlgWorldSectionEdit::_UpdateSection( INT32 idx, i3::pack::WORLD_SECTION_INFO * pInfo)
{
	m_SectionListCtrl.SetItemData( idx, (DWORD_PTR) pInfo);

	// Name
	m_SectionListCtrl.SetItemText( idx, 1, pInfo->m_szName);
}

void CDlgWorldSectionEdit::_AssignValue(void)
{
	INT32 i, idx;
	i3::pack::WORLD_SECTION_INFO * pSrc, * pDest;
	char conv[128];

	_RemoveAllSection();

	for( i = 0; i < m_pTable->getCount(); i++)
	{
		pSrc = m_pTable->getWorldSection( i);

		pDest = new i3::pack::WORLD_SECTION_INFO;

		memcpy( pDest, pSrc, sizeof(i3::pack::WORLD_SECTION_INFO));

		sprintf( conv, "%d", i);
		idx = m_SectionListCtrl.InsertItem( i, conv, 0);

		_UpdateSection( idx, pDest);
	}

	_OnUpdateAllWorldSection();
}

void CDlgWorldSectionEdit::_OnNewWorldSection(void)
{
	i3::pack::WORLD_SECTION_INFO * pInfo;
	INT32 cnt = m_SectionListCtrl.GetItemCount();

	pInfo = new i3::pack::WORLD_SECTION_INFO;

	i3mem::FillZero( pInfo, sizeof(i3::pack::WORLD_SECTION_INFO));

	sprintf( pInfo->m_szName, "Section%d", cnt);

	char conv[128];

	sprintf( conv, "%d", cnt);

	INT32 idx = m_SectionListCtrl.InsertItem( cnt, conv, 0);

	_UpdateSection( idx, pInfo);

	_OnUpdateAllWorldSection();

	m_SectionListCtrl.SetItemState( idx, 0xFFFFFFFF, LVIS_SELECTED);
	_SelectSection();


	m_bEditted = TRUE;
}

void CDlgWorldSectionEdit::_OnDeleteWorldSection(void)
{
	i3::pack::WORLD_SECTION_INFO * pInfo;
	POSITION pos;
	INT32 idx, cnt = 0;

	pos = m_SectionListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		idx = m_SectionListCtrl.GetNextSelectedItem( pos);

		pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( idx);

		delete pInfo;

		m_SectionListCtrl.DeleteItem( idx);

		cnt++;
	}

	if( cnt > 0)
	{
		_SelectSection();

		_OnUpdateAllWorldSection();
	}

	m_bEditted = TRUE;
}

BEGIN_MESSAGE_MAP(CDlgWorldSectionEdit, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_SECTION_LIST, OnNMRclickSectionList)
	ON_NOTIFY(NM_CLICK, IDC_SECTION_LIST, OnNMClickSectionList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_SHADOW_MASK, OnNMKillfocusShadowMask)
	ON_NOTIFY(NM_KILLFOCUS, IDC_VISIBILITY_MASK, OnNMKillfocusVisibilityMask)
	ON_NOTIFY(NM_DBLCLK, IDC_SECTION_LIST, OnNMDblclkSectionList)
	ON_NOTIFY(NM_CLICK, IDC_SHADOW_MASK, &CDlgWorldSectionEdit::OnNMClickShadowMask)
	ON_NOTIFY(NM_CLICK, IDC_VISIBILITY_MASK, &CDlgWorldSectionEdit::OnNMClickVisibilityMask)
	ON_BN_CLICKED(IDC_BTN_SELALL_SHADOW, &CDlgWorldSectionEdit::OnBnClickedBtnSelallShadow)
	ON_BN_CLICKED(IDC_BTN_UNSELALL_SHADOW, &CDlgWorldSectionEdit::OnBnClickedBtnUnselallShadow)
	ON_BN_CLICKED(IDC_BTN_SELALL_VISIBLE, &CDlgWorldSectionEdit::OnBnClickedBtnSelallVisible)
	ON_BN_CLICKED(IDC_BTN_UNSELALL_VISIBLE, &CDlgWorldSectionEdit::OnBnClickedBtnUnselallVisible)
END_MESSAGE_MAP()


// CDlgWorldSectionEdit message handlers

BOOL CDlgWorldSectionEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon;

		m_ChkImageList.Create( 13, 13, ILC_COLOR32, 3, 3);

		hIcon = AfxGetApp()->LoadIcon( IDI_ICO_CHK_UNCHECKED);
		m_ChkImageList.Add( hIcon);

		hIcon = AfxGetApp()->LoadIcon( IDI_ICO_CHK_CHECKED);
		m_ChkImageList.Add( hIcon);

		hIcon = AfxGetApp()->LoadIcon( IDI_ICO_CHK_NA);
		m_ChkImageList.Add( hIcon);
	}

	{
		m_SectionListCtrl.InsertColumn( 0, "ID", LVCFMT_LEFT, 60);
		m_SectionListCtrl.InsertColumn( 1, "Name", LVCFMT_LEFT, 230);
		m_SectionListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_ShadowMaskListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_ShadowMaskListCtrl.InsertColumn( 0, "ID", LVCFMT_LEFT, 60);
		m_ShadowMaskListCtrl.InsertColumn( 1, "Name", LVCFMT_LEFT, 230);
		m_ShadowMaskListCtrl.SetImageList( &m_ChkImageList, LVSIL_SMALL);

		m_VisibleMaskListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_VisibleMaskListCtrl.InsertColumn( 0, "ID", LVCFMT_LEFT, 60);
		m_VisibleMaskListCtrl.InsertColumn( 1, "Name", LVCFMT_LEFT, 230);
		m_VisibleMaskListCtrl.SetImageList( &m_ChkImageList, LVSIL_SMALL);
	}

	_AssignValue();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWorldSectionEdit::OnOK()
{
	// World-Section Table의 재구성
	if( m_bEditted == TRUE && m_pWorld != NULL)
	{
		i3World * pWorld = m_pWorld->getWorld();
		I3ASSERT( pWorld != NULL);
		if( pWorld->getWorldSectionTable() != NULL)
		{
			I3ASSERT( pWorld->getWorldSectionTable() == m_pTable);
		}

		i3LevelUndo::ADD_Edit( m_pWorld);
	}

	if( m_pTable != NULL)
	{
		INT32 i, cnt;

		cnt = m_SectionListCtrl.GetItemCount();

		m_pTable->SetSectionCount( cnt);

		for( i = 0; i < cnt; i++)
		{
			i3::pack::WORLD_SECTION_INFO * pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( i);

			i3mem::Copy( m_pTable->getWorldSection( i), pInfo, sizeof(i3::pack::WORLD_SECTION_INFO));
		}
	}

	m_bEditted	= FALSE;
	m_pWorld	= NULL;

	CDialog::OnOK();
}

void CDlgWorldSectionEdit::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_pWorld	= NULL;
	m_bEditted	= FALSE;

	CDialog::OnCancel();
}

void CDlgWorldSectionEdit::OnNMRclickSectionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu	Menu, *  pPopup;
	UINT32 cmd;
	CPoint pt;

	GetCursorPos( &pt);

	Menu.LoadMenu( IDR_POPUP_WORLDSECTION);

	pPopup = Menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_WORLDSECTION_NEW :
			_OnNewWorldSection();
			break;

		case ID_WORLDSECTION_DELETE :
			_OnDeleteWorldSection();
			break;

		default:
			break;
	}

	*pResult = 1;
}

void CDlgWorldSectionEdit::OnNMKillfocusShadowMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 cnt;
	CHK_STATE state;
	UINT64 mask = 0, validMask = 0;

	*pResult = 0;

	cnt = m_ShadowMaskListCtrl.GetItemCount();

	for(INT32 i = 0; i < cnt; i++)
	{
		state = GetShadowCheck( i);

		switch( state)
		{
			case CHECKED :
				mask |= ((UINT64)1L << i);
				validMask |= ((UINT64)1L << i);
				break;

			case UNCHECKED :
				validMask |= ((UINT64)1L << i);
				break;
		}
	}

	{
		i3::vector<i3::pack::WORLD_SECTION_INFO *> list;

		GetSelectedItems( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			i3::pack::WORLD_SECTION_INFO * pInfo = list[i];

			pInfo->m_ShadowMask = (pInfo->m_ShadowMask & ~validMask) | mask;
		}
	}

	m_bEditted = TRUE;
}


void CDlgWorldSectionEdit::OnNMClickSectionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	_SelectSection();

	*pResult = 0;
}

void CDlgWorldSectionEdit::OnNMKillfocusVisibilityMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 cnt;
	CHK_STATE state;
	UINT64 mask = 0, validMask = 0;

	*pResult = 0;

	cnt = m_VisibleMaskListCtrl.GetItemCount();

	for(INT32 i = 0; i < cnt; i++)
	{
		state = GetVisibilityCheck( i);

		switch( state)
		{
			case CHECKED :
				mask |= ((UINT64)1L << i);
				validMask |= ((UINT64)1L << i);
				break;

			case UNCHECKED :
				validMask |= ((UINT64)1L << i);
				break;
		}
	}

	{
		i3::vector<i3::pack::WORLD_SECTION_INFO *> list;

		GetSelectedItems( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			i3::pack::WORLD_SECTION_INFO * pInfo = list[i];

			pInfo->m_VisibilityMask = (pInfo->m_VisibilityMask & ~validMask) | mask;
		}
	}

	m_bEditted = TRUE;
}

void CDlgWorldSectionEdit::OnNMDblclkSectionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idxFirstSel;
	CDlgWorldSectionInput dlg;

	{
		POSITION pos = m_SectionListCtrl.GetFirstSelectedItemPosition();

		if( pos == NULL)
			return;

		idxFirstSel = m_SectionListCtrl.GetNextSelectedItem( pos);
	}

	i3::pack::WORLD_SECTION_INFO * pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( idxFirstSel);

	if( dlg.Execute( idxFirstSel, pInfo))
	{
		strcpy( pInfo->m_szName, dlg.getSelectedName());

		_UpdateSection( idxFirstSel, pInfo);
		_OnUpdateWorldSection( idxFirstSel);

		m_bEditted = TRUE;
	}

	*pResult = 0;
}

void CDlgWorldSectionEdit::_OnUpdateAllWorldSection(void)
{
	INT32 i;
	INT32 wsCnt, gap, oldCnt;

	wsCnt = m_SectionListCtrl.GetItemCount();
	oldCnt = m_ShadowMaskListCtrl.GetItemCount();

	gap = wsCnt - oldCnt;

	if( gap < 0)
	{
		// 뺀다
		for( i = 0; i < gap; i++)
		{
			m_ShadowMaskListCtrl.DeleteItem( oldCnt - i - 1);
			m_VisibleMaskListCtrl.DeleteItem( oldCnt - i - 1);
		}
	}
	else if( m_ShadowMaskListCtrl.GetItemCount() < wsCnt)
	{
		// 더한다.
		for( i = 0; i < gap; i++)
		{
			m_ShadowMaskListCtrl.InsertItem( oldCnt + i, "", 0);
			m_VisibleMaskListCtrl.InsertItem( oldCnt + i, "", 0);

			_OnUpdateWorldSection( oldCnt + i);
		}
	}
}

void CDlgWorldSectionEdit::_OnUpdateWorldSection( INT32 idx)
{
	i3::pack::WORLD_SECTION_INFO * pInfo;

	pInfo = (i3::pack::WORLD_SECTION_INFO *) m_SectionListCtrl.GetItemData( idx);

	char conv[512];

	sprintf( conv, "%d", idx);
	m_ShadowMaskListCtrl.SetItemText( idx, 0, conv);
	m_ShadowMaskListCtrl.SetItemText( idx, 1, pInfo->m_szName);

	m_VisibleMaskListCtrl.SetItemText( idx, 0, conv);
	m_VisibleMaskListCtrl.SetItemText( idx, 1, pInfo->m_szName);
}

void CDlgWorldSectionEdit::SetShadowCheck( INT32 idx, CHK_STATE state)
{
	LVITEM item;

	item.mask = LVIF_IMAGE;
	item.iItem = idx;
	item.iSubItem = 0;
	item.iImage = state;

	m_ShadowMaskListCtrl.SetItem( &item);
}

void CDlgWorldSectionEdit::SetVisibilityCheck( INT32 idx, CHK_STATE state)
{
	LVITEM item;

	item.mask = LVIF_IMAGE;
	item.iItem = idx;
	item.iSubItem = 0;
	item.iImage = state;

	m_VisibleMaskListCtrl.SetItem( &item);
}

CDlgWorldSectionEdit::CHK_STATE	CDlgWorldSectionEdit::GetShadowCheck( INT32 idx)
{
	LVITEM item;

	item.mask = LVIF_IMAGE;
	item.iItem = idx;
	item.iSubItem = 0;

	m_ShadowMaskListCtrl.GetItem( &item);

	return (CHK_STATE) item.iImage;
}

CDlgWorldSectionEdit::CHK_STATE	CDlgWorldSectionEdit::GetVisibilityCheck( INT32 idx)
{
	LVITEM item;

	item.mask = LVIF_IMAGE;
	item.iItem = idx;
	item.iSubItem = 0;

	m_VisibleMaskListCtrl.GetItem( &item);

	return (CHK_STATE) item.iImage;
}

void CDlgWorldSectionEdit::OnNMClickShadowMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfo = (LPNMITEMACTIVATE)(pNMHDR);

	if( pInfo->iItem != -1)
	{
		CHK_STATE state = GetShadowCheck( pInfo->iItem);

		if( state == CHECKED)
			state = UNCHECKED;
		else
			state = CHECKED;

		SetShadowCheck( pInfo->iItem, state);
	}
	
	*pResult = 0;
}

void CDlgWorldSectionEdit::OnNMClickVisibilityMask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfo = (LPNMITEMACTIVATE)(pNMHDR);

	if( pInfo->iItem != -1)
	{
		CHK_STATE state = GetVisibilityCheck( pInfo->iItem);

		if( state == CHECKED)
			state = UNCHECKED;
		else
			state = CHECKED;

		SetVisibilityCheck( pInfo->iItem, state);
	}
	
	*pResult = 0;
}

void CDlgWorldSectionEdit::OnBnClickedBtnSelallShadow()
{
	INT32 i, cnt;

	cnt = m_ShadowMaskListCtrl.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		SetShadowCheck( i, CHECKED);
	}

}

void CDlgWorldSectionEdit::OnBnClickedBtnUnselallShadow()
{
	INT32 i, cnt;

	cnt = m_ShadowMaskListCtrl.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		SetShadowCheck( i, UNCHECKED);
	}
}

void CDlgWorldSectionEdit::OnBnClickedBtnSelallVisible()
{
	INT32 i, cnt;

	cnt = m_VisibleMaskListCtrl.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		SetVisibilityCheck( i, CHECKED);
	}
}

void CDlgWorldSectionEdit::OnBnClickedBtnUnselallVisible()
{
	INT32 i, cnt;

	cnt = m_VisibleMaskListCtrl.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		SetVisibilityCheck( i, UNCHECKED);
	}
}
