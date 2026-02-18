#ifndef __I3GUI_EDITBOX_H__
#define __I3GUI_EDITBOX_H__

#include "i3GuiControl.h"

/*
enum I3GUI_EDITBOX_STATE
{
	I3GUI_EDITBOX_NORMAL = 0,
	I3GUI_EDITBOX_CLICKED,
	I3GUI_EDITBOX_ONMOUSE,	
	I3GUI_EDITBOX_DISABLED,

	MAX_I3GUI_EDITBOX_STATE
};
*/

enum I3GUI_EDIT_NOTIFY_TYPE
{
	I3GUI_EDIT_NOTIFY_NULL = 0,
	I3GUI_EDIT_NOTIFY_CHANGE,
	I3GUI_EDIT_NOTIFY_UPDATE,
	I3GUI_EDIT_NOTIFY_KILLFOCUS,
	I3GUI_EDIT_NOTIFY_SETFOCUS,
	I3GUI_EDIT_NOTIFY_MAXTEXT,
	
	I3GUI_EDIT_NOTIFY_CLICKED,		// ? 일단은 배틀카니발 Lobby에서 Clicked를 받고 싶어함
	I3GUI_EDIT_NOTIFY_ONMOUSE,		// ? 일단은 배틀카니발 Room에서 OnMouse를 받고 싶어함
	I3GUI_EDIT_NOTIFY_RCLICK,
	I3GUI_EDIT_NOTIFY_DOUBLECLICK,
	
	MAX_I3GUI_EDIT_NOTIFY
};

// ============================================================================
//
// i3GuiEditBox
//
// ============================================================================
class I3_EXPORT_GUI i3GuiEditBox : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiEditBox , i3GuiControl);

protected:
	bool					m_bUpdated = false;
	bool					m_bUsePassward = false;
	
	REAL32					m_fOffsetPosX = 0.0f;
	REAL32					m_fOffsetPosY = 0.0f;

	i3TextFontSetInfo		m_FontSetInfo;
	INT32					m_nMaxTextCount = 0;
	UINT32					m_TextStyle = 0;
	bool					m_bShadowEnable = false;
	FONT_SHADOW_QUALITY		m_ShadowQuality = FONT_SHADOW_QUALITY_LOW;
	COLOR					m_TextColor = { 255, 255, 255, 255 };
	INT32					m_TextAlignX = 0;
	INT32					m_TextAlignY = 0;
	bool					m_bTextAutoWrap = false;
	bool					m_bTextWordLineFeed = false;
	INT32					m_nTextSpaceX = 0;
	INT32					m_nTextSpaceY = 0;
	bool					m_bEnableOutCharacterProcess = false;
	
	i3Sprite2D *			m_pCaretSprite = nullptr;

	REAL32					m_fCurGuiResolutionRateHeight = 0.0f;

#if defined (I3_WINDOWS)
	i3IME *					m_pIME = nullptr;
	HWND					m_hWnd = nullptr;
#endif

	// Caret관련 변수
	COLOR					m_Col = { 255, 255, 255, 255 };							// Caret의색상
	REAL32					m_CaretWidth = 0.0f;
	REAL32					m_CaretHeight = 0.0f;
	REAL32					m_CaretLocalTime = 0.0f;				// Caret의 연산에 쓰이는 로컬 타임
	REAL32					m_CaretInvisibleTime = 0.5f;			// Caret의 깜빡이는 간격타임
	REAL32					m_CaretPosOffsetX = 0.0f;
	REAL32					m_CaretPosOffsetY = 0.0f;

#if defined ( I3G_DX)
	i3TextNodeDX2*			m_pTextDX = nullptr;
#endif
	i3TextNode *			m_pText = nullptr;
	i3Font*					m_pFont = nullptr;
	INT32					m_nTextSize = 10;
	
	INT32					m_nEditOriginalWidth = 0;
	INT32					m_nEditOriginalHeight = 0;

	i3GuiControl*			m_pObserverControl = nullptr;

	i3::wstring				m_wstrTextTemp;

	static INT32			m_nDefaultFontSize;

public:
	i3GuiEditBox();
	virtual ~i3GuiEditBox();  

	virtual void			OnKillFocus(void) override;
	virtual void			UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void			OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	void					SetNotifyObserver(i3GuiControl* pControl);

#if defined( I3_WINDOWS)
	void					CreateText( i3Font* pFont, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
										bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					ReCreateText( i3Font* pFont, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
										bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					CreateTextEx(const i3TextFontSetInfo * pFontSet, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
		bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);
	void					CreateTextEx(const char* pszFont, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
										bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					ReCreateTextEx(const i3TextFontSetInfo * pFontSet, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
		bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);
	void					ReCreateTextEx(const char* pszFont, INT32 TextCount = 256, INT32 TextSize = -1, bool bIMEEnable = false, CREATE_IME_ELEMENT* pStruct = nullptr, UINT32 TextStyle = FW_NORMAL,
										bool bShadowEnable = false, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					setFontSkip( bool bFlag)				{ m_pTextDX->setFontSkip( bFlag);			}
#elif defined( I3_WINCE)
	void					CreateText( i3Font* pFont, INT32 TextCount = 256);
	void					ReCreateText( i3Font* pFont, INT32 TextCount = 256);
#endif
	void					SetText( const i3::wliteral_range& wTextRng, bool bParseCmd = true);
#if defined (I3G_DX)
	void					SetTextEllipsis(const i3::wliteral_range& wTextRng);
#endif
//	void					SetText( const i3::wliteral_range& wTextRng);
	void					SetTextColor( COLOR * pCol);
	void					SetTextAlign(INT32 AlignX, INT32 AlignY);
	void					SetTextRectSize(UINT32 nWidth, UINT32 nHeight);	
	void					SetOnlyEditOffsetPos( REAL32 fX, REAL32 fY);
	void					SetOnlyTextOffsetPos( REAL32 fX, REAL32 fY);
	void					SetOffsetPos( REAL32 fX, REAL32 fY);	
	void					SetTextAutoWrap( bool bEnable);
	void					SetWordLineFeed( bool bEnable);
	void					SetTextSpace( INT32 nX, INT32 nY);	
	void					SetOutCharaProcess(bool bFlag);
	const wchar_t*			GetText(void) const;
	
#if defined ( I3G_DX)
	i3TextNodeDX2 *			getTextNodeDX(void)						{ return m_pTextDX;													}
	void					setOriginalSize( INT32 nWidth, INT32 nHeight)	{ m_nEditOriginalWidth = nWidth; m_nEditOriginalHeight = nHeight;	}
#endif
	i3Font *				getFont(void)							{ return m_pFont;													}
	i3TextNode *			getTextNode(void)						{ return m_pText;													}
	INT32					getMaxTextCount(void)					{ return m_nMaxTextCount;											}

	void					SetAlphaNumericOnly( bool bForce = true);
	void					setPasswardEnable( bool bFalg)			{ m_bUsePassward = bFalg;											}
	
#if defined (I3_WINDOWS)
	// IME관련
	void					UpdateIMEInput( REAL32 rDeltaSeconds);		
	void					SetIMEText( const i3::wliteral_range& wTextRng);
	IME_MODE				GetIMEMode(void);
	void					SetIMEMode(IME_MODE mode = IME_MODE_ALPHANUMERIC);
	UINT32					getIMEStyle(void)						{ if( m_pIME) return m_pIME->getIMEStyle(); else return 0;		}

	const i3::wstring&		getIMETextBuf(void)	const				{ return m_pIME->getText();	}

//	INT32					getIMETextBufCount(void)				{ if( m_pIME) return m_pIME->getIMETextBufCount();		else return 0;			}	
	i3InputIME *			getIME(void)							{ if( m_pIME) return m_pIME->getIME();				else return nullptr;		}	
	void					InputDone(void)							{ if( m_pIME) m_pIME->InputDone();	}
	EDIT_STATE				getEditState(void);
	bool					useIME(void)							{ return (m_pIME != nullptr);			}

	// Caret관련
	void					CreateCaret(void);
	void					setCaretColor( COLOR* pCol)				{ i3Color::Copy( &m_Col, pCol);												}
	void					setCaretColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)	{ i3Color::Set( &m_Col, r, g, b, a);							}
	void					setCaretIntervalTime( REAL32 fTime)		{ m_CaretInvisibleTime = fTime;												}
	void					setCaretPosOffset( REAL32 x, REAL32 y)	{ m_CaretPosOffsetX = x;	m_CaretPosOffsetY = y;							}
	REAL32					GetCaretPosX();
	REAL32					GetCharacterSizeX( const i3::wliteral_range& wTextRng) const;
#endif

	virtual void			OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile ) override;

	//
	virtual bool			OnEvent(UINT32 nEvent, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	void					ResizeText(void);

	static void				SetDefaultFontSize(INT32 nSize);
	static INT32			GetDefaultFontSize(void);

protected:
	virtual bool			_ResizeControl(void);	
	UINT32					_CalcTextWeight(void);
};

#endif //__I3GUI_EDITBOX_H__
