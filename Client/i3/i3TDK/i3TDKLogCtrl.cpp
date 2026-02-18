// i3TDKLogCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKLogCtrl.h"
#include "i3Base/string/ext/generic_string_cat.h"

static TCHAR _strTemp[ 1024] = _T("");

// i3TDKLogCtrl

IMPLEMENT_DYNAMIC(i3TDKLogCtrl, CEdit)

BOOL i3TDKLogCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	m_dwStyle = dwStyle;
	m_dwStyle |= WS_HSCROLL | WS_VSCROLL;
	m_dwStyle |= WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	
	return CEdit::Create( m_dwStyle, rect, pParentWnd, nID);
}

INT32 i3TDKLogCtrl::AddText( const TCHAR* pszString, ...)
{
	if( !m_hWnd)
	{
		I3TRACE("Warning! Not yet created i3TDKLogCtrl.\n");
		return -1;
	}

	INT32 nLine = GetLineCount();

	va_list arg;
	va_start( arg, pszString);
	_vstprintf( _strTemp, pszString, arg);
	va_end( arg);

	i3::generic_string_cat( _strTemp, _T("\r\n") );			//	문자열 마지막에 개행문자 추가

    if( i3::generic_string_size( _strTemp) > _countof(_strTemp) )
	{
		I3PRINTLOG(I3LOG_WARN,  "warning! Message is too long!!");
		return -1;
	}

	INT32 nBegin = LineIndex( nLine - 1);			//	마지막 라인의 시작 위치
	INT32 nEnd = nBegin + LineLength( nBegin);		//	마지막 라인의 마지막 위치

	SetSel( nBegin, nEnd);				//	마지막 위치로 캐럿 이동
	ReplaceSel( (LPCTSTR) _strTemp);	//	문자 출력

	LineScroll( nLine);					//	스크롤한다.

	//	가장 긴문자열을 저장해 둔다.
	const size_t lenMaxLongText = i3::generic_string_size(m_szMaxLongText);
	const size_t lenStrTemp = i3::generic_string_size(_strTemp);

	if( lenMaxLongText == 0)
		i3::safe_string_copy( m_szMaxLongText, _strTemp, _countof(m_szMaxLongText) );
	else if( lenMaxLongText < lenStrTemp )
		i3::safe_string_copy( m_szMaxLongText, _strTemp, _countof(m_szMaxLongText) );

	//	스크롤바의 Visible 여부를 체크
	checkScrollBar();

	return nLine;
}

const TCHAR * i3TDKLogCtrl::GetText( INT32 nLine) const
{
	if( !m_hWnd)
	{
		I3TRACE("Warning! Not yet created i3TDKLogCtrl.\n");
		return nullptr;
	}

	GetLine( nLine, _strTemp, _countof( _strTemp));

	return _strTemp;
}

void i3TDKLogCtrl::SetLogFont( const LOGFONT* pLogFont)
{
	if( !m_hWnd)
	{
		I3TRACE("Warning! Not yet created i3TDKLogCtrl.\n");
		return;
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect( pLogFont);
	SetFont( &m_Font);
}

void i3TDKLogCtrl::checkScrollBar( void)
{	
	SIZE size;
	RECT rt;
	LPCTSTR pszText = (LPCTSTR) m_szMaxLongText;

	//	현재 사용중인 폰트 적용 (중요!)
	HDC hdc = ::GetDC( m_hWnd);
	HFONT OldFont = (HFONT) SelectObject( hdc, m_Font.GetSafeHandle());
	
	//	실제 문자열의 pixel size를 알아온다. 
	//	실제 문자열의 pixel size를 알기위해선 현재 사용하고 있는 폰트를 적용해야 올바른 길이가 나온다.
	GetTextExtentPoint32( hdc, pszText, i3::generic_string_size( pszText), &size);
	
	SelectObject( hdc, OldFont);

	//	set visible scroll bar
	GetRect( &rt);
	if( rt.right < size.cx)
		this->ShowScrollBar( SB_HORZ, TRUE);
	else
		this->ShowScrollBar( SB_HORZ, FALSE);

	if( rt.bottom < size.cy * GetLineCount())
		this->ShowScrollBar( SB_VERT, TRUE);
	else
		this->ShowScrollBar( SB_VERT, FALSE);

}

BEGIN_MESSAGE_MAP(i3TDKLogCtrl, CEdit)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// i3TDKLogCtrl message handlers


int i3TDKLogCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	//	Default Font Setting
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight			= 12;
	logFont.lfWeight			= FW_DONTCARE;
	logFont.lfItalic			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	i3::generic_string_copy( logFont.lfFaceName, _T("굴림") );

	SetLogFont( &logFont);
	
	SetTabStops( 16);
	return 0;
}

void i3TDKLogCtrl::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	if( ::IsWindow( this->m_hWnd))
	{	
		checkScrollBar();		//	실시간으로 스크롤바의 visible 여부를 판단한다.
	}
}

void i3TDKLogCtrl::OnDestroy()
{
	CEdit::OnDestroy();

	//	destroy font
	m_Font.DeleteObject();
}

void i3TDKLogCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);
}
