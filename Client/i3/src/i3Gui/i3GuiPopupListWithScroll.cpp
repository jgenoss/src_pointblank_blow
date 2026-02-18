#include "i3GuiPCH.h"
#include "i3GuiEditBox.h"
#include "i3GuiPopupListWithScroll.h"

#define OFFSET_HEIGHT 2

I3_CLASS_INSTANCE( i3GuiPopupListWithScroll );

i3GuiPopupListWithScroll::~i3GuiPopupListWithScroll()
{
	m_ElementColorList.clear();
}

I3GUI_LISTPOPUP_SCROLL_SHAPE i3GuiPopupListWithScroll::_GetScrollCtrlByMousePos( REAL32 fX, REAL32 fY)
{
	if( m_pData->m_pShape)
	{
		POINT pt = { (LONG)(fX - this->getTotalPosX()), (LONG)(fY - this->getTotalPosY())};
		
		{
			i3GuiImage* pDown = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_DOWN);
			RECT rc = { (LONG)pDown->m_nX, (LONG)pDown->m_nY, (LONG)(pDown->m_nX + pDown->getWidth()), (LONG)(pDown->m_nY + pDown->getHeight())};
			if( PtInRect( &rc, pt)) 
				return I3GUI_CTRL_LISTPOPUP_SCROLL_DOWN;
		}

		{
			i3GuiImage* pUp = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_UP);
			RECT rc = { (LONG)pUp->m_nX, (LONG)pUp->m_nY, (LONG)(pUp->m_nX + pUp->getWidth()), (LONG)(pUp->m_nY + pUp->getHeight())};
			if( PtInRect( &rc, pt)) 
				return I3GUI_CTRL_LISTPOPUP_SCROLL_UP;
		}

		{
			i3GuiImage* pBar = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BAR);
			RECT rc = { (LONG)pBar->m_nX, (LONG)pBar->m_nY, (LONG)(pBar->m_nX + pBar->getWidth()), (LONG)(pBar->m_nY + pBar->getHeight())};
			if( PtInRect( &rc, pt)) 
				return I3GUI_CTRL_LISTPOPUP_SCROLL_BAR;
		}
	}

	return I3GUI_CTRL_LISTPOPUP_SCROLL_BG;
}

bool i3GuiPopupListWithScroll::PtInRect( RECT* rc, POINT pt)
{
	return rc->left < pt.x && rc->right >= pt.x && rc->top < pt.y && rc->bottom >= pt.y;
}

INT32 i3GuiPopupListWithScroll::_GetScrollBarIdx()
{
	if( m_fMoveSpeed <= 0.f) return 0;

	REAL32 rv = m_fCurScrollPos/m_fMoveSpeed;
	REAL32 sub = rv - (INT32)(rv);
	REAL32 idx = rv - sub;
	if( sub > 0.9998f)		//오차..
		idx++;

	return (INT32)idx;
}

/*virtual*/ void i3GuiPopupListWithScroll::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GuiPopupList::OnUpdate( rDeltaSeconds);
}

/*virtual*/ void i3GuiPopupListWithScroll::UserInput(REAL32 fX, REAL32 fY, REAL32 fDeltaZ, UINT32 state)
{
	if( getControlDisable())
	{
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	//Scroll Control 영역 외 부분은 Edit Control로 채워져 있기때문에 
	//Input이 Edit Control로 갑니다.
	
	i3GuiImage* pBar = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BAR);

	if( state & I3I_MOUSE_LBUTTON)
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);

		if( m_bEnableScroll)
		{
			//drag
			if( i3GuiRoot::getCapturedMouseControl() == this && m_bBarDrag)
			{
				pBar->m_nY = fY - this->getTotalPosY() - (pBar->getHeight()/2.f);
				i3GuiRoot::setCapturedMouseControl(this);
			}
			else
			{	
				m_eCurSelectedShape = _GetScrollCtrlByMousePos( fX, fY);
				switch( m_eCurSelectedShape)
				{
				case I3GUI_CTRL_LISTPOPUP_SCROLL_DOWN:
					{
						if( !m_bClickedButton)
						{
							pBar->m_nY += m_fMoveSpeed;
							m_bClickedButton = true;
							m_fFirstUpdateTime = 0.f;
							m_fUpdateTime = 0.f;
						}
						else
						{
							m_fFirstUpdateTime += 0.016f;
							if( m_fFirstUpdateTime < 0.8f) break;
						}

						m_fUpdateTime += 0.016f;
						if( m_fUpdateTime >= m_fUpdatePeriod)
						{
							pBar->m_nY += m_fMoveSpeed;
							m_fUpdateTime = 0.f;
						}
					}
					break;

				case I3GUI_CTRL_LISTPOPUP_SCROLL_UP:
					{
						if( !m_bClickedButton)
						{
							pBar->m_nY -= m_fMoveSpeed;
							m_bClickedButton = true;
							m_fFirstUpdateTime = 0.f;
							m_fUpdateTime = 0.f;
						}
						else
						{
							m_fFirstUpdateTime += 0.016f;
							if( m_fFirstUpdateTime < 0.8f) break;
						}

						m_fUpdateTime += 0.016f;
						if( m_fUpdateTime >= m_fUpdatePeriod)
						{
							pBar->m_nY -= m_fMoveSpeed;
							m_fUpdateTime = 0.f;
						}
					}
					break;

				case I3GUI_CTRL_LISTPOPUP_SCROLL_BAR:
					{
						m_bBarDrag = true;
					}
					break;
				}
				i3GuiRoot::setCapturedMouseControl(this);
			}
		}
	}
	else
	{
		setControlState(I3GUI_CONTROL_STATE_ONMOUSE);

		if( i3GuiRoot::getCapturedMouseControl() == this)
		{
			i3GuiRoot::setCapturedMouseControl(nullptr);

			m_bBarDrag = false;
			m_bClickedButton = false;
		}

		//wheel -120 or 120
		if( fDeltaZ < 0)
			pBar->m_nY += m_fMoveSpeed;

		if( fDeltaZ > 0)
			pBar->m_nY -= m_fMoveSpeed;

		//I3TRACE( "user input popuplist with scroll.. on mouse.. \n");
	}

	//sync
	if( pBar->m_nY >= m_fMaxArea)
	{
		pBar->m_nY = m_fMaxArea;	
	}

	if( pBar->m_nY <= m_fMinArea)
	{
		pBar->m_nY = m_fMinArea;
	}
	m_fCurScrollPos = pBar->m_nY - m_fMinArea;
}

/*virtual*/ void i3GuiPopupListWithScroll::CreateControl(void)
{
	{
		m_pSpriteSel = i3Sprite2D::new_object();
		m_pSpriteSel->Create( 1, false, true);

		m_pSpriteSel->SetEnable( 0, false);
		m_pSpriteSel->SetRect( 0, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteSel->SetColor(	0, &m_SelColor);

		GetNode()->AddChild( m_pSpriteSel);
	}

	INT32 max = m_nOriginalHeight / (m_nTextSize + OFFSET_HEIGHT);
	m_nMaxControlCount = (m_nControlCount > max) ? max : m_nControlCount;
	m_bEnableScroll = (m_nMaxControlCount < m_nControlCount);


	REAL32 fX = 0;
	REAL32 fY = 0;

	INT32 nEditWidth = m_nOriginalWidth;
	if( m_pData->m_pShape)
	{
		i3GuiImage* pImage = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BG);
		if( pImage)
			nEditWidth -= (INT32)pImage->getOriginalWidth();
	}

	for( INT32 i = 0 ; i < m_nMaxControlCount; i++)
	{
		i3GuiEditBox * pEdit = i3GuiEditBox::new_object_ref();

		pEdit->SetNotifyObserver(this);
		pEdit->setPosition( fX, fY);
		
		pEdit->setSize(nEditWidth, m_nTextSize);
		fY += (m_nTextSize + OFFSET_HEIGHT);

		m_ControlList.push_back( pEdit);

		// Game Graph에 연결
		this->AddChild( pEdit);
	}
}

/*virtual*/ void i3GuiPopupListWithScroll::SetDisplayCount( INT32 nDisplayCount)
{
	if( m_nTextCount > 0)
	{
		//1. 기존에 가지고 있던 Item, Edit Control을 삭제.
		if( m_ElementList.size() > 0)
		{
			m_ElementList.clear();
			m_ElementColorList.clear();
		}
		if( m_ControlList.size() > 0)
		{
			for( size_t i = 0; i < m_ControlList.size(); i++)
			{
				i3GuiEditBox * pEdit = m_ControlList[i];		
				this->RemoveChild( pEdit);
			}
			m_ControlList.clear();
		}

		//2. 선택 UI Sprite 삭제.
		if( m_pSpriteSel)
		{
			GetNode()->RemoveChild(m_pSpriteSel);
			I3_SAFE_RELEASE( m_pSpriteSel);
		}
		//---------------------------------------------------------삭제.

			
		//3. 해당 카운트에 맞게 다시 생성.
		m_nControlCount = nDisplayCount;
		CreateControl();
		for( INT32 i = 0; i < m_nMaxControlCount; i++)
		{
			i3GuiEditBox* pEdit = m_ControlList[i];

			if( i3::generic_string_size( m_FontSetInfo.GetFontName()) > 0)	
				pEdit->CreateTextEx( &m_FontSetInfo, m_nTextCount, m_nTextSize);
			else
				pEdit->CreateText( m_pFont, m_nTextCount, m_nTextSize);

			pEdit->setFontSkip(true);
			pEdit->SetTextAlign( 0, 1);
			pEdit->SetOffsetPos( 3.0f, 0.0f); 
		}
		//---------------------------------------------------------생성.


		_ResizePopUp();
		//---------------------------------------------------------사이즈.
	}
}

/*virtual*/ void i3GuiPopupListWithScroll::AddElement( const i3::rc_wstring& wstrTemp)
{
	COLOR DefaultColor;
	DefaultColor.SetARGB( 0xFFFFFFFF);
	m_ElementColorList.push_back( DefaultColor);

	i3GuiPopupList::AddElement( wstrTemp);
}

/*virtual*/ void i3GuiPopupListWithScroll::UpdateElement(void)
{
	for( INT32 i = 0; i<GetEditControlCount(); i++)
	{
		INT32 nElementPos = i + _GetScrollBarIdx();

		if( nElementPos < (INT32)m_ElementList.size())
		{
			i3GuiEditBox* pEdit = m_ControlList[i];
			pEdit->SetText( m_ElementList[nElementPos]);
			pEdit->SetTextColor((COLOR*)&m_ElementColorList[nElementPos]);
		}
	}
}

/*virtual*/ void i3GuiPopupListWithScroll::UpdateEditControl(void)
{
	for( size_t i = 0; i < m_ControlList.size(); i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];

		if( pEdit->getControlState() == I3GUI_CONTROL_STATE_RESERVED2)
		{
			m_nSelectedElement = i + _GetScrollBarIdx();

			setControlState(I3GUI_CONTROL_STATE_RESERVED2);
			PushControlNotify(I3GUI_POPUPLIST_NOTIFY_DOUBLECLICK, m_nSelectedElement, 0);
		}
		else if( pEdit->getControlState() == I3GUI_CONTROL_STATE_CLICKED)		// 마우스로 선택되어진 Edit를 반환합니다.
		{
			m_nSelectedElement = i + _GetScrollBarIdx();

			setControlState(I3GUI_CONTROL_STATE_CLICKED);
			PushControlNotify(I3GUI_POPUPLIST_NOTIFY_CLICKED, m_nSelectedElement, 0);

			// 컴보박스용 팝업리스트의 경우 선택되어지면 닫는다
			if( m_bExclusiveEnableControl)
			{
				SetFocus(false);
			}
		}
		else if( pEdit->getControlState() == I3GUI_CONTROL_STATE_R_CLICKED)
		{
			m_nSelectedElement = i + _GetScrollBarIdx();

			setControlState(I3GUI_CONTROL_STATE_R_CLICKED);
			PushControlNotify(I3GUI_POPUPLIST_NOTIFY_RCLICK, m_nSelectedElement, 0);

		}
		else if( pEdit->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)
		{
			m_pSpriteSel->SetEnable( 0, true);
			
			m_pSpriteSel->SetRect( 0, 
				getTotalPosX() + pEdit->getPositionX() * i3GuiRoot::getGuiResolutionWidth(), 
				getTotalPosY() + pEdit->getPositionY()*i3GuiRoot::getGuiResolutionHeight(), 
				(REAL32)pEdit->getWidth(), 
				(REAL32)pEdit->getHeight());

			PushControlNotify(I3GUI_POPUPLIST_NOTIFY_ONMOUSE, i, 0);
			break;
		}
		else
		{
			m_pSpriteSel->SetEnable( 0, false);
		}

	}
}

/*virtual*/ void i3GuiPopupListWithScroll::SetTextColor( const COLOR* pColor)
{
	i3GuiPopupList::SetTextColor( pColor);
}

/*virtual*/ void i3GuiPopupListWithScroll::SetTextColor( INT32 idxElement,const COLOR* pColor)
{
	I3ASSERT(idxElement < (INT32)m_ElementColorList.size() && "Idx Overflower");
	m_ElementColorList[idxElement] = *pColor;
}

/*virtual*/ void i3GuiPopupListWithScroll::SetSelectedElement( INT32 nIdx, bool bOnEvent /*= true*/)
{
	if( nIdx >= (INT32)m_ElementColorList.size() && nIdx < 0 && m_nSelectedElement == nIdx)
	{
		return;
	}

	m_nSelectedElement = nIdx;

	i3GuiImage* pBar = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BAR);
	pBar->m_nY = (m_nSelectedElement * m_fMoveSpeed) + m_fMinArea;
	if( pBar->m_nY >= m_fMaxArea)
	{
		pBar->m_nY = m_fMaxArea;	
	}

	if( pBar->m_nY <= m_fMinArea)
	{
		pBar->m_nY = m_fMinArea;
	}
	m_fCurScrollPos = pBar->m_nY - m_fMinArea;

	if(bOnEvent == true)
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
		PushControlNotify(I3GUI_POPUPLIST_NOTIFY_CLICKED, m_nSelectedElement, 0);
	}
}

/*virtual*/ void i3GuiPopupListWithScroll::Clear(void)
{
	i3GuiPopupList::Clear();
	m_ElementColorList.clear();
}

/*virtual*/ void i3GuiPopupListWithScroll::_ResizePopUp(void)
{
	INT32 i = 0;
	REAL32 fX = 0.0f;
	REAL32 fY = 0.0f;
	INT32 nGapY = 0;

	if( m_nControlCount <= 0)
		return;

	// 1. Edit Control 정리.
	INT32 nEditWidth = m_nOriginalWidth;
	if( m_pData->m_pShape)
	{
		i3GuiImage* pImage = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BG);
		if( pImage)
			nEditWidth -= (INT32)pImage->getOriginalWidth();
	}

	for( i = 0; i < m_nMaxControlCount; i++)
	{
		if( i < (INT32)m_ControlList.size())
		{
			i3GuiEditBox* pEdit = m_ControlList[i];		

			pEdit->setPosition( fX, fY);
			nGapY = pEdit->getTextNodeDX()->getTextHeight();								
			nGapY = (INT32)(nGapY / i3GuiRoot::getGuiResolutionHeight());	//C4244
			pEdit->setSize((INT32)(nEditWidth), nGapY + OFFSET_HEIGHT);	
			pEdit->setOriginalSize((INT32)(nEditWidth), nGapY + OFFSET_HEIGHT);	
			pEdit->setOriginalControlSize((INT32)(nEditWidth), nGapY + OFFSET_HEIGHT);	
			fY += (nGapY + OFFSET_HEIGHT);

			pEdit->ResizeText();	
		}
	}

	
	// 2. 컨트롤의 숫자만큼 팝업리스트 배경을 리사이즈 합니다. (Scroll 제외)
	REAL32 fShapeHeightRate = 1.0f;	
	fShapeHeightRate = (REAL32)((nGapY+OFFSET_HEIGHT) * m_nMaxControlCount) / (REAL32)(m_nOriginalHeight);

	if( m_pData->m_pShape)
	{
		REAL32 rx = i3GuiRoot::getGuiResolutionWidth();
		REAL32 ry = i3GuiRoot::getGuiResolutionHeight();

		for(i=0; i <= I3GUI_CTRL_SHAPE_BOTTOM_RIGHT; i++)
		{
			// Shape의 크기를 화면 해상도에 맞게 변경합니다.

			i3GuiImage *pImage = m_pData->m_pShape->getImage((I3GUI_CONTROL_SHAPE) i);

			// 컨트롤의 초기위치와 본사이즈로 재설정합니다.
			pImage->Reset();

			pImage->setWidth( (pImage->getWidth() * rx));
			pImage->setHeight( (pImage->getHeight() * ry * fShapeHeightRate));			

			pImage->m_nX = (pImage->m_nX * rx);
			pImage->m_nY = (pImage->m_nY * ry * fShapeHeightRate);
		}

		//scroll resize
		{
			i3GuiImage *pBg = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BG);
			pBg->Reset();
			pBg->setHeight( (pBg->getHeight() * ry * fShapeHeightRate));
			pBg->m_nX = (pBg->m_nX * rx);
			pBg->m_nY = (pBg->m_nY * ry * fShapeHeightRate);
			pBg->setEnable( (bool)m_bEnableScroll);

			i3GuiImage *pDown = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_DOWN);
			pDown->Reset();
			pDown->m_nX = (pDown->m_nX * rx);

			//scroll 버튼 사이즈는 scale 하지 않기 때문에 스케일 될 넓이 많큼 이동.
			REAL32 nScaleGapY = (pDown->getHeight() * ry * fShapeHeightRate) - pDown->getHeight();
			pDown->m_nY = (pDown->m_nY * ry * fShapeHeightRate) + (nScaleGapY);
			pDown->setEnable( (bool)m_bEnableScroll);

			
			
			i3GuiImage *pUp = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_UP);
			pUp->Reset();
			pUp->m_nX = (pUp->m_nX * rx);
			pUp->m_nY = (pUp->m_nY * ry * fShapeHeightRate);
			pUp->setEnable( (bool)m_bEnableScroll);
		
	
			
			i3GuiImage *pBar = m_pData->m_pShape->getImage( I3GUI_CTRL_LISTPOPUP_SCROLL_BAR);
			pBar->Reset();
			pBar->m_nX = (pBar->m_nX * rx);
			pBar->m_nY = (pBar->m_nY * ry * fShapeHeightRate) - nScaleGapY;
			pBar->setEnable( (bool)m_bEnableScroll);



			//---------------------------------------------------------Scroll 영역 설정.
			REAL32 fArea = pBg->getHeight() - (pUp->getHeight() + pDown->getHeight());
			if( fArea < 0.f) fArea = 0.f;

			
			INT32 nToTalCount = (m_nControlCount == 0) ? 1 : m_nControlCount;
			REAL32 fBarSize = (fArea * m_nMaxControlCount)/nToTalCount;
			pBar->setHeight( fBarSize);										//Bar의 사이즈는 Scale하지 않는다.

			m_fMinArea = pBar->m_nY;
			m_fMaxArea = m_fMinArea + (fArea - fBarSize);

			m_fMoveSpeed = fArea/nToTalCount;
			m_fCurScrollPos = 0.f;
		}
	}
}