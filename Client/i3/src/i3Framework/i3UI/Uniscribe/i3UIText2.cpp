#include "i3FrameworkPCH.h"
#include "Uniscribe/i3UIText2.h"
#include "Uniscribe/i3UIUniscribe.h"
#include "../i3Framework.h"
#include "i3Base/itl/range/algorithm/replace.h"
#include "i3Base/ParseColorString.h"


//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#define DEFAULT_TEXT_RECT_SIZE	2048

I3_CLASS_INSTANCE( i3UIText);

i3UIText::i3UIText() : m_wstrRange(m_string_storage.begin(), m_string_storage.end())
{
	m_rtText.set( 0, 0, DEFAULT_TEXT_RECT_SIZE, DEFAULT_TEXT_RECT_SIZE);
	
	COLOR col;

	i3Color::Set( &col, (UINT8)32, 32, 32, 200);
	m_ShadowColor = i3Color::ConvertRGBA32( &col);
	
	for (int i = 0; i < MAXCOMPBOX; i++)
		m_hSpriteCompBox[i] = -1;
	
	m_RunList.reserve( 64);
	m_rtScissors.set(0, 0, 0, 0);
}

i3UIText::~i3UIText()
{
	EndEdit();

	ResetText();

	I3_SAFE_RELEASE( m_pFont);

	//g_pUniscribe->ReleaseRuns( &m_RunList);

	_ClearLineInfoList();

	clearScriptSecntence();
}


LINE_INFO* i3UIText::_FindLineInfo( const wchar_t* pszStr, INT32 nLen)
{

	for( size_t i=0; i< m_LineInfoList.size(); i++)
	{
		LINE_INFO* pInfo = m_LineInfoList[i];

		INT32 nLongerLen = max( nLen, (INT32)pInfo->m_strLine.length());

		if(	wcsncmp( pInfo->m_strLine.c_str(), pszStr, nLongerLen) == 0 && pInfo->m_pFont == m_pFont)
		{
			return pInfo;
		}
	}


	return nullptr;
}

void i3UIText::_MoveLineInfo( LINE_INFO* pInfo, i3::vector<LINE_INFO*>& To)
{
	i3::vu::remove(m_LineInfoList, pInfo);
//	m_pLineInfoList->Remove( pInfo);
	To.push_back( pInfo);
}

void i3UIText::_FreeLineInfo( LINE_INFO* pInfo)
{

	i3::vector<SCRIPT_RUN*>& RunList = pInfo->m_RunList;

	for( size_t i=0; i< RunList.size() ; i++)
	{
		g_pUniscribe->ReleaseRun( RunList[i] );
	}
	RunList.clear();
	
	i3::vector<i3UIGlyph*>& GlyphList = pInfo->m_GlyphList;
	GlyphList.clear();

	I3_MUST_DELETE( pInfo);

}

void i3UIText::_ClearLineInfoList( void)
{
	for( size_t i=0; i< m_LineInfoList.size(); i++)
	{
		LINE_INFO* pInfo = m_LineInfoList[i];
		_FreeLineInfo( pInfo);
	}
	m_LineInfoList.clear();

	m_RunList.clear();
	m_GlyphList.clear();
}

//Line별로 Caching
//Performance가 문제가 되는 경우 new_object 대신 Pool을 이용하는 방식을 통해 개선. 
//String Compare가 여러번 일어나긴 하지만, Uniscribe를 호출하는 것 보다는 빠르다. 
void i3UIText::SplitStrings( const wchar_t* str)
{
//Clear
// 현재 str이 널값이나 empty가 되는 일이 불가능함....   (문자열이 비어있으면 SplitStrings 함수가 호출되지 않음)

	//Analyze Text
	INT32 start = 0;
	INT32 end = 0;

	i3::vector<LINE_INFO*> NewLineInfoList;

	int _tempCount1 = 0;
	int _tempCount2 = 0;

	while( true)
	{
		//for every line
		if( str[end] == 10 || str[end] == 0)
		{
			INT32 len = str[end] == 0 ? end-start : end-start+1;

			if( len == 0)
				break;

			LINE_INFO* pInfo = _FindLineInfo( &str[start], len);

			if( pInfo != nullptr)
			{
				_MoveLineInfo( pInfo, NewLineInfoList);
				_tempCount1++;
			}
			else
			{
				//Create line Info for the later use
				LINE_INFO* pNewInfo = new LINE_INFO;	//(LINE_INFO*)i3MemAlloc( sizeof( LINE_INFO));			// new지원되는 스토리지풀로 교체..
				
				NetworkDump_Malloc( pNewInfo, sizeof( LINE_INFO), __FILE__, __LINE__);
				
				//pNewInfo->m_pszLine = (STR16)i3MemAlloc( sizeof( CHAR16) * (len + 1));	//length + '0'
				pNewInfo->m_strLine.assign(str + start, len);

				pNewInfo->m_pFont = m_pFont;
				pNewInfo->m_numLineFeed = 0;

				i3UIFont* pFont = nullptr;
				//Uniscribe
				
				g_pUniscribe->CreateUIText(this, &str[start], len, pNewInfo->m_RunList, pNewInfo->m_GlyphList);
				
				if (m_ReCreateUniscribe)
				{
 					m_ReCreateUniscribe = false;
					if ((*i3UI::getCountryList()).size() > 0)
					{
						g_pUniscribe->ReleaseRuns(pNewInfo->m_RunList);

						int nTotalIndex = 0;
						for (int i = 0; i < (int)m_vecScriptSentence.size(); i++)
						{
							nTotalIndex += m_vecScriptSentence[i]->_end;
							pFont = g_pUniscribe->FontFallback(m_vecScriptSentence[i], m_pFont);
							g_pUniscribe->CreateUIText(this, &str[start + m_vecScriptSentence[i]->_begin], m_vecScriptSentence[i]->_end,
								pNewInfo->m_RunList, pNewInfo->m_GlyphList, pFont, nTotalIndex);
						}
					}
				}
			
				NewLineInfoList.push_back( pNewInfo);
				_tempCount2++;
			}
			
			if( str[end] == 0)
				break;

			start = end+1;
		}
		
		end++;
	}

	//I3TRACE( L"tempCount: %d, %d\n", _tempCount1, _tempCount2);

	//Clear
	_ClearLineInfoList();		// m_RunList와 m_GlyphList도 이 힘수에서 클리어 된다..
		
	//New
	i3::swap( m_LineInfoList , NewLineInfoList);		// swap은 포인터교환만으로 이루어진다..
//	m_pLineInfoList = pNewLineInfoList;

	for( size_t i=0; i< m_LineInfoList.size(); i++)
	{
		LINE_INFO* pInfo = m_LineInfoList[i];
		
		i3::vector<SCRIPT_RUN*>& RunList = pInfo->m_RunList;
		for( size_t j=0; j< RunList.size(); j++)
		{
			SCRIPT_RUN* pRun = RunList[j];
			
			pRun->_lineIdx = i;
			
			m_RunList.push_back( pRun);
		}

		i3::vector<i3UIGlyph*>& GlyphList = pInfo->m_GlyphList;

		for( size_t j=0; j< GlyphList.size(); j++)
		{
			m_GlyphList.push_back( GlyphList[j]);
		}
	}
}

void i3UIText::CreateText( void)
{
	i3_prof_func();

	if (m_wstrRange.empty())
	{
		_ClearLineInfoList();
	}

//	if( m_pszText != nullptr)
//	if (!m_wstrRange.empty())
	{	
		if( m_bEscapeSequence)
		{
			m_bContainEscSeq = ParseColorStringIfNeeded(m_wstrRange, m_DefColor, m_strRefinedText, 
				m_RefinedColorTable, m_RefinedColorIdxList, m_RefinedtoSrcMapper);
		}
		else
		{
			m_bContainEscSeq = false;
		}
		
		if( m_bContainEscSeq)
		{
			m_UserColorTableBeginIndex = DWORD(m_RefinedColorTable.size());
			SplitStrings( m_strRefinedText.c_str());		// 내부구현에서 길이값을 요구하지 않는 함수이다..
		}
		else
		{
			if (m_strRefinedText.empty() == false)
			{
				m_strRefinedText.clear();
				m_RefinedColorTable.clear();
				m_RefinedColorIdxList.clear();
				m_RefinedtoSrcMapper.clear();
			}
			m_UserColorTableBeginIndex = 0;
			SplitStrings( m_wstrRange.begin());				// 내부구현에서 길이값을 요구하지 않는 함수이다..
		}
		
		LayOutText();
	}
//	else
//	{
//		_ClearLineInfoList();			// 
//	}
}

void i3UIText::LayOutText( void)
{

	for( size_t i=0; i< m_LineInfoList.size(); i++)
	{
		LINE_INFO* pInfo = m_LineInfoList[i];
		pInfo->m_numLineFeed = 1;					// LayOutText호출마다 1로 리셋해주고, LayOutText에서 라인피드 재계산시 증가시켜준다.
	}

	//Text Rect
	m_rtText.set( 0, 0, m_rtView.getWidth()-1, m_rtView.getHeight()-1);
	
	INT32 width = m_rtText.getWidth()-1;
	INT32 height = m_rtText.getHeight()-1;

	//Letter Context
	I3UI_TEXT_PARSE_CTX ctx;
	ctx.m_pTextTail = nullptr;
	ctx.m_pFreePool = m_pFirstLetter;
	ctx.m_X = 0;
	ctx.m_Y = 0;
	ctx.m_CurTextColor = 0xFFFFFFFF;

	m_ExtentX = 0;

	//Variables
	INT16 nLine = 0;
	INT32 nTotalCharacterCount = 0;
	INT32 nTotalGlyphCount = 0;
	bool bRTL = false;
	i3UILetter* pLetter = nullptr;
	i3UILetter * pWordLetter = nullptr;

	INT32 nAdvanceY = m_nDefFontHeight + m_nSpaceofLettersY;

	for( size_t i=0; i< m_RunList.size(); i++)
	{
		SCRIPT_RUN* pRun = m_RunList[i];
		
		//pRun->_nClusters = 0;

		ctx.m_CurTextColor = getDefColor();		//Run자체의 Color
		
		bRTL = pRun->_psa->fRTL;

		//그동안 배치된 Charcter의 개수를 구해둔다. 
		INT32 nAccCharacterCount = _GetAccCharacterCount( pRun->_lineIdx, pRun->_logIdx);
		
		//RTL이면서 Run이 한줄을 넘어가는 경우에는 Character의 배치 순서가 복잡해진다. 
		bool bUseGlyphIndexTable = false;
		INT32 glyphindex[256] = {0,};		//big enough arbitrary number
		bool breakIndex[256] = {0,};
		
		if( bRTL && isAutoLineFeed() && !isScrollingHorizon())
		{
			INT32 nExpectedLetterCount = 0;
			for( INT32 j=0; j<pRun->_nChars; j++)
			{
				if( pRun->_pLogAttrs[j].fCharStop)
					nExpectedLetterCount++;
			}

			INT32 nExpectedRunAdvance = ctx.m_X + pRun->_width + m_nSpaceofLettersX * nExpectedLetterCount;
			
			if( nExpectedRunAdvance >= width)
			{
				_GenGlyphIndexTable( pRun, m_GlyphList, nTotalGlyphCount, ctx.m_X, width, glyphindex, breakIndex);
			
				bUseGlyphIndexTable = true;
			}
		}


		pWordLetter = nullptr;

		//각 Run 처리
		for( INT32 k=0; k< pRun->_nChars; k++)
		{
			INT32 j = k;	//LTR
			
			if( bRTL)		//RTL인 경우처리
			{
				if( bUseGlyphIndexTable)
				{
					j = glyphindex[k];
					
					if( breakIndex[k] == true)
					{
						ctx.m_X = m_rtText.left;
						ctx.m_Y += nAdvanceY;
						nLine++;
					}
				}
				else
					j = pRun->_nChars-(k+1);	//backwards
			}
		

			if( pRun->_pLogAttrs[j].fCharStop == false)
				continue;

			if( pRun->_pChars[j] == 10)		//new line
			{	
				pLetter = AddNullLetter( &ctx);
				pLetter->setIndex( nAccCharacterCount + j);
				pLetter->setCode( pRun->_pChars[j]);
				
				ctx.m_X = m_rtText.left;
				ctx.m_Y += nAdvanceY;
				nLine++;
				
				continue;
			}

			ctx.m_CurTextColor = getFontColor( nAccCharacterCount + j);		//EscapeSeq에 의해 변경된 컬러
						

			//Glyph와 Character정보를 조합해 Cluster를 만든다.
			//각 Letter는 Cluster와 1:1로 대응하며 Caret이동의 단위가 된다. 
			SPRITE_INFO info[MAX_GLYPH_PER_LETTER];
			i3mem::FillZero( info , sizeof( SPRITE_INFO) * MAX_GLYPH_PER_LETTER);
			
			ABC abc;				//cluster's abc
			INT32 nClusterWidth;

			INT32 nGlyphCount = g_pUniscribe->GenClusterInfo( pRun, m_GlyphList, j, nTotalGlyphCount, info, &abc, &nClusterWidth);

			//line break
			if( isAutoLineFeed() && !isScrollingHorizon())
			{
				if( ctx.m_X + abc.abcA + abc.abcB >= (UINT)width)
				{
					if( (ctx.m_X != m_rtText.left) )
					{
						ctx.m_X = m_rtText.left;
						ctx.m_Y += nAdvanceY + m_nLinePitch;

						// LINE_INFO에 개행문자 아닌 가로 사이즈에 의한 라인피드(줄내림) 횟수를 기록해준다..(2015.01.05.수빈)
						I3ASSERT(pRun->_lineIdx != -1);

						LINE_INFO* li = m_LineInfoList[pRun->_lineIdx];
						++li->m_numLineFeed;
						//
						nLine++;
					}

					if( isTextStyle( I3UI_TEXT_STYLE_LINFEEDWITHWORD) )
					{
						i3UILetter * pNext = pWordLetter;

						while( pNext != nullptr)
						{
							pNext->setPos( (INT16) ctx.m_X, (INT16) ctx.m_Y);
							ctx.m_X += pNext->getWidth();

							pNext = pNext->getNext();
						}

						pWordLetter = nullptr;
					}
				}
			}

			if( abc.abcA < 0)	//먼저 더한다. 
				ctx.m_X += abc.abcA;	//underhang( or padding)

			pLetter = AddLetter( &ctx, info, nGlyphCount);

			//pRun->_nClusters++;

			pLetter->setRightCaret( bRTL);
			pLetter->setLine( nLine);
			pLetter->setCode( pRun->_pChars[j]);

			if( pWordLetter == nullptr)
				pWordLetter = pLetter;

			if( pRun->_pChars[j] == L' ')
			{
				pWordLetter = nullptr;
			}

			INT32 width = abc.abcB;
			if( abc.abcA > 0) width += abc.abcA;
			if( abc.abcC > 0) width += abc.abcC;

			// 렌더링 크기에 맞는 abc를 구해야한다.

			pLetter->setWidth( (INT16)width);
			pLetter->setHeight( (INT16)m_nDefFontHeight);

			if( abc.abcA > 0)	//나중에 더한다.
				ctx.m_X += abc.abcA;

			m_ExtentX = (INT32)( MAX( m_ExtentX, ctx.m_X + (INT32) abc.abcB - m_rtText.left));
		
			ctx.m_X += abc.abcB + abc.abcC + m_nSpaceofLettersX;
			
			pLetter->setIndex( nAccCharacterCount + j);
		}

		nTotalCharacterCount += pRun->_nChars;
		nTotalGlyphCount += pRun->_nGlyphs;

		m_ExtentY = ctx.m_Y + nAdvanceY;
	}

	//Null Letter
	i3UILetter* pNullLetter = AddNullLetter( &ctx);
	pNullLetter->setIndex( nTotalCharacterCount);

	if( nTotalGlyphCount > 0)
		setMaxLine( nLine+1);
	else
		setMaxLine( 0);

	//Free Letter Pool
	if( ctx.m_pFreePool != nullptr)
	{
		FreeLetterList( ctx.m_pFreePool);

		if( m_pFirstLetter == ctx.m_pFreePool)	//No letter to display
			m_pFirstLetter = nullptr;
	}

	//Align
	_AlignLettersX( width);
	_AlignLettersY( height);

	//Render
	RenderText();

	//DWORD dwLayOutEnd = GetTickCount();
	//I3TRACE( "i3UIText::LayOutText: %d\n", dwLayOutEnd - dwLayOutStart);
}

void i3UIText::setVisible( bool bVisible)
{
	if( g_pUIRenderer == nullptr)
		return;

	i3UILetter* pNode = m_pFirstLetter;

	while( pNode != nullptr)
	{
		for( INT32 i=0; i< pNode->getSpriteCount(); i++)
		{
			INT32 hSprite = pNode->getSprite( i);
			if( hSprite != -1)
			{
				g_pUIRenderer->setEnableSprite( hSprite, bVisible);
			}
		}

		pNode = pNode->getNext();
	}
}

INT32 i3UIText::AdjustSpriteOrder( INT32 hSpriteEnd)
{
	/*
	if( g_pUIRenderer == nullptr)
		return -1;

	i3UILetter * pLetter = m_pFirstLetter;
	INT32 idxEnd = hSpriteEnd, idxSprite;

	while( pLetter != nullptr)
	{
		for( INT32 i=0; i< pLetter->getSpriteCount(); i++)
		{
			idxSprite = pLetter->getSprite( i);

			if( idxSprite != -1)// && idxEnd != -1)
			{
				g_pUIRenderer->AdjustOrder( idxEnd, & idxSprite, 1);

				idxEnd = idxSprite;
			}
		}

		pLetter = pLetter->getNext();
	}

	return idxEnd;
	*/

	return 0;
}

void i3UIText::ResetText(void)
{
	if((g_pTextManager != nullptr) && (m_pFirstLetter != nullptr))
	{
		FreeLetterList( m_pFirstLetter);
		m_pFirstLetter = nullptr;
	}

	m_ShowX = 0;
	m_ShowY = 0;
	m_ExtentX = 0;
	m_ExtentY = 0;
}
//
/*
void i3UIText::SetText(const i3::rc_wstring& rcText )
{
	i3_prof_func();
		
	m_strText = rcText;				// 레퍼런스 카운팅 스트링을 쓴다면 복사든 아니든 상관없게 된다..
									// const wchar_t* 인수도 허용되는데, 이경우엔 복사가 되고, i3::rc_wstring이 인수가 되면 포인터만 카운팅복사된다..
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...
}

void	i3UIText::SetText(const wchar_t* szText)
{
	i3_prof_func();
		
	m_strText = szText;				// 레퍼런스 카운팅 스트링을 쓴다면 복사든 아니든 상관없게 된다..
									// const wchar_t* 인수도 허용되는데, 이경우엔 복사가 되고, i3::rc_wstring이 인수가 되면 포인터만 카운팅복사된다..
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...
}
*/

void	i3UIText::SetText( const i3::rc_wstring& rcText)
{
	if( i3::generic_is_equal(m_wstrRange, rcText) )
		return;

	m_rc_string_storage = rcText;
	m_wstrRange = i3::as_literal(m_rc_string_storage);
	m_string_storage.clear();
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...
}

void	i3UIText::SetText( const i3::wstring& wstrText)
{
	if( i3::generic_is_equal(m_wstrRange, wstrText) )
		return;

	m_string_storage = wstrText;
	m_wstrRange = i3::as_literal(m_string_storage);
	m_rc_string_storage.clear();
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...	
}

void	i3UIText::SetText( const i3::wliteral_range& wTextRng)
{
	if( i3::generic_is_equal(m_wstrRange, wTextRng) )
		return;

	m_string_storage.assign(wTextRng.c_str(), wTextRng.size());
	m_wstrRange = i3::as_literal(m_string_storage);
	m_rc_string_storage.clear();
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...
}

void	i3UIText::SetText( const wchar_t* wszText)
{
	i3UIText::SetText(i3::wliteral_range(wszText));
}


void	i3UIText::SwapText( i3::wstring& tmpText)
{
	if( i3::generic_is_equal(m_wstrRange, tmpText) )
		return;

	i3::swap(m_string_storage, tmpText);
	m_wstrRange = i3::as_literal(m_string_storage);
	m_rc_string_storage.clear();
	CreateText();					// SetText()마다 호출된다.. 따라서 이 함수는 구현이 너무 무거우면 안된다...
}

void i3UIText::SetFont( i3UIFont * pFont)
{
	if( m_pFont != pFont)
	{
		I3_REF_CHANGE( m_pFont, pFont);

	//	CreateText();				// 칼라키워드 파싱은 전혀 필요없기 때문에 그쪽 루틴 전체를 제거한 구현이 필요하므로 주석걸고 별도 구현한다..

		if (!m_wstrRange.empty())
		{	
			if( m_bContainEscSeq)
				SplitStrings( m_strRefinedText.c_str());		// 내부구현에서 길이값을 요구하지 않는 함수이다..
			else
				SplitStrings( m_wstrRange.begin());				// 내부구현에서 길이값을 요구하지 않는 함수이다..

			LayOutText();
		}
		
	}
}

void i3UIText::SetAlign( I3UI_TEXT_ALIGN_X alignX, I3UI_TEXT_ALIGN_Y alignY)
{
	if( m_AlignX != alignX || m_AlignY != alignY)
	{

		m_AlignX = alignX;
		m_AlignY = alignY;

		//if( m_AlignX != I3UI_TEXT_ALIGN_X_LEFT)
		//	setAutoLineFeed( true);

		LayOutText();
	}
}

void i3UIText::SetSpaceofLetters( INT32 nX, INT32 nY)
{
	if( nX != m_nSpaceofLettersX || m_nSpaceofLettersY != nY)
	{
		m_nSpaceofLettersX = nX;
		m_nSpaceofLettersY = nY;

		LayOutText();
	}
}

void i3UIText::SetDefColor( UINT32 color)
{
	if( m_DefColor != color)
	{
		//Escape Sequence가 있는 경우 Color 정보를 업데이트
		if (m_RefinedColorTable.empty() == false)
		{
			// m_RefinedClrList에서 Default와 같은것만 골라서 바꿔주면 끝난다...
			// i3::replace 알고리즘을 쓰면 내부루프는 돌지만 파싱은 필요없고 눈에 보기에도 편해보인다..
			m_RefinedColorTable[0] = color;
		}
		m_DefColor = color;	
		LayOutText();
	}
}

void i3UIText::setTextStyle( UINT32 style)
{
	if( isTextStyle(style) == false)
	{
		m_Style = style; 

		LayOutText();
	}
}

void i3UIText::addTextStyle( UINT32 style)
{
	if( isTextStyle( style) == false)
	{
		m_Style |= style; 

		LayOutText();
	}
}

void i3UIText::removeTextStyle( UINT32 style)
{ 
	if( isTextStyle( style))
	{
		m_Style &= ~style; 

		LayOutText();
	}
}

void i3UIText::SetViewRect( INT32 x, INT32 y, INT32 w, INT32 h)
{
	if( m_rtView.left != x || m_rtView.top != y || 
		m_rtView.right != x+w || m_rtView.bottom != y+h)
	{
		m_rtView.set( x, y, x+w-1, y+h-1);

		LayOutText();
	}
}

void i3UIText::SetScissorsRect(const i3Rect& rt)
{
	m_rtScissors = rt;
}

void i3UIText::SetShowPos( INT32 l, INT32 t)
{
	m_ShowX = l;
	m_ShowY = t;

	RenderText();
	//LayOutText();
}

void i3UIText::SetShadowType( I3UI_TEXT_SHADOW shadow)
{
	if( g_pUIRenderer == nullptr)
		return;

	m_ShadowType = shadow;

	i3UILetter * pTemp = m_pFirstLetter;

	while( pTemp != nullptr)
	{
		for( INT32 i=0; i< pTemp->getSpriteCount(); i++)
		{
			INT32 hSprite = pTemp->getSprite( i);

			if( hSprite != -1)
			{
				VEC4D * pV;

				pV = g_pUIRenderer->getTexRange( hSprite);
				g_pUIRenderer->setTexRange( hSprite, pV->x, pV->y, (REAL32) shadow, pV->w);
			}
		}

		pTemp = pTemp->getNext();
	}
}

void i3UIText::_FreeSprite( void)
{
	if (m_bEnable == false)
	{
		if (m_pFirstLetter)
		{
			I3ASSERT(m_pFirstLetter->getSprite(0) == -1);
		}
		return;
	}

	i3UILetter* pLetter = m_pFirstLetter;

	while( pLetter != nullptr)
	{
		for( INT32 i=0; i< MAX_GLYPH_PER_LETTER; i++)
		{
			_FreeSprite( pLetter, i);
		}

		pLetter = pLetter->getNext();
	}
}

void	i3UIText::SetEnable(bool bEnable)
{
	if (bEnable == m_bEnable) return;

	if (bEnable == false)
		i3UIText::_FreeSprite();
	m_bEnable = bEnable;
}


void i3UIText::RenderText( void)
{
	i3UILetter* pLetter = m_pFirstLetter;

	//m_ShowX, m_ShowY 는 여기서만 고려하면 된다. 

	while( pLetter != nullptr)
	{
		//for( INT32 i=0; i< pLetter->getGlyphCount(); i++)
		for( INT32 i=0; i< MAX_GLYPH_PER_LETTER; i++)
		{
			if( i >= pLetter->getGlyphCount())
			{
				_FreeSprite( pLetter, i);
				continue;
			}

			i3UIGlyph* pGlyph = pLetter->getGlyph( i);

			if( pGlyph == nullptr)
			{
				_FreeSprite( pLetter, i);
				continue;
			}

			//Clipping
			i3Rect rtBox;

			rtBox.set(0, 0, m_rtView.getWidth() - 1, m_rtView.getHeight() - 1);

			if (m_rtScissors.right > 0 && m_rtScissors.bottom > 0)
			{
				//rect영역을 벗어난것은 무조건 hide처리
				if (m_rtScissors.left > m_rtScissors.right || m_rtScissors.top > m_rtScissors.bottom)
				{
					_FreeSprite(pLetter, i);
					continue;
				}

				rtBox.left = max(0, m_rtScissors.left - m_rtView.left);
				rtBox.top = max(0, m_rtScissors.top - m_rtView.top);
				rtBox.right += min(0, m_rtScissors.right - m_rtView.right);
				rtBox.bottom += min(0, m_rtScissors.bottom - m_rtView.bottom);
			}
			
			INT32 x = pLetter->getPosX() + pLetter->getSpritePosX( i) - m_ShowX;
			INT32 y = pLetter->getPosY() - m_ShowY;
			i3Rect rtLetter;
			rtLetter.set(	x, y, 
							x + pLetter->getGlyphWidth( i) - 1, y + pLetter->getHeight() - 1);

			//Sprite단위로 해야함
			i3Rect rtIntersect;
			TOUCH_TYPE touch = rtBox.Intersect( &rtLetter, &rtIntersect);

			bool bClip = false;

			if( touch == TOUCH_NONE)
			{
				_FreeSprite( pLetter, i);
				continue;
			}
			else if( touch == TOUCH_PARTIAL)
			{
				bClip = true;
			}

			_AllocSprite( pLetter, i);

			// Sprite 설정
			INT32 hSprite = pLetter->getSprite( i);
			if( hSprite == -1)	// 아직 할당하지 못했다. 왜 이경우가 나오는지는 모름..
				continue;		// 이제는 나와도 되도록 해야한다..

			//temp
			INT32 idx = pLetter->getIndex();
			bool bSelected = ((idx >= m_nIdxBeginBlock) && (idx < m_nIdxEndBlock));		//Letter를 분할해서 블록지정해야하는 경우..

			// Font Texture를 사용함을 알리기 위해, Texture Range를 음수로 설정
			g_pUIRenderer->setTexRange( hSprite, -1.0f, (bSelected?1.f:0.f), (REAL32) m_ShadowType, 0.0f);
					

			// Color
			COLOR c;
			i3Color::ConvertRGBA32( pLetter->getColor(), &c);
			g_pUIRenderer->setColor( hSprite, &c);

			
			REAL32 x1, y1, x2, y2;
			if( bClip == false)
			{
				// Position
				x1 = (REAL32) (m_rtView.left + x);
				y1 = (REAL32) (m_rtView.top + y);
				g_pUIRenderer->setPos( hSprite, x1, y1, 0.f);

				// Size
				g_pUIRenderer->setSizeAndRotate( hSprite, (REAL32) pLetter->getGlyphWidth( i), (REAL32) (pLetter->getHeight()), 0.0f);

				// Texture Coordinates
				x1 = (REAL32) pGlyph->getPosX() + pLetter->getGlyphOffsetX(i);
				y1 = (REAL32) pGlyph->getPosY();
				x2 = x1 + pGlyph->getWidth();

				REAL32 scaleH = (REAL32) pGlyph->getHeight() / (REAL32) pGlyph->getRealHeight();
				y2 = y1 + (pGlyph->getHeight() - scaleH);	//지금 글리프 사이즈가 레터사이즈보다 2큰가? 
				g_pUIRenderer->setTexCoord( hSprite, x1, y1, x2, y2);
			}
			else
			{
				// Position
				REAL32 x = (REAL32) (m_rtView.left + rtIntersect.left);
				REAL32 y = (REAL32) (m_rtView.top + rtIntersect.top);
				g_pUIRenderer->setPos( hSprite, x, y, 0.f);

				// Size
				g_pUIRenderer->setSizeAndRotate( hSprite, (REAL32) rtIntersect.getWidth(), (REAL32) rtIntersect.getHeight(), 0.0f);

				// scaleW의 양쪽 -2는 pGlyph->setWidth와 pGlyph->setRealWidth에서 각각 2를 더하여 최종 계산되었기 때문에, 비율계산때는 빼고 처리하는
				// 것이 바람직한 것으로 보고 수정함..
				// (2015.02.16.수빈)
				// 0으로 나눌수있는 상황이 제현됨... 방어코드 추가...
				REAL32 scaleW = (REAL32)max(I3_EPS, pGlyph->getWidth() - 2) / (REAL32)max(I3_EPS, pGlyph->getRealWidth() - 2);
				REAL32 scaleH = (REAL32) pGlyph->getHeight() / (REAL32) pGlyph->getRealHeight();

				// Texture Coordinates
				x1 = (REAL32) (pGlyph->getPosX()) + (REAL32)pLetter->getGlyphOffsetX(i) + (REAL32)(rtIntersect.left - rtLetter.left) * scaleW;
				y1 = (REAL32) (pGlyph->getPosY()) + (REAL32)(rtIntersect.top - rtLetter.top) * scaleH;

				x2 = x1 + (REAL32)(rtIntersect.getWidth()) * scaleW;
				y2 = y1 + (REAL32)(rtIntersect.getHeight()) * scaleH - scaleH;			// + 2.f;
				g_pUIRenderer->setTexCoord( hSprite, x1, y1, x2, y2);
			}
		}

		pLetter = pLetter->getNext();
	}
}

void i3UIText::RedrawGlyphs( void)		//Redraw onto VRAM
{
	for( size_t i=0; i< m_LineInfoList.size(); i++)
	{
		LINE_INFO* pInfo = m_LineInfoList[i];

		g_pUniscribe->_ScriptTextOut( pInfo->m_pFont, pInfo->m_RunList, m_nDefFontHeight);		// 글리프리스트 없는 오버로딩 버전..
	}

	RenderText();
}

i3UILetter* i3UIText::AddLetter( I3UI_TEXT_PARSE_CTX * pCtx, SPRITE_INFO* pInfo, INT32 nGlyphs)
{
	i3UILetter* pLetter = _AllocLetter( pCtx, nGlyphs);

	pLetter->setPos( (INT16)pCtx->m_X, (INT16)pCtx->m_Y);	// warning C4244
	pLetter->setState( I3UI_LETTER_STATE_MODIFIED);
	pLetter->setLocalTime( 0.0f);
	pLetter->setColor( pCtx->m_CurTextColor);
	pLetter->setClipped( false);

	for( INT32 i=0; i< nGlyphs; i++)
	{
		pLetter->setGlyph( i, pInfo[i]._pGlyph, pInfo[i]._nPosX, pInfo[i]._nOffsetX, pInfo[i]._nWidth);
	}

	return pLetter;
}

i3UILetter* i3UIText::AddNullLetter( I3UI_TEXT_PARSE_CTX * pCtx)
{
	i3UILetter* pLetter = _AllocLetter( pCtx, 0);

	pLetter->setState( I3UI_LETTER_STATE_MODIFIED);
	pLetter->setLocalTime( 0.0f);
	pLetter->setColor( pCtx->m_CurTextColor);
	pLetter->setClipped( false);
	pLetter->setCode( 0);

	i3UILetter* pLast = GetLastLetter();
	
	if( pLast == pLetter)	//필요한가??
	{
		pLetter->setWidth( 0);
		pLetter->setHeight( (INT16)m_nDefFontHeight);
		pLetter->setPos( 0, 0);
		pLetter->setLine( 0);
		return pLetter;
	}

	bool bRTL = pLast->isRightCaret();

	pLetter->setRightCaret( bRTL);
	pLetter->setWidth( 0);
	pLetter->setHeight( (INT16)m_nDefFontHeight);

	if( bRTL)
	{
		if( pLast->getCode() == 10)		//개행문자
		{
			pLetter->setPos( 0, pLast->getPosY() + pLast->getHeight());
			pLetter->setLine( pLast->getLine() + 1);
		}
		else
		{
			pLetter->setPos( pLast->getPosX(), pLast->getPosY());
			pLetter->setLine( pLast->getLine());
			
			//기존 연결 끊고
			pCtx->m_pTextTail = pLetter->getPrev();
			
			if( pLetter->getPrev() != nullptr)
				pLetter->getPrev()->setNext( nullptr);

			//Prev
			i3UILetter* pPrev = pLast->getPrev();
			if( pPrev != nullptr)
				pPrev->setNext( pLetter);
			else
				m_pFirstLetter = pLetter;
			pLetter->setPrev( pPrev);

			//Next
			pLast->setPrev( pLetter);
			pLetter->setNext( pLast);
		}
	}
	else
	{
		if( pLast->getCode() == 10)		//개행문자
		{
			pLetter->setPos( 0, pLast->getPosY() + pLast->getHeight());
			pLetter->setLine( pLast->getLine() + 1);
		}
		else
		{
			pLetter->setPos( pLast->getPosX() + pLast->getWidth(), pLast->getPosY());
			pLetter->setLine( pLast->getLine());
		}
	}
	
	return pLetter;
}

void i3UIText::FreeLetterList( i3UILetter * pNode)
{
	if( g_pUIRenderer == nullptr)
		return;

	i3UILetter * pHead = pNode;

	while( pNode != nullptr)
	{
		for( INT32 i=0; i< pNode->getSpriteCount(); i++)
		{
			INT32 hSprite = pNode->getSprite( i);
			if( hSprite != -1)
			{
				g_pUIRenderer->FreeSprite( hSprite);
				pNode->setSprite( i, -1);
			}
		}
		
		pNode = pNode->getNext();
	}

	g_pTextManager->FreeLetterList( pHead);
}

i3UILetter * i3UIText::_AllocLetter( I3UI_TEXT_PARSE_CTX * pCtx, INT32 nSpriteCount)
{
	if( g_pUIRenderer == nullptr)
		return nullptr;

	i3UILetter * pLetter;
	bool bAllocated = false;

	if( pCtx->m_pFreePool != nullptr)
	{
		// 기존에 사용되던 Letter를 재사용한다.
		// 이 경우, Sprite를 이미 가지고 있기 때문에 할당받을 필요가 없다.
		pLetter = pCtx->m_pFreePool;

		pCtx->m_pFreePool = pLetter->getNext();

		pLetter->setNext( nullptr);

		if( pCtx->m_pFreePool != nullptr)
			pCtx->m_pFreePool->setPrev( nullptr);
		
	}
	else
	{
		// 새로운 Letter를 할당해야 한다.
		// 이 경우, Sprite 역시 새롭게 할당받아 설정해주어야 한다.
		pLetter = g_pTextManager->AllocLetter();

		for( INT32 i=0; i< pLetter->getSpriteCount(); i++)
		{
			pLetter->setSprite( i, -1);
		}
		pLetter->setNext( nullptr);

		bAllocated = true;
	}

	pLetter->setIndex( -1);
	pLetter->setSpriteCount( nSpriteCount);

	pLetter->setPrev( pCtx->m_pTextTail);

	if( pCtx->m_pTextTail != nullptr)
		pCtx->m_pTextTail->setNext( pLetter);

	if( m_pFirstLetter == nullptr)
		m_pFirstLetter = pLetter;

	pCtx->m_pTextTail = pLetter;

	return pLetter;
}

void i3UIText::_AllocSprite( i3UILetter* pLetter, INT32 i)
{
	// 이것갖고도 모자라면 pLetter에 -1 직접 넣어야 한다..
	if (!m_bEnable)
		return;

	//I3ASSERT( i < pLetter->getSpriteCount());
	I3ASSERT( i < MAX_GLYPH_PER_LETTER);
	
	INT32 hCurSprite = pLetter->getSprite( i);

	if( hCurSprite == -1)
	{
		INT32 hSprite = g_pUIRenderer->AllocSprite( 0, this);
		I3ASSERT( hSprite != -1);

		g_pUIRenderer->setEnableSprite( hSprite, true);

		pLetter->setSprite( i, hSprite);
	}
}

void i3UIText::_FreeSprite( i3UILetter* pLetter, INT32 i)
{
	//I3ASSERT( i < pLetter->getSpriteCount());
	I3ASSERT( i < MAX_GLYPH_PER_LETTER);

	INT32 hCurSprite = pLetter->getSprite( i);

	if( hCurSprite != -1)
	{
		g_pUIRenderer->setEnableSprite( hCurSprite, false);
		g_pUIRenderer->FreeSprite( hCurSprite);

		pLetter->setSprite( i, -1);
	}
}

void i3UIText::CopyStyles( i3UIText* pDest)
{
	if( pDest == nullptr)
		return;

	pDest->SetFont( m_pFont);
	pDest->setTextStyle( m_Style);
	pDest->SetShadowColor( m_ShadowColor);
	pDest->SetShadowType( m_ShadowType);
	pDest->SetDefColor( m_DefColor);
	pDest->SetAlign( m_AlignX, m_AlignY);
}



UINT32 i3UIText::getFontColor( INT32 nCharIdx)
{
	// 현재 이스케이프 시퀀스가 없더라도, 칼라테이블과 칼라인덱스 테이블이 존재할수 있다.
	if (m_RefinedColorTable.empty() ) // m_bContainEscSeq == false) 
		return m_DefColor;

	if (0 <= nCharIdx  && nCharIdx < (INT32)m_RefinedColorIdxList.size() )		
		return m_RefinedColorTable[ m_RefinedColorIdxList[nCharIdx] ];

	return m_DefColor;
}

void i3UIText::_AlignLettersX( INT32 width)
{
	if( m_AlignX == I3UI_TEXT_ALIGN_X_LEFT || m_AlignX == I3UI_TEXT_ALIGN_X_INVALID)
		return;

	i3UILetter* pCur = m_pFirstLetter;
	
	i3UILetter* pFirst = pCur;
	i3UILetter* pLast = nullptr;
	
	INT16 nCurLine = 0;
	INT16 nLineWidth = 0;

	while( pCur != nullptr)
	{
		if( pCur->getLine() > nCurLine)
		{
			nLineWidth -= (INT16)m_nSpaceofLettersX;

			//process previous line
			INT16 deltaX = 0;
			if( m_AlignX == I3UI_TEXT_ALIGN_X_CENTER)
			{
				deltaX = (INT16)((width - nLineWidth) / 2);
			}
			else	//I3UI_TEXT_ALIGN_X_RIGHT
			{
				deltaX = (INT16)(width - nLineWidth);
			}
			
			while( pFirst != nullptr && pLast != nullptr && pFirst != pLast->getNext())
			{
				pFirst->setPos( pFirst->getPosX() + deltaX, pFirst->getPosY());
				pFirst = pFirst->getNext();
			}

			I3ASSERT( pLast != nullptr);

			//reset
			nCurLine = pCur->getLine();
			nLineWidth = 0;
		}
		
		nLineWidth += pCur->getWidth() + (INT16)m_nSpaceofLettersX;

		pLast = pCur;
		pCur = pCur->getNext();
	}

	nLineWidth -= (INT16)m_nSpaceofLettersX;

	INT16 deltaX = 0;
	if( m_AlignX == I3UI_TEXT_ALIGN_X_CENTER)
	{
		deltaX = (INT16)((width - nLineWidth) / 2);
	}
	else	//I3UI_TEXT_ALIGN_X_RIGHT
	{
		deltaX = (INT16)(width - nLineWidth);
	}

	while( pFirst != nullptr && pLast != nullptr && pFirst != pLast->getNext())
	{
		pFirst->setPos( pFirst->getPosX() + deltaX, pFirst->getPosY());
		pFirst = pFirst->getNext();
	}

	I3ASSERT( pLast != nullptr);
}

void i3UIText::_AlignLettersY( INT32 height)
{
	if( m_AlignY == I3UI_TEXT_ALIGN_Y_TOP || m_AlignY == I3UI_TEXT_ALIGN_Y_INVALID)
		return;

	//calculate delta
	INT16 deltaY = 0;
	if( m_AlignY == I3UI_TEXT_ALIGN_Y_MIDDLE)
	{
		deltaY = (INT16)((height - m_ExtentY) / 2);
	}
	else	//I3UI_TEXT_ALIGN_Y_BOTTOM
	{
		deltaY = (INT16)(height - m_ExtentY);
	}

	//apply to all letters
	i3UILetter* pLetter = m_pFirstLetter;
	while( pLetter != nullptr)
	{
		pLetter->setPos( pLetter->getPosX(), pLetter->getPosY() + deltaY);

		pLetter = pLetter->getNext();
	}
}

INT32 i3UIText::_GetAccCharacterCount( INT32 lineIdx, INT32 logIdx)
{
	INT32 nCharCount = 0;
	for( size_t i=0; i< m_RunList.size(); i++)
	{
		SCRIPT_RUN* pRun = m_RunList[i];

		if( pRun->_lineIdx < lineIdx)
		{
			nCharCount += pRun->_nChars;
		}
		else if( pRun->_lineIdx == lineIdx)
		{
			if( pRun->_logIdx < logIdx)
			{
				nCharCount += pRun->_nChars;
			}
		}
		else
		{
			break;
		}
	}

	return nCharCount;
}

struct CLUSTER_INFO
{
	INT32	_charIdx = 0;
	INT32	_width = 0;
};

void i3UIText::_GenGlyphIndexTable( const SCRIPT_RUN* pRun, const i3::vector<i3UIGlyph*>& GlyphList, INT32 nTotalGlyphs, 
									INT32 nCurX, INT32 nWidth, INT32* pOutGlyphIndex, bool* pOutLineBreak)
{
	I3ASSERT( pRun->_nChars <= 256);

	CLUSTER_INFO clusterInfo[256];	//cluster's abc (temp)
	INT32 n = 0;

	//일단 glyph를 역순으로 배치
	for( INT32 i=0; i< pRun->_nChars; i++)
	{
		//캐릭터는 뒤부터, 글리프는 앞부터
		INT32 j = pRun->_nChars-(i+1);	
		
		if( pRun->_pLogAttrs[j].fCharStop == false)
			continue;

		SPRITE_INFO info[MAX_GLYPH_PER_LETTER];
		i3mem::FillZero( info , sizeof( SPRITE_INFO) * MAX_GLYPH_PER_LETTER);
		ABC abc;
				
		g_pUniscribe->GenClusterInfo( pRun, m_GlyphList, j, nTotalGlyphs, info, &abc, &clusterInfo[n]._width);
		clusterInfo[n]._charIdx = j;
		n++;
	}
	
	//자를 위치를 뒤에서부터 계산
	bool bBreak = false;
	INT32 cnt = 0;

	INT32 prevBreak = n;	//cluster
	INT32 prevIdx = 0;		//char	
		
	for( INT32 i=n-1; i>=0; i--)
	{
		if( nCurX + clusterInfo[i]._width >= nWidth)
		{
			bBreak = true;
		}

		if( bBreak)
		{
			nCurX = 0;
			bBreak = false;
			
			if( i+1<n)
			{
				//find word wrap point
				INT32 nClusterBreak = -1;
				for( INT32 k=i+1; k<prevBreak; k++)
				{
					INT32 j=clusterInfo[k]._charIdx;
					if( pRun->_pLogAttrs[j].fSoftBreak == true)
					{
						nClusterBreak = k+1;
						break;
					}
				}

				if( nClusterBreak == -1 || nClusterBreak == prevBreak)
				{
					nClusterBreak = i+1;	//바로 다음 Cluster까지는 같은 줄에 유지
				}
				else
				{
					for( INT32 k=i+1; k<nClusterBreak; k++)
					{
						nCurX += clusterInfo[k]._width + m_nSpaceofLettersX;		//width 수정
					}
				}
								
				for( INT32 j=clusterInfo[nClusterBreak]._charIdx; j>=prevIdx; j--)
				{
					pOutGlyphIndex[cnt++] = j;
				}
				pOutLineBreak[cnt] = true;

				prevIdx = clusterInfo[nClusterBreak]._charIdx + 1;			
				prevBreak = nClusterBreak;		
			}
			else
			{
				pOutLineBreak[cnt] = true;
			}
		}

		nCurX += clusterInfo[i]._width + m_nSpaceofLettersX;
	}

	//index table생성
	for( INT32 j=pRun->_nChars-1; j>=prevIdx; j--)
	{
		pOutGlyphIndex[cnt++] = j;
	}
	
}

void i3UIText::OnDraw( i3UIRenderer * pRenderer)
{
	i3UILetter* pNode = m_pFirstLetter;

	while( pNode != nullptr)
	{
		for( INT32 i=0; i< pNode->getSpriteCount(); i++)
		{
			INT32 hSprite = pNode->getSprite( i);

			if( hSprite != -1)
			{
				pRenderer->Draw_Kick( hSprite);
			}
		}

		pNode = pNode->getNext();
	}
	
	if (i3UI::getGlobalIme() && i3UI::getGlobalIme()->getInputLanguage() == LANG_JAPANESE && m_wcCodeCaret != 0)
	{
			// 		for(int i=0; i<MAXCOMPBOX; i++)
			// 		{
			// 			if( m_hSpriteCompBox[i] != -1 )
			// 				pRenderer->Draw_Kick( m_hSpriteCompBox[i]);
			// 		}
	}
	else if(m_hSpriteCaret != -1)
	{
		pRenderer->Draw_Kick( m_hSpriteCaret);
	}
}

INT32 i3UIText::ConvertIndexToRawString( INT32 idxRefined)
{
	I3ASSERT( m_bContainEscSeq);
	
	return (INT32)m_RefinedtoSrcMapper[idxRefined];
}

bool	i3UIText::BuildSrcStringIntoRefinedStringMapper( i3::vector<size_t>& out) const
{
	out.clear();
	if (!m_bContainEscSeq)		// 문자열에 색상시퀀스가 없다면 처리하지 않고 false
		return false;

	I3ASSERT( !m_RefinedtoSrcMapper.empty() );
	
	const size_t num_orig_str = m_wstrRange.size();
	out.resize(num_orig_str, size_t(-1));		// 결과물에 -1이 남은 경우 색상을 위한 시퀀스이다..
	
	const size_t num_refined_str = m_RefinedtoSrcMapper.size();

	for (size_t i = 0 ; i < num_refined_str ; ++i)
		out[ m_RefinedtoSrcMapper[i] ] = i;
	
	return true;
}

DWORD	i3UIText::AddPartialTextColor( I3COLOR clr )
{
	if (m_bContainEscSeq == false)		// 이런 경우, 색상테이블과 색상인덱스배열이 모두 비어있을수 있음..(채워야한다)
	{
		if ( m_RefinedColorTable.empty() )
		{
			m_RefinedColorTable.push_back(m_DefColor);
			m_RefinedColorIdxList.resize(m_wstrRange.size(), 0);
			m_UserColorTableBeginIndex = 1;
		}
	}
	
	m_RefinedColorTable.push_back(reinterpret_cast<DWORD&>(clr));
	return DWORD(m_RefinedColorTable.size()) - 1;
}

void	i3UIText::ModifyPartialTextColor(DWORD clrIndex, I3COLOR clr)
{
	if ( clrIndex >= m_RefinedColorTable.size() )
		return;

	m_RefinedColorTable[clrIndex] = reinterpret_cast<DWORD&>(clr);
}


void	i3UIText::ModifyRefinedPartialTextColor( size_t refinedTextIndex, DWORD clrIndex)
{
	if (refinedTextIndex >= m_RefinedColorIdxList.size() )
		return;

	if (clrIndex >= m_RefinedColorTable.size())
		return;

	const DWORD prevClrIdx = m_RefinedColorIdxList[refinedTextIndex];

	if ( prevClrIdx > 0 && prevClrIdx < m_UserColorTableBeginIndex ) // 디폴트 색상 아닌 이스케잎 칼라 색상인 경우는 색상변경 막는다.
		return;

	m_RefinedColorIdxList[refinedTextIndex] = clrIndex;
}


// 텍스트 리셋시 이 함수는 false를 리턴하게 된다...
bool	i3UIText::IsPartialTextColorApplied() const
{
	return DWORD(m_RefinedColorTable.size()) > m_UserColorTableBeginIndex;
}


//저장된 문장들 정보 삭제
void i3UIText::clearScriptSecntence()
{
	for (int i = 0; i < (int)m_vecScriptSentence.size(); i++)
	{
		I3_SAFE_DELETE( m_vecScriptSentence[i] );
	}
	m_vecScriptSentence.clear();
}
