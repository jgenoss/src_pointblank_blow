#if !defined( __I3_GFX_FONT_DX2_H)
#define __I3_GFX_FONT_DX2_H

#include "i3GfxResource.h"

#define	FONT_TEXTURE_WIDTH		256
#define	FONT_TEXTURE_HEIGHT		256

typedef struct
{
	INT16	nGlyphWidth;
	INT16	nGlyphHeight;
	INT16	nStartX;
	INT16	nStartY;
	INT16	nTextureIdx;
	WCHAR16	Char;
}DX2_FONT_INFO;

#define		CHAR_FLAG_WORDBREAK		0x00000001
#define		CHAR_FLAG_LINEBREAK		0x00000002
#define		CHAR_FLAG_SETUP			0x80000000
#define		CHAR_FLAG_INVISIBLE		0x40000000

typedef struct
{
	UINT32				m_Flag;
	DX2_FONT_INFO *		m_pGlyph;
	COLOR				color;
	INT32				x;
	INT32				y;
} DISP_CHAR_INFO;

class I3_EXPORT_GFX i3GfxFontDX2 : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxFontDX2);

protected:
	LOGFONT			m_LogFont;
	HFONT			m_hFont;

	i3Texture *		m_pDrawTex;

	i3List			m_TexList;
	i3BinList		m_GlyphList;

	INT32			m_LastX;
	INT32			m_LastY;
	INT32			m_SpaceWidth;

	HDC				m_hDC;

protected:
	void			_removeAllTexture(void);
	void			_removeAllGlyph(void);
	void			_addTexture(void);
	i3Texture *		_getLastTexture(void)						{ return (i3Texture *) m_TexList.Items[ m_TexList.GetCount() - 1]; }
	void			_CopyFontImage( i3Texture * pTex, INT32 x, INT32 y, INT32 cx, INT32 cy);

public:
	i3GfxFontDX2(void);
	virtual ~i3GfxFontDX2(void);

	INT32			getGlyphHeight(void)						{ return m_LogFont.lfHeight; }
	DX2_FONT_INFO *	findGlyph( WCHAR16 ch);
	DX2_FONT_INFO *	getGlyph( WCHAR16 ch);

	void			BeginEdit(void);
	void			EndEdit(void);

	void			CreateFont( LOGFONT* pLogFont);

	INT32			getTextureCount(void)						{ return m_TexList.GetCount(); }
	i3Texture *		getTexture( INT32 idx)						{ return (i3Texture *) m_TexList.Items[idx]; }

	INT32			getDefaultSpaceWidth(void)					{ return m_SpaceWidth; }

	bool			isSame( LOGFONT * pFontInfo);

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};

class I3_EXPORT_GFX i3GfxFontDX2Manager : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GfxFontDX2Manager);

protected:
	i3List			m_FontList;

public:
	i3GfxFontDX2Manager(void);
	virtual ~i3GfxFontDX2Manager(void);

	INT32			getFontCount(void)					{ return m_FontList.GetCount(); }
	i3GfxFontDX2 *	getFont( INT32 idx)					{ return (i3GfxFontDX2 *) m_FontList.Items[idx]; }
	void			addFont( i3GfxFontDX2 * pFont);
	void			removeFont( i3GfxFontDX2 * pFont);
	i3GfxFontDX2 *	findFont( LOGFONT * pFontInfo);

	void			ReleaseAllFont(void);
	void			ReleaseUnusedFont(void);

	static i3GfxFontDX2 *	CreateFont( LOGFONT * pFontInfo);
	static void				ReleaseFont( i3GfxFontDX2 * pFont);
};


#endif	// __I3_GFX_FONT_DX2_H
