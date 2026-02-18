#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3TextAttrDX2.h"
#include "mbctype.h"
#include "i3Base/string./compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( i3TextAttrDX2);

static UINT32 s_nVBDrawIndex = 0;

//	해당 텍스트에서 가변 멀티바이트의 한 문자를 추출합니다. (반환 값은 멀티바이트의 바이트수)
/*
INT32 _ExtractMultibyteFromText(const char * text, char * outbuf, INT32 bufCount)
{
	//	다국어를 위해 한 문자의 최소 버퍼 크기는 4바이트입니다. 그러므로 버퍼크기는 널 종료 문자 포함해서 5이상 되어야 합니다.
	I3ASSERT( bufCount > 4 && outbuf != nullptr && text != nullptr);

	INT32 len = 0;
	if( *text & 0x80)
	{
		char * start = (char*) text;
		char * next = (char*) i3String::CharNext( i3Language::GetCurrentCodePage(), start);	//	다음 문자로 이동
		I3ASSERT( &(*start) <= &(*next));

		len = 0;
		//	한 문자를 이루는 가변 바이트를 저장한다.
		while( &(*start) != &(*next))
		{
            outbuf[len] = *start;
			start++;
			len++;

			I3ASSERT( &(*start) <= &(*next));

			if( len >= bufCount || *start == 0)
				break;
		}
	}
	else
	{
		outbuf[0] = *text;
		len = 1;
	} 

	I3ASSERT( len < bufCount);
	outbuf[len] = 0;

	return len;
}
*/

namespace 
{
	static const UINT32 illegal = 0xFFFFFFFFu;
	static const UINT32 incomplete = 0xFFFFFFFEu;
	
	inline bool is_1st_surrogate(wchar_t c) {	return 0xD800 <= c && c <= 0xDBFF;	}
	inline bool is_2nd_surrogate(wchar_t c)	{	return 0xDC00 <= c && c <= 0xDFFF;	}
	
	INT32 ExtractLetterFromText(const wchar_t* psz, wchar_t (&out)[2] )
	{
		out[0] = out[1] = 0;

		if ( *psz == 0 ) return 0;
		
		if ( !is_1st_surrogate(*psz) )
		{
			out[0] = *psz; return 1;
		}

		out[0] = *psz++;	
		
		if ( *psz == 0 ) return 1;
		
		out[1] = *psz;	
		
		return 2;
	}
}

i3TextAttrDX2::i3TextAttrDX2(void)
{
	SetID( I3_ATTRID_TEXTDX2);
}

i3TextAttrDX2::~i3TextAttrDX2(void)
{
	if( m_pFont != nullptr)
	{
		i3GfxFontDX2Manager::ReleaseFont( m_pFont);
		m_pFont = nullptr;
	}

	I3_SAFE_RELEASE( m_pVA);

	I3MEM_SAFE_FREE( m_pCharTable);
}

bool i3TextAttrDX2::CreateEx( const char * pszFontType, UINT32 Charset, INT32 nMaxCount, INT32 nFontSize, INT32 nFontWeight, bool bItalic, bool bUnderLine, INT32 nQuality, 
						   bool bShadow, FONT_SHADOW_QUALITY quality, REAL32 shadowOff, 
						   bool bUseVertexBank, bool bTransformedVertex)
{
	bool bRet = true;
	LOGFONTA log;
	INT32 fontSzPx;

	// point 단위 크기를 Pixel 단위로 환산.
	fontSzPx = i3Gfx::FontPointToPixel( nFontSize);
	//fontSzPx = nFontSize + 3;

	m_bUseVertexBank = bUseVertexBank;
	
	m_MaxCount = nMaxCount;

	m_fShadowOffSet = shadowOff;
	m_nShadowQuality = quality;

	if( bShadow)
	{
		addState( I3_TEXT_STATE_SHADOW);
	}

	I3ASSERT( nMaxCount > 0 );

	m_wstrText.clear();

	// Vertex format
	m_format.SetHasPosition();
	m_format.SetHasColor();		
	m_format.SetTextureCoordSetCount( 1);
	m_format.SetTextureCoordCount( 2);

	if( bTransformedVertex)
		m_format.SetIsTransformed();

	// 1. 폰트 생성
	{
		// Font Log 생성
		log.lfHeight			= fontSzPx;
		log.lfWidth				= 0;
		log.lfEscapement		= 0;
		log.lfOrientation		= 0;
		log.lfWeight			= nFontWeight;
		log.lfItalic			= static_cast<BYTE>(bItalic);
		log.lfUnderline			= static_cast<BYTE>(bUnderLine);
		log.lfStrikeOut			= 0;
		log.lfCharSet			= static_cast<BYTE>(Charset);

		log.lfOutPrecision		= 0;
		log.lfClipPrecision		= 0;
		log.lfQuality			= static_cast<BYTE>(nQuality);
		log.lfPitchAndFamily	= 0;

		if( pszFontType != nullptr)
			i3::safe_string_copy( log.lfFaceName, pszFontType, LF_FACESIZE);
		else
			i3::safe_string_copy( log.lfFaceName, "Tahoma", LF_FACESIZE);

		m_pFont = i3GfxFontDX2Manager::CreateFont( &log);
	}

	// 2. Text가 출력될 Vertex생성
	m_VtxStride = 6;
	if( bShadow)
	{
		switch( quality)
		{
		case FONT_SHADOW_QUALITY_LOW :			m_VtxStride += 6;		break;
		case FONT_SHADOW_QUALITY_NORMAL :
		case FONT_SHADOW_QUALITY_HIGH :			m_VtxStride += 6 * 4;	break;
		}
	}

	_CreateVertex();

	{
		m_pCharTable = (DISP_CHAR_INFO *) i3MemAlloc( sizeof( DISP_CHAR_INFO) * m_MaxCount);
		I3ASSERT( m_pCharTable);
	}

	return bRet;
}

void i3TextAttrDX2::_CreateVertex(void)
{
	if( !m_bUseVertexBank)
	{	
		m_pVA = i3VertexArray::new_object();
		m_pVA->Create( &m_format, m_MaxCount * m_VtxStride, 0);//I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE);		
	}
}

bool i3TextAttrDX2::_ParseCommand( PARSE_CTX * pCtx, const wchar_t* wszCmd)
{
	if (i3::generic_is_niequal( wszCmd, L"col:", 4) )
	{
		const INT32 convLen = 256;

		// Color 설정
		wchar_t * pszTemp = (wchar_t *)wszCmd + 4;

		COLOR textColor;
		wchar_t conv[convLen] = L"";
		INT32 idx = 0, order = 0;

		while( *pszTemp)
		{
			if( idx >= convLen)	break;

			if( isdigit( *pszTemp))
			{
				conv[idx] = *pszTemp; idx++;
			}
			else
			{
				conv[idx] = 0;

				if( *pszTemp != L' ')
				{
					switch( order)
					{
					case 0 :	i3Color::SetR( &textColor, (UINT8) i3::atoi( conv));	break;
					case 1 :	i3Color::SetG( &textColor, (UINT8) i3::atoi( conv));	break;
					case 2 :	i3Color::SetB( &textColor, (UINT8) i3::atoi( conv));	break;
					}
					order++;
				}
				idx = 0;
			}

			pszTemp++;
		}

		//if( idx > 0)
		{
			if( idx >= convLen)			idx--;		//	버퍼 넘어가지 않도록

			conv[idx] = 0;
			i3Color::SetA( &textColor, (UINT8) i3::atoi( conv));
			i3Color::SetA( &m_TextShadowColor, (UINT8) i3::atoi( conv));
		}

		// 기존 Text Color백업
		i3Color::Copy( &m_TextColor_BackUp, & pCtx->_Color);

		// 새로운 Text Color적용
		i3Color::Copy( & pCtx->_Color, &textColor);

		return true;
	}
	else if ( i3::generic_is_niequal( wszCmd, L"/col", 4) )
	{
		i3Color::Copy( &pCtx->_Color, &m_TextColor_BackUp);
		i3Color::SetA( &m_TextShadowColor, i3Color::GetA( &m_TextColor_BackUp));

		return true;
	}

	// 명령어 아님.
	return false;
}

DISP_CHAR_INFO * i3TextAttrDX2::_SetText( PARSE_CTX * pCtx, const i3::wliteral_range& rngText, bool bParseCmd)
{	
	bool bInCmd = false, bChar;
	static wchar_t wszCmd[4096];				// Command String의 Parsing 중에 다시 Command String이 나오면 문제가 발생할 수 있다.
	INT32 idxCmd = 0;

	wszCmd[0] = 0;

	const wchar_t * pszTemp = rngText.c_str();

	while( *pszTemp)
	{
		if ( m_Count >= m_MaxCount )
		{
			i3::string mbText;	i3::utf16_to_mb(rngText, mbText);
			I3PRINTLOG(I3LOG_FATAL,  "text buffer overflow. [%d][%d][%s]", m_Count, m_MaxCount, mbText);
			return pCtx->_pChar;
		}

		wchar_t ch[2] = {0,0};

//		INT32 len = _ExtractMultibyteFromText(pszTemp, ch, 8);	//	다국어용 멀티 바이트 문자를 추출한다.   (wchar_t에서는 불필요함)
		INT32 len = ExtractLetterFromText(pszTemp, ch);			//  서로게이트 문자를 감안해서 처리..   (UTF-16)

		bChar = true;
		
		if ( len == 1)
		{
			switch(ch[0])
			{
				case L'\n' :
				case L'\r' :
					if( bInCmd)
					{
						// Command의 생성을 멈추고 단순히 출력
						wchar_t wszNormalText[4096];
						wszNormalText[0] = 0;

						wszCmd[idxCmd] = 0;
						i3::generic_string_copy( wszNormalText, L"{");
						i3::generic_string_cat( wszNormalText, wszCmd);
						//sprintf( szTemp, "{%s", szCmd);					
						_SetText( pCtx, wszNormalText, false);
						bInCmd = false;
					}

					pCtx->_x = m_PosX;
					pCtx->_y += m_pFont->getGlyphHeight() + m_nSpaceVert;
					pCtx->_Flag |= CHAR_FLAG_LINEBREAK;

					bChar = true;
					m_LineCount ++;
					break;

				case L' ' :
					if( bInCmd)
					{
					}
					else
					{
						//pCtx->_x += m_pFont->getDefaultSpaceWidth();
						pCtx->_Flag |= CHAR_FLAG_WORDBREAK;

						//bChar = false;
					}
					break;

				case L'{' :
					if( bInCmd)
					{
						// 다시 Command Mode를 나타내는 문자가 나왔으므로 기존에 입력된 문자열은 출력할 문자열로 간주
						{
							wchar_t wszNormalText[4096];
							wszNormalText[0] = 0;

							wszCmd[idxCmd] = 0;
							i3::generic_string_copy( wszNormalText, L"{");
							i3::generic_string_cat( wszNormalText, wszCmd);
							_SetText( pCtx, wszNormalText, false);
						}

						// Command 시작으로 간주합니다.
						if( bParseCmd)
						{
							idxCmd = 0;
							bInCmd = true;			
							bChar = false;
						}					
					}
					else
					{
						if( bParseCmd)
						{
							idxCmd = 0;
							bInCmd = true;			// Command의 시작으로 간주
							bChar = false;
						}
					}
					break;

				case '}' :
					if( bParseCmd && bInCmd)
					{
						wszCmd[ idxCmd] = 0;
						bInCmd = false;

						// Command 완성
						if( _ParseCommand( pCtx, wszCmd) == false)
						{
							// 정상적인 Command가 아니기 때문에 그대로 출력문자로 간주
							wchar_t __temp[512] = {};
							i3::snprintf(__temp, 512, L"{%s", wszCmd);
							_SetText( pCtx, __temp, false);
						}
						else
						{
							bChar = false;
						}
					}
					break;
			}
		}

		if( bChar)
		{
			//if( bInCmd && len != 2)
			if( bInCmd && len == 1)			//	ascii 영문만
			{
				// Command Mode에서는 영문만 지원한다.
				//szCmd[ idxCmd] = (char)(ch & 0xFF);		idxCmd++;				
				wszCmd[ idxCmd] = ch[0];		idxCmd++;				
			}
			else
			{
				// Command Mode에서 한글이 나왔다면, 더이상 Command Mode가 아니므로 처리합니다.
				{
					if( bInCmd)
					{
						wchar_t wszNormalText[4096];
						wszNormalText[0] = 0;

						wszCmd[idxCmd] = 0;
						i3::generic_string_copy( wszNormalText, L"{");
						i3::generic_string_cat( wszNormalText, wszCmd);						
						_SetText( pCtx, wszNormalText, false);
						bInCmd = false;
					}
				}

				// 출력 문자 처리
				// 윤고딕에서는 그레이브(`)문자가 없음. 그러므로 ' 로 대체!
				//if( ch[0] == 96) ch[0] = 39;

				DX2_FONT_INFO * pGlyph = m_pFont->getGlyph( ch, len);
				I3ASSERT( pGlyph != nullptr);

				// New Line글자의 경우 화면상에 보이지는 않고 칸만 변경하므로, 글자의 길이를 0으로 합니다. 
				if( ch[0] == 10) pGlyph->nGlyphWidth = 0;
				
				pCtx->_pChar->m_pGlyph = pGlyph;
				pCtx->_pChar->x = pCtx->_x;
				pCtx->_pChar->y = pCtx->_y;
				pCtx->_pChar->m_Flag = pCtx->_Flag;
				pCtx->_Flag = 0;
				i3Color::Set( & pCtx->_pChar->color, & pCtx->_Color);

				pCtx->_pChar ++;
				m_Count++;

				pCtx->_x += pGlyph->nGlyphWidth + m_nSpaceHorz;
				m_nTextWidth += pGlyph->nGlyphWidth + m_nSpaceHorz;
			}
		}

		pszTemp += len;
	}

	if( bInCmd && (idxCmd > 0))
	{
		wszCmd[idxCmd] = 0;

		// revision 3360
		wchar_t wstr[1024];
		i3::safe_string_copy( wstr, wszCmd, idxCmd);
		_SetText( pCtx, L"{", false);
		_SetText( pCtx, wstr, false);
	}

	wszCmd[0] = 0;

	m_LineCount++;

	_ArrangeText();

	return pCtx->_pChar;
}

INT32 i3TextAttrDX2::CalcTextWidth(const i3::wliteral_range& rng) const
{
	INT32 nTotalWidth = 0;
	
	const wchar_t* pStr = rng.c_str();

	while( *pStr)
	{
		wchar_t ch[2] = {0,0};
		INT32 len = ExtractLetterFromText(pStr, ch);			//	다국어용 멀티 바이트 문자를 추출한다.
		I3ASSERT(len > 0);

		DX2_FONT_INFO * pGlyph = m_pFont->getGlyph( ch, len);
		I3ASSERT( pGlyph != nullptr);

		// New Line글자의 경우 화면상에 보이지는 않고 칸만 변경하므로, 글자의 길이를 0으로 합니다. 
		if( ch[0] == 10) pGlyph->nGlyphWidth = 0;
	
		nTotalWidth += pGlyph->nGlyphWidth + m_nSpaceHorz;

		pStr += len;
	}

	return nTotalWidth;
}

void i3TextAttrDX2::SetText( const i3::wliteral_range& rngText, bool bParseCmd)
{
	if(	i3::generic_is_equal(rngText, m_wstrText)  )
	{
		m_bSameText = true;
		return;
	}
	else
	{
		m_bSameText = false;
	}

	i3::string_ncopy_nullpad( m_wstrText, rngText, m_MaxCount - 1);

	PARSE_CTX ctx;

	ctx._pChar = m_pCharTable;
	ctx._x = m_PosX;
	ctx._y = m_PosY;
	ctx._Flag = 0;
	i3Color::Set( &ctx._Color, &m_TextColor);

	m_Count = 0;
	m_LineCount = 0;			// 전체 Line수
	m_nTextWidth = 0;

	m_pFont->BeginEdit();

	// 문자열 길이가 짧아져도 m_DisplayStart가 재계산되지 않아,
	// 앞문자들이 짤려보여 초기화합니다. - praptor
	m_idxDisplayStart = 0;

	_SetText( &ctx, rngText, bParseCmd);

	m_pFont->EndEdit();

	addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_TEX | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_COLOR);
}

void i3TextAttrDX2::SetEnable( bool bEnable)
{
	if( bEnable)
		addState( I3_TEXT_STATE_ENABLE);
	else
		removeState( I3_TEXT_STATE_ENABLE);
}

void i3TextAttrDX2::SetPos(INT32 nX, INT32 nY)
{
	if( m_PosX != nX || m_PosY != nY)
	{
		m_PosX = nX; 
		m_PosY = nY;

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_TEX | I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetTextSpace( INT32 nX, INT32 nY)
{
	if( m_nSpaceHorz != nX || m_nSpaceVert != nY)
	{
		m_nSpaceHorz = nX;
		m_nSpaceVert = nY;

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN);
	}
}

void i3TextAttrDX2::SetOffsetPos(REAL32 fX, REAL32 fY)
{
	if( m_fOffsetPosX != fX || m_fOffsetPosY != fY)
	{
		m_fOffsetPosX = fX; 
		m_fOffsetPosY = fY;	

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN);
	}
}

void i3TextAttrDX2::SetWidth(INT32 nWidth)
{
	if( m_nWidth != nWidth)
	{
		m_nWidth = nWidth;

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_TEX | I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetHeight(INT32 nHeight)
{
	if( m_nHeight != nHeight)
	{
		m_nHeight = nHeight;

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_TEX | I3_TEXT_STATE_UPDATE_COLOR);
	}
}
void i3TextAttrDX2::SetAlign( INT32 AlignX, INT32 AlignY )
{
	if( m_AlignX != AlignX || m_AlignY != AlignY)
	{
		m_AlignX = AlignX;
		m_AlignY = AlignY;

		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_TEX);
	}
}

void i3TextAttrDX2::SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	if( r != i3Color::GetR( &m_TextColor) ||
		g != i3Color::GetG( &m_TextColor) ||
		b != i3Color::GetB( &m_TextColor) ||
		a != i3Color::GetA( &m_TextColor) )
	{
		i3Color::Set( &m_TextColor, r, g, b, a);		

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetColor( COLOR * pColor)
{
	if( i3Color::GetR(pColor) != i3Color::GetR( &m_TextColor) ||
		i3Color::GetG(pColor) != i3Color::GetG( &m_TextColor) ||
		i3Color::GetB(pColor) != i3Color::GetB( &m_TextColor) ||
		i3Color::GetA(pColor) != i3Color::GetA( &m_TextColor) )
	{
		i3Color::Copy( &m_TextColor, pColor);

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetAlpha( UINT8 nAlpha)
{
	if( nAlpha != i3Color::GetA( &m_TextColor) )
	{
		UINT8 r = i3Color::GetR( &m_TextColor);
		UINT8 g = i3Color::GetG( &m_TextColor);
		UINT8 b = i3Color::GetB( &m_TextColor);

		i3Color::Set( &m_TextColor, r, g, b, nAlpha);		

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	if( r != i3Color::GetR( &m_TextShadowColor) ||
		g != i3Color::GetG( &m_TextShadowColor) ||
		b != i3Color::GetB( &m_TextShadowColor) ||
		a != i3Color::GetA( &m_TextShadowColor) )
	{
		i3Color::Set( &m_TextShadowColor, r, g, b, a);

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetShadowColor( COLOR * pColor)
{
	if( i3Color::GetR(pColor) != i3Color::GetR( &m_TextShadowColor) ||
		i3Color::GetG(pColor) != i3Color::GetG( &m_TextShadowColor) ||
		i3Color::GetB(pColor) != i3Color::GetB( &m_TextShadowColor) ||
		i3Color::GetA(pColor) != i3Color::GetA( &m_TextShadowColor) )
	{
		i3Color::Copy( &m_TextShadowColor, pColor);

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetShadowAlpha( UINT8 nAlpha)
{
	if( nAlpha != i3Color::GetA( &m_TextShadowColor) )
	{
		UINT8 r = i3Color::GetR( &m_TextShadowColor);
		UINT8 g = i3Color::GetG( &m_TextShadowColor);
		UINT8 b = i3Color::GetB( &m_TextShadowColor);

		i3Color::Set( &m_TextShadowColor, r, g, b, nAlpha);		

		_SetColor();

		addState( I3_TEXT_STATE_UPDATE_COLOR);
	}
}

void i3TextAttrDX2::SetAutoWrapEnable( bool bEnable)
{
	bool bState = isState( I3_TEXT_STATE_AUTOWRAP);

	if( bState != bEnable)
	{
		if( bEnable)
			addState( I3_TEXT_STATE_AUTOWRAP);
		else
			removeState( I3_TEXT_STATE_AUTOWRAP);

		addState( I3_TEXT_STATE_UPDATE_POS);
	}
}

INT32 i3TextAttrDX2::GetCaretPosX(INT32 nLength)
{
	INT32 nX = 0;
	INT32 CharCount = nLength - 1;

	if( CharCount < 0) CharCount = 0;

	if( (CharCount <= m_Count) && (nLength != 0) && (m_Count != 0))
	{	
		if( CharCount == m_Count)
			CharCount = m_Count - 1;
	
		if( CharCount < m_idxDisplayStart)
			return -1;

		DISP_CHAR_INFO * pInfo = & m_pCharTable[ CharCount];
		DISP_CHAR_INFO * pInfoStart = & m_pCharTable[ m_idxDisplayStart];

		nX = (pInfo->x + pInfo->m_pGlyph->nGlyphWidth + m_nSpaceHorz) - pInfoStart->x;
	}

	return nX;
}

INT32 i3TextAttrDX2::GetCaretPosY(INT32 nLength)
{
	INT32 nY = 0;
	INT32 CharCount = nLength - 1;

	if( CharCount < 0) CharCount = 0;

	if( (CharCount <= m_Count) && (nLength != 0) && (m_Count != 0))
	{	
		if( CharCount == m_Count)
			CharCount = m_Count - 1;
	
		if( CharCount < m_idxDisplayStart)
			return -1;

		DISP_CHAR_INFO * pInfo = & m_pCharTable[ CharCount];
		DISP_CHAR_INFO * pInfoStart = & m_pCharTable[ m_idxDisplayStart];
		nY = pInfo->y - pInfoStart->y;
	}

	return (INT32) (nY+m_fOffsetPosY);
}

void i3TextAttrDX2::AdjustOffset( REAL32 diffWidth)
{
	INT32 idx = m_idxDisplayStart;
	REAL32 w = 0.0f;

	if( diffWidth < 0.0f)
	{
		// 앞으로 조절
		idx--;
		while((idx > 0) && ( i3Math::abs(w) < i3Math::abs( diffWidth)))
		{
			w += _getCharWidth( idx);
			idx--;
		}

		if( idx < 0)
			idx = 0;

		m_idxDisplayStart = idx;
	}
	else
	{
		// 뒤로 조절
		while((idx < m_Count) && (w <= i3Math::abs( diffWidth)))
		{
			w = (REAL32) m_pCharTable[ idx].x - m_pCharTable[m_idxDisplayStart].x;
			idx++;
		}

		m_idxDisplayStart = idx - 1;
	}

	addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_TEX);
}

void i3TextAttrDX2::_ArrangeText(void)
{
	INT32 lineWidth, i, lineHeight, wordLength, lineCount = 0;
	INT32 idxStart, idxEnd, idxLastWordBreak;
	DISP_CHAR_INFO * pStart, * pEnd;
	INT32 startX, startY;
	m_bIsOutChar = false;

	lineHeight = getLineHeight();

	/*
	if( i3String::NCompare( m_pszText, "독일에서", 4) == 0)
	{
		bDump = true;
	}
	*/
	switch( m_AlignY)
	{
		case ALIGN_MIDDLE :		startY = (m_PosY + (m_nHeight >> 1) - ((m_LineCount * lineHeight) >> 1));	break;
		case ALIGN_BOTTOM :		startY = (m_PosY + m_nHeight - (m_LineCount * lineHeight));	break;
		default :				startY = (m_PosY);
	}

	startY += (INT32) m_fOffsetPosY;

	// 가로 Align
	for( idxStart = m_idxDisplayStart; idxStart < m_Count;)
	{
		// 한 줄의 끝을 찾는다.
		idxEnd = idxStart;
		lineWidth = 0;
		idxLastWordBreak = idxStart;

		pStart = & m_pCharTable[ idxStart];

		while( idxEnd < m_Count)
		{
			pEnd = & m_pCharTable[ idxEnd];

			if( (idxEnd != idxStart) && ((m_pCharTable[idxEnd].m_Flag & CHAR_FLAG_LINEBREAK) != 0))
				break;

			if( pEnd->m_Flag & CHAR_FLAG_WORDBREAK)
				idxLastWordBreak = idxEnd;

			lineWidth += _getCharWidth( idxEnd);

			if( lineWidth > m_nWidth)
			{
				if( isState( I3_TEXT_STATE_AUTOWRAP))
				{
					if( ( *pEnd->m_pGlyph->wChar != L' ') && (*pEnd->m_pGlyph->wChar != L'\t'))
					{
						// LineBreak가 발견되지 않았지만, 이미 폭을 넘어갔다.

						// World Break된 위치에서부터 현재까지의 길이를 구한다.
						wordLength = _calcGlyphWidth( idxLastWordBreak, idxEnd);

						if( wordLength > (m_nWidth - wordLength))
						{
							// 단어 길이가 너무 길다. 그렇다면 현재에서 자른다.
						}
						else
						{
							idxEnd = idxLastWordBreak;
							idxEnd++;
						}
					}
					break;
				}
			}
			
			idxEnd++;
		}

		idxEnd--;

		if( idxEnd < idxStart)			idxEnd = idxStart;

		pEnd = & m_pCharTable[ idxEnd];

		lineWidth = _calcGlyphWidth( idxStart, idxEnd);

		switch( m_AlignX)
		{
			case ALIGN_CENTER :	startX = (m_PosX + (m_nWidth >> 1) - (lineWidth >> 1));	break;
			case ALIGN_RIGHT :	startX = (m_PosX + m_nWidth - lineWidth);	break;
			default :			startX = (m_PosX); break;
		}

		startX += (INT32) m_fOffsetPosX;

		bool bDisplay = false;

		for( i = idxStart; i <= idxEnd; i++)
		{
			m_pCharTable[i].x = startX;
			m_pCharTable[i].y = startY;
			m_pCharTable[i].m_Flag &= ~CHAR_FLAG_SETUP;			// 재설정해야 함을 의미. Apply()에서 해당 플랙을 제거해 주어야 한다.

			// 출력 영역의 바깥으로 나가면 Out
			if( m_bOutCharacterProcess)
			{
				if( (startX < m_PosX) || ((startX + m_pCharTable[i].m_pGlyph->nGlyphWidth) > (m_PosX + m_nWidth)))
				{
					m_pCharTable[i].m_Flag |= CHAR_FLAG_INVISIBLE;
					m_bIsOutChar = true;
				}
				else
				{
					m_pCharTable[i].m_Flag &= ~CHAR_FLAG_INVISIBLE;
					bDisplay = true;
				}		
			}
			else
			{
				m_pCharTable[i].m_Flag &= ~CHAR_FLAG_INVISIBLE;
				bDisplay = true;
			}

			// 맨앞칸의 공백은 계산하지 않습니다.
			if( i == idxStart)				
			{
				if( *m_pCharTable[i].m_pGlyph->wChar != L' ')
				{
					startX += _getCharWidth( i);
				}
				else
				{
					if( i == 0)
					{
						startX += _getCharWidth( i);						
					}
					else
					{
						startX += 1;
					}
				}
			}
			else
			{
				startX += _getCharWidth( i);
			}

/*
			if( isState( I3_TEXT_STATE_AUTOWRAP))
			{
				if( (startX < m_PosX) || ((startX + m_pCharTable[i].m_pGlyph->nGlyphWidth) > (m_PosX + m_nWidth)))
				{
					startX = (m_PosX);
					startY += lineHeight;
				}
			}
*/		}

		if( bDisplay)
		{
			// 하나라도 출력되는 문자가 있기 때문에
			lineCount++;
		}

		idxStart = idxEnd + 1;
		startY += lineHeight;
	}

	// 세로 방향의 재조정 ( Line Wrapping때문에 )
	{
		switch( m_AlignY)
		{
			case ALIGN_MIDDLE :		startY = (m_PosY + (m_nHeight >> 1) - ((lineCount * lineHeight) >> 1));	break;
			case ALIGN_BOTTOM :		startY = (m_PosY + m_nHeight - (lineCount * lineHeight));	break;
			default :				startY = (m_PosY);
		}

		startY += (INT32) m_fOffsetPosY;

		for( i = m_idxDisplayStart; i < m_Count; i++)
		{
			m_pCharTable[i].y = startY;

			// 출력 영역의 바깥으로 나가면 Out
			if( m_bOutCharacterProcess)
			{
				if( (startY < m_PosY) || ((startY + m_pCharTable[i].m_pGlyph->nGlyphHeight) > m_PosY + m_nHeight))
				{
					m_pCharTable[i].m_Flag |= CHAR_FLAG_INVISIBLE;
					m_bIsOutChar = true;
				}
			}
			

			if( i < m_Count - 1)
			{
				if( m_pCharTable[i+1].x < m_pCharTable[i].x || ( m_pCharTable[i].x == m_PosX+m_fOffsetPosX && *m_pCharTable[i+1].m_pGlyph->wChar == 10))
				{
					// 줄 바뀐다.
					startY += lineHeight;
					m_LineCount ++;
				}
			}
		}
	}

	m_VisibleLineCount = lineCount;
}

void i3TextAttrDX2::_SetColor(void)
{
	// m_TextColor를 m_pCharTable에 적용하는 시점이 SetText()에서만 이루어져,
	// SetColor()시에는 변경이 적용되지 않아 별도로 적용합니다.

	for(INT32 i = 0; i < m_Count; i++)
	{
		i3Color::Set(&m_pCharTable[i].color, &m_TextColor);
	}
}

static REAL32	s_ShadowOffTable[4][2] =
{
	{	-1.0f,	-1.0f},
	{	1.0f,	-1.0f},
	{	-1.0f,	1.0f},
	{	1.0f,	1.0f}
};

inline void _setPos( i3VertexArray * pVA, INT32 idxChar, INT32 stride, DISP_CHAR_INFO * pInfo, bool bShadow, INT32 shadowQuality)
{
	INT32 idxVtx, off = 0;

	REAL32 x1 = (REAL32)( pInfo->x);
	REAL32 x2 = (REAL32)( pInfo->x + pInfo->m_pGlyph->nGlyphWidth);
	REAL32 y1 = (REAL32)( pInfo->y);
	REAL32 y2 = (REAL32)( pInfo->y + pInfo->m_pGlyph->nGlyphHeight);

	// DirectX에서는 최종 좌표값을 정수화 한 다음 0.5를 빼주어야 픽셀과 화면이 1:1로 대응한다.
	// 원래 최종적으로 나온 결과값으로 계산하는 게 맞지만,
	// 대략 입력된 좌표값이 정수라는 가정하에 중심좌표에서 0.5를 빼서 맞춘다.
	x1 -= 0.5f;
	x2 -= 0.5f;
	y1 -= 0.5f;
	y2 -= 0.5f;

	VEC3D pos[6];

	idxVtx = (stride * idxChar);

	if( bShadow)
	{
		INT32 i;

		switch( shadowQuality)
		{		
		case FONT_SHADOW_QUALITY_HIGH:				
		case FONT_SHADOW_QUALITY_NORMAL:
			// 바깥으로 살짝 Stretch 시킨 사각형
			for( i = 0; i < 4; i++)
			{
				i3Vector::Set(  &pos[0], x1 + s_ShadowOffTable[i][0], y1 + s_ShadowOffTable[i][1], 0.0f );
				i3Vector::Set(  &pos[1], x2 + s_ShadowOffTable[i][0], y1 + s_ShadowOffTable[i][1], 0.0f );
				i3Vector::Set(  &pos[2], x2 + s_ShadowOffTable[i][0], y2 + s_ShadowOffTable[i][1], 0.0f ); 
				i3Vector::Set(  &pos[3], x1 + s_ShadowOffTable[i][0], y2 + s_ShadowOffTable[i][1], 0.0f );
				i3Vector::Copy( &pos[4], &pos[0] );
				i3Vector::Copy( &pos[5], &pos[2] );

				pVA->SetPositions( s_nVBDrawIndex + idxVtx + (i * 6), 6, pos);
			}
			off = 4;
			break;	

		default:
			// 우 하측으로 살짝 밀어낸 사각형
			i3Vector::Set(  &pos[0], x1 + 1.0f, y1 + 1.0f, 0.0f );
			i3Vector::Set(  &pos[1], x2 + 1.0f, y1 + 1.0f, 0.0f );
			i3Vector::Set(  &pos[2], x2 + 1.0f, y2 + 1.0f, 0.0f ); 
			i3Vector::Set(  &pos[3], x1 + 1.0f, y2 + 1.0f, 0.0f );
			i3Vector::Copy( &pos[4], &pos[0] );
			i3Vector::Copy( &pos[5], &pos[2] );

			pVA->SetPositions( s_nVBDrawIndex + idxVtx, 6, pos);
			off = 1;
			break;
		}
	}

	i3Vector::Set(  &pos[0], x1, y1, 0.0f );
	i3Vector::Set(  &pos[1], x2, y1, 0.0f );
	i3Vector::Set(  &pos[2], x2, y2, 0.0f ); 
	i3Vector::Set(  &pos[3], x1, y2, 0.0f );
	i3Vector::Copy( &pos[4], &pos[0] );
	i3Vector::Copy( &pos[5], &pos[2] );

	pVA->SetPositions( s_nVBDrawIndex + idxVtx + (off * 6), 6, pos );
}

inline void _setUV( i3VertexArray * pVA, INT32 idxChar, INT32 stride, DISP_CHAR_INFO * pInfo, bool bShadow, INT32 shadowQuality)
{
	INT32 idxVtx, off = 0;
	REAL32 u1, v1, u2, v2;
	DX2_FONT_INFO * pFont = pInfo->m_pGlyph;
	REAL32 rcx, rcy;

	u1 = (REAL32)(pFont->nStartX);	
	u2 = (REAL32)(u1 + pFont->nGlyphWidth);	
	v1 = (REAL32)(pFont->nStartY);
	v2 = (REAL32)(v1 + pFont->nGlyphHeight);
	
	rcx = i3Math::Reciprocal( (REAL32)(FONT_TEXTURE_WIDTH));
	rcy = i3Math::Reciprocal( (REAL32)(FONT_TEXTURE_HEIGHT));

	u1 *= rcx;	
	u2 *= rcx;
	v1 *= rcy;
	v2 *= rcy;

	idxVtx = (idxChar * stride);

	VEC2D uv[6];

	i3Vector::Set(  &uv[0], u1, v1 );
	i3Vector::Set(  &uv[1], u2, v1 );
	i3Vector::Set(  &uv[2], u2, v2 );
	i3Vector::Set(  &uv[3], u1, v2 );
	i3Vector::Copy( &uv[4], &uv[0] );
	i3Vector::Copy( &uv[5], &uv[2] );

	if( bShadow)
	{
		INT32 i;

		switch( shadowQuality)
		{		
		case FONT_SHADOW_QUALITY_HIGH:				
		case FONT_SHADOW_QUALITY_NORMAL:
			// 바깥으로 살짝 Stretch 시킨 사각형
			for( i = 0; i < 4; i++)
			{
				pVA->SetTextureCoords( 0, s_nVBDrawIndex + idxVtx + (i * 6), 6, uv);
			}
			off = 4;
			break;	

		default:
			// 우 하측으로 살짝 밀어낸 사각형
			pVA->SetTextureCoords( 0, s_nVBDrawIndex + idxVtx, 6, uv);
			off = 1;
			break;
		}
	}

	pVA->SetTextureCoords( 0, s_nVBDrawIndex + idxVtx + (off * 6), 6, uv);
}

inline void _setColor( i3VertexArray * pVA, INT32 idxChar, INT32 stride, DISP_CHAR_INFO * pInfo, bool bShadow, COLOR * pShadowColor, INT32 shadowQuality)
{
	INT32 idxVtx = (idxChar * stride);
	INT32 off = 0;

	if( bShadow)
	{
		INT32 i;

		switch( shadowQuality)
		{		
		case FONT_SHADOW_QUALITY_HIGH:				
		case FONT_SHADOW_QUALITY_NORMAL:
			// 바깥으로 살짝 Stretch 시킨 사각형
			for( i = 0; i < 4; i++)
			{
				pVA->SetColor( s_nVBDrawIndex + idxVtx + (i * 6), 6, pShadowColor);
			}
			off = 4;
			break;	

		default:
			// 우 하측으로 살짝 밀어낸 사각형
			pVA->SetColor( s_nVBDrawIndex + idxVtx, 6, pShadowColor);
			off = 1;
			break;
		}
	}

	pVA->SetColor( s_nVBDrawIndex + idxVtx + (off * 6), 6, & pInfo->color);
}


void i3TextAttrDX2::ConnectVertexBank( UINT32 nCount)
{
	m_pVBElement = m_pContext->getVertexBank()->GetVertexArray( &m_format, nCount * m_VtxStride);
}

void i3TextAttrDX2::Apply( i3RenderContext * pContext)
{
	UINT32 nStartIdx = 0;	
	s_nVBDrawIndex= 0;

	m_pContext = pContext;

	if( pContext->isContextRivieved())
	{
		addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_COLOR | I3_TEXT_STATE_UPDATE_TEX);
	}

	if( m_Count <= 0)
		return;

	if( m_bUseVertexBank) 
	{		
		ConnectVertexBank( m_Count);
		I3_REF_CHANGE( m_pVA, m_pVBElement->m_pVA);
		nStartIdx = m_pVBElement->m_nCurIdx;			
		s_nVBDrawIndex = nStartIdx;

		addState( I3_TEXT_STATE_UPDATE_POS);
		addState( I3_TEXT_STATE_UPDATE_TEX);
		addState( I3_TEXT_STATE_UPDATE_COLOR);		
	}

	if( isState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_COLOR | I3_TEXT_STATE_UPDATE_TEX))
	{
		_ArrangeText();

		INT32 i, CharCount;
		INT32 idxStart, TexIndex;
		bool bBreak, bShadow = isState( I3_TEXT_STATE_SHADOW);

		m_DrawGroupCount = 0;

		if( m_bUseVertexBank) 
		{
			// 이부분을 주석으로 막은이유는 특정 그래픽카드(ATI..)에서 깜빡임 현상이 발생하기때문입니다.
//			if( nStartIdx == 0)
//			{
//				m_pVA->Lock(I3G_LOCK_OVERWRITE, nStartIdx, m_Count * m_VtxStride);
//			}
//			else
//			{
				//m_pVA->Lock(I3G_LOCK_NOOVERWRITE, nStartIdx, m_Count * m_VtxStride);
				m_pVA->Lock(0, nStartIdx, m_Count * m_VtxStride);
//			}
		}
		else
		{
			//m_pVA->Lock(I3G_LOCK_NOOVERWRITE, nStartIdx, m_Count * m_VtxStride);
			//m_pVA->Lock(I3G_LOCK_NOOVERWRITE);
			m_pVA->Lock(0);
		}

		// 빠른 Rendering을 위해, 동일한 Texture를 사용하는 글자들끼리 모은다.
		// 물론 글자들의 위치가 반드시 연결되어 있어야 할 필요는 없다.
		for( idxStart = m_idxDisplayStart, CharCount = 0; idxStart < m_Count; m_DrawGroupCount++)
		{
			// 첫번째 Char와 동일한 Texture를 사용하는 모든 후속 Char들을 모은다.
			bBreak = false;
			m_DrawGroup[ m_DrawGroupCount].m_cntPrim = 0;
			m_DrawGroup[ m_DrawGroupCount].m_idxCharStart = CharCount;
			m_DrawGroup[ m_DrawGroupCount].m_TextureIndex = m_pCharTable[ idxStart].m_pGlyph->nTextureIdx;

			TexIndex = m_pCharTable[ idxStart].m_pGlyph->nTextureIdx;

			for( i = idxStart; i < m_Count; i++)
			{
				if( m_pCharTable[i].m_pGlyph->nTextureIdx == TexIndex)
				{
					// 동일한 Texture를 사용한다.
					m_pCharTable[i].m_Flag |= CHAR_FLAG_SETUP;			// 설정되었음을 표시해둔다.

					if( (m_pCharTable[i].m_Flag & CHAR_FLAG_INVISIBLE) == 0)
					/*if( (i3Language::GetCurrentCodePage() != I3_LANG_CODE_KOREAN) ||
						(m_pCharTable[i].m_Flag & CHAR_FLAG_INVISIBLE) == 0)*/
					{
						if( *m_pCharTable[i].m_pGlyph->wChar != 10)	// New Line글자의 경우 화면상에 보여질 필요가 없으므로 Pass합니다.
						{
							m_DrawGroup[ m_DrawGroupCount].m_cntPrim += (m_VtxStride / 3);

							if( isState( I3_TEXT_STATE_UPDATE_POS))
							{
								_setPos( m_pVA, CharCount, m_VtxStride, &m_pCharTable[i], bShadow, m_nShadowQuality);
							}

							if( isState( I3_TEXT_STATE_UPDATE_TEX))
							{
								_setUV( m_pVA, CharCount, m_VtxStride, &m_pCharTable[i], bShadow, m_nShadowQuality);
							}

							if( isState( I3_TEXT_STATE_UPDATE_COLOR))
							{						
								_setColor( m_pVA, CharCount, m_VtxStride, &m_pCharTable[i], bShadow, &m_TextShadowColor, m_nShadowQuality);
							}

							CharCount++;
						}
					}

					if( bBreak == false)
						idxStart = i + 1;
				}
				else
				{
					// 다른 Texture를 사용한다.
					if( bBreak == false)
					{
						// 시작에서부터 처음으로 다른 Texture를 사용하는 Char가 검색되었다.
						bBreak = true;
					}
				}

			}

			// 다음 검색 대상 물색
			for( ; idxStart < m_Count; idxStart++)
			{
				if((m_pCharTable[idxStart].m_Flag & CHAR_FLAG_SETUP) == 0)
					break;
			}
		}

		m_pVA->Unlock();

		removeState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_COLOR | I3_TEXT_STATE_UPDATE_TEX);
	}

	// Draw
	if( m_DrawGroupCount > 0)
	{
		pContext->SetVertexArray( m_pVA);

		//I3G_TEXTURE_FILTER magFilter = pContext->getTextureFilterMag( I3G_TEXTURE_BIND_DIFFUSE);
		//I3G_TEXTURE_FILTER minFilter = pContext->getTextureFilterMin( I3G_TEXTURE_BIND_DIFFUSE);

		//pContext->SetTextureFilter( I3G_TEXTURE_BIND_DIFFUSE, I3G_TEXTURE_FILTER_POINT, I3G_TEXTURE_FILTER_POINT);

		for( INT32 i = 0; i < m_DrawGroupCount; i++)
		{
			DRAW_GROUP * pDraw = &m_DrawGroup[i];
			i3Texture * pTex;

			pTex = m_pFont->getTexture( pDraw->m_TextureIndex);
			I3ASSERT( pTex != nullptr);

			pContext->SetTexture( I3G_TEXTURE_BIND_DIFFUSE, pTex);

			m_pDrawShader = pContext->DrawPrim( m_pDrawShader, I3G_PRIM_TRILIST, s_nVBDrawIndex + (pDraw->m_idxCharStart * m_VtxStride), pDraw->m_cntPrim);
		}

		//pContext->SetTextureFilter( I3G_TEXTURE_BIND_DIFFUSE, magFilter, minFilter);
	}
}

INT32 i3TextAttrDX2::GetCharacterSizeX( const i3::wliteral_range& rng ) const
{
	if( rng.empty() )
		return 0; 

	wchar_t ch[2] = {0,0};
	INT32 len = ExtractLetterFromText( rng.c_str(), ch);

	//	다국어용 멀티 바이트 문자를 추출한다.

	DX2_FONT_INFO * pGlyph = m_pFont->findGlyph( ch, len);

	// 등록되어 있지 않은 폰트라면 등록합니다.
	if( pGlyph == nullptr)
	{
		pGlyph = m_pFont->getGlyph( ch, len);
	}

	I3ASSERT( pGlyph != nullptr);

	return pGlyph->nGlyphWidth;
}

UINT32 i3TextAttrDX2::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc = 0;
	return Rc;
}

UINT32 i3TextAttrDX2::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc = 0;
	return Rc;
}

void i3TextAttrDX2::OnLostDevice( bool bLostDevice)
{
	if( m_bUseVertexBank == false)
	{
		I3_SAFE_RELEASE( m_pVA);
	}

	m_pDrawShader = nullptr;
}

void i3TextAttrDX2::OnRevive( i3RenderContext * pCtx)
{
	_CreateVertex();

	m_pDrawShader = nullptr;

	addState( I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_COLOR | I3_TEXT_STATE_UPDATE_TEX);
}

#if defined( I3_DEBUG)
void i3TextAttrDX2::Dump(void)
{

}
#endif
