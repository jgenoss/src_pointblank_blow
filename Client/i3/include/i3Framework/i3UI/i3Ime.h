#if !defined( I3_IME_H__)
#define I3_IME_H__

struct I3IME_CREATEINFO
{
	HWND		_hWnd = nullptr;
	HINSTANCE	_hInst = nullptr;
	INT32		_nTextLimitCount = 0;
	UINT32		_StyleIME = 0;
};

#include <string>
#include <vector>


// when you create ime dummy window at i3ime.cpp you must use this value. 
//#define	I3IME_DUMMYWND_OFFSET	2048
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_FRAMEWORK i3Ime :	public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Ime, i3ElementBase);

	// for ime window
protected:
	i3EventReceiver	*	m_pOwner = nullptr;
	HWND				m_hParent = nullptr;
	HWND				m_hWnd = nullptr;

	HFONT				m_hFont = nullptr;
	LOGFONTA			*	m_pLogFont = nullptr;

	HIMC				m_hImc = nullptr;
	HINSTANCE			m_hDllIme = nullptr;

	HKL					m_hKLayout = nullptr;	// keyboard layout handle
	IME_MODE			m_ImeMode = IME_MODE_NATIVE;
	LANGID				m_langID = 0;
	WORD				m_CodePage = 0;
	DWORD				m_dwProperty = 0;

	POINT				m_WindowPosition = { 0, 0 };

	////////////////////////////////////////////////////////////////
	i3::wstring		m_input;
	i3::wstring		m_comp;
	i3::wstring		m_reading;
	i3::wstring		m_text;

	INT32				m_ulStart = 0;
	INT32				m_ulEnd = 0;

	INT32				m_nInsertPos = 0;				//현재 입력 버퍼 삽입 위치 (by WideChara)
	INT32				m_nCompositionCursorIndex = 0;	// composition cursor의 index
	INT32				m_nLimit = 4096;					// 입력받을 문자의 제한 갯수.

	///////////////////////////////////////////////////////////////
	bool				m_bMultiLine = false;
	bool				m_bUnicode = false;
	INT32				m_ImeState = 0;		// 다국어 모드
	wchar_t*			m_wszLangIndicator = nullptr;
	INT32				m_FeedEnterCount = 0;

	///////////////////////////////////////////////////////////////
	WCHAR			*	m_pTextBuffer = nullptr;
	INT32				m_nTextBufferSize = 0;

	UINT32				m_nStyle = 0;
	EDIT_STATE			m_nState = EDIT_STATE_NONE;

	//
	bool				m_bFocused = false;
	bool				m_bCompositioning = false;
	bool				m_bSelectingCandidate = false;
	bool				m_bCallDefProc = true;
	UINT32				m_PostKey = 0;

	bool				m_bAlphaNumericOnly = false;
	bool				m_bImeFocusLock = false;
	//
	INT32				m_nBlkStart = -1;
	INT32				m_nBlkEnd = -1;

	bool				m_bBlock = false;
	bool				m_bShiftPressed = false;
	bool				m_bCtrlPressed = false;
	bool				m_bUseCopyNPaste = true;


	//'Ctrl+G'와 같은 입력의 경우 OnKeyDown 후에 OnChar가 호출되는데
	//원하는 호출이 아니므로 건너뛰기 위한 변수입니다. 
	bool				m_bFunctionalKey = false;

	DWORD				m_dwConvMode = 0;
	DWORD				m_dwSentMode = 0;

protected:
	void				_clearBuffer();
	void				_clearCompBuffer(void);
	INT32				_GetInputSize(UINT32 imeStyle, bool bWithComp);

public:
	i3Ime(void);
	virtual ~i3Ime(void);
	INT32 GetCompBoxCount() { return m_ulEnd - m_ulStart; }

	bool				Create( I3IME_CREATEINFO * pCreateInfo);

	// set event receiver
	void				setOwner( i3EventReceiver * pRecv)	{	m_pOwner = pRecv;}
	i3EventReceiver	*	getOwner(void)						{	return m_pOwner;}

	HWND				getHwnd( void)						{ return m_hWnd; }

	// focus
	bool				SetFocus( bool bSet);
	void				KillFocus();
	bool				IsImeFocusLocked() const { return m_bImeFocusLock; }

	bool				isFocused(void)						{	return m_bFocused;}

	//
	void				SetupImeApi(HWND hWnd);
	void				SetImeEnable(bool bEnable);
	void				CheckToggleState(HWND hWnd);

	void				setImeFont( HFONT hFont, LOGFONTA * pFont);

	//
	void				SetUseCopyNPaste( bool bUse )					{ m_bUseCopyNPaste = bUse; }

	DWORD				GetImeModeValue();
	bool				SetImeMode(IME_MODE mode);
	IME_MODE			GetImeMode(void)						{	return m_ImeMode;}
	EDIT_STATE			getEditState(void)						{	return m_nState;}

	void				setAlphaNumericOnly(bool bANOnly)		{	m_bAlphaNumericOnly = bANOnly;}

	INT32				getLimit(void)							{	return m_nLimit;}
	void				setLimit( INT32 limit)					{	m_nLimit = limit;}

	void				SetEnableMultiLine(bool bEnable);

	////////////////////////////////////////////////////////////////////////////////////////////////
	void				Reset(void);
	void				ClearBuffer(void);

	bool				InsertInputFromCompositionString(HWND hWnd);

	////////////////////////////////////////////////////////////////////////////////////////////////
	void				OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool				OnStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool				OnComposition( HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool				OnEndComposition( HWND hWnd, WPARAM wParam, LPARAM lParam);

	bool				OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool				OnChar( HWND hWnd, WPARAM wParam, LPARAM lParam);

	////////////////////////////////////////////////////////////////////////////////////////////////
	void				OnKeyDownLeft( HWND hWnd);
	void				OnKeyDownRight( HWND hWnd);
	void				OnKeyDownDown( HWND hWnd);
	void				OnKeyDownUp( HWND hWnd);
	void				OnKeyDownDelete( void);
	void				OnKeyDownHome( HWND hWnd);
	void				OnKeyDownEnd( HWND hWnd);	
	void				OnKeyDownBackspace( void);
	void				OnKeyDownReturn( HWND hWnd);
	void				OnKeyDownSpacebar( HWND hWnd);
	void				OnKeyDownShift( HWND hWnd);	
	void				OnKeyDownControl( HWND hWnd);
	void				OnKeyDownEscape( HWND hWnd);
	void				OnKeyDownPageup( HWND hWnd);
	void				OnKeyDownPagedown( HWND hWnd);
	
	void				OnKeyUpShift( HWND hWnd);
	void				OnKeyUpControl( HWND hWnd);

	void				DeleteNChars( INT32 nStart, INT32 nEnd);
	void				InsertNChars( INT32 nInsPos, WCHAR* pChars, INT32 nLen);
	void				ReplaceNChars( INT32 nStart, INT32 nEnd, WCHAR* pChars, INT32 nLen);

	void				Copy( HWND hWnd);
	void				Cut( HWND hWnd);
	void				Paste( HWND hWnd);
	void				SelectAll( HWND hWnd);

	void				SetBlockIdx( INT32 nStartIdx, INT32 nEndIdx) { m_nBlkStart = nStartIdx; m_nBlkEnd = nEndIdx; }
	INT32				GetBlockStartIdx( void)			{	return m_bBlock ? m_nBlkStart : -1;	}
	INT32				GetBlockEndIdx( void)			{	return m_bBlock ? m_nBlkEnd : -1;	}
	void				SetIsBlock( bool bBlock)		{	m_bBlock = bBlock; }
	INT32				IsBlock( void)					{	return m_bBlock;	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	const	wchar_t	*	GetText(void);
	const i3::wstring&  GetTextString();


	INT32				GetText( i3::wstring * input);
	INT32				GetInput( i3::wstring * input);
	INT32				GetComp( i3::wstring * input);

	INT32				SetInputText(const WCHAR * text, INT32 len, bool bLimitText = true);
	INT32				AddInputText(const WCHAR * text, INT32 len, bool bLimitText = true);

	void				SetInputFinish(void);
	void				SetInputCancel(void);

	////////////////////////////////////////////////////////////////////////////////////////////////
	void				SetImeWindowPosition( INT32 x, INT32 y);
	void				OnSetCompWindowPos();
	void				SetCompositionWndPos( void);
	void				SetCompositionWndPos( INT32 x, INT32 y);
	void				SetCandidateWndPos(INT32 x, INT32 y);

	////////////////////////////////////////////////////////////////////////////////////////////////
	WORD				getCodePage(void)				{	return m_CodePage;}
	LANGID				getLangID(void)					{	return m_langID;}
	UINT32				getInputLanguage(void)			{	return PRIMARYLANGID( m_langID);}
	INT32				getCharSet(void)				{	return i3Language::GetCharsetFromLang(m_langID);}
	wchar_t*			getLanguageIndicator(void)		{	return m_wszLangIndicator;}

	void				setImeFont( HWND hWnd, HFONT hFont, LOGFONTA * pFont);
	////////////////////////////////////////////////////////////////////////////////////////////////
	INT32				GetCombinedCharLength( INT32 nTempPos);
	INT32				GetStringWidth( const wchar_t* wszStr, INT32 size = 0);

	INT32				GetInputLengthA( void);
	INT32				GetCompLengthA( void);
	INT32				GetInsertPositionA( void);
	INT32				GetCursorPosition(void);

	INT32				GetInputLengthW(void)			{	return m_input.size();}
	INT32				GetCompLengthW(void)			{	return m_comp.size();}

	void				SetInsertPosition( INT32 pos);
	INT32				getInsertPos(void)				{	return m_nInsertPos;}
	INT32				getCompositionCursorPos(void)	{	return m_nCompositionCursorIndex;}

	bool				isCompositioning(void)			{	return m_bCompositioning;}
	void				SetCompositioning(bool Sig)		{	m_bCompositioning = Sig;}
	bool				IsLimit(bool bWidthComp);
	bool				IsEnough(INT32 nAddSize, bool bWidthComp);
	bool				IsEnough(char cAddChar);

	////////////////////////////////////////////////////////////////////////////////////////////////
	UINT32				getStyle(void)					{	return m_nStyle;}
	void				addStyle( UINT32 style)			{	m_nStyle |= style;}
	void				removeStyle( UINT32 style)		{	m_nStyle &= ~style;}
	bool				isStyle(UINT32 style)			{	return ((m_nStyle & style) != 0);}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void				_editProc( EDIT_STATE state);
	static	LRESULT		WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT				imeWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC defaultProc = 0);
	UINT	(WINAPI	*	_GetReadingString)(HIMC,UINT,LPWSTR,PINT,BOOL*,PUINT) = nullptr;
	BOOL	(WINAPI *	_ShowReadingWindow)(HIMC, BOOL) = nullptr;


	//void			PrintDebugText( void);
	static void			EnableConsoleKey(bool enable);


	////////////////////////////////////////////////////////////////////////////////////////////////
	// MS Text services framework(TSF) 를 이용하는 함수들입니다.
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	static bool			ShowLanguageBarWnd(bool show); // IME LanguageBar 윈도우를 보이거나/숨김니다
	void				CaptureImeMode();
	void				RevertImeMode();

};

#endif//define( I3_ime_H__)