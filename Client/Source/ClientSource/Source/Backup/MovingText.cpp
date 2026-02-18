#include "pch.h"

#if 지워질_것들

#include "MovingText.h"

I3_CLASS_INSTANCE(CMovingText);//, i3GameObjBase);

CMovingText::CMovingText()
{
	m_pSceneNode = i3Node::new_object();

	m_movingText._textNode = NULL;
	SET_POINT2D(m_movingText._startPoint, 0, 0);
	SET_POINT2D(m_movingText._endPoint, 0, 0);
	m_movingText._text[0] = '\0';

	SET_POINT2D(m_startPoint, 0, 0);
	SET_POINT2D(m_endPoint, 0, 0);

	INT32 i;
	for( i = 0;i < 8; ++i)
	{
		m_pTextNode[i] = NULL;
	}
}

CMovingText::~CMovingText()
{
	if( m_movingText._textNode != NULL)
	{
		m_movingText._textNode->RemoveFromParent();
	}

	INT32 i;
	for( i = 0;i < 8; ++i)
	{
		I3_SAFE_RELEASE( m_pTextNode[i]);
	}
}

BOOL CMovingText::Create(i3Node *pParentNode, BOOL bEnable)
{
	BOOL bResult = i3GameObjBase::Create(pParentNode, bEnable);

	INT32 i;

	//Create TextNode
	for( i = 0;i < 8; ++i)
	{
		m_pTextNode[i] = i3TextNodeDX2::new_object();

		INT32 fontSize = (INT32) (60 - (48 * i3Math::sin(I3_PI2 * (i * 1.0f/8.0f))));

		m_pTextNode[i]->Create( GetDefaultFontName(), 8, fontSize);

		m_pTextNode[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);						
		m_pTextNode[i]->SetTextSpace(0, 5);
		m_pTextNode[i]->setSize(200, 100);
	}

	if (!bResult)
	{
		return bResult;
	}

	return TRUE;
}

void CMovingText::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	if (m_movingText._textNode != NULL)
	{
		static REAL32 rMovingTime = 0.0f;

		POINT2D currentPoint;
		SET_POINT2D(currentPoint, m_movingText._textNode->getPosX(), m_movingText._textNode->getPosY());

		if (!EQUAL_POINT2D(m_movingText._endPoint, currentPoint))
		{
			m_movingText._textNode->SetEnable(TRUE);

			// 이동 패턴
			rMovingTime = MIN(rMovingTime + rDeltaSeconds, 0.5f);

			if (0.3f <= rMovingTime)
			{
				REAL32 rMoveRatio = 1.0f;

				if (rMovingTime != 0.5f)
				{
					rMoveRatio = (rMovingTime - 0.3f) / 0.2f;
				}

				INT32 moveX = m_movingText._endPoint.x - m_movingText._startPoint.x;
				INT32 moveY = m_movingText._endPoint.y - m_movingText._startPoint.y;

				if (rMoveRatio != 1.0f)
				{
 					moveX = (INT32) ((REAL32)moveX * i3Math::sin(I3_PI2 * rMoveRatio));
					moveY = (INT32) ((REAL32)moveY * i3Math::sin(I3_PI2 * rMoveRatio));
				}

				{
					INT32 fontIndex = (INT32)(rMoveRatio * 7.0f);

					if( m_movingText._textNode != NULL)
					{
						m_movingText._textNode->RemoveFromParent();
					}

					I3_BOUNDCHK(fontIndex, 8);
					i3TextNodeDX2 * pTextNode = m_pTextNode[fontIndex];
					
					pTextNode->SetColor(128, 217, 0, (UINT8) (255 - (77 - i3Math::sin(I3_PI2 * rMoveRatio))) );
					pTextNode->SetText(m_movingText._text);
					pTextNode->SetEnable(TRUE);

					GetNode()->AddChild(pTextNode);
						
					m_movingText._textNode = pTextNode;
					m_movingText._textNode->setPos(m_movingText._startPoint.x + moveX, m_movingText._startPoint.y + moveY);
				}
			}
		}
		else
		{
			m_movingText._textNode->SetEnable(FALSE);
			rMovingTime = 0.0f;
		}
	}
}

void CMovingText::SetMovingPosition(POINT2D startPoint,POINT2D endPoint)
{
	m_startPoint = startPoint;
	m_endPoint = endPoint;
}

void CMovingText::SetMovingText(const char* movingText)
{
#if 1
	if (m_movingText._textNode == NULL)
	{
		I3TRACE("CREATE TEXT NODE : FONT SIZE( %d)\n", 0);

		i3TextNodeDX2 * pTextNode = m_pTextNode[0];

		GetNode()->AddChild(pTextNode);

		m_movingText._textNode = pTextNode;
	}

	m_movingText._textNode->setPos(m_startPoint.x, m_startPoint.y);
	m_movingText._textNode->SetColor(128, 217, 0, 255);		
	m_movingText._textNode->SetText((char*)movingText);
	m_movingText._textNode->SetEnable(FALSE);

	COPY_POINT2D(m_movingText._startPoint, m_startPoint);
	COPY_POINT2D(m_movingText._endPoint, m_endPoint);
	
	i3String::Copy( m_movingText._text, movingText, MAX_STRING_COUNT );
#else
	if (m_movingText._textNode == NULL)
	{
		I3_SAFE_NODE_RELEASE(m_movingText._textNode);
	}

	i3TextNodeDX2* pTextNode = i3TextNodeDX2::new_object();
	I3ASSERT(pTextNode != NULL);

	if (pTextNode != NULL)
	{
		pTextNode->Create( GetDefaultFontName(), FONT_COUNT_256, 60);		
		pTextNode->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		pTextNode->setPos(m_startPoint.x, m_startPoint.y);
		pTextNode->SetTextSpace(0, 5);
		pTextNode->setSize(200, 100);
		pTextNode->SetColor(128, 217, 0, 255);		
		pTextNode->SetText((char*)movingText);
		pTextNode->SetEnable(FALSE);

		GetNode()->AddChild(pTextNode);
		m_movingText._textNode = pTextNode;
		COPY_POINT2D(m_movingText._startPoint, m_startPoint);
		COPY_POINT2D(m_movingText._endPoint, m_endPoint);
		
		i3String::Copy(m_movingText._text, movingText);
	}
#endif
}

#endif
