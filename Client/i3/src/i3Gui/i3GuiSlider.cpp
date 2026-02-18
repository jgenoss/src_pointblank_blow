#include "i3GuiPCH.h"
#include "i3GuiSlider.h"

// ============================================================================
//
// i3GuiSlider
//
// ============================================================================

I3_CLASS_INSTANCE( i3GuiSlider);

i3GuiSlider::i3GuiSlider()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
	setInputMode( I3GUI_INPUT_MODE_PRESS);
}

void i3GuiSlider::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	m_pShape = pShape;

	i3GuiControl::SetControlShapeTemplate( pShape);

	//
	if( m_bToWidth)
	{
		if(pShape->getImage(0)->m_nX < 0) pShape->getImage(0)->m_nX = 0;
		m_fMinValue = (REAL32)pShape->getImage(0)->m_nX;
		m_fMaxValue = m_fMinValue + pShape->getImage(0)->m_nWidth - pShape->getImage(1)->m_nWidth;
	
		m_fSliderScale = m_fMaxValue * 0.01f;
		m_fSliderPos = m_fSliderScale * m_fCurrentValue;
		m_pData->m_pShape->getImage(1)->m_nX = (m_fSliderPos); 
	}
	else
	{
		if(pShape->getImage(0)->m_nY < 0) pShape->getImage(0)->m_nY = 0;
		m_fMinValue = (REAL32)pShape->getImage(0)->m_nY;
		m_fMaxValue = m_fMinValue + pShape->getImage(0)->m_nHeight - pShape->getImage(1)->m_nHeight;
	
		m_fSliderScale = m_fMaxValue * 0.01f;
		m_fSliderPos = m_fSliderScale * m_fCurrentValue;
		m_pData->m_pShape->getImage(1)->m_nY = (m_fSliderPos); 
	}
}

void i3GuiSlider::setToWidth( bool bTrue)
{
	m_bToWidth = bTrue;		

	if( bTrue)
	{
		if(m_pShape->getImage(0)->m_nX < 0) m_pShape->getImage(0)->m_nX = 0;
		m_fMinValue = (REAL32)m_pShape->getImage(0)->m_nX;
		m_fMaxValue = m_fMinValue + m_pShape->getImage(0)->m_nWidth - m_pShape->getImage(1)->m_nWidth;
	
		m_fSliderScale = m_fMaxValue * 0.01f;
		m_fSliderPos = m_fSliderScale * m_fCurrentValue;
		m_pData->m_pShape->getImage(1)->m_nX = (m_fSliderPos); 
	}
	else
	{
		if(m_pShape->getImage(0)->m_nY < 0) m_pShape->getImage(0)->m_nY = 0;
		m_fMinValue = (REAL32)m_pShape->getImage(0)->m_nY;
		m_fMaxValue = m_fMinValue + m_pShape->getImage(0)->m_nHeight - m_pShape->getImage(1)->m_nHeight;

		m_fSliderScale = m_fMaxValue * 0.01f;
		m_fSliderPos = m_fSliderScale * m_fCurrentValue;
		m_pData->m_pShape->getImage(1)->m_nY = (m_fSliderPos); 
	}	
}

// 갱신
void i3GuiSlider::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiControl::OnUpdate( rDeltaSeconds ); 

	switch(m_ControlState)
	{
	case I3GUI_CONTROL_STATE_NORMAL:
		ResetEvent();
		break;
	case I3GUI_CONTROL_STATE_ONMOUSE:
		break;
	case I3GUI_CONTROL_STATE_DISABLED:
		break;
	case I3GUI_CONTROL_STATE_DRAG:		
		UpdatePos();
		break;
	}	

	{
		if( m_bToWidth)
		{
			m_fMinValue = (REAL32)m_pShape->getImage(0)->m_nX;
			m_fMaxValue = m_fMinValue + m_pShape->getImage(0)->m_nWidth - m_pShape->getImage(1)->m_nWidth;
			m_fSliderScale = m_fMaxValue * 0.01f;
		}
		else
		{
			m_fMinValue = (REAL32)m_pShape->getImage(0)->m_nY;
			m_fMaxValue = m_fMinValue + m_pShape->getImage(0)->m_nHeight - m_pShape->getImage(1)->m_nHeight;
			m_fSliderScale = m_fMaxValue * 0.01f;
		}
	}

	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}
}

void i3GuiSlider::setCurValue( REAL32 fValue)				
{
	m_fCurrentValue = fValue;		
	m_fSliderPos = m_fSliderScale * m_fCurrentValue;
	UpdatePos();
}

void i3GuiSlider::SetSliderPos( REAL32 fPos)	
{
	m_fSliderPos = m_fSliderScale * fPos;	
	UpdatePos();
}

void i3GuiSlider::UpdatePos()
{
	m_pClickedControl = this;

	if( m_fSliderPos < m_fMinValue ) m_fSliderPos = m_fMinValue;
	if( m_fSliderPos > m_fMaxValue ) m_fSliderPos = m_fMaxValue;

	if(m_fSliderPos>=m_fMinValue && m_fSliderPos<=m_fMaxValue)
	{
		if( m_bToWidth)
		{
			m_pData->m_pShape->getImage(1)->m_nX = (m_fSliderPos);
			m_fCurrentValue = m_fSliderPos / m_fSliderScale;
		}
		else
		{
			m_pData->m_pShape->getImage(1)->m_nY = (m_fSliderPos);
			m_fCurrentValue = m_fSliderPos / m_fSliderScale;
		}
	}
}

void i3GuiSlider::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{	
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{
		if( m_bToWidth)
		{
			m_fSliderPos = fX - getTotalPosX() - (m_pShape->getImage(1)->m_nWidth/2);
		}
		else
		{
			m_fSliderPos = fY - getTotalPosY() - (m_pShape->getImage(1)->m_nHeight/2);
		}
		setControlState(I3GUI_CONTROL_STATE_DRAG);
		PushControlNotify(I3GUI_SLIDER_NOTIFY_DRAG, 0, 0);

		i3GuiRoot::setCapturedMouseControl( this);

		SetFocus(true);
	}
	else
	{
		setControlState(I3GUI_CONTROL_STATE_NORMAL);
		PushControlNotify(I3GUI_SLIDER_NOTIFY_NULL, 0, 0);

		if( i3GuiRoot::getCapturedMouseControl() == this)
		{
			i3GuiRoot::setCapturedMouseControl(nullptr);
		}
	}

//	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiSlider::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiSlider*>(pDest))
		return ;

	i3GuiSlider * pSlider = (i3GuiSlider*)pDest;

	pSlider->setControlState( m_ControlState );
}

void i3GuiSlider::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUISLIDER
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiSlider::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUISLIDER data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiSlider::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiSlider::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUISLIDER data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiSlider::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}
