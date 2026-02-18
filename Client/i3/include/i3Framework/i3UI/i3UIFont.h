#if !defined( __I3_UI_FONT_H)
#define __I3_UI_FONT_H

#include "i3UIGlyph.h"


//Point = -(FontHeight * 72 / 96)
#define _HeightToPoint(a) -MulDiv( (a), 72, 96)

//FontHeight = -(Point * 96 / 72)
//MulDiv(m_FontSizePt, GetDeviceCaps(hDC, LOGPIXELSY), 72);
#define _PointToHeight(a) -MulDiv( (a), 96, 72)

class i3UIFontManager;

typedef UINT32 I3UI_FONT_STYLE;
#define		I3UI_FONT_STYLE_BOLD		0x00000001
#define		I3UI_FONT_STYLE_ITALIC		0x00000002
#define		I3UI_FONT_STYLE_UNDERLINE	0x00000004
#define		I3UI_FONT_STYLE_STRIKEOUT	0x00000008

enum I3UI_FONT_QUALITY
{
	I3UI_FONT_QUALITY_ANTIALIASED = 0,
	I3UI_FONT_QUALITY_CLEARTYPE,
	I3UI_FONT_QUALITY_PROOF,
};

struct I3UI_FONTINFO
{
	i3::rc_string		m_strName;
	INT32				m_Size = 10;
	I3UI_FONT_QUALITY	m_Quality = I3UI_FONT_QUALITY_ANTIALIASED;
	I3UI_FONT_STYLE		m_Style = 0;
	UINT8				m_CharaSet = DEFAULT_CHARSET;
};

// Font정보
// 해당 폰트로 생성된 글자들은 unordered_map으로 관리한다.
class I3_EXPORT_FRAMEWORK i3UIFont : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIFont, i3ElementBase);
public:

protected:
	I3UI_FONTINFO		m_Info;

	i3::unordered_map<WORD, i3UIGlyph*>	m_GlyphList;			// 해쉬테이블로 자료구조 변경..

	i3UIFontManager *	m_pCache = nullptr;
	UINT32				m_Hash = 0;
	INT32				m_ActualSize = 0;

#if defined( I3G_DX)
	HFONT				m_hFont = nullptr;		// 실제 폰트
	HFONT				m_hRenderFont = nullptr;	// 렌더링을 위한 폰트 실제폰트보다 크다.
#endif

protected:
	void				RemoveAllGlyph(void);

public:
	virtual ~i3UIFont(void);

	void				Create( i3UIFontManager * pCache, I3UI_FONTINFO * pInfo);
	void				Reset(void);
	void				RecreateRenderFont( void);

	UINT32				getHashCode(void)							{ return m_Hash; }
	void				setHashCode( UINT32 hash)					{ m_Hash = hash;	}

	i3UIGlyph *			AddGlyph( WORD wGlyph, bool* pOutExistOnVRAM);
	i3UIGlyph *			FindGlyph( WORD wGlyph);
	INT32				getGlyphCount(void) const					{ return (INT32)m_GlyphList.size(); }

#if defined( I3G_DX)
	HFONT				getFontHandle(void)							{ return m_hFont;}
	HFONT				getRenderFontHandle(void)					{ return m_hRenderFont;}
#endif

	static UINT32		CalcHashCode( I3UI_FONTINFO * pInfo);

	const char*			getFontName( void)							{ return m_Info.m_strName.c_str();	}
	I3UI_FONT_STYLE		getFontStyle(void)							{ return m_Info.m_Style; }
	I3UI_FONT_QUALITY	getFontQuality(void)						{ return m_Info.m_Quality; }
	INT32				getActualFontHeight( void)					{ return m_ActualSize; }
	void				setActualFontHeight( INT32 height)			{ m_ActualSize = height; }
	INT32				getOriginalFontHeight( void)				{ return m_Info.m_Size; }

	bool				isItalic( void)		{	return (m_Info.m_Style & I3UI_FONT_STYLE_ITALIC) == 0 ? false : true;	}
};

#endif

