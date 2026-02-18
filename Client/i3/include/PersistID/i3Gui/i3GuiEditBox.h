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
	I3_CLASS_DEFINE( i3GuiEditBox );

protected:
	BOOL					m_bUpdated;
	BOOL					m_bUsePassward;
	
	REAL32					m_fOffsetPosX;
	REAL32					m_fOffsetPosY;

	char					m_szFontName[256];
	INT32					m_nMaxTextCount;
	UINT32					m_TextStyle;
	BOOL					m_bShadowEnable;
	FONT_SHADOW_QUALITY		m_ShadowQuality;
	COLOR					m_TextColor;
	INT32					m_TextAlignX;
	INT32					m_TextAlignY;	
	BOOL					m_bTextAutoWrap;
	BOOL					m_bTextWordLineFeed;
	INT32					m_nTextSpaceX;
	INT32					m_nTextSpaceY;
	
	i3Sprite2D *			m_pCaretSprite;

	REAL32					m_fCurGuiResolutionRateHeight;

#if defined (I3_WINDOWS)
	i3IME *					m_pIME;
	HWND					m_hWnd;
#endif

	// Caret관련 변수
	COLOR					m_Col;							// Caret의색상
	REAL32					m_CaretWidth;
	REAL32					m_CaretHeight;
	REAL32					m_CaretLocalTime;				// Caret의 연산에 쓰이는 로컬 타임
	REAL32					m_CaretInvisibleTime;			// Caret의 깜빡이는 간격타임
	REAL32					m_CaretPosOffsetX;
	REAL32					m_CaretPosOffsetY;	

#if defined ( I3G_DX)
	i3TextNodeDX2*			m_pTextDX;
#endif
	i3TextNode *			m_pText;
	i3Font*					m_pFont;
	INT32					m_nTextSize;
	
	INT32					m_nEditOriginalWidth;
	INT32					m_nEditOriginalHeight;

	i3GuiControl*			m_pObserverControl;

public:
	i3GuiEditBox();
	virtual ~i3GuiEditBox();  

	virtual void			OnKillFocus(void);
	virtual void			UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void			OnUpdate( REAL32 rDeltaSeconds );
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	void					SetNotifyObserver(i3GuiControl* pControl);

#if defined( I3_WINDOWS)
	void					CreateText( i3Font* pFont, INT32 TextCount = 256, INT32 TextSize = 9, BOOL bIMEEnable = FALSE, CREATE_IME_ELEMENT* pStruct = NULL, UINT32 TextStyle = FW_NORMAL,
										BOOL bShadowEnable = FALSE, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);	

	void					ReCreateText( i3Font* pFont, INT32 TextCount = 256, INT32 TextSize = 9, BOOL bIMEEnable = FALSE, CREATE_IME_ELEMENT* pStruct = NULL, UINT32 TextStyle = FW_NORMAL,
										BOOL bShadowEnable = FALSE, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					CreateTextEx(const char* pszFont, INT32 TextCount = 256, INT32 TextSize = 10, BOOL bIMEEnable = FALSE, CREATE_IME_ELEMENT* pStruct = NULL, UINT32 TextStyle = FW_NORMAL,
										BOOL bShadowEnable = FALSE, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					ReCreateTextEx(const char* pszFont, INT32 TextCount = 256, INT32 TextSize = 10, BOOL bIMEEnable = FALSE, CREATE_IME_ELEMENT* pStruct = NULL, UINT32 TextStyle = FW_NORMAL,
										BOOL bShadowEnable = FALSE, FONT_SHADOW_QUALITY ShadowQuality = FONT_SHADOW_QUALITY_LOW);

	void					setFontSkip( BOOL bFlag)				{ m_pTextDX->setFontSkip( bFlag);			}
#elif defined( I3_WINCE)
	void					CreateText( i3Font* pFont, INT32 TextCount = 256);
	void					ReCreateText( i3Font* pFont, INT32 TextCount = 256);
#endif
	void					SetText( char * pText, bool bParseCmd = true);
#if defined (I3G_DX)
	void					SetTextEllipsis(char* pText);
#endif
	void					SetText( WCHAR16 * pText);
	void					SetTextColor( COLOR * pCol);
	void					SetTextAlign(INT32 AlignX, INT32 AlignY);
	void					SetTextRectSize(UINT32 nWidth, UINT32 nHeight);	
	void					SetOffsetPos( REAL32 fX, REAL32 fY);	
	void					SetTextAutoWrap( BOOL bEnable);
	void					SetWordLineFeed( BOOL bEnable);
	void					SetTextSpace( INT32 nX, INT32 nY);	
	char *					GetText(void);
	
#if defined ( I3G_DX)
	i3TextNodeDX2 *			getTextNodeDX(void)						{ return m_pTextDX;													}
	void					setOriginalSize( INT32 nWidth, INT32 nHeight)	{ m_nEditOriginalWidth = nWidth; m_nEditOriginalHeight = nHeight;	}
#endif
	i3Font *				getFont(void)							{ return m_pFont;													}
	i3TextNode *			getTextNode(void)						{ return m_pText;													}
	INT32					getMaxTextCount(void)					{ return m_nMaxTextCount;											}

	void					setPasswardEnable( BOOL bFalg)			{ m_bUsePassward = bFalg;											}
	
#if defined (I3_WINDOWS)
	// IME관련
	void					UpdateIMEInput( REAL32 rDeltaSeconds);		
	void					SetIMEText( char * pText);
	IME_MODE				GetIMEMode(void);
	void					SetIMEMode(IME_MODE mode = IME_MODE_ALPHANUMERIC);
	UINT32					getIMEStyle(void)						{ if( m_pIME) return m_pIME->getIMEStyle(); else return 0;		}
	void					setImeProcFlag( UINT32 flag)			{ if( m_pIME) m_pIME->setImeProcFlag(flag);									}
	char *					getIMETextBuf(void)						{ if( m_pIME) return m_pIME->getIMETextBuf();			else return NULL;		}
	INT32					getIMETextBufCount(void)				{ if( m_pIME) return m_pIME->getIMETextBufCount();		else return 0;			}	
	i3InputIME *			getIME(void)							{ if( m_pIME) return m_pIME->getIME();				else return NULL;		}	
	void					InputDone(void)							{ if( m_pIME) m_pIME->InputDone();	}
	EDIT_STATE				getEditState(void);
	BOOL					useIME(void)							{ return (m_pIME != NULL ? TRUE : FALSE);			}

	// Caret관련
	void					CreateCaret(void);
	void					setCaretColor( COLOR* pCol)				{ i3Color::Copy( &m_Col, pCol);												}
	void					setCaretColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)	{ i3Color::Set( &m_Col, r, g, b, a);							}
	void					setCaretIntervalTime( REAL32 fTime)		{ m_CaretInvisibleTime = fTime;												}
	void					setCaretPosOffset( REAL32 x, REAL32 y)	{ m_CaretPosOffsetX = x;	m_CaretPosOffsetY = y;							}
	REAL32					GetCaretPosX();
	REAL32					GetCharacterSizeX( char *pszText);
#endif

	virtual void			OnBuildObjectList( i3List * pList );
	virtual UINT32			OnSave( i3ResourceFile * pResFile );
	virtual UINT32			OnLoad( i3ResourceFile * pResFile );

	void					ResizeText(void);

protected:
	virtual BOOL			_ResizeControl(void);	
};

#endif //__I3GUI_EDITBOX_H__
