#if !defined( __I3_UI_TEXT_H)
#define __I3_UI_TEXT_H

#include "i3UIFont.h"
#include "i3UILetter.h"


#define	I3UI_TEXT_STYLE_MULTILINE		0x00000001
#define I3UI_TEXT_STYLE_READONLY		0x00000002
#define	I3UI_TEXT_STYLE_AUTOLINEFEED	0x00000004
#define I3UI_TEXT_STYLE_OWN_BUFFER		0x00000008			//  ---> m_Style 이것 알고보니...여러군데서 리셋됩니다....이 플래그는 한번 기록되면 보관되어서 리셋될때 메모리 삭제해줘야됩니다....
															//  ---> 버퍼 메모리 관리 유무에 영향을 줍니다...플래그는 유지해보고, 어쨋거나 멤버 변수를 추가해서 병행해 쓰겠습니다....(11.07.01.수빈)

// text buffer에 있는 글자들의 위치를 기억하기 위함
// 메모리 할당/해제가 빈번히 일어나지 않도록 배열로 관리..
// 테스트 중
struct I3UI_TEXT_RECT
{
	INT32				left;
	INT32				top;
	INT32				right;
	INT32				bottom;
};


// Parse, SetText에서 글자를 출력하기 위한 정보
// 한 글자가 출력될때마다 Current값을 계속 업데이트 시켜주며 유지해야한다.
struct I3UI_TEXT_PARSE_CTX
{
	i3UILetter *	m_pTextTail;
	i3UILetter *	m_pFreePool;
	INT32			m_X;
	INT32			m_Y;
	UINT32			m_CurTextColor;

	bool			m_bQuit;
};

enum I3UI_TEXT_SHADOW
{
	I3UI_TEXT_SHADOW_NONE = 0,
	I3UI_TEXT_SHADOW_THIN,
	I3UI_TEXT_SHADOW_THICK,
};

enum I3UI_TEXT_ALIGN_X
{
	I3UI_TEXT_ALIGN_X_LEFT = 0,
	I3UI_TEXT_ALIGN_X_CENTER,
	I3UI_TEXT_ALIGN_X_RIGHT,
};

enum I3UI_TEXT_ALIGN_Y
{
	I3UI_TEXT_ALIGN_Y_TOP = 0,
	I3UI_TEXT_ALIGN_Y_MIDDLE,
	I3UI_TEXT_ALIGN_Y_BOTTOM,
};


// 에디트박스를 관리하기 위한 정보들을 관리하는 클래스
// 특히 화면상에 보여지는 애들
// Ex:
// m_pTextBuf: "This is a sample String"
// m_pText: ▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧▧
//          ↑
//      i3UILetter
class I3_EXPORT_FRAMEWORK i3UIText : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIText, i3ElementBase);
protected:
	// String Buffer ( malloc으로 직접 생성 또는 외부참조)
	WCHAR16 *		m_pTextBuf;			
	INT32			m_MaxLength;		

	i3UIFont *		m_pFont = nullptr;			
	i3Rect			m_Box;				// EditBox

	INT32			m_idxBeginBlock;
	INT32			m_idxEndBlock;

	INT32			m_idxCursor;		// 현재 커서의 String Buffer상의 Index
	WCHAR16			m_codeCursor;
	INT32			m_hCursorSprite;
	REAL32			m_timeCursor;
	i3UIGlyph *		m_pGlyph_Cursor;

	INT32			m_defTabWidth;
	INT32			m_defFontHeight;
	INT32			m_defFontWidth;

	i3UILetter*		m_pText;
	

	INT32			m_ShowX, m_ShowY;		// EditBox의 상대 좌표를 계산하기 위한 놈

	INT32			m_ExtentX, m_ExtentY;

	INT32			m_nLinePitch;


protected:
	void			Parse( const WCHAR16 * pText);

	bool			ParseCommand( I3UI_TEXT_PARSE_CTX * pCtx, const WCHAR16 * szCmd);

	i3UILetter *	_AllocLetter( I3UI_TEXT_PARSE_CTX * pCtx, bool bNeedSprite);
	i3UILetter *	AddLetter( I3UI_TEXT_PARSE_CTX * pCtx, i3UIGlyph * pGlyph, INT32 idx, bool bClip, i3Rect * pIntersect, bool bSelected);
	INT32			_EscapeSequence( I3UI_TEXT_PARSE_CTX* pCtx, const WCHAR16* pESeq, INT32 startIdx);

	void			_NormalLetter( I3UI_TEXT_PARSE_CTX * pCtx, const WCHAR16 * code, INT32 idx, INT32 Length);
	void			_WordWrap( I3UI_TEXT_PARSE_CTX * pCtx, INT32 idx, i3UIGlyph * pFirstGlyph);

	void			_FreeLetterList( i3UILetter * pNode);

	void			_TextAlignX( void);
	

	bool			isPrintableLetter( WCHAR16 * code);

	bool			IsWhiteCode( WCHAR16 c);
	INT32			StringTokenizer( WCHAR16* pszString, WCHAR16* pBuf);
	bool			IsInt( WCHAR16* pText);
	INT32			TextToInt( WCHAR16*	pText);
	//REAL32			TextToFloat( WCHAR16* pText);

	INT32			m_DefaultScrollAmountH;
	INT32			m_DefaultScrollAmountV;
public:
	i3UIText(void);
	virtual ~i3UIText(void);

	void		ResetText(void);
	
	void		SetFont( i3UIFont * pFont);
	i3UIFont*	getFont(void)							{ return m_pFont;}

	i3Rect	*	getView(void)							{ return &m_Box;}
	void		SetView( INT32 x, INT32 y, INT32 w, INT32 h);

	void		SetText( const WCHAR16 * pText, bool bCopy);	// Text가 수정될 때마다 호출해줘야 한다
	void		SetText( const WCHAR16 * pText);				// 이것 추가했습니다.. 이 동작은 기존에 보관된 스타일변수를 따라가게 하도록 합니다...

	WCHAR16 *	getText(void)							{ return m_pTextBuf;}

	INT32		getMaxLength( void)						{ return m_MaxLength;	}

	void		Reparse(void);

	void		BeginEdit(void);
	void		EndEdit(void);	
	
	// Caret 처리
	bool		isEditing(void);
	INT32		getCursorPos(void)						{ return m_idxCursor; }
	INT32		SetCursorPos( INT32 idx);		// 캐럿의 위치가 바뀔 때마다 호출해줘야 한다.
	
	void		SetCursorCode( WCHAR16 code);
	WCHAR16		getCursorCode(void)						{ return m_codeCursor; }

	/** \brief Caret의 Alpha를 설정*/
	void		SetCaretAlpha( REAL32 alpha);
	
	INT32		FindCharByPos( INT32 x, INT32 y);							// 상대 좌표로 해당 위치의 글자 Index 구하기.

	/** \brief y와 동일선상에 놓인 글자 중 가장 가까운 글자의 Index를 구한다.
		\param[in] bgHeight 배경의 높이값. 이값이 있으면 배경을 클릭시 찾아준다.	*/
	INT32		FindNearestCharByPos( INT32 x, INT32 y, INT32 bgHeight = 0);
	bool		FindLetterByIndex( INT32 idx, i3UILetter ** ppLetter);		// String Buffer의 Index로 i3UILetter 찾기
	bool		FindWholeLetterByIndex( INT32 idx, i3UILetter ** ppLetter);	// Same as FindLetterByIndex, but return false if clipped

	//
	//bool		ScrollLeft( INT32 nAmount = m_DefaultScrollAmountH);
	//bool		ScrollRight( INT32 nAmount = m_DefaultScrollAmountH);
	//bool		ScrollUp( INT32 nAmount = m_DefaultScrollAmountV);
	//bool		ScrollDown( INT32 nAmount = m_DefaultScrollAmountV);
	
public:
	bool		FindRectByIndex( INT32 idx, i3Rect* pRt);

	void		SetBlock( INT32 idxStart, INT32 idxEnd);

	INT32		getExtentWidth(void)					{ return m_ExtentX; }
	INT32		getExtentHeight(void)					{ return m_ExtentY; }

	INT32		getShowX(void)							{ return m_ShowX; }
	INT32		getShowY(void)							{ return m_ShowY; }

	INT32		SetShowPos( INT32 x, INT32 y);

	INT32		getDefTabWidth( void)					{ return m_defTabWidth;		}
	INT32		getDefFontHeight( void)					{ return m_defFontHeight;	}
	INT32		getDefFontWidth( void)					{ return m_defFontWidth;	}
	INT32		getEditBoxWidth( void)					{ return m_Box.getWidth();		}
	INT32		getEditBoxHeight( void)					{ return m_Box.getHeight();	}

	void		setVisible( bool bVisible = true);
	

	void		Dump(void);

	virtual INT32	AdjustSpriteOrder( INT32 hSpriteEnd);
	virtual void	AdjustLetters( void);


	// Text Style 관련
	// Align, Color, Shadow, Style
	I3UI_TEXT_ALIGN_X	m_AlignX;
	I3UI_TEXT_ALIGN_Y	m_AlignY;
	I3UI_TEXT_ALIGN_X	getAlignX( void)			{ return m_AlignX;	}
	I3UI_TEXT_ALIGN_Y	getAlignY( void)			{ return m_AlignY;	}
	void				SetAlign( I3UI_TEXT_ALIGN_X alignX, I3UI_TEXT_ALIGN_Y alignY)	{ m_AlignX = alignX;	m_AlignY = alignY;	}

	UINT32			m_DefColor;	
	UINT32			getDefColor( void)				{	return m_DefColor;	}
	void			SetDefColor( UINT32 color)		{	m_DefColor = color;	}

	UINT32			m_ShadowColor;
	UINT32			getShadowColor( void)			{	return m_ShadowColor;	}
	void			SetShadowColor( UINT32 color)	{	m_ShadowColor = color;	}
	
	I3UI_TEXT_SHADOW	m_ShadowType;
	I3UI_TEXT_SHADOW	getShadowType(void)				{ return m_ShadowType; }
	void				SetShadowType( I3UI_TEXT_SHADOW shadow);

	UINT32		m_Style = 0;
	bool		m_bUseBuffer = false;			// 나중에 이런 비슷한게 또 필요하면 멤버변수 이름 바꿉니다...

	UINT32		getStyle(void)							{ return m_Style; }
	void		setStyle( UINT32 style)					{ m_Style = style; }
	void		addStyle( UINT32 style)					{ m_Style |= style; }
	void		removeStyle( UINT32 style)				{ m_Style &= ~style; }
	bool		isStyle( UINT32 style)					{ return (m_Style & style) == style; }

	bool		isMultiLine(void)						{ return isStyle( I3UI_TEXT_STYLE_MULTILINE); }
	void		setMultiLine( bool bFlag)				{ bFlag ? addStyle( I3UI_TEXT_STYLE_MULTILINE) : removeStyle( I3UI_TEXT_STYLE_MULTILINE); }

	bool		isReadOnly(void)						{ return isStyle( I3UI_TEXT_STYLE_READONLY); }
	void		setReadOnly( bool bFlag)				{ bFlag ? addStyle( I3UI_TEXT_STYLE_READONLY) : removeStyle( I3UI_TEXT_STYLE_READONLY); }

	bool		isAutoLineFeed(void)					{ return isStyle( I3UI_TEXT_STYLE_AUTOLINEFEED); }
	void		setAutoLineFeed( bool bFlag)			{ bFlag ? addStyle( I3UI_TEXT_STYLE_AUTOLINEFEED) : removeStyle( I3UI_TEXT_STYLE_AUTOLINEFEED); }

	INT32		m_nLinePitch;
	INT32		getLinePitch(void)					{ return m_nLinePitch; }
	void		setLinePitch( INT32 pitch)			{ m_nLinePitch = pitch; }

	// space of letters
	INT32		m_nSpaceofLettersX;
	INT32		m_nSpaceofLettersY;

	INT32		getSpaceofLettersX(void)				{ return m_nSpaceofLettersX;}
	INT32		getSpaceofLettersY(void)				{ return m_nSpaceofLettersY;}
	void		SetSpaceofLetters( INT32 nX, INT32 nY);
};

#endif
