// CUIPaneOutput
#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneOutput.h"

#include "..\CEdit_Output.h"

IMPLEMENT_DYNAMIC(CUIPaneOutput, CUIPaneBase)

COutputEdit * g_pEditCtrl = NULL;

CUIPaneOutput::CUIPaneOutput()
{
	i3mem::FillZero( m_Msg, sizeof( m_Msg));
}

CUIPaneOutput::~CUIPaneOutput()
{
	i3Lua::SetLogCallBack( NULL);
}

BEGIN_MESSAGE_MAP(CUIPaneOutput, CUIPaneBase)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static void _LuaLogCallBack(const char *pszLog)
{
	if( g_pEditCtrl != NULL)
	{
		INT32 string_length = g_pEditCtrl->GetWindowTextLength(); 
         
        // 입력된 문자열이 있는 경우, 문자열의 마지막 위치에 커서를 설정한다. 
        if(string_length > 0) 
			//g_pEditCtrl->SetSel(string_length, string_length); 
			g_pEditCtrl->SetSel(string_length, string_length); 
         
        // 채팅 내용을 목록 에디트 컨트롤에 추가한다. 
        g_pEditCtrl->ReplaceSel((LPCTSTR)pszLog);
		g_pEditCtrl->ReplaceSel((LPCTSTR)"\r\n");
	}
}

// CUIPaneOutput message handlers
int CUIPaneOutput::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rc;
		GetClientRect( &rc);

		//virtual BOOL Create(   DWORD dwStyle,   const RECT& rect,   CWnd* pParentWnd,   UINT nID );
		if( !m_Edit.Create( WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE, rc, this, 33445))
		{
			I3TRACE(" Failed to Create Edit Control\n");
		}

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
		strcpy( logFont.lfFaceName, "돋움");

		m_Font.CreateFontIndirect( &logFont);
		
		m_Edit.SetFont( &m_Font);

		g_pEditCtrl = &m_Edit;

		i3Lua::SetLogCallBack( _LuaLogCallBack);

		//m_Edit.SetLimitText( 20);
	}
	return 0;
}

void CUIPaneOutput::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);

	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);
}

void CUIPaneOutput::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if( ::IsWindow( m_Edit.m_hWnd))
	{
		CRect rc;
		GetClientRect(rc);
		
		m_Edit.SetWindowPos( NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CUIPaneOutput::_OutputString( MSG_TYPE type, const char* pszString, bool bConcat)
{
	char szString[4096];
	
	switch( type)
	{
	case MSG_NORMAL:
		i3::snprintf( szString, sizeof( szString), "%s", pszString);
		break;
	case MSG_COMPILE:
		i3::snprintf( szString, sizeof( szString), "Compile: %s", pszString);
		break;
	default:
		I3ASSERT(0);
		break;
	}

	// SetWindowText
	if( ::IsWindow( m_Edit.m_hWnd))
	{
		if( bConcat)
		{
			char szBuf[4096];
			m_Edit.GetWindowText( szBuf, sizeof( szBuf));

			i3::snprintf( m_Msg, sizeof(szBuf)-2, "%s\n%s", szBuf, szString);
		}
		else
		{
			i3::string_ncopy_nullpad( m_Msg, szString, sizeof( m_Msg));
		}

		m_Edit.SetWindowText( (LPCTSTR)m_Msg);
	}
	
	ShowPane( TRUE, FALSE, TRUE);
}

void CUIPaneOutput::OutputCompileResult( const char* pszString, bool bConcat)
{
	_OutputString( MSG_COMPILE, pszString, bConcat);
}

void CUIPaneOutput::OutputNotice( const char* pszString, bool bConcat)
{
	_OutputString( MSG_NORMAL, pszString, bConcat);
}

