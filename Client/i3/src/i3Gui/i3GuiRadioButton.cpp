#include "i3GuiPCH.h"
#include "i3GuiRadioButton.h"

// ============================================================================
//
// i3GuiRadioButton
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiRadioButton);

i3GuiRadioButton::i3GuiRadioButton()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void i3GuiRadioButton::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	i3GuiControl::SetControlShapeTemplate( pShape);

	// 총 Radio Button의 갯수입니다. (2이면 Radio Button이 2개임)
	m_ButtonCount = m_pData->m_pShape->getShapeCount();
}

// 갱신
void i3GuiRadioButton::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiControl::OnUpdate( rDeltaSeconds ); 

	
	{
		i3GuiImage* pImage = m_pData->m_pShape->getImage(m_CheckedIndex);
		m_pData->m_pShape->getImage(m_ButtonCount-1)->m_nX = pImage->m_nX;
		m_pData->m_pShape->getImage(m_ButtonCount-1)->m_nY = pImage->m_nY;
	}

	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}
}

void i3GuiRadioButton::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	INT32 i = 0;
	i3GuiImage* pImage = nullptr;
	INT32 nPosX = 0, nPosY = 0;

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{	
		setControlState(I3GUI_CONTROL_STATE_CLICKED);
		PushControlNotify(I3GUI_RADIO_NOTIFY_CLICKED, 0, 0);		
		m_pClickedControl = this;

		for(i=0; i<m_ButtonCount-1; i++)
		{
			pImage = m_pData->m_pShape->getImage(i);
			nPosX = (INT32)(m_fStartX + pImage->m_nX);
			nPosY = (INT32)(m_fStartY + pImage->m_nY);
			if( (nPosX <= fX && nPosX+pImage->getWidth() >= fX) &&
				(nPosY <= fY && nPosY+pImage->getHeight() >= fY) )
			{
				m_CheckedIndex = i;
			}
		}
	} 

//	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiRadioButton::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiRadioButton*>(pDest))
		return ;

	i3GuiRadioButton * pRadioButton = (i3GuiRadioButton*)pDest;

	pRadioButton->setControlState( m_ControlState );
}

void i3GuiRadioButton::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUIRADIOBUTTON
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiRadioButton::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIRADIOBUTTON data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiRadioButton::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiRadioButton::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIRADIOBUTTON data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiRadioButton::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}
