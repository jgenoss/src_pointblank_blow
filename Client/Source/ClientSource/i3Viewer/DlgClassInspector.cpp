// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgClassInspector.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgClassInspector dialog

IMPLEMENT_DYNAMIC(CDlgClassInspector, CDialog)
/*
static INT32	ClassCompProc( CLASSSUMINFO * p1, CLASSSUMINFO * p2)
{
	if( p1->m_CRC > p2->m_CRC)
		return 1;
	else if( p1->m_CRC < p2->m_CRC)
		return -1;

	return 0;
}

static INT32	ClassFindProc( CLASSSUMINFO * p1, UINT32 * pCRC)
{
	if( p1->m_CRC > *pCRC)
		return 1;
	else if( p1->m_CRC < *pCRC)
		return -1;

	return 0;
}
*/

CDlgClassInspector::CDlgClassInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgClassInspector::IDD, pParent)
{
	
}

CDlgClassInspector::~CDlgClassInspector()
{
	RemoveAllClassSum();
}

void CDlgClassInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_LIST);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

void CDlgClassInspector::RemoveAllClassSum(void)
{
	i3::cu::for_each_delete_clear(m_ClassList);
}

bool CDlgClassInspector::EnumProc( i3ElementBase * pObj, void * pUserData)
{
	i3::vector_set<CLASSSUMINFO*>& List = *static_cast<i3::vector_set<CLASSSUMINFO*>* >(pUserData);
	CLASSSUMINFO * pInfo;
	INT32 line;
	i3ClassMeta * pMeta = pObj->meta();
	UINT32 crc = 0xFFFFFFFF;

	if( pObj->_getSourceFileName() != NULL)
	{
		crc = CRC32( crc, (UINT8 *) pObj->_getSourceFileName(), strlen( pObj->_getSourceFileName()));

		line = pObj->_getSourceLineCount();
		crc = CRC32( crc, (UINT8 *) &line, sizeof( line));
	}

	crc = CRC32( crc, (UINT8 *) &pMeta, sizeof( pMeta));

	i3::vector_set<CLASSSUMINFO*>::iterator it = List.find_other(crc);

	if( it == List.end() )
	{
		//Ãß°¡.
		pInfo = new CLASSSUMINFO;
		pInfo->m_pMeta = pMeta;
		pInfo->m_CRC = crc;
		pInfo->m_Count = 0;
		pInfo->m_Size = 0;

		if( pObj->_getSourceFileName() != NULL)
		{
			pInfo->m_pszSource = pObj->_getSourceFileName();
			pInfo->m_Line = pObj->_getSourceLineCount();
		}
		else
		{
			pInfo->m_pszSource = "<Local object>";
			pInfo->m_Line = 0;
		}

		List.insert( pInfo);
	}
	else
	{
		pInfo = *it; 
	}

	pInfo->m_Count++;
	pInfo->m_Size += pMeta->class_size();

	return true;
}

void CDlgClassInspector::_Scan(void)
{
	RemoveAllClassSum();

	i3ElementBase::EnumObjects( EnumProc, &m_ClassList);
}


UINT32 CDlgClassInspector::_UpdateItem( INT32 idx, CLASSSUMINFO * pInfo)
{
	char conv[512];

	m_LIST.SetItemData( idx, (DWORD_PTR) pInfo);

	// Source
	i3::snprintf( conv, sizeof(conv), "%s (%d)", pInfo->m_pszSource, pInfo->m_Line);
	m_LIST.SetItemText( idx, 1, conv);

	// Count
	i3::integral_to_comma_str( pInfo->m_Count, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 2, conv);

	// Size
	i3::integral_to_comma_str( pInfo->m_Size, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 3, conv);

	return pInfo->m_Size;
}

bool CDlgClassInspector::_CheckSearch( const char * pszStr, CLASSSUMINFO * pInfo)
{
	if( i3::contain_string( pInfo->m_pszSource, pszStr) >= 0)
		return true;

	if( i3::contain_string( pInfo->m_pMeta->class_name().c_str(), pszStr) >= 0)
		return true;

	return false;
}

void CDlgClassInspector::_Search( const char * pszStr)
{
	UINT32 count, size;
	
	bool bAdd;

	m_LIST.DeleteAllItems();

	count = 0;
	size = 0;

	for(size_t i = 0; i < m_ClassList.size(); i++)
	{
		CLASSSUMINFO * pInfo = m_ClassList.get_vector()[i];

		bAdd = true;

		if( pszStr != NULL)
		{
			bAdd = _CheckSearch( pszStr, pInfo);
		}

		if( bAdd)
		{	
			INT32 idx = m_LIST.InsertItem( count, pInfo->m_pMeta->class_name().c_str(), 0);

			_UpdateItem( idx, pInfo);

			count++;
			size += pInfo->m_Size;
		}
	}
	

	{
		char conv[256];

		i3::integral_to_comma_str( (INT32) count, conv, sizeof(conv));
		m_ED_TotalCount.SetWindowText( conv);

		i3::integral_to_comma_str( (INT32) size, conv, sizeof(conv));
		m_ED_TotalSize.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CDlgClassInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgClassInspector::OnHdnItemclickListTex)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgClassInspector::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgClassInspector message handlers

BOOL CDlgClassInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST.InsertColumn( 0, "Class", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 1, "Created location", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 2, "Count", LVCFMT_LEFT, 90);
		m_LIST.InsertColumn( 3, "Size", LVCFMT_LEFT, 90);
		
	}

	_ReplaceControls( -1, -1);

	_Scan();

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgClassInspector::_ReplaceControls( INT32 cx, INT32 cy)
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

void CDlgClassInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgClassInspector::OnOK()
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

int CALLBACK CDlgClassInspector::_SortByClass( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgClassInspector * pThis = (CDlgClassInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgClassInspector::_SortByLocation( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgClassInspector * pThis = (CDlgClassInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 1, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 1, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgClassInspector::_SortByCount( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	CLASSSUMINFO * pInfo1 = (CLASSSUMINFO *) lParam1;
	CLASSSUMINFO * pInfo2 = (CLASSSUMINFO *) lParam2;

	if( pInfo1->m_Count < pInfo2->m_Count)
		return 1;
	else if( pInfo1->m_Count > pInfo2->m_Count)
		return -1;

	return 0;
}

int CALLBACK CDlgClassInspector::_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	CLASSSUMINFO * pInfo1 = (CLASSSUMINFO *) lParam1;
	CLASSSUMINFO * pInfo2 = (CLASSSUMINFO *) lParam2;

	if( pInfo1->m_Size < pInfo2->m_Size)
		return 1;
	else if( pInfo1->m_Size > pInfo2->m_Size)
		return -1;

	return 0;
}

void CDlgClassInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Class
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByClass, (LPARAM) this);
			break;

		case 1 :			// Location
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByLocation, (LPARAM) this);
			break;

		case 2 :			// Count
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByCount, (LPARAM) this);
			break;

		case 3 :			// Size
			m_LIST.SortItems( (PFNLVCOMPARE) _SortBySize, (LPARAM) this);
			break;
	}

	*pResult = 0;
}

void CDlgClassInspector::SaveToFile( const char * pszPath)
{
	i3FileStream file;

	if( file.Create( pszPath, STREAM_WRITE) == FALSE)
		return;

	INT32 i, cnt;
	CLASSSUMINFO * pInfo;
	char conv[512];

	cnt = m_LIST.GetItemCount();

	file.Printf( conv, "Class Usage State\n");
	file.Printf( conv, "Class\tLocation\tCount\tSize\n");

	for( i = 0; i < cnt; i++)
	{
		pInfo = (CLASSSUMINFO *) m_LIST.GetItemData( i);

		file.Printf( conv, "%s\t%s (%d)\t%d\t%d\n", pInfo->m_pMeta->class_name().c_str(), pInfo->m_pszSource, pInfo->m_Line, pInfo->m_Count, pInfo->m_Size);
	}

	file.Close();
}

void CDlgClassInspector::OnBnClickedBtnSave()
{
	CFileDialog	Dlg( FALSE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"Report Text File(*.txt)|*.txt||", this);


	if( Dlg.DoModal() != IDOK)
		return;

	SaveToFile( LPCTSTR( Dlg.GetPathName()));
}
#endif

