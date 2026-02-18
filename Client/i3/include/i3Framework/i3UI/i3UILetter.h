#if !defined( __I3_UI_LETTER_H)
#define __I3_UI_LETTER_H

#include "i3UIGlyph.h"

#define		I3UI_LETTER_STATE_MODIFIED		0x0001
#define		I3UI_LETTER_STATE_CLIP			0x0002

// 화면상에 표시되는 글자들의 정보( pos, idx, color, time 등)
// double linked list로 관리
// Code정보는 없음( i3UIGlyph로 확인)
// String Buffer(WCHAR[])가 아님!

struct SPRITE_INFO
{
	i3UIGlyph*		_pGlyph = nullptr;
	INT32			_hSprite = -1;
	INT32			_nPosX = 0;			//여러개의 Glyph가 Letter를 구성하는 경우에 필요
	INT32			_nOffsetX = 0;		//Glyph의 부분 조각만 이용하는 경우에 필요
	INT32			_nWidth = 0;		//Glyph의 부분 조각만 이용하는 경우에 필요
};

#define MAX_GLYPH_PER_LETTER 8

class I3_EXPORT_FRAMEWORK i3UILetter
{
protected:
	INT16			m_X = 0;			//Rendering Pos
	INT16			m_Y = 0;
	INT16			m_W = 0;
	INT16			m_H = 0;
	
	UINT16			m_State = 0;

	WCHAR16			m_Code = 0;

	INT32			m_nIdx = -1;			//String Buffer 상의 Index

	SPRITE_INFO		m_pGlyphInfo[MAX_GLYPH_PER_LETTER];		// Rendering 되는 glyph 정보
	INT32			m_nSpriteCount = 0;
	
	UINT32			m_Color = 0xFFFFFFFF;	
	REAL32			m_Time = 0.0f;

	i3UILetter *	m_pPrev = nullptr;		//Rendering Pos에 대한 Prev, Next
	i3UILetter *	m_pNext = nullptr;

	bool			m_bSoftBreak = false; 
	bool			m_bRightCaret = false;
	INT16			m_nLine = -1;

public:

	void reset(void)
	{
		m_X = m_Y = 0;
		m_W = m_H = 0;
		m_State = 0;
		m_nIdx = -1;
		
		i3mem::FillZero( m_pGlyphInfo, sizeof( SPRITE_INFO) * MAX_GLYPH_PER_LETTER);
		for( INT32 i=0; i< MAX_GLYPH_PER_LETTER; i++)
		{
			m_pGlyphInfo[i]._hSprite = -1;
		}
		m_nSpriteCount = 0;
		
		m_Color = 0xFFFFFFFF;
		m_Time = 0.0f;

		m_bSoftBreak = true;
		m_bRightCaret = false;
		m_nLine = -1;

		m_Code = 0;
	}

	UINT16		getState(void)				{ return m_State; }
	void		setState( UINT16 state)		{ m_State = state; }
	void		addState( UINT16 mask)		{ m_State |= mask; }
	void		removeState( UINT16 mask)	{ m_State &= ~mask; }
	bool		isState( UINT16 mask)		{ return (m_State & mask) == mask; }

	bool		isModified(void)			{ return isState( I3UI_LETTER_STATE_MODIFIED); }
	void		setModified(void)			{ addState( I3UI_LETTER_STATE_MODIFIED); }
	void		clearModified(void)			{ removeState( I3UI_LETTER_STATE_MODIFIED); }

	bool		isClipped( void)			{ return isState( I3UI_LETTER_STATE_CLIP);	}
	void		setClipped( bool bTrue)		{ if( bTrue) addState( I3UI_LETTER_STATE_CLIP); else removeState( I3UI_LETTER_STATE_CLIP);	}

	INT16		getPosX(void)					{ return m_X; }
	INT16		getPosY(void)					{ return m_Y; }
	void		setPos( INT16 x, INT16 y)		{ m_X = x;	m_Y = y;	setModified();	}

	INT16		getWidth( void)					{ return m_W;	}
	void		setWidth( INT16 w)				{ m_W = w;	setModified();	}
	
	INT16		getHeight( void)				{ return m_H;	}
	void		setHeight( INT16 h)				{ m_H = h;	setModified();	}

	REAL32		getLocalTime(void)			{ return m_Time; }
	void		setLocalTime( REAL32 tm)	{ m_Time = tm; }
	void		addLocalTime( REAL32 tm)	{ m_Time += tm; }

	UINT32		getColor(void)				{ return m_Color; }
	void		setColor( UINT32 c)
	{
		m_Color = c;
		setModified();
	}

	i3UILetter *	getPrev(void)					{	return m_pPrev;		}
	void			setPrev( i3UILetter * pPrev)	{	m_pPrev = pPrev;	}

	i3UILetter *	getNext(void)					{	return m_pNext;		}
	void			setNext( i3UILetter * pNext)	{	m_pNext = pNext;	}

	INT32			getIndex( void)			{ return m_nIdx;	}
	void			setIndex( INT32 idx)	{ m_nIdx = idx;		}

	INT32			getSpriteCount( void)	{	return m_nSpriteCount;	}
	void			setSpriteCount( INT32 nCount)	{	m_nSpriteCount = nCount;	}

	INT32			getSprite( INT32 nIdx)	
	{	
		I3_BOUNDCHK( nIdx, MAX_GLYPH_PER_LETTER);		
		return m_pGlyphInfo[nIdx]._hSprite;	
	}
	void			setSprite( INT32 nIdx, INT32 hSprite)	
	{	
		I3_BOUNDCHK( nIdx, MAX_GLYPH_PER_LETTER);		
		m_pGlyphInfo[nIdx]._hSprite = hSprite;
	}

	INT32			getGlyphCount( void)	{	return m_nSpriteCount;	}
	i3UIGlyph*		getGlyph( INT32 nIdx)	
	{	
		I3_BOUNDCHK( nIdx, m_nSpriteCount);		
		return m_pGlyphInfo[nIdx]._pGlyph;	
	}
	
	void			setGlyph( INT32 nIdx, i3UIGlyph* pGlyph, INT32 nPosX = 0, INT32 nOffsetX = 0, INT32 nWidth = 0)
	{
		I3_BOUNDCHK( nIdx, m_nSpriteCount);		
		m_pGlyphInfo[nIdx]._pGlyph = pGlyph;
		m_pGlyphInfo[nIdx]._nPosX = nPosX;
		m_pGlyphInfo[nIdx]._nOffsetX = nOffsetX;
		m_pGlyphInfo[nIdx]._nWidth = nWidth;
	}

	INT32			getSpritePosX( INT32 nIdx)
	{
		I3_BOUNDCHK( nIdx, m_nSpriteCount);	
		return m_pGlyphInfo[nIdx]._nPosX;
	}

	INT32			getGlyphOffsetX( INT32 nIdx)
	{
		I3_BOUNDCHK( nIdx, m_nSpriteCount);	
		return m_pGlyphInfo[nIdx]._nOffsetX;
	}

	INT32			getGlyphWidth( INT32 nIdx)
	{
		I3_BOUNDCHK( nIdx, m_nSpriteCount);	
		return m_pGlyphInfo[nIdx]._nWidth;
	}

	INT16			getLine( void)			{	return m_nLine;	}
	void			setLine( INT16 nLine)	{	m_nLine = nLine;	}

	bool			isRightCaret( void)			{	return m_bRightCaret;	}
	void			setRightCaret( bool bRight)	{	m_bRightCaret = bRight;	}

	bool			isSoftBreak( void)				{	return m_bSoftBreak;	}
	void			setSoftBreak( bool bSoftBreak)	{	m_bSoftBreak = bSoftBreak;	}

	void			setCode( CHAR16 code)	{	m_Code = code;	}
	CHAR16			getCode( void)			{	return m_Code;	}
};

#endif
