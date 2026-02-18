#include "i3InputType.h"
#include "i3InputMethodEditor.h"
#include "i3InputMethodContext.h"

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

wchar_t		g_szIndicator[5][3] =  
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};

//	Input Context management
static i3InputMethodContext * g_IMEContext = NULL;

#define _LockIMC(hImc)		g_IMEContext->LockIMC( hImc)
#define _UnlockIMC(hImc)	g_IMEContext->UnlockIMC( hImc)
#define _LockIMCC(hImcc)	g_IMEContext->LockIMCC( hImcc)
#define _UnlockIMCC(hImcc)	g_IMEContext->UnlockIMCC( hImcc)

static WCHAR strTemp[4096] = L"";		//	내부 함수에서만 사용하는 작업용 버퍼입니다. 외부로 노출되면 안됩니다.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
I3_CLASS_INSTANCE( i3InputMethodEditor, i3ElementBase);

i3InputMethodEditor::i3InputMethodEditor( void)
{
	m_ulStart = 0;
	m_ulEnd = 0;
	m_nInsertPos = 0;
	m_hDllIme = NULL;
	m_ImeState = IMEUI_STATE_OFF;

	m_nTempSize = 0;

	m_pEditProc = NULL; 
	m_nState = EDIT_STATE_NONE;
	m_nLimit = 0;
	m_pParam = NULL;
	m_bIsComp = FALSE;
	m_PostKey = 0;
	m_modeIME = IME_MODE_UNKNOWN;
	m_nStyle = 0;

	m_langId = 0xff;
	m_codePage = 0xff;
	m_hkl = 0;
	m_hOldIMC = 0;
	m_bUnicodeIME = false;
	m_bVerticalReading = false;
	m_bVerticalCandidate = false;
	m_wszCurrIndicator = NULL;
	m_dwImeID[0] = m_dwImeID[1] = 0;
	m_FeedEnterCount = 0;

	m_nBlkStart = -1;
	m_nBlkEnd = -1;
	m_bShiftPressed = false;
	m_bCtrlPressed = false;
	m_bBlock = false;
}

i3InputMethodEditor::~i3InputMethodEditor( void)
{
	if( m_hDllIme != NULL) 
		FreeLibrary( m_hDllIme );
	
	if( g_IMEContext != NULL)
	{
		BOOL bReleaseContext = FALSE;

		if( g_IMEContext->GetRefCount() == 1)
			bReleaseContext = TRUE;

		g_IMEContext->Release();	

		if( bReleaseContext)
			g_IMEContext = NULL;
	}
}

void	i3InputMethodEditor::Create( INT32 nLimit, UINT32 nStyle)
{
	if( g_IMEContext == NULL)
	{
		g_IMEContext = i3InputMethodContext::NewObjectRef();
		I3ASSERT( g_IMEContext != NULL);

		g_IMEContext->Create();
	}

	g_IMEContext->AddRef();	

	m_nLimit = nLimit;
	m_nStyle = nStyle;
}

void i3InputMethodEditor::SetEditProc( I3IME_EDIT_PROC pFunc, void * pParam)
{
	m_pEditProc = pFunc; 
	m_pParam = pParam;
}

void i3InputMethodEditor::Reset( void)
{	
	ClearBuffer();
}

BOOL i3InputMethodEditor::IsNT( void)
{
	I3ASSERT( g_IMEContext != NULL);

	return g_IMEContext->IsNT();
}

void i3InputMethodEditor::_clearBuffer( void)
{
	m_input.resize(0);
	m_nInsertPos = 0;

	_clearCompBuffer();
}

void i3InputMethodEditor::_clearCompBuffer(void)
{
	//	조합 버퍼 클리어
	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);
	m_bIsComp = FALSE;
}

void i3InputMethodEditor::ClearBuffer( void)
{
	_clearBuffer();

	m_nState = EDIT_STATE_CLEAR;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void	i3InputMethodEditor::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	// Change the setting if input language is changed
	m_hkl = (HKL)lParam;
	m_langId = LOWORD(m_hkl);

	if( PRIMARYLANGID(m_langId) == LANG_ENGLISH)		//	ENGLISH(US) skip (use ANSI)
		m_codePage = 0;
	else
		m_codePage = (WORD) i3Language::GetCodePageFromLang(m_langId);
	
	// Check Property 
	DWORD dwProperty = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicodeIME = (dwProperty & IME_PROP_UNICODE) ? true : false;

	// Update Indicator
	CheckToggleState(hWnd);

	// Update m_dwImeID[]
	GetImeId();

	// Bind Proc
	SetupImeApi(hWnd);

	m_nState = EDIT_STATE_CHANGE_LANGUAGE;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}
 
bool	i3InputMethodEditor::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	// Composotion char
	HIMC hImc;

	m_nState = EDIT_STATE_NONE;

	//	Retrieves or updates the current composition string. 
	if(lParam & GCS_COMPSTR) 
	{
		g_IMEContext->SetCompositioning(this);

		hImc = ImmGetContext(hWnd);
		if(hImc != NULL)
		{
			//I3TRACE( "OnComposition, GCS_COMPSTR\n"); 
			if( m_bBlock)
			{
				DeleteNChars( m_nBlkStart, m_nBlkEnd);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
				m_bBlock = false;
			}


			INT32 addSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);
			
			addSize = addSize / sizeof(wchar_t);

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
			if( !IsLimitEnough(addSize, FALSE) ||  m_nLimit == 0)
			{
				INT32 tempSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);

				wchar_t* temp = (wchar_t*) alloca(tempSize);

				ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);

				m_comp.assign(temp, temp+tempSize / sizeof(wchar_t));

				ImmReleaseContext(hWnd, hImc);

				m_nState = EDIT_STATE_CHANGE_COMPOSITION_CHAR;
				m_bIsComp = TRUE;

				if( m_pEditProc != NULL)
					m_pEditProc( this, m_nState, m_pParam);
			}
			else
			{
				//	제한 갯수를 넘으면 조합 문자를 제거합니다.
				ImmSetCompositionStringW(hImc, SCS_SETSTR, "\0", 1, NULL, NULL);
				ImmReleaseContext(hWnd, hImc);

				_clearCompBuffer();
			}
		}
	}	

	//	Retrieves or updates the attribute of the composition string.
	if(lParam & GCS_COMPATTR) 
	{
		g_IMEContext->SetCompositioning(this);
			
		hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			INT32 start, end;
			INT32 tempSize = ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);

			wchar_t* temp = (wchar_t*) alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, tempSize);

			for( start=0; start<tempSize; ++start) if(temp[start]==ATTR_TARGET_CONVERTED || temp[start]==ATTR_TARGET_NOTCONVERTED) break;
			for( end=start; end<tempSize; ++end) if(temp[end]!=temp[start]) break;

			m_ulStart	= start;
			m_ulEnd		= end;

			ImmReleaseContext(hWnd, hImc);
		}			
	}

	//	Retrieves or updates the string of the composition result. 
	if(lParam & GCS_RESULTSTR) 
	{
		i3InputMethodEditor * pCompositioningIME = g_IMEContext->GetCompositioning();

		if( pCompositioningIME != NULL)
		{
			pCompositioningIME->InsertInputFromCompositionString(hWnd);

			g_IMEContext->SetCompositioning(NULL);
		}
	}

	return true;
}

BOOL i3InputMethodEditor::InsertInputFromCompositionString(HWND hWnd)
{
	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
	if( !IsLimit(FALSE) ||  m_nLimit == 0)
	{
		HIMC hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			INT32 limitSize = 0;
			INT32 checkSize = 0;
			INT32 tempSize = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0);

			wchar_t* temp = (wchar_t*) alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, temp, tempSize);
			INT32 length = tempSize / sizeof(wchar_t);
			INT32 inputSize = GetInputLengthW();	//	must without comp		

			checkSize = length;				
			limitSize = GetLimit();

			//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
			//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
			if( IsLimitEnough(checkSize))
			{
				INT32 overheadSize = (inputSize + length) - limitSize;	//	유니코드를 기준으로 

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

			ImmReleaseContext(hWnd, hImc);

			if( IsLimit() && m_nLimit > 0) 
				_clearCompBuffer();

			m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;
			m_bIsComp = FALSE;

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

					if( m_pEditProc != NULL)
						m_pEditProc( this, m_nState, m_pParam);

					return true;
				}
				break;
			default:
				break;
			}

			m_PostKey = 0;	//	처리후 제거

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);
		}

		return TRUE;
	}
		
	return FALSE;
}

bool	i3InputMethodEditor::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	// End conposition
	_clearCompBuffer();

	return true;
}

bool	i3InputMethodEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc = 0;
	m_nState = EDIT_STATE_NONE;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:			// Set candidate window position
	case IMN_CHANGECANDIDATE:	
		hImc = ImmGetContext(hWnd);
		if(hImc) 
		{ 
			//memset(NULL, 0, 10);

			m_reading.resize(0);

			INT32 candidateLen = ImmGetCandidateListW(hImc, 0, NULL, 0);

			if(candidateLen > 0) 
			{ 
				m_candidate.resize(candidateLen);

				ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)&m_candidate[0], candidateLen);
			}

			ImmReleaseContext(hWnd, hImc);
		}
		return true;

	case IMN_CLOSECANDIDATE:
		m_candidate.resize(0);
		return true;

	case IMN_SETCONVERSIONMODE:		// To detect the change of IME mode 
		{
		}
		//	No break;
	case IMN_SETOPENSTATUS:			// To detect the toggling of Japanese IME
		{
			//	유저의 키입력으로부터 IME 모드 변경을 불가능 하도록 한다.
			if( GetStyle() & IME_STYLE_IGNORE_KEYINPUT_IME_MODE)
			{
				SetImeMode(hWnd, m_modeIME);
			}
			else
			{
				 CheckToggleState(hWnd);

				//I3TRACE( "IME Active (%d)\n", IsImeActive());

				m_nState = EDIT_STATE_CHANGE_IME_MODE;

				if( m_pEditProc != NULL)
					m_pEditProc( this, m_nState, m_pParam);
			}

			return false;
		}
    case IMN_PRIVATE:
        GetPrivateReadingString(hWnd);

        // Trap some messages to hide reading window
        switch( m_dwImeID[0] )
        {
            case IMEID_CHT_VER42:
            case IMEID_CHT_VER43:
            case IMEID_CHT_VER44:
            case IMEID_CHS_VER41:
            case IMEID_CHS_VER42:
				if((lParam==1)||(lParam==2)) return true;
                break;

            case IMEID_CHT_VER50:
            case IMEID_CHT_VER51:
            case IMEID_CHT_VER52:
            case IMEID_CHT_VER60:
            case IMEID_CHS_VER53:
                if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) return true;
                break;
        }
        break;
	}

	return false;
}

void i3InputMethodEditor::OnKeyDownLeft( HWND hWnd)
{	// Left arrow key

	if( IsComp())
	{	//	조합중이면 
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
					I3ASSERT( m_nInsertPos < sizeof(strTemp));

					GetInput(strTemp, sizeof(strTemp));
					WCHAR * pStart = strTemp;
					WCHAR * pCurr = &strTemp[m_nInsertPos];
					INT32 lenCurr = i3String::LengthW( pCurr);

					WCHAR * pPrev = (WCHAR*)CharPrevW(pStart, pCurr);
					INT32 lenPrev= i3String::LengthW( pPrev);

					m_nInsertPos += (lenCurr - lenPrev);
				}
				break;
			default:
				{
					m_nInsertPos--;	
				}
				break;
			}

			if( m_bShiftPressed)
			{
				m_nBlkEnd = m_nInsertPos;
			}
			else
			{
				m_bBlock = false;
			}

			m_nState = EDIT_STATE_ONKEYDOWN_LEFT;

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);
			
		}
		else if( m_nInsertPos == 0 && !m_bShiftPressed)
		{
			m_bBlock = false;

			m_nState = EDIT_STATE_ONKEYDOWN_LEFT;

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);
		}

	}
}


void i3InputMethodEditor::OnKeyDownRight( HWND hWnd)
{	// Right arrow key

	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_RIGHT;
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
					I3ASSERT( m_nInsertPos <  sizeof(strTemp));

					GetInput(strTemp,  sizeof(strTemp));
					WCHAR * pStart = &strTemp[m_nInsertPos];
					if( *pStart == 0)
						break;

					INT32 lenStart = i3String::LengthW( pStart);

					WCHAR * pNext = (WCHAR*) CharNextW( pStart);
					INT32 lenNext = i3String::LengthW( pNext);

					m_nInsertPos += (lenStart - lenNext);
				}
				break;
			default:
				{
					m_nInsertPos++;
				}
				break;
			}

			if( m_bShiftPressed)
			{
				m_nBlkEnd = m_nInsertPos;
			}
			else
			{
				m_bBlock = false;
			}

			m_nState = EDIT_STATE_ONKEYDOWN_RIGHT;

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);

		}
		else if( m_nInsertPos == GetInputLengthW() && !m_bShiftPressed )
		{
			m_bBlock = false;

			m_nState = EDIT_STATE_ONKEYDOWN_RIGHT;

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);
		}
	}
}

void i3InputMethodEditor::SetInsertPositionW( INT32 pos)
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

void i3InputMethodEditor::OnKeyDownDown( HWND hWnd)
{
	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_DOWN;
		//	조합 종료
		SetInputFinish( hWnd);		
	}

	m_nState = EDIT_STATE_ONKEYDOWN_DOWN;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownUp( HWND hWnd)
{
	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_UP;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	
	m_nState = EDIT_STATE_ONKEYDOWN_UP;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownPageup( HWND hWnd)
{
	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_PRIOR;
		//	조합 종료
		SetInputFinish( hWnd);		
	}

	m_nState = EDIT_STATE_ONKEYDOWN_PAGEUP;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownPagedown( HWND hWnd)
{
	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_NEXT;
		//	조합 종료
		SetInputFinish( hWnd);		
	}

	m_nState = EDIT_STATE_ONKEYDOWN_PAGEDOWN;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}


void i3InputMethodEditor::OnKeyDownDelete( void)
{	// Delete key

	if( IsComp())	return;		//	조합중에 사용 안하도록

	switch( m_codePage)
	{
	case I3_LANG_CODE_THAI:		//	태국어는 한 문자가 n개의 조합형 유니코드로 이루어 졌기 때문에 한 문자를 지우려면 가변 크기를 계산해서 여러번 지워야한다.
		{
			if( m_nInsertPos < GetInputLengthW())
			{
				I3ASSERT( m_nInsertPos <  sizeof(strTemp));

				GetInput(strTemp,  sizeof(strTemp));
				WCHAR * pStart = &strTemp[m_nInsertPos];
				INT32 lenStart = i3String::LengthW( pStart);

				WCHAR * pNext = (WCHAR*) ::CharNextW( pStart);
				INT32 lenNext = i3String::LengthW( pNext);

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

	m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
	
}

void i3InputMethodEditor::OnKeyDownHome( HWND hWnd)
{	// Home key

	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_HOME;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	
	m_nState = EDIT_STATE_ONKEYDOWN_HOME;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownEnd( HWND hWnd)
{	// End key

	if( IsComp())
	{	//	조합중이면 
		m_PostKey = VK_END;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
	
	m_nState = EDIT_STATE_ONKEYDOWN_END;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownBackspace( void)
{	
	if( IsComp())	return;		//	조합중에 사용 안하도록

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

	m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

void i3InputMethodEditor::OnKeyDownSpacebar( HWND hWnd)
{	//	Space Bar key
	if( IsComp())
	{
		m_PostKey = VK_SPACE;
		//	조합 종료
		SetInputFinish( hWnd);		
	}
}

void i3InputMethodEditor::OnKeyDownReturn( HWND hWnd)
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

			if( m_pEditProc != NULL)
				m_pEditProc( this, m_nState, m_pParam);
		}
	}
	else
	{
		m_FeedEnterCount--;
	}
}

void	i3InputMethodEditor::OnKeyDownShift( HWND hWnd)
{
	m_bShiftPressed = true;
}

void	i3InputMethodEditor::OnKeyDownControl( HWND hWnd)
{
	m_bCtrlPressed = true;
}

void	i3InputMethodEditor::OnKeyDownEscape( HWND hWnd)
{
	m_bBlock = false;

	m_nState = EDIT_STATE_NONE;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);
}

// ******** OnKeyUp ********
void	i3InputMethodEditor::OnKeyUpShift( HWND hWnd)
{
	m_bShiftPressed = false;
}

void	i3InputMethodEditor::OnKeyUpControl( HWND hWnd)
{
	m_bCtrlPressed = false;
}

bool	i3InputMethodEditor::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam) 
	{
	case '\b':	// Back space key		
	case 27:	// Escape
	case '\r':	// Return key (i3IMEHandler::StateProcess 에서 처리)
	case '\n':	
		break;
	default:	// \t \r(return key) \n
		{
			if( IsStyle(IME_STYLE_NUMERIC_ONLY))
			{
				//	'0' ~ '9' 값만 받는다.
				if(wParam < 48 || wParam > 57)
					break;
			}
			else if( IsStyle(IME_STYLE_NUMERIC_AND_NUMERICAL))
			{
				//	'0' ~ '9', '+', '-', '.' 값만 받는다.
				if(wParam != 43 && wParam != 45 && wParam != 46)
				{
					if(wParam < 48 || wParam > 57)
						break;
				}
			}
			
			//if(wParam > 31) 
			{
				//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
				//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다.
				if( !IsLimitEnoughChar((char) wParam) || m_nLimit == 0)
				{
					wchar_t temp;
					BOOL bAddChar = TRUE;

					MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);
					
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
								WideCharToMultiByte(m_codePage, 0, &m_input[last], 1, &prev, 1, NULL, NULL);								

								//	이전 문자가 태국어 문자일때만, 이전 문자가 태국어가 아니면 조합될 필요없다.
								if( prev & 0x80)
								{
									//	현재 넘어온 문자가 태국어에서 조합 가능한 문자인지 검사하여 조합 불가능 상태라면 입력 버퍼에 추가하지 않는다.
									if( !i3String::IsComposite(m_codePage, prev, curr, 2))
									{
										//	혼자 자립할수 있는 태국어 문자라면 (자음이나 모음은 한글자로 입력될수 있으나 성조같은 것은 자립해서 입력하지 못하도록)
										if( !i3String::IsComposite(m_codePage, 0, curr, 2))
										{
											bAddChar = FALSE;
										}
									}
								}
							}				
						}
					}

					if(bAddChar)
					{
						if( wParam == '\r')
							temp = '\n';

						if( m_bBlock)
						{
							ReplaceNChars( m_nBlkStart, m_nBlkEnd, &temp, 1);
							m_nInsertPos = min( m_nBlkStart, m_nBlkEnd) + 1;
							m_bBlock = false;
						}
						else
						{
							InsertNChars( m_nInsertPos, &temp, 1);
							m_nInsertPos++;
						}

						m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

						if( m_pEditProc != NULL)
							m_pEditProc( this, m_nState, m_pParam);
					}
				}
				else if( m_bBlock)
				{
					wchar_t temp;
					MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);

					if( wParam == '\r')
						temp = '\n';

					ReplaceNChars( m_nBlkStart, m_nBlkEnd, &temp, 1);
					m_nInsertPos = min( m_nBlkStart, m_nBlkEnd) + 1;
					m_bBlock = false;

					m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

					if( m_pEditProc != NULL)
						m_pEditProc( this, m_nState, m_pParam);
				}
			}
		}
		break;
	}

	return true;
}

INT32 i3InputMethodEditor::SetInput( const WCHAR * text, INT32 len, BOOL bAvariableLimitText)
{
	//	유니코드 문자열의 길이를 받아온다.
	INT32 length = i3String::LengthW( text);

	//	길이가 0이상일 경우만
	if(text == NULL || len == 0 || length == 0)
	{
		_clearBuffer();

		return 0;
	}
	else
	{
		//	IME 입력 버퍼 클리어
		_clearBuffer();

		INT32 checkSize = length;
		INT32 limitSize = GetLimit();
		INT32 inputSize = GetInputLengthW();

		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
		//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
		if( bAvariableLimitText && IsLimitEnough(checkSize))
		{
			INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

			//	바이트 단위일 경우
			/*if( !IsStyle(IME_STYLE_LIMIT_CHARACTER))
			{
				overheadSize = (overheadSize / sizeof(wchar_t)) + (overheadSize % sizeof(wchar_t));
			}*/

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
		
		//i3MemFree( wTemp);

		return length;
	}
}

INT32 i3InputMethodEditor::AddInput( const WCHAR * text, INT32 len, BOOL bAvariableLimitText)
{
	INT32 limitSize = 0;			
	INT32 inputSize = 0;
	INT32 checkSize = 0;
	INT32 length	= i3String::LengthW( text);

	//	길이가 0이상일 경우만
	if(text == NULL || len == 0 || length == 0)
		return 0;

	if( bAvariableLimitText)
	{
		//	문자 제한 갯수를 넘을시 입력을 받지 못하도록
		if( IsLimit(FALSE) && m_nLimit > 0)
			return 0;
	}	
	
	checkSize = length;
	limitSize = GetLimit();
	inputSize = GetInputLengthW();	//	must without comp		

	//	문자 제한 갯수를 넘을시 입력을 받지 못하도록 
	//	제한 갯수 넘으면 마지막 조합중 문자는 입력받지 않습니다. (제한 갯수까지만 뒤에서 끊는다)
	if( bAvariableLimitText && IsLimitEnough(checkSize))
	{
		INT32 overheadSize = (inputSize + checkSize) - limitSize;	//	바이트로 계산

		//	뒤에서 짤라내고 마지막에 출력해야할 문자 크기만 계산한다.
		length -= overheadSize;
		
		if( length < 0)
			length = 0;
	}

	//	문자 삽입
	for( INT32 i = 0; i < length; i++)
	{
		m_input.insert( m_input.begin() + m_nInsertPos, 1, text[ i]);	

		m_nInsertPos++;
	}

	if( IsLimit() && m_nLimit > 0) 
		_clearCompBuffer();

	m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;
	m_bIsComp = FALSE;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);

	//i3MemFree( wTemp);

	return length;
}

void i3InputMethodEditor::SetInputFinish( HWND hwnd)
{
	HIMC hImc = ImmGetContext( hwnd);		

	if( IsComp())	//	조합 글자 입력 중지
	{
		//	조합 중지
		PostMessage( hwnd, WM_IME_ENDCOMPOSITION, 0, 0);

		//	조합중 문자 마무리
		ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0); 

		_clearCompBuffer();		
	}	

	ImmReleaseContext( hwnd, hImc);
}

void i3InputMethodEditor::SetInputCancel( HWND hwnd)
{
	HIMC hIMC = ImmGetContext( hwnd);

	if( IsComp())	//	조합 글자 입력 중지
	{
		_clearCompBuffer();

		//	조합 중지
		PostMessage( hwnd, WM_IME_ENDCOMPOSITION, 0, 0);

		//	조합중 문자 취소
		ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); 
	}	

	ImmReleaseContext( hwnd, hIMC);
}

BOOL i3InputMethodEditor::IsImeActive( void)
{
	return (m_ImeState == IMEUI_STATE_ON);
}

void i3InputMethodEditor::SetImeEnable( HWND hWnd, BOOL bEnable)
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
		m_hOldIMC = ImmAssociateContext(hWnd, NULL);		// It makes IME disable 

		if( m_hOldIMC != 0)
			_clearCompBuffer();
	}
}

BOOL i3InputMethodEditor::SetImeMode( HWND hWnd, IME_MODE mode)
{	
	BOOL ret = TRUE;
	HIMC hImc = ImmGetContext( hWnd);
	if( hImc == 0)
	{
		I3TRACE("[%s] Warning! Failed to set IME const mode.\n", __FUNCTION__);
		return FALSE;
	}

	//	현재 입력 언어를 체크하여 같다면 처리 안하도록 한다.
	DWORD dwConversion, dwSentence;
	if( ImmGetConversionStatus( hImc, &dwConversion, &dwSentence))
	{
		switch( mode)
		{
			case IME_MODE_ALPHANUMERIC:		
				{
					if( dwConversion == IME_CMODE_ALPHANUMERIC)		goto EXIT_IME_MODE;
				}
				break;
			case IME_MODE_NATIVE:			
				{
					if( dwConversion == IME_CMODE_NATIVE)			goto EXIT_IME_MODE;
				}
				break;
		}
	}
	else
	{
		ret = FALSE;

		goto EXIT_IME_MODE;
	}

	m_modeIME = mode;

	//	강제로 입력 언어를 변경한다.
	switch( mode)
	{
	case IME_MODE_ALPHANUMERIC:	//	영문 모드로 변경
		{
			if( ImmSetConversionStatus( hImc, IME_CMODE_ALPHANUMERIC, IME_SMODE_CONVERSATION) == 0)
			{
				I3TRACE("[%] Warning! Failed to set IME const mode.\n", __FUNCTION__);

				ret = FALSE;

				goto EXIT_IME_MODE;
			}
		}
		break;
	case IME_MODE_NATIVE:		//	자국어 모드로 변경
		{
			if( ImmSetConversionStatus( hImc, IME_CMODE_NATIVE, IME_SMODE_CONVERSATION) == 0)
			{
				I3TRACE("[%s] Warning! Failed to set IME const mode.\n", __FUNCTION__);
				
				ret = FALSE;

				goto EXIT_IME_MODE;
			}
		}
		break;
	}


EXIT_IME_MODE:
	ImmReleaseContext(hWnd, hImc);

	return ret;
}

IME_MODE i3InputMethodEditor::GetImeMode(void)
{
	return m_modeIME;
}

DWORD i3InputMethodEditor::GetImeModeValue( HWND hWnd)
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

void i3InputMethodEditor::DumpBuffer( void)
{
	WCHAR16 szState[256] = L"";
	switch(m_nState)
	{
	case EDIT_STATE_NONE:						i3String::CopyW(szState, L"NONE");				break;
	case EDIT_STATE_CLEAR:						i3String::CopyW(szState, L"CLEAR");				break;
	case EDIT_STATE_RETURN:						i3String::CopyW(szState, L"RETURN");			break;
	case EDIT_STATE_CHANGE_COMPOSITION_CHAR:	i3String::CopyW(szState, L"COMPOSITION_CHAR");	break;
	case EDIT_STATE_CHANGE_INPUT_CHAR:			i3String::CopyW(szState, L"INPUT_CHAR");		break;
	case EDIT_STATE_CHANGE_INSERT_POSITION:		i3String::CopyW(szState, L"INSERT_POSITION");	break;
	case EDIT_STATE_CHANGE_LANGUAGE:			i3String::CopyW(szState, L"LANGUAGE");			break;
	case EDIT_STATE_CHANGE_IME_MODE:			i3String::CopyW(szState, L"IME_MODE");			break;
	}

	I3TRACE( L"[%s:(0x%p)] : (%s) Inst(%d/%d) ", __FUNCTIONW__, this, szState, m_nInsertPos, m_nLimit);

	GetInput( strTemp, (INT32) sizeof( strTemp)/sizeof( WCHAR16));
	I3TRACE( L"Input(%s) ", strTemp);

	GetComp( strTemp, (INT32) sizeof( strTemp)/sizeof( WCHAR16));
	I3TRACE( L"Comp(%s) ", strTemp);

	GetExactInput( strTemp, (INT32) sizeof( strTemp)/sizeof( WCHAR16));
	I3TRACE( L"Exact(%s)\n", strTemp);
}

void i3InputMethodEditor::DumpBufferToCode(void)
{	
	char t[128] = "";
	for( INT32 i = 0; i < (INT32)m_input.size(); i++)
	{	
		INT32 v = (INT32)m_input[i];
		sprintf( t, "[Input buf %d] 0x%x\n", i, v);	
		I3TRACE(t);
	}
}

INT32 i3InputMethodEditor::_GetInputSize(UINT32 imeStyle, BOOL bWithComp)
{
	return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
	/*
	if( imeStyle & IME_STYLE_LIMIT_CHARACTER)
	{
		switch( m_codePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				if( m_nInsertPos == 0) return 0;

				INT32 len = WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, NULL, NULL);
				I3ASSERT( len < 4096);
				strTemp[len] = 0;
				
				return i3String::LengthChar( m_codePage, strTemp);
			}
			break;
		default:
			return GetInputLengthW() + (bWithComp ? GetCompLengthW() : 0);
			break;
		}
	}
	else	//	default
	{
		return GetInputLengthA() + (bWithComp ? GetCompLengthA() : 0);
	}
	*/
}

BOOL i3InputMethodEditor::IsLimit( BOOL bWithComp /* = TRUE */)		
{ 
	INT32 nInputLength = _GetInputSize( GetStyle(), bWithComp);

	return (m_nLimit <= nInputLength); 
}

BOOL i3InputMethodEditor::IsLimitEnough(INT32 nAddSize, BOOL bWithComp /* = TRUE */)		
{ 
	INT32 nInputLength = _GetInputSize( GetStyle(), bWithComp);
	
	return (m_nLimit < nInputLength + nAddSize); 
}

BOOL i3InputMethodEditor::IsLimitEnoughChar(char cAddChar)		
{ 
	return (m_nLimit < (GetInputLengthW() + 1));
	/*
	INT32 nInputLength = 0;

	if( IsStyle(IME_STYLE_LIMIT_CHARACTER))
	{
		switch( m_codePage)
		{
		case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
			{
				if( m_nInsertPos == 0) return 0;

				INT32 len = WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_input.size(), strTemp, 4096, NULL, NULL);
				I3ASSERT( len < 4096);

				strTemp[len] = cAddChar;
				strTemp[len+1] = 0;
				
				nInputLength = i3String::LengthChar( m_codePage, strTemp);
			}
			break;
		default:
			nInputLength = GetInputLengthW() + 1;
			break;
		}
	}
	else	//	default
	{
		nInputLength = GetInputLengthA() + 1;
	}
	
	return (m_nLimit < nInputLength); */
}

INT32 i3InputMethodEditor::GetExactInput( WCHAR* pOutBuffer, INT32 nOutLength)
{	
	WCHAR szComp[ 256] = L"";
	INT32 nCaretPos = GetInsertPositionW(); 
	INT32 retInput = 0;
	INT32 retComp = 0;

	if( pOutBuffer == NULL || nOutLength == 0) return 0; 

	retInput = GetInput( pOutBuffer, nOutLength);
	retComp = GetComp( szComp, sizeof( szComp));	


	//	최종 문자열의 크기가 인자로 들어온 버퍼 크기를 오버하지 않는지 검사
	//I3ASSERT( (i3String::LengthW( pOutBuffer) + i3String::LengthW( szComp)) < nOutLength);
	if( (i3String::LengthW( pOutBuffer) + i3String::LengthW( szComp)) >= nOutLength)
	{
		return retInput;
	}

	//	조합문자를 버퍼 중간에 삽입
	i3String::InsertStrW( pOutBuffer, szComp, nCaretPos, i3String::LengthW( szComp));

	pOutBuffer[ nOutLength - 1] = 0;	//	널문자

	return retInput + retComp;
}
 
INT32 i3InputMethodEditor::GetInput(WCHAR* out, INT32 len)
{
	i3String::NCopyW( out, m_input.c_str(), m_input.size());

	return m_input.size();
	/*
	INT32 ret = ConvertString(m_codePage, m_input.c_str(), m_input.size(), out, len);

	return ret;*/
}

INT32 i3InputMethodEditor::GetComp(WCHAR* out, INT32 len)
{
	i3String::NCopyW( out, m_comp.c_str(), m_input.size());

	return m_comp.size();
	/*
	INT32 ret = ConvertString(m_codePage, m_comp.c_str(), m_comp.size(), out, len);

	return ret;
	*/
}

INT32 i3InputMethodEditor::GetReading(WCHAR* out, INT32 len)
{
	i3String::NCopyW( out, m_reading.c_str(), m_input.size());

	return m_reading.size();
	/*
	INT32 ret = ConvertString(m_codePage, m_reading.c_str(), m_reading.size(), out, len);

	return ret;
	*/
}

INT32 i3InputMethodEditor::GetCandidate(DWORD index, WCHAR* text, INT32 len) 
{
	if(m_candidate.empty())
	{
		return 0;
	}
	else
	{
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount)
		{
			return 0;
		}
		else
		{
			wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

			INT32 length = wcslen(wText);

			i3String::NCopyW( text, wText, length);

			return length;
		}
	}
}

INT32		i3InputMethodEditor::GetCandidateCount( void) 
{
	if(m_candidate.empty())
	{
		return 0;
	}
	else
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
	}
}

INT32		i3InputMethodEditor::GetCandidateSelection( void) 
{
	if(m_candidate.empty())
	{
		return 0;
	} 
	else
	{
		if(PRIMARYLANGID(m_langId) == LANG_KOREAN)
			return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
		else
			return ((CANDIDATELIST*)&m_candidate[0])->dwSelection;
	}
}

INT32		i3InputMethodEditor::GetCandidatePageSize( void) 
{
	if(m_candidate.empty())
	{
		return 0;
	} 
	else
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageSize;
	}
}

INT32		i3InputMethodEditor::GetCandidatePageStart( void) 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageStart;
	}
}

void	i3InputMethodEditor::GetUnderLine(INT32* start, INT32* end) 
{ 	
	*start = m_ulStart;//WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulStart, NULL, 0, NULL, NULL); 
	*end = m_ulEnd;//WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulEnd, NULL, 0, NULL, NULL); 
}

void	i3InputMethodEditor::GetImeId( void)
{
    char    szTmp[1024];

	m_dwImeID[0] = m_dwImeID[1] = 0;

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

    if( dwVerSize )
	{
        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) 
		{
			LPVOID  lpVerData;
			UINT32    cbVerData;

            if( VerQueryValueW( lpVerBuffer, L"\\", &lpVerData, &cbVerData ) ) {

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

                    m_dwImeID[0] = dwVer | m_langId;
                    m_dwImeID[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}


void	i3InputMethodEditor::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	_GetReadingString = NULL;
    _ShowReadingWindow = NULL;

	if( ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
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

bool	i3InputMethodEditor::GetReadingWindowOrientation( void)
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( m_dwImeID[0] == 0 );
    if( !bHorizontalReading && ( m_dwImeID[0] & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = m_dwImeID[0] & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
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

void	i3InputMethodEditor::GetPrivateReadingString(HWND hWnd)
{
    if( !m_dwImeID[0] ) {
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
        BOOL bVertical;
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
		INPUTCONTEXT *lpIC = _LockIMC(hImc);

		if(lpIC == NULL) {
			temp = NULL;
			tempLen = 0;
		} else {
			LPBYTE p = 0;
			switch( m_dwImeID[0] )
			{
				case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
				case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
				case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
					p = *(LPBYTE *)((LPBYTE)_LockIMCC( lpIC->hPrivate ) + 24 );
					if( !p ) break;
					tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
					dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
					temp = (wchar_t *)( p + 56 );
					bUnicodeIme = true;
					break;

				case IMEID_CHT_VER50: // 5.0.x.x // WinME
					p = *(LPBYTE *)( (LPBYTE)_LockIMCC( lpIC->hPrivate ) + 3 * 4 );
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
					p = *(LPBYTE *)((LPBYTE)_LockIMCC( lpIC->hPrivate ) + 4);
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
						nOffset = ( m_dwImeID[1] >= 0x00000002 ) ? 8 : 7;

						p = *(LPBYTE *)((LPBYTE)_LockIMCC( lpIC->hPrivate ) + nOffset * 4);
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
						p = *(LPBYTE *)((LPBYTE)_LockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
					}
					break;

				default:
					temp = NULL;
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
				INT32 wstrLen = MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0); 
				wchar_t* wstr = (wchar_t*)alloca(sizeof(wchar_t)*wstrLen);
				MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, wstr, wstrLen); 
				m_reading.assign(wstr, wstrLen);
			}
		}

		_UnlockIMCC(lpIC->hPrivate);
		_UnlockIMC(hImc);

		m_bVerticalReading = !GetReadingWindowOrientation();
    }

	ImmReleaseContext(hWnd, hImc);
}

void	i3InputMethodEditor::CheckToggleState(HWND hWnd)
{
	// Update Indicator
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = g_szIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = g_szIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

	    m_bVerticalCandidate = true;
		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_bVerticalCandidate = m_dwImeID[0] == 0;
            m_wszCurrIndicator = g_szIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = g_szIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = g_szIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

	case LANG_THAI:
		m_bVerticalCandidate = false;		
		m_wszCurrIndicator = g_szIndicator[INDICATOR_NON_IME];
        break;

    default:
        m_wszCurrIndicator = g_szIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == g_szIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }
		
	// Check Toggle State 
	bool bIme = ImmIsIME( m_hkl ) != 0;

	HIMC hImc = ImmGetContext(hWnd);
    if( hImc ) {

        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) {

            DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

        } 
		else if( ( PRIMARYLANGID(m_langId) == LANG_THAI ) )  
		{
			DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;	
		}
		else {

            m_ImeState = ( bIme && ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }

		//	check mode
		switch(GetImeModeValue(hWnd))
		{
		case IME_CMODE_ALPHANUMERIC:	m_modeIME = IME_MODE_ALPHANUMERIC;	break;
		case IME_CMODE_NATIVE:			m_modeIME = IME_MODE_NATIVE;		break;
		default:						m_modeIME = IME_MODE_UNKNOWN;		break;
		}

		ImmReleaseContext(hWnd, hImc);
        
	} else {

        m_ImeState = IMEUI_STATE_OFF;
	}
}

#if 0		//	IME 메시지 덤프
	#define MSG_TRACER(szMsg, wParam, lParam)	I3TRACE3( "[IME] %s : w %08x, l %08x\n", szMsg, wParam, lParam);
#else
	#define MSG_TRACER(szMsg, wParam, lParam)	((void)0)	
#endif

LRESULT i3InputMethodEditor::MessageProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultProc)
{
	switch( message)
	{
	case WM_CREATE:	
		OnInputLanguageChange( hWnd, 0, (LPARAM) GetKeyboardLayout(0));
		
		return 0L;

	// IME Messages
	case WM_INPUTLANGCHANGE:
		MSG_TRACER( "WM_INPUTLANGCHANGE", wParam, lParam);
		OnInputLanguageChange( hWnd, wParam, lParam);		
		break;

	// Except IME mode control in Janap Win9x, dose not need to send this message
	case WM_IME_SETCONTEXT:
		{
			MSG_TRACER( "WM_IME_SETCONTEXT", wParam, lParam);
			lParam = 0;			
		}
		break;

	// Start composition
	case WM_IME_STARTCOMPOSITION:
		MSG_TRACER( "WM_IME_STARTCOMPOSITION", wParam, lParam);
		return 0L;

	// Composotion char
	case WM_IME_COMPOSITION:
		MSG_TRACER( "WM_IME_COMPOSITION", wParam, lParam);
		if( OnComposition( hWnd, wParam, lParam)) 
		{
			return 0L;
		}
		break;

	// End conposition
	case WM_IME_ENDCOMPOSITION:
		MSG_TRACER( "WM_IME_ENDCOMPOSITION", wParam, lParam);
		if( OnEndComposition( hWnd, wParam, lParam)) 
		{
			//I3TRACE( "OnEndComposition\n"); 
			return 0L;
		}
		break;
	
	// IME Notofy
	case WM_IME_NOTIFY:
		MSG_TRACER( "WM_IME_NOTIFY", wParam, lParam);
		if( OnNotify( hWnd, wParam, lParam)) 
		{
			return 0L;
		}
		break;

	//	Process Character
	case WM_CHAR:
		{	
			if( !m_bCtrlPressed)
			{
				MSG_TRACER( "WM_CHAR", wParam, lParam);
				//	문자 입력을 받는다.
				OnChar( hWnd, wParam, lParam);
			}
		}
		return 0L;

	//	Process Keyboad
	case WM_KEYDOWN:
		{
			MSG_TRACER( "WM_KEYDOWN", wParam, lParam);
			
			if( m_bCtrlPressed)
			{
				switch( wParam)
				{
					case 'G' :		Copy( hWnd);				break;
					case 'H' :		Cut( hWnd);				break;
					case 'B' :		Paste( hWnd);				break;
					case 'A' :		SelectAll( hWnd);			break;
					default :										break;
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
					
					default: 			m_FeedEnterCount = 0;		break;	// 피드카운트를 초기화합니다.
				}
			}
		}
		return 0L;

	case WM_KEYUP:
		{
			MSG_TRACER( "WM_KEYUP", wParam, lParam);

			switch( wParam)
			{
				case VK_SHIFT :			OnKeyUpShift( hWnd);		break;
				case VK_CONTROL :		OnKeyUpControl( hWnd);		break;
				default:	break;
			}
		}
		return 0L;

	case WM_SETFOCUS:		
		{
			//I3TRACE( "SET FOCUS (0x%p)\n", this);

			SetImeMode(hWnd, m_modeIME);
		}
		break;
	//	Kill focus
	case WM_KILLFOCUS:
		{
			//I3TRACE( "KILL FOCUS (0x%p)\n", this);
			SetInputFinish( hWnd);		//	조합 중지	
		}
		break;

	default:
		{
			if( defaultProc != 0)
				return CallWindowProc( defaultProc, hWnd, message, wParam, lParam);
			else
				return DefWindowProc( hWnd, message, wParam, lParam);
		}
		break;
	}

	

	return 0;
}

/*****************************************************************************\
* Function: GetStringWidth
*
* Calculate string width in pixel 
*
* Arguments:
*	LPCTSTR szStr - string 
\*****************************************************************************/
INT32 i3InputMethodEditor::GetStringWidth( HDC hdc, HFONT hFont, const wchar_t * szStr, INT32 size)
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
INT32 i3InputMethodEditor::GetCombinedCharLength( INT32 nTmpPos)
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

INT32 i3InputMethodEditor::GetCursorPosition(void)
{
	return GetInsertPositionW();
	/*
	switch( m_codePage)
	{
	case I3_LANG_CODE_THAI:	//	THAI    커서 위치에 대한 한 문자 단위로 계산시 태국어 1~4 바이트의 가변 크기를 고려해야 한다.
		{
			if( m_nInsertPos == 0) return 0;

			INT32 len = WideCharToMultiByte( m_codePage, 0, m_input.c_str(), m_nInsertPos, strTemp, sizeof(strTemp), NULL, NULL);
			I3ASSERT( len < sizeof(strTemp));
			strTemp[len] = 0;
			
			return i3String::LengthChar( m_codePage, strTemp);
		}
		break;
	default:
		return GetInsertPositionW();
		break;
	}*/
}

char * i3InputMethodEditor::GetCurrentLanguage( void)
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
	default:					pszLang = "ANSI";			break;
	}

	return pszLang;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3InputMethodEditor::DeleteNChars( INT32 nStart, INT32 nEnd)
{
	INT32 nMin = min( nStart, nEnd);
	INT32 nMax = max( nStart, nEnd);

	m_input.erase( nMin, nMax-nMin);

	//m_nInsertPos = nMin;
}

void i3InputMethodEditor::InsertNChars( INT32 nInsPos, WCHAR* pChars, INT32 nLen)
{
	m_input.insert( nInsPos, pChars, nLen);

	//m_nInsertPos = nInsPos + nLen;
}

void i3InputMethodEditor::ReplaceNChars( INT32 nStart, INT32 nEnd, WCHAR* pChars, INT32 nLen)
{
	INT32 nMin = min( nStart, nEnd);
	INT32 nMax = max( nStart, nEnd);

	m_input.replace( nMin, nMax-nMin, pChars, 0, nLen);

	//m_nInsertPos = nMin + nLen;
}

void i3InputMethodEditor::Copy( HWND hWnd)
{
	I3TRACE("Copy\n");
	
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

		m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

		if( m_pEditProc != NULL)
			m_pEditProc( this, m_nState, m_pParam);
	}
}

void i3InputMethodEditor::Cut( HWND hWnd)
{
	if( m_bBlock)
	{
		Copy( hWnd);
		DeleteNChars( m_nBlkStart, m_nBlkEnd);
		m_nInsertPos = min( m_nBlkStart, m_nBlkEnd);
		
		m_bBlock = false;

		m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

		if( m_pEditProc != NULL)
			m_pEditProc( this, m_nState, m_pParam);

	}
}

void i3InputMethodEditor::Paste( HWND hWnd)
{
	
	if( IsClipboardFormatAvailable( CF_UNICODETEXT))
	{
		HGLOBAL hmem;
		WCHAR16* ptr;

		OpenClipboard( hWnd);
		hmem = GetClipboardData( CF_UNICODETEXT);
		ptr = (WCHAR16*)GlobalLock( hmem);
		INT32 len = wcslen( ptr);
		
		
		if( m_bBlock)
		{
			INT32 nBlockLen = max( m_nBlkStart, m_nBlkEnd) - min( m_nBlkStart, m_nBlkEnd);

			if( (INT32)(m_input.size()) - nBlockLen + len <= m_nLimit)		//signed/unsigned mismatch
			{
				ReplaceNChars( m_nBlkStart, m_nBlkEnd, ptr, len);
				m_nInsertPos = min( m_nBlkStart, m_nBlkEnd) + len;
				m_bBlock = false;
			}
		}
		else
		{
			if( (INT32)(m_input.size()) + len <= m_nLimit)		//signed/unsigned mismatch
			{
				InsertNChars( m_nInsertPos, ptr, len);
				m_nInsertPos += len;
			}
		}

		GlobalUnlock( hmem);
		CloseClipboard();

		m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

		if( m_pEditProc != NULL)
			m_pEditProc( this, m_nState, m_pParam);
	}
}

void i3InputMethodEditor::SelectAll( HWND hWnd)
{
	m_nBlkStart = 0;
	m_nBlkEnd = m_nInsertPos = m_input.length();
	m_bBlock = true;

	m_nState = EDIT_STATE_CHANGE_INPUT_CHAR;

	if( m_pEditProc != NULL)
		m_pEditProc( this, m_nState, m_pParam);

	I3TRACE("SelectALl\n");
}

#endif
