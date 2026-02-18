#include "i3FrameworkPCH.h"
#include "i3UIListView_Sep.h"
#include "i3UITemplate_LVSep.h"

#include "i3UIListView_Item.h"
#include "i3UIListView_Header.h"

I3_CLASS_INSTANCE( i3UIListView_Sep);

i3UIListView_Sep::i3UIListView_Sep()
{
	SetPosNoNotify( 5.0f, getHeight());
}

void i3UIListView_Sep::_UpdateShapeState(void)
{
	setShapeVisible( I3UI_LVSEP_SHAPE_SEPERATOR, true);
}

bool i3UIListView_Sep::isPtInRect( POINT point)
{
	VEC3D* vPos = getPos();	
	REAL32 x = vPos->x;
	REAL32 y = vPos->y;

	if( point.x >= x && point.x <= x + getWidth())
	{
		if( point.y >= y && point.y <= y + getHeight())
		{
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
void i3UIListView_Sep::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDown( nFlag, point);

	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{
		if( i3::same_of<i3UIListView_Item*>(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			pItem->OnLButtonDown( nFlag, point);
		}
		else if( i3::same_of<i3UIListView_Header*>(pParent))
		{
			ParentNotify( I3UI_EVT_DRAGSTART, point.x, point.y);
		}
	}
}

void i3UIListView_Sep::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDblClk( nFlag, point);

	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{
		if( i3::same_of<i3UIListView_Item*>(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			pItem->OnLButtonDblClk( nFlag, point);
		}
	}
}

void i3UIListView_Sep::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonUp( nFlag, point);

	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{
		if( i3::same_of<i3UIListView_Item* >(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			pItem->OnLButtonUp( nFlag, point);
		}
		else if( i3::same_of<i3UIListView_Header* >(pParent))
		{
			ParentNotify( I3UI_EVT_DRAGEND, point.x, point.y);
		}
	}
}

// 포커스(?)가 없으면 안들어온다.
void i3UIListView_Sep::OnMouseMove( UINT32 nFlag, POINT point)
{
	i3UIControl::OnMouseMove( nFlag, point);

	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{
		if( i3::same_of<i3UIListView_Item* >(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			pItem->OnMouseMove( nFlag, point);
		}
	}
}

void i3UIListView_Sep::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UIControl::OnEnterMouse( x, y, keys);

	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{
		if( i3::same_of<i3UIListView_Item*>(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			pItem->OnEnterMouse( x, y, keys);
		}
	}
}

void i3UIListView_Sep::OnLeaveMouse( void)
{
	i3UIControl::OnLeaveMouse();

	i3GameNode* pParent = getParent();
	if( pParent != nullptr && i3::same_of< i3UIListView_Item* >(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnLeaveMouse();
	}
}

i3UIControl* i3UIListView_Sep::OnSelect( void)
{
	i3GameNode* pParent = getParent();
	if( pParent != nullptr)
	{ 
		if( i3::same_of<i3UIListView_Item* >(pParent))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
			return pItem;
		}
		else if( i3::same_of<i3UIListView_Header* >(pParent))
		{
			i3UIListView_Header* pHeader = (i3UIListView_Header*)pParent;
			return pHeader;
		}
	}

	return this;
}
