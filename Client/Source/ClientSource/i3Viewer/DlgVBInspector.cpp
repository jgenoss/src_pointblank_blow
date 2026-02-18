// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgVRAMState.h"
#include "DlgVBInspector.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

// CDlgVBInspector dialog

IMPLEMENT_DYNAMIC(CDlgVBInspector, CDialog)

CDlgVBInspector::CDlgVBInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVBInspector::IDD, pParent)
{

}

CDlgVBInspector::~CDlgVBInspector()
{
}

void CDlgVBInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VB, m_LIST);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

UINT32	CDlgVBInspector::CalcBufferSize( i3VertexArray * pVB)
{
	i3VertexArrayDX * pVA = (i3VertexArrayDX *) pVB;

	IDirect3DVertexBuffer9 * pDXVB = (IDirect3DVertexBuffer9 *) pVA->getDXInstance();

	D3DVERTEXBUFFER_DESC desc;

	pDXVB->GetDesc( &desc);

	return desc.Size;
}

void CDlgVBInspector::_FormatName( i3VertexArray * pVA, char * pszStr)
{
	char conv[256];
	i3VertexFormat * pFormat;

	pFormat = pVA->GetFormat();

	pszStr[0] = 0;

	if( pFormat->GetHasPosition())
		i3::generic_string_cat( pszStr, "P");

	if( pFormat->GetHasColor())
		i3::generic_string_cat( pszStr, "C");

	if( pFormat->GetHasNormal())
		i3::generic_string_cat( pszStr, "N");

	if( pFormat->GetHasTangent())
		i3::generic_string_cat( pszStr, "T");

	if( pFormat->GetHasBinormal())
		i3::generic_string_cat( pszStr, "B");

	if( pFormat->GetTextureCoordSetCount() > 0)
	{
		i3::snprintf( conv, sizeof(conv), "T%d", pFormat->GetTextureCoordSetCount());
		i3::generic_string_cat( pszStr, conv);
	}

	if( pFormat->GetBlendIndexCount() > 0)
	{
		i3::snprintf( conv, sizeof(conv), "I%d", pFormat->GetBlendIndexCount());
		i3::generic_string_cat( pszStr, conv);
	}

	if( pFormat->GetBlendWeightCount() > 0)
	{
		i3::snprintf( conv, sizeof(conv), "W%d", pFormat->GetBlendWeightCount());
		i3::generic_string_cat( pszStr, conv);
	}
}

UINT32 CDlgVBInspector::_UpdateItem( INT32 idx, i3VertexArray * pVB)
{
	char conv[512];

	m_LIST.SetItemData( idx, (DWORD_PTR) pVB);

	// Path
	i3::snprintf( conv, sizeof(conv), "%s (%d)", pVB->getResourceLoadPath(), pVB->getResourceLoadOffset());
	m_LIST.SetItemText( idx, 1, conv);

	// Source
	i3::snprintf( conv, sizeof(conv), "%s (%d)", pVB->_getSourceFileName(), pVB->_getSourceLineCount());
	m_LIST.SetItemText( idx, 2, conv);

	// Format
	_FormatName( pVB, conv);
	m_LIST.SetItemText( idx, 3, conv);

	// Memory
	UINT32 sz = CalcBufferSize( pVB);
	i3::integral_to_comma_str( (INT32) sz, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 4, conv);

	i3::snprintf( conv, sizeof(conv), "%p", pVB);
	m_LIST.SetItemText( idx, 5, conv);

	return sz;
}

bool CDlgVBInspector::_CheckSearch( const char * pszStr, i3VertexArray * pVA)
{
	char conv[512];

	if( pVA->GetName()[0] != 0)
		i3::snprintf( conv, sizeof(conv), "%s", pVA->GetName());
	else
		i3::string_ncopy_nullpad( conv, "<NONAMED>", sizeof(conv));

	if( i3::contain_string( conv, pszStr) >= 0)
		return true;

	if( i3::contain_string( pVA->getResourceLoadPath(), pszStr) >= 0)
		return true;

	if( i3::contain_string( pVA->_getSourceFileName(), pszStr) >= 0)
		return true;

	return false;
}

void CDlgVBInspector::_Search( const char * pszStr)
{
	INT32 idx, count = 0;
	UINT32 sz = 0;
	bool bAdd = true;
	char szName[512];

	m_LIST.DeleteAllItems();

	i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != NULL)
	{
		if( i3::kind_of<i3VertexArray*>(pRes))
		{
			i3VertexArray * pVB = (i3VertexArray *) pRes;

			bAdd = true;

			if( pVB->GetName()[0] != 0)
			{
				i3::snprintf( szName, sizeof(szName), "%s", pVB->GetName());
			}
			else
			{
				i3::string_ncopy_nullpad( szName, "<NONAMED>", sizeof(szName));
			}

			if( pszStr != NULL)
			{
				if( _CheckSearch( pszStr, pVB) == false)
					bAdd = false;
			}

			if( bAdd)
			{
				idx = m_LIST.InsertItem( count, szName, -1);

				sz += _UpdateItem( idx, pVB);

				count++;
			}
		}

		pRes = pRes->getNextRes();
	}

	{
		char conv[256];

		i3::snprintf( conv, sizeof(conv), "%d", count);
		m_ED_TotalCount.SetWindowText( conv);

		i3::integral_to_comma_str( (INT32) sz, conv, sizeof(conv));
		m_ED_TotalSize.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CDlgVBInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgVBInspector::OnHdnItemclickListTex)
END_MESSAGE_MAP()


// CDlgVBInspector message handlers

BOOL CDlgVBInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST.InsertColumn( 0, "Name", LVCFMT_LEFT, 80);
		m_LIST.InsertColumn( 1, "Path", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 2, "Src", LVCFMT_LEFT, 900);
		m_LIST.InsertColumn( 3, "Format", LVCFMT_LEFT, 90);
		m_LIST.InsertColumn( 4, "Memory", LVCFMT_LEFT, 90);
		m_LIST.InsertColumn( 5, "Instance", LVCFMT_LEFT, 90);
		
	}

	_ReplaceControls( -1, -1);

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVBInspector::_ReplaceControls( INT32 cx, INT32 cy)
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

void CDlgVBInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgVBInspector::OnOK()
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

int CALLBACK CDlgVBInspector::_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgVBInspector * pThis = (CDlgVBInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgVBInspector::_SortByPath( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgVBInspector * pThis = (CDlgVBInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 1, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 1, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgVBInspector::_SortBySrc( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgVBInspector * pThis = (CDlgVBInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 2, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 2, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}


int CALLBACK CDlgVBInspector::_SortByFormat( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgVBInspector * pThis = (CDlgVBInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 3, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 3, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgVBInspector::_SortByMemory( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3VertexArray * pTex1 = (i3VertexArray *) lParam1;
	i3VertexArray * pTex2 = (i3VertexArray *) lParam2;
	CDlgVBInspector * pThis = (CDlgVBInspector *) data;

	INT32 sz1, sz2;

	sz1 = pThis->CalcBufferSize( pTex1);
	sz2 = pThis->CalcBufferSize( pTex2);

	return sz2 - sz1;
}

void CDlgVBInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Name
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByName, (LPARAM) this);
			break;

		case 1 :			// Path
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByPath, (LPARAM) this);
			break;

		case 2 :			// Source
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortBySrc, (LPARAM) this);
			break;

		case 3 :			// Format
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByFormat, (LPARAM) this);
			break;

		case 4 :			// Memory
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByMemory, (LPARAM) this);
			break;
	}

	*pResult = 0;
}
#endif