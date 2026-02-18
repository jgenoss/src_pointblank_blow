#pragma once

//
#include "../i3UILetter.h"
#include "../i3UIFont.h"

#include "i3Base/pool/class_storage_pool.h"

struct SCRIPT_RUN;

// ======= From i3UIText =======
struct I3UI_TEXT_PARSE_CTX
{
	i3UILetter *	m_pTextTail = nullptr;
	i3UILetter *	m_pFreePool = nullptr;
	INT32			m_X = 0;
	INT32			m_Y = 0;
	UINT32			m_CurTextColor = 0;
};
struct SCRIPTSENTENCE;

typedef UINT32 I3UI_TEXT_STYLE;
#define	I3UI_TEXT_STYLE_MULTILINE		0x00000001
#define I3UI_TEXT_STYLE_READONLY		0x00000002
#define	I3UI_TEXT_STYLE_AUTOLINEFEED	0x00000004
#define I3UI_TEXT_STYLE_OWN_BUFFER		0x00000008
#define I3UI_TEXT_STYLE_SCROLLINGHORZ	0x00000010		///< Horizon Scroll 기능 사용
#define I3UI_TEXT_STYLE_LINFEEDWITHWORD	0x00000020		///< LineFeed를 Word단위로 한다.

#define MAXCOMPBOX 100

enum I3UI_TEXT_SHADOW
{
	I3UI_TEXT_SHADOW_NONE = 0,
	I3UI_TEXT_SHADOW_THIN,
	I3UI_TEXT_SHADOW_THICK,
};

enum I3UI_TEXT_ALIGN_X
{
	I3UI_TEXT_ALIGN_X_INVALID = -1,

	I3UI_TEXT_ALIGN_X_LEFT = 0,
	I3UI_TEXT_ALIGN_X_CENTER,
	I3UI_TEXT_ALIGN_X_RIGHT,
};

enum I3UI_TEXT_ALIGN_Y
{
	I3UI_TEXT_ALIGN_Y_INVALID = -1, 

	I3UI_TEXT_ALIGN_Y_TOP = 0,
	I3UI_TEXT_ALIGN_Y_MIDDLE,
	I3UI_TEXT_ALIGN_Y_BOTTOM,
};

class i3UIRenderer;



struct LINE_INFO : i3::class_storage_pool<LINE_INFO>
{
	i3UIFont*				m_pFont = nullptr;
	i3::rc_wstring			m_strLine;
	i3::vector<SCRIPT_RUN*>	m_RunList;
	i3::vector<i3UIGlyph*>	m_GlyphList;
	INT32					m_numLineFeed = 0;
};

namespace i3
{
	template<> struct reserve_storage_pool<LINE_INFO> {  enum { value = 4 };  };
}

class I3_EXPORT_FRAMEWORK i3UIText : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIText, i3ElementBase);

	friend class i3UICaptionControl;
private:

	i3UILetter*		m_pFirstLetter = nullptr;

	i3UIFont*			m_pFont = nullptr;			//Default Font

	// ======= Text Style =======
	I3UI_TEXT_ALIGN_X	m_AlignX = I3UI_TEXT_ALIGN_X_INVALID;
	I3UI_TEXT_ALIGN_Y	m_AlignY = I3UI_TEXT_ALIGN_Y_INVALID;
	UINT32				m_DefColor = 0xFFFFFFFF;
	UINT32				m_ShadowColor;				// [initialize at constructor]
	I3UI_TEXT_SHADOW	m_ShadowType = I3UI_TEXT_SHADOW_NONE;
	I3UI_TEXT_STYLE		m_Style = 0;
	
//  문자열 공간을 2개로 만들고 선택하게끔 고친다..
	i3::rc_wstring		m_rc_string_storage;
	i3::wstring			m_string_storage;

	i3::const_wchar_range	m_wstrRange;				// 실제로는 이것으로 처리한다..

	i3::wstring			m_strRefinedText;			// 이 3개의 멤버변수들은 칼라키워드가 존재하는 경우에만 작동된다..
	i3::vector<DWORD>	m_RefinedColorTable;
	i3::vector<DWORD>	m_RefinedColorIdxList;
	i3::vector<size_t>	m_RefinedtoSrcMapper;
	DWORD				m_UserColorTableBeginIndex = 0;

	i3Rect			m_rtView;			//View rectangle (m_Box)
	i3Rect			m_rtText;			//Text rectangle to align text. (left and top values must be 0)
										//This can be the same size of m_rtView, or can be set differently to allow scrolling (m_ExtentX, m_ExtentY) 

	INT32			m_ShowX = 0, m_ShowY = 0;			//Needed to implement scrolls
	INT32			m_ExtentX = 0, m_ExtentY = 0;
	INT32			m_nSpaceofLettersX = 0, m_nSpaceofLettersY = 0;

	INT32			m_nDefFontHeight = 10;
	INT32			m_nDefTabWidth = 20;
	i3Rect			m_rtScissors;

	bool			m_bEnable = true;		// 일단 보이는 것으로 간주한다..

	bool			m_bEscapeSequence = true;	//Escape Sequence를 Parsing할 것인지
	bool			m_bContainEscSeq = false;	//Escape Sequence를 포함하고 있는지
										// 설정은 내부에서만 사용한다.

	i3::vector<SCRIPT_RUN*>	m_RunList;			//SCRIPT_RUN*
	i3::vector<i3UIGlyph*>	m_GlyphList;

	i3::vector<LINE_INFO*> m_LineInfoList;	// List포인터는 일반벡터로 처리...(포인터로 해야할 근거가 없음..)

	INT32			m_nLinePitch = 0;

protected:
	
	void			SetEnable(bool bEnable);
	bool			GetEnable() const { return m_bEnable; }

	i3UILetter *	_AllocLetter( I3UI_TEXT_PARSE_CTX * pCtx, INT32 nSpriteCount);
	void			_AllocSprite( i3UILetter* pLetter, INT32 i);
	void			_FreeSprite( i3UILetter* pLetter, INT32 i);

	void			_FreeSprite( void);

	bool			_IsInt( WCHAR16* pText);
	INT32			_TextToInt( WCHAR16* pText);

	void			_GenGlyphIndexTable( const SCRIPT_RUN* pRun, const i3::vector<i3UIGlyph*>& GlyphList, INT32 nTotalGlyphs, 
										INT32 nCurX, INT32 nWidth, INT32* pOutGlyphIndex, bool* pOutLineBreak);


	LINE_INFO*		_FindLineInfo( const wchar_t* pszStr, INT32 nLen);
	void			_MoveLineInfo( LINE_INFO* pInfo, i3::vector<LINE_INFO*>& To);
	void			_FreeLineInfo( LINE_INFO* pInfo);
	void			_ClearLineInfoList( void);

public:
	i3UIText();
	virtual ~i3UIText();

	i3::vector<SCRIPT_RUN*>&	getRunList( void)	{	return m_RunList;	}
	i3::vector<i3UIGlyph*>&		getGlyphList( void)	{	return m_GlyphList;	}

	void		EnableEscapeSequence( bool bEnable)	{	m_bEscapeSequence = bEnable;	}

	UINT32		getFontColor( INT32 nCharIdx);

	void		ResetText(void);


	void			SetText( const i3::rc_wstring& rcText);
	void			SetText( const i3::wstring& wstrText);
	void			SetText( const i3::wliteral_range& wTextRng);
	void			SetText( const wchar_t* wszText);

	void			SwapText( i3::wstring& tmpText);

	const wchar_t*		getText( void)	const			{	return m_wstrRange.begin();	}
	std::size_t			getTextLength() const			{ return m_wstrRange.size(); }
	
	const i3::const_wchar_range& getTextRange() const		{ return m_wstrRange; }

	const i3::wstring&			 getRefinedText() const		{ return m_strRefinedText; }

	void		SplitStrings(const wchar_t* str);

	void		CreateText( void);		//Lay out (Using Uniscribe)
	void		LayOutText( void);		
	void		RenderText( void);		//Display
	void		RedrawGlyphs( void);	//VRAM

	//Text area
	i3Rect*				getTextRect( void)			{	return &m_rtText;	}

	//View area
	i3Rect*				getViewRect(void)			{	return &m_rtView;	}
	void				SetViewRect( INT32 x, INT32 y, INT32 w, INT32 h);
	void				SetScissorsRect(const i3Rect& rt);
	INT32				getViewWidth( void)			{	return m_rtView.getWidth();		}		//getEditBoxWidth()
	INT32				getViewHeight( void)		{	return m_rtView.getHeight();	}		//getEditBoxHeight()
	
	void				setVisible( bool bVisible);

	i3UILetter*			getFirstLetter( void)		{	return m_pFirstLetter;	}
	void				setFirstLetterNULL( void)	{	m_pFirstLetter = nullptr;	}
	i3UILetter*			AddLetter( I3UI_TEXT_PARSE_CTX * pCtx, SPRITE_INFO* pInfo, INT32 nGlyphs);
	i3UILetter*			AddNullLetter( I3UI_TEXT_PARSE_CTX * pCtx);
	void				FreeLetterList( i3UILetter * pNode);
	
	void			SetShowPos( INT32 l, INT32 t);
	INT32			getShowX( void)					{	return m_ShowX;		}
	INT32			getShowY( void)					{	return m_ShowY;		}

	INT32			AdjustSpriteOrder( INT32 idx);

	void			CopyStyles( i3UIText* pDest);
	
	//temporary
	INT32			getExtentWidth( void)		{	return m_ExtentX;		}
	INT32			getExtentHeight( void)		{	return m_ExtentY;		}

	INT32			getDefFontHeight( void)		{	return m_nDefFontHeight;	}
	void			setDefFontHeight( INT32 h)	{	m_nDefFontHeight = h;	}
	
	void			setDefTabWidth( INT32 w)	{	m_nDefTabWidth = w;		}
	INT32			getDefTabWidth( void)		{	return m_nDefTabWidth;		}
	
	bool			BuildSrcStringIntoRefinedStringMapper( i3::vector<size_t>& out) const;
	DWORD			AddPartialTextColor( I3COLOR clr );		// 리턴값은 색상테이블 인덱스이다.. // SetText시 이전것은 리셋되버리므로 주의.
	void			ModifyPartialTextColor(DWORD clrIndex, I3COLOR clr);

	void			ModifyRefinedPartialTextColor( size_t refinedTextIndex, DWORD clrIndex);	// 
	bool			IsPartialTextColorApplied() const;
	
	// ======= Editing Text =======
private:
	WCHAR16			m_wcCodeCaret = 0;			//Caret이 가리키는 Charcter 필요??
	INT32			m_hSpriteCaret = -1;		//Caret의 Sprite
	i3UIGlyph *		m_pGlyphCaret = nullptr;

	INT32			m_hSpriteCompBox[MAXCOMPBOX];	// [initialize at constructor]

	INT32			m_nIdxBeginBlock = -1;
	INT32			m_nIdxEndBlock = -1;

	i3UILetter*		m_pCurLetter = nullptr;
	INT32			m_nCurIdx = -1;
	INT32			m_nMaxLine = 0;

	i3::vector<SCRIPTSENTENCE*> m_vecScriptSentence;
	bool m_ReCreateUniscribe = false;
public:

	INT32			getNumCarriageLine() const			{ return (INT32)m_LineInfoList.size(); }
	INT32			getNumLineFeedOfCarriageLine(INT32 carrage_line_idx) const { return m_LineInfoList[carrage_line_idx]->m_numLineFeed; }

	WCHAR16			getCaretCode(void)						{ return m_wcCodeCaret; }
	void			SetCaretCode( INT32 nLetterIdx, int nendindex = 0);
	
	void			SetBlock( INT32 idxStart, INT32 idxEnd);
	
	void			UpdateCaretPos(INT32 nLetterIdx = 0, INT32 nSpriteIndex = 0);
	void			SetCaretPos( INT32 nIdx);
	INT32			getCaretPos(void)						{ return m_nCurIdx;	}	

	i3UILetter*		getCurLetter( void)					{	return m_pCurLetter;	}
	void			setCurLetter( i3UILetter* pLetter)	{	m_pCurLetter = pLetter;		m_nCurIdx = pLetter ? pLetter->getIndex() : -1;	}
	void			setMaxLine( INT32 nMaxLine)			{	m_nMaxLine = nMaxLine;	}
	INT32			getMaxLine() const					{ return m_nMaxLine; }

	void			SetCaretAlpha( REAL32 alpha);

	void			BeginEdit( void);
	void			EndEdit( void);
	bool			isEditing(void);
	void			CompBoxDisable();

	bool			isLetterVisible( i3UILetter* pLetter);

	void			ScrollToCaret( void);

	INT32			FindNearestLetterByPos( INT32 x, INT32 y, INT32 bgHeight = 0 /*?*/);
	bool			FindLetterByIndex( INT32 idx, i3UILetter ** ppLetter);

	i3UILetter*		GetLastLetter( void);

	INT32			OnKeyDownLeft( void);
	INT32			OnKeyDownRight( void);
	INT32			OnKeyDownHome( void);
	INT32			OnKeyDownEnd( void);
	
	INT32			OnKeyDownUp( void);
	INT32			OnKeyDownDown( void);
	INT32			OnKeyDownPageUp( void);
	INT32			OnKeyDownPageDown( void);

	// ======= Text Style ======= 
	// Font, Color, Align, Shadow, Style
	void				SetFont( i3UIFont * pFont);
	i3UIFont*			getFont(void)						{ return m_pFont;}
	
	I3UI_TEXT_ALIGN_X	getAlignX( void)			{ return m_AlignX;	}
	I3UI_TEXT_ALIGN_Y	getAlignY( void)			{ return m_AlignY;	}
	void				SetAlign( I3UI_TEXT_ALIGN_X alignX, I3UI_TEXT_ALIGN_Y alignY);	

	UINT32				getDefColor( void)				{	return m_DefColor;	}
	void				SetDefColor( UINT32 color);
	
	UINT32				getShadowColor( void)			{	return m_ShadowColor;	}
	void				SetShadowColor( UINT32 color)	{	m_ShadowColor = color;	}
	
	I3UI_TEXT_SHADOW	getShadowType(void)				{ return m_ShadowType; }
	void				SetShadowType( I3UI_TEXT_SHADOW shadow);

	I3UI_TEXT_STYLE		getTextStyle(void)						{ return m_Style; }
	void				setTextStyle( I3UI_TEXT_STYLE style);
	void				addTextStyle( I3UI_TEXT_STYLE style);
	void				removeTextStyle( I3UI_TEXT_STYLE style);
	bool				isTextStyle( I3UI_TEXT_STYLE style)		{ return (m_Style & style) == style; }
	
	bool				isMultiLine(void)						{ return isTextStyle( I3UI_TEXT_STYLE_MULTILINE); }
	void				setMultiLine( bool bFlag)				{ bFlag ? addTextStyle( I3UI_TEXT_STYLE_MULTILINE) : removeTextStyle( I3UI_TEXT_STYLE_MULTILINE); }

	bool				isReadOnly(void)						{ return isTextStyle( I3UI_TEXT_STYLE_READONLY); }
	void				setReadOnly( bool bFlag)				{ bFlag ? addTextStyle( I3UI_TEXT_STYLE_READONLY) : removeTextStyle( I3UI_TEXT_STYLE_READONLY); }

	INT32				getLinePitch(void)					{ return m_nLinePitch; }
	void				setLinePitch( INT32 pitch)			{ m_nLinePitch = pitch; }

	bool				isAutoLineFeed(void)					{ return isTextStyle( I3UI_TEXT_STYLE_AUTOLINEFEED); }
	void				setAutoLineFeed( bool bFlag)			
	{ 
		//if( m_AlignX != I3UI_TEXT_ALIGN_X_LEFT)		bFlag = true;	// Left Align이 아닌 경우에는 무조건 autolinefeed가 설정되도록 해야한다. 
		bFlag ? addTextStyle( I3UI_TEXT_STYLE_AUTOLINEFEED) : removeTextStyle( I3UI_TEXT_STYLE_AUTOLINEFEED); 
	}

	bool				isScrollingHorizon( void)				{ return isTextStyle( I3UI_TEXT_STYLE_SCROLLINGHORZ); }
	void				setScrollingHorizon( bool bFlag)		{ bFlag ? addTextStyle( I3UI_TEXT_STYLE_SCROLLINGHORZ) : removeTextStyle( I3UI_TEXT_STYLE_SCROLLINGHORZ); }


	//Runs & Letters
	INT32		_GetAccCharacterCount( INT32 lineIdx, INT32 logIdx);

	void		_AlignLettersX( INT32 width);
	void		_AlignLettersY( INT32 height);

	INT32		getSpaceofLettersX(void)				{ return m_nSpaceofLettersX;}
	INT32		getSpaceofLettersY(void)				{ return m_nSpaceofLettersY;}
	void		SetSpaceofLetters( INT32 nX, INT32 nY);

	virtual void	OnDraw( i3UIRenderer * pRenderer);

	bool			ContainsEscapeSequence( void)		{ return m_bContainEscSeq;	}
	INT32			ConvertIndexToRawString( INT32 idxRefined);

	void		OnRecreateUniscribe() { m_ReCreateUniscribe = true;  }	//uniscribe로 다시 생성
	bool		IsReCreateUniscribe() { return m_ReCreateUniscribe; }
	void		clearScriptSecntence();
	void		addScritSentence(SCRIPTSENTENCE* psentence) { m_vecScriptSentence.push_back(psentence); }
};

