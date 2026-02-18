// DlgTextureSelect.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgTextureSelect.h"
#include ".\dlgtextureselect.h"


// CDlgTextureSelect dialog

IMPLEMENT_DYNAMIC(CDlgTextureSelect, CDialog)
CDlgTextureSelect::CDlgTextureSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTextureSelect::IDD, pParent)
{
	m_pSelTex = NULL;
	m_hDC = NULL;
}

CDlgTextureSelect::~CDlgTextureSelect()
{
}

void CDlgTextureSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEX_LIST, m_TexListCtrl);
}

bool CDlgTextureSelect::Execute( i3LevelResTexture * pTex)
{
	m_pSelTex = pTex;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(CDlgTextureSelect, CDialog)
	ON_WM_DRAWITEM()
	ON_WM_DESTROY()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


// CDlgTextureSelect message handlers
BOOL CDlgTextureSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ImageList.Create( 64, 64, ILC_COLOR32, 1, 1);

		m_TexListCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL);

	}

	{
		HDC dc = ::GetDC( m_hWnd);

		m_hDC  = CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);
	}

	{
		m_TexListCtrl.SetBkColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_BACK_APP_BKGND));
		m_TexListCtrl.SetTextBkColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_BACK_APP_BKGND));
		m_TexListCtrl.SetTextColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL));

		m_TexListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 180);
		m_TexListCtrl.InsertColumn( 1, "Dimension", LVCFMT_LEFT, 80);
		m_TexListCtrl.InsertColumn( 2, "Format", LVCFMT_LEFT, 60);
	}

	{
		INT32 i, idx, selIdx = -1;
		i3LevelRes * pRes;

		for( i = 0; i < g_pResDB->getResCount(); i++)
		{
			pRes = g_pResDB->getRes( i);

			if( i3::same_of<i3LevelResTexture*>(pRes)) //->IsExactTypeOf( i3LevelResTexture::static_meta()))
			{
				idx = m_TexListCtrl.InsertItem( i, pRes->GetName(), 0);

				m_TexListCtrl.SetItemData( idx, (DWORD_PTR) pRes);

				if( pRes == m_pSelTex)
				{
					selIdx = idx;
				}
			}
		}

		if( selIdx != -1)
		{
			m_TexListCtrl.SetItemState( selIdx, 0xFFFFFFFF, LVIS_SELECTED);
			m_TexListCtrl.EnsureVisible( selIdx, FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgTextureSelect::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pInfo)
{
	i3LevelResTexture * pTex;
	HBITMAP hOldBitmap;
	RECT * pRt = & pInfo->rcItem;
	CRect rect, rtItem;
	CHeaderCtrl * pHeadCtrl = m_TexListCtrl.GetHeaderCtrl();
	char conv[256];

	pTex = (i3LevelResTexture *) pInfo->itemData;

	hOldBitmap = (HBITMAP) ::SelectObject( m_hDC, pTex->getBitmap());

	rect = *pRt;

	if( pInfo->itemState & ODS_SELECTED)
	{
		::SetTextColor( pInfo->hDC, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_HILIGHT));

		// Erase Background
		::FillRect( pInfo->hDC, pRt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_APP_BKGND));
	} 
	else if( pInfo->itemState & ODS_FOCUS)
	{
		::SetTextColor( pInfo->hDC, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_HILIGHT));

		// Erase Background
		::FillRect( pInfo->hDC, pRt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND));
	}
	else
	{
		::SetTextColor( pInfo->hDC, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL));

		// Erase Background
		::FillRect( pInfo->hDC, pRt, g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_APP_BKGND));
	}

	// Icon
	::BitBlt( pInfo->hDC, pRt->left + 1, pRt->top + 1, 64, 64, m_hDC, 0, 0, SRCCOPY);

	// Name
	{
		pHeadCtrl->GetItemRect( 0, &rtItem);
		rect.left = pRt->left + 64 + 4;
		rect.right = rtItem.right;

		::DrawText( pInfo->hDC, pTex->GetName(), strlen( pTex->GetName()), &rect, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	// Dimension
	{
		pHeadCtrl->GetItemRect( 1, &rtItem);
		rect.left = rtItem.left;
		rect.right = rtItem.right;

		sprintf( conv, "%dx%d", pTex->getWidth(), pTex->getHeight());
		::DrawText( pInfo->hDC, conv, -1, &rect, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	// Format
	{
		pHeadCtrl->GetItemRect( 2, &rtItem);
		rect.left = rtItem.left;
		rect.right = rtItem.right;

		::DrawText( pInfo->hDC, i3Gfx::GetImageFormatName( pTex->getFormat()), -1, &rect, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	::SelectObject( m_hDC, hOldBitmap);
}


void CDlgTextureSelect::OnDestroy()
{
	if( m_hDC != NULL)
	{
		::DeleteDC( m_hDC);
		m_hDC = NULL;
	}

	CDialog::OnDestroy();
}

void CDlgTextureSelect::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT pInfo)
{
	CDialog::OnMeasureItem(nIDCtl, pInfo);

	if( pInfo->CtlType == ODT_LISTVIEW)
	{
		pInfo->itemHeight = 66;
	}
}

void CDlgTextureSelect::OnOK()
{
	POSITION pos = m_TexListCtrl.GetFirstSelectedItemPosition();

	m_pSelTex = NULL;

	if( pos != NULL)
	{
		INT32 idx = m_TexListCtrl.GetNextSelectedItem( pos);

		m_pSelTex = (i3LevelResTexture *) m_TexListCtrl.GetItemData( idx);
	}

	CDialog::OnOK();
}
