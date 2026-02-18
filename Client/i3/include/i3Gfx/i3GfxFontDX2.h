#if !defined( __I3_GFX_FONT_DX2_H)
#define __I3_GFX_FONT_DX2_H

#include "i3GfxResource.h"

#define	FONT_TEXTURE_WIDTH		256
#define	FONT_TEXTURE_HEIGHT		256

struct DX2_FONT_INFO
{
	INT16	nGlyphWidth = 0;
	INT16	nGlyphHeight = 0;
	INT16	nStartX = 0;
	INT16	nStartY = 0;
	INT16	nTextureIdx = 0;
	INT16	nCharLength = 0;
	wchar_t	wChar[2] = { 0 };			//	한 문자가 가변 바이트의 크기인 최대 4바이트로 사용합니다.

};

#define		CHAR_FLAG_WORDBREAK		0x00000001
#define		CHAR_FLAG_LINEBREAK		0x00000002
#define		CHAR_FLAG_SETUP			0x80000000
#define		CHAR_FLAG_INVISIBLE		0x40000000

struct DISP_CHAR_INFO
{
	UINT32				m_Flag = 0;
	DX2_FONT_INFO *		m_pGlyph = nullptr;
	COLOR				color;
	INT32				x = 0;
	INT32				y = 0;
} ;

class I3_EXPORT_GFX i3GfxFontDX2 : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3GfxFontDX2, i3GfxResource);

protected:
	LOGFONTA			m_LogFont;
	HFONT			m_hFont = nullptr;

	i3Texture *		m_pDrawTex = nullptr;

	i3::vector<i3Texture*>	m_TexList;
	i3::unordered_map< UINT32, DX2_FONT_INFO*> m_GlyphList;

	INT32			m_LastX = 0;
	INT32			m_LastY = 0;
	INT32			m_SpaceWidth = 3;

	HDC				m_hDC = nullptr;

protected:
	void			_removeAllTexture(void);
	void			_removeAllGlyph(void);
	void			_addTexture(void);
	i3Texture *		_getLastTexture(void)						{ return m_TexList.back(); }
	void			_CopyFontImage( i3Texture * pTex, INT32 x, INT32 y, INT32 cx, INT32 cy);

	void			_DrawFontToDrawTex( DX2_FONT_INFO * pGlyph, SIZE * pSz);

public:
	i3GfxFontDX2(void);
	virtual ~i3GfxFontDX2(void);

	LOGFONTA	*		getLogFont(void)								{ return &m_LogFont;}
	HFONT			getHFont(void)									{ return m_hFont;}
	INT32			getGlyphHeight(void)						{ return m_LogFont.lfHeight; }

	DX2_FONT_INFO *	findGlyph( const wchar_t* ch, INT32 letter_len);
	DX2_FONT_INFO *	getGlyph( const wchar_t* ch, INT32 letter_len);

	void			BeginEdit(void);
	void			EndEdit(void);

	void			CreateFont( LOGFONTA* pLogFont);

	INT32			getTextureCount(void)						{ return (INT32)m_TexList.size(); }
	i3Texture *		getTexture( INT32 idx)						{ return m_TexList[idx]; }

	INT32			getDefaultSpaceWidth(void)					{ return m_SpaceWidth; }

	bool			isSame( LOGFONTA * pFontInfo);

	virtual bool	Destroy( bool bLostDevice) override;
	virtual bool	Revive( i3RenderContext * pCtx) override;
};

class I3_EXPORT_GFX i3GfxFontDX2Manager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GfxFontDX2Manager, i3ElementBase);

protected:
	i3::vector<i3GfxFontDX2*>	m_FontList;

public:
	i3GfxFontDX2Manager(void);
	virtual ~i3GfxFontDX2Manager(void);

	INT32			getFontCount(void)					{ return (INT32)m_FontList.size(); }
	i3GfxFontDX2 *	getFont( INT32 idx)					{ return m_FontList[idx]; }
	void			addFont( i3GfxFontDX2 * pFont);
	void			removeFont( i3GfxFontDX2 * pFont);
	i3GfxFontDX2 *	findFont( LOGFONTA * pFontInfo);

	void			ReleaseAllFont(void);
	void			ReleaseUnusedFont(void);

	static i3GfxFontDX2 *	CreateFont( LOGFONTA * pFontInfo);
	static void				ReleaseFont( i3GfxFontDX2 * pFont);
};


#endif	// __I3_GFX_FONT_DX2_H
