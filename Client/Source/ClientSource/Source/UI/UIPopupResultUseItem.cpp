#include "pch.h"
#include "UIPopupResultUseItem.h"

#include "UIMainFrame.h"
#include "ItemConsumer.h"
#include "UISideMenu.h"
#include "UISMenu_Ability.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"
#include "InvenUtil.h"
#include "i3Base/ColorString.h"

#include "UISignal.h"


I3_CLASS_INSTANCE( UIPopupResultUseItem);//, UIPopupItemBase);

/*virtual*/ void UIPopupResultUseItem::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupItemBase::_InitializeAtLoad( pScene);

	i3UICaptionControl * pCtrl = (i3UICaptionControl*) pScene->FindChildByName( "captionStatic");
	I3ASSERT( pCtrl != nullptr);

//	WCHAR16 szWTemp[ MAX_STRING_COUNT] = {0,};
//	i3String::CopyASCIItoWCHAR( szWTemp, GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));

	m_pFireEffect = (i3UIButton*)pScene->FindChildByName("FireEffect_btn");

	pCtrl->SetText(GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));
}

LuaState * UIPopupResultUseItem::_SetItemInfo( void)
{
	if (m_pFireEffect != nullptr) m_pFireEffect->SetVisible(false);

	LuaState * L = UIPopupItemBase::_SetItemInfo();

	if( L != nullptr)
	{
		bool	bUseColor = false;
		bool	bUseColor2 = false;
//		char	temp[MAX_STRING_COUNT * 2] = "";
		INT32	idxColor = ItemConsumer::instance()->m_nSelected + 1;
		I3COLOR * pColor = nullptr;

		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( m_ItemID);
		//I3ASSERT( pInfo != nullptr);

		//	BC의 ShopItem 항목에 각 아이템마다 정의되어 있는 메시지를 출력한다.
		i3::rc_wstring wstrResultStatic;
		i3::rc_wstring wstrResultValue;
		i3::rc_wstring wstrResultNotice;

		if( pInfo)
		{
			wstrResultStatic	= pInfo->GetResultStatic();
			wstrResultValue	= pInfo->GetResultValue();
			wstrResultNotice	= pInfo->GetResultNotice();
		}
 

		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

		ITEM_RESULT_METHOD	Method = use_item::ResultMethodStringToEnum(m_ItemID);
		switch (Method)
		{
		case RESULT_METHOD_COLOR_NICK	:	
			{
				wstrResultValue = MyUserInfoBasic.m_strNick;
				
				bUseColor2 = true; 
				break;
			}
		case RESULT_METHOD_COLOR_CLAN	:		
			{
				wstrResultValue = MyUserInfoBasic.m_strClanName;
				bUseColor = true;	
				break;
			}
		case RESULT_METHOD_NICK			:		
			{
				wstrResultValue = MyUserInfoBasic.m_strNick;
				break;
			}
		case RESULT_METHOD_RANK			:		
			{
				USER_INFO_INVITEM_DATA myUserInfoInvItemData;

				UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

				if (myUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
					wstrResultValue = GetCharaRank( myUserInfoInvItemData._wAprilFoolRank);
				else if (myUserInfoInvItemData._wRank != RANK_NONE)
					wstrResultValue = GetCharaRank( myUserInfoInvItemData._wRank);
				else
					I3TRACE("Invalid Rank information\n");
				break;
			}
		case RESULT_METHOD_COLOR_CROSS_HAIR	:	
			{
				wstrResultValue = L"";	
				pColor = (I3COLOR*)GetColorCashItem((UINT8)idxColor);			
				break;
			}
		case RESULT_METHOD_CLAN_NAME		:
			{
				//	클랜관련 정보는 서버에서 SUCCESS가 날라왔을때는 정보가 바로 갱신되지 않는다.
				//	전송 시간차 때문에 좀 있다 갱신되므로 가라로 성공한것처럼 보여야 한다.
				wstrResultValue = g_pFramework->GetClanContext()->GetClanNameText();
			}
			break;
		case RESULT_METHOD_CLAN_PERSON_PLUS	:
			{
				//	클랜관련 정보는 서버에서 SUCCESS가 날라왔을때는 정보가 바로 갱신되지 않는다.
				//	전송 시간차 때문에 좀 있다 갱신되므로 가라로 성공한것처럼 보여야 한다.
				CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();

				i3::sprintf(wstrResultValue, L"%d", pClanInfo->m_maxPerson);

			}
			break;
		case RESULT_METHOD_GRAB_BAG			:
			{
				
		//		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);
				
				if( pInfo)
					wstrResultValue = pInfo->GetCapsuleItemResultName(CShopContext::i()->getCapsuleItemIdx());

				// 이전 시점의 어떤 패킷에 있던 값을 
				// 게임컨텍스트에서 보관해놓고, 여기와서 그걸 쓰는 방식인데 좋아보이지는 않는다..
				//
			}
			break;
		case RESULT_METHOD_POINT:
			{
				UINT8 attr; UINT32 arg;
				inven::GetItemTypeArg(m_ItemID, &attr, &arg);
				wstrResultStatic = GAME_RCSTRING("STR_ITEM_USE_COMPLETE");
			}
			break;
		case RESULT_METHOD_CLAN_MARK :
			wstrResultStatic = GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CHANGE");
			break;
		case RESULT_METHOD_COLOR_FIRE_MUZZLE :

			USER_INFO_INVITEM_DATA InvData;
			UserInfoContext::i()->GetMyUserInfoInvItemData(&InvData);
			
			I3COLOR *pColor2 = const_cast<I3COLOR*>(GetColorbyIdx(0, InvData._ui16MuzzleFlashColor));
			if (m_pFireEffect != nullptr)
			{
				m_pFireEffect->SetVisible(true);
				m_pFireEffect->setColor(pColor2);
			}
				

			wstrResultValue = L"";
			break;
		}

		i3Lua::PushInteger( L, Method);
		i3Lua::PushStringUTF16To8( L, wstrResultStatic);

		if( bUseColor)	//	컬러
		{
			i3::wstring wstrValue;
			SetNameUsingColorTable( wstrValue, wstrResultValue, (UINT8)idxColor);
			i3Lua::PushStringUTF16To8( L, wstrValue);
		}
		else if(bUseColor2)
		{
			UINT8 colorIndex = (UINT8)ItemConsumer::instance()->m_nSelected;
			const I3COLOR& color = ColorNickPalette::i()->GetColor(colorIndex);

			i3::wstring wstrValue = wstrResultValue;
			::ColorString(wstrValue, color.r, color.g, color.b);

			i3Lua::PushStringUTF16To8( L, wstrValue);
		}
		else
			i3Lua::PushStringUTF16To8( L, wstrResultValue);

		i3Lua::PushStringUTF16To8( L, wstrResultNotice);
		i3Lua::PushColor( L, pColor);
		_EndLuaFunction( L, 9);
	}

	return L;

}

/*virtual*/ void UIPopupResultUseItem::OnCreate( i3GameNode * pParent)
{
	UIPopupItemBase::OnCreate( pParent);
	
	AddScene( "Scene/Popup/PBRe_PopUp_useItemComplete.i3UIs" );
}

/*virtual*/ bool UIPopupResultUseItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupItemBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( pArg1 == nullptr)
		return false;

	T_ItemID itemID = *static_cast<T_ItemID*>(pArg1);
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( itemID);
	//I3ASSERT( pInfo != nullptr);
	if( pInfo == nullptr) 
		return false;

	//	BC의 ShopItem 항목에 각 아이템마다 정의되어 있는 메시지를 출력한다.
	const i3::rc_wstring& wstrResultStatic	= pInfo->GetResultStatic();
	if( i3::generic_is_iequal( wstrResultStatic, L"NA") )
		return false;

	return true;
}

/*virtual*/ bool UIPopupResultUseItem::OnExitStart( void)
{
	if( UIPopupItemBase::OnExitStart() == false)
		return false;

	//왼쪽 메뉴 능력치 변경
	if( UISideMenu::instance() != nullptr)
	{
		SMenuAbility * pSMenuAbility = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
		if( pSMenuAbility != nullptr)
			pSMenuAbility->smenu_ability_update();
	}

//	UI::call_cb( "update_slot9", (void*) true, (void*) true); //9개 slot 상태 변경
	UISignal::UpdateSlot9(true, true); //9개 slot 상태 변경

	return true;
}
