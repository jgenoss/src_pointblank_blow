#if !defined( __I3UI_GLYPH_H)
#define __I3UI_GLYPH_H

#include "usp10.h"

#define	I3UI_GLYPH_STATE_LOADED		0x00000001

// VRAM에 올라가 있는 글자들에 대한 정보
// Code, Memory 상의 Rect
class I3_EXPORT_FRAMEWORK i3UIGlyph
{
protected:
	WORD		m_wGlyph = 0;		//id to retrieve from the texture
	ABC			m_abc = { 0, 0, 0 };

	INT16		m_W = 0;			// Rendering 되는 Width, Height
	INT16		m_H = 0;
	INT16		m_X = -1;			// VRAM상의 좌표 (-1 : VRAM에 올라가 있지 않다.)
	INT16		m_Y = -1;

	INT16		m_RealWidth = 0;	// 실제 Width값
	INT16		m_RealHeight = 0;	// 실제 Height값

public:
	i3UIGlyph() {}

	INT32		getPosX(void)				{ return static_cast<INT32>(m_X); }
	void		setPosX( INT32 x)			{ m_X = static_cast<INT16>(x); }

	INT32		getPosY(void)				{ return static_cast<INT32>(m_Y); }
	void		setPosY( INT32 y)			{ m_Y = static_cast<INT16>(y); }

	INT32		getWidth(void)				{ return static_cast<INT32>(m_W); }
	void		setWidth( INT32 w)			{ m_W = static_cast<INT16>(w); }
	
	INT32		getHeight(void)				{ return static_cast<INT32>(m_H); }
	void		setHeight( INT32 h)			{ m_H = static_cast<INT16>(h); }

	void		setGlyphCode( WORD wGlyph)	{	m_wGlyph = wGlyph;	}
	WORD		getGlyphCode( void)			{	return m_wGlyph;	}

	INT32		getABC( ABC* pOutABC)		{	*pOutABC = m_abc;	return m_abc.abcA + m_abc.abcB + m_abc.abcC;	}
	void		setABC( const ABC* pABC)	{	m_abc = *pABC;	}

	INT32		getRealWidth( void)			{ return static_cast<INT32>(m_RealWidth); }
	void		setRealWidth( INT32 w)		{ m_RealWidth = static_cast<INT16>(w); }

	INT32		getRealHeight( void)		{ return static_cast<INT32>(m_RealHeight); }
	void		setRealHeight( INT32 h)		{ m_RealHeight = static_cast<INT16>(h); }
};

#endif
