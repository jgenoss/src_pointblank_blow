#include "pch.h"

#include "UI/ItemImageMgr.h"
#include "UI/UIMainFrame_V11.h"

#include "UI_V11/UISlotInvenBase_V11.h"
#include "UI_V11/UIPhaseInventory_V11.h"

#include "UserInfoContext.h"

static UIPhaseInventory_V11 * GetInventory( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	UIPhaseSubBase * pCurPhase = pFrame->GetCurrentSubPhase();
	I3ASSERT( i3::same_of<UIPhaseInventory_V11*>(pCurPhase));

	return (UIPhaseInventory_V11*) pCurPhase;
}

extern "C" {
	int UISlotInvenBase_V11_ClickDetail( LuaState * L)
	{
		GetInventory()->OnSelect( L);
		GetInventory()->OnDetailInfo();
		return 0;
	}

	int UISlotInvenBase_V11_ClickSlot( LuaState * L)
	{
		return 0;
	}

	int UISlotInvenBase_V11_DBClickSlot( LuaState * L)
	{
		GetInventory()->OnEquip( L);
		return 0;
	}
	int UISlotInvenBase_V11_MouseWheel( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UISlotInvenBase_V11_Glue[] = {
	{ "uiClick_ShopItemDetail",		UISlotInvenBase_V11_ClickDetail },
	{ "uiClick_Shop_Slot",			UISlotInvenBase_V11_ClickSlot },
	{ "uiDoubleClick_ShopBuyItem",	UISlotInvenBase_V11_DBClickSlot },
	{ "uiMouseWheel", UISlotInvenBase_V11_MouseWheel },								// 샵에서 사용하기 위해 넣어봄..
	{NULL,						NULL}
};

I3_CLASS_INSTANCE( UISlotInvenBase_V11);

UISlotInvenBase_V11::UISlotInvenBase_V11()
{
	m_pImageCtrl = NULL;
}

UISlotInvenBase_V11::~UISlotInvenBase_V11()
{
	I3_SAFE_RELEASE( m_pImageCtrl);
}

/*virtual*/ void UISlotInvenBase_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UISlotBase::_InitializeAtLoad( pScene);

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_item_IIS" );
	I3ASSERT( m_pImageCtrl != NULL);

	//Register Lua Glue Functions
	LuaState* L = pScene->getLuaContext();
	if( L != NULL)
	{
		_RegisterLuaFunctions( L, UISlotInvenBase_V11_Glue);
	}

	OnUnselect();
}

/*virtual*/ void UISlotInvenBase_V11::_ClearAtUnload( i3UIScene * pScene)
{
	UISlotBase::_ClearAtUnload( pScene);

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE( m_pImageCtrl);
}

/*virtual*/ void UISlotInvenBase_V11::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "Scene/Main/PointBlankRe_item_itemSlot.i3UIs");
}

/*virtual*/ void UISlotInvenBase_V11::OnWheel( i3UIControl * pControl, INT32 zDelta)
{
	GetInventory()->OnWheel( pControl, zDelta);
}

/*virtual*/ bool UISlotInvenBase_V11::OnClick( i3UIControl * pControl)
{
	if( pControl != NULL)
	{
		if( OnSelect())
		{
			GetInventory()->EnableToolTip(this);
			return UISlotBase::OnClick( pControl);
		}
	}

	return false;
}

/*virtual*/ bool UISlotInvenBase_V11::OnSelect( bool bSound)
{
	LuaState * L = _CallLuaFunction("SetSlotSelection");
	if( L != NULL)
	{
		i3Lua::PushBoolean( L, true);	
		_EndLuaFunction( L, 1);
		return true;
	}

	return false;
}

void	UISlotInvenBase_V11::SetSlotEnable(bool bEnable)
{
	LuaState * L = _CallLuaFunction("SetShopSlotEnable");
	if ( L != NULL)
	{
		i3Lua::PushBoolean(L, bEnable);
		UILua::EndLuaFunction(L, 1);
	}
}

/*virtual*/ void UISlotInvenBase_V11::OnUnselect( void)
{
	LuaState * L = _CallLuaFunction("SetSlotSelection");
	if( L != NULL)
	{
		i3Lua::PushBoolean( L, false);	
		_EndLuaFunction( L, 1);
	}
}

UIPhaseInventory_V11 * UISlotInvenBase_V11::GetInventory( void)
{
	//	I3ASSERT( getParent() != NULL);
	I3ASSERT( i3::same_of<UIPhaseInventory_V11*>(getParent())); //->IsExactTypeOf( UIPhaseInventory::static_meta()));
	return (UIPhaseInventory_V11*) getParent();
}

/*virtual*/ void UISlotInvenBase_V11::SetEmptySlot( void)
{
	LuaState * L = _CallLuaFunction( "SetEmpty");
	_EndLuaFunction( L, 0);
}

void UISlotInvenBase_V11::SetInvenData( const char * pszItemName, UI_INVEN_CATEGORY category, bool is_repair_item, REAL32 repair_ratio, 
								   INT32 ItemID,	const char * pszDay, bool extension,
								   UI_INVEN_USAGE usage, UI_INVEN_NEWHOT newhot, UI_INVEN_EVENT event)
{
	LuaState * L = _CallLuaFunction("SetInvenData");
	if( L != NULL)
	{
		if( pszItemName != NULL)
			i3Lua::PushA2UTF8String( L, pszItemName);
		else
			i3Lua::PushA2UTF8String( L, "");

		i3Lua::PushBoolean( L, is_repair_item);
		i3Lua::PushNumber( L, repair_ratio);

		i3Lua::PushBoolean( L, extension);
		i3Lua::PushInteger( L, category);
		if( pszDay != NULL)
			i3Lua::PushA2UTF8String( L, pszDay);
		else
			i3Lua::PushA2UTF8String( L, "");
		i3Lua::PushInteger( L, static_cast<INT32>(usage));
		i3Lua::PushInteger( L, static_cast<INT32>(newhot));
		i3Lua::PushInteger( L, static_cast<INT32>(event));
		_EndLuaFunction( L, 9);
	}

	if( is_repair_item )
	{
		if( UserInfoContext::i()->IsNewRepair() )
		{
			L	= _CallLuaFunction( "SetRepairToolTip" );

			if( L != NULL )
			{
				i3::rc_wstring szEndurance;
				i3::rc_wstring szPenalty;
				
				UINT8	Endurace	= static_cast<UINT8>( MINMAX( 0.0f, repair_ratio * 100, 100.0f ) );
				i3::sprintf( szEndurance, L"%s : %u%%", GAME_RCSTRING("STR_POP_REPAIR_NAME"), Endurace );

				i3::string	strPenaltyName;
				if( g_pConfigEx != NULL )
				{
					g_pConfigEx->GetEndurancePenaltyName( Endurace, strPenaltyName );
				}
				i3::sprintf( szPenalty, L"%s : %s", GAME_RCSTRING("STR_POP_REPAIR_PENALTY") , GAME_RCSTRING(strPenaltyName.c_str()) );

				i3Lua::PushStringUTF16To8( L, szEndurance );
				i3Lua::PushStringUTF16To8( L, szPenalty );
				_EndLuaFunction( L, 2 );
			}
		}
		else
		{
			L	= _CallLuaFunction( "DisableToolTip" );

			if( L != NULL ) _EndLuaFunction( L, 0 );
		}
	}

	// ItemImage 다음의 class에서 자동으로 설정한다.
	GetItemImageMgr()->SelectImage(m_pImageCtrl, ItemID);
}

/*virtual*/ bool UISlotInvenBase_V11::OnEnterMouse( i3UIControl * pControl)
{
	return false;
}

/*virtual*/ void UISlotInvenBase_V11::OnLeaveMouse( i3UIControl * pControl)
{
	if( g_pFramework->getMouse() != NULL)
	{
		i3Rect rect;


		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		INT32 MouseX = static_cast<INT32>(pt.x);
		INT32 MouseY = static_cast<INT32>(pt.y);

		if( m_pFrameWnd)
		{
			VEC3D * pFramePos = m_pFrameWnd->getAccumulatedPos();

			rect.left = (INT32)getX( pFramePos);
			rect.right = rect.left + (INT32)getX( m_pFrameWnd->getSize());
			rect.top = (INT32)getY( pFramePos);
			rect.bottom = rect.top + (INT32)getY( m_pFrameWnd->getSize());

			//			if( rect.isInsidePt( MouseX, MouseY) == false)
			if(( rect.left > MouseX) || (rect.right <= MouseX) || 
				(rect.top > MouseY) || (rect.bottom <= MouseY))
			{
				GetInventory()->LeaveSlot();
			}
			//  조건문을 수정한 이유는 포지션/사이즈 계산값은  왼쪽닫힘/오른쪽 열림 반개구간인데
			//  충돌체크 검사는 양쪽 모두 양폐 구간으로 되어있어서 입니다. (11.10.24.수빈)

		}
	}
}
