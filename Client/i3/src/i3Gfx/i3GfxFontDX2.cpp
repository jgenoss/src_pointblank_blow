#include "i3GfxType.h"
#include "i3Base.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxFontDX2.h"
#include "i3GfxUtil.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/generic_string_size.h"

//I3_CLASS_INSTANCE( i3GfxFontDX2Manager, i3ElementBase);
I3_CLASS_INSTANCE( i3GfxFontDX2Manager);

static i3GfxFontDX2Manager * s_pFontManager = nullptr;

i3GfxFontDX2Manager::i3GfxFontDX2Manager(void)
{
	s_pFontManager = this;
	m_FontList.reserve(16);
}

i3GfxFontDX2Manager::~i3GfxFontDX2Manager(void)
{
	ReleaseAllFont();
	s_pFontManager = nullptr;
}

void i3GfxFontDX2Manager::ReleaseAllFont(void)
{
	
	i3GfxFontDX2 * pFont;

	for(size_t i = 0; i < m_FontList.size(); i++)
	{
		pFont = m_FontList[i];

		I3_MUST_RELEASE(pFont);
	}

	m_FontList.clear();
}

void i3GfxFontDX2Manager::ReleaseUnusedFont(void)
{
	
	i3GfxFontDX2 * pFont;

	for(size_t i = 0; i < m_FontList.size(); i++)
	{
		pFont = m_FontList[i];

		if( pFont->GetRefCount() == 1)
		{
			I3_MUST_RELEASE(pFont);
			m_FontList.erase(m_FontList.begin() + i);
			i--;
		}
	}
}

i3GfxFontDX2 *	i3GfxFontDX2Manager::findFont( LOGFONTA * pFontInfo)
{
	i3GfxFontDX2 * pFont;
	INT32 i;

	for( i = 0; i < getFontCount(); i++)
	{
		pFont = getFont( i);

		if( pFont->isSame( pFontInfo))
		{
			return pFont;
		}
	}

	return nullptr;
}

void			i3GfxFontDX2Manager::addFont( i3GfxFontDX2 * pFont)
{
	m_FontList.push_back( pFont);

	I3_MUST_ADDREF( pFont);
}

void i3GfxFontDX2Manager::removeFont( i3GfxFontDX2 * pFont)
{
	I3ASSERT( pFont != nullptr);

	if( pFont->GetRefCount() == 1)
	{
		i3::vu::remove(m_FontList, pFont);		//	m_FontList.Remove( pFont);
	}

	I3_MUST_RELEASE(pFont);
}

i3GfxFontDX2 *	i3GfxFontDX2Manager::CreateFont( LOGFONTA * pFontInfo)
{
	i3GfxFontDX2 * pNewFont;

	if( (strcmp( pFontInfo->lfFaceName, "굴림") == 0) ||
		(strcmp( pFontInfo->lfFaceName, "돋음") == 0))
	{
		strncpy( pFontInfo->lfFaceName, "윤고딕240", sizeof(pFontInfo->lfFaceName)-1);
		pFontInfo->lfQuality = ANTIALIASED_QUALITY;
	}
	else if( (strcmp( pFontInfo->lfFaceName, "Arial") == 0))
	{
		pFontInfo->lfQuality = ANTIALIASED_QUALITY;
	}
	else if( (strcmp( pFontInfo->lfFaceName, "Calibri") == 0))
	{
		pFontInfo->lfQuality = ANTIALIASED_QUALITY;
	}

	// 동일한 Font가 이미 생성되어 있는지 확인한다.
	pNewFont = s_pFontManager->findFont( pFontInfo);
	if( pNewFont != nullptr)
	{
		I3_MUST_ADDREF( pNewFont);
		return pNewFont;
	}

	pNewFont = i3GfxFontDX2::new_object_ref();
	I3ASSERT( pNewFont != nullptr);

	pNewFont->CreateFont( pFontInfo);

	s_pFontManager->addFont( pNewFont);

	return pNewFont;
}

void i3GfxFontDX2Manager::ReleaseFont( i3GfxFontDX2 * pFont)
{
	if( s_pFontManager != nullptr)
	{
		s_pFontManager->removeFont( pFont);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
#define CONV_CHAR(c1, c2, c3, c4)				(((UINT32)c1 << 24 & 0xFF000000) | \
												((UINT32)c2 << 16 & 0xFF0000) | \
												((UINT32)c3 << 8 & 0xFF00) | \
												((UINT32)c4 & 0xFF))
*/
#define CONV_CHAR(c1, c2)						( ( (UINT32)c1 << 16  & 0xFFFF0000) | ( (UINT32)c2 & 0x0000FFFF) )

//I3_CLASS_INSTANCE( i3GfxFontDX2, i3GfxResource);
I3_CLASS_INSTANCE( i3GfxFontDX2);

/*
static INT32	_GlyphCompareProc( DX2_FONT_INFO * p1, DX2_FONT_INFO * p2)
{
	UINT32 c1 = CONV_CHAR(p1->Char[0], p1->Char[1], p1->Char[2], p1->Char[3]); 
	UINT32 c2 = CONV_CHAR(p2->Char[0], p2->Char[1], p2->Char[2], p2->Char[3]); 

	if( c1 > c2)
		return 1;
	else if( c1 < c2)
		return -1;

	return 0;
}

static INT32	_GlyphFindProc( DX2_FONT_INFO * p1, char * ch)
{
	//	한 문자가 가변 바이트의 크기인 최대 4바이트를 넘으면 문제가 있습니다. komet
	INT32 len = i3::generic_string_size(ch);
	I3ASSERT( len <= 4);

	char chr[4] = "";	

	UINT32 c1 = CONV_CHAR(p1->Char[0], p1->Char[1], p1->Char[2], p1->Char[3]); 

	chr[0] = (len >= 1 ? ch[0] : 0);
	chr[1] = (len >= 2 ? ch[1] : 0);
	chr[2] = (len >= 3 ? ch[2] : 0);
	chr[3] = (len >= 4 ? ch[3] : 0);

	UINT32 c2 = CONV_CHAR(chr[0], chr[1], chr[2], chr[3]); 

	if( c1 > c2)
		return 1;
	else if( c1 < c2)
		return -1;

	return 0;
}
*/
i3GfxFontDX2::i3GfxFontDX2(void)
{
	i3mem::FillZero( &m_LogFont, sizeof( m_LogFont));

	m_TexList.reserve(32);
}

i3GfxFontDX2::~i3GfxFontDX2(void)
{
	_removeAllTexture();
	_removeAllGlyph();

	if( m_hFont != nullptr)
	{
		::DeleteObject( m_hFont);
		m_hFont = nullptr;
	}

	I3_SAFE_RELEASE( m_pDrawTex);
}

void i3GfxFontDX2::_removeAllTexture(void)
{
	
	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		i3Texture * pTex = getTexture( i);

		I3_MUST_RELEASE(pTex);
	}

	m_TexList.clear();
}

void i3GfxFontDX2::_removeAllGlyph(void)
{
	i3::unordered_map<UINT32, DX2_FONT_INFO*>::iterator it_end = m_GlyphList.end();

	for(i3::unordered_map<UINT32, DX2_FONT_INFO*>::iterator it = m_GlyphList.begin(); it != it_end; ++it)
	{
		DX2_FONT_INFO * pInfo = it->second;
		i3MemFree( pInfo);
	}

	m_GlyphList.clear();
}

void i3GfxFontDX2::CreateFont( LOGFONTA* pLogFont)
{
	i3mem::Copy( &m_LogFont, pLogFont, sizeof(LOGFONTA));

	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_THAI)
	{
		// 태국어의 경우, Vista에서의 Tahoma Font가 아주 작게 표시되어
		// 강제로 일정 크기 이상의 Font를 생성해 준다.
		//
		OSVERSIONINFO info;

		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx( &info);

		if( info.dwMajorVersion > 5)
		{
			if( i3::generic_is_iequal( m_LogFont.lfFaceName, "Tahoma") )
				m_LogFont.lfHeight = abs( m_LogFont.lfHeight) + 2;
			else
				m_LogFont.lfHeight = abs( m_LogFont.lfHeight);
		}
		else
		{
			m_LogFont.lfHeight = abs( m_LogFont.lfHeight);
		}
	}
	else
	{
		m_LogFont.lfHeight = abs( m_LogFont.lfHeight);		
	}

	m_hFont = CreateFontIndirect( &m_LogFont);
	I3ASSERT( m_hFont != nullptr);

	m_LastX = m_LastY = 0;

	m_LogFont.lfHeight = abs( m_LogFont.lfHeight);

	// DC를 구해 Font를 Drawing할 Texture.
	INT32 sz;

	sz = i3Gfx::GetNearestPowerOfTwo( m_LogFont.lfHeight + 2);

	if( sz < 32)
		sz = 32;

	I3_SAFE_RELEASE( m_pDrawTex);
	m_pDrawTex = i3Texture::new_object();
#if defined( I3_DEBUG)
	m_pDrawTex->SetName( "Font");
#endif
	m_pDrawTex->Create( sz, sz, 1, I3G_IMAGE_FORMAT_BGRX_8888, 0);

	{
		HDC dc;
		SIZE sz;

		m_pDrawTex->getSurface(0)->getSurface()->GetDC( &dc);
		HFONT hOldFont = (HFONT) ::SelectObject( dc, m_hFont);

		::GetTextExtentPoint32( dc, " ", 1, &sz);

		::SelectObject( dc, hOldFont);
		m_pDrawTex->getSurface(0)->getSurface()->ReleaseDC( dc);

		m_SpaceWidth = sz.cx;
	}
}

DX2_FONT_INFO *	i3GfxFontDX2::findGlyph( const wchar_t * ch, INT32 letter_len)
{
	UINT32 c2 = 0;
	
	switch( letter_len )
	{
	case 0:	break;
	case 1:	c2 = CONV_CHAR(ch[0], 0); break;
	case 2:	c2 = CONV_CHAR(ch[0], ch[1]); break;
	default: break;
	}

	i3::unordered_map<UINT32, DX2_FONT_INFO*>::iterator it = m_GlyphList.find(c2);
	
	return ( it != m_GlyphList.end() ) ? it->second : nullptr;

}

#define _SPACE_		1

void i3GfxFontDX2::_addTexture(void)
{
	i3Texture * pTex;

	// 마지막 Texture
	if( m_TexList.size() > 0)
	{
		pTex = _getLastTexture();

		if( pTex->isLocked())
		{
			pTex->Unlock( pTex->getLockedLevel());
		}
	}

	pTex = i3Texture::new_object();
#if defined( I3_DEBUG)
	{
		char conv[256];
		sprintf( conv, "FontTex[%d] %s, %d(%d)\n", (INT32)m_TexList.size(), m_LogFont.lfFaceName, m_LogFont.lfHeight, m_LogFont.lfWeight);
		pTex->SetName( conv);
	}
#endif
	pTex->Create( FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);

	m_TexList.push_back( pTex);

	m_LastX = 0;
	m_LastY = 0;
}

void i3GfxFontDX2::BeginEdit(void)
{
}

void i3GfxFontDX2::EndEdit(void)
{
	i3Texture * pTex;

	if( m_TexList.size() <= 0)
		return;

	pTex = _getLastTexture();

	if( pTex->isLocked())
		pTex->Unlock( pTex->getLockedLevel());
}

void i3GfxFontDX2::_CopyFontImage( i3Texture * pTex, INT32 x, INT32 y, INT32 cx, INT32 cy)
{
	m_pDrawTex->Lock( 0, I3G_LOCK_NO_DIRTY_UPDATE);

	if( pTex->isLocked() == false)
	{
		pTex->Lock( 0, 0);
	}

	UINT32	* pSrc, * pDest, * pSrcBase, * pDestBase;
	INT32 i, j;

	pSrcBase = (UINT32 *) m_pDrawTex->GetLockedBuffer( m_pDrawTex->getLockedLevel());
	pDestBase = ((UINT32 *) pTex->GetLockedBuffer( pTex->getLockedLevel())) + (y * pTex->GetWidth()) + x;

	for( i = 0; i < cy; i++)
	{
		pSrc = pSrcBase;
		pDest = pDestBase;

		for( j = 0; j < cx; j++)
		{
			pDest[j] = (pSrc[j] & 0x00FFFFFF) | ((pSrc[j] & 0xFF) << 24);		// Alpha
		}

		pSrcBase += m_pDrawTex->GetWidth();
		pDestBase += pTex->GetWidth();
	}

	m_pDrawTex->Unlock( m_pDrawTex->getLockedLevel());

	// pTex는 Unlock하지 않는다.
	// 그것은 문자를 개별적으로 Update할 때마다 Lock()->Unlock()이 반복되는 것을 피하기 위함.
}

void i3GfxFontDX2::_DrawFontToDrawTex( DX2_FONT_INFO * pGlyph, SIZE * pSize)
{
	if ( g_pRenderContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == true)
	{
		pSize->cx = 0;
		pSize->cy = 0;
		return;
	}

	HDC dc;
	i3Surface * pSurface;
	IDirect3DSurface9 * pDXSurface;

	pSurface = m_pDrawTex->getSurface(0);
	pDXSurface = pSurface->getSurface();

	if (pDXSurface)
		pDXSurface->GetDC( &dc);
	else
		return;

	HFONT hOldFont = (HFONT) ::SelectObject( dc, m_hFont);

	{	//	다국어를 위해서 기존 한글 2바이트 체계에서 가변 바이트로 변경 komet			
		::SetBkColor( dc, RGB( 0, 0, 0));
		::SetTextColor( dc, RGB( 255, 255, 255));

		if( pSize != nullptr)
			::GetTextExtentPoint32W( dc, pGlyph->wChar, pGlyph->nCharLength, pSize);

		::TextOutW( dc, 0, 0, pGlyph->wChar, pGlyph->nCharLength);
	}

	::SelectObject( dc, hOldFont);
	m_pDrawTex->getSurface(0)->getSurface()->ReleaseDC( dc);
}

//	ch는 한 문자여야 함
DX2_FONT_INFO *	i3GfxFontDX2::getGlyph( const wchar_t* ch, INT32 letter_len)
{
	i3Texture * pTex;
	SIZE Size;
	Size.cx = 0;
	Size.cy = 0;

	if( ch == nullptr || ch[0] == 0)
		return nullptr;

	DX2_FONT_INFO * pGlyph = findGlyph( ch, letter_len);

	if( pGlyph != nullptr)
		return pGlyph;			// 이미 해당 문자가 있다.

	pGlyph = (DX2_FONT_INFO *) i3MemAlloc( sizeof(DX2_FONT_INFO));	
	I3ASSERT( pGlyph != nullptr);
	i3mem::FillZero(pGlyph, sizeof(DX2_FONT_INFO));		//	반드시 초기화 필요

	//	한 문자가 가변 바이트의 크기인 최대 4바이트를 넘으면 문제가 있습니다. komet
	INT32 len = letter_len;
//	I3ASSERT( len <= 4);

	pGlyph->nCharLength = (INT16) len;

	for(INT32 i=0; i<len; i++)
	{
		pGlyph->wChar[i] = ch[i];
	}

	// 글자 그리기
	_DrawFontToDrawTex( pGlyph, &Size);

	if( m_TexList.size() > 0)
	{ 
		if( (m_LastX + Size.cx + _SPACE_) > FONT_TEXTURE_WIDTH)
		{
			if( (m_LastY + ((Size.cy + _SPACE_) * 2)) > FONT_TEXTURE_HEIGHT)
			{
				// Texture의 수용한도를 넘어간다.
				// 새 Texture가 필요하다.
				_addTexture();
			}
			else
			{
				m_LastX = 0;
				m_LastY += (Size.cy + _SPACE_);
			}
		}
	}
	else
	{
		_addTexture();
	}

	// 그린 Font를 Texture로 복사
	if (g_pRenderContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false)
	{
		pTex = _getLastTexture();

		_CopyFontImage( pTex, m_LastX, m_LastY, Size.cx, Size.cy);
	}

	pGlyph->nGlyphWidth		= (INT16) Size.cx;
	pGlyph->nGlyphHeight	= (INT16) Size.cy;
	pGlyph->nStartX			= (INT16) m_LastX;
	pGlyph->nStartY			= (INT16) m_LastY;
	pGlyph->nTextureIdx		= (INT16) m_TexList.size() - 1;

	m_LastX += Size.cx + _SPACE_;
	
	
	UINT32 c1 = CONV_CHAR(pGlyph->wChar[0], pGlyph->wChar[1]);
	
	m_GlyphList.insert( i3::unordered_map<UINT32, DX2_FONT_INFO*>::value_type(c1, pGlyph));

	return pGlyph;
}

bool i3GfxFontDX2::isSame( LOGFONTA * pInfo)
{
	if(	( pInfo->lfHeight			!= m_LogFont.lfHeight) ||
		( pInfo->lfCharSet			!= m_LogFont.lfCharSet) ||
		( pInfo->lfWeight			!= m_LogFont.lfWeight) ||
		( pInfo->lfQuality			!= m_LogFont.lfQuality) ||
		( pInfo->lfItalic			!= m_LogFont.lfItalic) ||
		( pInfo->lfUnderline		!= m_LogFont.lfUnderline))
	{
		return false;
	}
		
	if( i3::generic_is_iequal( pInfo->lfFaceName, m_LogFont.lfFaceName) == false)
		return false;

	return true;
}

bool i3GfxFontDX2::Destroy( bool bLostDevice)
{
	// TexList를 Clear하지 않아야 한다.
	// 그래야 Revive할 때, 생성할 Texture의 개수를 확인할 수 있기 때문이다.
	
	return i3GfxResource::Destroy( bLostDevice);
}

bool i3GfxFontDX2::Revive( i3RenderContext * pCtx)
{
	
	DX2_FONT_INFO * pInfo;
	i3Texture * pTex;

	// 여러 번 호출되어도 무방.
	m_pDrawTex->Revive( pCtx);

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		pTex = m_TexList[i];

		pTex->Revive( pCtx);
	}

	i3::unordered_map<UINT32, DX2_FONT_INFO*>::iterator it_end = m_GlyphList.end();

	for(i3::unordered_map<UINT32, DX2_FONT_INFO*>::iterator it = m_GlyphList.begin(); it != it_end; ++it)
	{
		pInfo = it->second;
		
		// 글자 그리기
		_DrawFontToDrawTex( pInfo, nullptr);

		// 그린 Font를 Texture로 복사
		pTex = getTexture( pInfo->nTextureIdx);

		_CopyFontImage( pTex, pInfo->nStartX, pInfo->nStartY, pInfo->nGlyphWidth, pInfo->nGlyphHeight);
	}

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		pTex = m_TexList[i];

		if( pTex->isLocked())
		{
			pTex->Unlock( pTex->getLockedLevel());
		}
	}

	return true;
}
