#if !defined( I3_INPUT_IME_H)
#define I3_INPUT_IME_H

//=================================================================================================
// Windows IME (Input Method Edit) (ANSI 모드 사용) 
//
// 출처 : GpgStudy source & MSDN Sample
// 작성일 : 2007.1.30
// 담당자 : 백성민
// Sample : Samples/i3Input/IME/IME
//
// 변경사항.
// Vietnam Charset/Codepage 처리 추가 (switch문)
// (2007.1.29) 기존 코드에는 입력 버퍼에 중간 삽입, 중간 삭제 기능이 없었으므로 현시점에 기능을 추가합니다.
// (2007.1.30) Callback Proc을 등록할 수 있게 하였으며 Edit의 State 기능을 추가합니다.
//
// note.
// * MS Windosw 계열에서만 작동하며 이외 플래폼에서는 작동하지 않습니다. 
// * 기본적으로 ANSI 버젼에서 작동하는 코드이며 다국어 입/출력 관련 코드가 들어 있습니다.
// * Create 이후 Edit를 하기 전에 적어도 한번은 언어 설정을 위해 OnInputLanguageChange() 함수를 호출해야 문자 조합시 정상 작동한다.
// * I3_IME_EDIT_PROC 콜백 함수를 생성할때 등록하면 문자열 정보가 갱싱될때 마다 한번씩 호출해준다.
// * 반드시 WndProc에서 MessageProc를 호출해어야 윈도우에서 발생하는 IME 메시지를 내부에서 처리해준다.
//
// * (중국어) CHT Versions per Operating System
//       Operating System           CHT IME Version 
//         Windows 98                     4.2 
//        Windows 2000                    4.3 
//           unknown                      4.4 
//         Windows ME                     5.0 
//          Office XP                     5.1 
//         Windows XP                     5.2 
//    Standalone web downloadble          6.0 
//	
// * MSDN Site :	Using an Input Method Editor in a Game 
//					http://msdn2.microsoft.com/en-us/library/bb206300.aspx
//=================================================================================================
#if defined( I3_WINDOWS)

// 아래 경고를 제거하기 위해
// warning! class <XXX> needs to have dll-interface to be used by clients of class <YYY> 
#pragma warning(disable: 4251)

#include "i3Base.h"
#include "i3InputIMEContext.h"

#include <string>
#include <vector>

//	IME Edit State
enum EDIT_STATE
{
	EDIT_STATE_NONE = 0,				//	아무 일도 일어나지 않았다.
	EDIT_STATE_CLEAR,					//	Clear
	EDIT_STATE_RETURN,					//	리턴( 엔터)키가 입력됐다.
	EDIT_STATE_CHANGE_COMPOSITION_CHAR,	//	문자가 조합중이다. Insert Position은 변경되지 않는다.
	EDIT_STATE_CHANGE_INPUT_CHAR,		//	문자 입력됐다. Insert Position 또한 변경
	EDIT_STATE_CHANGE_INSERT_POSITION,	//	Insert Postion (Caret)의 위치만 변경됐다.
	EDIT_STATE_CHANGE_LANGUAGE,			//	언어가 바꿨다.
	EDIT_STATE_CHANGE_IME_MODE,			//	IME 모드(한/영)가 바뀌었다.
	EDIT_STATE_COMPOSITION_START,		// 
	EDIT_STATE_COMPOSITION_END,			//

	EDIT_STATE_ONKEYDOWN_UP,
	EDIT_STATE_ONKEYDOWN_DOWN,
	EDIT_STATE_ONKEYDOWN_RIGHT,
	EDIT_STATE_ONKEYDOWN_LEFT,
	EDIT_STATE_ONKEYDOWN_HOME,
	EDIT_STATE_ONKEYDOWN_END,
	EDIT_STATE_ONKEYDOWN_PAGEUP,
	EDIT_STATE_ONKEYDOWN_PAGEDOWN,
	EDIT_STATE_ONKEYDOWN_ESCAPE,
	
	EDIT_STATE_ONCHAR,
};

//	IME mode
enum IME_MODE
{
	IME_MODE_UNKNOWN = -1,
	IME_MODE_ALPHANUMERIC = 0,		//	영어
	IME_MODE_NATIVE,				//	다국어
	// 더이상의 추가작업시에는 IME_MODE_ALPHANUMERIC사용부분에 대한 소스내 주의가 필요합니다. 
};

//	IME Style (Create할때 지정합니다.)
#define IME_STYLE_NUMERIC_ONLY				0x00000001		//	양수만 가능
#define IME_STYLE_NUMERIC_AND_NUMERICAL		0x00000002		//	양수, 음수, 실수 가능
#define IME_STYLE_LIMIT_CHARACTER			0x00000010		//	문자 입력 제한을 문자 갯수 단위로 검사한다. (없으면 바이트 단위로 검사합니다.)
#define IME_STYLE_IGNORE_KEYINPUT_IME_MODE	0x00000020		//	키입력(한/영)으로부터 IME 모드 변경을 무시한다. (키입력으로 한/영 변환이 안된다.)
#define IME_STYLE_ALPHA_NUMERIC_ONLY		0x00000040		//  알파벳,숫자만입력
#define IME_STYLE_ALPHA_ONLY                0x00000080		//  알파벳만 입력

#define IME_STYLE_MULTILINE					0x00000100		//	멀티라인입력가능
#define IME_STYLE_AUTO_UPPERCASE			0x00000200		//	알파벳 입력시 대문자 변환
#define IME_STYLE_AUTO_LOWERCASE			0x00000400		//	알파벳 입력시 소문자 변환




class i3InputIME;

//	Edit Callback Function. 문자열 정보가 갱신되면 등록된 함수를 호출해준다. 기본적으로 문자 조합중일때도 호출한다.
typedef void (* I3_IME_EDIT_PROC) ( i3InputIME * pIME, EDIT_STATE state, void * pParam);

// when you create ime dummy window at i3ime.cpp you must use this value. 
#define	I3IME_DUMMYWND_OFFSET	2048
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
class I3_EXPORT_INPUT i3InputIME : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3InputIME, i3ElementBase);
	
public:
	
	i3InputIME(void);
	virtual ~i3InputIME( void);

	void		Create( INT32 nLimit = 0, UINT32 nStyle = 0);		
	void		Reset( void);
	void		ClearBuffer( void);
	void		DumpBuffer( void);				//	input buffer dump	
	void		DumpBufferToCode(void);
	void		SetEditProc( I3_IME_EDIT_PROC pFunc, void * pParam = nullptr);		//	Registration a IME result callback function
	LRESULT		MessageProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultProc = 0);	// IME Message processing

	// Windows & IME Messages on WndProc
	void		OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam);	// Change the setting if input language is changed
	bool		OnStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool		OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);			// Composotion char
	bool		OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);		// End conposition
	bool		OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
//	bool		OnChar( INT32 nScanCode, IDirectInputDevice8* pKeyboard);
	bool		OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		OnKeyDownLeft( HWND hWnd);
	void		OnKeyDownRight( HWND hWnd);
	void		OnKeyDownDelete( void);
	void		OnKeyDownHome( HWND hWnd);
	void		OnKeyDownEnd( HWND hWnd);	
	void		OnKeyDownBackspace( void);
	void		OnKeyDownReturn( HWND hWnd);
	void		OnKeyDownSpacebar( HWND hWnd);
	void		OnKeyDownProcess( HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		OnSetCompWindowPos( HWND hWnd);
	
	//	Input data
	INT32		GetExactInput( i3::wstring& out);
	INT32		GetInput(i3::wstring& out);
	INT32		GetComp(i3::wstring& out);	
	INT32		GetReading(i3::wstring& out);
	INT32		GetCompToIndex(i3::wstring& out);	// composition indicator의 위치까지 string을 얻어옴.//caret의 위치를 파악하기위해서.

	//	주의! 안에서 메모리 생성/해제를 하므로 루프 또는 매프레임 하지 마세요...komet
	INT32		SetInput( const i3::wliteral_range& wTextRng, bool bAvariableLimitText = true);
	INT32		AddInput( const i3::wliteral_range& wTextRng, bool bAvariableLimitText = true);

//	INT32		GetCandidate(DWORD index, char* text, int len);
	INT32		GetCandidateCount( void);
	INT32		GetCandidateSelection( void);
	INT32		GetCandidatePageSize( void);
	INT32		GetCandidatePageStart( void);
	void		GetUnderLine(INT32 * start, INT32 * end);

	//
	void		SetIMEOffsetPosition( INT32 x, INT32 y);
	void		SetCompositionPos( HWND hWnd);
	void		SetCompositionPos( HWND hWnd, INT32 x, INT32 y);
	void		setImeFont( HWND hWnd, HFONT hFont, LOGFONTA * pFont);

	void		SetInputFinish( HWND hwnd);		//	강제로 조합중 입력을 중지한다.
	void		SetInputCancel( HWND hwnd);

	// Property
	bool		IsVerticalReading( void)		{ return m_bVerticalReading;			}
	bool		IsVerticalCandidate( void)		{ return m_bVerticalCandidate;			}
	WORD		GetCodePage( void)				{ return m_codePage;					}
	LANGID		GetLangId( void)				{ return m_langId;						}
	UINT32      GetInputLang(void)				{ return PRIMARYLANGID(m_langId);		}	//	LANG_KOREAN
	INT32		GetCharSet( void)				{ return i3Language::GetCharsetFromLang(m_langId);	}
	wchar_t*	GetIndicator( void)				{ return m_wszCurrIndicator;			}	
	INT32		GetCombinedCharLength( INT32 nTmpPos);
	INT32		GetStringWidth( HDC hdc, HFONT hFont, const wchar_t * szStr, INT32 size = 0);	//	string width pixel
	INT32		GetStringWidth( HDC hdc, HFONT hFont, const wchar_t * szStr, INT32 nStart, INT32 nEnd)	
				{ 
					return GetStringWidth( hdc, hFont, (szStr + nStart), (nEnd - nStart));
				}
	INT32		GetStringWidthFromInsert( HDC hdc, HFONT hFont)		//	Pixel width of string until the insert position
				{
					return GetStringWidth( hdc, hFont, m_input.c_str(), m_nInsertPos);
				}
	INT32		GetInputStringWidth( HDC hdc, HFONT hFont)		{	return GetStringWidth( hdc, hFont, m_input.c_str());	}	// input width pixel
	INT32		GetCompStringWidth( HDC hdc, HFONT hFont)		{	return GetStringWidth( hdc, hFont, m_comp.c_str());		}	// composition width pixel
	INT32		GetReadingStringWidth( HDC hdc, HFONT hFont)	{	return GetStringWidth( hdc, hFont, m_reading.c_str());	}	// reading width pixel
	INT32		GetInputLengthW( void)			{ return (INT32) m_input.size();		}	//	Input string length by WideChara
//	INT32		GetInputLengthA( void);														//	Input string length by MultiByte
	INT32		GetCompLengthW( void)			{ return (INT32) m_comp.size();			}	//	composition string length by WideChara
//	INT32		GetCompLengthA( void);														//	composition string length by MultiByte
	INT32		GetInsertPositionW( void)		{ return m_nInsertPos;					}	//	Insert position by WideChara
//	INT32		GetInsertPositionA( void);													//	Insert position by MultiByte
	INT32		GetCursorPosition(void);
	char *		GetCurrentLanguage( void);		//	현재 언어의 문자열 형태로 반환
	bool		IsNT( void);					//	Windows NT System 계열 여부	
	bool		IsComp( void)					{ return m_bIsComp;						}	//	조합중 여부
	bool		IsLimit( bool bWithComp = true);					//	제한된 문자 갯수보다 입력 갯수가 같거나 많은면 true를 반환한다. true가 나오면 제한에 걸린 것이다.
	bool		IsLimitEnough(INT32 nAddSize, bool bWithComp = true);	//	제한된 문자 갯수보다 입력 갯수가 같거나 많은면 true를 반환한다. 추가 사이즈를 포함하여 계산한다.
	bool		IsLimitEnoughChar(char cAddChar);
	void		SetImeEnable( HWND hWnd, bool bEnable);										//	한/영 전환을 막는다. (다국어 모드)
	bool		SetImeMode( HWND hWnd, IME_MODE mode);										//	IME 모드를 전환
	IME_MODE	GetImeMode(void);
	DWORD		GetImeModeValue( HWND hWnd);
	bool		IsImeActive( void);			//	다국어 모드인지 여부 (한/영)	
	INT32		GetImeState( void)				{ return m_ImeState;					}
	INT32		GetLimit( void)					{ return m_nLimit;						}	//	문자 제한 갯수	
	EDIT_STATE	GetState( void)					{ return m_nState;						}
	UINT32		GetImeStyle(void)					{ return m_nStyle;						}
	void		AddImeStyle(UINT32 nStyle)			{ m_nStyle = m_nStyle | nStyle;			}
	void		RemoveImeStyle(UINT32 nStyle)		{ m_nStyle = m_nStyle & ~nStyle;		}
	bool		IsImeStyle(UINT32 nStyle)			{ return (m_nStyle & nStyle) != 0;			}

	void		setEnterFeedCount( INT32 cnt)	{ m_FeedEnterCount = cnt; }
	INT32		getCompositionCursorPos(void)	{	return m_nCompCursorIndex;}
	bool		isSelectingCandidate(void)		{	return m_bSelectingCandidate;}

protected:
	void		SetupImeApi(HWND hWnd);
	void		GetImeId( void);
	bool		GetReadingWindowOrientation( void);
	void		GetPrivateReadingString(HWND hWnd);
	void		CheckToggleState(HWND hWnd);	
	bool		InsertInputFromCompositionString(HWND hWnd);

	INT32		_GetInputSize(UINT32 imeStyle, bool bWithComp);
	
	void		_clearBuffer( void);
	void		_clearCompBuffer(void);

protected:
	i3::wstring			m_input;		//	현재 조합이 끝난 문자열만 저장
	i3::wstring			m_comp;			//	현재 조합중인 문자열만 저장
	i3::wstring			m_reading;
	i3::vector<BYTE>	m_candidate;
	
	INT32				m_ulStart = 0;
	INT32				m_ulEnd = 0;
	INT32				m_nInsertPos = 0;	//	현재 입력 버퍼 삽입 위치 (by WideChara)
	INT32				m_nLimit = 0;		//	문자 제한 갯수 (by WideChara. 0 (default) 일 경우 입력 무제한)
	INT32				m_nTempSize = 0;
	EDIT_STATE			m_nState = EDIT_STATE_NONE;		//	현재 입력 상태
	UINT32				m_PostKey = 0;
	bool				m_bIsComp = false;

	DWORD				m_dwImeProp = 0;
	HKL					m_hkl = 0;
	HIMC				m_hOldIMC = 0;
	LANGID				m_langId = 0xff;		//	사용 언어
	WORD				m_codePage = 0xff;
	DWORD				m_dwWindowsMajorVersion = 0;	//windows version

	bool				m_bCallDefProc = true;

	bool				m_bUnicodeIME = false;	//	유니코드 여부
	bool				m_bVerticalReading = false;			//	세로쓰기 여부	
	bool				m_bVerticalCandidate = false;
	INT32				m_ImeState;		//	다국어 모드
	wchar_t*			m_wszCurrIndicator = nullptr;

	bool				m_bSelectingCandidate = false;
	INT32				m_nCompCursorIndex = 0;

	HFONT				m_hFont = nullptr;

	DWORD				m_dwId[2] = { 0, 0 };

	HINSTANCE			m_hDllIme = nullptr;

	UINT				(WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, bool*, PUINT ) = nullptr;
	bool				(WINAPI * _ShowReadingWindow)( HIMC, bool ) = nullptr;

	I3_IME_EDIT_PROC 	m_pEditProc = nullptr;	//	Result Callback Func
	void *				m_pParam = nullptr;
	UINT32				m_nStyle = 0;
	IME_MODE			m_modeIME = IME_MODE_UNKNOWN;
	IME_MODE			m_ForceConvMode = IME_MODE_UNKNOWN;

	INT32				m_FeedEnterCount = 0;

	POINT				m_Pos = { 0, 0 };
};

#endif

#endif //I3_INPUT_IME_H


