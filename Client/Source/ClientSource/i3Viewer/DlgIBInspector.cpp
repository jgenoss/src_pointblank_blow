// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgVRAMState.h"
#include "DlgIBInspector.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgIBInspector dialog

IMPLEMENT_DYNAMIC(CDlgIBInspector, CDialog)

CDlgIBInspector::CDlgIBInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIBInspector::IDD, pParent)
{

}

CDlgIBInspector::~CDlgIBInspector()
{
}

void CDlgIBInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IB, m_LIST);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

UINT32	CDlgIBInspector::CalcBufferSize( i3IndexArray * pIA)
{
	IDirect3DIndexBuffer9 * pDXVB = (IDirect3DIndexBuffer9 *) pIA->GetDxBuffer();

	D3DINDEXBUFFER_DESC desc;

	pDXVB->GetDesc( &desc);

	return desc.Size;
}

UINT32 CDlgIBInspector::_UpdateItem( INT32 idx, i3IndexArray * pIA)
{
	char conv[256];

	m_LIST.SetItemData( idx, (DWORD_PTR) pIA);

	// Memory
	UINT32 sz = CalcBufferSize( pIA);
	i3::integral_to_comma_str( (INT32) sz, conv, sizeof(conv));
	m_LIST.SetItemText( idx, 1, conv);

	return sz;
}

void CDlgIBInspector::_Search( const char * pszStr)
{
	INT32 idx, count = 0;
	UINT32 sz = 0;
	bool bAdd = true;
	char szName[512];

	m_LIST.DeleteAllItems();

	i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != NULL)
	{
		if( i3::kind_of<i3IndexArray*>(pRes))
		{
			i3IndexArray * pIA = (i3IndexArray *) pRes;

			bAdd = true;

			if( pIA->getResourceLoadPath()[0] != 0)
			{
				i3::string_ncopy_nullpad( szName, pIA->getResourceLoadPath(), sizeof(szName));
			}
			else
			{
				if( pIA->GetName()[0] != 0)
					i3::string_ncopy_nullpad( szName, pIA->GetName(), sizeof(szName));
				else
				{
					i3::snprintf( szName, sizeof(szName), "%s(%d)", pIA->_getSourceFileName(), pIA->_getSourceLineCount());
				}
			}

			if( pszStr != NULL)
			{
				if( i3::contain_string( szName, pszStr) == -1)
					bAdd = false;
			}

			if( bAdd)
			{
				idx = m_LIST.InsertItem( count, szName, -1);

				sz += _UpdateItem( idx, pIA);

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

BEGIN_MESSAGE_MAP(CDlgIBInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgIBInspector::OnHdnItemclickListTex)
END_MESSAGE_MAP()


// CDlgIBInspector message handlers

BOOL CDlgIBInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST.InsertColumn( 0, "Path", LVCFMT_LEFT, 260);
		m_LIST.InsertColumn( 1, "Memory", LVCFMT_LEFT, 90);
		
	}

	_ReplaceControls( -1, -1);

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgIBInspector::_ReplaceControls( INT32 cx, INT32 cy)
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

void CDlgIBInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgIBInspector::OnOK()
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

int CALLBACK CDlgIBInspector::_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgIBInspector * pThis = (CDlgIBInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgIBInspector::_SortByMemory( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3IndexArray * pTex1 = (i3IndexArray *) lParam1;
	i3IndexArray * pTex2 = (i3IndexArray *) lParam2;
	CDlgIBInspector * pThis = (CDlgIBInspector *) data;

	INT32 sz1, sz2;

	sz1 = pThis->CalcBufferSize( pTex1);
	sz2 = pThis->CalcBufferSize( pTex2);

	return sz2 - sz1;
}

void CDlgIBInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Path
			m_LIST.SortItemsEx( (PFNLVCOMPARE) _SortByName, (LPARAM) this);
			break;

		case 1 :			// Memory
			m_LIST.SortItems( (PFNLVCOMPARE) _SortByMemory, (LPARAM) this);
			break;
	}

	*pResult = 0;
}

#endif
