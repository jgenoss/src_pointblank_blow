#include "i3FrameworkPCH.h"
#include "i3UIFontManager.h"
#include "i3Base/itl/container_util.h"

#define		DRAW_FONT_LIMIT		256
#define	FONT_TEX_COUNT	4	// 1024x1024 4장을 사용하자(2의 배수로 증가시켜야 동작함)

I3_CLASS_INSTANCE( i3UIFontManager);

static INT32	_FontCompProc( i3UIFont * pFont1, i3UIFont * pFont2)
{
	if( pFont1->getHashCode() > pFont2->getHashCode())
		return 1;
	else if( pFont1->getHashCode() < pFont2->getHashCode())
		return -1;

	return 0;
}

static INT32	_FontFindProc( i3UIFont * pFont, UINT32 code)
{
	if( pFont->getHashCode() > code)
		return 1;
	else if( pFont->getHashCode() < code)
		return -1;

	return 0;
}

i3UIFontManager::i3UIFontManager(void)
{
	m_TextureList.reserve( 4);
	m_CacheList.reserve( 4);
}

i3UIFontManager::~i3UIFontManager(void)
{
	i3::cu::for_each_release_clear(m_TextureList, &i3Texture::Release);
	i3::cu::for_each_release_clear(m_CacheList, &i3UIFontCache::Release);

	RemoveAllFonts();
}

bool i3UIFontManager::Create( bool bLegacy)
{
	m_bLegacy = bLegacy;

	for( UINT32 i = 0; i < FONT_TEX_COUNT; i++)
	{
		i3UIFontCache * pCache = i3UIFontCache::new_object();
		pCache->Create( MAX_FONT_TEX_SIZE, MAX_FONT_TEX_SIZE);
		m_CacheList.push_back( pCache);
	}

	_CreateTextures();

	return true;
}

void i3UIFontManager::_CreateTextures(void)
{
	COLOR col;
	i3Color::Set( &col, (UINT8) 0, 0, 0, 0);

	for( UINT32 i = 0; i < FONT_TEX_COUNT; i++)
	{
		i3Texture * pNewTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		pNewTex->SetName( "FontTex");
#endif
		pNewTex->Create( MAX_FONT_TEX_SIZE, MAX_FONT_TEX_SIZE, 1, I3G_IMAGE_FORMAT_BGRX_8888, I3G_USAGE_FORCE_VRAM | I3G_USAGE_RENDERTARGET);

		// Clear
		{
			g_pRenderContext->ColorFill( pNewTex, &col);
		}

		m_TextureList.push_back( pNewTex);
	}
}

INT32 i3UIFontManager::FindTextureIndex( INT32 x, INT32 y, INT32 w, INT32 h)
{
	INT32 xIdx = (x+w) / MAX_FONT_TEX_SIZE;
	INT32 yIdx = (y+h) / MAX_FONT_TEX_SIZE;

	return xIdx + (yIdx * (FONT_TEX_COUNT / 2));
}

INT32 i3UIFontManager::FindTextureIndex( REAL32 x, REAL32 y, REAL32 w, REAL32 h)
{
	return FindTextureIndex( static_cast<INT32>(x), static_cast<INT32>(y), static_cast<INT32>(w), static_cast<INT32>(h) );
}

i3Texture * i3UIFontManager::FindTexture( INT32 x, INT32 y, INT32 w, INT32 h)
{
	INT32 tIdx = FindTextureIndex( x, y, w, h);
	return m_TextureList.at( tIdx);
}

void i3UIFontManager::RemoveAllFonts(void)
{
	i3::cu::for_each_release_clear(m_FontList, &i3UIFont::Release);
}

void i3UIFontManager::DeallocateAllGlyphs(void)
{
	i3::cu::for_each_container(m_FontList, i3::mem_fun(&i3UIFont::Reset));
}

i3UIFont * i3UIFontManager::GetFont( const char * pszName, INT32 height, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut, I3UI_FONT_QUALITY quality)
{
	I3UI_FONTINFO info;

	if( m_strDefaultFontName.empty() ) 
		info.m_strName = pszName;
	else
		info.m_strName = m_strDefaultFontName; 

	info.m_Size = height + m_ExternalFontSize;

	if( bBold)
		info.m_Style |= I3UI_FONT_STYLE_BOLD;

	if( bItalic)
		info.m_Style |= I3UI_FONT_STYLE_ITALIC;

	if( bUnderline)
		info.m_Style |= I3UI_FONT_STYLE_UNDERLINE;

	if( bStrikeOut)
		info.m_Style |= I3UI_FONT_STYLE_STRIKEOUT;

	info.m_Quality = quality;

	info.m_CharaSet = m_DefaultCharaSet;

	UINT32 code = i3UIFont::CalcHashCode( &info);

	// 테스트 삼아 넣어보고..
	i3::pair<i3::unordered_map< UINT32, i3UIFont*>::iterator, bool> pr = m_FontList.insert( i3::make_pair(code, static_cast<i3UIFont*>(0) ) );
	
	if (pr.second == true) // 인서트가 성공하면 새것..
	{
		i3UIFont * pFont = i3UIFont::new_object();
		pFont->Create( this, &info);
		pFont->setHashCode( code);
		(pr.first)->second = pFont;

		return pFont;
	}
	
	return (pr.first)->second;		// 기존에 있던 것..

}

i3UIFont * i3UIFontManager::GetFont(UINT8 CodeSet, i3UIFont* pFont, char* fontName, int nExternalSize)
{
	I3UI_FONTINFO info;

	info.m_CharaSet = CodeSet;

	if(fontName)
		info.m_strName = fontName;
	else
		info.m_strName = pFont->getFontName();
	info.m_Size = pFont->getOriginalFontHeight();
	info.m_Size += nExternalSize - m_ExternalFontSize;
	info.m_Style = pFont->getFontStyle();
	info.m_Quality = pFont->getFontQuality();

	UINT32 code = i3UIFont::CalcHashCode(&info);

	// 테스트 삼아 넣어보고..
	i3::pair<i3::unordered_map< UINT32, i3UIFont*>::iterator, bool> pr = m_FontList.insert(i3::make_pair(code, static_cast<i3UIFont*>(0)));

	if (pr.second == true) // 인서트가 성공하면 새것..
	{
		i3UIFont * pFont = i3UIFont::new_object();
		pFont->Create(this, &info);
		pFont->setHashCode(code);
		(pr.first)->second = pFont;

		return pFont;
	}

	return (pr.first)->second;		// 기존에 있던 것..

}
	
i3UIGlyph *	 i3UIFontManager::AddGlyph( i3UIFont * pFont, WORD wGlyph, bool* pOutExistOnVRAM)
{
	return pFont->AddGlyph( wGlyph, pOutExistOnVRAM);
}

void i3UIFontManager::SetDefaultFontName( const char * pszFontName)
{
	I3ASSERT( pszFontName);
	m_strDefaultFontName = pszFontName;
}

void i3UIFontManager::SetDefaultFontCharaSet( UINT8 charaSet)
{
	m_DefaultCharaSet = charaSet;
}

bool i3UIFontManager::OnDestroy( bool bLostDevice)
{
	i3::cu::for_each_release_clear(m_TextureList, &i3Texture::Release);

	DeallocateAllGlyphs();

	return true;
}

bool i3UIFontManager::OnRevive( i3RenderContext * pCtx)
{
	_CreateTextures();

	i3::cu::for_each_container(m_CacheList,  i3::mem_fun( &i3UIFontCache::Reset));
	i3::cu::for_each_container(m_FontList,  i3::mem_fun( &i3UIFont::RecreateRenderFont));

	m_CurrentCacheIdx = 0;

	return true;
}

#if defined( I3G_DX)

bool i3UIFontManager::ValidateGlyph( i3UIGlyph* pGlyph, i3Surface* pTransTex)
{
	i3UIFontCache * pFontCache = m_CacheList.at( m_CurrentCacheIdx);

	I3UI_FONT_CACHE_NODE * pInfo = pFontCache->AddGlyph( pGlyph);
	if( pInfo == nullptr)
	{
		m_CurrentCacheIdx++;
		I3ASSERT( m_CurrentCacheIdx < m_CacheList.size());
		pFontCache = m_CacheList.at( m_CurrentCacheIdx);
		pInfo = pFontCache->AddGlyph( pGlyph);
	}

	if( pInfo != nullptr)
	{
		pInfo->m_X += (m_CurrentCacheIdx % 2) * MAX_FONT_TEX_SIZE;
		pInfo->m_Y += (m_CurrentCacheIdx / 2) * MAX_FONT_TEX_SIZE;

		pGlyph->setPosX( pInfo->m_X);
		pGlyph->setPosY( pInfo->m_Y);

		i3Texture * pTex = FindTexture( pInfo->m_X, pInfo->m_Y, pGlyph->getWidth(), pGlyph->getHeight());

		i3Surface* pDest = pTex->getSurface( 0);
		
		INT32 clipX = pInfo->m_X % MAX_FONT_TEX_SIZE;
		INT32 clipY = pInfo->m_Y % MAX_FONT_TEX_SIZE;

		pTransTex->CopySurfaceTo( 0, 0, pGlyph->getWidth(), pGlyph->getHeight(), 
									pDest, clipX, clipY);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN,  "Not enough font cache texture!!!!");
	}

	return true;
}

#endif

void i3UIFontManager::Dump( const char * pszFolder)
{
	char szFull[ MAX_PATH];

	for( UINT32 i = 0; i < m_TextureList.size(); i++)
	{
		i3TextureDX * pDXTex = static_cast<i3TextureDX*>(m_TextureList.at(i));

		i3::snprintf( szFull, sizeof( szFull), "%s/Font_Cache%d.TGA", pszFolder, i);

		D3DXSaveTextureToFile( szFull, D3DXIFF_TGA, pDXTex->GetDXImage(), nullptr);
	}
}