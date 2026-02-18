#include "i3FrameworkPCH.h"
#include "i3UIRadioButton.h"

// ============================================================================
//
// i3UIRadioButton
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIRadioButton, i3UIControl );

i3UIRadioButton::i3UIRadioButton()
{
}

i3UIRadioButton::~i3UIRadioButton()
{
}


void i3UIRadioButton::_UpdateShape(void)
{
	INT32 idx = I3UI_RADIOBUTTON_SHAPE_NORMAL;

	if( getSelected())
	{
		idx = I3UI_RADIOBUTTON_SHAPE_PUSH_NORMAL;

		if( isControlDisabled())
		{
			if( GetShapeEnable( I3UI_RADIOBUTTON_SHAPE_PUSH_DISABLED))
				idx = I3UI_RADIOBUTTON_SHAPE_PUSH_DISABLED;
		}
		else if( isControlState( I3UI_CTRL_STATE_ONMOUSE))
		{
			if( GetShapeEnable( I3UI_RADIOBUTTON_SHAPE_PUSH_ONMOUSE))
				idx = I3UI_RADIOBUTTON_SHAPE_PUSH_ONMOUSE;
		}
	}
	else
	{
		if( isControlDisabled())
		{
			if( GetShapeEnable( I3UI_RADIOBUTTON_SHAPE_DISABLED))
				idx = I3UI_RADIOBUTTON_SHAPE_DISABLED;
		}
		else if( isControlState( I3UI_CTRL_STATE_ONMOUSE))
		{
			if( GetShapeEnable( I3UI_RADIOBUTTON_SHAPE_ONMOUSE))
				idx = I3UI_RADIOBUTTON_SHAPE_ONMOUSE;
		}
	}

	INT32 i;

	for( i = 0; i < I3UI_RADIOBUTTON_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, (i == idx));
	}
}

void i3UIRadioButton::setSelected( bool bState)
{
	if( bState)
		addControlState( I3UI_CTRL_STATE_SELECTED);
	else
		removeControlState( I3UI_CTRL_STATE_SELECTED);

	Notify( I3UI_EVT_CHANGED, 0, 0);
}

void i3UIRadioButton::OnBindTemplate(void)
{
	_UpdateShape();
}

void i3UIRadioButton::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	_UpdateShape();
}

void	i3UIRadioButton::OnLButtonUp( UINT32 nFlag, POINT point)
{
	setSelected( true);

	i3UIControl::OnLButtonUp( nFlag, point);
}

void	i3UIRadioButton::OnKeyUp( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			setSelected( true);
			break;
	}

	i3UIControl::OnKeyUp( nKey);
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

typedef struct ALIGN4 _tag_i3Persist_GuiRadioButton
{
	UINT8	m_ID[4];
	UINT32	pad[8];
}I3_PERSIST_UIRADIOBUTTON;

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UIRadioButton::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	I3_PERSIST_UIRADIOBUTTON data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "URB1", 4);

	Rc = pResFile->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIRadioButton::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	I3_PERSIST_UIRADIOBUTTON data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}
