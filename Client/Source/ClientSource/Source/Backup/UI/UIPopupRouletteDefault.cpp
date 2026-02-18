#include "pch.h"
#include "UIPopupRouletteDefault.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UIRouletteView.h"


#include "../GuiNotifyReceiver.h"
#include "../TextSet.h"

#include "i3Base/string/ext/sprintf.h"

I3_CLASS_INSTANCE( UIPopupRoulettDefault);//, UIPopupBase);


UIPopupRoulettDefault::UIPopupRoulettDefault()
{
}

UIPopupRoulettDefault::~UIPopupRoulettDefault()
{

	
}

void UIPopupRoulettDefault::_ModifyControl( void)
{


}
void UIPopupRoulettDefault::SetName()
{

		i3::string Name;
		UINT32 pitemid = g_pGameContext->GetRSItemID() ;
		INT32 goodsClass = GET_ITEM_FLAG_TYPE(pitemid);
		INT32 num = GET_ITEM_FLAG_NUMBER( pitemid);

		switch((ITEM_TYPE) goodsClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( pitemid);
				CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, num);
				pInfo->GetWeaponName( Name);
				//i3::safe_string_copy(Name, pInfo->GetName(), MAX_STRING_COUNT);
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(pitemid);
				if( pEquipInfo)
				{
					g_pCharaInfoDataBase->GetCharacterName(pitemid, Name); //sizeof(outItemName));
				}		
				else
				{
					i3::sprintf(Name, "%s \nItem:%d", INVALID_TEXT,   pitemid);
				}
			}
			break;
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_PERIOD:
		case ITEM_TYPE_WRAP_COUNT:
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pitemid);
				if( pInfo )
				{
					g_pShopItemInfoDataBase->GetItemName(pitemid, Name);
				}
				else
					i3::sprintf(Name, "%s\nItem:%d", INVALID_TEXT, pitemid);
			}
			break;
		}

	
	LuaState * L = _CallLuaFunction(0, "setItemName");
	
	if( L != NULL)
	{

		i3Lua::PushA2UTF8String( L, Name.c_str());
		_EndLuaFunction( L, 1);
	}




}

/*virtual*/ void UIPopupRoulettDefault::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/Scene/Popup/PBRe_PopUp_defaultWin.i3UIe");

	
	
}

/*virtual*/ bool UIPopupRoulettDefault::OnKey_Enter( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettDefault::OnKey_Escape( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettDefault::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_ModifyControl();


	return true;
}

void UIPopupRoulettDefault::OnUpdate( REAL32 rDeltaSeconds)
{

	UIPopupBase::OnUpdate(rDeltaSeconds);

}
/*virtual*/ bool UIPopupRoulettDefault::OnOKButton( void)
{
	ConfirmOK();

	return true;
}

void UIPopupRoulettDefault::ConfirmOK(void)
{
	g_pFramework->GetUIMainframe()->TogglePopup( UPW_ROULETTE_DEFAULT);
}

/*virtual*/ void UIPopupRoulettDefault::OnLoadAllScenes()
{
	if (GetScene(0) != NULL)
	{
		
		m_ImgBox = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet0");
	
		m_ImgBox->SetVisible(FALSE);
	
		i3UIFrameWnd * pUWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIButtonImageSet0");
		pUWnd->SetVisible(FALSE);

		i3UIFrameWnd * pImageWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIImageBox0");
		pImageWnd->SetVisible(FALSE);
	

		
		i3UIFrameWnd * pWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd5");
		
		i3Vector::Copy( &m_vOriginalSize, pWnd->getSize());
		m_slide.AddFrameWnd(pWnd, UISlide::POPUP);
		m_slide.SetOriginalPos(380.f,280.f);


		if( m_bScaleAnimation)
		{

		
			m_slide.SetTargetTime(false, 1.f / 10.f );
			m_slide.SetTargetTime(true, 1.f / 2.f);
		}
		else
		{
			m_slide.SetTargetTime(false, 0.f );
			m_slide.SetTargetTime(true, 0.f);
		}
	}
	else
	{
		m_slide.SetTargetTime(false, 0.f);
		m_slide.SetTargetTime(true, 0.f);
	}

	
	
}

/*	virtual*/ void	UIPopupRoulettDefault::OnEntranceEnd( void)
{


	i3UIFrameWnd * pUWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIButtonImageSet0");
	pUWnd->SetVisible(true);

	i3UIFrameWnd * pImageWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIImageBox0");
	pImageWnd->SetVisible(true);


	m_ImgBox->SetVisible(true);

	SetName();
	OpenEventWin(g_pGameContext->GetRSItemID());


	VEC3D vPos;
	i3Vector::Set( &vPos,1.f,1.f,0.f);
	g_pSndMng->StartRouletteSnd(GSND_ROULETTE_BOOM,&vPos,false);


}

/*virtual*/ void UIPopupRoulettDefault::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_ImgBox);
}

void UIPopupRoulettDefault::OpenEventWin(INT32 ItemId)
{
	GetItemImageMgr()->SelectImage(m_ImgBox,ItemId);
}


