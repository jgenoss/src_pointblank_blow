#include "i3FrameworkPCH.h"
#include "i3UISocketBox.h"
#include "i3UIManager.h"
#include "i3UITemplate_SocketBox.h"
#include "../i3Framework.h"

// ============================================================================
//
// i3UIButton : 버튼
//
// ============================================================================

I3_CLASS_INSTANCE( i3UISocketBox);

void i3UISocketBox::_UpdateShapeState(void)
{
	i3UIControl::_UpdateShapeState();

	INT32 i, idx;

	idx = I3UI_SOCKET_SHAPE_NORMAL;

	if( !isActivated())
	{
		idx = I3UI_SOCKET_SHAPE_DISABLED;
	}
	else
	{
		if( isSelected())
		{
			idx = I3UI_SOCKET_SHAPE_PUSHED;
		}
		else if( isState( I3UI_STATE_ONMOUSE))
		{
			idx = I3UI_SOCKET_SHAPE_ONMOUSED;
		}
	}

	// 텍스트의 위치 조정
	{
		i3UIShape * pShape = getShape( idx);

		if( m_pText != nullptr)
		{
			m_pText->SetScissorsRect(m_rtScissors);
			m_pText->SetViewRect((INT32)pShape->getPosX(), (INT32)pShape->getPosY(), (INT32)pShape->getWidth(), (INT32)pShape->getHeight());
		}
	}

	for( i = 0; i < I3UI_SOCKET_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, ( i == idx));
	}
}

void i3UISocketBox::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UISocketBox::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDown( nFlag, point);

	addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);

	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);

	// 마우스 입력을 Capture한다.
	i3Framework::SetCapture( this);
}

void i3UISocketBox::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnRButtonDown( nFlag, point);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UISocketBox::OnLButtonUp( UINT32 nFlag, POINT point)
{
	// 마우스 입력을 Capture 해제한다.
	i3Framework::ReleaseCapture();

	removeState( I3UI_STATE_SELECTED);

	addState( I3UI_STATE_NEED_UPDATE);

	i3UIControl::OnLButtonUp( nFlag, point);
}

void i3UISocketBox::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UIControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);

	if( isState( I3UI_STATE_FOCUSED))
	{
		ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);
	}
}

void i3UISocketBox::OnLeaveMouse(void)
{
	i3UIControl::OnLeaveMouse();

	// 이유를 불문하고 마우스 커서가 바깥으로 나갔다면
	// 눌려진 상태를 제거해야 한다.
	removeState( I3UI_STATE_SELECTED);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UISocketBox::OnKeyDown( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);
			break;
	}

	i3UIControl::OnKeyDown( nKey);
}

void i3UISocketBox::OnKeyUp( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			removeState( I3UI_STATE_SELECTED);

			Notify( I3UI_EVT_CLICKED, 0, 0);

			addState( I3UI_STATE_NEED_UPDATE);
			break;
	}

	i3UIControl::OnKeyUp( nKey);
}

void i3UISocketBox::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UISOCKETBOX
	{
		INT8		m_ID[4] = { 'U', 'B', 'T', '1' };
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#if defined(PACKING_RIGHT)
	#pragma pack(pop)					// [test required] push를 pop으로 변경. 중복 push를 하는 문제가 있습니다. 여기서 pop하면 다음 패킹 구조에서 스텍이 어떻게 될지 모릅니다. 2017.02.09 soon9
#else
	#pragma pack(push, 4)
#endif
#endif

UINT32 i3UISocketBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UISOCKETBOX data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UISocketBox::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UISOCKETBOX data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

bool i3UISocketBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3UISocketBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	return bResult;
}
