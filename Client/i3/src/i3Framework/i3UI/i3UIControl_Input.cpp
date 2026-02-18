#include "i3FrameworkPCH.h"
#include "i3UIControl.h"
#include "../i3Framework.h"

void	i3UIControl::OnCreate(void)
{
}

bool	i3UIControl::OnActivate( bool bActive)
{
//	if( isState( I3UI_STATE_FOCUSED))
//	{
//		i3Framework::SetCurrentFocus( nullptr);
//	}

	// UIManager에게서만 호출될 수 있다.
	if( bActive)
	{
		removeState( I3UI_STATE_DEACTIVATED);

		Notify( I3UI_EVT_ACTIVATE, 0, 0);
	}
	else
	{
		addState( I3UI_STATE_DEACTIVATED);

		Notify( I3UI_EVT_INACTIVATE, 0, 0);
	}

	return true;
}

void	i3UIControl::OnBindTemplate(void)
{
	if( m_pTemplate == nullptr)
	{
		m_hashTemplate = 0;
		return;
	}

	//따로 계산 안하고 있는거 그대로 가져다 쓴다.
	m_hashTemplate = m_pTemplate->getChecksum();
}

void	i3UIControl::OnShow( bool bShow)
{
	if( isState( I3UI_STATE_FOCUSED))
	{
		i3Framework::SetCurrentFocus( nullptr);
	}

	if( bShow)
		removeState( I3UI_STATE_HIDED);
	else
		addState( I3UI_STATE_HIDED);
}

void	i3UIControl::OnEnabled( bool bEnable)
{
	if( isState( I3UI_STATE_FOCUSED) && bEnable == false)		// 불활성화시에만 포커스를 제거..하는 것으로 수정( 11.08.09.수빈)
	{
		i3GameNode * pParent = getParent();
		while( pParent)
		{
			if( i3::same_of<i3UIFrameWnd* >(pParent))
			{// framewnd의 parent가 scene..
				if( ((i3UIFrameWnd*)pParent)->isTopFrameWnd())
					break;
			}

			pParent = pParent->getParent();
		}

		if (pParent != nullptr &&
			pParent->isEnable() == false)
		{
			if ( i3Framework::GetCurrentFocus() == pParent)
			{
			//	i3Framework::SetCurrentFocus( nullptr);
				i3UI::SetFocusControl(nullptr);
			}
		}
	}

	if( bEnable)
	{
		Notify( I3UI_EVT_ENABLE, 0, 0);
	}
	else
	{
		Notify( I3UI_EVT_DISABLE, 0, 0);
	}
}

void i3UIControl::OnMove( REAL32 x, REAL32 y)
{
	Notify( I3UI_EVT_MOVE, 0, 0);
}

void i3UIControl::OnSize( REAL32 cx, REAL32 cy)
{
	if( m_pTemplate != nullptr)
	{
		m_pTemplate->AdjustLayout( cx, cy, m_pRects, m_nShapeCount);
	}

	Notify( I3UI_EVT_SIZE, (INT32)cx, (INT32)cy);
}


i3UIControl* i3UIControl::OnSelect( void)
{
	return this;
}


bool i3UIControl::OnKillFocus( i3UIControl * pByCtrl)
{
//	I3TRACE("--------OnKillFocus: %s\n", GetMeta()->GetClassName());

	removeState( I3UI_STATE_FOCUSED);

	Notify( I3UI_EVT_KILLFOCUS, (INT32)pByCtrl, 0);
	
	return true;
}

bool	i3UIControl::OnSetFocus(void)
{
//	I3TRACE("--------OnSetFocus: %s\n", GetMeta()->GetClassName());

	addState( I3UI_STATE_FOCUSED);

	Notify( I3UI_EVT_SETFOCUS, 0, 0);

	return true;
}

void	i3UIControl::OnMouseMove( UINT32 nFlag, POINT point)
{
	s_CursorPos = point;

	if( isState(I3UI_STATE_LBTN_DRAG))
	{
		OnDragging( point.x, point.y);
	}
}

void	i3UIControl::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	addState( I3UI_STATE_ONMOUSE | I3UI_STATE_NEED_UPDATE);

	Notify( I3UI_EVT_ENTERMOUSE, 0, 0);

	m_bHasTtTimer = true;
}

void	i3UIControl::OnLeaveMouse(void)
{
	removeState( I3UI_STATE_ONMOUSE);

	Notify( I3UI_EVT_LEAVEMOUSE, 0, 0);

	_LoseTimer();	

	addState( I3UI_STATE_NEED_UPDATE);
}

void	i3UIControl::OnClicked( REAL32 x, REAL32 y)
{
	//I3TRACE("i3UIControl::OnClicked: %s\n", GetMeta()->GetClassName());

	Notify( I3UI_EVT_CLICKED, (INT32)x, (INT32)y);
}

void	i3UIControl::OnDblClicked(void)
{
	Notify( I3UI_EVT_DBL_CLICKED, -1, -1);
}

void	i3UIControl::OnRClicked(void)
{
	Notify( I3UI_EVT_R_CLICKED, -1, -1);
}

void	i3UIControl::OnRDblClicked(void)
{
	Notify( I3UI_EVT_R_DBL_CLICKED, -1, -1);
}

void	i3UIControl::OnMapCoord( POINT * pPoint)
{
	i3UIManager * pManager = i3UI::getManager();

	pManager->ConvertMouseCoord( pPoint);
}

void	i3UIControl::OnLButtonDown( UINT32 nFlag, POINT point)
{
	addState( I3UI_STATE_LBTN_DRAG);

	_LoseTimer();

	OnDragStart( point.x, point.y);
}

void	i3UIControl::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if( isState( I3UI_STATE_LBTN_DRAG))
	{
		removeState( I3UI_STATE_LBTN_DRAG);

		OnDragEnd( point.x, point.y);
	}
	
	REAL32 x = m_AccPos.x;
	REAL32 y = m_AccPos.y;
	REAL32 w = m_Size.x;
	REAL32 h = m_Size.y;

	if( point.x > x && point.x < x + w)
	{
		if( point.y > y && point.y < y + h)
		{
			OnClicked( (REAL32)point.x, (REAL32)point.y);
		}
	}
}

void	i3UIControl::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	OnDblClicked();
}

void	i3UIControl::OnRButtonDown( UINT32 nFlag, POINT point)
{
	addState( I3UI_STATE_RBTN_DRAG);

	_LoseTimer();

	OnDragStart( point.x, point.y);
}

void	i3UIControl::OnRButtonUp( UINT32 nFlag, POINT point)
{
	if( isState( I3UI_STATE_RBTN_DRAG))
	{
		removeState( I3UI_STATE_RBTN_DRAG);

		OnDragEnd( point.x, point.y);
	}
	
	REAL32 x = m_AccPos.x;
	REAL32 y = m_AccPos.y;
	REAL32 w = m_Size.x;
	REAL32 h = m_Size.y;

	if( point.x > x && point.x < x + w)
	{
		if( point.y > y && point.y < y + h)
		{
			OnRClicked();
		}
	}
}

void	i3UIControl::OnRButtonDblClk( UINT32 nFlag, POINT point)
{
	OnRDblClicked();
}

void	i3UIControl::OnMButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3UIControl::OnMButtonUp( UINT32 nFlag, POINT point)
{
	Notify( I3UI_EVT_M_CLICKED, 0, 0);
}

void	i3UIControl::OnMButtonDblClk( UINT32 nFlag, POINT point)
{
	Notify( I3UI_EVT_M_DBL_CLICKED, 0, 0);
}

void	i3UIControl::OnMouseWheel( UINT32 nFlag, short zDelta, POINT point)
{
	//I3TRACE("i3UIControl::OnMouseWheel\n");
	Notify( I3UI_EVT_MOUSEWHEEL, (INT32)zDelta, 0, true);
}

bool	i3UIControl::OnDragStart( INT32 x, INT32 y)
{
	Notify( I3UI_EVT_DRAGSTART, x, y, true);

	return true;
}

bool	i3UIControl::OnDragging( INT32 x, INT32 y)
{
	Notify( I3UI_EVT_DRAGGING, x, y, true);
	return true;
}

bool	i3UIControl::OnDragEnd( INT32 x, INT32 y)
{
	Notify( I3UI_EVT_DRAGEND, x, y, true);

	return true;
}

