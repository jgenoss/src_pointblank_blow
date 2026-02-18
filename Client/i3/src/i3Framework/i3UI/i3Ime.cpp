#include "i3FrameworkPCH.h"
#include "i3Ime.h"
#include "i3Base/string/ext/generic_string_size.h"

#include "msctf.h" // ms text services framework(TSF)

I3_CLASS_INSTANCE( i3Ime);

enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
enum { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

wchar_t		g_aszIndicator[5][3] =  
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};

static char strTemp[4096] = "";		//	내부 함수에서만 사용하는 작업용 버퍼입니다. 외부로 노출되면 안됩니다.

#if 0		//	IME 메시지 덤프
	#define MSG_TRACER(szMsg, wParam, lParam, hWnd)	I3TRACE4( "[IME] %s : w %08x, l %08x, hw %p\n", szMsg, wParam, lParam, hWnd);
#else
	#define MSG_TRACER(szMsg, wParam, lParam, hWnd)	((void)0)	
#endif

static i3Ime				*	s_pThis = nullptr;
static i3InputMethodContext	*	g_imm	= nullptr;


static bool gs_enableIMEConsoleKey = false;


ATOM Registeri3IMEWNDClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)i3Ime::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)nullptr;
	wcex.lpszClassName	= "i3ImeWndClass";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

i3Ime::i3Ime(void)
{
	s_pThis	= nullptr;
}

i3Ime::~i3Ime(void)
{
	if( m_hDllIme != nullptr) 
		FreeLibrary( m_hDllIme );

	I3_SAFE_RELEASE( g_imm);

	s_pThis = nullptr;
}

void i3Ime::_clearBuffer( void)
{
	m_input.resize(0);
	m_nInsertPos = 0;

	_clearCompBuffer();
}

void i3Ime::_clearCompBuffer(void)
{
	//	조합 버퍼 클리어
	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);
	m_bCompositioning = false;
}

void i3Ime::ClearBuffer( void)
{
	_clearBuffer();

	_editProc( EDIT_STATE_CLEAR);
}

bool	i3Ime::Create( I3IME_CREATEINFO * pCreateStruct)
{
	g_imm = i3InputMethodContext::new_object();
	g_imm->Create();

	m_hParent = pCreateStruct->_hWnd;

	Registeri3IMEWNDClass(pCreateStruct->_hInst);

	m_hWnd = CreateWindow( "i3ImeWndClass", nullptr, WS_CHILD, 2048, 2048, 0, 0, m_hParent, nullptr, pCreateStruct->_hInst, nullptr);

	m_hImc = ImmGetContext( m_hWnd);

	return (bool)(m_hImc != nullptr);
}

void i3Ime::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	_GetReadingString = nullptr;
    _ShowReadingWindow = nullptr;

	if ( ImmGetIMEFileNameA( m_hKLayout, szImeFile, sizeof( szImeFile ) - 1 ) > 0 ) {
		I3TRACE("IME Test value(filename) : %s\n", szImeFile);
		if( m_hDllIme )
		{
			FreeLibrary( m_hDllIme );
		}
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) {
			_GetReadingString = (UINT32 (WINAPI*)(HIMC, UINT32, LPWSTR, PINT, BOOL*, PUINT32))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) {
				HIMC hImc = ImmGetContext(hWnd);
				if(hImc) {
					_ShowReadingWindow( hImc, false );
					ImmReleaseContext(hWnd, hImc);
				}
			}
		}
	}
}

INT32 i3Ime::_GetInputSize(UINT32 imeStyle, bool bWithComp)
{
	if( imeStyle & IME_STYLE_LIMIT_CHARACTER)
	{
		switch( m_CodePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				if( m_nInsertPos == 0) return 0;

				INT32 len = i3String::WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, nullptr, nullptr);
				I3ASSERT( len < 4096);
				strTemp[len] = 0;
				
				return i3String::LengthChar( m_CodePage, strTemp);
			}
			break;
		default:
			return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
			break;
		}
	}
	else	//	default
	{
		return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
	}
}


bool	i3Ime::SetFocus( bool bFlag)
{

	I3TRACE("i3Ime:SetFocus(bFlag) m_bFocused = From: %d, To: %d \n", m_bFocused, bFlag);
	
	if (m_bImeFocusLock)		// ::SetFocus와 WM_SETFOCUS,WM_KILLFOCUS가 경우에 따라, 반복호출되는 케이스가 있어 막습니다..
		return false;

	m_bImeFocusLock = true;
	m_bFocused = bFlag;

	if( bFlag)
	{
		if( ::GetFocus() != m_hWnd)
		{
			::EnableWindow( m_hWnd, TRUE);

			s_pThis = this;

			SetImeEnable( !m_bAlphaNumericOnly);

			if(m_bAlphaNumericOnly)
				SetImeMode(IME_MODE_ALPHANUMERIC);

			::SetFocus( m_hWnd);

			m_bImeFocusLock = false;

			return true;
		}
	}
	else
	{
		::EnableWindow( m_hWnd, FALSE);

		SetImeEnable( false);

		_clearBuffer();
		::SetFocus(m_hParent);			// nullptr로 되었던것을 예전Ime클래스처럼 도로 m_hParent로 복귀...(11.12.16.수빈)
	}


	m_bImeFocusLock = false;

	return false;
}

void	i3Ime::KillFocus(void)
{

	I3TRACE("KillFocus(): m_bFocused = From: %d, To: %d \n", m_bFocused, 0);
	
	if (m_bImeFocusLock)		// ::SetFocus와 WM_SETFOCUS,WM_KILLFOCUS가 경우에 따라, 반복호출되는 케이스가 있어 막습니다..
		return;
	if (!m_bFocused)
		return;

	m_bImeFocusLock = true;
	m_bFocused = false;

	//chunjong.song 14.6.25 - WM_KEYUP 메시지가 호출되지않고 포커스 해제시 shift,ctrl 눌림상태 지속 해제
	m_bShiftPressed = false;
	m_bCtrlPressed = false;

	m_nBlkStart = 0;
	m_nBlkEnd = 0;
	m_bBlock = false;

	SetInputFinish();	

	_clearBuffer();

	if( ::GetFocus() == m_hWnd )
	{
		SetImeEnable( false);
		//::SetFocus(nullptr);
		::SetFocus(m_hParent);			// nullptr로 되었던것을 예전Ime클래스처럼 도로 m_hParent로 복귀...(11.12.16.수빈)
		//::SetFocus(nullptr);
		//::SetFocus(m_hParent);
	}

	m_bImeFocusLock = false;
}

void	i3Ime::SetImeEnable( bool bEnable)
{

	//DWORD dwConvMode = 0;
	//DWORD dwSentMode = 0;

	if( bEnable)  
	{
		if( m_hImc != 0)
		{
			ImmAssociateContext(m_hParent, m_hImc);			// If you want to enable
															// IME again, then you can
															// use the previous stored IME
															// context(hIMC)
															// to restore IME.

			//ImmGetConversionStatus( m_hImc, &dwConvMode, &dwSentMode );
			//dwConvMode =  dwConvMode & ~IME_CMODE_NATIVE ;
			//ImmSetConversionStatus( m_hImc, dwConvMode, dwSentMode );
		}

		m_nState = EDIT_STATE_NONE;
	
	}
	else
	{

		//ImmSetCompositionString(m_hImc, SCS_SETSTR, TEXT(""), sizeof(TCHAR), TEXT(""), sizeof(TCHAR));
		//ImmSetOpenStatus(m_hImc, FALSE);
		//ImmGetConversionStatus( m_hImc, &dwConvMode, &dwSentMode );
		//dwConvMode =  dwConvMode & ~IME_CMODE_NATIVE ;
		//ImmSetConversionStatus( m_hImc, dwConvMode, dwSentMode );

		ImmAssociateContext(m_hParent, nullptr);		// It makes IME disable 
	
		if( m_hImc != 0)
			_clearCompBuffer();
	}

}

bool i3Ime::SetImeMode(IME_MODE mode)
{
	bool bRet = true;
	HIMC hImc = ImmGetContext( m_hWnd);
	if( hImc == 0)
	{
		I3TRACE("[%s] Warning! Failed to set IME const mode.\n", __FUNCTION__);
		return false;
	}

/*
	//	현재 입력 언어를 체크하여 같다면 처리 안하도록 한다.
	DWORD dwConversion, dwSentence;
	
	if( ImmGetConversionStatus( hImc, &dwConversion, &dwSentence))
	{
		DWORD cmode = dwConversion & IME_CMODE_LANGUAGE;

		switch( mode)
		{
			case IME_MODE_ALPHANUMERIC:		
				{
					if( cmode == IME_CMODE_ALPHANUMERIC)		goto EXIT_IME_MODE;
				}
				break;
			case IME_MODE_NATIVE:			
				{
					if( cmode == IME_CMODE_NATIVE)				goto EXIT_IME_MODE;
				}
				break;
		}
	}
	else
	{
		bRet = false;

		goto EXIT_IME_MODE;
	}
*/

	// chunjong.song 2014.10.16 - ImmGetConversionStatus대신 ImmGetOpenStatus을 사용해서 합니다.
	// ime상태가 영문입력상태가 아닌데도 불구하고  ImmGetConversionStatus함수가 conversion값을  0으로 돌려주는경우가 있어서 
	if(ImmGetOpenStatus(hImc)) 
	{
		// 영문상태가아님
		if(mode != IME_MODE_ALPHANUMERIC) 
			goto EXIT_IME_MODE;
	}
	else
	{
		// 영문상태
		if(mode == IME_MODE_ALPHANUMERIC) 
			goto EXIT_IME_MODE;
	}

	m_ImeMode = mode;

	//	강제로 입력 언어를 변경한다.
	switch( mode)
	{
	case IME_MODE_ALPHANUMERIC:	//	영문 모드로 변경
		{
			if( ImmSetConversionStatus( hImc, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE) == 0)
			{
				I3TRACE("[%] Warning! Failed to set IME IME_CMODE_ALPHANUMERIC mode.\n", __FUNCTION__);

				bRet = false;

				goto EXIT_IME_MODE;
			}
		}
		break;
	case IME_MODE_NATIVE:		//	자국어 모드로 변경
		{
			DWORD dwConvMode = IME_CMODE_NATIVE;
			if (PRIMARYLANGID(m_langID) == LANG_JAPANESE)
			{
				::ImmSetOpenStatus(hImc, TRUE);
				dwConvMode |= IME_CMODE_JAPANESE;
				dwConvMode |= IME_CMODE_FULLSHAPE;
			}

			if (::ImmSetConversionStatus(hImc, dwConvMode, IME_SMODE_CONVERSATION) == 0)
			{
				I3TRACE("[%s] Warning! Failed to set IME IME_CMODE_NATIVE mode.\n", __FUNCTION__);
				
				bRet = false;

				goto EXIT_IME_MODE;
			}
		}
		break;
	}

EXIT_IME_MODE:
	ImmReleaseContext(m_hWnd, hImc);

	return bRet;
}

DWORD i3Ime::GetImeModeValue()
{
	DWORD ret = 0xFFFFFFFF;
	HIMC hImc = ImmGetContext( m_hWnd);
	if( hImc)
	{
		DWORD dwConversion, dwSentence;
		if( ImmGetConversionStatus( hImc, &dwConversion, &dwSentence))
		{
			ret = (dwConversion & 0xFFFF);
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "[%s] Failed. GetStatus.", __FUNCTION__);
		}

		ImmReleaseContext( m_hWnd, hImc);

		return ret;
	}

	I3TRACE("[%s] failed ImmGetContext.\n", __FUNCTION__);

	return ret;
}

void i3Ime::SetEnableMultiLine(bool bEnable)
{
	m_bMultiLine = bEnable;
}

void i3Ime::CheckToggleState(HWND hWnd)
{
	// Update Indicator
    switch (PRIMARYLANGID(m_langID)) 
	{
    case LANG_KOREAN:
        m_wszLangIndicator = g_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_wszLangIndicator = g_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

		switch(SUBLANGID(m_langID)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_wszLangIndicator = g_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszLangIndicator = g_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszLangIndicator = g_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

	case LANG_THAI:
		m_wszLangIndicator = g_aszIndicator[INDICATOR_NON_IME];
        break;

    default:
        m_wszLangIndicator = g_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszLangIndicator == g_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langID, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszLangIndicator[0] = szLang[0];
        m_wszLangIndicator[1] = towlower( szLang[1] );
    }
		
	// Check Toggle State 
	bool bIme = ImmIsIME( m_hKLayout ) != 0;

	HIMC hImc = ImmGetContext(hWnd);
    if( hImc ) 
	{
		DWORD dwConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

		if( ( PRIMARYLANGID(m_langID) == LANG_CHINESE ) && bIme ) 
		{
			m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
		} 
		else if( ( PRIMARYLANGID(m_langID) == LANG_THAI ) )  
		{
			m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;	
		}
		else 
		{
			m_ImeState = ( bIme && ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}

		//	check mode
		DWORD dwLang = dwConvMode & IME_CMODE_LANGUAGE;

		if (PRIMARYLANGID(m_langID) == LANG_JAPANESE && dwLang == 0)
		{
			dwLang = IME_CMODE_NATIVE;
		}

		switch(dwLang)
		{
		case IME_CMODE_ALPHANUMERIC:	m_ImeMode = IME_MODE_ALPHANUMERIC;	break;
		case IME_CMODE_NATIVE:			m_ImeMode = IME_MODE_NATIVE;		break;
		default:						m_ImeMode = IME_MODE_UNKNOWN;		break;
		}

		ImmReleaseContext(hWnd, hImc);
	}
	else 
	{
        m_ImeState = IMEUI_STATE_OFF;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void	i3Ime::Reset(void)
{
	ClearBuffer();
}

bool	i3Ime::InsertInputFromCompositionString(HWND hWnd)
{
	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
	if( !IsLimit(false) ||  m_nLimit == 0)
	{
		HIMC hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			INT32 limitSize = 0;
			INT32 checkSize = 0;
			INT32 tempSize = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, nullptr, 0);

			wchar_t* temp = (wchar_t*) alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, temp, tempSize);
			INT32 length = tempSize / sizeof(wchar_t);
			INT32 inputSize = GetInputLengthW();	//	must without comp		

			//	문자 갯수로 체크
//			if( isStyle(IME_STYLE_LIMIT_CHARACTER))
//			{
				checkSize = length;				
				limitSize = getLimit();
//			}
			//	바이트 갯수로 체크
//			else
//			{
//				checkSize = ImmGetCompositionStringA(hImc, GCS_RESULTSTR, nullptr, 0);
//				limitSize = getLimit() / sizeof(wchar_t);
//			}

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
			if((m_nLimit > 0) && !IsEnough(checkSize, false))
			{
				INT32 overheadSize = (inputSize + length) - limitSize;	//	유니코드를 기준으로 
				
				if( overheadSize < 0)		overheadSize = 0;

				//	뒤에서 짤라내고 마지막에 출력해야할 문자 크기만 계산한다.
				length -= overheadSize;

				if( length < 0)
					length = 0;
			}

			if( m_nInsertPos <= GetInputLengthW())		
			{
				//	중간 삽입
				for( INT32 i = 0; i < length; i++)
				{
				  	//alt + = 키를 누른 후엔 특수기호값이 넘어온다..
					//특수기호값 리턴
					if (PRIMARYLANGID(m_langID) != LANG_JAPANESE)
					{
						if (temp[i] >= 0xFF01 && temp[i] <= 0xFFE3)
							continue;
					}

					m_input.insert( m_input.begin() + m_nInsertPos, 1, temp[ i]);	
					m_nInsertPos++;
				}
			}

			ImmReleaseContext(hWnd, hImc);

			_clearCompBuffer();

			m_bCompositioning = false;

			//	문자 조합후(END_COMPOSITION)  키처리들
			switch( m_PostKey)
			{
			case VK_LEFT:		
				{
					//if( m_nInsertPos > 0)	
					//	m_nInsertPos--;	
				}
				break;
			case VK_RIGHT:		break;
			case VK_DELETE:		break;
			case VK_HOME:		
				{
					m_nInsertPos = 0; 
				}
				break;
			case VK_END:		
				{
					m_nInsertPos = m_input.size();
				}
				break;
			case VK_BACK:		break;	
			case VK_SPACE:
				{
					/*if( m_nInsertPos < m_nLimit)
					{	//	공백 추가
						m_input.insert( m_input.begin() + m_nInsertPos, 1, TCHAR(' '));					

						m_nInsertPos++;
					}*/
				}	
				break;
			case VK_RETURN:		
				{
					m_PostKey = 0;

					//	Enter키 입력 처리
					////////////////////////////////////	edit proc
					_editProc( EDIT_STATE_RETURN);

					return true;
				}
				break;
			default:
				break;
			}

			m_PostKey = 0;	//	처리후 제거

			////////////////////////////////////	edit proc
			_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
		}

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void	i3Ime::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Change the setting if input language is changed
	m_hKLayout = (HKL)lParam;
	m_langID = LOWORD(m_hKLayout);

	if( PRIMARYLANGID(m_langID) == LANG_ENGLISH)		//	ENGLISH(US) skip (use ANSI)
		m_CodePage = 0;
	else
		m_CodePage = (WORD) i3Language::GetCodePageFromLang(m_langID);
	
	// Check Property 
	m_dwProperty = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicode = (m_dwProperty & IME_PROP_UNICODE) ? true : false;

	// Update Indicator
	CheckToggleState(hWnd);

	// Bind Proc
	SetupImeApi(hWnd);

	////////////////////////////////////	edit proc
	_editProc(EDIT_STATE_CHANGE_LANGUAGE);
}

bool	i3Ime::OnStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( isStyle(IME_STYLE_NUMERIC_ONLY))
	{
		//	'0' ~ '9' 값만 받는다.
		if(wParam < 48 || wParam > 57)
			return false;
	}

	g_imm->SetCompositioning( this);

	OnSetCompWindowPos();

	////////////////////////////////////	edit proc
	_editProc(EDIT_STATE_COMPOSITION_START);

	return true;
}

bool	i3Ime::OnComposition( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( isStyle(IME_STYLE_NUMERIC_ONLY))
	{
		//	'0' ~ '9' 값만 받는다.
		if(wParam < 48 || wParam > 57)
			return false;
	}

	HIMC hImc;
	bool bRet = true;

	m_nState = EDIT_STATE_NONE;

	//	Retrieves or updates the string of the composition result. 
	if(lParam&GCS_RESULTSTR) 
	{
		InsertInputFromCompositionString(hWnd);

		bRet = false;
		
		goto Exit;
	}

	//	Retrieves or updates the current composition string. 
	if(lParam&GCS_COMPSTR) 
	{
		hImc = ImmGetContext(hWnd);
		if(hImc != nullptr)
		{
			INT32 addSize = 0;

			if( m_bBlock)
			{
				DeleteNChars( m_nBlkStart, m_nBlkEnd);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
				m_bBlock = false;
			}

			if( isStyle(IME_STYLE_LIMIT_CHARACTER))
			{
				// not including the null terminating character. The return value is always the size in bytes, 
				// even if the requested data is a Unicode string
				addSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
				addSize = addSize / sizeof(wchar_t);
			}
			else
			{
				addSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
				addSize = addSize/ sizeof(wchar_t);
			}

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
			INT32 tempSize	= ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
			INT32 wsize		= tempSize/sizeof(wchar_t);
			wchar_t* temp	= (wchar_t*) alloca(tempSize+2);

			ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);
			temp[wsize] = 0;

			if( IsEnough(addSize, false) ||  m_nLimit == 0)
			{
				m_comp.assign(temp, temp+wsize);

				ImmReleaseContext(hWnd, hImc);

				m_bCompositioning = true;

				//Update
				m_nCompositionCursorIndex = 0xffff & ImmGetCompositionStringW(hImc, GCS_CURSORPOS, nullptr, 0);
								
				if (getInputLanguage() != LANG_JAPANESE)
				{
					////////////////////////////////////	edit proc
					_editProc(EDIT_STATE_CHANGE_COMPOSITION_CHAR);
				}
			}
			else
			{
				ImmReleaseContext(hWnd, hImc);

				SetInputCancel();

				goto Exit;
			}
		}
	}

	//	Retrieves or updates the attribute of the composition string.
	if(lParam&GCS_COMPATTR) 
	{
		hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			INT32 start, end;
			INT32 tempSize = ImmGetCompositionStringW(hImc, GCS_COMPATTR, nullptr, 0);
			if( tempSize > 0)
			{
				BYTE* temp = (BYTE*) alloca(sizeof(BYTE) * tempSize);

				ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, sizeof(BYTE) * tempSize);

				for( start=0; start<tempSize; ++start) 
					if(temp[start]==ATTR_TARGET_CONVERTED || temp[start]==ATTR_TARGET_NOTCONVERTED) break;
				for( end=start; end<tempSize; ++end) 
					if(temp[end]!=temp[start]) break;

				m_ulStart	= start;
				m_ulEnd		= end;

				if (getInputLanguage() == LANG_JAPANESE)
				{
					_editProc(EDIT_STATE_CHANGE_COMPOSITION_CHAR);
				}
			}

			ImmReleaseContext(hWnd, hImc);
		}			
	}

Exit:
	//OnSetCompWindowPos();

	return bRet;
}

bool	i3Ime::OnEndComposition( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( isStyle(IME_STYLE_NUMERIC_ONLY))
	{
		//	'0' ~ '9' 값만 받는다.
		if(wParam < 48 || wParam > 57)
			return false;
	}

	_clearCompBuffer();

	g_imm->SetCompositioning(nullptr);

	////////////////////////////////////	edit proc
	_editProc(EDIT_STATE_COMPOSITION_END);

   return true;
}

bool	i3Ime::OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
		m_nState = EDIT_STATE_NONE;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:		// Set candidate window position
		{
			m_bSelectingCandidate = true;

			if (m_dwProperty & IME_PROP_AT_CARET)
			{
				OnSetCompWindowPos();
			}
		}

		MSG_TRACER( "IMN_OPENCANDIDATE", wParam, lParam, hWnd);
		return true;

	case IMN_CHANGECANDIDATE:
		if( IMN_CHANGECANDIDATE == wParam)
		{
			MSG_TRACER( "IMN_CHANGECANDIDATE", wParam, lParam, hWnd);
		}
		return true;

	case IMN_CLOSECANDIDATE:
		MSG_TRACER( "IMN_CLOSECANDIDATE", wParam, lParam, hWnd);
		m_bSelectingCandidate = false;
		return true;
		break;

	case IMN_SETCONVERSIONMODE:		// To detect the change of IME mode 
		{
			MSG_TRACER( "IMN_SETCONVERSIONMODE", wParam, lParam, hWnd);

			
			// 영어<->다국어전환 불가상태인가
			if(isStyle(IME_STYLE_IGNORE_KEYINPUT_IME_MODE))
			{
				IME_MODE curMode = GetImeMode();
				HIMC hImc = ImmGetContext(m_hWnd);
				
				if(hImc) 
				{
					BOOL opened = ::ImmGetOpenStatus(hImc);
					::ImmReleaseContext(m_hWnd, hImc);

					// 영문전용모드임에도 다국어입력상태인가
					if(opened) 
					{   
						// IME입력기는 다국어상태,
                        // i3Ime는 영문상태이므로 IME입력기를 영문으로바꾼다
						if(curMode == IME_MODE_ALPHANUMERIC)
						{	
							SetImeMode(IME_MODE_ALPHANUMERIC);
							return false;
						}
					}
					else
					{   
                        // IME입력기는 영문상태
                        // i3Ime는 다국어상태, IME입력기를 다국어상태로바꾼다
						if(curMode != IME_MODE_ALPHANUMERIC)
						{	
							SetImeMode(curMode);
							return false;
						}
					}
				}				
			}
		}
		//	No break;
	case IMN_SETOPENSTATUS:			// To detect the toggling of Japanese IME
		{
			if( wParam == IMN_SETOPENSTATUS)	MSG_TRACER( "IMN_SETOPENSTATUS", wParam, lParam, hWnd);

			{
				CheckToggleState(hWnd);

				////////////////////////////////////	edit proc
				_editProc(EDIT_STATE_CHANGE_IME_MODE);

				return true;
			}
		}
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		{
			MSG_TRACER( "IMN_SETCOMPOSITIONWINDOW", wParam, lParam, hWnd);
			return true;
		}
		break;
    case IMN_PRIVATE:
		MSG_TRACER( "IMN_PRIVATE", wParam, lParam, hWnd);
        switch(lParam) { 
        case WM_IME_STARTCOMPOSITION: 
            MSG_TRACER("WM_IME_STARTCOMPOSITION", wParam, lParam, hWnd);
            break; 
        case WM_IME_COMPOSITION: 
            MSG_TRACER("WM_IME_COMPOSITION", wParam, lParam, hWnd);
            break; 
        case WM_IME_ENDCOMPOSITION: 
            MSG_TRACER("WM_IME_ENDCOMPOSITION", wParam, lParam, hWnd);
            break; 
        default: 
            MSG_TRACER("default: ", wParam, lParam, hWnd);
            break; 
        } 

		return true;
        break;
	case IMN_SETSENTENCEMODE:
		MSG_TRACER( "IMN_SETSENTENCEMODE", wParam, lParam, hWnd);
		break;
	default: 
		MSG_TRACER( "DEFAULT notify:", wParam, lParam, hWnd);
		break;
	}

	return false;
}

bool	i3Ime::OnChar( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//Block이 어딘가에 설정되어 있는 상태에서
	//OnChar로 들어오면 일단 먼저 무조건 그부분을 지운다.
	if( m_bBlock)
	{
		DeleteNChars( m_nBlkStart, m_nBlkEnd);
		m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
		m_bBlock = false;
	}



	switch(wParam) 
	{
	case '\r':	// Return key
	case '\n':
		{
			//OnKeyDownReturn(hWnd);
		}
		break;
	case '\b':	// Back space key	
		{
			//OnKeyDownBackspace();
		}
		break;
	case '\t':
	case 27:
		break;
	default:
		{
			if( isStyle(IME_STYLE_NUMERIC_ONLY))
			{
				//	'0' ~ '9' 값만 받는다.
				if(wParam < 48 || wParam > 57)
					break;
			}
			else if( isStyle(IME_STYLE_NUMERIC_AND_NUMERICAL))
			{
				//	'0' ~ '9', '+', '-', '.' 값만 받는다.
				if(wParam != 43 && wParam != 45 && wParam != 46)
				{
					if(wParam < 48 || wParam > 57)
						break;
				}
			}
			else if(isStyle(IME_STYLE_ALPHA_NUMERIC_ONLY))
			{				
				if(isalnum(wParam) == 0)
					break;
			}
			else if(isStyle(IME_STYLE_ALPHA_ONLY))
			{
				if(isalpha(wParam) == 0)
					break;
			}
			
			if(wParam > 31) 
			{
				//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
				//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
				if( IsEnough((char) wParam) || m_nLimit == 0)
				{
					wchar_t temp;
					bool bAddChar = true;

					i3String::MultiByteToWideChar(m_CodePage, 0, (char*)&wParam, 1, &temp, 1);
					
					//	태국어는 조합 문자가 WM_IME_STARTCOMPOSITION 메시지로 넘어오지 않으며 WM_CHAR로 개별 문자가 넘어오므로 개별 처리해야 한다.
					if( m_CodePage == I3_LANG_CODE_THAI)	//	Check Thai composite character
					{
						char prev = 0;
						char curr = (char) wParam;

						//	현재 문자가 태국어일때만 
						if( curr & 0x80)
						{
							//	현재 문자와 비교하기 위해 입력 버퍼의 맨 마지막 문자만 가져온다.
							if( m_input.size() > 0)
							{
								//INT32 last = m_input.size() - 1;		
								INT32 last = m_nInsertPos - 1;
								WideCharToMultiByte(m_CodePage, 0, &m_input[last], 1, &prev, 1, nullptr, nullptr);								

								//	이전 문자가 태국어 문자일때만, 이전 문자가 태국어가 아니면 조합될 필요없다.
								if( prev & 0x80)
								{
									//	현재 넘어온 문자가 태국어에서 조합 가능한 문자인지 검사하여 조합 불가능 상태라면 입력 버퍼에 추가하지 않는다.
									if( !i3String::IsComposite(m_CodePage, prev, curr, 2))
									{
										//	혼자 자립할수 있는 태국어 문자라면 (자음이나 모음은 한글자로 입력될수 있으나 성조같은 것은 자립해서 입력하지 못하도록)
										if( !i3String::IsComposite(m_CodePage, 0, curr, 2))
										{
											bAddChar = false;
										}
									}
								}
							}				
						}
					}

					if( m_CodePage == I3_LANG_CODE_CHINESE_SIMPLIFIED || m_CodePage == I3_LANG_CODE_CHINESE_TRADITIONAL)
					{
						if(wParam > 160)
						{
							bAddChar = false;
						}
					}

					if(bAddChar)
					{
						if(isStyle(IME_STYLE_AUTO_UPPERCASE))
						{
							temp = towupper(temp);
						}
						else if(isStyle(IME_STYLE_AUTO_LOWERCASE))
						{
							temp = towlower(temp);
						}

						if( m_nInsertPos < GetInputLengthW())
							//	중간 삽입
							m_input.insert( m_input.begin() + m_nInsertPos, 1, temp);
						else
							m_input.push_back(temp);
						
						m_nInsertPos++;
						////////////////////////////////////	edit proc
						_editProc(EDIT_STATE_CHANGE_INPUT_CHAR);
					}
				}
			}
		}
		break;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void	i3Ime::OnKeyDownLeft( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_LEFT;
		//	조합 종료
		SetInputFinish();		
	}
	else
	{
		if( m_nInsertPos > 0)	
		{
			m_nInsertPos--;	

			if( m_bShiftPressed)
			{
				m_nBlkEnd = m_nInsertPos;
			}
			else
			{
				m_bBlock = false;
			}

			_editProc( EDIT_STATE_ONKEYDOWN_LEFT);
		}
		else if( m_nInsertPos == 0 && !m_bShiftPressed)
		{
			m_bBlock = false;

			_editProc( EDIT_STATE_ONKEYDOWN_LEFT);
		}
	}
}

void	i3Ime::OnKeyDownRight( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_RIGHT;
		//	조합 종료
		SetInputFinish();		
	}
	else
	{
		if( m_nInsertPos < GetInputLengthW())
		{
			m_nInsertPos++;
			if( m_bShiftPressed)
			{
				m_nBlkEnd = m_nInsertPos;
			}
			else
			{
				m_bBlock = false;
			}

			_editProc( EDIT_STATE_ONKEYDOWN_RIGHT);
		}
		else if( m_nInsertPos == GetInputLengthW() && !m_bShiftPressed )
		{
			m_bBlock = false;

			_editProc( EDIT_STATE_ONKEYDOWN_RIGHT);
		}
	}
}

void i3Ime::OnKeyDownDown( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_DOWN;
		//	조합 종료
		SetInputFinish();		
	}

	_editProc( EDIT_STATE_ONKEYDOWN_DOWN);
}

void i3Ime::OnKeyDownUp( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_UP;
		//	조합 종료
		SetInputFinish();		
	}
	
	_editProc( EDIT_STATE_ONKEYDOWN_UP);
}

void	i3Ime::SetInsertPosition( INT32 pos)
{
	m_nInsertPos = pos;

	if( m_bShiftPressed)
	{
		m_nBlkEnd = m_nInsertPos;
	}
	else
	{
		m_bBlock = false;
	}
}

void	i3Ime::OnKeyDownDelete( void)
{
	if( isCompositioning())	return;		//	조합중에 사용 안하도록

	switch( m_CodePage)
	{
	case I3_LANG_CODE_THAI:		//	태국어는 한 문자가 n개의 조합형 유니코드로 이루어 졌기 때문에 한 문자를 지우려면 가변 크기를 계산해서 여러번 지워야한다.
		{
			if( m_nInsertPos < GetInputLengthW())
			{
				WCHAR * pStart = &m_input[m_nInsertPos];
				INT32 lenStart = i3::generic_string_size( pStart);

				WCHAR * pNext = (WCHAR*) ::CharNextW( pStart);
				INT32 lenNext = i3::generic_string_size( pNext);

				INT32 delCnt =  lenStart - lenNext;

				for(INT32 i=0; i<delCnt; i++)
					m_input.erase( m_input.begin() + m_nInsertPos);
			}
		}
		break;
	default:
		{
			if( m_bBlock)
			{
				DeleteNChars( m_nBlkStart, m_nBlkEnd);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
				m_bBlock = false;
			}
			else
			{
				if( m_nInsertPos < (INT32)m_input.size())
					DeleteNChars( m_nInsertPos, m_nInsertPos+1);
			}
		}
		break;
	}

	_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
}

void	i3Ime::OnKeyDownHome( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_HOME;
		//	조합 종료
		SetInputFinish();		
	}
	
	_editProc( EDIT_STATE_ONKEYDOWN_HOME);
}

void	i3Ime::OnKeyDownEnd( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_END;
		//	조합 종료
		SetInputFinish();		
	}
	
	_editProc( EDIT_STATE_ONKEYDOWN_END);
}

void	i3Ime::OnKeyDownBackspace( void)
{
	if( isCompositioning())	return;		//	조합중에 사용 안하도록

	if( m_bBlock)
	{
		DeleteNChars( m_nBlkStart, m_nBlkEnd);
		m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
		m_bBlock = false;
	}
	else
	{
		if( m_nInsertPos > 0)
		{
			DeleteNChars( m_nInsertPos-1, m_nInsertPos);
			m_nInsertPos--;	
		}
	}

	_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
}

void	i3Ime::OnKeyDownReturn( HWND hWnd)
{
	if( m_FeedEnterCount <= 0)
	{
		// Return key
		if( isCompositioning())
		{	//	조합중이면 
			m_PostKey = VK_RETURN;	

			//	조합 종료
			SetInputFinish();	
		}
		else
		{
			//	조합 종료
			SetInputFinish();	

			if( m_bBlock)
			{
				DeleteNChars( m_nBlkStart, m_nBlkEnd);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
				m_bBlock = false;
			}

			_editProc( EDIT_STATE_RETURN);
		}
	}
	else
	{
		m_FeedEnterCount--;
	}
}

void	i3Ime::OnKeyDownSpacebar( HWND hWnd)
{
	if( isCompositioning())
	{
		m_PostKey = VK_SPACE;
		//	조합 종료
		SetInputFinish();		
	}
}

void	i3Ime::OnKeyDownShift( HWND hWnd)
{
	m_bShiftPressed = true;
}

void	i3Ime::OnKeyDownControl( HWND hWnd)
{
	m_bCtrlPressed = true;
}

void	i3Ime::OnKeyDownEscape( HWND hWnd)
{
	m_bBlock = false;

	_editProc( EDIT_STATE_ONKEYDOWN_ESCAPE);
}

// ******** OnKeyUp ********
void	i3Ime::OnKeyUpShift( HWND hWnd)
{
	m_bShiftPressed = false;
}

void	i3Ime::OnKeyUpControl( HWND hWnd)
{
	m_bCtrlPressed = false;
}

void i3Ime::DeleteNChars( INT32 nStart, INT32 nEnd)
{
	INT32 nMin = min( nStart, nEnd);
	INT32 nMax = max( nStart, nEnd);

	// 마지막 커서에 위치한 경우 nStart와 nEnd가 동일하여 하나를 삭제하기 위해 선택영역을 줍니다.
	/*if( nMin == nMax && nMin > 0)
		nMin--;*/

	m_input.erase( nMin, nMax-nMin);

	//m_nInsertPos = nMin;
}

void i3Ime::InsertNChars( INT32 nInsPos, WCHAR* pChars, INT32 nLen)
{
	m_input.insert( nInsPos, pChars, nLen);

	//m_nInsertPos = nInsPos + nLen;
}

void i3Ime::ReplaceNChars( INT32 nStart, INT32 nEnd, WCHAR* pChars, INT32 nLen)
{
	INT32 nMin = min( nStart, nEnd);
	INT32 nMax = max( nStart, nEnd);

	m_input.replace( nMin, nMax-nMin, pChars, 0, nLen);

	//m_nInsertPos = nMin + nLen;
}


void i3Ime::OnKeyDownPageup( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_PRIOR;
		//	조합 종료
		SetInputFinish();		
	}

	_editProc(EDIT_STATE_ONKEYDOWN_PAGEUP);
}

void i3Ime::OnKeyDownPagedown( HWND hWnd)
{
	if( isCompositioning())
	{	//	조합중이면 
		m_PostKey = VK_NEXT;
		//	조합 종료
		SetInputFinish();		
	}

	_editProc( EDIT_STATE_ONKEYDOWN_PAGEDOWN);
}


void i3Ime::Copy( HWND hWnd)
{
	if( false == m_bUseCopyNPaste )
	{
		return;
	}

	if( m_bBlock)
	{
		HGLOBAL hmem;
		WCHAR16* ptr;

		int start = min( m_nBlkStart, m_nBlkEnd);
		int end = max( m_nBlkStart, m_nBlkEnd);
		int len = end - start;
		
		if( len > 0)
		{
			/*WCHAR* str = (WCHAR*)malloc( len*sizeof(WCHAR)+1);
			wmemcpy( str, &m_buf[start], len);
			str[ len] = 0;*/
				
			hmem = GlobalAlloc( GHND, (len+1)*sizeof(WCHAR));
			ptr = (WCHAR16*)GlobalLock( hmem);
			wmemcpy( ptr, &m_input[start], len);
			ptr[ len] = 0;
			GlobalUnlock( hmem);
			
			if( OpenClipboard( hWnd))
			{
				EmptyClipboard();
				SetClipboardData( CF_UNICODETEXT, hmem);
				//SetClipboardData( CF_TEXT, hmem);
				CloseClipboard();
			}
			else
			{
				GlobalFree( hmem);
			}
		}

		_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
	}
}

void i3Ime::Cut( HWND hWnd)
{
	if( false == m_bUseCopyNPaste )
	{
		return;
	}

	if( m_bBlock)
	{
		Copy( hWnd);
		DeleteNChars( m_nBlkStart, m_nBlkEnd);
		m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
		
		m_bBlock = false;

		_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
	}
}

void i3Ime::Paste( HWND hWnd)
{
	if( false == m_bUseCopyNPaste )
	{
		return;
	}

	if( IsClipboardFormatAvailable( CF_UNICODETEXT))
	{
		HGLOBAL hmem;
		WCHAR16* ptr;

		BOOL bResult = ::OpenClipboard( hWnd);

		if( bResult == FALSE)
			return;

		hmem = GetClipboardData( CF_UNICODETEXT);
		ptr = (WCHAR16*)GlobalLock( hmem);
		INT32 len = wcslen( ptr);

		//	중간 삽입
		for( INT32 i = 0; i < len; i++)
		{
			//alt + = 키를 누른 후엔 특수기호값이 넘어온다..
			//'*' 로 변신
			if( ptr[i] >= 0xFF01 &&  ptr[i] <= 0xFFE3 )
				ptr[i] = '*';
		}

		// revision 2073
		// 클랜 가입쪽지 51자부터 붙여넣기 문제
		/*INT32 nBytelen = WideCharToMultiByte( m_CodePage, 0, ptr, wcslen( ptr), strTemp, 4096, nullptr, nullptr);
		INT32 ninputBytelen = WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, nullptr, nullptr);*/
		INT32 nBytelen = 0;
		INT32 ninputBytelen = 0;

		//	문자갯수로체크
//		if( isStyle(IME_STYLE_LIMIT_CHARACTER))
//		{
			nBytelen = wcslen( ptr);
			ninputBytelen = m_input.size();
//		}
		//바이트 갯수로 체크
//		else
//		{
//			nBytelen = i3String::WideCharToMultiByte( m_CodePage, 0, ptr, wcslen( ptr), strTemp, 4096, nullptr, nullptr);
//			ninputBytelen = i3String::WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, nullptr, nullptr);
//		}
		
		if( m_bBlock)
		{
			INT32 nblkBytelen = 0;
			INT32 nblkstart = min( m_nBlkStart, m_nBlkEnd);
			INT32 nblkend = max( m_nBlkStart, m_nBlkEnd);
			INT32 nBlockLen = nblkend - nblkstart;
			
			if (nBlockLen <= 0) return;
			
			std::wstring tstr;
			tstr.resize(nBlockLen);
			std::copy( m_input.begin()+nblkstart, m_input.begin()+nblkend-1,  tstr.begin());

			nblkBytelen = i3String::WideCharToMultiByte( m_CodePage, 0, tstr.c_str(), tstr.size(), strTemp, 4096, nullptr, nullptr);

			if( ninputBytelen - nblkBytelen + nBytelen <= m_nLimit)		//signed/unsigned mismatch
			{
				ReplaceNChars( m_nBlkStart, m_nBlkEnd, ptr, len);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd) + len;
				m_bBlock = false;
			}
		}
		else
		{
			if( ninputBytelen + nBytelen <= m_nLimit)		//signed/unsigned mismatch
			{
				InsertNChars( m_nInsertPos, ptr, len);
				m_nInsertPos += len;
			}
		}

		GlobalUnlock( hmem);
		CloseClipboard();

		_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
	}
}

void i3Ime::SelectAll( HWND hWnd)
{
	m_nBlkStart = 0;
	m_nBlkEnd = m_nInsertPos = m_input.length();
	m_bBlock = true;

	_editProc( EDIT_STATE_CHANGE_INPUT_CHAR);
}

////////////////////////////////////////////////////////////////////////////////////////////////
const	wchar_t	* i3Ime::GetText(void) 
{
	GetText(&m_text);

	return m_text.c_str();
}

const i3::wstring&  i3Ime::GetTextString() 
{
	GetText(&m_text);
	return m_text;
}

INT32	i3Ime::GetText( i3::wstring * pinput)
{
	GetInput(pinput);

	if( m_comp.size() > 0)
		pinput->insert( getInsertPos(), (wchar_t*)m_comp.c_str(), m_comp.size());

	return pinput->size();
}

INT32	i3Ime::GetInput( i3::wstring * pinput)
{
//	pinput->assign(m_input);
	*pinput = m_input;

	return m_input.size();
}

INT32	i3Ime::GetComp( i3::wstring * pcomp)
{
	pcomp->assign( m_comp);

	return m_comp.size();
}

INT32	i3Ime::SetInputText(const WCHAR * text, INT32 len, bool bLimitText)
{
	INT32 length = 0;
	
	if( text != nullptr)
		length = i3::generic_string_size( text);
	
	//	길이가 0이상일 경우만
	if( text == nullptr || len == 0 || i3::generic_string_size( text) == 0)
	{
		_clearBuffer();

		return 0;
	}
	else
	{
		INT32 limitSize = 0;			
		INT32 inputSize = 0;
		INT32 checkSize = 0;

		//	IME 입력 버퍼 클리어
		_clearBuffer();

		//	문자 갯수로 체크
		if( isStyle(IME_STYLE_LIMIT_CHARACTER))
		{
			checkSize = length;
			limitSize = getLimit();
			inputSize = GetInputLengthW();	//	must without comp		
		}
		//	바이트 갯수로 체크
		else
		{
			checkSize = len;
			limitSize = getLimit();
			inputSize = GetInputLengthW();	//	must without comp		
		}

		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
		//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
		if( bLimitText && !IsEnough(checkSize, false))
		{
			INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

			//	바이트 단위일 경우
//			if( !isStyle(IME_STYLE_LIMIT_CHARACTER))
//			{
//				overheadSize = (overheadSize / sizeof(wchar_t)) + (overheadSize % sizeof(wchar_t));
//			}

			//	뒤에서 짤라내고 마지막에 출력해야할 문자 크기만 계산한다.
			length -= overheadSize;
			
			if( length < 0)
				length = 0;
		}

		INT32 i;

		for( i = 0; i < length; i++)
		{	//	IME 입력 버퍼에 삽입
			m_input.insert( m_input.begin() + i, 1, text[ i]);	
		}

		m_nInsertPos = i;

		return length;
	}
}

INT32	i3Ime::AddInputText(const WCHAR * text, INT32 len, bool bLimitText)
{
	INT32 length = len;
	INT32 limitSize = 0;			
	INT32 inputSize = 0;
	INT32 checkSize = 0;

	//	길이가 0이상일 경우만
	if(text == nullptr || len == 0 || i3::generic_string_size( text) == 0)
		return 0;

	if( bLimitText)
	{
		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
		if( IsLimit(false) && m_nLimit > 0)
			return 0;
	}	
	
	//	문자 갯수로 체크
	if( isStyle(IME_STYLE_LIMIT_CHARACTER))
	{
		checkSize = length;
		limitSize = getLimit();
		inputSize = GetInputLengthW();	//	must without comp		
	}
	//	바이트 갯수로 체크
	else
	{
		checkSize = len;
		limitSize = getLimit();
		inputSize = GetInputLengthW();	//	must without comp		
	}

	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
	//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
	if( bLimitText && !IsEnough(checkSize, false))
	{
		INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

		//	바이트 단위일 경우
//		if( !isStyle(IME_STYLE_LIMIT_CHARACTER))
//		{
//			overheadSize = (overheadSize / sizeof(wchar_t)) + (overheadSize % sizeof(wchar_t));
//		}

		//	뒤에서 짤라내고 마지막에 출력해야할 문자 크기만 계산한다.
		length -= overheadSize;
		
		if( length < 0)
			length = 0;
	}

	//	문자 삽입
	for( INT32 i = 0; i < length; i++)
	{
		m_input.insert( m_input.begin() + m_nInsertPos, 1, text[i]);	

		m_nInsertPos++;
	}

	if( IsLimit(false) && m_nLimit > 0) 
		_clearCompBuffer();

	m_bCompositioning = false;
	////////////////////////////////////	edit proc
	_editProc(EDIT_STATE_CHANGE_INPUT_CHAR);

	return length;
}

void	i3Ime::SetInputFinish()
{
	if( isCompositioning())	//	조합 글자 입력 중지
	{
		HIMC hImc = ImmGetContext( m_hWnd);		

		//	조합 중지
		PostMessage( m_hWnd, WM_IME_ENDCOMPOSITION, 0, 0);

		//	조합중 문자 마무리
		ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0); 

		_clearCompBuffer();		

		ImmReleaseContext( m_hWnd, hImc);
	}	
}

void	i3Ime::SetInputCancel()
{
	if( isCompositioning())	//	조합 글자 입력 중지
	{
		_clearCompBuffer();

		//	조합 중지
		PostMessage( m_hWnd, WM_IME_ENDCOMPOSITION, 0, 0);

		HIMC hIMC = ImmGetContext( m_hWnd);

		//	조합중 문자 취소
		ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); 

		ImmReleaseContext( m_hWnd, hIMC);
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////
void	i3Ime::SetImeWindowPosition( INT32 x, INT32 y)
{
	m_WindowPosition.x = x;
	m_WindowPosition.y = y;
}

void	i3Ime::OnSetCompWindowPos()
{	
	HIMC hImc = ImmGetContext( m_hWnd);
	if( hImc)
	{
		m_nCompositionCursorIndex = 0xffff & ImmGetCompositionStringW(hImc, GCS_CURSORPOS, nullptr, 0);

		ImmReleaseContext( m_hWnd, hImc);
	}

	SetCompositionWndPos();
}

void	i3Ime::SetCompositionWndPos()
{
	SIZE		pixel;
	INT32		xpos		= 0;
	HDC			hdc			= ::GetDC( m_hWnd);
	HFONT		hOldFont	= nullptr;

	if( m_hFont != nullptr)	 hOldFont = (HFONT)SelectObject( hdc, m_hFont);

	// input pos
	GetTextExtentPoint32W( hdc, m_input.data(), m_nInsertPos, &pixel);
	xpos = pixel.cx;

	// composition pos
	if((m_comp.size() == 0) || (m_nCompositionCursorIndex == 0))
	{
		//I3TRACE("candiindex: (   %d    )  compsize: (    %d     ) x : (   %d   )\n", m_nCompositionCursorIndex, m_comp.size(), 0);

		if (getInputLanguage() == LANG_JAPANESE)
		{
			SetCandidateWndPos(xpos, 0);
		}
		else
		{
			SetCompositionWndPos(xpos, 0);
		}
	}
	else
	{
		GetTextExtentPoint32W( hdc, m_comp.data(), m_nCompositionCursorIndex, &pixel);

		//I3TRACE("candiindex: (   %d    )  compsize: (    %d     ) x : (   %d   )\n", m_nCompositionCursorIndex, m_comp.size(), pixel.cx);

		if (getInputLanguage() == LANG_JAPANESE)
		{
			SetCandidateWndPos(xpos + pixel.cx, 0);
		}
		else
		{
			SetCompositionWndPos(xpos, 0);
		}
	}

	if( hOldFont != nullptr)	::SelectObject( hdc, hOldFont);

	::ReleaseDC( m_hWnd, hdc);
}

void	i3Ime::SetCompositionWndPos(INT32 x, INT32 y)
{
	COMPOSITIONFORM CompForm;
		
	HIMC hImc = ImmGetContext(m_hWnd);
	if( hImc != nullptr)
	{
		CompForm.dwStyle = CFS_POINT;

		CompForm.ptCurrentPos.x = (INT32)m_WindowPosition.x - (INT32)I3IME_DUMMYWND_OFFSET + x;
		CompForm.ptCurrentPos.y = (INT32)m_WindowPosition.y - (INT32)I3IME_DUMMYWND_OFFSET + y;

		//I3TRACE("Set COMPOSITION WINDOW : posx: %d,  posY: %d\n", (INT32)(CompForm.ptCurrentPos.x + I3IME_DUMMYWND_OFFSET), (INT32)(CompForm.ptCurrentPos.y + I3IME_DUMMYWND_OFFSET));

		// 조합 윈도우의 위치를 설정한다.
		if( ::ImmSetCompositionWindow(hImc, &CompForm) == FALSE)
		{
			I3TRACE("failed set composition window position \n");
		}

		ImmReleaseContext(m_hWnd, hImc);
	}
}

void i3Ime::SetCandidateWndPos(INT32 x, INT32 y)
{
	HIMC hImc = ImmGetContext(m_hWnd);

	CANDIDATEFORM cdf;

	if (m_dwProperty & IME_PROP_AT_CARET)
	{
		cdf.dwIndex = 0;
		cdf.dwStyle = CFS_DEFAULT;
		//cdf.dwStyle = CFS_CANDIDATEPOS;
		cdf.ptCurrentPos.x = (INT32)m_WindowPosition.x - (INT32)I3IME_DUMMYWND_OFFSET + x;
		cdf.ptCurrentPos.y = (INT32)m_WindowPosition.y - (INT32)I3IME_DUMMYWND_OFFSET + y;
		ImmSetCandidateWindow(hImc, &cdf);
	}
	else
	{
		UINT i;

		// The candidate position should be decided by a near caret
		// IME. There are 4 candidate form in the input context

		for (i = 0; i < 4; i++)
		{
			if (!ImmGetCandidateWindow(hImc, i, &cdf))
			{
				continue;
			}

			if (cdf.dwStyle == CFS_DEFAULT)
			{
				continue;
			}

			cdf.dwStyle = CFS_DEFAULT;

			ImmSetCandidateWindow(hImc, &cdf);
		}

	}

	ImmReleaseContext(m_hWnd, hImc);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void	i3Ime::setImeFont( HFONT hFont, LOGFONTA * pFont)
{
	m_hFont = hFont;

	HIMC hImc = ImmGetContext( m_hWnd);
	if( hImc != nullptr)
	{
		ImmSetCompositionFont( hImc, pFont);

		ImmReleaseContext( m_hWnd, hImc);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
INT32 i3Ime::GetInputLengthA( void)
{
	return i3String::WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_input.size(), nullptr, 0, nullptr, nullptr);
}

INT32 i3Ime::GetCompLengthA( void)
{
	return i3String::WideCharToMultiByte( m_CodePage, 0, m_comp.c_str(), m_comp.size(), nullptr, 0, nullptr, nullptr);
}

INT32 i3Ime::GetInsertPositionA( void)		
{ 
	return i3String::WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_nInsertPos, nullptr, 0, nullptr, nullptr);
} 

INT32 i3Ime::GetCursorPosition(void)
{
	switch( m_CodePage)
	{
	case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
		{
			if( m_nInsertPos == 0) return 0;

			INT32 len = i3String::WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_nInsertPos, strTemp, sizeof(strTemp), nullptr, nullptr);
			I3ASSERT( len < sizeof(strTemp));
			strTemp[len] = 0;
			
			return i3String::LengthChar( m_CodePage, strTemp);
		}
		break;
	default:
		return getInsertPos();
		break;
	}
}
/*****************************************************************************\
* Function: GetCombinedCharLength
*
* Get the length of combined character. 
*
* This implementation only support Surrogate. You need to handle Combined 
* characters as necessary.  
*
* return value:
*	If UNICODE, 
*		2 if surrogate char, else return 1
*	If not UNICODE
*		2 if DBCS char, else return 1
\*****************************************************************************/
INT32 i3Ime::GetCombinedCharLength( INT32 nTmpPos)
{
	INT32 i, nRet = 1;

	// Check surrogate char
	for( i = 0; i < nTmpPos; i++)
	{
		if( (0xD800 <= m_input[i]) && (m_input[i] <= 0xDBFF))
		{
			nRet = 2;
			++i;
		}
		else
			nRet = 1;
	}
	
	return nRet;
}

/*****************************************************************************\
* Function: GetStringWidth
*
* Calculate string width in pixel 
* 정상적으로 동작하기 위해선, ime가 enable될때, font및 size를 정상적으로 설정 해주어야 합니다
* Arguments:
*	LPCTSTR szStr - string 
\*****************************************************************************/
INT32 i3Ime::GetStringWidth( const wchar_t * szStr, INT32 size)
{
	SIZE		pixel;
	HDC			hdc = ::GetDC( m_hWnd);

	HFONT	oldFont = (HFONT)SelectObject( hdc, m_hFont);

	if( size <= 0)	size = wcslen( szStr);

	GetTextExtentPoint32W( hdc, szStr, size, &pixel);

	SelectObject( hdc, oldFont);

	::ReleaseDC( m_hWnd, hdc);

	return pixel.cx;
}

bool i3Ime::IsLimit(bool bWithComp)
{
	INT32 nInputLength = _GetInputSize( getStyle(), bWithComp);

	return (m_nLimit <= nInputLength); 
}

bool i3Ime::IsEnough(INT32 nAddSize, bool bWithComp)
{
	INT32 nInputLength = _GetInputSize( getStyle(), bWithComp);
	
	return (m_nLimit >= nInputLength + nAddSize); 
}

bool i3Ime::IsEnough(char cAddChar)
{
	INT32 nInputLength = 0;

	if( getStyle() & IME_STYLE_LIMIT_CHARACTER)
	{
		switch( m_CodePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				//if( m_nInsertPos == 0)	return true; 

				INT32 len = WideCharToMultiByte( m_CodePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, nullptr, nullptr);
				I3ASSERT( len < 4095);

				strTemp[len] = cAddChar;
				strTemp[len+1] = 0;
				
				nInputLength = i3String::LengthChar( m_CodePage, strTemp);
			}
			break;
		default:
			nInputLength = GetInputLengthW() + 1;
			break;
		}
	}
	else	//	default
	{
		nInputLength = GetInputLengthW() + 1;
	}
	
	return (m_nLimit >= nInputLength); 
}

////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT	i3Ime::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( s_pThis != nullptr)
	{
		return s_pThis->imeWndProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc( hWnd, message, wParam,lParam);
}

LRESULT	i3Ime::imeWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultProc)
{
	switch( message)
	{
	case WM_CREATE:	
		OnInputLanguageChange( hWnd, 0, (LPARAM) GetKeyboardLayout(0));
		
		return 0L;

	// IME Messages
	case WM_INPUTLANGCHANGE:
		MSG_TRACER( "WM_INPUTLANGCHANGE", wParam, lParam, hWnd);
		{
			OnInputLanguageChange( hWnd, wParam, lParam);		
			if ((PRIMARYLANGID(m_langID) == LANG_JAPANESE))
			{
				HIMC hImc = ImmGetContext(hWnd);

				DWORD dwConvMode, dwSentMode;
				::ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

				::ImmSetOpenStatus(hImc, TRUE);
				dwConvMode |= IME_CMODE_JAPANESE;
				dwConvMode |= IME_CMODE_FULLSHAPE;
				::ImmSetConversionStatus(hImc, dwConvMode, dwSentMode);

				::ImmReleaseContext(hWnd, hImc);
			}
		}
		break;

	// Except IME mode control in Janap Win9x, dose not need to send this message
	case WM_IME_SETCONTEXT:
		{
			MSG_TRACER( "WM_IME_SETCONTEXT", wParam, lParam, hWnd);

			OnInputLanguageChange( hWnd, 0, (LPARAM) GetKeyboardLayout(0));
			
			return DefWindowProc( hWnd, message, wParam, lParam);
		}
		break;

	// Start composition
	case WM_IME_STARTCOMPOSITION:
		MSG_TRACER( "WM_IME_STARTCOMPOSITION", wParam, lParam, hWnd);
		OnStartComposition(hWnd, wParam,lParam);
		return 0L;
		break;

	// Composotion char
	case WM_IME_COMPOSITION:
		MSG_TRACER( "WM_IME_COMPOSITION", wParam, lParam, hWnd);
		OnComposition( hWnd, wParam, lParam);
		return 0L;
		break;

	// End conposition
	case WM_IME_ENDCOMPOSITION:
		MSG_TRACER( "WM_IME_ENDCOMPOSITION", wParam, lParam, hWnd);
		OnEndComposition( hWnd, wParam, lParam);
		return 0L;
		break;
	
	// IME Notofy
	case WM_IME_NOTIFY:
		//MSG_TRACER( "WM_IME_NOTIFY", wParam, lParam, hWnd);
		if( OnNotify( hWnd, wParam, lParam) == false)
		{
			return 0L;
		}
		break;
	//	Process Character
	case WM_CHAR:
		{	
			if( !m_bFunctionalKey)
			{
				MSG_TRACER( "WM_CHAR", wParam, lParam, hWnd);
				//	문자 입력을 받는다.
				OnChar( hWnd, wParam, lParam);
			}

			m_bFunctionalKey = false;

			return 0L;
		}
		break;

	//	Process Keyboad
	case WM_KEYDOWN:
		{
			MSG_TRACER( "WM_KEYDOWN", wParam, lParam, hWnd);
			
			if( m_bCtrlPressed)
			{
				switch( wParam)
				{
					case 'C' :
					case 'G' :		
						Copy( hWnd);		
						m_bFunctionalKey = true;		
						break;
					case 'X' :
					case 'H' :		
						Cut( hWnd);			
						m_bFunctionalKey = true;		
						break;
					case 'V' :
					case 'B' :
						Paste( hWnd);		
						m_bFunctionalKey = true;		
						break;
					case 'A' :		
						SelectAll( hWnd);	
						m_bFunctionalKey = true;		
						break;

					case 0xc0 : // '`'  -- ctrl+'`' , 0x11(VK_CONTROL), 0xC0 이런순서로 들어옴
						if(gs_enableIMEConsoleKey)
						{
							// kbdHook에서 선행처리가 된후(콘솔열기), 이곳에 들어올것임
							SetInputFinish();

							// WM_KEYUP이 안올떄가있어서
							OnKeyUpControl(hWnd);
							m_bFunctionalKey = true;
						}
						break;

					//case 'D' :		
						//PrintDebugText();		
						//break;		//temp
					default :								
						break;
				}

				
			}
			else
			{
				if( m_bShiftPressed)
				{
					switch( wParam)
					{
					case VK_RIGHT:
					case VK_LEFT:
					case VK_DOWN:
					case VK_UP:
					case VK_HOME:
					case VK_END:
					case VK_PRIOR:
					case VK_NEXT:
						if( !m_bBlock)
						{
							m_bBlock = true;
							m_nBlkStart = m_nBlkEnd = m_nInsertPos;
						}
						break;
					default:
						break;
					}
				}
				
				switch( wParam)
				{
					case VK_LEFT :		OnKeyDownLeft( hWnd);		break;
					case VK_RIGHT :		OnKeyDownRight( hWnd);		break;
					case VK_DOWN :		OnKeyDownDown( hWnd);		break;
					case VK_UP :		OnKeyDownUp( hWnd);			break;
					case VK_PRIOR :		OnKeyDownPageup( hWnd);		break;	//pageup
					case VK_NEXT :		OnKeyDownPagedown( hWnd);	break;	//pagedown
					case VK_HOME :		OnKeyDownHome( hWnd);		break;
					case VK_END :		OnKeyDownEnd( hWnd);		break;
					case VK_DELETE :	OnKeyDownDelete();			break;
					case VK_BACK :		OnKeyDownBackspace();		break;
					case VK_SPACE :		OnKeyDownSpacebar( hWnd);	break;
					case VK_RETURN :	OnKeyDownReturn( hWnd);		break;
					case VK_SHIFT :		OnKeyDownShift( hWnd);		break;
					case VK_CONTROL :	OnKeyDownControl( hWnd);	break;
					case VK_ESCAPE :	OnKeyDownEscape( hWnd);		break;
					case VK_PROCESSKEY: 
						{
							if( I3_LANG_CODE_CHINESE_SIMPLIFIED != i3Language::GetCurrentCodePage() )
							{
								break;
							}

							LPARAM scan = (lParam & 0x00FF0000) >> 16;
							UINT vk	= ::MapVirtualKey(scan, MAPVK_VSC_TO_VK_EX); 
							if ( vk != 0 )
							{
								if (vk == VK_RETURN)
								{
									OnKeyDownReturn(hWnd);
								}
							}
						}


						break;
					
					default: 			m_FeedEnterCount = 0;		break;	// 피드카운트를 초기화합니다.
				}
			}
		}

		return 0L;
		break;
	case WM_KEYUP:
		{
			MSG_TRACER( "WM_KEYUP", wParam, lParam, hWnd);
			
			switch( wParam)
			{
				case VK_SHIFT :			OnKeyUpShift( hWnd);		break;
				case VK_CONTROL :		OnKeyUpControl( hWnd);		break;
				default:	break;
			}
		}
		break;

	case WM_SETFOCUS:		
		{
			MSG_TRACER( "WM_SETFOCUS", wParam, lParam, hWnd);
			SetImeMode(m_ImeMode);
			CaptureImeMode();

			return 0L;
		}
		break;
	//	Kill focus
	case WM_KILLFOCUS:
		{
			MSG_TRACER( "WM_KILLFOCUS", wParam, lParam, hWnd);
			SetInputFinish();		//	조합 중지	
			RevertImeMode();

			return 0L;
		}
		break;
	case WM_SYSKEYUP:
		{
			MSG_TRACER( "WM_SYSKEYUP", wParam, lParam, hWnd);
			
			switch( wParam)
			{
				case VK_CONTROL :		OnKeyUpControl( hWnd);		break;
				default:	break;
			}
		}
		break;

	default:
		{
			I3TRACE("default : %x", message); 
			MSG_TRACER( "  param", wParam, lParam, hWnd);
		}
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam);
}

void i3Ime::_editProc( EDIT_STATE state)
{
	m_nState = state;

	switch( state)
	{
	case EDIT_STATE_RETURN:
		if( m_bMultiLine)
		{
			AddInputText( L"\n", i3::generic_string_size( L"\n"));
		}
		break;	//?
	case EDIT_STATE_CLEAR:				
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	
	case EDIT_STATE_CHANGE_INPUT_CHAR:	
		{	
			GetText( &m_text);

			// 포커스를 잃었다면, TextBuf에 있는 내용을 초기화 합니다.
			if( !m_bFocused)
			{
				m_text.clear();
			}
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

void i3Ime::EnableConsoleKey(bool enable)
{
	gs_enableIMEConsoleKey = enable;
}


//
// Text Service Framework
//

// import라이브러리는 없고, 자주사용되지않을거라 판단되어, 함수사용후 DLL해제합니다.
bool i3Ime::ShowLanguageBarWnd(bool show)
{
	bool rst = false;
	typedef HRESULT (WINAPI *PTF_CREATELANGBARMGR)(ITfLangBarMgr**);	
	HMODULE hMSCTF = ::LoadLibraryA("msctf.dll");

	if(!hMSCTF)
		return rst;
	
    PTF_CREATELANGBARMGR pfnCreateLangBarMgr;
    pfnCreateLangBarMgr = (PTF_CREATELANGBARMGR)GetProcAddress(hMSCTF, "TF_CreateLangBarMgr");

    if(pfnCreateLangBarMgr)
    {
        ITfLangBarMgr* pLangBarMgr = nullptr;
		if(SUCCEEDED((*pfnCreateLangBarMgr)(&pLangBarMgr)))
        {
			DWORD flag = TF_SFT_SHOWNORMAL;			

			if(!show) 
				flag = TF_SFT_HIDDEN;

			if(SUCCEEDED(pLangBarMgr->ShowFloating(flag)))
				rst = true;

            pLangBarMgr->Release();
        }
    }

	::FreeLibrary(hMSCTF);

	return rst;
}

//************************************
// User:      kiwon.kim
// Date:  	  2015/11/20
// Access:    public 
// Returns:   void
// Qualifier: Ime style에 따라서 기존 Ime를 저장하고 영어로 바꾼다
//************************************
void i3Ime::CaptureImeMode()
{
	if (isStyle(IME_STYLE_NUMERIC_ONLY | IME_STYLE_NUMERIC_AND_NUMERICAL | IME_STYLE_ALPHA_NUMERIC_ONLY | IME_STYLE_ALPHA_ONLY))
	{
		HIMC hImc = ImmGetContext(m_hWnd);

		DWORD dwConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

		m_dwConvMode = dwConvMode;
		m_dwSentMode = dwSentMode;

		::ImmSetOpenStatus(hImc, TRUE);
		::ImmSetConversionStatus(hImc, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);

		::ImmReleaseContext(m_hWnd, hImc);
	}

}
//************************************
// User:      kiwon.kim
// Date:  	  2015/11/20
// Access:    public 
// Returns:   void
// Qualifier: ime를 원래대로 되돌린다
//************************************
void i3Ime::RevertImeMode()
{
	if (isStyle(IME_STYLE_NUMERIC_ONLY | IME_STYLE_NUMERIC_AND_NUMERICAL | IME_STYLE_ALPHA_NUMERIC_ONLY | IME_STYLE_ALPHA_ONLY))
	{
		HIMC hImc = ImmGetContext(m_hWnd);

		::ImmSetOpenStatus(hImc, TRUE);
		::ImmSetConversionStatus(hImc, m_dwConvMode, m_dwSentMode);

		::ImmReleaseContext(m_hWnd, hImc);
	}
}
