#ifndef __I3UI_EDITBOX_H__
#define __I3UI_EDITBOX_H__

#include "i3UICaptionControl.h"
#include "i3Ime.h"
#include "i3UIScrollBar.h"
// ============================================================================
//
// i3UIEditBox
//
// ============================================================================
typedef UINT32 I3UI_EDITBOX_STYLE;
#define	I3UI_EDITBOX_STYLE_PASSWORD			0x00000001
#define	I3UI_EDITBOX_STYLE_READONLY			0x00000002
#define I3UI_EDITBOX_STYLE_NOBORDER			0x00000004
#define I3UI_EDITBOX_STYLE_CHAT				0x00000008
#define I3UI_EDITBOX_STYLE_CHATVIEW			0x00000010
#define I3UI_EDITBOX_STYLE_MULTILINE		0x00000020
#define I3UI_EDITBOX_STYLE_LIMIT_BYTESIZE	0x00000040
#define I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS	0x00000080

#define I3UI_EDITBOX_STYLE_SCROLL			0x00000F00
#define I3UI_EDITBOX_STYLE_SCROLL_LEFT		0x00000100
#define I3UI_EDITBOX_STYLE_SCROLL_RIGHT		0x00000200
#define I3UI_EDITBOX_STYLE_SCROLL_UP		0x00000400
#define I3UI_EDITBOX_STYLE_SCROLL_DOWN		0x00000800

#define I3UI_EDITBOX_STYLE_NUMERIC			0x00001000		// 숫자만 입력
#define I3UI_EDITBOX_STYLE_REAL_NUMERIC		0x00002000		// 실수형 입력
#define I3UI_EDITBOX_STYLE_ANNOUNCE			0x00004000	// 안내문구(튜토리얼...)
#define I3UI_EDITBOX_STYLE_ALPHA_NUMERIC	0x00010000		// 알파벳,숫자만
#define I3UI_EDITBOX_STYLE_ALPHA            0x00020000		// 알파벳만
#define I3UI_EDITBOX_STYLE_AUTO_UPPERCASE	0x00040000		// 알파벳입력시 자동 대문자로변환
#define I3UI_EDITBOX_STYLE_AUTO_LOWERCASE	0x00080000		// 알파벳입력시 자동 소문자로변환

#define I3UI_EDITBOX_STYLE_SCROLL_BUTTON_DISABLE	0x00100000	//스크롤바 PrevButton,NextButton을 Disable 합니다.

class I3_EXPORT_FRAMEWORK i3UIEditBox : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIEditBox, i3UICaptionControl );

public:
	// Text Scrolling 기능
	enum UI_BOX_SCROLL_STATE
	{
		UBSS_START = -1,
		UBSS_INIT,
		UBSS_FLOW,
		UBSS_END,
		UBSS_STOP,
	};

	enum UI_BOX_PROPERTY
	{
		UIBOX_NORMAL,
		UIBOX_PASSWORD,
		UIBOX_CHATINPUT,
		UIBOX_CHATVIEW,
		UIBOX_SCROLLING_LEFT,
		UIBOX_SCROLLING_RIGHT,
		UIBOX_SCROLLING_UP,
		UIBOX_SCROLLING_DOWN,
		UIBOX_NUMERIC,
	};

private:
	INT32					m_nCompPos = 0;		//중국어 조합중인 경우 Caret의 Offset

protected:
	// Persist
	I3UI_EDITBOX_STYLE		m_EditBoxStyle = 0;

	//ime
	UINT32					m_nImeStyle = 0;

	i3Ime			*		m_pIme = nullptr;
	IME_MODE				m_Imemode = IME_MODE_NATIVE;

	// Volatile
	bool					m_bUpdated = false;

	i3::wstring				m_wstrPasswordBuffer;

	// ScrollBar
	i3UIScrollBar*			m_pUIScrollBar = nullptr;
	REAL32					m_fUIScrollBarWidth = 20.0f;
	bool					m_bUseScroll = true;			// UIEditor에서 설정가능하도록
	INT32					m_nScrollStep = 3;

	UI_BOX_PROPERTY			m_BoxProperty = UIBOX_NORMAL;

	UI_BOX_SCROLL_STATE		m_ScrollState = UBSS_START;
	REAL32					m_CurrentOffset = 0.0f;
	REAL32					m_MoveOffset = 0.0f;
	REAL32                  m_MoveSpeed = 1.0f;
	
	INT32					m_TextStartPosX = 0;
	INT32					m_TextStartPosY = 0;

	bool					m_bUseCopyNPaste = true;
	bool					m_bContinueScrollPos = false;
	bool					m_bDisableScrollButton = false;

#if defined( I3_DEBUG)
	bool					m_bMultiLine = true;

	//Property Grid용으로만 사용. 필요한 경우 m_EditBoxStyle을 사용해야 혼란스럽지 않음.
	bool					m_bReadOnly = false;
#endif

	bool					m_bIsEllipsis = false;

protected:
	void					_ScrollToEnd( void);
	virtual void			_UpdateShapeState(void) override;
	void					_UpdateScrollBarRect( void);
	void					_UpdateScrollBarValue( void);
	void					_DragStart( INT32 nX, INT32 nY);
	void					_Dragging( INT32 nX, INT32 nY);
	void					_DragEnd( INT32 nX, INT32 nY);
	void					_loadEditInfo( void);

	void					_UpdateTextScroll( REAL32 rDeltaSeconds);

	void					ApplySetInputCharOnlyToIME();
	void					SetInputCharOnlyByEditBoxStyle(UINT32 editBoxStyle);

public:
	UI_BOX_SCROLL_STATE		getScrollState( void)						{ return m_ScrollState; }
	void					setScrollState( UI_BOX_SCROLL_STATE state)	{ m_ScrollState = state; }

	void					setScrollMoveSpeed( REAL32 speed)			{ m_MoveSpeed = speed;}
	REAL32					getScrollMoveOffset() const					{ return m_MoveOffset;}
	REAL32					getScrollCurrentOffset() const				{ return m_CurrentOffset;}


	i3UIScrollBar*			getScrollBar( void)							{ return m_pUIScrollBar; }

	void					setUseCopyNPaste( bool bUse )					{ m_bUseCopyNPaste = bUse; }
	void					setContinueScrollPos(bool bContinue)			{ m_bContinueScrollPos = bContinue;}
	void					setTextStartPosX(INT32 PosX)					{ m_TextStartPosX = PosX; }
	void					setTextStartPosY(INT32 PosY)					{ m_TextStartPosY = PosY; }
	void enableScrollBar(bool enable) { m_bUseScroll = enable; }

	void					setBoxProperty(UI_BOX_PROPERTY boxProperty)  { m_BoxProperty = boxProperty; };
	UI_BOX_PROPERTY			getBoxProperty(UI_BOX_PROPERTY boxProperty)  { return m_BoxProperty; };

public:
	i3UIEditBox();
	virtual ~i3UIEditBox();  

	virtual void			OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void			OnUpdateTextPos( void) override;

	virtual void			OnEnabled( bool bEnable) override;
	virtual bool			OnKillFocus( i3UIControl * pByCtrl = nullptr) override;
	virtual bool			OnSetFocus() override;

	virtual bool			GetTextRect( i3::pack::RECT* pRect) override;

	virtual void			OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void			OnMouseWheel( UINT32 nFlag, short zDelta, POINT point) override;
	virtual void			OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;

	virtual bool			OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void			OnSize( REAL32 cx, REAL32 cy) override;

	// revision 3518
	virtual bool			IsEllipsisEnable( void) override { return isEditBoxStyle( I3UI_EDITBOX_STYLE_ENABLE_ELLIPSIS);}
	void					SetEllipsis(bool bFlag);

	virtual i3UIControl*	OnSelect( void) override;

	I3UI_EDITBOX_STYLE		getEditBoxStyle(void)						{ return m_EditBoxStyle; }
	void					addEditBoxStyle( I3UI_EDITBOX_STYLE style)	{ m_EditBoxStyle |= style; }
	void					removeEditBoxStyle( I3UI_EDITBOX_STYLE style) { m_EditBoxStyle &= ~style; }
	bool					isEditBoxStyle( UINT32 mask)			{ return (m_EditBoxStyle & mask) != 0; }

	void					setPasswordEnable( bool bFlag)
	{ 
		if( bFlag)
		{
			m_EditBoxStyle |= I3UI_EDITBOX_STYLE_PASSWORD; 
		}
		else
		{
			m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_PASSWORD;

			i3::wstring().swap(m_wstrPasswordBuffer); //.swap(i3::wstring(L""));		// remove capicity..
		}
	}
	


	void					setChatEnable( bool bFlag )	{
		if(bFlag)	m_EditBoxStyle |= I3UI_EDITBOX_STYLE_CHAT;
		else		m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_CHAT;
	}

	void					setChatViewEnable( bool bFlag )	{
		if(bFlag)	m_EditBoxStyle |= I3UI_EDITBOX_STYLE_CHATVIEW;
		else		m_EditBoxStyle &= ~I3UI_EDITBOX_STYLE_CHATVIEW;
	}

	virtual void			SetText( const wchar_t * pText) override;
	virtual void			SetText( const i3::wliteral_range& wTextRng) override;
	virtual void			SetText( const i3::rc_wstring& wstrText) override;
	virtual void			SetText( const i3::wstring& wstrText) override;
	virtual void			SetText( const i3::stack_wstring& wstrText) override;


	i3Ime *					GetIME( void)						{ return m_pIme; }

	const WCHAR16*			getPassword(void) const				{ return m_wstrPasswordBuffer.c_str();}
	bool					getPasswordEnable(void)				{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_PASSWORD); }
	bool					getChatEnable( void)				{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_CHAT); }
	bool					getChatViewEnable( void)			{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_CHATVIEW); }

	void					setBorderVisible( bool bFlag);
	bool					isBorderVisible(void)				{ return !isEditBoxStyle( I3UI_EDITBOX_STYLE_NOBORDER); }
	
	// IME관련
	void					SetIMEText( const WCHAR16 * pText);
	const wchar_t *			getIMETextBuf(void)					{ return m_pIme->GetText(); }
	const i3::wstring&		getIMETextBufString()				{ return m_pIme->GetTextString(); }

	void					setAlphaNumericOnly( bool bSet)		{ m_Imemode = (bSet)? IME_MODE_ALPHANUMERIC : IME_MODE_NATIVE;}
	IME_MODE				GetIMEMode(void);
	void					SetIMEMode(IME_MODE mode = IME_MODE_ALPHANUMERIC);
	
	EDIT_STATE				getEditState(void);
	bool					isReadOnly(void)					{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_READONLY);			}
	void					SetReadOnly( bool bFlag);

	// 알파벳입력시 처리방법 caseType=0:대소문자입력상태(기본상태),  caseType=1:대문자로자동변환, caseType=2 : 소문자로 자동변환
	void					SetInputAlphabetAutoCase(INT32 caseType);
	void					SetInputCharOnlyAlphaNumeric();
	void					SetInputCharOnlyNumeric();
	bool					IsInputCharOnlyAlphaNumeric() { return isEditBoxStyle(I3UI_EDITBOX_STYLE_ALPHA_NUMERIC) 
																|| isEditBoxStyle(I3UI_EDITBOX_STYLE_ALPHA) ? true : false; }

	void					SetLetterLimit( INT32 limit);
	bool					isLimitByteSize(void)				{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_LIMIT_BYTESIZE);	}

	bool					isScrolling( void)					{ return isEditBoxStyle( I3UI_EDITBOX_STYLE_SCROLL)?true:false; }
	void					ScrollToEnd();

	void					OnKeyDownUp();
	void					OnKeyDownDown();
	void					OnKeyDownRight();
	void					OnKeyDownLeft();
	void					OnKeyDownHome();
	void					OnKeyDownEnd();
	void					OnKeyDownPageup();
	void					OnKeyDownPagedown();
	void					OnKeyDownReturn();
	void					OnKeyDownEscape();
	void					OnChar( bool bComp, bool keydown_event = false);
	void					RemoveColorKeyword(i3::wstring &);

	void					UpdateTextToIme();

	virtual UINT32			OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile ) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;
	virtual void			OnBindTemplate(void) override;

	static	void			RegisterLuaFunction(LuaState * pState);

#if defined( I3_DEBUG)
	virtual void			GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void			OnTDKPropertyChanged( const char* szName) override;
#endif

private:
	template<class StringType>
	void					SetTextImpl( const StringType& str);

};

#endif //__I3UI_EDITBOX_H__
