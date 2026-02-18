#include "i3GuiPCH.h"
#include "i3GuiPopupList.h"
#include "i3GuiEditBox.h"

#define OFFSET_HEIGHT 2

// ============================================================================
//
// i3GuiStatic : 이미지 혹은 글자
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiPopupList);

i3GuiPopupList::i3GuiPopupList()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

i3GuiPopupList::~i3GuiPopupList()
{
	GetNode()->RemoveChild(m_pSpriteSel);
	I3_SAFE_RELEASE( m_pSpriteSel);
}

void i3GuiPopupList::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	m_pShape = pShape;

	i3GuiControl::SetControlShapeTemplate( pShape);	

	if( !m_bOnceUpdate)
	{
		m_bOnceUpdate = true;
		m_nOriginalWidth = getWidth();
		m_nOriginalHeight = getHeight();
	}
}

// 갱신
void i3GuiPopupList::OnUpdate( REAL32 rDeltaSeconds )
{		
	UpdateElement();
	UpdateRenderObjects();
	
	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED ||
		getControlState() == I3GUI_CONTROL_STATE_RESERVED2)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}

	i3GuiControl::OnUpdate( rDeltaSeconds );
}

void i3GuiPopupList::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable())
	{
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
	}
	// Mouse가 버튼위에 올려져 있습니다.
	else
	{
		setControlState(I3GUI_CONTROL_STATE_ONMOUSE);
	}
}

void i3GuiPopupList::AddElement( const i3::rc_wstring& wstrTemp)
{

	m_ElementList.push_back( wstrTemp);

	m_nElementCount++;

	UpdateElement();
}

void i3GuiPopupList::UpdateElement(void)
{

	for( INT32 i = 0; i<m_nControlCount; i++)
	{
		if( i< (INT32)m_ElementList.size())
		{
			i3GuiEditBox* pEdit = m_ControlList[i];
			pEdit->SetText( m_ElementList[i] );
		}
	}
}

void i3GuiPopupList::SetSelColor(UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	i3Color::Set( &m_SelColor, r, g, b, a);
	m_pSpriteSel->SetColor(	0, r, g, b, a);
}

void i3GuiPopupList::CreateControl(void)
{
	// 마우스 오버되어있는 Sel의 테두리입니다.
	{
		m_pSpriteSel = i3Sprite2D::new_object();
		m_pSpriteSel->Create( 1, false, true);

		m_pSpriteSel->SetEnable( 0, false);
		m_pSpriteSel->SetRect( 0, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteSel->SetColor(	0, &m_SelColor);

		GetNode()->AddChild( m_pSpriteSel);
	}
    
	// Control을 생성합니다.
	//if( !m_bOnceUpdate)
//	{
		//INT32 nGapY = m_nOriginalHeight / m_nControlCount;
		INT32 nGapY = 16;
//	}


	REAL32 fX = 0;
	REAL32 fY = 0;

	for( INT32 i = 0 ; i < m_nControlCount ; i++)
	{
		i3GuiEditBox * pEdit = i3GuiEditBox::new_object_ref();

		pEdit->SetNotifyObserver(this);
		pEdit->setPosition( fX, fY);
		pEdit->setSize(m_nOriginalWidth, nGapY);
		fY += nGapY;

		m_ControlList.push_back( pEdit);

        // Game Graph에 연결
		this->AddChild( pEdit);
	}
}

void i3GuiPopupList::CreateText( INT32 nDisplayCount, i3Font * pFont, INT32 nTextCount, INT32 nTextSize)
{
	INT32 i = 0;
	REAL32 fX = 0;
	REAL32 fY = 0;
	INT32 nGapY = 0;

	m_pFont = pFont;
	m_nTextCount = nTextCount;
	m_nTextSize = nTextSize;

	m_nControlCount = nDisplayCount;

	CreateControl();
	
	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->CreateText( m_pFont, m_nTextCount, m_nTextSize);
		pEdit->setFontSkip(true);
		pEdit->SetTextAlign( 0, 1);
		pEdit->SetOffsetPos( 3.0f, 0.0f); 
		
		nGapY = pEdit->getTextNodeDX()->getTextHeight();	
		pEdit->setPosition( fX, fY);
		pEdit->setSize(m_nOriginalWidth, nGapY + OFFSET_HEIGHT);
		pEdit->setOriginalSize( m_nOriginalWidth, nGapY + OFFSET_HEIGHT);
		pEdit->setOriginalControlSize((INT32)(m_nOriginalWidth), nGapY + OFFSET_HEIGHT);
		fY += nGapY + OFFSET_HEIGHT;
	}
}


void i3GuiPopupList::CreateTextEx( INT32 nDisplayCount, const i3TextFontSetInfo * pFontSet, INT32 nTextCount, INT32 nTextSize)
{
	INT32 i = 0;
	REAL32 fX = 0;
	REAL32 fY = 0;
	INT32 nGapY = 0;

	m_FontSetInfo	= *pFontSet;
	m_nTextCount	= nTextCount;
	m_nTextSize		= nTextSize;

	m_nControlCount = nDisplayCount;

	CreateControl();

	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->CreateTextEx( &m_FontSetInfo, m_nTextCount, m_nTextSize);
		pEdit->setFontSkip(true);
		pEdit->SetTextAlign( 0, 1);
		pEdit->SetOffsetPos( 3.0f, 0.0f); 

		nGapY = pEdit->getTextNodeDX()->getTextHeight();
		pEdit->setPosition( fX, fY);
		pEdit->setSize(m_nOriginalWidth, nGapY + OFFSET_HEIGHT);
		pEdit->setOriginalSize( m_nOriginalWidth, nGapY + OFFSET_HEIGHT);
		pEdit->setOriginalControlSize((INT32)(m_nOriginalWidth), nGapY + OFFSET_HEIGHT);
		fY += nGapY + OFFSET_HEIGHT;
	}
}

void i3GuiPopupList::CreateTextEx( INT32 nDisplayCount, const char* pszFont, UINT32 CharSet, INT32 nTextCount, INT32 nTextSize)
{
	i3TextFontSetInfo	fontSetInfo(pszFont, CharSet);
	return CreateTextEx(nDisplayCount, &fontSetInfo, nTextCount, nTextSize);
}


void i3GuiPopupList::SetDisplayCount(INT32 nDisplayCount)
{
	INT32 i=0;

	if( m_nTextCount > 0)
	{
		m_ElementList.clear();

		if( m_ControlList.size() > 0)
		{
			for( size_t i = 0; i < m_ControlList.size(); i++)
			{
				i3GuiEditBox * pEdit = m_ControlList[i];		
				this->RemoveChild( pEdit);
			}
			m_ControlList.clear();
		}

		if( m_pSpriteSel)
		{
			GetNode()->RemoveChild(m_pSpriteSel);
			I3_SAFE_RELEASE( m_pSpriteSel);
		}

		if( i3::generic_string_size( m_FontSetInfo.GetFontName() ) > 0)
		{
			m_nControlCount = nDisplayCount;

			CreateControl();

			for( i=0; i<m_nControlCount; i++)
			{
				i3GuiEditBox* pEdit = m_ControlList[i];
				pEdit->CreateTextEx( &m_FontSetInfo, m_nTextCount, m_nTextSize);
				pEdit->setFontSkip(true);
				pEdit->SetTextAlign( 0, 1);
				pEdit->SetOffsetPos( 3.0f, 0.0f); 
			}
		}
		else
		{
			m_nControlCount = nDisplayCount;

			CreateControl();

			for( i=0; i<m_nControlCount; i++)
			{
				i3GuiEditBox* pEdit = m_ControlList[i];
				pEdit->CreateText( m_pFont, m_nTextCount, m_nTextSize);
				pEdit->setFontSkip(true);
				pEdit->SetTextAlign( 0, 1);
				pEdit->SetOffsetPos( 3.0f, 0.0f); 
			}
		}

		_ResizePopUp();
	}
}

void i3GuiPopupList::SetTextAlign( INT32 AnignX, INT32 AnignY)
{
	INT32 i = 0;
	
	for( i=0; i<m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->SetTextAlign( AnignX, AnignY);

		if( AnignX != 0) pEdit->SetOffsetPos( 0.0f, 0.0f); 
		else pEdit->SetOffsetPos( 3.0f, 0.0f); 
	}
}

void i3GuiPopupList::SetSelectedElement(INT32 nIdx, bool bOnEvent)
{
	if( nIdx >= (INT32)m_ControlList.size() && nIdx < 0 && m_nSelectedElement == nIdx)
	{
		return;
	}

	m_nSelectedElement = nIdx;

	if(bOnEvent == true)
	{
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
		PushControlNotify(I3GUI_POPUPLIST_NOTIFY_CLICKED, m_nSelectedElement, 0);
	}
}

void i3GuiPopupList::UpdateEditControl(void)
{
	INT32 i = 0;

	for( i = 0; i<m_nControlCount; i++)
	{
		if( i< (INT32)m_ControlList.size())
		{
			i3GuiEditBox* pEdit = m_ControlList[i];
			
			if (I3GUI_CONTROL_STATE_RESERVED2 == pEdit->getControlState())
			{
				m_nSelectedElement = i;

				setControlState(I3GUI_CONTROL_STATE_RESERVED2);
				PushControlNotify(I3GUI_POPUPLIST_NOTIFY_DOUBLECLICK, m_nSelectedElement, 0);
			}
			else if( pEdit->getControlState() == I3GUI_CONTROL_STATE_CLICKED)		// 마우스로 선택되어진 Edit를 반환합니다.
			{
				m_nSelectedElement = i;

				setControlState(I3GUI_CONTROL_STATE_CLICKED);
				PushControlNotify(I3GUI_POPUPLIST_NOTIFY_CLICKED, m_nSelectedElement, 0);

				// 컴보박스용 팝업리스트의 경우 선택되어지면 닫는다
				if (m_bExclusiveEnableControl)
				{
					SetFocus(false);
				}
			}
			else if (pEdit->getControlState() == I3GUI_CONTROL_STATE_R_CLICKED)
			{
				m_nSelectedElement = i;

				setControlState(I3GUI_CONTROL_STATE_R_CLICKED);
				PushControlNotify(I3GUI_POPUPLIST_NOTIFY_RCLICK, m_nSelectedElement, 0);

			}
			else if( pEdit->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)	// 마우스가 올려져있는 Edit의 선택셀을 화면에 표시합니다.
			{
				m_pSpriteSel->SetEnable( 0, true);
				
				{
					m_pSpriteSel->SetRect( 0, getTotalPosX() + pEdit->getPositionX() * i3GuiRoot::getGuiResolutionWidth(), getTotalPosY() + pEdit->getPositionY()*i3GuiRoot::getGuiResolutionHeight(), (REAL32)pEdit->getWidth(), (REAL32)pEdit->getHeight());
				}
				
				PushControlNotify(I3GUI_POPUPLIST_NOTIFY_ONMOUSE, i, 0);
				break;
			}
			else
			{
				m_pSpriteSel->SetEnable( 0, false);
			}
		}
	}
}


INT32 i3GuiPopupList::GetTextHeight(void)
{
	INT32 nHeight = 0;

	for(size_t j = 0; j < m_ControlList.size(); j++)
	{
		i3GuiEditBox* edit = m_ControlList[j];

		INT32 controlHeight = edit->getTextNodeDX()->getTextHeight();

		if( nHeight < controlHeight) nHeight = controlHeight;
	}

	return nHeight; 
}

INT32 i3GuiPopupList::GetTextWidth(void)
{
	INT32 nWidth = 0;

	for(size_t j = 0; j < m_ControlList.size(); j++)
	{
		i3GuiEditBox* edit = m_ControlList[j];

		INT32 controlWidth = edit->getTextNodeDX()->getTextWidth();

		if( nWidth < controlWidth) nWidth = controlWidth;
	}

	return nWidth;
}

void i3GuiPopupList::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiPopupList*>(pDest))
		return ;

	i3GuiPopupList * pPopupList = (i3GuiPopupList*)pDest;

	pPopupList->setControlState( m_ControlState );
}

void i3GuiPopupList::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUIPOPUPLIST
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiPopupList::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIPOPUPLIST data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiPopupList::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiPopupList::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIPOPUPLIST data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiPopupList::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

bool i3GuiPopupList::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	// 복합 컨트롤의 경우, 내부 컴포넌트와 외부 컬랙션 컨트롤간의 이벤트 전달에 시차가 발생합니다.
	// 따라서, 내부 컴보넌트의 UserInput 발생시, 외부 컬랙션의 OnEvent를 호출하여 즉시 처리하도록 합니다.

	UpdateEditControl();

	return true;
}

const i3::rc_wstring& i3GuiPopupList::GetText(UINT32 idxElement) const
{
	return m_ElementList[idxElement];
}

void i3GuiPopupList::SetText(UINT32 idxElement, const i3::rc_wstring& wstr)
{
	I3ASSERT(idxElement < (UINT32)m_nElementCount && "Idx Overflower");

	i3::vu::set_value_force(m_ElementList, idxElement, wstr);
}

void i3GuiPopupList::SetTextColor(const COLOR* pColor)
{
	for(INT32 i = 0; i < m_nControlCount; i++)
	{
		i3GuiEditBox* pEdit = m_ControlList[i];
		pEdit->SetTextColor((COLOR*)pColor);
	}
}

void i3GuiPopupList::SetTextColor(INT32 idxElement,const COLOR* pColor)
{
	I3ASSERT(idxElement < m_nControlCount && "Idx Overflower");

	i3GuiEditBox* pEdit = m_ControlList[idxElement];
	pEdit->SetTextColor((COLOR*)pColor);
}

void i3GuiPopupList::Clear(void)
{
	m_ElementList.clear();
	m_nElementCount = 0;
	m_nSelectedElement = 0;
}

void i3GuiPopupList::SetFixedElementCount(bool bFixed)
{
	m_bFixedElementCount = bFixed;
}

bool i3GuiPopupList::GetFixedElementCount(void) const
{
	return m_bFixedElementCount;
}

bool i3GuiPopupList::_ResizeControl(void)
{
	bool rv = i3GuiControl::_ResizeControl();

	_ResizePopUp();

	return rv;
}

void i3GuiPopupList::_ResizePopUp(void)
{
	INT32 i = 0;
	REAL32 fX = 0.0f;
	REAL32 fY = 0.0f;
	INT32 nGapY = 0;

	if( m_nControlCount <= 0)
		return;

	if (false == m_bFixedElementCount)
	{
		for( i = 0; i < m_nControlCount; i++)
		{
			if( i < (INT32)m_ControlList.size())
			{
				i3GuiEditBox* pEdit = m_ControlList[i];			

				pEdit->setPosition( fX, fY);
				nGapY = pEdit->getTextNodeDX()->getTextHeight();								
				nGapY = (INT32)(nGapY / i3GuiRoot::getGuiResolutionHeight());	//C4244
				//pEdit->setSize((INT32)(m_nOriginalWidth * i3GuiRoot::getGuiResolutionWidth()), nGapY + OFFSET_HEIGHT);
				pEdit->setSize((INT32)(m_nOriginalWidth), nGapY + OFFSET_HEIGHT);	
				pEdit->setOriginalSize((INT32)(m_nOriginalWidth), nGapY + OFFSET_HEIGHT);	
				pEdit->setOriginalControlSize((INT32)(m_nOriginalWidth), nGapY + OFFSET_HEIGHT);	
				fY += nGapY + OFFSET_HEIGHT;

				pEdit->ResizeText();	
			}
		}
	}
	else
	{
		//INT32 width = (INT32)(m_nOriginalWidth * i3GuiRoot::getGuiResolutionWidth());
		//INT32 height = (INT32)(m_nOriginalHeight * i3GuiRoot::getGuiResolutionHeight()) / m_nControlCount;
		INT32 width = (INT32)(m_nOriginalWidth);
		INT32 height = (INT32)(m_nOriginalHeight) / m_nControlCount;
		INT32 orginalControlHeight = m_nOriginalHeight / m_nControlCount;

		for(size_t j = 0; j < m_ControlList.size(); j++)
		{
			i3GuiEditBox* edit = m_ControlList[j];
			
			edit->setPosition(0.0f, (REAL32)(orginalControlHeight * j));
			edit->setSize(width, height);			
			edit->setOriginalControlSize(width, height);	
			edit->ResizeText();
		}
	}



	// 컨트롤의 숫자만큼 팝업리스트 배경을 리사이즈 합니다.
	REAL32 fShapeHeightRate = 1.0f;

	if (false == m_bFixedElementCount)
	{	
		fShapeHeightRate = (REAL32)((nGapY+OFFSET_HEIGHT) * m_nControlCount) / (REAL32)(m_nOriginalHeight);
	}

	if( m_pData->m_pShape)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();
		REAL32 rx = i3GuiRoot::getGuiResolutionWidth();
		REAL32 ry = i3GuiRoot::getGuiResolutionHeight();

		for(i=0; i<nShapeCount; i++)
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
	}
}
