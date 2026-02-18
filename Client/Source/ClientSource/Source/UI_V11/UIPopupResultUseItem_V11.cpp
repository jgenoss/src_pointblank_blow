#include "pch.h"
#include "UI_V11/UIPopupResultUseItem_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/ItemConsumer.h"
#include "UI/CBFuction.h"
#include "UI/UISideMenu.h"
#include "UI/UISMenu_Ability.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"
#include "InvenUtil.h"
#include "i3Base/ColorString.h"

I3_CLASS_INSTANCE( UIPopupResultUseItem_V11);//, UIPopupItemBase);

//
namespace V11_HELPER
{
	const I3COLOR g_ColorCashItem[COLOR_CASHITEM_COUNT] =
	{
		I3COLOR( 255,255,255, 255 ),	//	흰색
		I3COLOR( 121,23,0, 255 ),		//	빨강
		I3COLOR( 227,96,0, 255 ),		//	주황
		I3COLOR( 255,211,3, 255 ),		//	노랑
		I3COLOR( 196,213,0, 255 ),		//	잔디
		I3COLOR( 128,217,0, 255 ),		//	연두
		I3COLOR( 32,171,0, 255 ),		//	초록
		I3COLOR( 3,181,255, 255 ),		//	밝은 파랑
		I3COLOR( 0,96,227, 255 ),		//	파랑
		I3COLOR( 0,35,107, 255 ),		//	짙은 파랑
		I3COLOR( 115,37,117, 255 ),	//	보라
	};

	const I3COLOR * GetColorCashItem(INT32 idx)
	{
		I3_BOUNDCHK( idx, COLOR_CASHITEM_COUNT);
		return &g_ColorCashItem[idx];
	}
}

/*virtual*/ void UIPopupResultUseItem_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupItemBase::_InitializeAtLoad( pScene);

	i3UICaptionControl * pCtrl = (i3UICaptionControl*) pScene->FindChildByName( "captionStatic");
	I3ASSERT( pCtrl != NULL);

	//	WCHAR16 szWTemp[ MAX_STRING_COUNT] = {0,};
	//	i3String::CopyASCIItoWCHAR( szWTemp, GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));

	pCtrl->SetText(GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));
}

LuaState * UIPopupResultUseItem_V11::_SetItemInfo( void)
{
	LuaState * L = UIPopupItemBase::_SetItemInfo();

	if( L != NULL)
	{
		BOOL	bUseColor = FALSE;
		BOOL	bUseColor2 = FALSE;
		//		char	temp[MAX_STRING_COUNT * 2] = "";
		INT32	idxColor = ItemConsumer::instance()->m_nSelected + 1;
		I3COLOR * pColor = NULL;

		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( m_ItemID);
		//I3ASSERT( pInfo != NULL);

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
#ifdef __UNICODE_STRING__
				wstrResultValue = MyUserInfoBasic.m_strNick;
#else
				i3::mb_to_utf16(MyUserInfoBasic.m_strNick, wstrResultValue);	
#endif				

				bUseColor2 = TRUE; 
				break;
			}
		case RESULT_METHOD_COLOR_CLAN	:		
			{
#ifdef __UNICODE_STRING__
				wstrResultValue = MyUserInfoBasic.m_strClanName;
#else
				i3::mb_to_utf16(MyUserInfoBasic.m_strClanName, wstrResultValue);	
#endif				
				bUseColor = TRUE;	
				break;
			}
		case RESULT_METHOD_NICK			:		
			{
#ifdef __UNICODE_STRING__
				wstrResultValue = MyUserInfoBasic.m_strNick;
#else
				i3::mb_to_utf16( MyUserInfoBasic.m_strNick, wstrResultValue);	
#endif
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
				wstrResultValue = L"";	pColor = (I3COLOR*)V11_HELPER::GetColorCashItem((UINT8)idxColor);			
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

				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(m_ItemID);

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

			switch(InvData._ui16MuzzleFlashColor)
			{
			case 0:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_DEFAULT");
				break;
			case 1:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_RED");
				break;
			case 2:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_ORANE");
				break;
			case 3:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_YELLOW");
				break;
			case 4:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_GREEN");
				break;
			case 5:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_BLUE");
				break;
			case 6:
				wstrResultValue = GAME_RCSTRING("STR_POPUP_COLOREFFECT_COLOR_PURPLE");
				break;
			default:
				break;
			}

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

/*virtual*/ void UIPopupResultUseItem_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupItemBase::OnCreate( pParent);

	AddScene( "UIRe1/Scene/Popup/PBRe_PopUp_useItemComplete.i3UIs");
}

/*virtual*/ bool UIPopupResultUseItem_V11::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupItemBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( pArg1 == NULL)
		return false;

	T_ItemID itemID = *(T_ItemID*)pArg1;
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( itemID);
	//I3ASSERT( pInfo != NULL);
	if( pInfo == NULL) 
		return false;

	//	BC의 ShopItem 항목에 각 아이템마다 정의되어 있는 메시지를 출력한다.
	const i3::rc_wstring& wstrResultStatic	= pInfo->GetResultStatic();
	if( i3::generic_is_iequal( wstrResultStatic, L"NA") )
		return false;

	return true;
}

/*virtual*/ bool UIPopupResultUseItem_V11::OnExitStart( void)
{
	if( UIPopupItemBase::OnExitStart() == false)
		return false;

	//왼쪽 메뉴 능력치 변경
	if( UISideMenu::instance() != NULL)
	{
		SMenuAbility * pSMenuAbility = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
		if( pSMenuAbility != NULL)
			pSMenuAbility->smenu_ability_update();
	}

	UI::call_cb( "update_slot9", (void*) true, (void*) true); //9개 slot 상태 변경

	return true;
}
