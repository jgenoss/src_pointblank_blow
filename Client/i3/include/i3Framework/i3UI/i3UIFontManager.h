#if !defined( __I3_UI_FONT_MANAGER_H)
#define __I3_UI_FONT_MANAGER_H

#include "i3UIFont.h"
#include "i3UIGlyph.h"
#include "i3UIFontCache2.h"

#define		MAX_FONT_TEX_SIZE		1024	// 5000대 그래픽 카드가 지원하는 최대 텍스쳐 사이즈

class i3UIUniscribe;

class I3_EXPORT_FRAMEWORK i3UIFontManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIFontManager, i3ElementBase);

	friend i3UIFont;
	friend i3UIUniscribe;

protected:
	i3::unordered_map<UINT32, i3UIFont*> m_FontList;

	i3::rc_string	m_strDefaultFontName;
	i3::vector<i3Texture*>	m_TextureList;
	i3::vector<i3UIFontCache*>	m_CacheList;

	UINT32			m_CurrentCacheIdx = 0;
	INT32			m_ExternalFontSize = 0;
	UINT8			m_DefaultCharaSet = DEFAULT_CHARSET;
	bool			m_bLegacy = true;
	

protected:
	void			_CreateTextures(void);
	
#if defined( I3G_DX)
	bool			ValidateGlyph( i3UIGlyph* pGlyph, i3Surface* pTransTex);
#endif

	void			DeallocateAllGlyphs(void);

public:
	i3UIFontManager(void);
	virtual ~i3UIFontManager(void);

	bool			Create( bool bLegacy);

	INT32			FindTextureIndex( INT32 x, INT32 y, INT32 w, INT32 h);
	INT32			FindTextureIndex( REAL32 x, REAL32 y, REAL32 w, REAL32 h);
	i3Texture *		FindTexture( INT32 x, INT32 y, INT32 w, INT32 h);
	i3Texture *		GetTexture( INT32 idx)					{ return m_TextureList.at(idx); }

	void			setExternalFontSize( INT32 size)		{ m_ExternalFontSize = size; }

	INT32			getFontCount(void) const				{ return (INT32)m_FontList.size(); }
	void			RemoveAllFonts(void);

	i3UIFont *		GetFont(	const char * pszName, 
								INT32 height, 
								bool bBold = false, 
								bool bItalic = false, 
								bool bUnderline = false, 
								bool bStrikeOut = false,
								I3UI_FONT_QUALITY quality = I3UI_FONT_QUALITY_ANTIALIASED);

	i3UIFont *		GetFont(UINT8 CodeSet, i3UIFont* pFon, char* fontName=nullptr, int nExternalSize=0);
	
	i3UIGlyph *		AddGlyph( i3UIFont * pFont, WORD wGlyph, bool* pOutExistOnVRAM);

	void			SetDefaultFontName( const char * pszFontName);
	void			SetDefaultFontCharaSet( UINT8 charaSet);

	bool			IsLegacyMode( void)					{ return m_bLegacy; }

	void			Dump( const char * pszFolder);

	bool			OnDestroy( bool bLostDevice);
	bool			OnRevive( i3RenderContext * pCtx);
};

#endif
