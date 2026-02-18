// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgHeapInspector.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgHeapInspector dialog

IMPLEMENT_DYNAMIC(CDlgHeapInspector, CDialog)
/*
static INT32	MemCompProc( MEMSUMINFO * p1, MEMSUMINFO * p2)
{
	if( p1->m_CRC > p2->m_CRC)
		return 1;
	else if( p1->m_CRC < p2->m_CRC)
		return -1;

	return 0;
}

static INT32	MemFindProc( MEMSUMINFO * p1, UINT32 * pCRC)
{
	if( p1->m_CRC > *pCRC)
		return 1;
	else if( p1->m_CRC < *pCRC)
		return -1;

	return 0;
}
*/

CDlgHeapInspector::CDlgHeapInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeapInspector::IDD, pParent)
{
	
}

CDlgHeapInspector::~CDlgHeapInspector()
{
	RemoveAllMemSum();
}

void CDlgHeapInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_LIST);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

void CDlgHeapInspector::RemoveAllMemSum(void)
{
	i3::cu::for_each_delete_clear(m_MemList);
}

bool CDlgHeapInspector::EnumBlockProc( I3MEMBLOCK * pBlock, void * pUserData)
{
	i3::vector_set<MEMSUMINFO*>& List = *static_cast<i3::vector_set<MEMSUMINFO*>*>(pUserData);
	MEMSUMINFO * pInfo;

	UINT32 crc = CRC32( 0xFFFFFFFF, (UINT8 *) pBlock->m_pAllocPos, strlen( pBlock->m_pAllocPos));
	crc = CRC32( crc, (UINT8 *) &pBlock->m_Line, sizeof( pBlock->m_Line));

	i3::vector_set<MEMSUMINFO*>::iterator it = List.find_other(crc);

	if( it == List.end() )
	{
		//Ãß°¡.
		pInfo = new MEMSUMINFO;
		pInfo->m_pszSource = pBlock->m_pAllocPos;
		pInfo->m_Line = pBlock->m_Line;
		pInfo->m_CRC = crc;
		pInfo->m_Count = 0;
		pInfo->m_Size = 0;

		List.insert( pInfo);
	}
	else
	{
		pInfo = *it;
	}

	pInfo->m_Count++;
	pInfo->m_Size += pBlock->m_Size;

	return true;
}

void CDlgHeapInspector::_ScanMemory(void)
{
	RemoveAllMemSum();

	i3mem::Enum( EnumBlockProc, &m_MemList);
}


UINT32 CDlgHeapInspector::_UpdateItem( INT32 idx, MEMSUMINFO * pInfo)
{
	char conv[512];

	m_LIST.SetItemData( idx, (DWORD_PTR) pInfo);

	// Source

	// Count
	i3::integral_to_comma_str( pInfo->m_Count, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 1, conv);

	// Size
	i3::integral_to_comma_str( pInfo->m_Size, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 2, conv);

	return pInfo->m_Size;
}

bool CDlgHeapInspector::_CheckSearch( const char * pszStr, MEMSUMINFO * pInfo)
{
	if( i3::contain_string( pInfo->m_pszSource, pszStr) >= 0)
		return true;

	return false;
}

void CDlgHeapInspector::_Search( const char * pszStr)
{
	UINT32 count, size;
	bool bAdd;
	char conv[512];

	m_LIST.DeleteAllItems();

	count = 0;
	size = 0;

	for(size_t i = 0; i < m_MemList.size(); i++)
	{
		MEMSUMINFO * pInfo = m_MemList.get_vector()[i];

		bAdd = true;

		if( pszStr != NULL)
		{
			bAdd = _CheckSearch( pszStr, pInfo);
		}

		if( bAdd)
		{
			i3::snprintf( conv, _countof(conv), "%s (%d)", pInfo->m_pszSource, pInfo->m_Line);
			
			INT32 idx = m_LIST.InsertItem( count, conv, 0);

			_UpdateItem( idx, pInfo);

			count++;
			size += pInfo->m_Size;
		}
	}
	

	{
//		char conv[256];

		i3::integral_to_comma_str( (INT32) count, conv, _countof(conv));
		m_ED_TotalCount.SetWindowText( conv);

		i3::integral_to_comma_str( (INT32) size, conv, _countof(conv));
		m_ED_TotalSize.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CDlgHeapInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgHeapInspector::OnHdnItemclickListTex)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgHeapInspector::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgHeapInspector message handlers

BOOL CDlgHeapInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST.InsertColumn( 0, "Allocatted location", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 1, "Count", LVCFMT_LEFT, 90);
		m_LIST.InsertColumn( 2, "Size", LVCFMT_LEFT, 90);
		
	}

	_ReplaceControls( -1, -1);

	_ScanMemory();

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHeapInspector::_ReplaceControls( INT32 cx, INT32 cy)
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

void CDlgHeapInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgHeapInspector::OnOK()
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

int CALLBACK CDlgHeapInspector::_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgHeapInspector * pThis = (CDlgHeapInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgHeapInspector::_SortByCount( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	MEMSUMINFO * pInfo1 = (MEMSUMINFO *) lParam1;
	MEMSUMINFO * pInfo2 = (MEMSUMINFO *) lParam2;

	if( pInfo1->m_Count < pInfo2->m_Count)
		return 1;
	else if( pInfo1->m_Count > pInfo2->m_Count)
		return -1;

	return 0;
}

int CALLBACK CDlgHeapInspector::_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	MEMSUMINFO * pInfo1 = (MEMSUMINFO *) lParam1;
	MEMSUMINFO * pInfo2 = (MEMSUMINFO *) lParam2;

	if( pInfo1->m_Size < pInfo2->m_Size)
		return 1;
	else if( pInfo1->m_Size > pInfo2->m_Size)
		return -1;

	return 0;
}

void CDlgHeapInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Name
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByName, (LPARAM) this);
			break;

		case 1 :			// Count
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByCount, (LPARAM) this);
			break;

		case 2 :			// Size
			m_LIST.SortItems( (PFNLVCOMPARE) _SortBySize, (LPARAM) this);
			break;
	}

	*pResult = 0;
}

void CDlgHeapInspector::SaveToFile( const char * pszPath)
{
	i3FileStream file;

	if( file.Create( pszPath, STREAM_WRITE) == FALSE)
		return;

	INT32 i, cnt;
	MEMSUMINFO * pInfo;
	char conv[512];

	cnt = m_LIST.GetItemCount();

	file.Printf( conv, "Memory Usage State\n");
	file.Printf( conv, "Name\tCount\tSize\n");

	for( i = 0; i < cnt; i++)
	{
		pInfo = (MEMSUMINFO *) m_LIST.GetItemData( i);

		file.Printf( conv, "%s (%d)\t%d\t%d\n", pInfo->m_pszSource, pInfo->m_Line, pInfo->m_Count, pInfo->m_Size);
	}

	file.Close();
}

void CDlgHeapInspector::OnBnClickedBtnSave()
{
	CFileDialog	Dlg( FALSE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"Report Text File(*.txt)|*.txt||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	SaveToFile( LPCTSTR( Dlg.GetPathName()));
}

#endif

