#include "i3SceneDef.h"
#include "i3TextNodeDX2.h"
#include "i3LightingEnableAttr.h"
#include "i3Base/string/ext/ismb.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_cat.h"

i3TextFontSetInfo::i3TextFontSetInfo( const char * FontName, UINT32 CharSet /*= DEFAULT_CHARSET*/ )
{
	SetFontName(FontName);
	SetCharset(CharSet);
}


I3_CLASS_INSTANCE( i3TextNodeDX2);

i3TextNodeDX2::~i3TextNodeDX2(void)
{
	I3_SAFE_RELEASE( m_pTextAttrDX);
	I3_SAFE_RELEASE( m_pGeomery);
	I3_SAFE_RELEASE( m_pAttrSet);	
}

bool i3TextNodeDX2::Create( const i3TextFontSetInfo * pFontSetInfo, INT32 nTextMaxCount, INT32 nSize, UINT32 nWeight, bool bItalic, bool bUnderLine, INT32 nQuality,
						   bool bEnable, FONT_SHADOW_QUALITY nShadowQuality, REAL32 OffSet, bool bUseVertexBank, bool bTransformedVertex)
{
	I3ASSERT( m_pTextAttrDX == nullptr);

	m_pTextAttrDX = i3TextAttrDX2::new_object();
	I3ASSERT( m_pTextAttrDX != nullptr);

	if( ! m_pTextAttrDX->CreateEx( pFontSetInfo->GetFontName(), pFontSetInfo->GetCharset(), nTextMaxCount, nSize, nWeight, bItalic, bUnderLine, nQuality, bEnable, nShadowQuality, OffSet, bUseVertexBank, bTransformedVertex) )
	{
		I3TRACE( "%s (%d) : [i3TextNodeDX2::Create]\n", __FILE__, __LINE__ );
		I3ASSERT_0;
		return false;
	}


	m_pAttrSet = i3AttrSet::new_object();
	AddChild(m_pAttrSet);

	I3ASSERT( m_pGeomery == nullptr);
	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	m_pAttrSet->AddChild(m_pGeomery);	
	m_pGeomery->AppendGeometryAttr(m_pTextAttrDX);

	return true;
}

 bool i3TextNodeDX2::Create( const char* pszFont, INT32 nTextMaxCount, INT32 nSize, UINT32 nWeight, bool bItalic, bool bUnderLine, INT32 nQuality,
 						   bool bEnable, FONT_SHADOW_QUALITY nShadowQuality, REAL32 OffSet, bool bUseVertexBank, bool bTransformedVertex)
{
	i3TextFontSetInfo	fontSetInfo(pszFont, DEFAULT_CHARSET);
 	return Create(&fontSetInfo, nTextMaxCount, nSize, nWeight, bItalic, bUnderLine, nQuality,
 		bEnable, nShadowQuality, OffSet, bUseVertexBank, bTransformedVertex);
}

namespace
{
	inline bool is_1st_surrogate(wchar_t c) {	return 0xD800 <= c && c <= 0xDBFF;	}
}

void i3TextNodeDX2::SetTextEllipsis(const i3::wliteral_range& wTextRng)
{
	SetText(wTextRng);
	
	// 글폭이 컨트롤폭을 넘어가면 "..."으로 끊는다
	if (getTextWidth() > getWidth())
	{
		INT32 ellipsisLength = 0;

		// "..." 폭 구하기
		SetText(L"...");
		REAL32 margin = (REAL32)(getWidth() - getTextWidth());

		// "..." 폭을 제외한 여유공간에 들어갈 글자 계산
		INT32 length = i3::generic_string_size(wTextRng);
	
		for(INT32 i = 0; i < length; i++)
		{
			if( wTextRng[i] == 10) break;

			margin -= getCharacterSizeX(&wTextRng[i]);

			if (0.0f > margin)
			{
				break;
			}

			// 멀티바이트 문자를 위한 문자단위 증가
			
			if ( is_1st_surrogate(wTextRng[i]) )
			{
				++i;
			}

			ellipsisLength = i + 1;
		}


		// 계산한 위치에 "..."을 붙인다
		wchar_t output[2048];
		i3::string_ncopy_nullpad(output, wTextRng, ellipsisLength);		
		i3::generic_string_cat(output, L"...");	
		size_t output_size = i3::generic_string_size(output);
		output[output_size] = 0;		
		SetText(i3::wliteral_range(output, output + output_size) );
	}
}
