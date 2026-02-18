#include "i3SceneDef.h"
#include "i3IME.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/utf16_to_mb.h"

//I3_CLASS_INSTANCE( i3IME, i3ElementBase);
I3_CLASS_INSTANCE( i3IME);

static i3IME *		s_pThis = nullptr;

ATOM RegisterIMEClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)i3IME::ControlDummyProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= "i3IMEClass";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

i3IME::~i3IME(void)
{	
	I3_SAFE_RELEASE( m_pInputIME);

	::DestroyWindow( m_hWnd);

	s_pThis = nullptr;

	m_hParentHwnd = nullptr;
	m_hWnd = nullptr;
}

void i3IME::CreateIME( CREATE_IME_ELEMENT* pStruct)
{
	m_hParentHwnd = pStruct->_hWnd;

	RegisterIMEClass(pStruct->_hInst);

	// IME Dummy 컨트롤 생성
	m_hWnd = ::CreateWindow( "i3IMEClass", NULL, WS_CHILD, I3IME_DUMMYWND_OFFSET, I3IME_DUMMYWND_OFFSET, 0, 0, m_hParentHwnd, NULL, pStruct->_hInst, NULL);

	// IME에 사용되는 Text Buffer 생성 및 초기화
	{
	//	I3MEM_SAFE_FREE(m_pTextBuf);

	//	m_nTextMaxCount = (pStruct->_nTextLimitCount * 4) + 2;	//	native language long size

	//	m_pTextBuf = (char*)i3MemAlloc(m_nTextMaxCount);
	//	i3mem::FillZero( m_pTextBuf, sizeof(char)*m_nTextMaxCount);
	}

	// Input IME 생성 및 초기화
	{
		if( m_pInputIME) I3_SAFE_RELEASE( m_pInputIME);
		m_pInputIME = i3InputIME::new_object();
		m_pInputIME->Create( pStruct->_nTextLimitCount, pStruct->_StyleIME);
		m_pInputIME->OnInputLanguageChange( m_hWnd, 0, (LPARAM) GetKeyboardLayout(0));

		m_pInputIME->SetEditProc( IMEProc, (void*)this);
	}
	
	if((pStruct->_StyleIME & IME_STYLE_MULTILINE) != 0) m_bEnableMultiLine = true;
}

void i3IME::setAlphaNumericOnly(bool bANOnly)
{
	m_bAlphaNumericOnly = bANOnly;
}

void i3IME::SetWindowPosition(INT32 x, INT32 y)
{
	if( m_pInputIME != nullptr)
	{
		m_pInputIME->SetIMEOffsetPosition( x, y);
	}
}

void i3IME::SetIMEFont( HFONT hFont, LOGFONTA * lpFont)
{
	if( m_pInputIME != nullptr)
	{
		m_pInputIME->setImeFont( m_hWnd, hFont, lpFont);
	}
}

void i3IME::KillFocus()
{
	if( m_pInputIME != nullptr)
	{
		m_pInputIME->SetInputFinish(m_hWnd);	

		SetIMEEnable(false);
	}

	::SetFocus( m_hParentHwnd);
}

bool i3IME::SetFocus( bool bFlag)	
{ 
	m_bFocus = bFlag;

	if( bFlag)
	{
		if( ::GetFocus() != m_hWnd)
		{
			::EnableWindow( m_hWnd, TRUE);

			s_pThis = this;

			::SetFocus( m_hWnd);

			SetIMEEnable((m_bAlphaNumericOnly == false));

			return true;
		}
	}
	else
	{
		::EnableWindow( m_hWnd, FALSE);

		::SetFocus( m_hParentHwnd);

		SetIMEEnable( false);

		m_wstrTextBuf.clear();
	}

	return false;
}

void i3IME::SetIMEEnable( bool bFlag)
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

	m_pInputIME->SetImeEnable( m_hWnd, bFlag);
}

void i3IME::SetIMEMode( IME_MODE mode)
{
	m_pInputIME->SetImeMode( m_hWnd, mode);
}

IME_MODE i3IME::GetIMEMode(void)
{
	return m_pInputIME->GetImeMode();
}

/*
const char* i3IME::getIMETextBuf(void)			
{	
	return m_pTextBuf;	
}
*/

void i3IME::SetInput( const i3::wliteral_range& wTextRng )
{
//	I3ASSERT( pszText != nullptr);

	m_pInputIME->SetInputCancel( m_hWnd);
	m_pInputIME->SetInput( wTextRng );
	m_pInputIME->GetExactInput( m_wstrTextBuf);
}

void i3IME::DebugDump()
{
	m_pInputIME->DumpBuffer();

	i3::string str;
	i3::utf16_to_mb(m_wstrTextBuf, str);
	I3TRACE("i3IME::DebugDump::m_pTextBuf : %s\n", str);
}

void i3IME::InputDone(void)
{
	//	IME 버퍼 상태를 초기화한다.
	m_pInputIME->ClearBuffer();

	m_nState = EDIT_STATE_NONE;
}

void i3IME::StateProcess(EDIT_STATE state)
{
	//char szTemp[1024];
	m_nState = state;

	switch( state)
	{
	case EDIT_STATE_RETURN:
		if( m_bEnableMultiLine)
		{
			m_pInputIME->AddInput( L"\n");
		}
	case EDIT_STATE_CLEAR:				
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	
	case EDIT_STATE_CHANGE_INPUT_CHAR:	
		{	
			m_pInputIME->GetExactInput(	m_wstrTextBuf ); //	m_pTextBuf, getIMETextBufCount());

#if 0		//	현재 입력 버퍼와 조합중 문자를 출력합니다.
			DebugDump();
#endif

			// 포커스를 읽었다면, TextBuf에 있는 내용을 초기화 합니다.
			if( !m_bFocus) 
				m_wstrTextBuf.clear();
		}
		break;
	case EDIT_STATE_CHANGE_LANGUAGE:					break;		
	default:		break;
	}

	if( m_pOwner != nullptr)
	{
		m_pOwner->OnEvent( I3_EVT_IME, this, m_nState);
	}
}

LRESULT i3IME::ControlDummyProc( HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	if( s_pThis)
	{
		if( s_pThis->getIME() != nullptr)
			return s_pThis->getIME()->MessageProc( hwnd, message, wParam, lParam);
	}

	return DefWindowProc( hwnd, message, wParam,lParam);
}

void i3IME::IMEProc( i3InputIME * pIME, EDIT_STATE state, void * pParam)
{
	i3IME* pIMEClass = (i3IME*)pParam;

	I3ASSERT(pIMEClass->m_pInputIME == pIME);

	pIMEClass->StateProcess(state);
}