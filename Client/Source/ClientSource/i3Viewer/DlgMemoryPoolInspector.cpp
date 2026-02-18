// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgMemoryPoolInspector.h"
#include "DlgPoolSpec.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgMemoryPoolInspector dialog

IMPLEMENT_DYNAMIC(CDlgMemoryPoolInspector, CDialog)

CDlgMemoryPoolInspector::CDlgMemoryPoolInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMemoryPoolInspector::IDD, pParent)
{
	m_pszSearch		= NULL;
}

CDlgMemoryPoolInspector::~CDlgMemoryPoolInspector()
{
}

void CDlgMemoryPoolInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_LIST);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

UINT32 CDlgMemoryPoolInspector::_UpdateItem( INT32 idx, i3FixedMemoryPool * pPool)
{
	char conv[512];
	UINT32 sz, pageCount;

	m_LIST.SetItemData( idx, (DWORD_PTR) pPool);

	// Page Count
	pageCount = pPool->GetPageCount();
	i3::snprintf( conv, sizeof(conv), "%d", pageCount);
	m_LIST.SetItemText( idx, 1, conv);

	// Unit
	i3::snprintf( conv, sizeof(conv), "%d", pPool->getUnitSize());
	m_LIST.SetItemText( idx, 2, conv);

	// Size
	sz = pageCount * pPool->getPageSize();
	i3::integral_to_comma_str( (INT32) sz, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 3, conv);

	return sz;
}

bool CDlgMemoryPoolInspector::_CheckSearch( const char * pszStr, i3FixedMemoryPool * pPool)
{
	if( i3::contain_string( pPool->getName(), pszStr) >= 0)
		return true;

	return false;
}


bool CDlgMemoryPoolInspector::EnumProc( i3FixedMemoryPool * pPool, void * pUserData)
{
	CDlgMemoryPoolInspector * pDlg = (CDlgMemoryPoolInspector *) pUserData;
	CListCtrl * pListCtrl = & pDlg->m_LIST;
	bool bAdd = true;
	UINT32 sz;

	if( pDlg->m_pszSearch != NULL)
	{
		bAdd = pDlg->_CheckSearch( pDlg->m_pszSearch, pPool);
	}

	if( bAdd)
	{
		INT32 idx = pListCtrl->InsertItem( pDlg->m_TotalCount, pPool->getName(), 0);

		sz = pDlg->_UpdateItem( idx, pPool);

		pDlg->m_TotalCount++;
		pDlg->m_TotalSize += sz;
	}

	return true;
}

void CDlgMemoryPoolInspector::_Search( const char * pszStr)
{
	m_LIST.DeleteAllItems();

	m_TotalCount = 0;
	m_TotalSize = 0;
	m_pszSearch = pszStr;

	i3FixedMemoryPool::Enum( EnumProc, this);

	{
		char conv[256];

		i3::integral_to_comma_str( (INT32) m_TotalCount, conv, sizeof(conv));
		m_ED_TotalCount.SetWindowText( conv);

		i3::integral_to_comma_str( (INT32) m_TotalSize, conv, sizeof(conv));
		m_ED_TotalSize.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CDlgMemoryPoolInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgMemoryPoolInspector::OnHdnItemclickListTex)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgMemoryPoolInspector::OnBnClickedBtnSave)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDlgMemoryPoolInspector::OnNMDblclkList)
END_MESSAGE_MAP()


// CDlgMemoryPoolInspector message handlers

BOOL CDlgMemoryPoolInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST.InsertColumn( 0, "Name", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 1, "Page Count", LVCFMT_LEFT, 80);
		m_LIST.InsertColumn( 2, "Unit", LVCFMT_LEFT, 80);
		m_LIST.InsertColumn( 3, "Size", LVCFMT_LEFT, 80);
		
	}

	_ReplaceControls( -1, -1);

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMemoryPoolInspector::_ReplaceControls( INT32 cx, INT32 cy)
{
	CRect rt;

	if( cx == -1)
	{
		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_ED_Search.GetWindowRect( &rt);

	ScreenToClient( &rt);

	m_LIST.SetWindowPos( NULL, 3, rt.bottom + 4, cx - 6, cy - (rt.bottom + 4) - 3, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDlgMemoryPoolInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgMemoryPoolInspector::OnOK()
{
	if( GetFocus() == &m_ED_Search)
	{
		char conv[512];

		m_ED_Search.GetWindowText( conv, sizeof(conv));

		if( conv[0] == 0)
			_Search( NULL);
		else
			_Search( conv);
	}
}

int CALLBACK CDlgMemoryPoolInspector::_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgMemoryPoolInspector * pThis = (CDlgMemoryPoolInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgMemoryPoolInspector::_SortByPageCount( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3FixedMemoryPool * pInfo1 = (i3FixedMemoryPool *) lParam1;
	i3FixedMemoryPool * pInfo2 = (i3FixedMemoryPool *) lParam2;

	INT32 cnt1, cnt2;

	cnt1 = pInfo1->GetPageCount();
	cnt2 = pInfo2->GetPageCount();

	return cnt2 - cnt1;
}

int CALLBACK CDlgMemoryPoolInspector::_SortByUnit( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3FixedMemoryPool * pInfo1 = (i3FixedMemoryPool *) lParam1;
	i3FixedMemoryPool * pInfo2 = (i3FixedMemoryPool *) lParam2;

	if( pInfo1->getUnitSize() < pInfo2->getUnitSize())
		return 1;
	else if( pInfo1->getUnitSize() > pInfo2->getUnitSize())
		return -1;

	return 0;
}

int CALLBACK CDlgMemoryPoolInspector::_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3FixedMemoryPool * pPool1 = (i3FixedMemoryPool *) lParam1;
	i3FixedMemoryPool * pPool2 = (i3FixedMemoryPool *) lParam2;

	UINT32 sz1, sz2;

	sz1 = pPool1->GetPageCount() * pPool1->getPageSize();
	sz2 = pPool2->GetPageCount() * pPool2->getPageSize();

	if( sz1 < sz2)
		return 1;
	else if( sz1 > sz2)
		return -1;

	return 0;
}

void CDlgMemoryPoolInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Name
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByName, (LPARAM) this);
			break;

		case 1 :			// Page Count
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByPageCount, (LPARAM) this);
			break;

		case 2 :			// Unit
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByUnit, (LPARAM) this);
			break;

		case 3 :			// Size
			m_LIST.SortItems( (PFNLVCOMPARE) _SortBySize, (LPARAM) this);
			break;
	}

	*pResult = 0;
}

void CDlgMemoryPoolInspector::SaveToFile( const char * pszPath)
{
	i3FileStream file;

	if( file.Create( pszPath, STREAM_WRITE) == FALSE)
		return;

	INT32 i, cnt, sz;
	i3FixedMemoryPool * pPool;
	char conv[512];

	cnt = m_LIST.GetItemCount();

	file.Printf( conv, "Memory Pool Usage State\n");
	file.Printf( conv, "Name\tPage Count\tUnit\tSize\n");

	for( i = 0; i < cnt; i++)
	{
		pPool = (i3FixedMemoryPool *) m_LIST.GetItemData( i);

		sz = pPool->GetPageCount() * pPool->getPageSize();

		file.Printf( conv, "%s\t%d\t%d\t%d\n", pPool->getName(), pPool->GetPageCount(), pPool->getUnitSize(), sz);
	}

	file.Close();
}

void CDlgMemoryPoolInspector::OnBnClickedBtnSave()
{
	CFileDialog	Dlg( FALSE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"Report Text File(*.txt)|*.txt||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	SaveToFile( LPCTSTR( Dlg.GetPathName()));
}

void CDlgMemoryPoolInspector::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE * pInfo = (NMITEMACTIVATE *) pNMHDR;
	i3FixedMemoryPool * pPool;
	CDlgPoolSpec dlg;

	pPool = (i3FixedMemoryPool *) m_LIST.GetItemData( pInfo->iItem);

	dlg.Execute( pPool);

	*pResult = 0;
}


#endif
