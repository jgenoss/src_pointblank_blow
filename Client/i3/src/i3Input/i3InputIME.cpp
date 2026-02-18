#include "i3InputType.h"
#include "i3InputIMEContext.h"
#include "i3InputIME.h"

#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#if defined( I3_WINDOWS)

#include <imm.h>
#include <winver.h>

/*----------------------------------------------------------------------------*/ 

#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

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

//	Input Context management
static i3InputIMEContext * g_imm = nullptr;

static char			g_szTemp[4096] = "";		//	내부 함수에서만 사용하는 작업용 버퍼입니다. 외부로 노출되면 안됩니다.
//static wchar_t		g_wszTemp[2048] = L"";

#if 0		//	IME 메시지 덤프
#define MSG_TRACER(szMsg, wParam, lParam, hWnd)	I3TRACE4( "[IME] %s : w %08x, l %08x, hw %p\n", szMsg, wParam, lParam, hWnd);
#else
	#define MSG_TRACER(szMsg, wParam, lParam, hWnd)	((void)0)	
#endif

/*
INT32	ConvertString(UINT32 codePage, const wchar_t* wText, INT32 wLen, char* text, INT32 len)
{
	I3ASSERT( codePage != 0xff);

	if(text == 0) 
	{
		return i3String::WideCharToMultiByte(codePage, 0, wText, wLen, nullptr, 0, nullptr, nullptr);
	} 
	else 
	{
		i3mem::FillZero( text, len);

		INT32 tLen = i3String::WideCharToMultiByte(codePage, 0, wText, wLen, nullptr, 0, nullptr, nullptr);

		if(tLen > len) 
		{
			return 0;
		} 
		else 
		{
			return i3String::WideCharToMultiByte(codePage, 0, wText, wLen, text, tLen, nullptr, nullptr);
		}
	}
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE( i3InputIME);

i3InputIME::i3InputIME() : m_ImeState(IMEUI_STATE_OFF)
{
}

i3InputIME::~i3InputIME( void)
{
	if( m_hDllIme != nullptr)
		FreeLibrary( m_hDllIme );
	
	if( g_imm != nullptr)
	{
		bool bReleaseContext = false;

		if( g_imm->GetRefCount() == 1)
			bReleaseContext = true;

		g_imm->Release();	

		if( bReleaseContext)
			g_imm = nullptr;
	}
}

void	i3InputIME::Create( INT32 nLimit, UINT32 nStyle)
{
	if( g_imm == nullptr)
	{
		g_imm = i3InputIMEContext::new_object_ref();
		I3ASSERT( g_imm != nullptr);

		g_imm->Create();
	}

	g_imm->AddRef();	

	m_nLimit = nLimit;
	m_nStyle = nStyle;

	//	TextFrameService (고급 텍스트) 중지. Vista에선 사라짐.
	//ImmDisableTextFrameService( GetCurrentThreadId());
}

void i3InputIME::SetEditProc( I3_IME_EDIT_PROC pFunc, void * pParam)
{
	m_pEditProc = pFunc; 
	m_pParam = pParam;
}

void i3InputIME::Reset( void)
{	
	ClearBuffer();
}

bool i3InputIME::IsNT( void)
{
	I3ASSERT( g_imm != nullptr);

	return g_imm->IsNT();
}

void i3InputIME::_clearBuffer( void)
{
	m_input.resize(0);
	m_nInsertPos = 0;

	_clearCompBuffer();
}

void i3InputIME::_clearCompBuffer(void)
{
	//	조합 버퍼 클리어
	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);
	m_bIsComp = false;
}

void i3InputIME::ClearBuffer( void)
{
	_clearBuffer();

	m_nState = EDIT_STATE_CLEAR;

	if( m_pEditProc != nullptr)
		m_pEditProc( this, m_nState, m_pParam);
}

void	i3InputIME::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Change the setting if input language is changed
	m_hkl = (HKL)lParam;
	m_langId = LOWORD(m_hkl);

	if( PRIMARYLANGID(m_langId) == LANG_ENGLISH)		//	ENGLISH(US) skip (use ANSI)
		m_codePage = 0;
	else
		m_codePage = (WORD) i3Language::GetCodePageFromLang(m_langId);
	
	// Check Property 
	m_dwImeProp = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicodeIME = (m_dwImeProp & IME_PROP_UNICODE) ? true : false;

	m_bCallDefProc = !g_imm->isVistaorLater();

	CheckToggleState( hWnd);

	// Update m_dwId[]
	GetImeId();

	// Bind Proc
	SetupImeApi(hWnd);

	m_nState = EDIT_STATE_CHANGE_LANGUAGE;

	if( m_pEditProc != nullptr)
		m_pEditProc( this, m_nState, m_pParam);
}

bool	i3InputIME::OnStartComposition( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	g_imm->SetCompositioning(this);

	//SetCompositionPos( hWnd);
	OnSetCompWindowPos(hWnd);

	m_nState = EDIT_STATE_COMPOSITION_START;
	if( m_pEditProc != nullptr)
	{
		m_pEditProc( this, m_nState, m_pParam);
	}

	return true;
}
 
bool	i3InputIME::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	// Composotion char
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

			if( IsImeStyle(IME_STYLE_LIMIT_CHARACTER))
			{
				// not including the null terminating character. The return value is always the size in bytes, 
				// even if the requested data is a Unicode string
				addSize = ::ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);
				addSize = addSize / sizeof(wchar_t);
			}
			else
			{
				addSize = ::ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);
				addSize = addSize / sizeof(wchar_t);
			}

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
			INT32 tempSize	= ::ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);
			INT32 wsize		= tempSize/sizeof(wchar_t);
			wchar_t* temp	= (wchar_t*) alloca(tempSize+2);

			::ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);
			temp[wsize] = 0;

			if( !IsLimitEnough(addSize, false) ||  m_nLimit == 0)
			{
				m_comp.assign(temp, temp+wsize);

				ImmReleaseContext(hWnd, hImc);

				m_nState = EDIT_STATE_CHANGE_COMPOSITION_CHAR;
				m_bIsComp = true;

				if( m_pEditProc != nullptr)
					m_pEditProc( this, m_nState, m_pParam);
			}
			else
			{
				ImmReleaseContext(hWnd, hImc);

				SetInputCancel( hWnd);
				/*
				//	제한 갯수를 넘으면 조합 문자를 제거합니다.
				ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				
 				//::ImmSetCompositionStringW(hImc, SCS_SETSTR, "\0", 1, NULL, NULL);
				

				_clearCompBuffer();
				*/
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
			INT32 tempSize = ::ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);
			if( tempSize > 0)
			{
				BYTE* temp = (BYTE*) alloca(tempSize);

				::ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, tempSize);

				for( start=0; start<tempSize; ++start) 
					if(temp[start]==ATTR_TARGET_CONVERTED || temp[start]==ATTR_TARGET_NOTCONVERTED) break;
				for( end=start; end<tempSize; ++end) 
					if(temp[end]!=temp[start]) break;

				m_ulStart	= start;
				m_ulEnd		= end;
			}

			::ImmReleaseContext(hWnd, hImc);
		}			
	}

Exit:
	//SetCompositionPos( hWnd, 0, 0);

	if (m_dwImeProp & IME_PROP_AT_CARET)
	{
		OnSetCompWindowPos(hWnd);
	}

	return bRet;
}

void i3InputIME::SetIMEOffsetPosition( INT32 x, INT32 y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void i3InputIME::setImeFont( HWND hWnd, HFONT hFont, LOGFONTA * pFont)
{
	m_hFont = hFont;

	HIMC hImc = ::ImmGetContext( hWnd);
	if( hImc != nullptr)
	{
		::ImmSetCompositionFont( hImc, pFont);

		::ImmReleaseContext( hWnd, hImc);
	}
}

void i3InputIME::SetCompositionPos( HWND hWnd, INT32 x, INT32 y)
{
	COMPOSITIONFORM CompForm;
		
	HIMC hImc = ::ImmGetContext(hWnd);
	if( hImc != nullptr)
	{
		CompForm.dwStyle = CFS_POINT;

		CompForm.ptCurrentPos.x = (INT32)m_Pos.x - (INT32)I3IME_DUMMYWND_OFFSET + x;
		CompForm.ptCurrentPos.y = (INT32)m_Pos.y - (INT32)I3IME_DUMMYWND_OFFSET + y;

		//I3TRACE("Set COMPOSITION WINDOW : posx: %d,  posY: %d\n", (INT32)(CompForm.ptCurrentPos.x + I3IME_DUMMYWND_OFFSET), (INT32)(CompForm.ptCurrentPos.y + I3IME_DUMMYWND_OFFSET));

		// 조합 윈도우의 위치를 설정한다.
		if( ::ImmSetCompositionWindow(hImc, &CompForm) == FALSE)
		{
			I3TRACE("failed set composition window position \n");
		}

		::ImmReleaseContext(hWnd, hImc);
	}
}

bool i3InputIME::InsertInputFromCompositionString(HWND hWnd)
{
	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
	if( !IsLimit(FALSE) ||  m_nLimit == 0)
	{
		HIMC hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			INT32 limitSize = 0;
			INT32 checkSize = 0;
			INT32 tempSize = ::ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0);

			wchar_t* temp = (wchar_t*) alloca(tempSize);

			::ImmGetCompositionStringW(hImc, GCS_RESULTSTR, temp, tempSize);
			INT32 length = tempSize / sizeof(wchar_t);
			INT32 inputSize = GetInputLengthW();	//	must without comp		

			//	문자 갯수로 체크
	//		if( IsImeStyle(IME_STYLE_LIMIT_CHARACTER))
	//		{
				checkSize = length;				
				limitSize = GetLimit();
	//		}
			//	바이트 갯수로 체크
	//		else
	//		{
	//			checkSize = ImmGetCompositionStringA(hImc, GCS_RESULTSTR, NULL, 0);
	//			limitSize = GetLimit() / sizeof(wchar_t);
	//		}

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
			if( IsLimitEnough(checkSize))
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
				//m_input.append( m_input.begin() + m_nInsertPos, temp+length);
				for( INT32 i = 0; i < length; i++)
				{
					m_input.insert( m_input.begin() + m_nInsertPos, 1, temp[ i]);	

					m_nInsertPos++;
				}
			}

			::ImmReleaseContext(hWnd, hImc);

			//if( IsLimit() && m_nLimit > 0) 
			_clearCompBuffer();

			m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;
			m_bIsComp = false;

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
					m_nState = EDIT_STATE_RETURN; 

					if( m_pEditProc != nullptr)
						m_pEditProc( this, m_nState, m_pParam);

					return true;
				}
				break;
			default:
				break;
			}

			m_PostKey = 0;	//	처리후 제거

			if( m_pEditProc != nullptr)
				m_pEditProc( this, m_nState, m_pParam);
		}

		return true;
	}

	return false;
}

bool	i3InputIME::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	_clearCompBuffer();

	//SetCompositionPos( hWnd, 0, 0);

	g_imm->SetCompositioning(nullptr);

	m_nState = EDIT_STATE_COMPOSITION_END;
	if( m_pEditProc != nullptr)
	{
		m_pEditProc( this, m_nState, m_pParam);
	}
	
   return true;
}

bool	i3InputIME::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_nState = EDIT_STATE_NONE;
	bool bRet = false;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:		// Set candidate window position
		{
			{
				m_bSelectingCandidate = true;

				if (m_dwImeProp & IME_PROP_AT_CARET)
				{
					OnSetCompWindowPos(hWnd);
				}
			}

			//OnKeyDownCompositionLeft( hWnd);
			MSG_TRACER( "IMN_OPENCANDIDATE", wParam, lParam, hWnd);
			bRet = true;
		}
		break;

	case IMN_SETCANDIDATEPOS:
		{
			MSG_TRACER( "IMN_SETCANDIDATEPOS", wParam, lParam, hWnd);
			
			bRet = true;
		}break;
	case IMN_CHANGECANDIDATE:
		{
			if( IMN_CHANGECANDIDATE == wParam)
			{
				MSG_TRACER( "IMN_CHANGECANDIDATE", wParam, lParam, hWnd);
			}

			//hImc = ImmGetContext(hWnd);
			//if(hImc) 
			//{ 
			//	//memset(nullptr, 0, 10);
			//	memset(&g_canBuf, 0, sizeof(g_canBuf));

			//	m_reading.resize(0);

			//	INT32 candidateLen = ImmGetCandidateListW(hImc, 0, nullptr, 0);

			//	if(candidateLen > 0) 
			//	{ 
			//		m_candidate.resize(candidateLen);

			//		ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)&m_candidate[0], candidateLen);

			//		INT32 candidatecount = GetCandidateCount();
			//		for(INT32 i=0 ; i < 9; ++i) 
			//		{
			//			INT32 canIndex = i+g_canIndex;
			//			if(candidatecount <= canIndex) 
			//			{
			//				break;
			//			}

			//			INT32 len = i3::generic_string_size(g_canBuf);

			//			char conv[MAX_PATH];

			//			GetCandidate( (DWORD)i, conv, MAX_PATH - 1);

			//			i3::snprintf( g_canBuf + len, MAX_PATH - 1 - len, " %d %s", (i+1)%10, conv);
			//		}

			//		I3TRACE( "candidate buf(%s)\n", g_canBuf);
			//	}

			//	ImmReleaseContext(hWnd, hImc);
			//}
			bRet = true;
		}
		break;
	case IMN_CLOSECANDIDATE:
		{
			MSG_TRACER( "IMN_CLOSECANDIDATE", wParam, lParam, hWnd);
			m_bSelectingCandidate = false;
			m_candidate.resize(0);

			bRet = true;
		}
		break;
	case IMN_SETCONVERSIONMODE:		// To detect the change of IME mode 
		{
			MSG_TRACER( "IMN_SETCONVERSIONMODE", wParam, lParam, hWnd);
			//	유저의 키입력으로부터 IME 모드 변경을 불가능 하도록 한다.
			/*if( GetImeStyle() & IME_STYLE_IGNORE_KEYINPUT_IME_MODE)
			{
				SetImeMode(hWnd, m_modeIME);
				return false;
			}*/

			// 한영전환 불가상태인가
			if(GetImeStyle() & IME_STYLE_IGNORE_KEYINPUT_IME_MODE)
			{
				HIMC hImc = ::ImmGetContext(hWnd);
				if(hImc)
				{
					// 영문전용모드임에도 다국어입력상태인가
					BOOL opened = ::ImmGetOpenStatus(hImc);
					::ImmReleaseContext(hWnd, hImc);

					IME_MODE curMode = GetImeMode();

					if(opened) 
					{   
                        // IME입력기는 다국어상태,
                        // i3Ime는 영문상태이므로 IME입력기를 영문으로바꾼다
						if(curMode == IME_MODE_ALPHANUMERIC)
						{	
							SetImeMode(hWnd, IME_MODE_ALPHANUMERIC);
							return false;
						}
					}
					else
					{   
                        // IME입력기는 영문상태
                        // i3Ime는 다국어상태 IME입력기를 다국어상태로
						if(curMode != IME_MODE_ALPHANUMERIC)
						{	
							SetImeMode(hWnd, curMode);
							return false;
						}
					}
				}
			}

			bRet = true;
		}break;
		//	No break;
	case IMN_SETOPENSTATUS:			// To detect the toggling of Japanese IME
		{
			if( wParam == IMN_SETOPENSTATUS)	MSG_TRACER( "IMN_SETOPENSTATUS", wParam, lParam, hWnd);

			{
				CheckToggleState(hWnd);

				m_nState = EDIT_STATE_CHANGE_IME_MODE;

				if( m_pEditProc != nullptr)
					m_pEditProc( this, m_nState, m_pParam);

				bRet = false;
			}
		}
		break;
	case IMN_SETCOMPOSITIONFONT:
		{
			MSG_TRACER( "IMN_SETCOMPOSITIONFONT", wParam, lParam, hWnd);
			bRet = true;
		}
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		{
			MSG_TRACER( "IMN_SETCOMPOSITIONWINDOW", wParam, lParam, hWnd);
/*			
			COMPOSITIONFORM compForm;

			HIMC hImc = ImmGetContext( hWnd);
			if( hImc != nullptr)
			{
				if( ImmGetCompositionWindow( hImc, &compForm))
				{
					I3TRACE("composition window X: %d, Y: %d\n", compForm.ptCurrentPos.x + (INT32)I3IME_DUMMYWND_OFFSET, compForm.ptCurrentPos.y + (INT32)I3IME_DUMMYWND_OFFSET);
				}

				LOGFONTA font;
				if( ImmGetCompositionFont( hImc, &font))
				{
					I3TRACE("composition font: %s height: %d\n", font.lfFaceName, font.lfHeight);
				}

				ImmReleaseContext( hWnd, hImc);
			}
			*/
			bRet = true;
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

		bRet = true;
		/*
		GetPrivateReadingString(hWnd);
		
        // Trap some messages to hide reading window
       switch( m_dwId[0] )
        {
            case IMEID_CHT_VER42:
            case IMEID_CHT_VER43:
            case IMEID_CHT_VER44:
            case IMEID_CHS_VER41:
            case IMEID_CHS_VER42:
				if((lParam==1)||(lParam==2)) return false;
                break;

            case IMEID_CHT_VER50:
            case IMEID_CHT_VER51:
            case IMEID_CHT_VER52:
            case IMEID_CHT_VER60:
            case IMEID_CHS_VER53:
                if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) return false;
                break;
        }
		return true;
		*/
        break;
	case IMN_SETSENTENCEMODE:
		MSG_TRACER( "IMN_SETSENTENCEMODE", wParam, lParam, hWnd);
		bRet = false;
		break;
	case IMN_OPENSTATUSWINDOW:  /* fall-through */
	case IMN_CLOSESTATUSWINDOW:
		bRet = true;
		break;
	default: 
		MSG_TRACER( "DEFAULT notify:", wParam, lParam, hWnd);
		break;
	}

	return bRet;
}

void i3InputIME::OnKeyDownProcess( HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( !IsComp())	return;

	//candidate window가 열린 상태에서 
	/*
	if((lParam & 0xff0000) == 0x4b0000) //왼쪽 방향키
	{
		OnSetCompWindowPos( hWnd);
	}

	if((lParam & 0xff0000) == 0x4d0000) // 오른쪽 방향키
	{ 
		OnSetCompWindowPos( hWnd);
	}*/
}

void	i3InputIME::SetCompositionPos( HWND hWnd)
{
	SIZE		pixel;
	HDC			hdc			= ::GetDC( hWnd);
	HFONT		hOldFont	= nullptr;

	if( m_bCallDefProc == true)
	{
		SetCompositionPos( hWnd, 0, 0);

		::ReleaseDC( hWnd, hdc);
		return;
	}

	if((m_comp.size() == 0) || (m_nCompCursorIndex == 0))
	{
		//I3TRACE("candiindex: (   %d    )  compsize: (    %d     ) x : (   %d   )\n", m_nCompCursorIndex, m_comp.size(), 0);

		SetCompositionPos( hWnd, 0, 0);
	}
	else
	{
		if( m_hFont != nullptr)	 hOldFont = (HFONT)SelectObject( hdc, m_hFont);

		GetTextExtentPoint32W( hdc, m_comp.data(), m_nCompCursorIndex, &pixel);

		//I3TRACE("candiindex: (   %d    )  compsize: (    %d     ) x : (   %d   )\n", m_nCompCursorIndex, m_comp.size(), pixel.cx);

		SetCompositionPos( hWnd, pixel.cx, 0);

		if( hOldFont != nullptr)	::SelectObject( hdc, hOldFont);
	}

	::ReleaseDC( hWnd, hdc);
}

void i3InputIME::OnSetCompWindowPos( HWND hWnd)
{
	HIMC hImc = ImmGetContext( hWnd);
	if( hImc != nullptr)
	{
		m_nCompCursorIndex = 0xffff & ::ImmGetCompositionStringW(hImc, GCS_CURSORPOS, NULL, 0);

		::ImmReleaseContext( hWnd, hImc);
	}

	SetCompositionPos( hWnd);
}

void i3InputIME::OnKeyDownLeft( HWND hWnd)
{
	// Left arrow key
	if( IsComp())
	{
		//	조합중이면 
		m_PostKey = VK_LEFT;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	else
	{
		if( m_nInsertPos > 0)	
		{
			switch( m_codePage)
			{
			case I3_LANG_CODE_THAI:		//	태국어는 한 문자가 n개의 조합형 유니코드로 이루어 졌기 때문에 trail byte를 무시하고 lead byte 단위로 이동해야 합니다.
				{
					I3ASSERT( m_nInsertPos < sizeof(g_szTemp));

					i3::utf16_to_mb(m_input, g_szTemp, m_codePage);

					char * pStart = g_szTemp;
					char * pCurr = &g_szTemp[m_nInsertPos];
					INT32 lenCurr = i3::generic_string_size( pCurr);

					char * pPrev = (char*) i3String::CharPrev( m_codePage, pStart, pCurr);
					INT32 lenPrev= i3::generic_string_size( pPrev);

					m_nInsertPos += (lenCurr - lenPrev);
				}
				break;
			default:
				{
					m_nInsertPos--;	
				}
				break;
			}

			m_nState = EDIT_STATE_CHANGE_INSERT_POSITION;

			if( m_pEditProc != nullptr)
				m_pEditProc( this, m_nState, m_pParam);
		}
	}
}

void i3InputIME::OnKeyDownRight( HWND hWnd)
{	// Right arrow key

	if( IsComp())
	{	
		//	조합중이면 
		m_PostKey = VK_LEFT;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	else
	{
		if( m_nInsertPos < GetInputLengthW())
		{
			switch( m_codePage)
			{
			case I3_LANG_CODE_THAI:		//	태국어는 한 문자가 n개의 조합형 유니코드로 이루어 졌기 때문에 trail byte를 무시하고 lead byte 단위로 이동해야 합니다.
				{
				//	I3ASSERT( m_nInsertPos <  sizeof(wstrTemp) / 2);

					i3::utf16_to_mb(m_input, g_szTemp, m_codePage);

					const char * pStart = &g_szTemp[m_nInsertPos];
					if( *pStart == 0)
						break;

					INT32 lenStart = i3::generic_string_size( pStart);

					char * pNext = (char*) i3String::CharNext( m_codePage, pStart);
					INT32 lenNext = i3::generic_string_size( pNext);

					m_nInsertPos += (lenStart - lenNext);
				}
				break;
			default:
				{
					m_nInsertPos++;
				}
				break;
			}

			m_nState = EDIT_STATE_CHANGE_INSERT_POSITION;

			if( m_pEditProc != nullptr)
				m_pEditProc( this, m_nState, m_pParam);
		}
	}
}

void i3InputIME::OnKeyDownDelete( void)
{	// Delete key

	if( IsComp())	return;		//	조합중에 사용 안하도록

	if( m_nInsertPos < GetInputLengthW())		
	{	//	커서 위치 문자 하나 삭제
		//m_input.erase( m_input.begin() + m_nInsertPos);		

		//	커서 위치 문자 하나 삭제
		switch( m_codePage)
		{
		case I3_LANG_CODE_THAI:		//	태국어는 한 문자가 n개의 조합형 유니코드로 이루어 졌기 때문에 한 문자를 지우려면 가변 크기를 계산해서 여러번 지워야한다.
			{
				I3ASSERT( m_nInsertPos <  sizeof(g_szTemp));

				i3::utf16_to_mb(m_input, g_szTemp, m_codePage);

				char * pStart = &g_szTemp[m_nInsertPos];
				INT32 lenStart = i3::generic_string_size( pStart);

				char * pNext = (char*) i3String::CharNext( m_codePage, pStart);
				INT32 lenNext = i3::generic_string_size( pNext);

				INT32 delCnt =  lenStart - lenNext;

				for(INT32 i=0; i<delCnt; i++)
					m_input.erase( m_input.begin() + m_nInsertPos);
			}
			break;
		default:
			{
				m_input.erase( m_input.begin() + m_nInsertPos);
			}
			break;
		}

		m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

		if( m_pEditProc != nullptr)
			m_pEditProc( this, m_nState, m_pParam);
	}
}

void i3InputIME::OnKeyDownHome( HWND hWnd)
{	// Home key

	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_HOME;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	else
	{
		//	문자열 처음으로
		m_nInsertPos = 0; 
	}
}

void i3InputIME::OnKeyDownEnd( HWND hWnd)
{	// End key

	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_END;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	else
	{
		//	문자열 마지막으로 
		m_nInsertPos = m_input.size();
	}
}

void i3InputIME::OnKeyDownBackspace( void)
{	// Backspace key
	if( IsComp())	return;		//	조합중에 사용 안하도록

	if( m_nInsertPos > 0)
	{
		if( m_nInsertPos < GetInputLengthW())		
		{	//	커서 바로 전 문자 하나 삭제
			m_input.erase(  m_input.begin() + m_nInsertPos - 1);				
		}
		else
		{
			if (!m_input.empty())
				m_input.resize( m_input.size() - 1);
		}

		m_nInsertPos--;	

		m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

		if( m_pEditProc != nullptr)
			m_pEditProc( this, m_nState, m_pParam);
	}
}

void i3InputIME::OnKeyDownSpacebar( HWND hWnd)
{	//	Space Bar key
	if( IsComp())
	{
		m_PostKey = VK_SPACE;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
}

void i3InputIME::OnKeyDownReturn( HWND hWnd)
{	
	if( m_FeedEnterCount <= 0)
	{
		// Return key
		if( IsComp())
		{	//	조합중이면 
			m_PostKey = VK_RETURN;	

			//	조합 종료
			SetInputFinish( hWnd);	
		}
		else
		{
			//	조합 종료
			SetInputFinish( hWnd);	

			m_nState = EDIT_STATE_RETURN;

			if( m_pEditProc != nullptr)
				m_pEditProc( this, m_nState, m_pParam);
		}
	}
	else
	{
		m_FeedEnterCount--;
	}
}

bool	i3InputIME::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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
			if( IsImeStyle(IME_STYLE_NUMERIC_ONLY))
			{
				//	'0' ~ '9' 값만 받는다.
				if(wParam < 48 || wParam > 57)
					break;
			}
			else if( IsImeStyle(IME_STYLE_NUMERIC_AND_NUMERICAL))
			{
				//	'0' ~ '9', '+', '-', '.' 값만 받는다.
				if(wParam != 43 && wParam != 45 && wParam != 46)
				{
					if(wParam < 48 || wParam > 57)
						break;
				}
			}
			else if(IsImeStyle(IME_STYLE_ALPHA_NUMERIC_ONLY))
			{				
				if(isalnum((wint_t)wParam) == 0)
					break;
			}
			else if(IsImeStyle(IME_STYLE_ALPHA_ONLY))
			{
				if(isalpha((wint_t)wParam) == 0)
					break;
			}


			if(wParam > 31) 
			{
				//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
				//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
				if( !IsLimitEnoughChar((char) wParam) || m_nLimit == 0)
				{
					wchar_t temp;
					bool bAddChar = true;

					i3String::MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);
					
					//	태국어는 조합 문자가 WM_IME_STARTCOMPOSITION 메시지로 넘어오지 않으며 WM_CHAR로 개별 문자가 넘어오므로 개별 처리해야 한다.
					if( m_codePage == I3_LANG_CODE_THAI)	//	Check Thai composite character
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
								::WideCharToMultiByte(m_codePage, 0, &m_input[last], 1, &prev, 1, NULL, NULL);

								//	이전 문자가 태국어 문자일때만, 이전 문자가 태국어가 아니면 조합될 필요없다.
								if( prev & 0x80)
								{
									//	현재 넘어온 문자가 태국어에서 조합 가능한 문자인지 검사하여 조합 불가능 상태라면 입력 버퍼에 추가하지 않는다.
									if( !i3String::IsComposite(m_codePage, prev, curr, 2))
									{
										//	혼자 자립할수 있는 태국어 문자라면 (자음이나 모음은 한글자로 입력될수 있으나 성조같은 것은 자립해서 입력하지 못하도록)
										if( !i3String::IsComposite(m_codePage, 0, curr, 2))
										{
											bAddChar = false;
										}
									}
								}
							}				
						}
					}

					if(bAddChar)
					{
						if(IsImeStyle(IME_STYLE_AUTO_UPPERCASE))
						{
							temp = towupper(temp);
						}
						else if(IsImeStyle(IME_STYLE_AUTO_LOWERCASE))
						{
							temp = towlower(temp);
						}

						if( m_nInsertPos < GetInputLengthW())
							//	중간 삽입
							m_input.insert( m_input.begin() + m_nInsertPos, 1, temp);
						else
							m_input.push_back(temp);
						
						m_nInsertPos++;

						m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

						if( m_pEditProc != nullptr)
							m_pEditProc( this, m_nState, m_pParam);
					}
				}
			}
		}
		break;
	}

	return true;
}

INT32 i3InputIME::SetInput( const i3::wliteral_range& wTextRng, bool bAvariableLimitText)
{
	//	길이가 0이상일 경우만
	if(wTextRng.size() == 0)
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

		//	유니코드 문자열의 길이를 받아온다.
	//	INT32 length = i3String::MultiByteToWideChar( m_codePage, 0, text, len, nullptr, 0);
		//	유니코드 크기 만큼 버퍼 생성
	//	wchar_t * wTemp = (wchar_t *) i3MemAlloc( sizeof( wchar_t) * (length+1));
	//	I3ASSERT( wTemp != nullptr);
		//	유니코드 버퍼로 문자열을 받아온다.
	//	i3String::MultiByteToWideChar( m_codePage, 0, text, len, wTemp, length);
		
		INT32 length = wTextRng.size();

		////	문자 갯수로 체크
		//if( IsImeStyle(IME_STYLE_LIMIT_CHARACTER))				
		//{
		//	checkSize = length;
		//	limitSize = GetLimit();
		//	inputSize = GetInputLengthW();	//	must without comp		
		//}
		////	바이트 갯수로 체크
		//else
		{
			checkSize = length;
			limitSize = GetLimit();
			inputSize = GetInputLengthW();	//	must without comp		
		}

		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
		//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
		if( bAvariableLimitText && IsLimitEnough(checkSize))
		{
			INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

			//	바이트 단위일 경우
//			if( !IsImeStyle(IME_STYLE_LIMIT_CHARACTER))
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
			m_input.insert( m_input.begin() + i, 1, wTextRng[ i]);	
		}

		m_nInsertPos = i;
		
//		i3MemFree( wTemp);

		return length;
	}
}

INT32 i3InputIME::AddInput( const i3::wliteral_range& wTextRng, bool bAvariableLimitText)
{
	INT32 limitSize = 0;			
	INT32 inputSize = 0;
	INT32 checkSize = 0;

	//	길이가 0이상일 경우만
	if( i3::generic_string_size( wTextRng) == 0)
		return 0;

	if( bAvariableLimitText)
	{
		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
		if( IsLimit(false) && m_nLimit > 0)
			return 0;
	}	
	
	//	유니코드 문자열의 길이를 받아온다.
	INT32 length = wTextRng.size();
	//i3String::MultiByteToWideChar( m_codePage, 0, text, len, nullptr, 0);

	//	유니코드 크기 만큼 버퍼 생성
	//wchar_t * wTemp = (wchar_t *) i3MemAlloc( sizeof( wchar_t) * (length+1));
	//I3ASSERT( wTemp != nullptr);
	// wTemp[length] = 0;

	//	유니코드 버퍼로 문자열을 받아온다.
	// i3String::MultiByteToWideChar( m_codePage, 0, text, len, wTemp, length);

	//	문자 갯수로 체크
	if( IsImeStyle(IME_STYLE_LIMIT_CHARACTER))
	{
		checkSize = length;
		limitSize = GetLimit();
		inputSize = GetInputLengthW();	//	must without comp		
	}
	//	바이트 갯수로 체크
	else
	{
		checkSize = length;
		limitSize = GetLimit();
		inputSize = GetInputLengthW();	//	must without comp		
	}

	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
	//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
	if( bAvariableLimitText && IsLimitEnough(checkSize))
	{
		INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

		//	바이트 단위일 경우
//		if( !IsImeStyle(IME_STYLE_LIMIT_CHARACTER))
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
		m_input.insert( m_input.begin() + m_nInsertPos, 1, wTextRng[ i]);	

		m_nInsertPos++;
	}

	if( IsLimit() && m_nLimit > 0) 
		_clearCompBuffer();

	m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;
	m_bIsComp = false;

	if( m_pEditProc != nullptr)
		m_pEditProc( this, m_nState, m_pParam);

//	i3MemFree( wTemp);

	return length;
}

void i3InputIME::SetInputFinish( HWND hwnd)
{
	if( IsComp())	//	조합 글자 입력 중지
	{
		HIMC hImc = ImmGetContext( hwnd);		

		//	조합 중지
		PostMessage( hwnd, WM_IME_ENDCOMPOSITION, 0, 0);

		//	조합중 문자 마무리
		ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0); 

		_clearCompBuffer();		

		ImmReleaseContext( hwnd, hImc);
	}	
}

void i3InputIME::SetInputCancel( HWND hwnd)
{
	if( IsComp())	//	조합 글자 입력 중지
	{
		_clearCompBuffer();

		//	조합 중지
		PostMessage( hwnd, WM_IME_ENDCOMPOSITION, 0, 0);

		HIMC hIMC = ImmGetContext( hwnd);

		//	조합중 문자 취소
		ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); 

		ImmReleaseContext( hwnd, hIMC);
	}	
}

bool i3InputIME::IsImeActive( void)
{
	return (m_ImeState == IMEUI_STATE_ON);
}

void i3InputIME::SetImeEnable( HWND hWnd, bool bEnable)
{
	if( bEnable)  
	{
		if( m_hOldIMC != 0)
		{
			ImmAssociateContext(hWnd, m_hOldIMC);			// If you want to enable
															// IME again, then you can
															// use the previous stored IME
															// context(hIMC)
															// to restore IME.
		}

		m_nState = EDIT_STATE_NONE;
	}
	else
	{
		m_hOldIMC = ::ImmAssociateContext(hWnd, NULL);		// It makes IME disable 

		if( m_hOldIMC != 0)
			_clearCompBuffer();
	}
}

bool i3InputIME::SetImeMode( HWND hWnd, IME_MODE mode)
{	
	bool ret = true;
	HIMC hImc = ImmGetContext( hWnd);
	if( hImc == 0)
	{
		I3TRACE("[%s] Warning! Failed to set IME const mode.\n", __FUNCTION__);
		return false;
	}

	// chunjong.song 2014.10.16 - ImmGetConversionStatus대신 ImmGetOpenStatus을 사용해서 합니다.
	// ime상태가 영문입력상태가 아닌데도 불구하고  ImmGetConversionStatus함수가 conversion값을  0으로 돌려주는경우가 있어서 
	if(::ImmGetOpenStatus(hImc)) 
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

	m_modeIME = mode;

	//	강제로 입력 언어를 변경한다.
	switch( mode)
	{
	case IME_MODE_ALPHANUMERIC:	//	영문 모드로 변경
		{
			if( ImmSetConversionStatus( hImc, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE) == 0)
			{
				I3TRACE("[%] Warning! Failed to set IME IME_CMODE_ALPHANUMERIC mode.\n", __FUNCTION__);

				ret = false;

				goto EXIT_IME_MODE;
			}
		}
		break;
	case IME_MODE_NATIVE:		//	자국어 모드로 변경
		{
			if( ImmSetConversionStatus( hImc, IME_CMODE_NATIVE, IME_SMODE_CONVERSATION) == 0)
			{
				I3TRACE("[%s] Warning! Failed to set IME IME_CMODE_NATIVE mode.\n", __FUNCTION__);
				
				ret = false;

				goto EXIT_IME_MODE;
			}
		}
		break;
	}

EXIT_IME_MODE:
	ImmReleaseContext(hWnd, hImc);

	return ret;
}

IME_MODE i3InputIME::GetImeMode(void)
{
	return m_modeIME;
}

DWORD i3InputIME::GetImeModeValue( HWND hWnd)
{
	DWORD ret = 0xFFFFFFFF;
	HIMC hImc = ImmGetContext( hWnd);
	if( hImc)
	{
		DWORD dwConversion, dwSentence;
		if( ImmGetConversionStatus( hImc, &dwConversion, &dwSentence))
		{
			ret = (dwConversion & 0xFFFF);
		}
		else
		{
			I3TRACE("[%s] Failed. Get status.\n", __FUNCTION__);
		}

		ImmReleaseContext(hWnd, hImc);

		return ret;
	}
	
	I3TRACE("[%s] failed Imm context .\n", __FUNCTION__);

	return ret;
}

void i3InputIME::DumpBuffer( void)
{
	char szState[256] = "";
	switch(m_nState)
	{
	case EDIT_STATE_NONE:						i3::safe_string_copy(szState, "NONE", 256);				break;
	case EDIT_STATE_CLEAR:						i3::safe_string_copy(szState, "CLEAR", 256);				break;
	case EDIT_STATE_RETURN:						i3::safe_string_copy(szState, "RETURN", 256);				break;
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	i3::safe_string_copy(szState, "COMPOSITION_CHAR", 256);	break;
	case EDIT_STATE_CHANGE_INPUT_CHAR:			i3::safe_string_copy(szState, "INPUT_CHAR", 256);			break;
	case EDIT_STATE_CHANGE_INSERT_POSITION:		i3::safe_string_copy(szState, "INSERT_POSITION", 256);	break;
	case EDIT_STATE_CHANGE_LANGUAGE:			i3::safe_string_copy(szState, "LANGUAGE", 256);			break;
	case EDIT_STATE_CHANGE_IME_MODE:			i3::safe_string_copy(szState, "IME_MODE", 256);			break;
	}

	I3TRACE( "[%s:(0x%p)] : (%s) Inst(%d/%d) ", __FUNCTION__, this, szState, m_nInsertPos, m_nLimit);


	i3::utf16_to_mb(m_input, g_szTemp, sizeof(g_szTemp) );
	I3TRACE( "Input(%s) ", g_szTemp);

	i3::utf16_to_mb(m_comp, g_szTemp, sizeof(g_szTemp));
	I3TRACE( "Comp(%s) ", g_szTemp);

	i3::wstring wstrTemp;
	GetExactInput( wstrTemp );
	i3::utf16_to_mb(wstrTemp, g_szTemp, sizeof(g_szTemp));
	I3TRACE( "Exact(%s)\n", g_szTemp);
}

void i3InputIME::DumpBufferToCode(void)
{	
	char str[128] = "";
	for( INT32 i = 0; i < (INT32)m_input.size(); i++)
	{	
		INT32 v = (INT32)m_input[i];
		i3::snprintf( str, 128, "[Input buf %d] 0x%x\n", i, v);	
		I3TRACE(str);
	}
}

INT32 i3InputIME::_GetInputSize(UINT32 imeStyle, bool bWithComp)
{
	if( imeStyle & IME_STYLE_LIMIT_CHARACTER)
	{
		switch( m_codePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				if( m_nInsertPos == 0) return 0;

				INT32 len = i3String::WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_input.size(), g_szTemp, 4096, NULL, NULL);
				I3ASSERT( len < 4096);
				g_szTemp[len] = 0;
				
				return i3String::LengthChar( m_codePage, g_szTemp);
			}
			break;
		default:
			return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
			break;
		}
	}
	else	//	default
	{
	//	return GetInputLengthA() + (bWithComp ? GetCompLengthA() : 0);
		return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
	}
}

bool i3InputIME::IsLimit( bool bWithComp /* = true */)		
{ 
	INT32 nInputLength = _GetInputSize( GetImeStyle(), bWithComp);

	return (m_nLimit <= nInputLength); 
}

bool i3InputIME::IsLimitEnough(INT32 nAddSize, bool bWithComp /* = true */)		
{ 
	INT32 nInputLength = _GetInputSize( GetImeStyle(), bWithComp);
	
	return (m_nLimit < nInputLength + nAddSize); 
}

bool i3InputIME::IsLimitEnoughChar(char cAddChar)		
{ 
	INT32 nInputLength = 0;

	if( GetImeStyle() & IME_STYLE_LIMIT_CHARACTER)
	{
		switch( m_codePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				if( m_nInsertPos == 0) return 0;

				INT32 len = i3String::WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_input.size(), g_szTemp, 4096, NULL, NULL);
				I3ASSERT( len < 4095);

				g_szTemp[len] = cAddChar;
				g_szTemp[len+1] = 0;
				
				nInputLength = i3String::LengthChar( m_codePage, g_szTemp);
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
	
	return (m_nLimit < nInputLength); 
}

INT32 i3InputIME::GetExactInput( i3::wstring& out)
{	

	INT32 nCaretPos = GetInsertPositionW(); 
	INT32 ret = 0;

//	if( pOutBuffer == nullptr || nOutLength == 0) return 0; 
	
	ret = GetInput( out );


	//	최종 문자열의 크기가 인자로 들어온 버퍼 크기를 오버하지 않는지 검사

	size_t len_wszComp = m_comp.size(); 
	ret += len_wszComp;

	// I3ASSERT( INT32(i3::generic_string_size( out) + len_wszComp) < nOutLength);

	//	조합문자를 버퍼 중간에 삽입
	out.insert(nCaretPos, m_comp);

	// pOutBuffer[ nOutLength - 1] = 0;	//	널문자

	return ret;
}
 
INT32 i3InputIME::GetInput( i3::wstring& out )
{
	out = m_input;
	return out.size();
}

INT32 i3InputIME::GetComp(i3::wstring& out)
{
	out = m_comp;
	return out.size();
}

INT32 i3InputIME::GetCompToIndex(i3::wstring& out)
{
	if( m_nCompCursorIndex == 0)
		return 0;
	else
	{
		out.assign(m_comp.begin(), m_comp.begin() + m_nCompCursorIndex);
		return out.size();
	}
}

INT32 i3InputIME::GetReading(i3::wstring& out)
{
	out = m_reading;
	return out.size();
}

/*
INT32 i3InputIME::GetCandidate(DWORD index, char* text, INT32 len) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) {
			return 0;
		} else {

			if(m_bUnicodeIME) {
				
				wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

				return ConvertString(m_codePage, wText, wcslen(wText), text, len);

			} else {

				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) {
					return strlen(temp);
				} else {
					INT32 tempLen = strlen(temp);

					if(len < tempLen) {
						return 0;
					} else {
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}
*/

INT32		i3InputIME::GetCandidateCount( void) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
	}
}

INT32		i3InputIME::GetCandidateSelection( void) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		if(PRIMARYLANGID(m_langId) == LANG_KOREAN)
			return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
		else
			return ((CANDIDATELIST*)&m_candidate[0])->dwSelection;
	}
}

INT32		i3InputIME::GetCandidatePageSize( void) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageSize;
	}
}

INT32		i3InputIME::GetCandidatePageStart( void) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageStart;
	}
}

void	i3InputIME::GetUnderLine(INT32* start, INT32* end) 
{ 	
	*start = m_ulStart;
	*end   = m_ulEnd;

//	*start = i3String::WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulStart, NULL, 0, NULL, NULL); 
//	*end = i3String::WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulEnd, NULL, 0, NULL, NULL); 
}

void	i3InputIME::GetImeId( void)
{
    char    szTmp[1024];

	m_dwId[0] = m_dwId[1] = 0;

    if(!((m_hkl==_CHT_HKL) || (m_hkl==_CHT_HKL2) || (m_hkl==_CHS_HKL)))
		return;
        
	if ( !ImmGetIMEFileNameA( m_hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
        return;

    if ( !_GetReadingString ) {

        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) {

	        return;
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSize( szTmp, &dwVerHandle );

    if( dwVerSize ) {

        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) {

			LPVOID  lpVerData;
			UINT32    cbVerData;

            if( VerQueryValue( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) {

                DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                if( _GetReadingString
                    ||
                    ( m_langId == LANG_CHT &&
                        ( dwVer == MAKEIMEVERSION(4, 2) || 
                        dwVer == MAKEIMEVERSION(4, 3) || 
                        dwVer == MAKEIMEVERSION(4, 4) || 
                        dwVer == MAKEIMEVERSION(5, 0) ||
                        dwVer == MAKEIMEVERSION(5, 1) ||
                        dwVer == MAKEIMEVERSION(5, 2) ||
                        dwVer == MAKEIMEVERSION(6, 0) ) )
                    ||
                    ( m_langId == LANG_CHS &&
                        ( dwVer == MAKEIMEVERSION(4, 1) ||
                        dwVer == MAKEIMEVERSION(4, 2) ||
                        dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

                    m_dwId[0] = dwVer | m_langId;
                    m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}


void	i3InputIME::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	_GetReadingString = nullptr;
    _ShowReadingWindow = nullptr;

	if ( ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof( szImeFile ) - 1 ) > 0 ) 
	{
		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) 
		{
			_GetReadingString = (UINT32 (WINAPI*)(HIMC, UINT32, LPWSTR, PINT, bool*, PUINT32))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(bool (WINAPI*)(HIMC, bool))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) 
			{
				HIMC hImc = ImmGetContext(hWnd);
				if(hImc)
				{
					_ShowReadingWindow( hImc, false );
					ImmReleaseContext(hWnd, hImc);
				}
			}
		}
	}
}

bool	i3InputIME::GetReadingWindowOrientation( void)
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( m_dwId[0] == 0 );
    if( !bHorizontalReading && ( m_dwId[0] & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = m_dwId[0] & 0xFFFF0000;
        strncpy( szRegPath, "software\\microsoft\\windows\\currentversion\\", sizeof( szRegPath)-1 );
        strncat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT", sizeof( szRegPath)-1 );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = ::RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

void	i3InputIME::GetPrivateReadingString(HWND hWnd)
{
    if( !m_dwId[0] ) {
        m_reading.resize(0);
        return;
    }

	HIMC hImc = ImmGetContext(hWnd);
    if( !hImc ) {
        m_reading.resize(0);
        return;
    }

    
    DWORD dwErr = 0;

    if( _GetReadingString ) {

        UINT32 uMaxUiLen;
        bool bVertical;
        // Obtain the reading string size
        INT32 wstrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );

		if( wstrLen == 0 ) {
			m_reading.resize(0);
		} else {
			wchar_t *wstr = (wchar_t*)alloca(sizeof(wchar_t) * wstrLen);
            _GetReadingString( hImc, wstrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
			m_reading.assign(wstr, wstr+wstrLen);
		}

		m_bVerticalReading = bVertical ? true : false;

		ImmReleaseContext(hWnd, hImc);

    } else {

        // IMEs that doesn't implement Reading String API
		wchar_t* temp = 0;
		DWORD tempLen = 0;
	    bool bUnicodeIme = false;
		INPUTCONTEXT *lpIC = g_imm->LockIMC(hImc);

		if(lpIC == nullptr)
		{
			temp = nullptr;
			tempLen = 0;
		}
		else 
		{
			LPBYTE p = 0;
			switch( m_dwId[0] )
			{
				case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
				case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
				case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
					p = *(LPBYTE *)((LPBYTE)g_imm->LockIMCC( lpIC->hPrivate ) + 24 );
					if( !p ) break;
					tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
					dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
					temp = (wchar_t *)( p + 56 );
					bUnicodeIme = true;
					break;

				case IMEID_CHT_VER50: // 5.0.x.x // WinME
					p = *(LPBYTE *)( (LPBYTE)g_imm->LockIMCC( lpIC->hPrivate ) + 3 * 4 );
					if( !p ) break;
					p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
					temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = false;
					break;

				case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
				case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
				case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
					p = *(LPBYTE *)((LPBYTE)g_imm->LockIMCC( lpIC->hPrivate ) + 4);
					if( !p ) break;
					p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
					temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = true;
					break;

				// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
				case IMEID_CHS_VER41:
					{
						INT32 nOffset;
						nOffset = ( m_dwId[1] >= 0x00000002 ) ? 8 : 7;

						p = *(LPBYTE *)((LPBYTE)g_imm->LockIMCC( lpIC->hPrivate ) + nOffset * 4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
						dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
						dwErr = min( dwErr, tempLen );
						temp = (wchar_t *)(p + 6*4 + 16*2*1);
						bUnicodeIme = true;
					}
					break;

				case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
					{
						OSVERSIONINFOA osi;
						osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
						GetVersionExA( &osi );

						INT32 nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
						p = *(LPBYTE *)((LPBYTE)g_imm->LockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
					}
					break;

				default:
					temp = nullptr;
					tempLen = 0;
					break;
			}
		}

		if(tempLen == 0) {
			m_reading.resize(0);
		} else {
			if( bUnicodeIme ) {
				m_reading.assign(temp, tempLen);
			} else {
				INT32 wstrLen = i3String::MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0);
				wchar_t* wstr = (wchar_t*)alloca(sizeof(wchar_t)*wstrLen);
				i3String::MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, wstr, wstrLen); 
				m_reading.assign(wstr, wstrLen);
			}
		}

		if( lpIC != nullptr)
			g_imm->UnlockIMCC( lpIC->hPrivate);

		g_imm->UnlockIMC(hImc);

		m_bVerticalReading = !GetReadingWindowOrientation();
    }

	ImmReleaseContext(hWnd, hImc);
}

void	i3InputIME::CheckToggleState(HWND hWnd)
{
	// Update Indicator
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

	    m_bVerticalCandidate = true;
		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_bVerticalCandidate = m_dwId[0] == 0;
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

	case LANG_THAI:
		m_bVerticalCandidate = false;		
		m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
        break;

    default:
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == g_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }
		
	// Check Toggle State 
	bool bIme = ImmIsIME( m_hkl ) != 0;

	HIMC hImc = ImmGetContext(hWnd);
    if( hImc ) 
	{
		DWORD dwConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

		if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) 
		{
			m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
		} 
		else if( ( PRIMARYLANGID(m_langId) == LANG_THAI ) )  
		{
			m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;	
		}
		else 
		{
			m_ImeState = ( bIme && ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}

		//	check mode
		DWORD dwLang = dwConvMode & IME_CMODE_LANGUAGE;
		switch(dwLang)
		{
		case IME_CMODE_ALPHANUMERIC:	m_modeIME = IME_MODE_ALPHANUMERIC;	break;
		case IME_CMODE_NATIVE:			m_modeIME = IME_MODE_NATIVE;		break;
		default:						m_modeIME = IME_MODE_UNKNOWN;		break;
		}

		ImmReleaseContext(hWnd, hImc);
	}
	else 
	{
        m_ImeState = IMEUI_STATE_OFF;
	}
}

LRESULT i3InputIME::MessageProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultProc)
{
	switch( message)
	{
	case WM_CREATE:
		{
			OnInputLanguageChange( hWnd, 0, (LPARAM) GetKeyboardLayout(0));

			return 0;
		}
		break;
		
	// IME Messages
	case WM_INPUTLANGCHANGE:
		MSG_TRACER( "WM_INPUTLANGCHANGE", wParam, lParam, hWnd);
		{
			OnInputLanguageChange( hWnd, wParam, lParam);		
		}
		break;

	// Except IME mode control in Janap Win9x, dose not need to send this message
	case WM_IME_SETCONTEXT:
		{
			MSG_TRACER( "WM_IME_SETCONTEXT", wParam, lParam, hWnd);

			if( wParam != 0)
			{
				OnInputLanguageChange( hWnd, 0, (LPARAM) GetKeyboardLayout(0));

				if (m_dwImeProp & IME_PROP_SPECIAL_UI)
				{
					// EMPTY
				}
				else if (m_dwImeProp & IME_PROP_AT_CARET)
				{
					if( m_bCallDefProc == false)
						lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
				}
				else
				{
					// EMPTY
				}
			}
		}
		break;

	// Start composition
	case WM_IME_STARTCOMPOSITION:
		MSG_TRACER( "WM_IME_STARTCOMPOSITION", wParam, lParam, hWnd);
		OnStartComposition(hWnd, wParam,lParam);

		if( m_bCallDefProc)
		{
			return DefWindowProc( hWnd, message, wParam, lParam);
		}

		return 0;
		break;
	// Composotion char
	case WM_IME_COMPOSITION:
		MSG_TRACER( "WM_IME_COMPOSITION", wParam, lParam, hWnd);
		if( OnComposition( hWnd, wParam, lParam))
		{
			if( m_bCallDefProc)
			{
				return DefWindowProc( hWnd, message, wParam, lParam);
			}
		}

		return 0;
		break;

	// End conposition
	case WM_IME_ENDCOMPOSITION:
		MSG_TRACER( "WM_IME_ENDCOMPOSITION", wParam, lParam, hWnd);

		OnEndComposition( hWnd, wParam, lParam);

		if( m_bCallDefProc)
		{
			return DefWindowProc( hWnd, message, wParam, lParam);
		}

		return 0;
		break;
	
	// IME Notofy
	case WM_IME_NOTIFY:
		MSG_TRACER( "WM_IME_NOTIFY", wParam, lParam, hWnd);
		if( OnNotify( hWnd, wParam, lParam) == false)
			return 0;
		break;
	//	Process Character
	case WM_CHAR:
		{	
			MSG_TRACER( "WM_CHAR", wParam, lParam, hWnd);
			//	문자 입력을 받는다.
			OnChar( hWnd, wParam, lParam);

			return 0;
		}
		break;

	//	Process Keyboad
	case WM_KEYDOWN:
		{
			MSG_TRACER( "WM_KEYDOWN", wParam, lParam, hWnd);
			switch( wParam)
			{
				case VK_LEFT :		OnKeyDownLeft( hWnd);	break;
				case VK_RIGHT :		OnKeyDownRight( hWnd);		break;
				case VK_DELETE :	OnKeyDownDelete();			break;
				case VK_HOME :		OnKeyDownHome( hWnd);		break;
				case VK_END :		OnKeyDownEnd( hWnd);		break;
				case VK_BACK :		OnKeyDownBackspace();		break;
				case VK_SPACE :		OnKeyDownSpacebar( hWnd);	break;
				case VK_RETURN :	OnKeyDownReturn( hWnd);		break;
				case VK_PROCESSKEY:	OnKeyDownProcess( hWnd,wParam, lParam);break;
				//case VK_ESCAPE:		OnKeyDownProcess( hWnd,wParam, lParam);break;
				default: 			m_FeedEnterCount = 0;		break;	// 피드카운트를 초기화합니다.
			}
		}

		return 0;
		break;
	case WM_KEYUP:
		{
			MSG_TRACER( "WM_KEYUP", wParam, lParam, hWnd);
		}
		break;

	case WM_SETFOCUS:		
		{
			MSG_TRACER( "WM_SETFOCUS", wParam, lParam, hWnd);
			SetImeMode(hWnd, m_modeIME);
			return 0;
		}
		break;
	//	Kill focus
	case WM_KILLFOCUS:
		{
			MSG_TRACER( "WM_KILLFOCUS", wParam, lParam, hWnd);
			SetInputFinish( hWnd);		//	조합 중지	
			return 0;
		}
		break;

	default:
		{
			MSG_TRACER( "  param", wParam, lParam, hWnd);
		}
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam);
}

/*****************************************************************************\
* Function: GetStringWidth
*
* Calculate string width in pixel 
*
* Arguments:
*	LPCTSTR szStr - string 
\*****************************************************************************/
INT32 i3InputIME::GetStringWidth( HDC hdc, HFONT hFont, const wchar_t * szStr, INT32 size)
{
	SIZE		pixel;

	HFONT		pOld = (HFONT) SelectObject( hdc, hFont);

	if( size <= 0)	size = wcslen( szStr);

	GetTextExtentPoint32W( hdc, szStr, size, &pixel);

	SelectObject( hdc, pOld);

	return pixel.cx;
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
INT32 i3InputIME::GetCombinedCharLength( INT32 nTmpPos)
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

/*
INT32 i3InputIME::GetInputLengthA( void)
{
	return i3String::WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_input.size(), nullptr, 0, nullptr, nullptr);
}

INT32 i3InputIME::GetCompLengthA( void)
{
	return i3String::WideCharToMultiByte( m_codePage, 0, m_comp.c_str(), m_comp.size(), nullptr, 0, nullptr, nullptr);
}

INT32 i3InputIME::GetInsertPositionA( void)		
{ 
	return i3String::WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_nInsertPos, nullptr, 0, nullptr, nullptr);
} 
*/

INT32 i3InputIME::GetCursorPosition(void)
{
	switch( m_codePage)
	{
	case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
		{
			if( m_nInsertPos == 0) return 0;

			INT32 len = ::WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_nInsertPos, g_szTemp, sizeof(g_szTemp), NULL, NULL);
			I3ASSERT( len < sizeof(g_szTemp));
			g_szTemp[len] = 0;
			
			return i3String::LengthChar( m_codePage, g_szTemp);
		}
		break;
	default:
		return GetInsertPositionW();
		break;
	}
}

char * i3InputIME::GetCurrentLanguage( void)
{
	char * pszLang = "UNKNOWN";

	LANGID	langid = GetLangId();

	switch( PRIMARYLANGID( langid))
	{
	case LANG_JAPANESE:			pszLang = "JAPANESE";		break;
	case LANG_KOREAN:			pszLang = "KOREAN";			break;
	case LANG_CHINESE:		
		switch( SUBLANGID( langid))
		{
		case SUBLANG_CHINESE_SIMPLIFIED:	pszLang = "CHINESE_SIMPLIFIED";		break;
		case SUBLANG_CHINESE_TRADITIONAL:	pszLang = "CHINESE_TRADITIONAL";	break;
		default:							pszLang = "ANSI";					break;
		}
		break;
	case LANG_GREEK:			pszLang = "GREEK";			break;
	case LANG_TURKISH:			pszLang = "TURKISH";		break;
	case LANG_HEBREW:			pszLang = "HEBREW";			break;
	case LANG_ARABIC:			pszLang = "ARABIC";			break;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:		pszLang = "BALTIC";			break;
	case LANG_THAI:				pszLang = "THAI";			break;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:		pszLang = "EASTEUROPE";		break;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:		pszLang = "RUSSIAN";		break;
	case LANG_VIETNAMESE:		pszLang = "VIETNAMESE";		break;
	case LANG_PORTUGUESE:	
		switch( SUBLANGID( langid))
		{
		case SUBLANG_PORTUGUESE:			pszLang = "PORTUGUESE";	break;
		case SUBLANG_PORTUGUESE_BRAZILIAN:	pszLang = "PORTUGUESE (BRAZIL)";	break;
		default:							pszLang = "ANSI";		break;
		}
		break;
	default:					pszLang = "ANSI";			break;
	}

	return pszLang;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
