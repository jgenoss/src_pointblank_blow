#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3TextAttr.h"

#include "i3Font.h"
#include <string.h>
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/atoi.h"

#if defined( I3G_PSP)
	#define		I3_GFXCAPS_TRANSFORMED_VERTEX			1
#else
	#define		I3_GFXCAPS_TRANSFORMED_VERTEX			0
#endif

#define	TEXT_LOCK		(I3G_LOCK_NOOVERWRITE)

// ============================================================================
//
// i3TextInfo
//
// ============================================================================

I3_CLASS_INSTANCE( i3TextInfo);

bool i3TextInfo::Create( i3Font *pFont )
{
	I3ASSERT( pFont != nullptr );
	m_pFont = pFont;
	return true;
}

void i3TextInfo::SetChar( WCHAR16 wCh )
{
	I3ASSERT( wCh > 0 );
	m_wChar = wCh;
	m_pFont->GetGlyphInfoByChar( &m_GlyphInfo, wCh );
}

void i3TextInfo::SetColor( COLOR *pColor )
{
	i3Color::Set( &m_Color, pColor );
}

void i3TextInfo::SetPosition( INT32 x, INT32 y )
{
	m_X = x;
	m_Y = y;
}


#if I3_GFXCAPS_TRANSFORMED_VERTEX
	void i3TextInfo::GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex)
#else
	void i3TextInfo::GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex, REAL32 rcx, REAL32 rcy)
#endif
{
#ifdef I3_DEBUG_TEXTATTR
	if( g_bDebugTextAttr )	
	{
		I3TRACE( "%s : %d\n", __FUNCTION__, nIndex );
		I3TRACE( "m_wChar = %d\n", m_wChar );
		I3TRACE( "m_X = %d\n", m_X );
		I3TRACE( "m_Y = %d\n", m_Y );
		I3TRACE( "nWidth = %d\n", m_GlyphInfo.nWidth );
		I3TRACE( "nHeight = %d\n", m_GlyphInfo.nHeight );
	}
#endif

	// Position
	{
		REAL32 x1 = (REAL32)m_X;
		REAL32 x2 = (REAL32)(m_X + (INT32)(m_GlyphInfo.nWidth * m_fScaleX));
		REAL32 y1 = (REAL32)m_Y;
		REAL32 y2 = (REAL32)(m_Y + (INT32)(m_GlyphInfo.nHeight * m_fScaleY));

#if defined( I3G_DX ) || defined( I3G_PSP )
		x1 -= 0.5f;
		x2 -= 0.5f;
		y1 -= 0.5f;
		y2 -= 0.5f;
#endif

		#if ! I3_GFXCAPS_TRANSFORMED_VERTEX
			#if defined( I3_FIXED_POINT)
				x1 = ( x1 / rcx) - __RT_1;				// Clipping Space Coordinates로 변환
				y1 = -(( y1 / rcy) - __RT_1);
				x2 = ( x2 / rcx) - __RT_1;
				y2 = -(( y2 / rcy) - __RT_1);
			#else
				x1 = i3Math::Mul( x1, rcx) - 1.0f;				// Clipping Space Coordinates로 변환
				y1 = -(i3Math::Mul( y1, rcy) - 1.0f);
				x2 = i3Math::Mul( x2, rcx) - 1.0f;
				y2 = -(i3Math::Mul( y2, rcy) - 1.0f);
			#endif
		#endif

		VEC3D vecPos[6];
		i3Vector::Set( &vecPos[0], x1, y1, 0.0f );
		i3Vector::Set( &vecPos[1], x2, y1, 0.0f );
		i3Vector::Set( &vecPos[2], x2, y2, 0.0f ); 
		i3Vector::Set( &vecPos[3], x1, y2, 0.0f );
		i3Vector::Copy( &vecPos[4], &vecPos[0] );
		i3Vector::Copy( &vecPos[5], &vecPos[2] );

		pVA->SetPositions( nIndex, 6, vecPos );
	}

	// UV
	{
		REAL32 u1, v1, u2, v2;

		u1 = i3Vector::GetU( &m_GlyphInfo.UV[0]);
		v1 = i3Vector::GetV( &m_GlyphInfo.UV[0]);
		u2 = i3Vector::GetU( &m_GlyphInfo.UV[1]);
		v2 = i3Vector::GetV( &m_GlyphInfo.UV[1]);

		VEC2D uv[6];

		i3Vector::Set( &uv[0], u1, v1 );
		i3Vector::Set( &uv[1], u2, v1 );
		i3Vector::Set( &uv[2], u2, v2 );
		i3Vector::Set( &uv[3], u1, v2 );
		i3Vector::Copy( &uv[4], &uv[0] );
		i3Vector::Copy( &uv[5], &uv[2] );

		pVA->SetTextureCoords( 0, nIndex, 6, uv );
	}

	// Color
	pVA->SetColor( nIndex, 6, &m_Color );
}

// Effect
void i3TextInfo::SetEffect( INT32 nIndex, REAL32 rTime )
{
}

bool i3TextInfo::UpdateEffect( REAL32 fDeltaSeconds )
{
	return false; // 업데이트할 이펙트가 없음.
}

void i3TextInfo::GetEffectVertexArray( i3VertexArray *pVA, UINT32 &nIndex )
{
}

// ============================================================================
//
// i3TextAttr
//
// ============================================================================

I3_CLASS_INSTANCE( i3TextAttr);

i3TextAttr::i3TextAttr()
{
	SetID( I3_ATTRID_TEXT );
}

i3TextAttr::~i3TextAttr()
{
	I3_SAFE_RELEASE( m_pVA[0] );
	I3_SAFE_RELEASE( m_pVA[1] );

	//I3MEM_SAFE_FREE( m_pCharCount, m_hCharCountMemID );
	I3MEM_SAFE_FREE_POINTER( m_pCharCount ); 

	for(size_t i = 0; i < m_TextInfoList.size(); i++ )
	{
		i3TextInfo *pTextInfo = m_TextInfoList[i];
        I3_SAFE_RELEASE( pTextInfo );
	}
	
	I3MEM_SAFE_FREE_POINTER( m_pTextLineArray );	
}

bool i3TextAttr::Create( i3Font *pFont, UINT32 nMaxCount, bool bAlwaysUpdate, UINT32 nMaxLine )
{
//	I3HMEM hMem;

	I3ASSERT( pFont != nullptr );
	m_pFont = pFont;

	i3VertexFormat format;

	I3ASSERT( nMaxCount > 0 );
	m_MaxCount = nMaxCount;
	m_bAlwaysUpdate = true;
	m_nNumTexture = m_pFont->GetNumTexture();

	// Line Count Buffer
	I3ASSERT( m_pTextLineArray == nullptr );

	//hMem = i3MemAlloc( sizeof(UINT32) * nMaxLine);
	m_pTextLineArray = (UINT32 *) i3MemAlloc( sizeof(UINT32) * nMaxLine);

	m_nMaxLineArray = nMaxLine;

	// Text
//	I3ASSERT( m_pText == nullptr );
//	I3ASSERT( m_pTempText == nullptr );
//	m_pText = (WCHAR16 *) i3MemAlloc( sizeof(WCHAR16) * (m_MaxCount + 1));
//	m_pTempText = ( WCHAR16 *) i3MemAlloc( sizeof(WCHAR16) * (m_MaxCount + 1));
//	memset( m_pText, 0, sizeof(WCHAR16) * (m_MaxCount + 1) );
//	memset( m_pTempText, 0, sizeof(WCHAR16) * (m_MaxCount + 1) );

	// Font Text
	UINT32 i;
	for( i = 0; i < m_MaxCount; i++)
	{
		i3TextInfo *pTextInfo = i3TextInfo::new_object();
		pTextInfo->Create( m_pFont );
		m_TextInfoList.push_back( pTextInfo );
	}

	I3ASSERT( m_pCharCount == nullptr );

	m_pCharCount = (UINT32 *) i3MemAlloc( sizeof(UINT32) * m_nNumTexture);

#if 0
	{
		// 메모리 누수 탐지
		void *pCheck = m_pCharCount;
		static int nCount = 0;
		static int nCheck = -1;
		if( nCheck == nCount )
		{
			g_ni3DebugIndex = m_hCharCountMemID;
			g_pi3DebugPointer = this;
			I3ASSERT_0;
		}
		I3TRACE( __FUNCTION__ " : %d, %p\n", nCount, pCheck );
		nCount++;
	}
#endif

	// Vertex format
	format.SetHasPosition();
	format.SetHasColor();
	format.SetTextureCoordSetCount( 1 );
	format.SetTextureCoordCount( 2 );

	#if I3_GFXCAPS_TRANSFORMED_VERTEX
		format.SetIsTransformed();
	#endif

	m_pVA[0] = i3VertexArray::new_object();
	m_pVA[1] = i3VertexArray::new_object();

	// 버텍스 버퍼를 두개를 준비해서 Double Buffer 방식으로 Update 및 Rendering을 한다.
	m_pVA[0]->Create( &format, m_MaxCount * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
	m_pVA[1]->Create( &format, m_MaxCount * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));

	m_bEdited = true;

	SetEnable(true);

	return true;
}

void i3TextAttr::SetEnable( bool bFlag )
{
	m_bEnable = bFlag;
	m_bEdited = true;
}

void i3TextAttr::SetAlign( UINT32 AlignX, UINT32 AlignY )
{
	m_AlignX = AlignX;
	m_AlignY = AlignY;
	m_bEdited = true;
}

void i3TextAttr::SetAutoWrap( bool bTrue, UINT32 nMaxWidth )
{
	m_bAutoWrap = bTrue;
	m_MaxLineWidth = nMaxWidth;
	m_bEdited = true;
}

void i3TextAttr::SetPos( VEC2D *pPos )
{
	i3Vector::Copy( &m_Position, pPos );
	m_bEdited = true;
}

void i3TextAttr::SetPos( REAL32 x, REAL32 y )
{
	i3Vector::Set( &m_Position, x, y );
	m_bEdited = true;
}

void i3TextAttr::SetColor( COLOR * pColor)
{
	i3Color::Set( &m_Color, pColor);
	m_bEdited = true;
}
void i3TextAttr::SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	i3Color::Set( &m_Color, r, g, b, a);
	m_bEdited = true;
}

void i3TextAttr::SetScale( REAL32 fScaleX, REAL32 fScaleY)
{
	UINT32 i = 0;

	for( i = 0; i < m_Count; i++ )
		{
			i3TextInfo *pTextInfo = m_TextInfoList[i];
			I3ASSERT( pTextInfo != nullptr );
			
			if( pTextInfo->getVisible())
			{
				if(pTextInfo->getEnable())
				{
					pTextInfo->SetScale( fScaleX, fScaleY);
				}
			}
		}	
}

// Set ASCII Text
/*
bool i3TextAttr::SetText( const char *szText )
{
	UINT32 i = 0;

	if( szText == nullptr )
	{
		szText = "";
	}

#if defined I3_WINDOWS
	bool bUnicode = false;

	// 한글인지 영문인지 확인합니다.
	const UINT32 TextLen = (UINT32)i3::generic_string_size(szText);

	for( i=0; i< TextLen; i++)
	{
		if(szText[i] & 0x80)
		{
			bUnicode = true;
			break;
		}
	}

	if( bUnicode)
	{
		WCHAR16 wszTemp[1024] = {0,};
		i3String::MultiByteToWideChar( CP_ACP, MB_COMPOSITE, szText, i3::generic_string_size( szText), (LPWSTR) wszTemp, 1024);
		return SetText( wszTemp);
	}
#endif
	
	UINT32 nLength = (UINT32)i3::generic_string_size( szText );
	if( nLength > m_MaxCount )
	{
		//I3TRACE( "[%s] Exceed String Buffer Size. (%d > %d)\n", __FUNCTION__, nLength, m_MaxCount );
		I3ASSERT_0;
		return false;
	}

	// 문자열 길이가 기존과 같은 경우 같은 문자열인지 검사
	if( nLength == m_Count )
	{
		i3::mb_to_utf16(szText, m_pTempText);
//		i3String::CopyASCIItoWCHAR( m_pTempText, szText );
		if( i3::generic_is_equal( m_pTempText, m_pText ) )
		{
			return true;
		}
		i3::safe_string_copy( m_pText, m_pTempText, m_MaxCount + 1 );
	}
	// 문자열 길이가 기존과 다를 경우...
	else
	{
		m_Count = nLength;
//		i3String::CopyASCIItoWCHAR( m_pText, szText );
		i3::mb_to_utf16(szText, m_pText);
	}

	if( m_bProgressText)
	{
		for( i = 0; i < m_Count; i++ )
		{
			i3TextInfo *pTextInfo = m_TextInfoList[i];
			pTextInfo->setEnable(false);
		}
	}

	m_bEdited = true;

	return true;
}
*/
// Set Unicode Text
bool i3TextAttr::SetText( const i3::wliteral_range& wTextRng )
{
	UINT32 i = 0;


	UINT32 nLength = i3::generic_string_size( wTextRng );
	if( nLength > m_MaxCount )
	{
		//I3TRACE( "[%s] Exceed String Buffer Size. (%d > %d)\n", __FUNCTION__, nLength, m_MaxCount );
		I3ASSERT_0;
		return false;
	}

	// 문자열 길이가 기존과 같은 경우 같은 문자열인지 검사
	if( nLength == m_Count )
	{
		i3::safe_string_copy( m_wstrTempText, wTextRng, m_MaxCount + 1 );
		if( i3::generic_is_equal( m_wstrTempText, m_wstrText )  )
		{
			return true;
		}
		i3::safe_string_copy( m_wstrText, m_wstrTempText, m_MaxCount + 1 );
	}
	else
	{
		m_Count = nLength;
		i3::safe_string_copy( m_wstrText, wTextRng, m_MaxCount + 1 );
	}

	if( m_bProgressText)
	{
		for( i = 0; i < m_Count; i++ )
		{
			i3TextInfo *pTextInfo = m_TextInfoList[i];
			pTextInfo->setEnable(false);
		}
	}


	m_bEdited = true;

	return true;
}

void i3TextAttr::UpdateTextInfo()
{
	UINT32 i = 0; 
	UINT32 nHeightCount = 1;
	UINT32 nWidthCount = 0;
	REAL32 fWidth = 0.0f;
	REAL32 fHeight = 0.0f;
	REAL32 fTotalLineWidth = 0.0f;
	REAL32 fTotalLineHeight = 0.0f;
	REAL32 x = 0.0f;
	REAL32 y = 0.0f;
	UINT32 nIndex = 0;
	REAL32 TempWidth = 0.0f;
	REAL32 SwapWidth = 0.0f;
	
	m_TotalWidth = 0;
	m_TotalHeight = 0;

	// 행을 넘어갈때의 단어의 길이가 저장됩니다.(단어의 구분은 스페이스로 합니다)
	UINT32 nSwapSpaceCount = 0;

	// Text들을 모두 읽어들여 한라인에 들어가는 문자의 갯수를 배열에 저장합니다.
	for( i = 0; i < m_Count; i++ )
	{
		// 문자를 증가시킵니다.
		nWidthCount++;		

		i3TextInfo *pTextInfo = m_TextInfoList[i];

		//////////////////////////////////////////////////////////////////////////////////////
		// Font Color를 처리합니다.
		// ex) 나는{>255 0 0 255;바보<}다!
		INT32 nJumpCount = 0;
		INT32 nJumpStartIndex = 0;
		char szTemp[4] = {0,};
		COLOR FontColor;
		INT32 ColorCount = 0;
		INT32 ChangeCount = 0;
		if (m_wstrText[i] == L'{')
		{								
			if( m_wstrText[i+1] == L'>')
			{
				i++;
				nWidthCount++;	
				nJumpCount++;
				nJumpStartIndex = i-2;

				m_bChangingFontColor = true;

				while( m_wstrText[i] != ';')
				{
					//////////////////////////////
					// Color
					if(m_wstrText[i] == L' ')
					{
						if(ChangeCount == 0)
						{
							UINT8 r = (UINT8)i3::atoi(szTemp);
							i3Color::SetR(&FontColor, r);
						}
						else if(ChangeCount == 1)
						{
							UINT8 g = (UINT8)i3::atoi(szTemp);
							i3Color::SetG(&FontColor, g);
						}
						else if(ChangeCount == 2)
						{
							UINT8 b = (UINT8)i3::atoi(szTemp);
							i3Color::SetB(&FontColor, b);
						}
						else if(ChangeCount == 3)
						{
							UINT8 a = (UINT8)i3::atoi(szTemp);
							i3Color::SetA(&FontColor, a);
						}

						ChangeCount++;
						ColorCount = 0;
						memset(szTemp, 0, 4);
					}
					else if( m_wstrText[i] >= '0' && m_wstrText[i] <= '9')
					{
						if(m_wstrText[i+1] < '0' || m_wstrText[i+1] > '9')
						{
							if(szTemp[0] == 0) szTemp[0] = '0';
							if(szTemp[1] == 0) szTemp[1] = '0';
							szTemp[2] = (char)m_wstrText[i];
						}
						else if(m_wstrText[i+2] < '0' || m_wstrText[i+2] > '9')
						{
							if(szTemp[0] == 0) szTemp[0] = '0';
							szTemp[1] = (char)m_wstrText[i];
						}
						else if(m_wstrText[i+3] < '0' || m_wstrText[i+3] > '9')
						{
							szTemp[0] = (char)m_wstrText[i];
						}

//						szTemp[ColorCount] = (char)m_pText[i];
//						ColorCount++;
					}
					//////////////////////////////

					i++;
					nWidthCount++;
					nJumpCount++;
				}

				if(ChangeCount == 3)
				{
					UINT8 a = (UINT8)i3::atoi(szTemp);
					i3Color::SetA(&FontColor, a);
				}

				i3Color::Copy(&m_BackUpColor, &m_Color);
				i3Color::Copy(&m_Color, &FontColor);

				if( nJumpStartIndex < 0) nJumpStartIndex = 0;

				(m_TextInfoList[nJumpStartIndex])->setJumpIndex(nJumpCount);
				continue;
			}

		}		

		if(m_bChangingFontColor)
		{
			if (m_wstrText[i] == '<')
			{			
				if( m_wstrText[i+1] == '}')
				{
					i++;
					nWidthCount++;
					i3Color::Copy(&m_Color, &m_BackUpColor);
					m_bChangingFontColor = false;
					continue;
				}
			}
		}

		pTextInfo->SetColor( &m_Color );
		//
		//////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////



		if(m_wstrText[i] != 10 && m_wstrText[i] != 13)
		{
			pTextInfo->setVisible(true);
			pTextInfo->SetChar( m_wstrText[i] );				
		}
		else
		{
			pTextInfo->setVisible(false);
		}

		TempWidth += pTextInfo->GetWidth();
		fWidth = (REAL32)pTextInfo->GetWidth();

		// 단어의 길이를 증가합니다.(한문장.. 한문장..)
		nSwapSpaceCount++;
		SwapWidth += fWidth;

		// 새로운 스페이스가 나오게 되면, 단어의 길이를 초기화합니다.
		if(m_wstrText[i] == 32)
		{
			nSwapSpaceCount = 0;
			SwapWidth = 0.0f;
		}

		// 개행문자가 들어갔을때의 처리
		if(m_wstrText[i] == 10 || m_wstrText[i] == 13)
		{
			if(nWidthCount == 1)
			{
				nWidthCount = 0;
				TempWidth = 0.0f;
				continue;
			}

			m_pTextLineArray[nIndex] = nWidthCount-1;
			nIndex++;
			nHeightCount++;	
			nWidthCount = 0;
			TempWidth = 0.0f;

			// 만약, 행을 넘길때, 단어가 끝나지 않았다면, 다음행으로 단어를 추가합니다.

			if(nIndex == m_nMaxLineArray) break;
			continue;
		}
		// Text Box Size보다 큰경우일때의 처리
		else if( m_bAutoWrap && ( m_nWidth > 0) )
		{
            if( m_nWidth <= TempWidth)
			{
				m_pTextLineArray[nIndex] = nWidthCount;
				nIndex++;
				nHeightCount++;			
				nWidthCount = 0;	
				TempWidth = 0.0f;

				if( m_bWordLineFeed)
				{
					// 만약, 행을 넘길때, 단어가 끝나지 않았다면, 다음행으로 단어를 추가합니다.
					m_pTextLineArray[nIndex-1] -= nSwapSpaceCount;
					nWidthCount = nSwapSpaceCount;
					TempWidth = SwapWidth;
					nSwapSpaceCount = 0;
					SwapWidth = 0.0f;
				}
				if(nIndex == m_nMaxLineArray) break;				
			}
		}
		// 자동 줄바꿈일때, 한라인에 들어갈 최대Text가 들어갔을때의 처리
		else if( m_bAutoWrap && m_MaxLineWidth <= nWidthCount )
		{
			m_pTextLineArray[nIndex] = nWidthCount;
			nIndex++;
			nHeightCount++;			
			nWidthCount = 0;	
			TempWidth = 0.0f;

			// 만약, 행을 넘길때, 단어가 끝나지 않았다면, 다음행으로 단어를 추가합니다.

			if(nIndex == m_nMaxLineArray) break;
		}			
	}  

	// 위의 For문 연산되에 남은 문자들을 마지막 배열에 담습니다.
	if(nIndex < m_nMaxLineArray)
	{
		m_pTextLineArray[nIndex] = nWidthCount;
		nIndex++;
	}

	// Align형식에 맞추어서 Text의 초기위치를 변경합니다.(Y축)	
	i3TextInfo *pTextInfo = m_TextInfoList[0];
	
	fHeight = (REAL32)pTextInfo->GetHeight();
	if(fHeight < 0.0f) fHeight = 13.0f;
	fHeight += REAL32( m_HeightGap );

	fTotalLineHeight = i3Math::Mul( fHeight, (REAL32)nHeightCount);
	m_TotalHeight = (UINT32) fTotalLineHeight;

	switch(m_AlignY)
	{
	case TEXT_ATTR_ALIGN_TOP:		y = i3Vector::GetY( &m_Position );							break;
	case TEXT_ATTR_ALIGN_MIDDLE:
		REAL32 fHalfHeight;

		#if defined( I3_FIXED_POINT)
			fHalfHeight = fTotalLineHeight >> 1; 
		#else
			fHalfHeight = fTotalLineHeight * 0.5f;
		#endif
			
		y = i3Vector::GetY( &m_Position ) + (m_nHeight >> 1) - fHalfHeight;
		break;
	case TEXT_ATTR_ALIGN_BOTTOM:	y = i3Vector::GetY( &m_Position ) + m_nHeight - fTotalLineHeight;		break;
	}


	// 위에 계산된 모든 정보를 이용하여 Text를 배치합니다.
	UINT32 test = 0;
	for( i = 0; i < nIndex; i++ )
	{
		// 한줄에 쓰이는 총 Text길이를 구합니다. 이때, 개행문자(10)의 경우는 길이에 포함시키지 않습니다.
		UINT32 tempOptset = 0;
		for(UINT32 k=test; k<m_pTextLineArray[i]+test+tempOptset; k++)
		{	
			if(m_wstrText[k] == 10 || m_wstrText[k] == 13)
			{		
				tempOptset++;
				continue;
			}

			i3TextInfo *pTextInfo = m_TextInfoList[k];
			fTotalLineWidth += pTextInfo->GetWidth();
		}			

		// Align형식에 맞추어서 각 라인마다 Text의 초기위치를 변경합니다.(X축)
		switch(m_AlignX)
		{
		case TEXT_ATTR_ALIGN_LEFT:		x = i3Vector::GetX( &m_Position );							break;
		case TEXT_ATTR_ALIGN_CENTER:
			REAL32 fTemp;

			#if defined( I3_FIXED_POINT)
				fTemp = fTotalLineWidth >> 1;
			#else
				fTemp = fTotalLineWidth * 0.5f;
			#endif

			x = i3Vector::GetX( &m_Position ) + (m_nWidth >> 1) - fTemp;	
			break;

		case TEXT_ATTR_ALIGN_RIGHT:		x = i3Vector::GetX( &m_Position ) + m_nWidth - fTotalLineWidth;		break;
		}

		// 배열에 담긴 문자의 숫자대로 라인마다 Text의 위치와 컬러를 수정합니다.
		for(UINT32 n=0; n<m_pTextLineArray[i]; n++)
		{
			i3TextInfo *pTextInfo = m_TextInfoList[test];

			if(m_wstrText[test] == 10 || m_wstrText[test] == 13)
			{
				COLOR TempColor;
				i3Color::Set(&TempColor, (UINT8) 0, 0, 0, 0);
				pTextInfo->SetColor( &TempColor );

				test++;
				n--;
				continue;
			}
			
			//	pTextInfo->SetColor( &m_Color );
			pTextInfo->SetPosition( (INT32) x, (INT32) y);
		
			// 또한, 공백문자일때, 그 문자가 그 줄의 가장 선두라면, x좌표를 증가 시키지 않습니다.
			if(n==0)
			{
				if(m_wstrText[test] != 32)	x += pTextInfo->GetWidth();
			}
			else
			{
				x += pTextInfo->GetWidth();
			}
			
			test++;

		}

		m_TotalWidth = MAX( m_TotalWidth, (UINT32) fTotalLineWidth);

		// 한 라인이 끝났으므로 Y축으로 한라인 증가 시킵니다.
		y += fHeight + 1.0f;
		fTotalLineWidth = 0.0f;
	}
}

void i3TextAttr::UpdateVertexArray( i3RenderContext * pContext)
{
	UINT32 t, i, vIdx = 0;

	I3ASSERT( m_BufferIndex == 0 || m_BufferIndex == 1 );
	i3VertexArray *pVA = m_pVA[m_BufferIndex];

#if ! I3_GFXCAPS_TRANSFORMED_VERTEX
	REAL32 rcx, rcy;

	REAL32 scrW = (REAL32) pContext->GetCurrentRTWidth();
	REAL32 scrH = (REAL32) pContext->GetCurrentRTHeight();

	#if defined( I3_FIXED_POINT)
		rcx = pTarget->GetWidth() >> 1;
		rcy = pTarget->GetHeight() >> 1;
	#else
		rcx = 1.0f / (scrW * 0.5f);
		rcy = 1.0f / (scrH * 0.5f);
	#endif
#endif

	pVA->Lock( TEXT_LOCK);

	I3ASSERT( m_Count <= (UINT32) m_TextInfoList.size() );
	for( t = 0; t < (UINT32) m_nNumTexture; t++ )
	{
		m_pCharCount[t] = 0;
		for( i = 0; i < m_Count; i++ )
		{
			i3TextInfo *pTextInfo = nullptr;
			pTextInfo = m_TextInfoList[i];
			I3ASSERT( pTextInfo != nullptr );
			
			if( pTextInfo->getVisible())
			{
				if(pTextInfo->getEnable())
				{
					if( pTextInfo->GetTextureIndex() == t )
					{
						#if I3_GFXCAPS_TRANSFORMED_VERTEX
							pTextInfo->GetTextVertexArray( pVA, vIdx );
						#else
							pTextInfo->GetTextVertexArray( pVA, vIdx, rcx, rcy);
						#endif

						vIdx += 6;
						m_pCharCount[t]++;
					}
				}
			}
		}
	}

	pVA->Unlock();
}

void i3TextAttr::Apply( i3RenderContext * pContext )
{
	if( m_Count <= 0 || !m_bEnable)
		return;

	REAL32 rDeltaSeconds = pContext->GetDeltaSec();
	

	if(m_bProgressText)
	{
		ProcessProgressText( rDeltaSeconds);
	}

	I3ASSERT( m_pVA[0] != nullptr);
	I3ASSERT( m_pVA[1] != nullptr);

	if( m_bAlwaysUpdate || m_bEdited )
	{
		UpdateTextInfo();
		m_BufferIndex ^= 1;
		UpdateVertexArray( pContext);
		m_bEdited = false;
	}

	UINT32 t, nStartIndex = 0;
	for( t = 0; t < m_nNumTexture; t++ )
	{
		if( m_pCharCount[t] > 0)
		{
			i3Texture *pTex = m_pFont->GetTexture( t );
			
			pContext->SetTexture( I3G_TEXTURE_BIND_DIFFUSE, pTex );

			pContext->SetVertexArray( m_pVA[m_BufferIndex] );
			
			m_pDrawShader = pContext->DrawPrim( m_pDrawShader, I3G_PRIM_TRILIST, nStartIndex, m_pCharCount[t] << 1 );
			nStartIndex += m_pCharCount[t] * 6;
		}
	}
}

void i3TextAttr::ProcessProgressText(REAL32 rDeltaSeconds)
{			
	if(m_bProgressTextStart)
	{			
		m_fProgressDeltaTime += rDeltaSeconds;

		if(m_fProgressDeltaTime > m_fProgressTextScrollSpeed)
		{
			if(m_nProgressTextCurrentIndex <= getTextCount())
			{				
				getTextInfoByIndex( m_nProgressTextCurrentIndex)->setEnable(true);
				INT32 JumpIndex = getTextInfoByIndex( m_nProgressTextCurrentIndex)->getJumpIndex();
				m_nProgressTextCurrentIndex = m_nProgressTextCurrentIndex + 1 + JumpIndex;
				m_fProgressDeltaTime = 0.0f;
			}
			else
			{
				m_bProgressText = false;
			}
		}
	}
}

void i3TextAttr::StartProgressText( bool bStart)
{
	m_bProgressTextStart = bStart;
}

void i3TextAttr::ResetProgressText()
{
	m_bProgressTextStart = false;
	m_fProgressDeltaTime = 0.0f;
	m_fProgressTextScrollSpeed = 0.0f;
	m_nProgressTextCurrentIndex = 0;
}

void i3TextAttr::SetImmediatelyProgressText()
{
	m_bProgressTextStart = false;

	INT32 i = 0;
	for(i=0; i<getTextCount(); i++)
	{
		getTextInfoByIndex(i)->setEnable(true);
		INT32 JumpIndex = getTextInfoByIndex(i)->getJumpIndex();
		i = i + JumpIndex;
	}
}


void i3TextAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED FONT_ATTR_HEADER
	{
		UINT32			m_Count = 0;
	};

	struct PACKED FONT_ATTR_INFO
	{
		UINT32				m_Color = 0;
		UINT8				m_bEnable = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextAttr::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3TextAttr::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
