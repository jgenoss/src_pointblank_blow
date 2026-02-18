#include "i3FrameworkPCH.h"
#include "i3IMEHandler.h"

I3_CLASS_INSTANCE( i3IMEHandler, i3ElementBase);

static i3IMEHandler *		s_pThis;	

i3IMEHandler::i3IMEHandler( void* pVoid)
{
	m_pOwner	= NULL;

	m_pTextBuf = NULL;
	m_pIME = NULL;
	m_nTextMaxCount = 0;
	m_bFocus = FALSE;
	m_bEnableMultiLine = FALSE;

	s_pThis = NULL;
}

i3IMEHandler::~i3IMEHandler(void)
{	
	I3MEM_SAFE_FREE( m_pTextBuf);
	I3_SAFE_RELEASE( m_pIME);

	::DestroyWindow( m_hWnd);

	s_pThis = NULL;
}

void i3IMEHandler::_ClearBuffer(void)
{
	if( m_pTextBuf != NULL)
	{
		i3mem::FillZero( m_pTextBuf, sizeof(WCHAR)*(m_nTextMaxCount));
	}
}

void i3IMEHandler::CreateIME( IME_CREATE_CONTEXT* pStruct)
{
	m_hParentHwnd = pStruct->_hWnd;

	// IME Dummy 컨트롤 생성
	m_hWnd = CreateWindow( "static", NULL, WS_CHILD, 2048, 2048, 1, 1, m_hParentHwnd, NULL, pStruct->_hInst, NULL);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (long)ControlDummyProc);

	// IME에 사용되는 Text Buffer 생성 및 초기화
	{
		I3MEM_SAFE_FREE(m_pTextBuf);

		m_nTextMaxCount = (pStruct->_nTextLimitCount) + 2;	//	native language long size
		m_pTextBuf = (WCHAR*)i3MemAlloc(m_nTextMaxCount * sizeof( WCHAR));
		_ClearBuffer();
	}

	// Input IME 생성 및 초기화
	{
		if( m_pIME) I3_SAFE_RELEASE( m_pIME);
		m_pIME = i3InputMethodEditor::NewObject();

		m_pIME->Create( pStruct->_nTextLimitCount, pStruct->_StyleIME);
		m_pIME->OnInputLanguageChange( m_hWnd, 0, (LPARAM) GetKeyboardLayout(0));

		m_pIME->SetEditProc( IMEProc, (void*)this);
	}

	if((pStruct->_StyleIME & IME_STYLE_MULTILINE) != 0) m_bEnableMultiLine = TRUE;
}

void i3IMEHandler::KillFocus()
{
	if( m_pIME != NULL)
		m_pIME->SetInputFinish(m_hWnd);		

	::SetFocus( NULL);
}

BOOL i3IMEHandler::SetFocus( BOOL bFlag)	
{ 
	m_bFocus = bFlag;

	if( bFlag)
	{
		if( ::GetFocus() != m_hWnd)
		{
			::EnableWindow( m_hWnd, TRUE);

			s_pThis = this;

			::SetFocus( m_hWnd);

			return TRUE;
		}
	}
	else
	{
		::EnableWindow( m_hWnd, FALSE);

		::SetFocus( m_hParentHwnd);

		_ClearBuffer();
	}

	return FALSE;
}

void i3IMEHandler::SetIMEEnable( BOOL bFlag)
{
	// 지금까지의 모든 Input Key들을 시작하기 전에 캔슬시킨다.
	// 이것은 Enter키가 흔히 입력창을 활성화시키기도, 비활성화시키기도 하는
	// 기능으로 중복해 사용되기 때문에 발생하는 문제를 피하기 위함이다.
	/*
	{
		MSG msg;

		while( PeekMessage( &msg, m_hWnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));
	}
	*/

	m_pIME->SetImeEnable( m_hWnd, bFlag);
}

void i3IMEHandler::SetIMEMode( IME_MODE mode)
{
	m_pIME->SetImeMode( m_hWnd, mode);
}

IME_MODE i3IMEHandler::GetIMEMode(void)
{
	return m_pIME->GetImeMode();
}

const WCHAR* i3IMEHandler::getIMETextBuf(void)			
{	
	return m_pTextBuf;	
}

void i3IMEHandler::SetInput( const WCHAR* pszText)
{
	I3ASSERT( pszText != NULL);

	m_pIME->SetInputCancel( m_hWnd);
	m_pIME->SetInput( pszText, i3String::LengthW( pszText));
	m_pIME->GetExactInput( m_pTextBuf, getIMETextBufCount());
}

void i3IMEHandler::DebugDump()
{
	m_pIME->DumpBuffer();

	I3TRACE( L"i3IMEHandler::DebugDump::m_pTextBuf : %s\n", m_pTextBuf);
}

void i3IMEHandler::InputDone(void)
{
	//	IME 버퍼 상태를 초기화한다.
	m_pIME->ClearBuffer();

	m_nState = EDIT_STATE_NONE;
}

void i3IMEHandler::StateProcess(EDIT_STATE state)
{
	//char szTemp[1024];
	m_nState = state;

	switch( state)
	{
	case EDIT_STATE_RETURN:
		if( m_bEnableMultiLine)
		{
			m_pIME->AddInput( L"\n", i3String::LengthW( L"\n"));
		}
	case EDIT_STATE_CLEAR:				
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	
	case EDIT_STATE_CHANGE_INPUT_CHAR:	
		{	
			m_pIME->GetExactInput( m_pTextBuf, getIMETextBufCount());
			INT32 nLength = i3String::LengthW( m_pTextBuf);
			//INT32 nLength2 = getIMETextBufCount();
			m_pTextBuf[ nLength+1] = 0;

#if 0		//	현재 입력 버퍼와 조합중 문자를 출력합니다.
			DebugDump();
#endif

			// 포커스를 읽었다면, TextBuf에 있는 내용을 초기화 합니다.
			if( !m_bFocus)
				_ClearBuffer();
		}
		break;
	case EDIT_STATE_CHANGE_LANGUAGE:					break;		
	default:		break;
	}

	if( m_pOwner != NULL)
	{
		m_pOwner->OnEvent( I3_EVT_IME, this, m_nState);
	}
}

LRESULT i3IMEHandler::ControlDummyProc( HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	LRESULT rt = 0;

	if( s_pThis)
	{
		if( s_pThis->getIME() != NULL)
			rt = s_pThis->getIME()->MessageProc( hwnd, message, wParam, lParam);
	}

	return rt;
}

void i3IMEHandler::IMEProc( i3InputMethodEditor* pIME, EDIT_STATE state, void * pParam)
{
	i3IMEHandler* pIMEClass = (i3IMEHandler*)pParam;

	I3ASSERT(pIMEClass->m_pIME == pIME);

	pIMEClass->StateProcess(state);
}