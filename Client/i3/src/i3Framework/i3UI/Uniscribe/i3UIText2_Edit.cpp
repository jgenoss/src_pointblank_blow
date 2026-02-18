#include "i3FrameworkPCH.h"
#include "Uniscribe/i3UIText2.h"
#include "../i3Framework.h"

#define DEAFULT_PAGE_STEP 4

void i3UIText::SetCaretCode( INT32 nLetterIdx, int nendindex)
{
	m_wcCodeCaret = 0;
	
	if( nLetterIdx > -1)
	{
		i3UILetter* pLetter = nullptr;
		FindLetterByIndex( nLetterIdx, &pLetter);
		
		if( pLetter != nullptr && pLetter->getSpriteCount() > 0)
		{
			m_wcCodeCaret = pLetter->getGlyph( 0)->getGlyphCode();
		}
	}

	m_pGlyphCaret = g_pTextManager->AddGlyph( m_pFont, m_wcCodeCaret, nullptr);

	//I3TRACE( "SetCaretCode: %d, %wc\n", nLetterIdx, m_wcCodeCaret);
		
	UpdateCaretPos(nLetterIdx, nendindex);

	I3ASSERT( m_pFont != nullptr);
}

void i3UIText::SetBlock( INT32 idxStart, INT32 idxEnd)
{
	m_nIdxBeginBlock = idxStart;
	m_nIdxEndBlock = idxEnd;
}

void i3UIText::SetCaretPos( INT32 idx)
{
	//idx가 CharStop이 아닐수도 있다.
	
	i3UILetter* pCur = m_pFirstLetter;
	
	i3UILetter* pPrev = nullptr;
	INT32 nMin = -1;
	
	while( pCur != nullptr)
	{
		INT32 curIdx = pCur->getIndex();
		
		if( curIdx == idx)
		{
			m_nCurIdx = idx;
			m_pCurLetter = pCur;
			break;
		}
		
		if( curIdx < idx && curIdx > nMin)
		{
			nMin = curIdx;
			pPrev = pCur;
		}
		
		pCur = pCur->getNext();
	}
	
	if( pCur == nullptr)
	{
		if( pPrev != nullptr)
		{
			m_nCurIdx = nMin;
			m_pCurLetter = 	pPrev;
		}
	}

	//I3TRACE( "SetCaretPos: %d, %d\n", idx, m_nCurIdx);

	UpdateCaretPos(idx);
}

void i3UIText::UpdateCaretPos(INT32 nLetterIdx, INT32 nSpriteIndex)
{
	if (m_hSpriteCaret == -1)
		return;

	REAL32 x, y;			// Caret Pos
	REAL32 cx, cy, u, v;	// Caret Size and Texture Coordinate 

	i3UILetter* pLetter = nullptr;
	REAL32 x1, y1, x2, y2;
	x1 = y1 = x2 = y2 = 0;

	if (m_wcCodeCaret != 0)		// IME. 글자 조합중
	{
		if (i3UI::getGlobalIme()->getInputLanguage() == LANG_JAPANESE)
		{
			FindLetterByIndex(nLetterIdx, &pLetter);

			if (pLetter)
			{
				if (pLetter != nullptr && pLetter->getSpriteCount() > 0)
				{
					m_wcCodeCaret = pLetter->getGlyph(0)->getGlyphCode();
				}

				m_pGlyphCaret = g_pTextManager->AddGlyph(m_pFont, m_wcCodeCaret, nullptr);

				I3ASSERT(m_pGlyphCaret != nullptr);

				cx = (REAL32)pLetter->getGlyphWidth(0) - 2;	//상자크기
				cy = (REAL32)pLetter->getHeight();
				//cx = (REAL32) m_pGlyphCaret->getWidth()-2;	//상자크기
				//cy = (REAL32) m_pGlyphCaret->getHeight();
				u = (REAL32)m_pGlyphCaret->getPosX();
				v = (REAL32)m_pGlyphCaret->getPosY();

				x1 = (REAL32)pLetter->getGlyph(0)->getPosX() + pLetter->getGlyphOffsetX(0);
				y1 = (REAL32)pLetter->getGlyph(0)->getPosY();
				x2 = x1 + pLetter->getGlyph(0)->getWidth();

				REAL32 scaleH = (REAL32)pLetter->getGlyph(0)->getHeight() / (REAL32)pLetter->getGlyph(0)->getRealHeight();
				y2 = y1 + (pLetter->getGlyph(0)->getHeight() - scaleH);	//지금 글리프 사이즈가 레터사이즈보다 2큰가? 
			}
			else
			{
				cx = (REAL32)m_pGlyphCaret->getWidth();
				cy = (REAL32)m_pGlyphCaret->getHeight();
				u = (REAL32)m_pGlyphCaret->getPosX();
				v = (REAL32)m_pGlyphCaret->getPosY();
			}
		}
		else
		{
			I3ASSERT(m_pGlyphCaret != nullptr);

			cx = (REAL32)m_pGlyphCaret->getWidth();
			cy = (REAL32)m_pGlyphCaret->getHeight();
			u = (REAL32)m_pGlyphCaret->getPosX();
			v = (REAL32)m_pGlyphCaret->getPosY();
		}
	}
	else		// 기본 그냥 가느다란 선으로 표현
	{
		cx = 2.0f;
		cy = (REAL32)m_nDefFontHeight;
		u = 0.0f;
		v = 0.0f;
	}

	if (m_pCurLetter == nullptr)	// m_pText가 nullptr인 경우.
	{
		x = (REAL32)m_rtView.left;
		y = (REAL32)m_rtView.top;
	}
	else
	{
		if (i3UI::getGlobalIme()->getInputLanguage() == LANG_JAPANESE)
		{
			if (nSpriteIndex > 0)
			{
				if (pLetter)
				{
					x = (REAL32)(m_rtView.left + pLetter->getPosX());
					y = (REAL32)(m_rtView.top + pLetter->getPosY());
				}
				else
				{
					x = (REAL32)(m_rtView.left + m_pCurLetter->getPosX());
					y = (REAL32)(m_rtView.top + m_pCurLetter->getPosY());
				}
			}
			else
			{
				x = (REAL32)(m_rtView.left + m_pCurLetter->getPosX());
				y = (REAL32)(m_rtView.top + m_pCurLetter->getPosY());

				if (m_pCurLetter->isRightCaret())
				{
					x += m_pCurLetter->getWidth();
				}
			}
		}
		else
		{
			x = (REAL32)(m_rtView.left + m_pCurLetter->getPosX());
			y = (REAL32)(m_rtView.top + m_pCurLetter->getPosY());

			if (m_pCurLetter->isRightCaret())
			{
				x += m_pCurLetter->getWidth();
			}
		}
	}

	i3Rect rtIntersect;
	bool bVisible = true;

	{
		i3Rect rtBox;
		rtBox.set(0, 0, m_rtView.getWidth() - 1, m_rtView.getHeight() - 1);		//m_rtText
		rtBox.Offset(m_ShowX, m_ShowY);

		i3Rect rtCaret;
		rtCaret.set((INT32)(x - m_rtView.left), (INT32)(y - m_rtView.top), (INT32)(x - m_rtView.left + cx - 1), (INT32)(y - m_rtView.top + cy - 1));


		TOUCH_TYPE touch = rtBox.Intersect(&rtCaret, &rtIntersect);

		if (touch == TOUCH_NONE)
			bVisible = false;
	}

	if (i3UI::getGlobalIme()->getInputLanguage() == LANG_JAPANESE)
	{
		g_pUIRenderer->setEnableSprite(m_hSpriteCompBox[nSpriteIndex], bVisible);
	}
	else
	{
		g_pUIRenderer->setEnableSprite(m_hSpriteCaret, bVisible);
	}

	if (bVisible)
	{
		if (i3UI::getGlobalIme()->getInputLanguage() == LANG_JAPANESE && m_wcCodeCaret != 0)
		{
			g_pUIRenderer->setPos(m_hSpriteCompBox[nSpriteIndex], (REAL32)(rtIntersect.left + m_rtView.left - m_ShowX), (REAL32)(rtIntersect.top + m_rtView.top - m_ShowY), 0.f);

			g_pUIRenderer->setSizeAndRotate(m_hSpriteCompBox[nSpriteIndex], (REAL32)cx/*-1.0f*/, (REAL32)cy/*-1.0f*/, 0.0f);

			g_pUIRenderer->setTexCoord(m_hSpriteCompBox[nSpriteIndex], x1, y1, x2, y2);
			//g_pUIRenderer->setTexCoord( m_hSpriteCompBox[nSpriteIndex], u, v, u + cx, v + cy);
			g_pUIRenderer->setTexRange(m_hSpriteCompBox[nSpriteIndex], -1.0f, 1.0f, 0, 0.0f);
		}
		else
		{
			g_pUIRenderer->setPos(m_hSpriteCaret, (REAL32)(rtIntersect.left + m_rtView.left - m_ShowX), (REAL32)(rtIntersect.top + m_rtView.top - m_ShowY), 0.f);

			if (m_wcCodeCaret != 0)	// 조합중일땐 intersect 사각형대로
				g_pUIRenderer->setSizeAndRotate(m_hSpriteCaret, (REAL32)rtIntersect.getWidth()/*-1.0f*/, (REAL32)rtIntersect.getHeight()/*-1.0f*/, 0.0f);
			else						// 일반적으로는 캐럿 선두께 유지한채로 
				g_pUIRenderer->setSizeAndRotate(m_hSpriteCaret, cx, (REAL32)rtIntersect.getHeight()/*-1.0f*/, 0.0f);

			g_pUIRenderer->setTexCoord(m_hSpriteCaret, u, v, u + cx, v + cy);
			g_pUIRenderer->setTexRange(m_hSpriteCaret, -1.0f, 1.0f, 0.7f, 0.0f);
		}
	}
}

void i3UIText::SetCaretAlpha( REAL32 alpha)
{
	if( m_hSpriteCaret != -1)
	{
		COLOR color;
		i3UILetter* pLast = GetLastLetter();
		if( pLast != nullptr)
			i3Color::ConvertRGBA32( pLast->getColor(), &color);
		else
			i3Color::ConvertRGBA32( g_pUIRenderer->getColor( m_hSpriteCaret), &color);
		
		color.a = (UINT8)(MINMAX( 0.f, alpha, 1.f) * 255.f);

//		DWORD dw_caret_color = getFontColor(getCaretPos());
		/*
		DWORD dw_caret_color = m_DefColor;
		COLOR caret_color;	
		caret_color.SetRGBA(dw_caret_color);
		color.r = caret_color.r;
		color.g = caret_color.g;
		color.b = caret_color.b;
		*/
//
		if (m_wcCodeCaret != 0)
		{
			for (int i = 0; i < MAXCOMPBOX; i++)
			{
				g_pUIRenderer->setColor(m_hSpriteCompBox[i], &color);
			}
		}
		else
			g_pUIRenderer->setColor( m_hSpriteCaret, &color);
	}
}

void i3UIText::CompBoxDisable()
{
	for (int i = 0; i < MAXCOMPBOX; i++)
		g_pUIRenderer->setEnableSprite(m_hSpriteCompBox[i], false);
}

void i3UIText::BeginEdit( void)
{
	//EditBox인 경우 Escape Sequence 처리를 하지 않는다.
	//EnableEscapeSequence( false);

	if( m_hSpriteCaret == -1)
	{
		m_hSpriteCaret = g_pUIRenderer->AllocSprite( 0, this);
		I3ASSERT( m_hSpriteCaret != -1);

		for (int i = 0; i < MAXCOMPBOX; i++)
			m_hSpriteCompBox[i] = g_pUIRenderer->AllocSprite(0, this);
	}

	if( m_hSpriteCaret != -1)
	{
		g_pUIRenderer->setEnableSprite( m_hSpriteCaret, true);
		//I3TRACE( "BeginEdit\n");
		for (int i = 0; i < MAXCOMPBOX; i++)
			g_pUIRenderer->setEnableSprite(m_hSpriteCompBox[i], false);
	}

	m_wcCodeCaret = 0;

	UpdateCaretPos();
}

void i3UIText::EndEdit( void)
{
	if( m_hSpriteCaret != -1)
	{
		g_pUIRenderer->FreeSprite( m_hSpriteCaret);
		m_hSpriteCaret = -1;

		for (int i = 0; i < MAXCOMPBOX; i++)
		{
			g_pUIRenderer->FreeSprite(m_hSpriteCompBox[i]);

			m_hSpriteCompBox[i] = -1;
		}

		//I3TRACE( "EndEdit\n");
	}
}

bool i3UIText::isEditing(void)
{
	return (m_hSpriteCaret != -1);	
}

bool i3UIText::isLetterVisible( i3UILetter* pLetter)
{
	i3Rect rtBox;
	rtBox.set( 0, 0, m_rtView.getWidth()-1, m_rtView.getHeight()-1);		//m_rtText
	rtBox.Offset( m_ShowX, m_ShowY);
	
	i3Rect rtLetter;
	rtLetter.set( pLetter->getPosX(), pLetter->getPosY(), pLetter->getPosX() + pLetter->getWidth(), pLetter->getPosY() + pLetter->getHeight());

	i3Rect rtIntersect;
	TOUCH_TYPE touch = rtBox.Intersect( &rtLetter, &rtIntersect);

	if( touch == TOUCH_WHOLE)
		return true;

	return false;
}

void i3UIText::ScrollToCaret( void)
{
	i3UILetter* pLetter = m_pCurLetter;

	if( pLetter == nullptr)
		return;

	if( isLetterVisible( pLetter))
	{
		UpdateCaretPos();
		return;
	}
	
	i3Rect rtBox;
	rtBox.set( 0, 0, m_rtView.getWidth()-1, m_rtView.getHeight()-1);		//m_rtText

	//X check
	while( pLetter->getPosX() < m_ShowX)
	{
		m_ShowX--;
	}
	
	while( pLetter->getPosX() + pLetter->getWidth() >= m_ShowX + rtBox.getWidth())
	{
		m_ShowX++;
	}

	if( isLetterVisible( pLetter))
	{
		UpdateCaretPos();
		return;
	}

	//Y check
	while( pLetter->getPosY() < m_ShowY)
	{
		m_ShowY--;
	}
		
	while( pLetter->getPosY() + pLetter->getHeight() >= m_ShowY + rtBox.getHeight())
	{
		m_ShowY++;
	}

	if( isLetterVisible( pLetter))
	{
		UpdateCaretPos();
		return;
	}

	//I3ASSERT( 0);		//Scroll이 제대로 되지 않았음. 또는 스크롤을 해도 다 안보임
}

INT32 i3UIText::FindNearestLetterByPos( INT32 x, INT32 y, INT32 bgHeight)
{
	x += m_ShowX;
	y += m_ShowY;

	INT32 idx = -1;

	i3UILetter* pCur = m_pFirstLetter;
	i3UILetter* pPrev = nullptr;

	INT32 nLine = 0;

	//decide line(y)
	while( pCur != nullptr)
	{
		if( y < pCur->getPosY())
			break;

		nLine = pCur->getLine();
		//pPrev = pCur;
		pCur = pCur->getNext();
	}

	//decide x
	pCur = m_pFirstLetter;
	pPrev = nullptr;
	
	while( pCur != nullptr)
	{
		if( pCur->getLine() > nLine)
		{
			if (pPrev)
			{
				idx = pPrev->getIndex();
			}
			else
			{
				I3ASSERT(pPrev != nullptr);
			}
			break;
		}

		if( pCur->getLine() == nLine)
		{
			if( x < pCur->getPosX())
			{
				if( pPrev == nullptr || pPrev->getLine() != nLine)
					idx = pCur->getIndex();
				else
					idx = pPrev->getIndex();
				break;
			}
		}
					
		pPrev = pCur;
		pCur = pCur->getNext();
	}

	if( pCur == nullptr && pPrev != nullptr)
		idx = pPrev->getIndex();
	
	return idx;	// y와 동일선상에 아무 letter도 없는 경우
}

bool i3UIText::FindLetterByIndex( INT32 idx, i3UILetter ** ppLetter)
{
	i3UILetter * pTemp = m_pFirstLetter;
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

i3UILetter* i3UIText::GetLastLetter( void)
{
	INT32 max = -1;
	
	i3UILetter* pCur = m_pFirstLetter;
	i3UILetter* pLast = pCur;
	
	while( pCur != nullptr)
	{
		if( pCur->getIndex() > max)
		{
			max = pCur->getIndex();
			pLast = pCur;
		}

		pCur = pCur->getNext();
	}

	return pLast;
}

//OnKeyDownLeft와 OnKeyDownRight는
//위치상으로 옆에 있는 Character를 찾는 것이 아니라
//index상으로 옆에 있는 Character를 찾아야한다. 
//어디에 놓여있을지 알 수 없음
INT32 i3UIText::OnKeyDownLeft( void)
{
	//m_nCurIdx보다 작은 값 중 가장 큰 값을 가진 Letter를 찾는다. 
	if( m_pCurLetter != nullptr)
	{
		INT32 max = -1;
		
		i3UILetter* pLeft = m_pCurLetter;
		INT32 idx = m_nCurIdx;
		
		i3UILetter* pLetter = m_pFirstLetter;
		while( pLetter != nullptr)
		{
			idx = pLetter->getIndex();

			if( idx < m_nCurIdx && idx > max)
			{
				max = idx;
				pLeft = pLetter;
			}
			
			pLetter = pLetter->getNext();
		}

		m_pCurLetter = pLeft;
		m_nCurIdx = pLeft->getIndex();
	}

	//I3TRACE( "OnKeyDownLeft: %p, %d\n", m_pCurLetter, m_nCurIdx);

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownRight( void)
{
	//m_nCurIdx보다 큰 값 중 가장 작은 값을 가진 Letter를 찾는다.
	if( m_pCurLetter != nullptr)
	{
		INT32 min = 100000;
		
		i3UILetter* pRight = m_pCurLetter;
		INT32 idx = m_nCurIdx;
		
		i3UILetter* pLetter = m_pFirstLetter;
		while( pLetter != nullptr)
		{
			idx = pLetter->getIndex();

			if( idx > m_nCurIdx && idx < min)
			{
				min = idx;
				pRight = pLetter;
			}
			
			pLetter = pLetter->getNext();
		}

		m_pCurLetter = pRight;
		m_nCurIdx = pRight->getIndex();
	}

	//I3TRACE( "OnKeyDownRight: %p, %d\n", m_pCurLetter, m_nCurIdx);

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownHome( void)
{
	if( m_pCurLetter != nullptr)
	{
		INT32 nCurLine = m_pCurLetter->getLine();

		INT32 nMin = m_nCurIdx;
		i3UILetter* pMinLetter = m_pCurLetter;
		
		i3UILetter* pLeft = m_pCurLetter->getPrev();
		while( pLeft != nullptr)
		{
			if( pLeft->getLine() != nCurLine)
				break;
			
			if( pLeft->getIndex() < nMin)
			{
				nMin = pLeft->getIndex();
				pMinLetter = pLeft;
			}
			
			pLeft = pLeft->getPrev();
		}

		i3UILetter* pRight = m_pCurLetter->getNext();
		while( pRight != nullptr)
		{
			if( pRight->getLine() != nCurLine)
				break;

			if( pRight->getIndex() < nMin)
			{
				nMin = pRight->getIndex();
				pMinLetter = pRight;
			}
		
			pRight = pRight->getNext();
		}

		m_pCurLetter = pMinLetter;
		m_nCurIdx = nMin;
	}

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownEnd( void)
{
	if( m_pCurLetter != nullptr)
	{
		INT32 nCurLine = m_pCurLetter->getLine();

		INT32 nMax = m_nCurIdx;
		i3UILetter* pMaxLetter = m_pCurLetter;
		
		i3UILetter* pLeft = m_pCurLetter->getPrev();
		while( pLeft != nullptr)
		{
			if( pLeft->getLine() != nCurLine)
				break;

			if( pLeft->getIndex() > nMax)
			{
				nMax = pLeft->getIndex();
				pMaxLetter = pLeft;
			}
			
			pLeft = pLeft->getPrev();
		}

		i3UILetter* pRight = m_pCurLetter->getNext();
		while( pRight != nullptr)
		{
			if( pRight->getLine() != nCurLine)
				break;

			if( pRight->getIndex() > nMax)
			{
				nMax = pRight->getIndex();
				pMaxLetter = pRight;
			}
		
			pRight = pRight->getNext();
		}

		m_pCurLetter = pMaxLetter;
		m_nCurIdx = nMax;
	}

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownUp( void)
{
	if( m_pCurLetter != nullptr)
	{
		INT32 nLine = m_pCurLetter->getLine();
		INT32 x = m_pCurLetter->getPosX();		//정확히는 Cursor의 Position을 가져와야함

		if( nLine > 0)
		{
			i3UILetter* pTemp = m_pCurLetter->getPrev();

			while( pTemp != nullptr && pTemp->getLine() == nLine)
			{
				pTemp = pTemp->getPrev();
			}

			i3UILetter* pPrev = nullptr;
			while( pTemp != nullptr && pTemp->getPosX() >= x)	//정확히는 Letter의 Width도 고려해야 함
			{
				if( pTemp->getLine() != nLine-1)
				{
					pTemp = nullptr;
					break;
				}

				pPrev = pTemp;
				pTemp = pTemp->getPrev();
			}

			if( pTemp == nullptr)
				pTemp = pPrev;

			I3ASSERT_RETURN( pTemp != nullptr, m_nCurIdx);
			
			m_pCurLetter = pTemp;
			m_nCurIdx = pTemp->getIndex();
		}
	}

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownDown( void)
{
	if( m_pCurLetter != nullptr)
	{
		INT32 nLine = m_pCurLetter->getLine();
		INT32 x = m_pCurLetter->getPosX();		//정확히는 Cursor의 Position을 가져와야함

		if( nLine+1 < m_nMaxLine)
		{
			i3UILetter* pTemp = m_pCurLetter->getNext();

			while( pTemp != nullptr && pTemp->getLine() == nLine)
			{
				pTemp = pTemp->getNext();
			}

			i3UILetter* pPrev = nullptr;
			while( pTemp != nullptr && pTemp->getPosX() <= x)	//정확히는 Letter의 Width도 고려해야 함
			{
				if( pTemp->getLine() != nLine+1)
				{
					pTemp = nullptr;
					break;
				}

				pPrev = pTemp;
				pTemp = pTemp->getNext();
			}

			if( pTemp == nullptr)
				pTemp = pPrev;
			
			I3ASSERT_RETURN( pTemp != nullptr, m_nCurIdx);

			m_pCurLetter = pTemp;
			m_nCurIdx = pTemp->getIndex();
		}
	}

	return m_nCurIdx;
}

INT32 i3UIText::OnKeyDownPageUp( void)
{
	for( INT32 i=0; i< DEAFULT_PAGE_STEP-1; i++)		//Temp
	{
		OnKeyDownUp();
	}

	return OnKeyDownUp();
}

INT32 i3UIText::OnKeyDownPageDown( void)
{
	for( INT32 i=0; i< DEAFULT_PAGE_STEP-1; i++)		//Temp
	{
		OnKeyDownDown();
	}

	return OnKeyDownDown();
}
