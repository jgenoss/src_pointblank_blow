#include "i3FrameworkPCH.h"
#include "i3UIImageButton.h"
#include "i3UIManager.h"
#include "i3UITemplate_Button.h"
#include "../i3Framework.h"

// ============================================================================
//
// i3UIImageButton : 버튼
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIImageButton, i3UIControl );

i3UIImageButton::i3UIImageButton()
{
}

i3UIImageButton::~i3UIImageButton()
{
}

void i3UIImageButton::_UpdateShape(void)
{
	INT32 i, idxBtn, idxIcon;

	idxBtn = I3UI_IMAGEBUTTON_SHAPE_BTN_NORMAL;
	idxIcon = I3UI_IMAGEBUTTON_SHAPE_ICON_NORMAL;

	if( isControlDisabled())
	{
		if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_BTN_DISABLED))
			idxBtn = I3UI_IMAGEBUTTON_SHAPE_BTN_DISABLED;

		if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_ICON_DISABLED))
			idxIcon = I3UI_IMAGEBUTTON_SHAPE_ICON_DISABLED;
	}
	else
	{
		if( isControlSelected())
		{
			if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_BTN_PUSHED))
				idxBtn = I3UI_IMAGEBUTTON_SHAPE_BTN_PUSHED;

			if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_ICON_PUSHED))
				idxIcon = I3UI_IMAGEBUTTON_SHAPE_ICON_PUSHED;
		}
		else if( isControlState( I3UI_CTRL_STATE_ONMOUSE))
		{
			if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_BTN_ONMOUSED))
				idxBtn = I3UI_IMAGEBUTTON_SHAPE_BTN_ONMOUSED;

			if( GetShapeEnable( I3UI_IMAGEBUTTON_SHAPE_ICON_ONMOUSED))
				idxIcon = I3UI_IMAGEBUTTON_SHAPE_ICON_ONMOUSED;
		}
	}

	for( i = 0; i < I3UI_IMAGEBUTTON_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, ( i == idxBtn) || ( i == idxIcon));
	}
}

void i3UIImageButton::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	_UpdateShape();
}

void i3UIImageButton::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDown( nFlag, point);

	addControlState( I3UI_CTRL_STATE_SELECTED);

	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);

	_UpdateShape();

	// 마우스 입력을 Capture한다.
	i3Framework::SetCapture( this);
}

void i3UIImageButton::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonUp( nFlag, point);

	// 마우스 입력을 Capture 해제한다.
	i3Framework::ReleaseCapture();

	removeControlState( I3UI_CTRL_STATE_SELECTED);

	_UpdateShape();
}

void i3UIImageButton::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UIControl::OnEnterMouse( x, y, keys);

	_UpdateShape();

	if( isControlState( I3UI_CTRL_STATE_FOCUSED))
	{
		ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);
	}
}

void i3UIImageButton::OnLeaveMouse(void)
{
	i3UIControl::OnLeaveMouse();

	// 이유를 불문하고 마우스 커서가 바깥으로 나갔다면
	// 눌려진 상태를 제거해야 한다.
	removeControlState( I3UI_CTRL_STATE_SELECTED);

	_UpdateShape();
}

void i3UIImageButton::OnKeyDown( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			addControlState( I3UI_CTRL_STATE_SELECTED);

			_UpdateShape();
			break;
	}

	i3UIControl::OnKeyDown( nKey);
}

void i3UIImageButton::OnKeyUp( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			removeControlState( I3UI_CTRL_STATE_SELECTED);

			Notify( I3UI_EVT_CLICKED, 0, NULL);
			_UpdateShape();
			break;
	}

	i3UIControl::OnKeyUp( nKey);
}

void i3UIImageButton::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

typedef struct _tagi3PersistUIImageButton
{
	INT8		m_ID[4];
	UINT32		pad[8];
}I3_PERSIST_UIIMAGEBUTTON;

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif
UINT32 i3UIImageButton::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	I3_PERSIST_UIIMAGEBUTTON data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof( data));
	i3mem::Copy( data.m_ID, "UIB1", 4);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIImageButton::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_UIIMAGEBUTTON data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

bool i3UIImageButton::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3UIImageButton::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	return bResult;
}
