// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgVRAMState.h"
#include "DlgTexInspector.h"
#include "i3Base/string/ext/integral_to_comma_str.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgTexInspector dialog

IMPLEMENT_DYNAMIC(CDlgTexInspector, CDialog)

CDlgTexInspector::CDlgTexInspector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTexInspector::IDD, pParent)
{

}

CDlgTexInspector::~CDlgTexInspector()
{
}

void CDlgTexInspector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEX, m_LIST_Tex);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalCount);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
}

UINT32	CDlgTexInspector::CalcBufferSize( i3Texture * pTex)
{
	INT32 i, w, h;
	UINT32 total, sz;

	total = 0;
	w = pTex->GetWidth();
	h = pTex->GetHeight();

	for( i = 0; (i < (INT32) pTex->GetLevelCount()) && (w > 0) && (h > 0); i++)
	{
		sz = i3Gfx::CalcRasterBufferSize( w, h, 4, pTex->GetFormat());

		if( i3::kind_of<i3TextureCube*>(pTex))
		{
			sz *= 6;
		}

		total += sz;

		w = w >> 1;
		h = h >> 1;
	}

	return total;
}

UINT32 CDlgTexInspector::_UpdateTex( INT32 idx, i3Texture * pTex)
{
	char conv[256];

	m_LIST_Tex.SetItemData( idx, (DWORD_PTR) pTex);

	// Width
	i3::snprintf( conv, sizeof(conv), "%d", pTex->GetWidth());
	m_LIST_Tex.SetItemText( idx, 1, conv);

	// Height
	i3::snprintf( conv, sizeof(conv), "%d", pTex->GetWidth());
	m_LIST_Tex.SetItemText( idx, 2, conv);

	// Format
	m_LIST_Tex.SetItemText( idx, 3, i3Gfx::GetImageFormatName( pTex->GetFormat()));

	// Level
	i3::snprintf( conv, sizeof(conv), "%d", pTex->GetLevelCount());
	m_LIST_Tex.SetItemText( idx, 4, conv);

	// Memory
	UINT32 sz = CalcBufferSize( pTex);
	i3::integral_to_comma_str( (INT32) sz, conv, sizeof(conv));
	m_LIST_Tex.SetItemText( idx, 5, conv);

	return sz;
}

void CDlgTexInspector::_Search( const char * pszStr)
{
	INT32 idx, count = 0;
	UINT32 sz = 0;
	bool bAdd;

	m_LIST_Tex.DeleteAllItems();

	i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();

	while( pRes != NULL)
	{
		if( i3::kind_of<i3Texture*>(pRes))
		{
			i3Texture * pTex = (i3Texture *) pRes;

			bAdd = true;

			if( (pTex->GetUsageFlag() & I3G_USAGE_RENDERTARGET) == 0)
			{
				char szName[512];

				if( pTex->getResourceLoadPath()[0] != 0)
				{
					i3::string_ncopy_nullpad( szName, pTex->getResourceLoadPath(), sizeof(szName));
				}
				else
				{
					if( pTex->GetName()[0] != 0)
						i3::string_ncopy_nullpad( szName, pTex->GetName(), sizeof(szName));
					else
						i3::snprintf( szName, sizeof(szName), "%s(%d)", pTex->_getSourceFileName(), pTex->_getSourceLineCount());
				}

				if( pszStr != NULL)
				{
					if( i3::contain_string( szName, pszStr) == -1)
						bAdd = false;
				}

				if( bAdd)
				{
					idx = m_LIST_Tex.InsertItem( count, szName, -1);

					sz += _UpdateTex( idx, pTex);

					count++;
				}
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

BEGIN_MESSAGE_MAP(CDlgTexInspector, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgTexInspector::OnHdnItemclickListTex)
END_MESSAGE_MAP()


// CDlgTexInspector message handlers

BOOL CDlgTexInspector::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LIST_Tex.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST_Tex.InsertColumn( 0, "Path", LVCFMT_LEFT, 260);
		m_LIST_Tex.InsertColumn( 1, "Width", LVCFMT_LEFT, 50);
		m_LIST_Tex.InsertColumn( 2, "Height", LVCFMT_LEFT, 50);
		m_LIST_Tex.InsertColumn( 3, "Format", LVCFMT_LEFT, 90);
		m_LIST_Tex.InsertColumn( 4, "Level", LVCFMT_LEFT, 40);
		m_LIST_Tex.InsertColumn( 5, "Memory", LVCFMT_LEFT, 90);
		
	}

	_ReplaceControls( -1, -1);

	_Search( NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTexInspector::_ReplaceControls( INT32 cx, INT32 cy)
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

	m_LIST_Tex.SetWindowPos( NULL, 3, rt.bottom + 4, cx - 6, cy - (rt.bottom + 4) - 3, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDlgTexInspector::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST_Tex.m_hWnd))
	{
		_ReplaceControls( cx, cy);
	}
}

void CDlgTexInspector::OnOK()
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

int CALLBACK CDlgTexInspector::_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	char szName1[ MAX_PATH];
	char szName2[ MAX_PATH];
	CDlgTexInspector * pThis = (CDlgTexInspector *) data;
	CListCtrl * pCtrl = &pThis->m_LIST_Tex;

	pCtrl->GetItemText( lParam1, 0, szName1, sizeof(szName1));
	pCtrl->GetItemText( lParam2, 0, szName2, sizeof(szName2));

	return i3::generic_compare( szName1, szName2);
}

int CALLBACK CDlgTexInspector::_SortByWidth( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3Texture * pTex1 = (i3Texture *) lParam1;
	i3Texture * pTex2 = (i3Texture *) lParam2;

	INT32 w1, w2;

	w1 = (INT32) pTex1->GetWidth();
	w2 = (INT32) pTex2->GetWidth();

	return (w1 - w2) * -1;
}

int CALLBACK CDlgTexInspector::_SortByHeight( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3Texture * pTex1 = (i3Texture *) lParam1;
	i3Texture * pTex2 = (i3Texture *) lParam2;

	INT32 h1, h2;

	h1 = (INT32) pTex1->GetWidth();
	h2 = (INT32) pTex2->GetWidth();

	return (h1 - h2) * -1;
}

int CALLBACK CDlgTexInspector::_SortByFormat( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3Texture * pTex1 = (i3Texture *) lParam1;
	i3Texture * pTex2 = (i3Texture *) lParam2;

	if( pTex1->GetFormat() > pTex2->GetFormat())
		return 1;

	if( pTex1->GetFormat() < pTex2->GetFormat())
		return -1;

	return 0;
}

int CALLBACK CDlgTexInspector::_SortByLevel( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3Texture * pTex1 = (i3Texture *) lParam1;
	i3Texture * pTex2 = (i3Texture *) lParam2;

	return (pTex1->GetLevelCount() - pTex2->GetLevelCount()) * -1;
}

int CALLBACK CDlgTexInspector::_SortByMemory( LPARAM lParam1, LPARAM lParam2, LPARAM data)
{
	i3Texture * pTex1 = (i3Texture *) lParam1;
	i3Texture * pTex2 = (i3Texture *) lParam2;
	CDlgTexInspector * pThis = (CDlgTexInspector *) data;

	INT32 sz1, sz2;

	sz1 = pThis->CalcBufferSize( pTex1);
	sz2 = pThis->CalcBufferSize( pTex2);

	return sz2 - sz1;
}

void CDlgTexInspector::OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	switch( phdr->iItem)
	{
		case 0 :			// Path
			m_LIST_Tex.SortItemsEx( (PFNLVCOMPARE) _SortByName, (LPARAM) this);
			break;

		case 1 :			// Width
			m_LIST_Tex.SortItems( (PFNLVCOMPARE) _SortByWidth, (LPARAM) this);
			break;

		case 2 :			// Height
			m_LIST_Tex.SortItems( (PFNLVCOMPARE) _SortByHeight, (LPARAM) this);
			break;

		case 3 :			// Format
			m_LIST_Tex.SortItems( (PFNLVCOMPARE) _SortByFormat, (LPARAM) this);
			break;

		case 4 :			// Level
			m_LIST_Tex.SortItems( (PFNLVCOMPARE) _SortByLevel, (LPARAM) this);
			break;

		case 5 :			// Memory
			m_LIST_Tex.SortItems( (PFNLVCOMPARE) _SortByMemory, (LPARAM) this);
			break;
	}

	*pResult = 0;
}

#endif