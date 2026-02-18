#include "i3GuiPCH.h"
#include "i3GuiTextBox.h"

// ============================================================================
//
// i3GuiTextBox
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiTextBox);

i3GuiTextBox::i3GuiTextBox()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void i3GuiTextBox::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	i3GuiControl::SetControlShapeTemplate( pShape);

	m_nWidth = (INT32) pShape->getImage(0)->m_nWidth;
	m_nHeight = (INT32) pShape->getImage(0)->m_nHeight;
}

// 갱신
void i3GuiTextBox::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiEditBox::OnUpdate( rDeltaSeconds ); 

	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
	{
		if(getTextNode()->getProgressText())
		{
			setControlState(I3GUI_CONTROL_STATE_PROGRESS);
		}
		else
		{
			setControlState( I3GUI_CONTROL_STATE_NORMAL);
		}
	}
}

void i3GuiTextBox::SetTextBox( WCHAR16 * pText)
{
	m_pText = pText;	

	SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	SetTextRectSize(m_nWidth, m_nHeight);
	SetOffsetPos(3.0f, 3.0f);
	
#if defined ( I3G_DX)
	if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX)
	{
		getTextNodeDX()->setAutoWrapEnable(true);
	}
	else if( i3GuiRoot::getTextNodeUsage() == I3GUI_TEXT_USAGE_TYPE_TEXTNODE)
#endif
	{
		getTextNode()->SetAutoWrap(true, 1024);
		getTextNode()->setProgressText(true);
		getTextNode()->setProgressTextScrollSpeed( m_fScrollSpeed);
		getTextNode()->SetPos( (getTotalPosX()+m_fOffsetPosX),  (getTotalPosX()+m_fOffsetPosY));
	}

	getTextNode()->SetText(m_pText);
}

void i3GuiTextBox::SetImmediatelyTextBox()
{
	getTextNode()->SetImmediatelyProgressText();
}

void i3GuiTextBox::ResetTextBox()
{
	m_pText = nullptr;
	getTextNode()->SetText( (const WCHAR16 *) L"");
	getTextNode()->ResetProgressText();
}

void i3GuiTextBox::setScrollSpeed( REAL32 fSpeed)					
{
	m_fScrollSpeed = fSpeed;	
	getTextNode()->setProgressTextScrollSpeed( m_fScrollSpeed);
}

void i3GuiTextBox::StartTextBox()
{
	if(m_pText)
	{
		getTextNode()->StartProgressText(true);
		setControlState(I3GUI_CONTROL_STATE_PROGRESS);
	}
}


void i3GuiTextBox::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	if( getControlDisable()) 
	{		
		i3GuiObjBase::UserInput(fX, fY, state);
		return;
	}

	// Mouse의 왼쪽 버튼이 클릭되었습니다.
	if(state & I3I_MOUSE_LBUTTON)
	{
		if( getControlState() != I3GUI_CONTROL_STATE_PROGRESS)
		{
			m_pClickedControl = this;
			setControlState(I3GUI_CONTROL_STATE_CLICKED);
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
		if(! getTextNode()->getProgressText())
		{
			setControlState(I3GUI_CONTROL_STATE_ONMOUSE);			
		}
	}

//	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiTextBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiTextBox*>(pDest))
		return ;

	i3GuiTextBox * pTextBox = (i3GuiTextBox*)pDest;

	pTextBox->setControlState( m_ControlState );
}

void i3GuiTextBox::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUITEXTBOX
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiTextBox::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUITEXTBOX data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiTextBox::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiTextBox::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUITEXTBOX data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiTextBox::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}
