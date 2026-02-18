#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3TextAttrDX.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3TextAttrDX);

i3TextAttrDX::i3TextAttrDX(void)
{
	SetID( I3_ATTRID_TEXTDX);
}

i3TextAttrDX::~i3TextAttrDX(void)
{
	I3_SAFE_RELEASE( m_pGfxFontDX);
	I3MEM_SAFE_FREE_POINTER(m_pTextBuf);
}

bool i3TextAttrDX::Create( const char* pszFont, INT32 nTextMaxCount, INT32 nSize, UINT32 nWeight, bool bItalic, DWORD dwQuality)
{
	INT32 nHeight = 0;

	m_nTextMaxCount = nTextMaxCount;

	m_Weight = nWeight;

	if( m_pTextBuf) i3MemFree(m_pTextBuf);
	m_pTextBuf = (char*)i3MemAlloc(m_nTextMaxCount);
	i3mem::FillZero( m_pTextBuf, sizeof(char)*m_nTextMaxCount);

	nHeight = CalculateFontSize( nSize);
	m_pGfxFontDX = i3GfxFontDX::new_object();
	m_pGfxFontDX->Create( g_pD3DDevice, pszFont, nHeight, nWeight, bItalic, dwQuality);

	// Default로 Attr의 Position를 설정합니다(초기 값임...)
	i3Vector::Set(&m_Position, (REAL32)m_PosX, (REAL32)m_PosY);

	// Default로 Attr의 Width와 Height를 설정합니다(임의의 값임...)
	INT32 fCharAvgWidth = GetCharacterSizeX("Z");
	INT32 fCharAvgHeight = GetCharacterSizeY("O");
	m_nWidth = fCharAvgWidth * 500;
	m_nHeight = fCharAvgHeight;

	return true;
}


bool i3TextAttrDX::SetText( const char *szText )
{
	if( szText == nullptr )
	{
		szText = "";
	}
	
	INT32 nLength = (UINT32)i3::generic_string_size( szText );
	if( nLength > m_nTextMaxCount-1 )
	{
		I3TRACE( "[%s] Exceed String Buffer Size. (%d > %d)\n", __FUNCTION__, nLength, m_nTextMaxCount );
		I3ASSERT_0;
		return false;
	}

	// 문자열 길이가 기존과 같은 경우 같은 문자열인지 검사
	if( nLength == m_curTextBufCount )
	{
		// 문자열 내용이 기존과 같은 경우인지 검사
		if( i3::generic_is_iequal( m_pTextBuf, szText ) )
		{
			return true;
		}
	}
	
	// 문자열 길이가 기존과 다르며, 내용이 틀릴경우
	{
		m_curTextBufCount = nLength;
		i3mem::FillZero( m_pTextBuf, sizeof(char)*m_nTextMaxCount);
		i3::string_ncopy_nullpad( m_pTextBuf, szText, sizeof(char)*m_curTextBufCount);

		SIZE Size;
		::GetTextExtentPoint32(m_pGfxFontDX->getFont()->GetDC(), m_pTextBuf, i3::generic_string_size( m_pTextBuf), &Size);
		m_nTextWidth = Size.cx;
		m_nTextHeight = Size.cy;
	}

	return true;
}

void i3TextAttrDX::DrawText(void)
{
	UINT32 DrawFormatFlag = 0;

	// Text Rect에 맞추어서 자동적으로 행이 바뀝니다.
	if( m_bAutoWrap)
	{
		DrawFormatFlag = DT_WORDBREAK;	
	}

	// Align형식에 맞추어서 각 라인마다 Text의 초기위치를 변경합니다.
	{		
		
		switch(m_AlignY)
		{
		case ALIGN_TOP:			DrawFormatFlag |= DT_TOP;		break;
		case ALIGN_MIDDLE:		DrawFormatFlag |= DT_VCENTER;	break;
		case ALIGN_BOTTOM:		DrawFormatFlag |= DT_BOTTOM;	break;
		}

		switch(m_AlignX)
		{
		case ALIGN_LEFT:		DrawFormatFlag |= DT_LEFT;		break;
		case ALIGN_CENTER:		DrawFormatFlag |= DT_CENTER;	break;
		case ALIGN_RIGHT:		DrawFormatFlag |= DT_RIGHT;		break;
		}
	}

	// Text를 그립니다.
	{
		if( m_bShadowEnable)
		{
			INT32 nPosOffset = 1;
			RECT TextShadowArea;

			switch( m_nShadowQuality)
			{
			case FONT_SHADOW_QUALITY_HIGH:
				// 그림자 (좌하)
				//m_pGfxFontDX->getSprite();
				SetRect( &TextShadowArea, m_PosX+nPosOffset, m_PosY+nPosOffset, m_PosX+m_nWidth+nPosOffset, m_PosY+m_nHeight+nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												  i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (우상)
				SetRect( &TextShadowArea, m_PosX-nPosOffset, m_PosY-nPosOffset, m_PosX+m_nWidth-nPosOffset, m_PosY+m_nHeight-nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (좌상)
				SetRect( &TextShadowArea, m_PosX+nPosOffset, m_PosY-nPosOffset, m_PosX+m_nWidth+nPosOffset, m_PosY+m_nHeight-nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (우하)
				SetRect( &TextShadowArea, m_PosX-nPosOffset, m_PosY+nPosOffset, m_PosX+m_nWidth-nPosOffset, m_PosY+m_nHeight+nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));
			case FONT_SHADOW_QUALITY_NORMAL:
				// 그림자 (상)
				SetRect( &TextShadowArea, m_PosX, m_PosY-nPosOffset, m_PosX+m_nWidth, m_PosY+m_nHeight-nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (하)
				SetRect( &TextShadowArea, m_PosX, m_PosY+nPosOffset, m_PosX+m_nWidth, m_PosY+m_nHeight+nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (좌)
				SetRect( &TextShadowArea, m_PosX-nPosOffset, m_PosY, m_PosX+m_nWidth-nPosOffset, m_PosY+m_nHeight );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));

				// 그림자 (우)
				SetRect( &TextShadowArea,m_PosX+nPosOffset, m_PosY, m_PosX+m_nWidth+nPosOffset, m_PosY+m_nHeight );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));
				break;
			case FONT_SHADOW_QUALITY_LOW:
				// 그림자 (좌하)
				SetRect( &TextShadowArea, m_PosX+nPosOffset, m_PosY+nPosOffset, m_PosX+m_nWidth+nPosOffset, m_PosY+m_nHeight+nPosOffset );
				m_pGfxFontDX->getFont()->DrawText(nullptr, m_pTextBuf, -1, &TextShadowArea, DrawFormatFlag, 
									D3DCOLOR_ARGB(i3Color::GetA(&m_TextShadowColor), i3Color::GetR(&m_TextShadowColor),
												  i3Color::GetG(&m_TextShadowColor), i3Color::GetB(&m_TextShadowColor)));
				break;
			}
		}

		// 내용
		RECT TextArea =			{ m_PosX, m_PosY, m_PosX+m_nWidth, m_PosY+m_nHeight };
		m_pGfxFontDX->getFont()->DrawText( nullptr, m_pTextBuf, -1, &TextArea, DrawFormatFlag, 
								D3DCOLOR_ARGB(i3Color::GetA(&m_TextColor), i3Color::GetR(&m_TextColor),
											  i3Color::GetG(&m_TextColor), i3Color::GetB(&m_TextColor)));

//		m_pGfxFontDX->getFont()->DrawText(m_pGfxFontDX->getSprite(), m_pTextBuf, -1, &TextArea, DrawFormatFlag, 
//								D3DCOLOR_ARGB(i3Color::GetA(&m_TextColor), i3Color::GetR(&m_TextColor),
//											  i3Color::GetG(&m_TextColor), i3Color::GetB(&m_TextColor)));
	}
}

//////////////////////////////////////////////////////////////////////////
// 폰트의 크기를 계산함
//////////////////////////////////////////////////////////////////////////
INT32 i3TextAttrDX::CalculateFontSize( INT32 nSize)
{
	HDC hDC = ::GetDC( NULL );
	INT32 i = -( MulDiv( nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );
	::ReleaseDC( NULL, hDC );

	return i;
}

void i3TextAttrDX::setEnable( bool bEnable)					
{ 
	m_bEnable = bEnable;	
}

void i3TextAttrDX::setPos(UINT32 nX, UINT32 nY)						
{ 
	m_PosX = nX; 
	m_PosY = nY;	

	i3Vector::Set(&m_Position, (REAL32)nX, (REAL32)nY);
}

void i3TextAttrDX::setWidth(UINT32 nWidth)								
{
	m_nWidth = nWidth;	
}

void i3TextAttrDX::setHeight(UINT32 nHeight)							
{
	m_nHeight = nHeight;	
}

void i3TextAttrDX::setShadowEnable( bool bEnable, FONT_SHADOW_QUALITY nQuality)
{
	m_bShadowEnable = bEnable;
	m_nShadowQuality = nQuality;
}

void i3TextAttrDX::setAutoWrapEnable( bool bEnable)
{
	m_bAutoWrap = bEnable;
}

void i3TextAttrDX::SetAlign( UINT32 AlignX, UINT32 AlignY )
{
	m_AlignX = AlignX;
	m_AlignY = AlignY;
}

void i3TextAttrDX::SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	i3Color::Set( &m_TextColor, r, g, b, a);
}

void i3TextAttrDX::SetColor( COLOR * pColor)
{
	i3Color::Copy( &m_TextColor, pColor);
}

void i3TextAttrDX::SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	i3Color::Set( &m_TextShadowColor, r, g, b, a);
}

void i3TextAttrDX::SetShadowColor( COLOR * pColor)
{
	i3Color::Copy( &m_TextShadowColor, pColor);
}

void i3TextAttrDX::Apply( i3RenderContext * pContext)
{
	if( m_bEnable)
	{
		if( m_curTextBufCount > 0)
		{
			//m_pGfxFontDX->getSprite()->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

			// Text표시!
			DrawText();
	
			//m_pGfxFontDX->getSprite()->End();
		}
	}
}

VEC2D *  i3TextAttrDX::GetPos(void)
{
	return &m_Position;
}

INT32  i3TextAttrDX::GetCaretPosX(INT32 nLength)
{
	if( nLength > 0)
	{
		char szTemp[1024] = {0,};
		i3::string_ncopy_nullpad( szTemp, m_pTextBuf, sizeof(char)*nLength);

		SIZE Size;
		::GetTextExtentPoint32(m_pGfxFontDX->getFont()->GetDC(), szTemp, i3::generic_string_size( szTemp), &Size);

		return Size.cx;
	}

	return 0;
}

INT32  i3TextAttrDX::GetCharacterSizeX( const char *pszText)
{
	SIZE Size;

	LPD3DXFONT pFont = m_pGfxFontDX->getFont();

	::GetTextExtentPoint32(pFont->GetDC(), pszText, i3::generic_string_size( pszText), &Size);

	return Size.cx;
}

INT32  i3TextAttrDX::GetCharacterSizeY( const char *pszText)
{
	SIZE Size;
	::GetTextExtentPoint32(m_pGfxFontDX->getFont()->GetDC(), pszText, i3::generic_string_size( pszText), &Size);

	return Size.cy;
}

UINT32 i3TextAttrDX::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc = 0;
	return Rc;
}

UINT32 i3TextAttrDX::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc = 0;
	return Rc;
}

#if defined( I3_DEBUG)
void i3TextAttrDX::Dump(void)
{
}
#endif
