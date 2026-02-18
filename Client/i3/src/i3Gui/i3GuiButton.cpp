#include "i3GuiPCH.h"
#include "i3GuiButton.h"

// ============================================================================
//
// i3GuiButton : 버튼
//
// ============================================================================
//I3_CLASS_INSTANCE( i3GuiButton, i3GuiControl );
I3_CLASS_INSTANCE( i3GuiButton);

static INT32 nId = 0;
i3GuiButton::i3GuiButton()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);

	nId++;
	m_nID = nId;

	setLastEvent( I3GUI_BUTTON_NOTIFY_NULL);
}


void i3GuiButton::SetStateAs( I3GUI_BUTTON_STYLE style)
{
	switch( style)
	{
		case I3GUI_BUTTON_STYLE_PUSH_3STATE :
			// Normal, OnMouse, Push
			SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
			SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, 1);
			SetUserDefine( I3GUI_CONTROL_STATE_CLICKED, 2);
			setInputMode(I3GUI_INPUT_MODE_PRESS);
			break;

		default :
			SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
			SetUserDefine( I3GUI_CONTROL_STATE_CLICKED, 1);
			setInputMode(I3GUI_INPUT_MODE_PRESS);
			break;
	}
}

void i3GuiButton::OnUpdate( REAL32 rDeltaSeconds )
{	
	UpdateRenderObjects();

	if( getInputMode() == I3GUI_INPUT_MODE_STROKE)
	{
		if( getControlState() == I3GUI_CONTROL_STATE_CLICKED)
		{
			setControlState( I3GUI_CONTROL_STATE_NORMAL);
		}
	}

	setLastEvent( I3GUI_BUTTON_NOTIFY_NULL);

	if (m_bDownClickAction)
	{
		if (I3GUI_CONTROL_STATE_CLICKED == m_ControlState)
		{
			setCtrlOffset( 0, 1);
		}
		else
		{
			setCtrlOffset( 0, 0);
		}
	}

	i3GuiControl::OnUpdate( rDeltaSeconds );
}

void i3GuiButton::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiRoot::setCapturedMouseControl( nullptr);
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	if( getInputMode() == I3GUI_INPUT_MODE_STROKE)
	{	// 기존 BUTTON_DOWN에서 작동하는 button code
		// Mouse의 왼쪽 버튼이 클릭되었습니다.
		if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_PRESS))
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);		
			PushControlNotify(I3GUI_BUTTON_NOTIFY_CLICKED, 0, 0);
		}
		// Mouse의 오른쪽 버튼이 클릭되었습니다.
		else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_RBUTTON_PRESS))
		{
			setControlState( I3GUI_CONTROL_STATE_R_CLICKED);
		}
		// Mouse의 중간 버튼이 클릭되었습니다.
		else if( IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_MBUTTON_PRESS))
		{
	//		setControlState( I3GUI_CONTROL_STATE_CLICKED3);
		}
		// Mouse가 버튼위에 올려져 있습니다.
		else
		{
			setControlState(I3GUI_CONTROL_STATE_ONMOUSE);
			PushControlNotify(I3GUI_BUTTON_NOTIFY_ONMOUSE, (INT32)fX, (INT32)fY);
		}

		SetOldInputState(I3I_MOUSE_LBUTTON);
		return;
	}

	if( getInputMode() == I3GUI_INPUT_MODE_REPEAT)
	{
		//if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_REPEAT))
		if (IS_MOUSE_STATE(state, I3GUI_MOUSE_STATE_LBUTTON_PRESS))
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);
			PushControlNotify(I3GUI_BUTTON_NOTIFY_CLICKED, 0, 0);
		}
		else
		{
			setControlState(I3GUI_CONTROL_STATE_ONMOUSE);
			PushControlNotify(I3GUI_BUTTON_NOTIFY_ONMOUSE, (INT32)fX, (INT32)fY);
		}

		return;
	}

	if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_UP))
	{
		if( i3GuiRoot::getCapturedMouseControl() != this ) return;

		i3GuiRoot::setCapturedMouseControl( nullptr);
		if( IsOnMouse( fX, fY))
		{
			setLastEvent( I3GUI_BUTTON_NOTIFY_CLICKED);
			setControlState(I3GUI_CONTROL_STATE_NORMAL);		

			PushControlNotify(I3GUI_BUTTON_NOTIFY_CLICKED, 0, 0);
		}
		return;
	}

	if( !IsOnMouse(fX, fY))
	{		
		setControlState(I3GUI_CONTROL_STATE_NORMAL);
		return;
	}

	//if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_DOWN))
	if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_PRESS))
	{
		if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_DOWN) || IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_LBUTTON_DCLICK))
		{
			i3GuiRoot::setCapturedMouseControl( this);
		}
		
		if( i3GuiRoot::getCapturedMouseControl() == this)
		{
			setControlState(I3GUI_CONTROL_STATE_CLICKED);
			m_pClickedControl = this;
		}		

		return;
	}

	if( IS_MOUSE_STATE( state, I3GUI_MOUSE_STATE_RBUTTON_UP))
	{
		setControlState(I3GUI_CONTROL_STATE_R_CLICKED);
		return;
	}

	setControlState(I3GUI_CONTROL_STATE_ONMOUSE);
	PushControlNotify(I3GUI_BUTTON_NOTIFY_ONMOUSE, (INT32)fX, (INT32)fY);
}

void i3GuiButton::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct GUIBUTTON
	{
		INT32	m_State = 0;
		INT32	m_nID = 0;
	};
}

#if defined( I3_COMPILER_VC)
#if defined(PACKING_RIGHT)
	#pragma pack(pop)				// [test required] 중복 push입니다. pop으로 변경함. 2017.02.09
#else
	#pragma pack(push, 4)
#endif
#endif

UINT32 i3GuiButton::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::GUIBUTTON data;
	data.m_State = getControlState();
	data.m_nID	 = (INT32)m_nID;

	Rc = pStream->Write(&data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiButton::OnSave()", "Could not save button");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiButton::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIBUTTON data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiButton::OnLoad()", "Could not read Button");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;
	m_nID	= data.m_nID;

	return Result;
}

bool i3GuiButton::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3GuiControl::OnSaveXML( pFile, pXML);

	pXML->addAttr( "ControlID", m_nID);

	return bResult;
}

bool i3GuiButton::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3GuiControl::OnLoadXML( pFile, pXML);

	pXML->getAttr( "ControlID", &m_nID);

	return bResult;
}

void i3GuiButton::SetDownClickAction(bool bDownClick)
{
	if (bDownClick)
	{
		m_bClickAction = false;
		m_bDownClickAction = true;
	}
	else
	{
		m_bDownClickAction = false;
	}
}
