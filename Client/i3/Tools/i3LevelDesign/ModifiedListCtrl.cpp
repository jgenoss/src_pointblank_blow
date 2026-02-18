// ModifiedListCtrl.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "ModifiedListCtrl.h"

// CModifiedListCtrl

IMPLEMENT_DYNAMIC(CModifiedListCtrl, CListCtrl)
CModifiedListCtrl::CModifiedListCtrl()
{
}

CModifiedListCtrl::~CModifiedListCtrl()
{
}


BEGIN_MESSAGE_MAP(CModifiedListCtrl, CListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CModifiedListCtrl message handlers
BOOL CModifiedListCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT ID)
{
	//	항목의 데이터를 직접 관리하기 위해 LVS_OWNERDATA를 사용.
	dwStyle |= WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE | LVS_SHOWSELALWAYS | LVS_REPORT;  

	return CListCtrl::Create( dwStyle, rect, pParentWnd, ID);
}

void CModifiedListCtrl::SetLogFont( const LOGFONT * pLogFont)
{
	if( !m_hWnd)
	{
		I3TRACE("Warning! Not yet created CModifiedListCtrl.\n");
		return;
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect( pLogFont);
	SetFont( &m_Font);
}

int CModifiedListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight			= 14;
	logFont.lfWeight			= FW_DONTCARE;
	logFont.lfItalic			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy( logFont.lfFaceName, "Tahoma");

	SetLogFont( &logFont);

	//	Extend style set
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT;

	SetExtendedStyle( dwExStyle  );

	return 0;
}
