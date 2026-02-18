#include "i3GuiPCH.h"
#include "i3GuiCheckBox.h"

// ============================================================================
//
// i3GuiCheckBox
//
// ============================================================================

I3_CLASS_INSTANCE( i3GuiCheckBox);

i3GuiCheckBox::i3GuiCheckBox()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}


void i3GuiCheckBox::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	i3GuiControl::SetControlShapeTemplate( pShape);

	m_nCheckIdx = m_pData->m_pShape->getShapeCount() - 1;
	if( m_nCheckIdx < 0) m_nCheckIdx = 0;

	if( !m_bCheck)
	{
		m_pData->m_pShape->getImage( m_nCheckIdx)->setEnable(false);
	}
}

void i3GuiCheckBox::setCheckBoxState( bool bState)
{ 
	m_bCheck = bState;
	m_pData->m_pShape->getImage(m_nCheckIdx)->setEnable(m_bCheck);
}

// 갱신
void i3GuiCheckBox::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiControl::OnUpdate( rDeltaSeconds ); 

	UpdateRenderObjects();

	if( getControlState() == I3GUI_CONTROL_STATE_CLICKED)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}

	if (getControlState() == I3GUI_CONTROL_STATE_FOCUS)
	{
		setControlState( I3GUI_CONTROL_STATE_NORMAL);
	}
}

void i3GuiCheckBox::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{
		setControlState( I3GUI_CONTROL_STATE_CLICKED);
		PushControlNotify(I3GUI_CHECK_NOTIFY_CLICKED, !m_bCheck, 0);
		m_pClickedControl = this;

		if( getIsFocus())
		{
			setCheckBoxState(!m_bCheck);
		}
	}
	// Mouse의 오른쪽 버튼이 클릭되었습니다.
	else if(state & I3I_MOUSE_RBUTTON)
	{
//		setControlState( I3GUI_CONTROL_STATE_CLICKED2);
	}
	// Mouse의 중간 버튼이 클릭되었습니다.
	else if(state & I3I_MOUSE_MBUTTON)
	{
//		setControlState( I3GUI_CONTROL_STATE_CLICKED3);
	}
	// Mouse가 버튼위에 올려져 있습니다.
	else
	{
		setControlState(I3GUI_CONTROL_STATE_FOCUS);		
		PushControlNotify(I3GUI_CHECK_NOTIFY_ONMOUSE, 0, 0);
	}

//	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiCheckBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of< i3GuiCheckBox* >(pDest))
		return ;

	i3GuiCheckBox * pCheckBox = (i3GuiCheckBox*)pDest;

	pCheckBox->setControlState( m_ControlState );
}

void i3GuiCheckBox::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUICHECKBOX
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiCheckBox::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUICHECKBOX data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiCheckBox::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiCheckBox::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUICHECKBOX data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiCheckBox::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}

bool i3GuiCheckBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3GuiControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3GuiCheckBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return i3GuiControl::OnLoadXML( pFile, pXML);
}

void i3GuiCheckBox::SetShapeEnable( INT32 index, bool bEnable)
{
	i3GuiControl::SetShapeEnable(index, bEnable);

	m_pData->m_pShape->getImage(m_nCheckIdx)->setEnable(m_bCheck);
}
