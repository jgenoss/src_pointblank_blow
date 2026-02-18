#include "i3FrameworkPCH.h"
#include "i3UIFont.h"
#include "i3UIGlyph.h"
#include "i3UIFontManager.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3UIManager.h"
#include "i3Framework.h"


I3_CLASS_INSTANCE( i3UIFont);

/*
static INT32	_GlyphComp( i3UIGlyph * p1, i3UIGlyph * p2)
{
	WORD wCode1 = p1->getGlyphCode();
	WORD wCode2 = p2->getGlyphCode();

	if( wCode1 > wCode2)
		return 1;
	else if( wCode1 < wCode2)
		return -1;

	return 0;
}
*/

static INT32	_GlyphFind( i3UIGlyph * p1, WORD* pwGlyph)
{
	WORD wCode = p1->getGlyphCode();

	if( wCode > *pwGlyph)
		return 1;
	else if( wCode < *pwGlyph)
		return -1;

	return 0;
}

i3UIFont::~i3UIFont(void)
{
	RemoveAllGlyph();

	::DeleteObject( m_hFont);
	::DeleteObject( m_hRenderFont);
}

void i3UIFont::RemoveAllGlyph(void)
{
	i3::cu::for_each_delete_clear(m_GlyphList);
}

void i3UIFont::Create( i3UIFontManager * pCache, I3UI_FONTINFO * pInfo)
{
	m_pCache = pCache;

	INT32 size = abs( pInfo->m_Size);

	m_Info = *pInfo;

#if defined( I3G_DX)
	{
		LOGFONTA font;

		i3::string_ncopy_nullpad( font.lfFaceName, m_Info.m_strName.c_str(), sizeof( font.lfFaceName));
		
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
				if( m_Info.m_strName == "Tahoma")
					font.lfHeight = size + 2;
				else
					font.lfHeight = size;
			}
			else
			{
				font.lfHeight = size;
			}
		}
		else
		{
			font.lfHeight		= size;
		}

		font.lfWidth		= 0;
		font.lfEscapement	= 0;
		font.lfOrientation	= 0;

		if( m_Info.m_Style & I3UI_FONT_STYLE_BOLD)
			font.lfWeight	= 700;
		else
			font.lfWeight	= 400;

		if( m_Info.m_Style & I3UI_FONT_STYLE_ITALIC)
			font.lfItalic	= TRUE;
		else
			font.lfItalic	= FALSE;

		if( m_Info.m_Style & I3UI_FONT_STYLE_UNDERLINE)
			font.lfUnderline = TRUE;
		else
			font.lfUnderline = FALSE;

		if( m_Info.m_Style & I3UI_FONT_STYLE_STRIKEOUT)
			font.lfStrikeOut = TRUE;
		else
			font.lfStrikeOut = FALSE;

		font.lfCharSet		= pInfo->m_CharaSet;
		font.lfOutPrecision	= OUT_DEFAULT_PRECIS;
		font.lfClipPrecision= CLIP_DEFAULT_PRECIS;

		switch( m_Info.m_Quality)
		{
			default :							font.lfQuality		= ANTIALIASED_QUALITY;	break;
			case I3UI_FONT_QUALITY_CLEARTYPE:	font.lfQuality		= CLEARTYPE_QUALITY;	break;
			case I3UI_FONT_QUALITY_PROOF :		font.lfQuality		= PROOF_QUALITY;		break;
		}

		font.lfQuality		= ANTIALIASED_QUALITY;

		font.lfPitchAndFamily	= DEFAULT_PITCH;

		m_hFont	= ::CreateFontIndirect( &font);

		/*if( pCache->IsLegacyMode())
		{
			i3Viewer * pViewer = i3UI::getManager()->getFramework()->GetViewer();
			REAL32 qual = 1.f;
			
			switch( pViewer->GetRenderContext()->getGfxOption()->getTextureQuality())
			{
			case 0 :	qual = 2.f;
			case 1 :	qual = 1.5f;
			case 2 :	qual = 1.2f;
			default :	qual = 1.f;
			}
			if( i3UI::getToolDevMode())
				qual = 2.f;

			qual = 1.f;
			font.lfHeight = abs( (INT32)(pInfo->m_Size * qual) );
		}
		else*/
			font.lfHeight = abs( (INT32)(pInfo->m_Size) );

		// 정확한 크기의 Font가 생성되기 위해서는 lfHeight의 값이 음수로 들어가야 Pixel 단위(Device Unit)에서 정확한 것이 나온다.
		// LOGFONT.lfHeight MSDN 참조.
		font.lfHeight = -font.lfHeight;

		m_hRenderFont = ::CreateFontIndirect( &font);
	}
#endif
}

void i3UIFont::RecreateRenderFont( void)
{
#if defined( I3G_DX)
	::DeleteObject( m_hFont);
	::DeleteObject( m_hRenderFont);
#endif

	Create( m_pCache, &m_Info);
}

i3UIGlyph* i3UIFont::AddGlyph( WORD wGlyph, bool* pOutExistOnVRAM)
{
	i3UIGlyph* pGlyph = FindGlyph( wGlyph);
	
	if( pGlyph == nullptr)
	{
		pGlyph = new i3UIGlyph;
		NetworkDump_Malloc( pGlyph, sizeof( i3UIGlyph), __FILE__, __LINE__);

		pGlyph->setGlyphCode( wGlyph);

		m_GlyphList.insert( i3::make_pair(wGlyph, pGlyph) );
	}

	if( pOutExistOnVRAM != nullptr)
		*pOutExistOnVRAM = (pGlyph->getPosX() >= 0);
	
	return pGlyph;
}

i3UIGlyph * i3UIFont::FindGlyph( WORD wGlyph)
{
//	m_GlyphList.SetCompareProc( (COMPAREPROC) _GlyphFind);

//	INT32 idx = m_GlyphList.FindItem( (void *)&wGlyph);

//	m_GlyphList.SetCompareProc( (COMPAREPROC) _GlyphComp);
	
	i3::unordered_map<WORD, i3UIGlyph*>::iterator it = m_GlyphList.find(wGlyph);

	if( it == m_GlyphList.end() )
		return nullptr;
	return it->second;
}

void i3UIFont::Reset(void)
{

	struct fn 
	{
		void operator()( i3UIGlyph* pGlyph) 
		{
			pGlyph->setPosX(-1);	pGlyph->setPosY(-1);
		}
	};
	
	i3::cu::for_each_container(m_GlyphList, fn());

/*
	INT32 i;
	i3UIGlyph * pGlyph;

	for( i = 0; i < getGlyphCount(); i++)
	{
		pGlyph = getGlyph( i);
		pGlyph->setPosX( -1);		// VRAM에 할당되지 않은 것으로 설정.
		pGlyph->setPosY( -1);
	}
*/
}

UINT32		i3UIFont::CalcHashCode( I3UI_FONTINFO * pInfo)
{
	UINT32 crc;

	crc = CRC32( 0xFFFFFFFF, (UINT8 *) pInfo->m_strName.c_str(), pInfo->m_strName.length());

	crc = CRC32( crc, (UINT8 *) &pInfo->m_Size, sizeof( pInfo->m_Size));
	crc = CRC32(crc, (UINT8 *)&pInfo->m_CharaSet, sizeof(pInfo->m_CharaSet));
	crc = CRC32( crc, (UINT8 *) &pInfo->m_Quality, sizeof( pInfo->m_Quality));
	crc = CRC32( crc, (UINT8 *) &pInfo->m_Style, sizeof( pInfo->m_Style));

	return crc;
}

