#include "i3GuiPCH.h"
#include "i3GuiList.h"
#include "i3GuiEditBox.h"

// ============================================================================
//
// i3GuiList : Scroll Bar가 있는 List Control
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiList);

i3GuiList::i3GuiList()
{
	setInputMode( I3GUI_INPUT_MODE_PRESS);
	setControlState(I3GUI_CONTROL_STATE_NORMAL);	
}

i3GuiList::~i3GuiList()
{
	I3_SAFE_RELEASE( m_pSpriteSel);
}

void i3GuiList::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	m_pShape = pShape;

	i3GuiControl::SetControlShapeTemplate( pShape);	
}

// 갱신
void i3GuiList::OnUpdate( REAL32 rDeltaSeconds )
{
	switch(m_ControlState)
	{
	case I3GUI_CONTROL_STATE_NORMAL:
		break;
	case I3GUI_CONTROL_STATE_ONMOUSE:
		break;
	case I3GUI_CONTROL_STATE_DISABLED:
		break;
	case I3GUI_CONTROL_STATE_DRAG:		
		break;
	}	

	if( !m_bOnceUpdate) 
	{
		m_bOnceUpdate = true;
		onceUpdate();
	}
	
	UpdatePos();

	UpdateElement();

	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED ||
		getControlState() == I3GUI_CONTROL_STATE_RESERVED2)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}

	i3GuiControl::OnUpdate( rDeltaSeconds );
}

void i3GuiList::UpdateEditControl(void)
{
	// 툴등에선 생성되지 않는다.
	if (m_pSpriteSel)
	{
		m_pSpriteSel->SetEnable( 0, false);
	}

	for(INT32 i = 0; i < m_nControlCount && i < m_nElementCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];

		switch(pEdit->getControlState())
		{
		case I3GUI_CONTROL_STATE_RESERVED2:
			m_nSelectedElement = GetCurrentTop() + i;
			setControlState(I3GUI_CONTROL_STATE_RESERVED2);			
			PushControlNotify(I3GUI_LIST_NOTIFY_DOUBLECLICK, m_nSelectedElement, 0);
			break;
		case I3GUI_CONTROL_STATE_CLICKED:
			m_nSelectedElement = GetCurrentTop() + i;
			setControlState(I3GUI_CONTROL_STATE_CLICKED);			
			PushControlNotify(I3GUI_LIST_NOTIFY_CLICKED, m_nSelectedElement, 0);
			break;
		case I3GUI_CONTROL_STATE_R_CLICKED:
			m_nSelectedElement = GetCurrentTop() + i;
			setControlState(I3GUI_CONTROL_STATE_R_CLICKED);			
			PushControlNotify(I3GUI_LIST_NOTIFY_RCLICK, m_nSelectedElement, 0);
			break;
		case I3GUI_CONTROL_STATE_ONMOUSE:
			if( m_pSpriteSel != nullptr)
			{
				m_pSpriteSel->SetEnable( 0, true);
				m_pSpriteSel->SetRect( 0, 
					getTotalPosX() + (pEdit->getPositionX() * i3GuiRoot::getGuiResolutionWidth()),
					getTotalPosY() + (pEdit->getPositionY() * i3GuiRoot::getGuiResolutionHeight()),
										(REAL32)pEdit->getWidth(), 
										(REAL32)pEdit->getHeight());
			}

			PushControlNotify(I3GUI_LIST_NOTIFY_ONMOUSE, i, 0);
			break;
		}
	}
}

void i3GuiList::CreateControl(void)
{
	INT32 i = 0;

	// 마우스 오버되어있는 Sel의 테두리입니다.
	{
		m_pSpriteSel = i3Sprite2D::new_object();
		m_pSpriteSel->Create( 1, false, true);

		m_pSpriteSel->SetEnable( 0, false);
		m_pSpriteSel->SetRect( 0, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteSel->SetColor(	0, 96, 196, 213, 255);

		GetNode()->AddChild( m_pSpriteSel);
	}
    
	// Control을 생성합니다.
	REAL32 rGapY = m_pShape->getImage(LIST_BACKGROUND)->m_nHeight / (REAL32)m_nControlCount;
	//INT32 nGabageY = (INT32)(m_pShape->getImage(LIST_BACKGROUND)->m_nHeight) % m_nControlCount;	//C4189
	
	if( m_nControlCount <= 0) m_nControlCount = 0;
	
	REAL32 fX = (REAL32)m_pShape->getImage(LIST_BACKGROUND)->m_nX;
	REAL32 fY = (REAL32)m_pShape->getImage(LIST_BACKGROUND)->m_nY;
	
	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox * pEdit = i3GuiEditBox::new_object_ref();

		pEdit->SetNotifyObserver(this);

		fY = (m_pShape->getImage(LIST_BACKGROUND)->m_nHeight * (REAL32)i) / (REAL32)m_nControlCount;
		
		pEdit->setPosition( fX, fY);
		pEdit->setSize( (INT32) m_pShape->getImage(LIST_BACKGROUND)->m_nWidth, (INT32)rGapY);

		fY += rGapY;

		m_ControlList.push_back( pEdit);

        // Game Graph에 연결
		this->AddChild( pEdit);
	}
}

void i3GuiList::SetSelColor(UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	m_pSpriteSel->SetColor(	0, r, g, b, a);
}

void i3GuiList::CreateText( INT32 nDisplayCount, i3Font * pFont, INT32 nTextCount, INT32 TextSize)
{
	INT32 i = 0;

	m_nControlCount = nDisplayCount;

	CreateControl();

	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->CreateText( pFont, nTextCount, TextSize);
		pEdit->setFontSkip(true);
		pEdit->SetTextAlign( 0, 1);
		pEdit->SetOffsetPos( 3.0f, 0.0f); 
	}
}

void i3GuiList::CreateTextEx(INT32 nDisplayCount, const i3TextFontSetInfo * pFontSetInfo , INT32 nTextCount, INT32 TextSize)
{
	INT32 i = 0;

	m_nControlCount = nDisplayCount;

	CreateControl();

	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->CreateTextEx(pFontSetInfo, nTextCount, TextSize);
		pEdit->setFontSkip(true);
		pEdit->SetTextAlign( 0, 1);
		pEdit->SetOffsetPos( 3.0f, 0.0f); 
	}
}

void i3GuiList::CreateTextEx(INT32 nDisplayCount, const char* pszFont, UINT32 Charset , INT32 nTextCount, INT32 TextSize)
{
	i3TextFontSetInfo fontSetInfo(pszFont, Charset);
	CreateTextEx(nDisplayCount, &fontSetInfo, nTextCount, TextSize);
}

void i3GuiList::SetTextAlign( INT32 AnignX, INT32 AnignY)
{		
	INT32 i = 0;

	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->SetTextAlign(AnignX, AnignY);

		if( AnignX != 0) pEdit->SetOffsetPos( 0.0f, 0.0f); 
		else pEdit->SetOffsetPos( 3.0f, 0.0f); 
	}
}

void i3GuiList::SetSliderPos( REAL32 fPos)
{
	m_fScrollPos = fPos;	
	UpdatePos();
}

void i3GuiList::UpdatePos()
{	
	m_fMinValue = (REAL32)m_pShape->getImage(LIST_SCROLL)->m_nY;
	m_fMaxValue = m_fMinValue + m_pShape->getImage(LIST_SCROLL)->m_nHeight - m_pShape->getImage(LIST_BAR)->m_nHeight;
	// Scale값을 설정합니다.
	m_fScrollScale = (m_fMaxValue - m_fMinValue) * 0.01f;

	if( m_fScrollPos < m_fMinValue ) m_fScrollPos = m_fMinValue;
	if( m_fScrollPos > m_fMaxValue ) m_fScrollPos = m_fMaxValue;

	if(m_fScrollPos>=m_fMinValue && m_fScrollPos<=m_fMaxValue)
	{
		m_pData->m_pShape->getImage(LIST_BAR)->m_nY = (m_fScrollPos);
		m_fCurScrollValue = (m_fScrollPos-m_fMinValue) / m_fScrollScale;
	}
}

void i3GuiList::SetCurValue( REAL32 fValue)				
{
	m_fCurScrollValue = fValue;		
	m_fScrollPos = m_fScrollScale * m_fCurScrollValue;
	UpdatePos();
}

void i3GuiList::AddElement( const i3::rc_wstring& wstr )
{
	m_ElementList.push_back( wstr);

	m_nElementCount++;

	UpdatePos();
}

void i3GuiList::AddHeadElement(const i3::rc_wstring& wstr)
{
	m_ElementList.insert(m_ElementList.begin(), wstr);

	m_nElementCount++;

	UpdatePos();
}

void i3GuiList::UpdateElement(void)
{
	// 리스트 최상단 시작 엘리먼트를 구한다
	INT32 idxElement = GetCurrentTop();

	// 각 에디트박스를 돌면서 값을 넣는다
    for(INT32 i = 0; i < m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];

		if (idxElement < (INT32)m_ElementList.size())
		{
			pEdit->SetTextEllipsis(m_ElementList[idxElement]);
			idxElement++;
		}
		else
		{
			pEdit->SetText(L"");
		}
	}
}

void i3GuiList::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	// 요소가 화면에 표시될 개수보다 같거나 적으면 입력처리를 하지 않는다

	//INT32 i = 0;	//C4189

	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{

		if(m_nElementCount <= m_nControlCount)
		{

		}
		else
		{

			if (_DragScroll(fX, fY, state))
			{
				i3GuiRoot::setCapturedMouseControl(this);
			}
		}
	}
	else
	{
		if( i3GuiRoot::getCapturedMouseControl() == this)
		{
			i3GuiRoot::setCapturedMouseControl(nullptr);
		}
	}

	if (TESTBIT(state, I3GUI_MOUSE_STATE_LBUTTON_REPEAT))
	{
		MovePrev(fX, fY, state);
		MoveNext(fX, fY, state);
	}
}

void i3GuiList::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiList*>(pDest))
		return ;

	i3GuiList * pList = (i3GuiList*)pDest;

	pList->setControlState( m_ControlState );
}

void i3GuiList::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUILIST
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiList::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUILIST data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiStatic::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiList::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUILIST data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiList::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

bool i3GuiList::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	// 복합 컨트롤의 경우, 내부 컴포넌트와 외부 컬랙션 컨트롤간의 이벤트 전달에 시차가 발생합니다.
	// 따라서, 내부 컴보넌트의 UserInput 발생시, 외부 컬랙션의 OnEvent를 호출하여 즉시 처리하도록 합니다.

	UpdateEditControl();

	return true;
}

const i3::rc_wstring& i3GuiList::GetText(UINT32 idxElement) const
{
	return m_ElementList[idxElement];
}

void i3GuiList::SetText(UINT32 idxElement, const i3::rc_wstring& str)
{
	I3ASSERT(idxElement < (UINT32)m_nElementCount && "Idx Overflower");	

	// 새 문자열 설정
	//m_ElementList.SetItem(idxElement, strElement);
	i3::vu::set_value_force(m_ElementList, idxElement, str);
}

void i3GuiList::SetTextColor(const COLOR* pColor)
{
	for(INT32 i = 0; i < m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->SetTextColor((COLOR*)pColor);
	}
}

void i3GuiList::SetTextColor(INT32 idxControl,const COLOR* pColor)
{
	I3ASSERT(idxControl < m_nControlCount && "Idx Overflower");

	i3GuiEditBox* pEdit = m_ControlList[idxControl];
	pEdit->SetTextColor((COLOR*)pColor);
}

void i3GuiList::Clear(void)
{
	m_ElementList.clear();
	m_nElementCount = 0;
	m_nSelectedElement = 0;
}

UINT32 i3GuiList::GetCurrentTop(void)
{
	if (m_nElementCount < m_nControlCount)
	{
		return 0;
	}
	else
	{
		UINT32 nOverCount = m_nElementCount - m_nControlCount;

		return (UINT32)((nOverCount * m_fCurScrollValue * 0.01f) + 0.5f);
	}
}

void i3GuiList::MovePrev(REAL32 fX, REAL32 fY, UINT32 state)
{
	//INT32 i = 0;	//C4189
	INT32 posX = 0;
	INT32 nWidth = 0; 
	INT32 posY = 0;
	INT32 nHeight = 0;

	// 위쪽 증가 버튼
	posX	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_UP)->m_nX;
	nWidth	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_UP)->m_nWidth;
	posY	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_UP)->m_nY;
	nHeight = (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_UP)->m_nHeight;

	if( fX >= (getTotalPosX()+posX) && fX <= (getTotalPosX()+posX+nWidth))
	{
		if( fY >= (getTotalPosY()+posY) && fY <= (getTotalPosY()+posY+nHeight))
		{
			if (m_nElementCount > m_nControlCount)
			{
				m_fScrollPos -= (m_fScrollScale * (100.0f / (m_nElementCount - m_nControlCount)));
				PushControlNotify(I3GUI_LIST_NOTIFY_DRAG, 0, 0);
			}
			else
			{
				m_fScrollPos = m_fMinValue;
			}

			UpdatePos();
		}
	}
}

void i3GuiList::MoveNext(REAL32 fX, REAL32 fY, UINT32 state)
{
	//INT32 i = 0;	//C4189
	INT32 posX = 0;
	INT32 nWidth = 0; 
	INT32 posY = 0;
	INT32 nHeight = 0;

	// 아래쪽 증가버튼
	posX	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_DOWN)->m_nX;
	nWidth	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_DOWN)->m_nWidth;
	posY	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_DOWN)->m_nY;
	nHeight = (INT32) m_pData->m_pShape->getImage(LIST_SCROLL_BUTTON_DOWN)->m_nHeight;	

	if( fX >= (getTotalPosX()+posX) && fX <= (getTotalPosX()+posX+nWidth)) 
	{
		if( fY >= (getTotalPosY()+posY) && fY <= (getTotalPosY()+posY+nHeight))
		{
			if(m_nElementCount > m_nControlCount)
			{
				m_fScrollPos += (m_fScrollScale * (100.0f / (m_nElementCount - m_nControlCount)));
				PushControlNotify(I3GUI_LIST_NOTIFY_DRAG, 0, 0);
			}
			else
			{
				m_fScrollPos = m_fMaxValue;
			}

			UpdatePos();
		}
	}
}

void i3GuiList::MovePrev(void)
{
	if (m_nElementCount > m_nControlCount)
	{		
		m_fScrollPos -= (m_fScrollScale * (100.0f / (m_nElementCount - m_nControlCount)));
	}
	else
	{
		return;
		//m_fScrollPos = m_fMinValue;
	}

	UpdatePos();
}

void i3GuiList::MoveNext(void)
{
	if(m_nElementCount > m_nControlCount)
	{		
		m_fScrollPos += (m_fScrollScale * (100.0f / (m_nElementCount - m_nControlCount)));
	}
	else
	{
		return;
		//m_fScrollPos = m_fMaxValue;
	}

	UpdatePos();
}

void i3GuiList::onceUpdate(void)
{
	// Scroll할 정보를 각Control들의 크기로 구합니다.
	if(m_pShape->getImage(LIST_SCROLL)->m_nY < 0) m_pShape->getImage(LIST_SCROLL)->m_nY = 0;
	
	// 초기에 Scroll Bar의 위치를 설정합니다.
	m_fScrollPos = (REAL32)m_pShape->getImage(LIST_SCROLL)->m_nY;
	m_pData->m_pShape->getImage(LIST_BAR)->m_nY = m_pData->m_pShape->getImage(LIST_SCROLL)->m_nY;

	m_nOriginalWidth = (INT32)m_pShape->getImage(LIST_BACKGROUND)->m_nWidth;
	m_nOriginalHeight = (INT32)m_pShape->getImage(LIST_BACKGROUND)->m_nHeight;
}

bool i3GuiList::_DragScroll(REAL32 fX,REAL32 fY,UINT32 state)
{
	bool isDragging = false;

	INT32 posX	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL)->m_nX;
	INT32 nWidth	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL)->m_nWidth;
	INT32 posY	= (INT32) m_pData->m_pShape->getImage(LIST_SCROLL)->m_nY;
	INT32 nHeight = (INT32) m_pData->m_pShape->getImage(LIST_SCROLL)->m_nHeight;

	if (i3GuiRoot::getCapturedMouseControl() == this)
	{
		m_fScrollPos = fY - getTotalPosY() - (m_pShape->getImage(LIST_BAR)->m_nHeight/2);

		setControlState(I3GUI_CONTROL_STATE_DRAG);

		if (m_nElementCount > m_nControlCount)
		{					
			PushControlNotify(I3GUI_LIST_NOTIFY_DRAG, 0, 0);
		}

		isDragging = true;

		return isDragging;
	}

	if( fX >= (getTotalPosX()+posX) && fX <= (getTotalPosX()+posX+nWidth))
	{
		if( fY >= (getTotalPosY()+posY) && fY <= (getTotalPosY()+posY+nHeight))
		{
			m_fScrollPos = fY - getTotalPosY() - (m_pShape->getImage(LIST_BAR)->m_nHeight/2);

			setControlState(I3GUI_CONTROL_STATE_DRAG);

			if (m_nElementCount > m_nControlCount)
			{					
				PushControlNotify(I3GUI_LIST_NOTIFY_DRAG, 0, 0);
			}

			isDragging = true;
		}
	}

	return isDragging;
}

bool i3GuiList::_ResizeControl(void)
{
	if (false == i3GuiControl::_ResizeControl())
	{
		return false;
	}

	_ResizeList();	

	return true;
}

void i3GuiList::_ResizeList(void)
{
	INT32 i = 0;
	REAL32 fX = 0;
	REAL32 fY = 0;

	for( i = 0; i<m_nControlCount; i++)
	{
		if( i<(INT32)m_ControlList.size())
		{
			i3GuiEditBox* pEdit = m_ControlList[i];
			pEdit->ResizeText();

			INT32 nGapY = (m_nOriginalHeight) / m_nControlCount;
			pEdit->setPosition( fX, fY);
			pEdit->setSize(m_nOriginalWidth, nGapY);
			pEdit->setOriginalControlSize(m_nOriginalWidth, nGapY);	

			fY += nGapY;
		}
	}
}
