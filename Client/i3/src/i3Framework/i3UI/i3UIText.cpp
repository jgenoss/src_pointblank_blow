#include "i3FrameworkPCH.h"
#include "i3UIText.h"
#include "i3UITextManager.h"
#include "i3UIRenderer.h"

//I3_CLASS_INSTANCE( i3UIText, i3ElementBase);
I3_CLASS_INSTANCE( i3UIText);
//	해당 텍스트에서 가변 멀티바이트의 한 문자를 추출합니다. (반환 값은 멀티바이트의 바이트수)
INT32 _ExtractMultibyteFromText(const WCHAR16 * text, WCHAR16 * outbuf, INT32 bufCount)
{
	//	다국어를 위해 한 문자의 최소 버퍼 크기는 4바이트입니다. 그러므로 버퍼크기는 널 종료 문자 포함해서 5이상 되어야 합니다.
	I3ASSERT( bufCount > 4 && outbuf != nullptr && text != nullptr);

	INT32 len = 0;

	WCHAR16 * start = (WCHAR16*) text;
	WCHAR16 * next = ::CharNextW( start);	//	다음 문자로 이동
	I3ASSERT( &(*start) <= &(*next));

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

	I3ASSERT( len < bufCount);
	outbuf[len] = 0;

	return len;
}

i3UIText::i3UIText(void)
{
	m_pFont = nullptr;

	m_DefColor = 0xFFFFFFFF;

	m_pText = nullptr;
	m_pTextBuf = nullptr;
	m_MaxLength = 0;

	m_ShowX = 0;
	m_ShowY = 0;
	m_ExtentX = 0;
	m_ExtentY = 0;

	m_idxCursor = 0;
	m_idxBeginBlock = -1;
	m_idxEndBlock = -1;
	m_codeCursor = 0;
	m_hCursorSprite = -1;
	m_timeCursor = 0.0f;
	m_defFontHeight = 13;
	m_defFontWidth = 13;
	m_pGlyph_Cursor = nullptr;

	m_ShadowType = I3UI_TEXT_SHADOW_NONE;
	m_AlignX = I3UI_TEXT_ALIGN_X_LEFT;		// 현재 I3UI_TEXT_ALIGN_X_LEFT 만 동작합니다. 

	m_defTabWidth = 4;

	m_Box.set( 0, 0, 0, 0);

	m_nSpaceofLettersX = m_nSpaceofLettersY = 5;

	m_nLinePitch = 0;
}

i3UIText::~i3UIText(void)
{
	EndEdit();

	ResetText();

	I3_SAFE_RELEASE( m_pFont);

	if (m_bUseBuffer)							// 버퍼를 쓴다면...
	{
		I3MEM_SAFE_FREE_POINTER(m_pTextBuf);
	}
}

void i3UIText::SetFont( i3UIFont * pFont)
{
	I3_REF_CHANGE( m_pFont, pFont);

	if( m_pFont != nullptr)
	{
		WCHAR16 temp[4] = { 0x20, };

		i3UIGlyph * pGlyph = g_pTextManager->AddGlyph( m_pFont, temp, 1);

		if( pGlyph != nullptr)
		{
			m_defTabWidth = pGlyph->getWidth() * 4;
			m_defFontHeight = pGlyph->getHeight();
			m_defFontWidth = pGlyph->getWidth();
		}
	}
}

void i3UIText::ResetText(void)
{
	if((g_pTextManager != nullptr) && (m_pText != nullptr))
	{
		_FreeLetterList( m_pText);

		m_pText = nullptr;
	}

	m_ShowX = 0;
	m_ShowY = 0;
	m_ExtentX = 0;
	m_ExtentY = 0;
}

void i3UIText::_FreeLetterList( i3UILetter * pNode)
{
	if( g_pUIRenderer == nullptr)
		return;

	i3UILetter * pHead = pNode;

	while( pNode != nullptr)
	{
		if( pNode->getSprite() != -1)
		{
			g_pUIRenderer->FreeSprite( pNode->getSprite());
			pNode->setSprite( -1);
		}

		pNode = pNode->getNext();
	}

	g_pTextManager->FreeLetterList( pHead);
}

i3UILetter * i3UIText::_AllocLetter( I3UI_TEXT_PARSE_CTX * pCtx, bool bNeedSprite)
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

		pLetter->setSprite( -1);
		pLetter->setNext( nullptr);

		bAllocated = true;
	}

	if( bNeedSprite)
	{
		if( pLetter->getSprite() == -1)
		{
			INT32 hSprite = g_pUIRenderer->AllocSprite( 0, this);
			I3ASSERT( hSprite != -1);

			pLetter->setSprite( hSprite);
		}
	}
	else
	{
		if( pLetter->getSprite() != -1)
		{
			g_pUIRenderer->FreeSprite( pLetter->getSprite());

			pLetter->setSprite( -1);
		}
	}

	pLetter->setPrev( pCtx->m_pTextTail);

	if( pCtx->m_pTextTail != nullptr)
		pCtx->m_pTextTail->setNext( pLetter);

	if( m_pText == nullptr)
		m_pText = pLetter;

	pCtx->m_pTextTail = pLetter;

	return pLetter;
}

void i3UIText::setVisible( bool bVisible)
{
	if( g_pUIRenderer == nullptr)
		return;

	i3UILetter * pNode = m_pText;

	while( pNode != nullptr)
	{
		if( pNode->getSprite() != -1)
		{
			g_pUIRenderer->setEnable( pNode->getSprite(), bVisible);
		}

		pNode = pNode->getNext();
	}
}

i3UILetter * i3UIText::AddLetter( I3UI_TEXT_PARSE_CTX * pCtx, i3UIGlyph * pGlyph, INT32 idx, bool bClip, i3Rect * pIntersect, bool bSelected)
{
	i3UILetter * pLetter;
	INT32 hSprite;

	pLetter = _AllocLetter( pCtx, pGlyph != nullptr);

	pLetter->setPos( (INT16)pCtx->m_X, (INT16)pCtx->m_Y);	// warning C4244
	pLetter->setState( I3UI_LETTER_STATE_MODIFIED);
	pLetter->setGlyph( pGlyph);
	pLetter->setIndex( (INT16)idx);		// warning C4244
	pLetter->setLocalTime( 0.0f);
	pLetter->setColor( pCtx->m_CurTextColor);
	pLetter->setClipped( bClip);

	// Sprite 설정
	if( pGlyph != nullptr)
	{
		REAL32 x1, y1, x2, y2;
		COLOR c;

		hSprite = pLetter->getSprite();

		// Color
		i3Color::ConvertRGBA32( pLetter->getColor(), &c);
		g_pUIRenderer->setColor( hSprite, &c);

		x1 = (REAL32) (m_Box.left + pLetter->getPosX());
		y1 = (REAL32) (m_Box.top + pLetter->getPosY());

		// Font Texture를 사용함을 알리기 위해, Texture Range를 음수로 설정
		if( bSelected)
			g_pUIRenderer->setTexRange( hSprite, -1.0f, 1.0f, 1.0f, (REAL32) m_ShadowType);
		else
			g_pUIRenderer->setTexRange( hSprite, -1.0f, 0.0f, -1.0f, (REAL32) m_ShadowType);

		if( bClip == false)
		{
			// Position
			g_pUIRenderer->setPos( hSprite, x1, y1, 0.0f);

			// Size
			g_pUIRenderer->setSizeAndRotate( hSprite, (REAL32) pGlyph->getWidth(), (REAL32) pGlyph->getHeight(), 0.0f);

			// Texture Coordinates
			x1 = (REAL32) pGlyph->getPosX();
			y1 = (REAL32) pGlyph->getPosY();
			x2 = x1 + pGlyph->getWidth();
			y2 = y1 + pGlyph->getHeight();
			g_pUIRenderer->setTexCoord( hSprite, x1, y1, x2, y2);
		}
		else
		{
			// Position
			x1 += (REAL32) pIntersect->left;
			y1 += (REAL32) pIntersect->top;
			g_pUIRenderer->setPos( hSprite, x1, y1, 0.0f);

			// Size
			g_pUIRenderer->setSizeAndRotate( hSprite, (REAL32) pIntersect->getWidth(), (REAL32) pIntersect->getHeight(), 0.0f);

			// Texture Coordinates
			x1 = (REAL32) (pGlyph->getPosX() + pIntersect->left);
			y1 = (REAL32) (pGlyph->getPosY() + pIntersect->top);
			x2 = x1 + pIntersect->getWidth();
			y2 = y1 + pIntersect->getHeight();
			g_pUIRenderer->setTexCoord( hSprite, x1, y1, x2, y2);
		}
	}

	return pLetter;
}

bool i3UIText::isPrintableLetter( WCHAR16 * code)
{
	if( (code[0] >= '0') && (code[0] <= '9'))
		return true;

	if( (code[0] >= 'A') && (code[0] <= 'Z'))
		return true;

	if( (code[0] >= 'a') && (code[0] <= 'z'))
		return true;

	return false;
}

void i3UIText::_WordWrap( I3UI_TEXT_PARSE_CTX * pCtx, INT32 idxStart, i3UIGlyph * pFirstGlyph)
{
	WCHAR16 * c;
	WCHAR16 outbuf[8];
	i3UIGlyph * pGlyph;
	INT32 maxH = 0, idx = idxStart;
	INT32 w = 0, i;
	INT32 len = 0;

	bool bPrintable;

	if( isPrintableLetter( pFirstGlyph->getCode()))
	{
		// 현재 위치에서부터 전방으로 White Letter를 검색한다.
		do
		{	
			len = _ExtractMultibyteFromText( &m_pTextBuf[ idx], outbuf, 8);

			pGlyph = g_pTextManager->AddGlyph( m_pFont, outbuf, len);
			c = pGlyph->getCode();

			bPrintable = isPrintableLetter( c);

			if( bPrintable)
			{
				//w	+= pGlyph->getWidth();
				w	+= pGlyph->getBaselineOffset() + m_nSpaceofLettersX;
			}

			maxH = MAX( maxH, pGlyph->getHeight() + m_nSpaceofLettersY);

			idx--;
		} while( w < m_Box.getWidth() && bPrintable);

		if( isPrintableLetter(c) == false)
			idx += 2;

		if( w >= m_Box.getWidth())
		{
			// 문장의 처음까지 줄 바꿈될 적절한 공백이 없다.
			// 그냥 pLetter 문자부터 아래로 내린다.
			idx = idxStart;
			//w = pFirstGlyph->getWidth();
			w = pFirstGlyph->getBaselineOffset() + m_nSpaceofLettersX;
		}
	}
	else
	{
		//w = pFirstGlyph->getWidth();
		w = pFirstGlyph->getBaselineOffset() + m_nSpaceofLettersX;
		maxH = pFirstGlyph->getHeight() + m_nSpaceofLettersY;
	}
	
	// pStart부터 줄바꿈을 시작하도록 좌표를 조정해 준다.
	pCtx->m_X = -m_ShowX + w;
	pCtx->m_Y += maxH;

	i3UILetter * pLetter;

	bool bCorrect = FindLetterByIndex( idx, &pLetter);

	if( (pLetter != nullptr) && (bCorrect))
	{
		INT16 offX, offY, x, y;

		offX = (INT16)(-m_ShowX - pLetter->getPosX());		// warning C4244
		offY = (INT16)(pCtx->m_Y - pLetter->getPosY());		// warning C4244

		for( i = idx; (i <= idxStart) && (pLetter != nullptr); i++)
		{
			x = pLetter->getPosX() + offX;
			y = pLetter->getPosY() + offY;

			pLetter->setPos( x, y);

			if( pLetter->getSprite() != -1)
			{
				g_pUIRenderer->setPos( pLetter->getSprite(), (REAL32) (m_Box.left + x), (REAL32) (m_Box.top + y), 0.0f);
			}

			pLetter = pLetter->getNext();
		}

		// _WordWrap 호출 전에 TOUCH_NONE 이어서 Letter 가 생성되지 않았다. 
		// 여기서 Letter를 생성하여 reposition 해준다. 
		//if( (i <= idxStart) && (pLetter == nullptr))
		//{
		//}

	}
}

void i3UIText::_NormalLetter( I3UI_TEXT_PARSE_CTX * pCtx, const WCHAR16 * code, INT32 idx, INT32 Length)
{
	i3UIGlyph * pGlyph;
	bool bSelected;

	pGlyph = g_pTextManager->AddGlyph( m_pFont, code, Length);

	if( pGlyph != nullptr)
	{
		INT32 x, y;
		i3Rect rcLetter, intersect;

		pCtx->m_X -= ( pGlyph->getWidth() - pGlyph->getBaselineOffset());

		x = m_Box.left + pCtx->m_X; 
		y = m_Box.top + pCtx->m_Y;

		rcLetter.set( x, y, x + pGlyph->getWidth(), y + pGlyph->getHeight());

		bSelected = ((idx >= m_idxBeginBlock) && (idx < m_idxEndBlock));

		// Letter의 영역이 View Box의 영역에 들어오는지,
		// 만약 그렇다면 교집합의 영역을 구한다.
		TOUCH_TYPE touch = m_Box.Intersect( &rcLetter, &intersect);

		if( touch != TOUCH_NONE)
		{
			bool bClip = false;

			// 절대좌표인 intersectino을 상대좌표로 변환
			intersect.Offset( -x, -y);

			if( touch == TOUCH_PARTIAL)
			{
				if( isAutoLineFeed())
				{
					if( intersect.getHeight() < pGlyph->getHeight())	// height가 잘리는 경우
						bClip = true;
					else	// width가 잘리는 경우
						bClip = false;
				}
				else
					bClip = true;
			}

			AddLetter( pCtx, pGlyph, idx, bClip, &intersect, bSelected);
		}
		else if( isAutoLineFeed())
		{
			if( (pCtx->m_Y + pGlyph->getHeight() + pGlyph->getHeight()) <= m_Box.getHeight())
			{
				AddLetter( pCtx, pGlyph, idx, false, &intersect, bSelected);	// intersect가 인자로 넘겨지지만, bClip == false이기 때문에 사용되지 않는다.
			}
		}

		// 좌표 이동
		//pCtx->m_X += pGlyph->getWidth();
		if( pGlyph->getBaselineOffset() != pGlyph->getWidth())
		{
			pCtx->m_X += pGlyph->getWidth() + m_nSpaceofLettersX;
		}
		else
		{
			pCtx->m_X += pGlyph->getBaselineOffset() + m_nSpaceofLettersX;
		}
		
		if( pCtx->m_X > m_Box.getWidth())
		{
			if( isAutoLineFeed())
			{
				_WordWrap( pCtx, idx, pGlyph);
			}
		}
	}
}

bool i3UIText::IsInt( WCHAR16* pText)
{
	INT32 i=0;

	if( pText[i] == 0)
		return false;

	while( pText[i] != 0)
	{
		if( pText[i] < '0' || pText[i] > '9')
			return false;
		i++;
	}
	return true;
}

INT32 i3UIText::TextToInt( WCHAR16*	pText)
{
	INT32 val = 0;
	INT32 i = 0;

	while( pText[i] != 0)
	{
		val = val * 10 + (pText[i] - '0');
		i++;
	}

	return val;
}

bool i3UIText::IsWhiteCode( WCHAR16 c)
{
	static WCHAR16 _whitecode[] = { ' ', '\n', '\t', '\r', '\b', 0};
	
	WCHAR16* pCur = _whitecode;
	
	while( *pCur != 0)
	{
		if( *pCur == c)
			return true;
		pCur++;
	}
	
	return false;
}

INT32 i3UIText::StringTokenizer( WCHAR16* pszString, WCHAR16* pBuf)
{
	INT32 nToken = 0;
	WCHAR16* pCur = pszString;

	while( *pCur != 0)
	{
		while( (*pCur != 0) && (IsWhiteCode( *pCur) == true) )
		{
			pCur++;
		}

		if( *pCur == 0)
			return nToken;

		while( (*pCur != 0) && (IsWhiteCode( *pCur) == false) )
		{
			*pBuf = *pCur;
			pBuf++;
			pCur++;
		}

		if( *pCur == 0)
			return nToken;
	}

	return nToken;
}

//REAL32 i3UIText::TextToFloat( WCHAR16* pText)
//{
//}

// pESeq: a string starts with '%' and ends with 0
// len: length of pESeq
INT32 i3UIText::_EscapeSequence( I3UI_TEXT_PARSE_CTX* pCtx, const WCHAR16* pESeq, INT32 startIdx)
{
	// i3TLineLexer
	WCHAR16*	pTemp = (WCHAR16*)pESeq;
	WCHAR16		Token[256];
	WCHAR16		Sep;
	i3TLineLexer<WCHAR16> lexer;
	
	// first character must be '%'
	I3ASSERT( pTemp[0] == '%');
	
	lexer.SetMasterString( &pTemp[1]);
	bool		bCommand = true;
	bool		bCompleteSequence = false;
	
	while( lexer.GetNextToken( Token, &Sep, L" \n\t%") != -1)
	{
		// Check Sequence
		if( wcsncmp( Token, L"RGB", max( wcslen( Token), 3)) == 0 && bCommand)
		{
			if( Sep == '%')
			{
				pCtx->m_CurTextColor = m_DefColor;		// RGB%
				bCompleteSequence = true;
			}
			else
			{
				INT32 rgb[3];
				INT32 nCnt = 0;
				
				// To get r,g,b
				while( lexer.GetNextToken( Token, &Sep, L" \n\t%") != -1)
				{
					if( IsInt( Token) == true)
					{
						if( nCnt > 2)	// RGB 0 0 0 0 ..;;
							break;
						
						rgb[nCnt++] = TextToInt( Token);
					}
					else if( Token[0] != 0)
					{
						break;
					}
					
					// Complete
					if( Sep == '%')
					{
						if( nCnt == 3)		// RGB r g b %
						{
							I3COLOR c;
							c.r = (UINT8)rgb[0];	// warning C4244
							c.g = (UINT8)rgb[1];	// warning C4244
							c.b = (UINT8)rgb[2];	// warning C4244
							c.a = 0xFF;
							//I3TRACE( "_EscapeSequence: %d, %d, %d\n", c.r, c.g, c.b);

							pCtx->m_CurTextColor = c.GetRGBA();
						}
						else if( nCnt == 0)		// RGB %
						{
							pCtx->m_CurTextColor = m_DefColor;		
						}
						
						bCompleteSequence = true;
						break;
					}
				}
			}
		}

		if( bCompleteSequence)
			break;

		// %%
		if( Sep == '%')
		{
			if( lexer.GetCurrentPosition() == 1)
			{
				WCHAR16 temp[4] = { '%',};
				_NormalLetter( pCtx, temp, startIdx, 1);
			}
			
			break;
		}
	}

	return lexer.GetCurrentPosition() + 1;	// plus 1 for first '%'
}

void i3UIText::Parse( const WCHAR16 * pText)
{
	if( g_pTextManager == nullptr)
		return;

	INT32 i = 0, j = 0;
	I3UI_TEXT_PARSE_CTX ctx;

	// 기존에 활용되었던 i3UILetter가 있다면, 설정만 달리하여 재사용한다.
	ctx.m_X = -m_ShowX;
	ctx.m_Y = -m_ShowY;
	ctx.m_pTextTail = nullptr;				
	ctx.m_pFreePool = m_pText;			// nullptr일 수 있다.
	ctx.m_CurTextColor = m_DefColor;
	ctx.m_bQuit = false;

	m_pText = nullptr;

	m_ExtentX = 0;
	m_ExtentY = 0;

	WCHAR16 szCmd[4096], outbuf[8], tempc[4];
	bool bInCmd = false;
	INT32 idxCmd = 0;
	INT32 idx = 0;

	bool bChar = false;

	while( (ctx.m_bQuit == false) && (pText[i] != 0))
	{
		bChar = true;

		INT32 len = _ExtractMultibyteFromText( &pText[i], outbuf, 8);
		if( len == 1)
		{
			switch( pText[i])
			{
				case L'\r' :	// return key...  
					break;
				case L'\n' :
					//_NormalLetter( &ctx, pText[i], idx);

					ctx.m_X = -m_ShowX;
					ctx.m_Y += m_defFontHeight;
					break;
				case L'\t' :
					{
						_NormalLetter( &ctx, &pText[i], idx, 1);

						ctx.m_X = ((m_ShowX + ctx.m_X + m_defTabWidth) / m_defTabWidth) * m_defTabWidth;
					}
					break;

				case L'%':
					{
						/*while( pText[i] != 0)
						{
							if( pText[i] == '%')
							{
								_EscapeSequence2( );
								break;
							}
						}

						if( pText[i] == 0)
						{
						}*/
						
						//prev ver.
						INT32 nCnt = _EscapeSequence( &ctx, &pText[i], idx);
						i += nCnt - 1;
						idx += nCnt - 1;

						break;	
					}
				case L'{' :
					if( bInCmd)
					{
						// 다시 Command Mode를 나타내는 문자가 나왔으므로 기존에 입력된 문자열은 출력할 문자열로 간주
						tempc[0] = L'{';
						tempc[1] = 0;

						_NormalLetter( &ctx, tempc, idx, 1);

						for( j = 0; j < idxCmd; j++)
						{
							_NormalLetter( &ctx, &szCmd[j], idx, 1);
							idx++;
						}				
					}
					
					{
						idxCmd = 0;
						bInCmd = true;			// Command의 시작으로 간주
						bChar = false;
					}
					break;

				case L'}' :
					if( bInCmd)
					{
						szCmd[ idxCmd] = 0;
						bInCmd = false;

						// Command 완성
						if( ParseCommand( &ctx, szCmd) == false)
						{
							// 정상적인 Command가 아니기 때문에 그대로 출력문자로 간주
							tempc[0] = L'{';
							tempc[1] = 0;

							_NormalLetter( &ctx, tempc, idx, 1);

							for( j = 0; j < idxCmd; j++)
							{
								_NormalLetter( &ctx, &szCmd[j], idx, 1);
								idx++;
							}
						}
						else
						{
							bChar = false;
						}
					}
					break;

				default :
					if( !bInCmd)
						_NormalLetter( &ctx, &pText[i], idx, 1);
					break;
			}
		}
		else
		{
			_NormalLetter( &ctx, &pText[i], idx, len);
		}

		if( bChar)
		{
			if( bInCmd)
			{
				szCmd[ idxCmd] = pText[i];
				idxCmd++;
			}
			else
			{
				i3UIGlyph* pGlyph = m_pFont->FindGlyph( &pText[i], len);
				INT32 nLetterHeight = m_defFontHeight;
				if( pGlyph != nullptr)
				{
					nLetterHeight = pGlyph->getHeight();
				}
				
				m_ExtentX = MAX( m_ExtentX, (ctx.m_X + m_ShowX));
				m_ExtentY = MAX( m_ExtentY, (ctx.m_Y + m_ShowY + nLetterHeight));

				idx++;
			}
		}

		i += len;
	}

	// 아래의 0문자를 추가하지 않으면, 문자열 끝에서 입력하는 Caret의 좌표계산에도
	// 이상이 생기기 때문에 제거되어선 안됩니다.
	tempc[0] = 0x20;
	tempc[1] = 0;

	_NormalLetter( &ctx, tempc, idx, 1);

	// 기본적으로 Left Align 되므로 그 경우에는 패스
	//if( m_AlignX == I3UI_TEXT_ALIGN_X_CENTER || m_AlignX == I3UI_TEXT_ALIGN_X_RIGHT)
	_TextAlignX();
	

	if( ctx.m_pFreePool != nullptr)
	{
		// 이전 버퍼가 남아있었으며, 설정이 끝난 후에 문자열이 짧아진 경우
		_FreeLetterList( ctx.m_pFreePool);
	}

	AdjustLetters();
}

bool i3UIText::ParseCommand( I3UI_TEXT_PARSE_CTX * pCtx, const WCHAR16 * szCmd)
{
	if( i3String::NCompareW( szCmd, L"col:", 4) == 0)
	{
		const INT32 convLen = 256;

		// Color 설정
		WCHAR16 * pszTemp = (WCHAR16 *)szCmd + 4;
		WCHAR16 conv[convLen] = L"";
		I3COLOR textColor = {0,0,0,0};
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

				if( *pszTemp != ' ')
				{
					switch( order)
					{
						case 0 :	i3Color::SetR( &textColor, (UINT8) _wtoi( conv));	break;
						case 1 :	i3Color::SetG( &textColor, (UINT8) _wtoi( conv));	break;
						case 2 :	i3Color::SetB( &textColor, (UINT8) _wtoi( conv));	break;
						//case 3 :	i3Color::SetA( &textColor, (UINT8) _wtoi( conv));	break;
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
			i3Color::SetA( &textColor, (UINT8) _wtoi( conv));
			m_ShadowColor = (m_ShadowColor & 0x00FFFFFF) | (_wtoi( conv)<<24);
		}

		// 새로운 Text Color적용
		pCtx->m_CurTextColor = textColor.GetRGBA();

		return true;
	}
	else if( i3String::NCompareW( szCmd, L"/col", 4) == 0)
	{
		pCtx->m_CurTextColor = m_DefColor;
		m_ShadowColor = (m_ShadowColor & 0x00FFFFFF) | (m_DefColor & 0xFF000000);

		return true;
	}

	// 명령어 아님.
	return false;
}

void i3UIText::_TextAlignX( void)
{
	i3UILetter* pFirst = m_pText;
	i3UILetter* pLast = nullptr;
	
	i3UILetter* pTemp = pFirst;

	while( pFirst != nullptr)
	{
		pLast = pTemp;
		pTemp = pFirst->getNext();
		
		while( pTemp != nullptr && pTemp->getPosY() == pFirst->getPosY())
		{
			pLast = pTemp;
			pTemp = pTemp->getNext();
		}

		I3ASSERT( pLast != nullptr);	

		INT32 nLeft = pFirst->getPosX();
		INT32 nRight = pLast->getPosX() + pLast->getGlyph()->getWidth();
		// code == 0인 glyph의 width가 0이기 때문에
		// Italic font인 경우에는 마지막 glyph의 바로 앞의 glyph의 width를 고려해주어야 합니다.
		if( pLast->getGlyph()->getCode() == 0)
		{
			i3UILetter* pPrev = pLast->getPrev();
			if( pPrev != nullptr)
			{
				i3UIGlyph* pGlyph = pPrev->getGlyph();
				INT32 w = pGlyph->getWidth();
				INT32 b = pGlyph->getBaselineOffset();

				if( w != b)
					nRight += w - b;
			}
		}
		INT32 cx = nRight - nLeft;

		INT32 move = 0;
		
		switch( m_AlignX)
		{
		case I3UI_TEXT_ALIGN_X_CENTER:		move = (m_Box.getWidth() - cx) / 2;		break;
		case I3UI_TEXT_ALIGN_X_RIGHT:		move = m_Box.getWidth() - cx;			break;
		default:							break;
		}

		for( pTemp = pFirst; pTemp != nullptr && pTemp->getPrev() != pLast; pTemp = pTemp->getNext())
		{
			INT32 x = pTemp->getPosX() + move;
			INT32 y = pTemp->getPosY();
			
			pTemp->setPos( (INT16)x, (INT16)y);	// warning C4244
			
			if( pTemp->getSprite() != -1)
			{
				g_pUIRenderer->setPos( pTemp->getSprite(), (REAL32) (m_Box.left + x), (REAL32) (m_Box.top + y), 0.0f);
			}
		}

		pFirst = pLast->getNext();
	}
}

bool i3UIText::FindRectByIndex( INT32 idx, i3Rect* pRt)
{

	if( isAutoLineFeed())
	{
		i3UILetter* pLetter = m_pText;

		while( pLetter != nullptr)
		{
			if( pLetter->getIndex() == idx)
			{
				INT32 x = pLetter->getPosX();
				INT32 y = pLetter->getPosY();

				i3UIGlyph* pGlyph = pLetter->getGlyph();
				INT32 w = pGlyph->getWidth();
				INT32 h = pGlyph->getHeight();

				pRt->set( x, y, x+w, y+h);
				return true;
			}
			pLetter = pLetter->getNext();
		}

		return false;
	}
	else
	{
		WCHAR16* pText = m_pTextBuf;
		WCHAR16	 outbuf[8];
		INT32 i=0;

		if( pText == nullptr)
			return false;

		INT32		len		= _ExtractMultibyteFromText( &pText[i], outbuf, 8);
		i3UIGlyph * pGlyph	= m_pFont->FindGlyph( &pText[i], len);

		if( pGlyph == nullptr)
			return false;

		INT32 x = -m_ShowX;
		INT32 y = -m_ShowY;

		//i3UIGlyph* pTemp = m_pFont->FindGlyph( ' ');		// warning C4189
		//INT32 nSpaceWidth = pTemp->getWidth();			// warning C4189
		
		while( pGlyph != nullptr)
		{
			if( i > idx)
				break;

			pRt->left = x;
			pRt->top = y;
			pRt->right = pRt->left + pGlyph->getWidth();
			pRt->bottom = pRt->top + pGlyph->getHeight();

			switch( pText[i])
			{
			case '\r':
				break;
			case '\n':
				x = -m_ShowX;
				y += m_defFontHeight;
				break;
			case '\t' :
				x = ((m_ShowX + x + m_defTabWidth) / m_defTabWidth) * m_defTabWidth;
				break;
			default:
				x += pGlyph->getWidth();
				break;
			}

			len		= _ExtractMultibyteFromText( &pText[i], outbuf, 8);
			pGlyph  = m_pFont->FindGlyph( &pText[i], len);

			i += len;
		}

		if( i <= idx )	
			return false;

		return true;
	}
}

void i3UIText::Reparse(void)
{
	if( g_pTextManager == nullptr)
		return;

	if( m_pTextBuf != nullptr)
	{
		WCHAR16 tempc[4] = { 0x20,};
		g_pTextManager->AddGlyph( m_pFont, tempc, 1);
		Parse( m_pTextBuf);
	}
}

void i3UIText::SetText( const WCHAR16 * pText, bool bCopy)
{
	INT32 len = i3::generic_string_size( pText);

	if( bCopy)
	{
		if( m_pTextBuf == nullptr || m_bUseBuffer == false )		// 버퍼할당이 안된 옵션에서, 할당옵션 쪽으로 다시 넣는다면 realloc 곤란하므로 조건 추가...(11.07.01.수빈)
		{

			m_pTextBuf = (WCHAR16 *) i3MemAlloc( (len << 1) + 2);
			m_pTextBuf[0] = 0;		// 수정...  문자열비교를 하려면..일단 뭐가 되든 0값은 들어가야됨....(11.07.01.수빈)
		}
		else
		{
			if( len > m_MaxLength)
			{
				m_pTextBuf = (WCHAR16 *) i3MemReAlloc( m_pTextBuf, (len << 1) + 2);
			}
		}
		
		m_bUseBuffer = true;
		//addStyle( I3UI_TEXT_STYLE_OWN_BUFFER);		// 원래 CompareW 뒤에 있던걸 옮겼음...CompareW뒤에 놓으면,...운나쁘면...이 플래그를 못얻게 될수도 있습니다..(메모리릭이 실제로 발생.11.07.01.수빈.)
		I3ASSERT( m_pTextBuf != nullptr);

		if( i3String::CompareW( m_pTextBuf, pText) == 0)
			return;

		i3::safe_string_copy( m_pTextBuf, pText);
	}
	else
	{
		if ( m_pTextBuf &&  m_bUseBuffer == true  )					// 추가..
			i3MemFree(m_pTextBuf);		// 확인도 안하고, 대입부터 하면 메모리 손실 나요.....(2011.07.01.수빈) .. 

		m_pTextBuf = (WCHAR16 *) pText;
		m_bUseBuffer = false;
//		removeStyle( I3UI_TEXT_STYLE_OWN_BUFFER);
	}

	if( len > m_MaxLength)
		m_MaxLength = len;

	Parse( m_pTextBuf);
}

void	i3UIText::SetText( const WCHAR16 * pText)
{
	i3UIText::SetText(pText, isStyle( I3UI_TEXT_STYLE_OWN_BUFFER ) );
}


void i3UIText::SetView( INT32 x, INT32 y, INT32 w, INT32 h)
{
	m_Box.set( x, y, x + w, y + h);

	if( m_pTextBuf != nullptr)
	{
		Parse( m_pTextBuf);
	}
}

bool i3UIText::FindLetterByIndex( INT32 idx, i3UILetter ** ppLetter)
{
	i3UILetter * pTemp = m_pText;
	i3UILetter * pPrev = nullptr;

	while( (pTemp != nullptr) && (pTemp->getIndex() < idx))
	{
		pPrev = pTemp;
		pTemp = pTemp->getNext();
	}

	//I3TRACE("pTemp->getIndex():%d, idx:%d\n", pTemp->getIndex(), idx);

	if(( pTemp == nullptr) || (pTemp->getIndex() != idx))
	{
		// 원하는 Letter를 검색하지 못했고, 이전 Letter를 반환한다.
		*ppLetter = pPrev;
		return false;
	}

	*ppLetter = pTemp;

	return true;
}

bool i3UIText::FindWholeLetterByIndex( INT32 idx, i3UILetter ** ppLetter)
{
	bool result = FindLetterByIndex( idx, ppLetter);
	i3UILetter* pLetter = *ppLetter;

	if( pLetter != nullptr && pLetter->isClipped())		// 잘린 경우
		return false;

	return result;
}

void i3UIText::BeginEdit(void)
{
	if( m_hCursorSprite == -1)
	{
		m_hCursorSprite = g_pUIRenderer->AllocSprite( 0, this);
		I3ASSERT( m_hCursorSprite != -1);
	}

	if( m_hCursorSprite != -1)
	{
		g_pUIRenderer->setEnable(m_hCursorSprite, true);
		//I3TRACE( "BeginEdit\n");
	}

	m_timeCursor = 0.0f;
	m_codeCursor = 0;

	SetCursorPos( m_idxCursor);
}

void i3UIText::EndEdit(void)
{
	if( m_hCursorSprite != -1)
	{
		g_pUIRenderer->FreeSprite( m_hCursorSprite);
		m_hCursorSprite = -1;

		I3TRACE( "EndEdit\n");
	}
}

bool i3UIText::isEditing(void)
{
	return (m_hCursorSprite != -1);
}

// idx 는 String Buffer에서의 idx
INT32 i3UIText::SetCursorPos( INT32 idx)
{
	REAL32 x, y;			// Caret Pos
	REAL32 cx, cy, u, v;	// Caret Size and Texture Coordinate 
	i3UILetter * pLetter;	
	bool isCorrectLetter;

	m_idxCursor = idx;		

	if( m_hCursorSprite == -1)
		return -1;

	isCorrectLetter = FindLetterByIndex( idx, &pLetter);
	
	if( m_codeCursor != 0)		// IME. 글자 조합중
	{
		I3ASSERT( m_pGlyph_Cursor != nullptr);

		cx = (REAL32) m_pGlyph_Cursor->getWidth();
		cy = (REAL32) m_pGlyph_Cursor->getHeight();
		u = (REAL32) m_pGlyph_Cursor->getPosX();
		v = (REAL32) m_pGlyph_Cursor->getPosY();
	}
	else		// 기본 그냥 가느다란 선으로 표현
	{
		cx = 2.0f;			
		cy = (REAL32) m_defFontHeight;
		u = 0.0f;
		v = 0.0f;
	}

	if( pLetter == nullptr)	// m_pText가 nullptr인 경우.
	{
		x = (REAL32) m_Box.left;
		y = (REAL32) m_Box.top;
	}
	else
	{
		if( isCorrectLetter)
		{
			x = (REAL32) (m_Box.left + pLetter->getPosX());
			y = (REAL32) (m_Box.top + pLetter->getPosY());
		}
		else
		{
			i3UIGlyph * pGlyph = pLetter->getGlyph();

			x = (REAL32) (m_Box.left + pLetter->getPosX() + pGlyph->getWidth());
			y = (REAL32) (m_Box.top + pLetter->getPosY());
		}
	}

	g_pUIRenderer->setPos( m_hCursorSprite, x, y, 0.0f);
	g_pUIRenderer->setSizeAndRotate( m_hCursorSprite, cx, cy, 0.0f);

	REAL32 x1, y1, x2, y2;

	x1 = u;
	y1 = v;
	x2 = x1 + cx;
	y2 = y1 + cy;

	g_pUIRenderer->setTexCoord( m_hCursorSprite, x1, y1, x2, y2);
	g_pUIRenderer->setTexRange( m_hCursorSprite, -1.0f, 1.0f, 1.0f, 0.0f);

	if( pLetter == nullptr)
		return 0;

	return pLetter->getIndex();
}

void i3UIText::SetCursorCode( WCHAR16 code)
{
	m_codeCursor = code;

	WCHAR16 tempc[4] = { code,};

	m_pGlyph_Cursor = g_pTextManager->AddGlyph( m_pFont, tempc, 1);
		
	SetCursorPos( m_idxCursor);

	I3ASSERT( m_pFont != nullptr);
}

void i3UIText::SetCaretAlpha( REAL32 alpha)
{
	if( m_hCursorSprite != -1)
	{
		COLOR color;
		i3Color::ConvertRGBA32( g_pUIRenderer->getColor( m_hCursorSprite), &color);

		color.a = (UINT8)(MINMAX( 0.f, alpha, 1.f) * 255.f);
		g_pUIRenderer->setColor( m_hCursorSprite, &color);
	}
}

INT32 i3UIText::FindCharByPos( INT32 x, INT32 y)
{
	i3UILetter * pTemp = m_pText;

	while( pTemp != nullptr)
	{
		INT32 x1, y1, x2, y2;

		y1 = pTemp->getPosY();
		y2 = y1 + pTemp->getGlyph()->getHeight();

		if( (y >= y1) && (y < y2))	// y < y2
		{
			x1 = pTemp->getPosX();
			x2 = x1 + pTemp->getGlyph()->getWidth();

			if( (x >= x1) && (x <= x2))		// '\n'..  
				return pTemp->getIndex();
		}

		pTemp = pTemp->getNext();
	}

	return -1;
}

INT32 i3UIText::FindNearestCharByPos( INT32 x, INT32 y, INT32 bgHeight)
{
	i3UILetter* pTemp = m_pText;

	i3UILetter* pLeftmost = nullptr;
	i3UILetter* pRightmost = nullptr;

	while( pTemp != nullptr)
	{
		INT32 y1 = pTemp->getPosY();
		INT32 y2 = y1;
		
		if( bgHeight > 0)
		{
			y2 += bgHeight;
		}
		else
		{
			y2 += pTemp->getGlyph()->getHeight();
		}

		if( (y >= y1) && (y < y2))	// y < y2
		{
			if( pLeftmost == nullptr)
			{
				pLeftmost = pTemp;
				pRightmost = pTemp;
			}
			else
			{
				pRightmost = pTemp;
			}
		}
		else if( y < y1)
			break;

		pTemp = pTemp->getNext();
	}

	if( pLeftmost != nullptr && pRightmost != nullptr)	// y와 같은 줄에 Letter가 존재
	{
		if( x < pLeftmost->getPosX())
			return pLeftmost->getIndex();	// 그 줄 가장 왼쪽에 있는 Letter
		else if( x > pRightmost->getPosX() + pRightmost->getGlyph()->getWidth())
			return pRightmost->getIndex();	// 그 줄 가장 오른쪽에 있는 Letter
		else
		{
			// 거꾸로 검색( nullptr Letter)
			for( i3UILetter* pTemp = pRightmost; pTemp != pLeftmost; pTemp = pTemp->getPrev())
			{
				INT32 x1 = pTemp->getPosX();
				INT32 x2 = x1 + pTemp->getGlyph()->getWidth();

				if( (x >= x1) && (x <= x2))		// '\n'..  
					return pTemp->getIndex();	
			}
			return pLeftmost->getIndex();
		}
	}

	return -1;	// y와 동일선상에 아무 letter도 없는 경우
}

INT32 i3UIText::SetShowPos( INT32 x, INT32 y)
{
	//I3TRACE("i3UIText::SetShowPos: %d, %d\n", x, y);
	m_ShowX = x;
	m_ShowY = y;

	Reparse();

	return 0;
}

void i3UIText::SetShadowType( I3UI_TEXT_SHADOW shadow)
{
	m_ShadowType = shadow;

	i3UILetter * pTemp = m_pText;

	while( pTemp != nullptr)
	{
		if( pTemp->getSprite() != nullptr && g_pUIRenderer != nullptr)
		{
			VEC4D * pV;

			pV = g_pUIRenderer->getTexRange( pTemp->getSprite());
			g_pUIRenderer->setTexRange( pTemp->getSprite(), pV->x, pV->y, pV->z, (REAL32) shadow);
		}

		pTemp = pTemp->getNext();
	}
}

void i3UIText::SetBlock( INT32 idxStart, INT32 idxEnd)
{
	// WCHAR16
	m_idxBeginBlock = idxStart;
	m_idxEndBlock = idxEnd;
}

INT32 i3UIText::AdjustSpriteOrder( INT32 hSpriteEnd)
{
	/*
	if( g_pUIRenderer == nullptr)
		return -1;

	i3UILetter * pLetter = m_pText;
	INT32 idxEnd = hSpriteEnd, idxSprite;

	while( pLetter != nullptr)
	{
		idxSprite = pLetter->getSprite();

		g_pUIRenderer->AdjustOrder( idxEnd, & idxSprite, 1);

		idxEnd = idxSprite;

		pLetter = pLetter->getNext();
	}

	return idxEnd;
	*/

	return 0;
}

void i3UIText::Dump(void)
{
	i3UILetter * pLetter = m_pText;
	INT32 i = 0;

	I3TRACE( "-------------- Dump ----------------\n");

	while( pLetter != nullptr)
	{
		char conv[256];
		char temp[4];

		i3UIGlyph * pGlyph = pLetter->getGlyph();

		if( pGlyph != nullptr)
		{
			WCHAR16 * code = pGlyph->getCode();
			i3String::CopyWCHARtoASCII( temp, sizeof( temp), code, pGlyph->getLength());
			i3::snprintf( conv, sizeof(conv), "[%d] : %c (%d)\n", i, temp, pLetter->getIndex());
		}
		else
			i3::snprintf( conv, sizeof(conv), "[%d] : <%x> (%d)\n", i, pLetter, pLetter->getIndex());

		i++;
		pLetter = pLetter->getNext();

		OutputDebugString( conv);
	}

	I3TRACE( "-------------- End of Dump ----------------\n");
}

void i3UIText::AdjustLetters( void)
{
	i3UILetter* pLetter = m_pText;

	while( pLetter != nullptr)
	{
		INT32 x = m_Box.left + pLetter->getPosX();
		INT32 y = m_Box.top + pLetter->getPosY();

		i3UIGlyph* pGlyph = pLetter->getGlyph();
		INT32 w = pGlyph->getWidth();
		INT32 h = pGlyph->getHeight();

		i3Rect rcLetter;
		rcLetter.set( x, y, x+w, y+h);

		i3Rect intersect;
		TOUCH_TYPE touch = m_Box.Intersect( &rcLetter, &intersect);

		INT32 hSprite = pLetter->getSprite();

		if( touch != TOUCH_NONE)
		{
			g_pUIRenderer->setPos( hSprite, (REAL32)(intersect.left), (REAL32)(intersect.top), 0.0f);
			g_pUIRenderer->setSizeAndRotate( hSprite, (REAL32) intersect.getWidth(), (REAL32) intersect.getHeight(), 0.0f);

			REAL32 x1 = (REAL32) (pGlyph->getPosX() + (intersect.left - x));
			REAL32 y1 = (REAL32) (pGlyph->getPosY() + (intersect.top - y));
			REAL32 x2 = x1 + intersect.getWidth();
			REAL32 y2 = y1 + intersect.getHeight();
			g_pUIRenderer->setTexCoord( hSprite, x1, y1, x2, y2);
		}
		else
		{
			g_pUIRenderer->setSizeAndRotate( hSprite, 0.0f, 0.0f, 0.0f);
			//g_pTextManager->FreeLetter( pLetter); ??
		}

		pLetter = pLetter->getNext();
	}
}

void i3UIText::SetSpaceofLetters( INT32 nX, INT32 nY)
{
	m_nSpaceofLettersX = nX;
	m_nSpaceofLettersY = nY;

	AdjustLetters();
}