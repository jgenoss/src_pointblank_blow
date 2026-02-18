#include "pch.h"
#include "UIUtil.h"

#include "UIMainFrame.h"
#include "UIMath.h"
#include "UIPhaseLobby.h"
#include "UIBattleFrame.h"

#include "../UserLocationInfo.h"
#include "../RevengeManager.h"
#include "../GameGUI.h"
#include "../GameContextUtil.h"


#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/atoi.h"

#include "../CommunityContext.h"
#include "ClanGameContext.h"
#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/ColorString.h"
#include "i3Base/RemoveColorString.h"

#include "UIPhaseGachaShop.h"

#include "ClientStageInfoUtil.h"

UIFloatingBase * GameUI::GetFloating( UIFLOATWIN type)
{
	if( g_pFramework->GetUIMainframe() != nullptr)
		return g_pFramework->GetUIMainframe()->GetFloating( type);
	return nullptr;
}

UIPopupBase * GameUI::GetPopup( UIPOPUPWIN type)
{
	if( g_pFramework->GetUIMainframe() != nullptr)
		return g_pFramework->GetUIMainframe()->GetPopup( type);
	return nullptr;
}

UIBattlePopupBase *	GameUI::GetBattlePopup( UIBATTLE_POPUP type)
{
	if( UIBattleFrame::i() != nullptr)
		return UIBattleFrame::i()->GetPopup( type);
	return nullptr;
}

UIPhaseBase * GameUI::GetPhase( UIPHASE type)
{
	if( g_pFramework->GetUIMainframe() != nullptr)
		return g_pFramework->GetUIMainframe()->GetPhase( type);
	return nullptr;
}

UIPhaseSubBase * GameUI::GetSubPhase( UISUBPHASE type)
{
	if( g_pFramework->GetUIMainframe() != nullptr)
		return g_pFramework->GetUIMainframe()->GetSubPhase( type);
	return nullptr;
}

void GameUI::ClosePopup( UIPOPUPWIN type)
{
	if( g_pFramework->GetUIMainframe() != nullptr)
		return g_pFramework->GetUIMainframe()->ClosePopup( type);
}

void GameUI::ExitGamePopup(const i3::rc_wstring& exit_reason)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, exit_reason, nullptr, EXIT_CALLBACK(UIBase::CbExitNormal,true) );
}

void GameUI::GetTextFriendState( i3::wstring& out_wstr, UINT32 State )
{
	switch(GET_FRIEND_STATE(State))
	{
	case FRIEND_STATE_REQUEST:		out_wstr = GAME_RCSTRING("STR_POPUP_SECRET");		/*비공개*/	break;
	case FRIEND_STATE_HOLD:			out_wstr = GAME_RCSTRING("STR_POPUP_SECRET");		/*비공개*/	break;
	case FRIEND_STATE_OFFLINE:		out_wstr = GAME_RCSTRING("STR_POPUP_OFFLINE_STATE");/*오프라인*/ break;
	case FRIEND_STATE_ONLINE:		out_wstr = GAME_RCSTRING("STR_POPUP_READY_STATE");	/*대기중*/	break;
	case FRIEND_STATE_LOBBY:
	{
		CUserLocationInfo	UserLocInfo(State);
		i3::sprintf(out_wstr, GAME_RCSTRING("STR_POPUP_WHAT_POSITION_LOBBY"),
			UserLocInfo.GetDisplayServerChannelNumber());/*%03d서버*/

		break;
	}
	case FRIEND_STATE_ROOM:
		{
			CUserLocationInfo	UserLocInfo(State);

			i3::sprintf(out_wstr, GAME_RCSTRING("STR_POPUP_WHAT_SERVER"),
				UserLocInfo.GetDisplayServerChannelNumber());/*%03d서버*/

			if (FRIEND_STATE_ROOM <= GET_FRIEND_STATE(State))
			{
				i3::stack_wstring wstr_room;
				i3::sprintf(wstr_room, GAME_RCSTRING("STR_POPUP_WHAT_ROOM"),
					UserLocInfo.GetDisplayRoomNumber());/* %d번 방*/
				i3::generic_string_cat(out_wstr, wstr_room);
			}
		}
		break;
	default : out_wstr = L"-"; break;
	}
}

void GameUI::GetTextClanState( i3::wstring& out_wstr, UINT64 State)
{
	CUserLocationInfo	UserLocInfo( State);
	INT32 nMatch = UserLocInfo.GetMatch();

	if( nMatch != MATCH_TEAM_INVALID_IDX)
	{
	

		switch( UserLocInfo.GetState())
		{
		case FRIEND_STATE_OFFLINE:
				out_wstr = GAME_RCSTRING("STR_POPUP_OFFLINE_STATE");/*오프라인*/
			break;
		case FRIEND_STATE_LOBBY:
			{
				i3::sprintf( out_wstr, GAME_RCSTRING("STR_POPUP_WHAT_POSITION_LOBBY"),
						UserLocInfo.GetDisplayServerChannelNumber());/*%03d서버*/
			}
			break;
		case FRIEND_STATE_ROOM:
			{
				i3::wstring wstr_state2;
				GetBattleTeamName( wstr_state2, nMatch-1, true );

				i3::sprintf( out_wstr, GAME_RCSTRING("STR_POPUP_WHAT_PLAY"),
						UserLocInfo.GetDisplayServerChannelNumber(), wstr_state2);/*%03d-%s 게임중*/
			}
			break;
		}
	}
	else
	{
		GameUI::GetTextFriendState( out_wstr, FRIEND_STATE_64_TO_32( State));
	}
}

bool GameUI::IsCommunityFriend( const i3::wliteral_range& wNickRng)
{
	UINT32 i;

	for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if( node != nullptr)
		{
			if( i3::generic_is_iequal( wNickRng, node->_wstr_nick) )
			{
				return true;
			}
		}
	}

	return false;
}

bool GameUI::IsCommunityFriend( INT64 uid)
{
	UINT32 i;

	for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if( node != nullptr)
		{
			if(uid == node->_uid)
			{
				return true;
			}
		}
	}

	return false;
}


INT64 GameUI::FindUserUidByNick( const i3::wliteral_range& wNickRng )
{
	// 자신은 막는다.
	if ( UserInfoContext::i()->IsMyNickName(wNickRng) )			// 이 함수는 구현이 무겁다...
	{
		/*자신의 닉네임을 입력하셨습니다.*/
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_SELF"));
		return 0;
	}

	// 친구 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if(nullptr == node) continue;

		if ( i3::generic_is_iequal(wNickRng, node->_wstr_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	// 클랜 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember(i);

		if (i3::generic_is_iequal(wNickRng, node->_wstr_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	return 0;
}

void GameUI::GetStageMapName(bool useRandomMap, STAGE_ID stageID, i3::rc_wstring& out_wstrMapName)
{
	if(useRandomMap)
	{
		out_wstrMapName = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
	}
	else
	{
		out_wstrMapName = CStageInfoUtil::GetMapData((UINT32)stageID)->getDisplayName();
	}
}

//1.5 버전에 안 맞는 함수 입니다. 사용하지 마세요.
void GameUI::GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, WEAPON_CLASS_TYPE type, WEAPON::RES_ID num)
{
	CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( type, num);

	if( pInfo != nullptr)
	{
		
		pInfo->GetWeaponName( outName);

		if( outName.empty() )
			i3::sprintf( outName, L"NO NAME\nClass(%d)\nNum(%d)", type, num);
	

		if( pOutDesc != nullptr)
			pInfo->GetDescription( *pOutDesc);
		
		if (pOutCategory != nullptr)
		{
			g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, *pOutCategory );
		}
	}
	else
	{	
		
		i3::sprintf( outName, L"%s\nClass(%d)\nNum(%d)", INVALID_TEXTW, type, num);

		if( pOutDesc != nullptr)
			(*pOutDesc).clear(); 
		if (pOutCategory != nullptr)
			(*pOutCategory).clear(); 
	}
}

const I3COLOR * GameUI::GetNickNameColor( UINT8 colorIdx)
{
//	if( colorIdx < COLOR_CASHITEM_COUNT )
//	{
		return GetColorCashItem(colorIdx);

//	}
//	return nullptr;
	}

UINT8 GameUI::GetNickNameColorIndexByColor( I3COLOR * pColor)
{
	for(INT32 i = 0; i < COLOR_CASHITEM_COUNT; i++)
	{
		const I3COLOR* colorCash = GetColorCashItem( UINT8(i) );
		if(i3Color::IsSame((COLOR*)colorCash, (COLOR*)pColor))
			return UINT8(i);
	}

	return 0;
}

void GameUI::GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, T_ItemID ItemID)
{
	WEAPON_CLASS_TYPE type = WEAPON::ItemID2Class( ItemID);
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( ItemID);

	GetWeaponString( outName, pOutDesc, pOutCategory, type, number);
}

INT32 GameUI::GetWeaponImageIndex( T_ItemID ItemID)
{
	WEAPON_CLASS_TYPE weaponClassType = WEAPON::ItemID2Class( ItemID);
	WEAPON::RES_ID weaponNum = WEAPON::ItemID2ResID( ItemID);

	// 무기 이미지
	return g_pFramework->GetWeaponOrdIdx( weaponClassType, weaponNum);
}

void GameUI::GetNormalizedWeaponInfo( T_ItemID itemID, 
									INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
									INT32& Recoil_H, INT32& Speed, 
									i3::stack_wstring& out_wstrRange, i3::stack_wstring& out_wstrBulletCount)
{
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemID );

	if( pInfo == nullptr)
	{
		Damage = 0;
		Rapid = 0;
		Accuracy = 0;
		Recoil_V = 0;
		Recoil_H = 0;
		Speed = 0;
		
		out_wstrRange = L"-";
		out_wstrBulletCount = L"-";
	}
	else
	{
		Damage	= pInfo->GetPercentage(WEAPON::INFO_DAMAGE);
		Rapid	= pInfo->GetPercentage(WEAPON::INFO_RAPID);
		Accuracy = pInfo->GetPercentage(WEAPON::INFO_HIT_RATE);
		Recoil_V = pInfo->GetPercentage(WEAPON::INFO_VERT_RECOIL);
		Recoil_H = pInfo->GetPercentage(WEAPON::INFO_HORZ_RECOIL);
		Speed	= pInfo->GetPercentage(WEAPON::INFO_SPEED);
		
		pInfo->MakeWeaponRangeString(out_wstrRange);
		pInfo->MakeBulletCountString(out_wstrBulletCount);
	}
}

void	GameUI::GetGaugeString(INT32 gauge, i3::stack_wstring& out_wstr )
{
	if (gauge >= 0)
	{
		i3::sprintf(out_wstr, L"%d", gauge);
	}
	else
	{
		out_wstr = L"-"; 
	}
}

void GameUI::GetCharacterString(  i3::rc_wstring& outName, i3::rc_wstring& outDesc, UINT32 charaID)
{	
	const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo( charaID);
	if( pEquipInfo)
	{
		g_pCharaInfoDataBase->GetCharacterName( charaID, outName);
		pEquipInfo->getDescription( outDesc);
	}		
	else
	{
		i3::sprintf( outName, L"%s\nCharaID:%d", INVALID_TEXTW, charaID);
		outDesc.clear();
	}
}

INT32	GameUI::GetItemImageIndex( INT32 & outUsageType, INT32& outTexIndex, T_ItemID ItemID)
{
	outUsageType = GET_ITEM_TYPE( ItemID);

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( ItemID);

	I3ASSERT_RETURN(pInfo, 0);			// 에잉..잘 모르겠다!!!! 0으로 놓자...

	if ( GET_ITEM_TYPE( ItemID) == ITEM_TYPE_POINT )
	{
		outUsageType = pInfo->GetType();
	}

	I3ASSERT(outUsageType >= 0 && (outUsageType >= ITEM_TYPE_MAINTENANCE && outUsageType <= ITEM_TYPE_WRAP_COUNT));

	outTexIndex = pInfo->GetTexIndex();

	return pInfo->GetShapeIndex();
}

void GameUI::GetItemString( i3::rc_wstring& outName, i3::rc_wstring& outDesc, T_ItemID ItemID)
{
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( ItemID);
	
	if( pInfo )
	{
		g_pShopItemInfoDataBase->GetItemName( ItemID, outName);
		outDesc = pInfo->GetDescription();
	}
	else
	{
		i3::sprintf( outName, L"%s\nItem:%d", INVALID_TEXTW, ItemID);
		outDesc.clear();
	}
}

void GameUI::GetItemTermString( i3::rc_wstring& pOut, const size_t sizeOutMax, bool IsCountItem, UINT8 Type, UINT32 Arg)
{
	UINT32 Max_Term = 0;

	switch(Type)
	{
	case ITEM_ATTR_TYPE_BUY: // 구매 완료
		{
			if( !IsCountItem  )
			{				
				UINT32 term = ConvertSecToUseDay(Arg);
				Max_Term = UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);
				Max_Term = ConvertSecToUseDay(Max_Term);

				if (term >= Max_Term)
				{
					term = Max_Term;
				}

				if( term > 1)
				{
					//	남은기간 : %d일
					i3::sprintf(pOut, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"), term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
				else
				{
					UINT32 hour = ConvertSecToUseHour(Arg);	// 시간 단위로 표시
					//	남은기간 : %d시간
					i3::sprintf(pOut, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"), hour, GAME_RCSTRING("STR_UI_HOUR"));
				}
			}
			else
			{
				// 남은개수 : %d개
				i3::sprintf(pOut, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_COUNT"), Arg, GAME_RCSTRING("STBL_IDX_ITEM_COUNT_UNIT"));
			}
		}
		break;
	case ITEM_ATTR_TYPE_AUTH: // 사용중		/* Fall through */
		{
			// 기간제라면.

			UINT32 term = 0;
			UINT32 sec = 0;			
			Max_Term = UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);
			Max_Term = ConvertSecToUseDay(Max_Term);

			if( Arg > 0)
			{
				sec = UserInfoContext::i()->GetInventorySecTime(Arg);

				if( 0 > (INT32)sec ) sec = 0;

				//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
				if( sec >= ITEM_TERM_ADJUST_VALUE)
					sec -= ITEM_TERM_ADJUST_VALUE;

				term = ConvertSecToUseDay( sec);
			}

			if (term >= Max_Term)
			{
				term = Max_Term;
			}

			if (1 < term)
			{
				//	남은기간 : %d일
				i3::sprintf(pOut, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"), term, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));	
			}
			else if( term == 0)
			{
				//	남은기간 : 0시간
				i3::sprintf(pOut, L"%s : {col:210,0,0,255}0%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"), GAME_RCSTRING("STR_UI_HOUR"));
			}
			else if( sec > 0)
			{
				UINT32 hour = ConvertSecToUseHour( sec);

				if( hour > 24)
				{
					//	남은기간 : %d시간
					i3::sprintf(pOut, L"%s : {col:255,255,255,255}%d%s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"), hour,
						GAME_RCSTRING("STR_UI_HOUR"));
				}
				else
				{
					// 남은기간 : 24시간 미만
					i3::sprintf(pOut, L"%s : {col:210,0,0,255}1%s %s{/col}", GAME_RCSTRING("STBL_IDX_ITEM_REMAIN_TERM"),
						GAME_RCSTRING("STR_UI_HOUR"), GAME_RCSTRING("STBL_IDX_ITEM_LESS_THEN"));
				}
			}
		}
		break;

	default:
		{
#if defined (ENABLE_INVEN_DISPLAY_OBT)
			//	남은기간 : OBT 한정
			i3::sprintf(pOut, "%s : {col:255,255,255,255}%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), GAME_STRING("STBL_IDX_ITEM_UNLIMITED"));
#else
			i3::sprintf(pOut,L"");
#endif
		}
		/* Do nothing */
		break;
	}
}

void GameUI::GetRatioEquippedItemsByEquipInfo( CEquipInfo* pEquipInfo, float& outExp, float& outPoint, float& outPenetrate,
											  float& outDamage, float& outDefense, float& outSpeed, float& outAccuracy)
{
	outPenetrate += pEquipInfo->GetAbilityValue( EQUIP::eABILITY_PENETRATE);
	outDamage    += pEquipInfo->GetAbilityValue( EQUIP::eABILITY_DAMAGE);
	outDefense   += pEquipInfo->GetAbilityValue( EQUIP::eABILITY_DEFENSE_BODY);
	outSpeed     += pEquipInfo->GetAbilityValue( EQUIP::eABILITY_MOVESPEED_FRONT);
	outAccuracy  += pEquipInfo->GetAbilityValue( EQUIP::eABILITY_ACCURACY);
}

void		GameUI::GetRatioEquippedItems(bool is_red, float& outExp, float& outPoint, float& outPenetrate, float& outDamage, 
								  float& outDefense, float& outHelmetDefense, float& outSpeed, float& outAccuracy)
{
	outExp = 0.f; outPoint = 0.f;  outPenetrate = 0.f; outDamage = 0.f; outDefense = 0.f; outHelmetDefense = 0.f;  outSpeed = 0.f;   outAccuracy = 0.f;

	// vv3 - check
	const CHARA_PARTS_INFO * pPartInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo();
	T_ItemID chara_id = pPartInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	T_ItemID head_id = pPartInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_HEAD);
	T_ItemID item_id = pPartInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_SKIN);


	const CEquipInfo* pEquipInfo = g_pCharaInfoDataBase->GetInfo( chara_id);
	if( pEquipInfo != nullptr)
		GetRatioEquippedItemsByEquipInfo( const_cast<CEquipInfo*>(pEquipInfo), outExp, outPoint, outPenetrate, outDamage, outDefense, outSpeed, outAccuracy);

	pEquipInfo = g_pCharaInfoDataBase->GetInfo( head_id);
	if( pEquipInfo != nullptr)
	{
		GetRatioEquippedItemsByEquipInfo( const_cast<CEquipInfo*>(pEquipInfo), outExp, outPoint, outPenetrate, outDamage, outDefense, outSpeed, outAccuracy);
		if ( i3::kind_of<CHeadInfo*>(pEquipInfo))
		{
			outHelmetDefense += static_cast<const CHeadInfo*>(pEquipInfo)->getDefenseRate();
		}
	}

	pEquipInfo = g_pCharaInfoDataBase->GetInfo( item_id);
	if( pEquipInfo !=  nullptr)
		GetRatioEquippedItemsByEquipInfo( const_cast<CEquipInfo*>(pEquipInfo), outExp, outPoint, outPenetrate, outDamage, outDefense, outSpeed, outAccuracy);
}

INT32 GameUI::GetRibbonShape( INT32 ribbonID)
{
	return 10;	// 마지막 쉐입 인덱스로 고정
}

INT32 GameUI::GetEnsignShape( INT32 ensignID)
{
	return 9;	// 마지막 쉐입 인덱스로 고정
}

INT32 GameUI::GetMedalShape( INT32 medalID)
{
	return 22;	// 마지막 쉐입 인덱스로 고정
}

bool GameUI::GetUserLocationInfo( CUserLocationInfo &Out, USERLIST_TAB tab, INT32 idx)
{
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( idx);
		if (!node) return false;
		
		//Out = CUserLocationInfo(node->_state); //임시객체를 생성한다.
		Out.SetState(node->_state);
		return true;
	}

	switch( tab )
	{
	case USERLIST_TAB_FRIEND:
		{
			const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);
			if (!node) return false;

			Out.SetState(node->_state);
		}
		break;

	case USERLIST_TAB_CLAN:
		{
			const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( idx);
			if (!node)return false;

			Out.SetState(node->_state);
		}
		break;

	default:
		{
			const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

			INT32 i32State = pUserInfo->_UserInfo.m_State;
			INT32 i32ServerIdx = pUserInfo->_UserInfo.m_ServerIdx;
			INT32 i32ChanenlIdx =  pUserInfo->_UserInfo.m_ChannelNum;
			INT32 i32RoomIdx = pUserInfo->_UserInfo.m_RoomIdx;

			if(i32ServerIdx < 0) return false;

			UINT32 ui32State = static_cast<UINT32>(MAKE_FRIEND_STATE(i32State, i32ServerIdx, i32ChanenlIdx, i32RoomIdx));

			Out.SetState(ui32State);
		}
		break;
	}

	return true;
}

void GameUI::FindAwayUser( USERLIST_TAB tab, INT32 idx)
{
	// 클랜 매치 중이면 찾아가기 실패
	if( g_pFramework->GetClanContext()->InBattleTeam() )
		return;

	if( ClanGameContext::i()->getMyMercenaryState() ) 
	{
		// 용병은 클랜전 방 생성 및 참가가 불가합니다.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_CLANCHANNEL_MERCENARY_INVITE_FAIL"));
		return; 
	}

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	if( i3::same_of<UIPhaseGachaShop*>(pSubPhase)) 
	{
		// hansoft. 11249. 뽑기상점 입장 상태에서 친구 찾아가기 기능 사용 시 레디룸으로 이동됨.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_CANNOT_VISIT"));
		return;
	}

	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != nullptr);

	CUserLocationInfo	UserLocInfo;
	if( !GetUserLocationInfo(UserLocInfo, tab, idx))
		return;

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	//CHECK_ASSERT(pOtherServerInfo != nullptr);
	if( pOtherServerInfo == nullptr) return;

	// 양쪽 모두 대회서버이면 이동이 가능하도록 한다.
	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_CAN_NOT_VISIT_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_CAN_NOT_VISIT_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}

	switch( tab )
	{
	case USERLIST_TAB_FRIEND:	FindAwayFriend( idx);	break;
	case USERLIST_TAB_CLAN:		FindAwayClan( idx);		break;
	default:					FindAwayNormalUser();	break;
	}
}

void GameUI::FindAwayFriend( INT32 idx)
{
	if (-1 != idx)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);
		
		if( node &&
			(FRIEND_STATE_ROOM == GET_FRIEND_STATE(node->_state)) )
		{
			// 현재 레뒤룸에 있고, 서버, 체널 룸이 같으면 이미 방에 참여중입니다.
			bool	SameRoom = gcu::IsSameRoom(node->_state);
			UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
			if( UIPHASE_READY_ROOM == pFrame->GetCurrentPhaseType() && SameRoom)
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
			else
			{
				CCommunityContext::i()->SetMovingToFriendUser(idx);
				g_pFramework->GetUIMainframe()->OnInviteMove();
			}
		}
		else
		{
			MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*찾아가시려면 친구가 방에 있어야합니다.*/
		}
	}
	else
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*찾아갈 친구를 선택하셔야합니다.*/
	}
}

void GameUI::FindAwayClan( INT32 idx)
{
	if (-1 == idx)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*찾아갈 친구를 선택하셔야합니다.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( idx);

	if( node == nullptr )
		return;

	CUserLocationInfo	UserLocInfo(node->_state);
	UINT64 clan_state = node->_state;

	if(UserLocInfo.IsMatch())
	{
		if ( FRIEND_STATE_ROOM != GET_FRIEND_STATE(clan_state))
		{
			CCommunityContext::i()->SetMovingToClanUser(idx);
			g_pFramework->GetUIMainframe()->OnInviteMoveClan();
		}
		else
		{
			MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_VISIT_MATCHING"));/*매치 중에는 찾아가기를 하실 수 없습니다.*/
		}
	}
	else
	{
		if (node && FRIEND_STATE_ROOM == GET_FRIEND_STATE(clan_state))
		{
			// 현재 레뒤룸에 있고, 서버, 체널 룸이 같으면 이미 방에 참여중입니다.
			bool	SameRoom = gcu::IsSameRoom(clan_state);
			UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
			if( UIPHASE_READY_ROOM == pFrame->GetCurrentPhaseType() && SameRoom)
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
			else
			{
				CCommunityContext::i()->SetMovingToClanUser(idx);
				g_pFramework->GetUIMainframe()->OnInviteMoveClan();
			}
		}
		else
		{
			MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*찾아가시려면 친구가 방에 있어야합니다.*/
		}
	}
}

void GameUI::FindAwayNormalUser( void)
{
	const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

	if(pUserInfo->_UserInfo.m_RoomIdx < INIT_0XFFF)
	{
		INT32 i32State = pUserInfo->_UserInfo.m_State;
		INT32 i32ServerIdx = pUserInfo->_UserInfo.m_ServerIdx;
		INT32 i32ChanenlIdx =  pUserInfo->_UserInfo.m_ChannelNum;
		INT32 i32RoomIdx = pUserInfo->_UserInfo.m_RoomIdx;

		UINT32 ui32UserState = static_cast<UINT32>(MAKE_FRIEND_STATE(i32State, i32ServerIdx, i32ChanenlIdx, i32RoomIdx));

		bool	SameRoom = gcu::IsSameRoom(ui32UserState);
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame->GetCurrentPhaseType() == UIPHASE_READY_ROOM && SameRoom)
		{
			MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
		}
		else
		{
			CCommunityContext::i()->SetMovingToNormalUser(ui32UserState);
			//g_pFramework->GetUIMainframe()->OnInviteMove( false );
			UIPhaseLobby * pLobby = static_cast<UIPhaseLobby*>(g_pFramework->GetUIMainframe()->GetPhase(UIPHASE_LOBBY));
			pLobby->SelectRoom(pUserInfo->_UserInfo.m_RoomIdx);
			pLobby->EnterRoom(pUserInfo->_UserInfo.m_RoomIdx);
			
		}
	}
	else
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*찾아가시려면 친구가 방에 있어야합니다.*/
	}
}

static void	cbAddFriend( void* user_p, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		const i3::rc_wstring& wstr_nick = *reinterpret_cast<i3::rc_wstring*>(user_p);
		CCommunityContext::i()->InsertFriend( wstr_nick );
	}
}

void GameUI::AddFriend( const i3::rc_wstring& wstrNick)
{
	UINT32 i;

	if (UserInfoContext::i()->IsMyNickName( wstrNick))
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_ADD_CHARA"));/*자신을 추가할 수 없습니다.*/
		return;
	}
	
	if (CCommunityContext::i()->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_NO_MORE_ADD_FRIEND"));/*더이상 친구를 추가할 수 없습니다.*/
		return;
	}

	for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if(nullptr == node) continue;

		if (i3::generic_is_iequal( wstrNick, node->_wstr_nick) )
		{
			i3::rc_wstring wstr_message;

			i3::sprintf(wstr_message, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_ALREADY_FRIEND"), wstrNick);/*[%s]님은 이미 친구목록에 등록되어 있습니다.*/
			MsgBox( MSG_TYPE_GAME_OK, wstr_message);
			return;
		}
	}

	CCommunityContext::i()->InsertFriend( wstrNick );
	//char szMessage[MAX_STRING_COUNT];
	//sprintf_s( szMessage, GAME_RCSTRING("STR_UI_POPUP_ADD_FRIEND_DETAIL"), pszNick); /*[%s]님께 친구 추가 요청을 보냅니다.*/
	//MsgBox( MSG_TYPE_GAME_QUERY, szMessage, GAME_RCSTRING("STR_POPUP_ADD_FRIEND"), MAKE_CALLBACK(cbAddFriend), (void*) &wstrNick);
}

void GameUI::RemoveFriend( INT32 idx)
{
	const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);

	if (nullptr == node)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*삭제할 친구를 선택하셔야합니다.*/
	}
	else
	{
		CCommunityContext::i()->DeleteViewFriend( idx);
		/*char szMessage[MAX_STRING_COUNT];
		sprintf( szMessage, GAME_RCSTRING("STR_UI_DELETE_FRIEND_DESC"), m_szNick);*/
	}
}

void GameUI::AcceptFriend( INT32 idx)
{
	const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);

	if( node != nullptr)
	{
		CCommunityContext::i()->AcceptViewFriend( idx);
		/*char szMsg[260];
		sprintf_s( szMsg, GAME_RCSTRING("STR_UI_ACCEPT_FRIEND_DESC"), node->_nick );
		MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_ACCEPT_FRIEND")*/
	}
	else
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_ACCEPT"));/*수락하실 친구를 선택하셔야합니다.*/
	}
}

void GameUI::RefuseFriend( INT32 idx, const CMsgBoxCallBack & cbfunc, void * pThis)
{
	const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( idx);

	if (nullptr == node)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_REFUSE"));/*친구요청을 거절할 친구를 선택하셔야합니다.*/
	}
	else
	{
		cbfunc.OnCallback( pThis, 0);
		//char message[MAX_STRING_COUNT];

		//i3::snprintf( message, sizeof(message), GAME_RCSTRING("STR_POPUP_MESSAGE_REFUSE_FRIEND"), node->_nick);/*[%s]님의 친구요청을 거절하시겠습니까?*/
		//MsgBox( MSG_TYPE_GAME_QUERY, message, nullptr, cbfunc, pThis);
	}
}

void GameUI::InviteUser( USERLIST_TAB tab, INT32 idx)
{
	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != nullptr);

	CUserLocationInfo	UserLocInfo;
	if( !GetUserLocationInfo(UserLocInfo, tab, idx))
		return;

	if( UserLocInfo.GetServerNumber() == 254 ) //상대방 친구 수락이 안 된 상태가 255 임..
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WAITING_ACCEPT_FRIEND") ); /*친구 수락 대기 중 입니다.*/
		return;
	}

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	if( pOtherServerInfo == 0 ) return;

	CHECK_ASSERT(pOtherServerInfo != nullptr);

	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_CAN_NOT_INVITE_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_CAN_NOT_INVITE_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}

	// 현재 매치 중인지 아닌지에 대한 체크
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		InviteClanMember( idx);
	}
	else
	{
		InviteFriend( tab, idx );
	}
}

void GameUI::InviteFriend( USERLIST_TAB tab, INT32 idx)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( ( UIPHASE_READY_ROOM != pFrame->GetCurrentPhaseType() ) && ( UIPHASE_CLANLOBBY != pFrame->GetCurrentPhaseType() ))
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_IN_ROOM"));/*방에서만 초대할 수 있습니다.*/
		return;
	}

	// 친구 or 클랜 탭인 경우에는 idx  < 0 이면 선택을 안한것이 됩니다.
	if(tab == USERLIST_TAB_FRIEND || tab == USERLIST_TAB_CLAN)
	{
		if (0 > idx)
		{
			MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
			return;
		}
	}
	
	switch( tab )
	{
	case USERLIST_TAB_FRIEND:
		{
			const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(idx);

			if (nullptr == node)
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
				return;
			}

			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
				return;
			}

			bool	SameRoom = gcu::IsSameRoom(node->_state);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == false)
			{
				CCommunityContext::i()->InviteViewFriend(idx);
			}
			else
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
		}
		break;
 	case USERLIST_TAB_CLAN:
 		{
 			const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember(idx);
 
 			if (nullptr == node)
 			{
 				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
 				return;
 			}
 
 			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
 			{
 				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
 				return;
 			}
 
 			bool	SameRoom = gcu::IsSameRoom(node->_state);
 			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == false)
 			{
 				CCommunityContext::i()->InviteViewClan(idx);
 			}
 			else
 			{
 				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
 			}
 		}
 		break;
	default:
		{
			const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

			if (FRIEND_STATE_ONLINE > pUserInfo->_UserInfo.m_State)
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
				return;
			}

			INT32 i32State = pUserInfo->_UserInfo.m_State;
			INT32 i32ServerIdx = pUserInfo->_UserInfo.m_ServerIdx;	// 서버인덱스는 0부터 시작되므로 -1 합니다.
			INT32 i32ChanenlIdx =  pUserInfo->_UserInfo.m_ChannelNum;
			INT32 i32RoomIdx = pUserInfo->_UserInfo.m_RoomIdx;

			UINT32 i32UserState = static_cast<UINT32>(MAKE_FRIEND_STATE(i32State, i32ServerIdx, i32ChanenlIdx, i32RoomIdx));

			bool	SameRoom = gcu::IsSameRoom(i32UserState);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(i32UserState) || SameRoom == false)
			{
				CCommunityContext::i()->InviteNormalUser();
			}
			else
			{
				MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
		}
		break;
	}
}

void GameUI::InviteClanMember( INT32 idx)
{
	
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if ( ( UIPHASE_CLANLOBBY != pFrame->GetCurrentPhaseType() ))
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_CLAN_IN_LOBBY"));/*클랜 팀 로비에서만 초대할 수 있습니다.*/
		return;
	}

	if (0 > idx)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*초대할 클랜원을 선택하셔야 합니다.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember(idx);

	if (nullptr == node)
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*초대할 클랜원을 선택하셔야 합니다.*/
		return;
	}

	if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
		return;
	}

	if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) ||
		g_pServerInfo->GetCurGameServerIndex() != GET_FRIEND_SERVER(node->_state) ||
		g_pServerInfo->GetCurChannelNumber() != GET_FRIEND_CHANNEL(node->_state) ||
		ClanGameContext::i()->getMyMatchSquadInfo()->getClanTeamIdx() != GET_FRIEND_MATCH( node->_state ) - 1)
	{
		if (CCommunityContext::i()->InviteViewClan(idx))
		{
			i3::rc_wstring wstr_message;
			i3::sprintf( wstr_message, GAME_RCSTRING("STR_POPUP_MESSAGE_SEND_INVITE_MESSAGE"), node->_wstr_nick);/*[%s]님에게 초대 메시지를 전송하였습니다.*/
			MsgBox( MSG_TYPE_GAME_OK, wstr_message);
		}
	}
	else
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_ALREADY_JOIN_TEAM"));/*이미 팀에 참여중입니다.*/
	}
}

void GameUI::InviteClan( const i3::wliteral_range& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis)
{
	//	자기가 클랜 초대 기능이 있을 경우
	if( !g_pFramework->GetClanContext()->IsPossibleClanInvite())
	{	
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NO_HAVE_CLAN_INVITE"));/*클랜 초대 권한이 없습니다.*/
		return;
	}

	if (UserInfoContext::i()->IsMyNickName(wstrNick)) 
	{
		MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_ADD_CHARA"));/*자신을 추가할 수 없습니다.*/
		return;
	}

	i3::rc_wstring wstr_message;

	//	"[ %s ]님에게 클랜 초대를 하시겠습니까?"
	i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_LOBBY_Q_CLAN_INVITE_USER"), wstrNick);
	
	MsgBox( MSG_TYPE_GAME_QUERY, wstr_message, nullptr, cbfunc, pThis);
}

void GameUI::AddBlockList( const i3::rc_wstring& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis)
{
	if (UserInfoContext::i()->IsMyNickName( wstrNick ))
	{
		MsgBox( MSG_TYPE_GAME_QUERY, L"자기자신은 차단할 수 없습니다.", nullptr, cbfunc, pThis);
		return;
	}

	i3::rc_wstring message;

	//	"[ %s ]님을 차단하시겠습니까?"
	i3::sprintf( message, GAME_RCSTRING("STR_TBL_GUI_LOBBY_Q_BLOCK_USER"), wstrNick);
	
	MsgBox( MSG_TYPE_GAME_QUERY, message, nullptr, cbfunc, pThis);
}

void GameUI::SetClanTexture( i3UIImageBoxEx * pImgBox, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag)
{
	UINT32 nBackImageIdx = CLANMARK_FRAME_IMAGE_IDX(nClanMarkFlag);
	UINT32 nShapeImageIdx = CLANMARK_CONTENT_IMAGE_IDX(nClanMarkFlag);
	UINT32 nBackColorIdx = CLANMARK_FRAME_COLOR_IDX(nClanMarkFlag);
	UINT32 nShapeColorIdx = CLANMARK_CONTENT_COLOR_IDX(nClanMarkFlag);

	i3Texture * pTex;
	INT32 texU, texV, texW, texH;

	if( nBackImageIdx == 0 && nShapeImageIdx == 0 && nBackColorIdx == 0 && nShapeColorIdx == 0)
	{// 클랜마크가 없을 경우  Default Mark 사용
		pTex = g_pClanMarkManager->GetDefaultClanMarkTex( size);
		texU = g_pClanMarkManager->GetDefaultClanMarkU( size);
		texV = g_pClanMarkManager->GetDefaultClanMarkV( size);
		texW = texH = g_pClanMarkManager->GetDefaultClanMarkWH( size);
	}
	else if( MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx) == CLAN_MARK_FLAG_NONE)
	{	//	비클랜원이다. 각 Index가 255를 넘어서면 안된다.
		pImgBox->EnableCtrl( false);
		pImgBox->DeleteShapeAll();
		return;
	}
	else if( nBackImageIdx == 0 || nShapeImageIdx == 0 || nBackColorIdx == 0 || nShapeColorIdx == 0 )
	{	//	인덱스는 1 base 이다. 하나라도 0이면 인자가 잘못된 것입니다.
		return;
	}
	else
	{
		//	클랜 마크 이미지
		ClanMarkInfo * pClanMarkInfo = g_pClanMarkManager->GetClanMarkTex( size, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
		I3ASSERT( pClanMarkInfo != nullptr);

		pTex = pClanMarkInfo->_image;
		texU = pClanMarkInfo->_texU;
		texV = pClanMarkInfo->_texV;
		texW = pClanMarkInfo->_texW;
		texH = pClanMarkInfo->_texH;
	}

	INT32 finder = pImgBox->FindShape( pTex, texU, texV, texW, texH);
	if( finder == -1)
	{
		finder = pImgBox->AddTexture( pTex, texU, texV, texW, texH);
		pImgBox->CreateBaseSceneObjects();
	}
	
	I3ASSERT( finder != -1);
	pImgBox->SetCurrentImage( finder);

	if( size == CLAN_MARK_SMALL)
		pImgBox->SetShapeSize( 21.f, 21.f);
	else
		pImgBox->SetShapeSize( (REAL32)texW, (REAL32)texH);

	pImgBox->EnableCtrl( true);
}

void GameUI::SetClanTextureAndCenterPos( i3UIImageBoxEx * pImgBox, i3UIControl * pControl, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag, REAL32 rShapeSize)
{
	SetClanTexture( pImgBox, size, nClanMarkFlag);
	pImgBox->SetShapeSize(rShapeSize, rShapeSize);

	REAL32 w = pControl->getWidth();
	REAL32 h = pControl->getHeight();

	REAL32 newPosX = (w - pImgBox->getWidth()) * 0.5f;
	REAL32 newPosY = (h - pImgBox->getHeight()) * 0.5f;

	/*if( newPosX < 0.f)
		newPosX = 0.f;
	if( newPosY < 0.f)
		newPosY = 0.f;*/

	pImgBox->setPos( newPosX, newPosY);
}

void GameUI::SetClanTextureForListView( i3UIListView_Cell * pCell, void * pData)
{
	I3ASSERT( pCell != nullptr);
	if( pCell->getCellChild() != nullptr)
	{
		i3UIControl * pCtrl = pCell->getCellChild();

		if( i3::same_of<i3UIImageBoxEx*>(pCtrl))
		{
			UINT32 markFlag = GetClanMarkFlag((char *)pData);
			SetClanTextureAndCenterPos( (i3UIImageBoxEx*) pCtrl, pCell, CLAN_MARK_SMALL, markFlag, 20.0f);
		}
	}
}

UINT32 GameUI::GetClanMarkFlag(const char * pszData)
{
	char szData[4][4];

	INT32 letter = 0;
	INT32 count = 0;

	while( pszData)
	{
		szData[count][letter++] = (char)(*pszData);
		pszData++;
		if( letter > 2)
		{
			szData[count][letter] = 0;
			count++;
			letter = 0;

			if( count > 3)
				break;
		}				
	}

	UINT32 back_image_idx = MIN( ::atoi( (const char*) szData[0]), 255);
	UINT32 shape_image_idx = MIN( ::atoi( (const char*) szData[1]), 255);
	UINT32 back_color_idx = MIN( ::atoi( (const char*) szData[2]), 255);
	UINT32 shape_color_idx = MIN( ::atoi( (const char*) szData[3]), 255);

	return MAKE_CLANMARK_FLAG(back_image_idx, shape_image_idx, back_color_idx, shape_color_idx);
}

INT32 GameUI::SetClanMarkToLua( LuaState * L, UINT32 mark)
{
	if( mark != 0xFFFFFFFF)
	{
		UINT32 nBackImageIdx = CLANMARK_FRAME_IMAGE_IDX(mark);
		UINT32 nShapeImageIdx = CLANMARK_CONTENT_IMAGE_IDX(mark);
		UINT32 nBackColorIdx = CLANMARK_FRAME_COLOR_IDX(mark);
		UINT32 nShapeColorIdx = CLANMARK_CONTENT_COLOR_IDX(mark);

		I3ASSERT( nBackImageIdx < 0xFF );
		I3ASSERT( nShapeImageIdx < 0xFF );
		I3ASSERT( nBackColorIdx < 0xFF );
		I3ASSERT( nShapeColorIdx < 0xFF );

		i3Lua::PushInteger( L, nBackImageIdx);
		i3Lua::PushInteger( L, nShapeImageIdx);
		i3Lua::PushInteger( L, nBackColorIdx);
		i3Lua::PushInteger( L, nShapeColorIdx);

		i3Lua::PushUserData( L, (void*) SetClanTextureForListView);
	}
	else
	{
		i3Lua::PushInteger( L, 255);
		i3Lua::PushInteger( L, 255);
		i3Lua::PushInteger( L, 255);
		i3Lua::PushInteger( L, 255);

		i3Lua::PushUserData( L, (void*)SetClanTextureForListView);
	}

	return 5;	// 5개의 Stack Push count
}

UI_MODE_TYPE GameUI::GetStageModeType( CSI_STAGE* stage, bool isClanMatch)
{
	if (isClanMatch)											return UMT_CLANMATCH;
	if (stage != nullptr)
	{
		
		if (stage->IsAiModeforAiType(AI_USE_MADNESS))				return UMT_MADDNES;
		else if (stage->IsAiMode())									return UMT_AI;

		if (stage->IsHeadShotOnly())								return UMT_HEADHUNTER;
		if (stage->IsHudOff() && stage->IsOneShotOneKill())			return UMT_CHAOS;
		if (stage->IsSpace())										return UMT_SPACE;
		if (stage->IsHudOff() && stage->IsModeforModeType(STAGE_MODE_BOMB))		return UMT_HARDCORE_BOMB;
		if (stage->IsHudOff() && stage->IsModeforModeType(STAGE_MODE_CONVOY))	return UMT_HARDCORE_CONVOY;

		if (stage->IsModeforModeType(STAGE_MODE_DEATHMATCH))		return UMT_DEATHMATCH;
		if (stage->IsModeforModeType(STAGE_MODE_BOMB))				return UMT_BOMBMISSION;
		if (stage->IsModeforModeType(STAGE_MODE_DESTROY))			return UMT_DESTROY;
		if (stage->IsModeforModeType(STAGE_MODE_ANNIHILATION))		return UMT_ANNIHILATION;
		if (stage->IsModeforModeType(STAGE_MODE_DEFENCE))			return UMT_DEFENCE;
		if (stage->IsModeforModeType(STAGE_MODE_ESCAPE))			return UMT_ESCAPE;
		if (stage->IsModeforModeType(STAGE_MODE_TUTORIAL))			return UMT_TUTORIAL;
		if (stage->IsModeforModeType(STAGE_MODE_CROSSCOUNT))		return UMT_CCMODE;
		if (stage->IsModeforModeType(STAGE_MODE_CONVOY))			return UMT_CONVOY;
		if (stage->IsModeforModeType(STAGE_MODE_RUN_AWAY))			return UMT_RUN_AWAY;
	}
	return UMT_NONE;
}

INT32 GameUI::GetModeOptionShapeIndex(ROOM_INFO_BASIC & roomInfo)
{
	// 무기 제한이 없는 경우 50부터 시작되나, ULM_NONE 값이 -1이기 때문에, 51부터 인덱싱을 시작합니다.
	// team_chamge_shape 와 limit_flag를 더하여 값을 반환합니다.
	// ex) 1. 공수교대 비활성 && 무기제한 없음. (0 + (-1))				: -1로 반환되어 모드 아이콘 비활성.
	// ex) 2. 공수교대 비활성 && 샷건전 제한.	(0 + ULM_SHOTGUN(4))	: 54로 반환되어 샷건전 아이콘 활성.
	// ex) 3. 공수교대 활성 && 무기제한 없음.	(51 + (-1))				: 50으로 반환되어 공수교대 아이콘 활성
	// ex) 4. 공수교대 활성 && 샷건전 제한.		(51 + ULM_SHOTGUN(4))	: 55로 반환되어 공수교대 + 샷건전 아이콘 활성

	// 공수교대용 Shape 시작 인덱스
	const INT32 AUTO_TEAM_CHANGE_SHAPE_IDX = 51;

	// 공수교대 설정 여부 검사
	INT32 team_change_shape = 0;
	{
		if ( GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(roomInfo._InfoFlag) == 1)
		{
			team_change_shape = AUTO_TEAM_CHANGE_SHAPE_IDX;
		}
	}

	// 무기 제한 설정 여부 검사
	UI_LIMIT_MODE limit_flag = ULM_NONE;
	{
		UINT8 weapon_flag = roomInfo._WeaponFlag;

		if ( GET_ROOM_INFO_WEAPON_SNIPER(weapon_flag) == 1)			limit_flag = ULM_SNIPER;	// 저격전
		else if ( GET_ROOM_INFO_WEAPON_SHOTGUN(weapon_flag) == 1)	limit_flag = ULM_SHOTGUN;	// 샷건전
		else if ( GET_ROOM_INFO_WEAPON_KNUCKLE(weapon_flag) == 1)	limit_flag = ULM_KNUCKLE;	// 너클전
		else if ( GET_ROOM_INFO_WEAPON_00(weapon_flag) == 0)	//	주무기 lock
		{
			if ( GET_ROOM_INFO_WEAPON_01(weapon_flag) == 0)		//	보조무기 lock
			{
				if ( GET_ROOM_INFO_WEAPON_RPG7(weapon_flag) == 1)	limit_flag = ULM_ROCKET;	// RPG
				else if ( GET_ROOM_INFO_WEAPON_02(weapon_flag) == 1)
				{
					// 근접 무기가 lock 일 경우는 너클전/칼전이다.
					// 구분은 해당 StageMask로 구분한다.
					if ( GET_ROOM_INFO_WEAPON_KNUCKLE(weapon_flag) == 1)	limit_flag = ULM_KNUCKLE;	// 너클전
					else													limit_flag = ULM_KNIFE;		// 칼전
				}
				else
				{
					I3PRINTLOG(I3LOG_FATAL, "Invalid weaponflag!!!");
				}
			}
			else if ( GET_ROOM_INFO_WEAPON_RPG7(weapon_flag) == 1)	limit_flag = ULM_ROCKET;	// RPG전
			else													limit_flag = ULM_PISTOL;	// 권총전
		}
	}

	return (team_change_shape + limit_flag);
}

INT32 GameUI::GetMapImageShapeIndex( CSI_STAGE* stageType)
{
	if (stageType->IsHeadShotOnly())								return -1;

	switch( stageType->GetStageMode() )
	{
	case STAGE_MODE_DEATHMATCH:			return 2;
	case STAGE_MODE_BOMB:				return 0;
	case STAGE_MODE_DESTROY:			return 3;
	case STAGE_MODE_ANNIHILATION:		return 1;
	case STAGE_MODE_DEFENCE:			return 6;
	case STAGE_MODE_ESCAPE:				return 5;
	case STAGE_MODE_CROSSCOUNT:			return 9;
	default: return 2;
	}
}

UIMessageBox * GameUI::MsgBox( MSG_TYPE type, const i3::rc_wstring& wstr_message, const i3::rc_wstring* pwstr_caption, const CMsgBoxCallBack & cbfunc, void * pThis, INT32 fontSize)
{
//	char szCaption[ MAX_STRING_COUNT];
	i3::rc_wstring wstrCaption;
	I3ASSERT( g_pFramework != nullptr);
	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();
	I3ASSERT( pManager != nullptr);

	UIMessageBox * pMessageBox = nullptr;

	if( pwstr_caption )
		wstrCaption = *pwstr_caption;
	else
		wstrCaption = GAME_RCSTRING( "STBL_IDX_COMMON_MSGBOX_CAPTION");
	
//	char msg[ MAX_STRING_COUNT * 2];

	switch( type)
	{
	case MSG_TYPE_ERROR :
		{
			pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_ERROR, cbfunc, pThis, fontSize, 1, 1);

			I3PRINTLOG(I3LOG_NOTICE, wstr_message );		
		}
		break;
	case MSG_TYPE_SYSTEM_OK :	pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_SYSTEM_OK, cbfunc, pThis, fontSize, 1, 1);			break;
	case MSG_TYPE_SYSTEM_QUERY : pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_SYSTEM_OKCANCEL, cbfunc, pThis, fontSize, 1, 1);	break;
	case MSG_TYPE_SYSTEM_WAIT : pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_WAIT, cbfunc, pThis, fontSize, 1, 1);			break;
	case MSG_TYPE_GAME_OK :		pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_OK, cbfunc, pThis, fontSize, 1, 1);				break;
	case MSG_TYPE_GAME_QUERY :	pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_OKCANCEL, cbfunc, pThis, fontSize, 1, 1);		break;
	case MSG_TYPE_GAME_WAIT :	pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_WAIT, cbfunc, pThis, fontSize, 1, 1);			break;
	case MSG_TYPE_GAME_USER :	pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_USER_OKCANCEL, cbfunc, pThis, fontSize, 1, 1);	break;
	case MSG_TYPE_WEB:			pMessageBox = pManager->OpenMessageBox(wstr_message, wstrCaption, MBF_WEB, cbfunc, pThis, fontSize, 1, 1);				break;
	case MSG_TYPE_BLOCK_NOTICE:	pMessageBox = pManager->OpenMessageBox(wstr_message, wstrCaption, MBF_SYSTEM_BLOCKNOTICE, cbfunc, pThis, fontSize, 1, 1);	break;
	case MSG_TYPE_INBATTLE :
		if( g_pFramework->IsBattleStage())
		{
			i3::stack_wstring wstr_msg;
			i3::sprintf( wstr_msg, GAME_RCSTRING("STR_TBL_NETWORK_EMERGENCY_NOTICE"), wstr_message);/*{col:255,120,0,255}[긴급공지] %s{/col}*/
			g_pFramework->getChatBox()->PutSystemChatting( wstr_msg );
		}
		else
		{
			pMessageBox = pManager->OpenMessageBox( wstr_message, wstrCaption, MBF_GAME_OK, cbfunc, pThis, fontSize, 1, 1);			break;
		}
		break;
	default:
		I3PRINTLOG(I3LOG_FATAL,  "invalid message type!!! %d", type);
		break;
	}

	return pMessageBox;
}

UIMessageBox * GameUI::WebBox(const i3::rc_wstring& wstr_message, SIZE32 Size, const i3::rc_wstring& URL)
{
	MsgBoxOpenParam	Param;
	Param.Msg		= wstr_message;
	Param.Caption	= wstr_message;
	Param.Style		= MBF_WEB;
	Param.FontSize	= 10;
	Param.AlignX	= 1;
	Param.AlignY	= 1;
	Param.WebURL	= URL;
	Param.WebScreenSize	= Size;

	I3ASSERT( g_pFramework != nullptr);

	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();
	I3ASSERT( pManager != nullptr);

	return pManager->OpenMessageBox( Param);
}

void GameUI::CloseMsgBox( UIMessageBox * pMsgBox)
{
	I3ASSERT( pMsgBox != nullptr);
	I3ASSERT( g_pFramework != nullptr);
	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();

	I3ASSERT( pManager != nullptr);
	pManager->CloseMessageBox( pMsgBox);
}

void GameUI::CloseAllMsgBox()
{
	I3ASSERT( g_pFramework != nullptr);
	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();

	I3ASSERT( pManager != nullptr);
	pManager->CloseAllMessageBox();
}

void GameUI::CloseGameMsgBox()
{
	I3ASSERT( g_pFramework != nullptr);
	CMessageBoxManager * pManager = g_pFramework->GetMessageBoxManager();

	I3ASSERT( pManager != nullptr);
	pManager->CloseGameMessageBox();
}

void GameUI::PrintSystemChat( const i3::rc_wstring& str)
{
	I3ASSERT( g_pFramework != nullptr);
	I3ASSERT( g_pFramework->getChatBox() != nullptr);

	g_pFramework->getChatBox()->PutSystemChatting(str);
}


//
// SetEvent에서 페이즈전환을 위한 패킷을 보낼때 리시브 받는 동안 입력을 막기 위한 조건을 설정합니다.
//

void	GameUI::ProcessInputDisableChangePhase(INT32 eventID)
{
	switch(eventID)
	{
	// 메인페이즈
	case EVENT_ENTER_LOBBY:
	
	// 레디룸페이즈로 들어가기 위한 샌드패킷으로, 실패시에는 실패한 곳에서 입력을 푼다..
	case EVENT_JOIN_ROOM:
	case EVENT_CREATE_ROOM:
	case EVENT_JOIN_QUICK:

	// 서브페이즈..
	case EVENT_ENTER_INVENTORY:
	case EVENT_ENTER_SHOP:
	case EVENT_ENTER_INFO:
	case EVENT_ENTER_ROOM :
		i3UI::setDisableUIInput(true);
		break;

	default:	break;
	}
}

void GameUI::ErrorHandleStage( INT32 GameEvent, INT32 arg)
{
	if( EV_IS_ERROR( arg, EVENT_ERROR_GAME_WRONG_POSITION))
	{// 잘못된 위치
		// 로비로 이동합니다.
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		I3ASSERT( pFrame != nullptr);
		pFrame->MovePhase( UIPHASE_LOBBY);
	}
}

INT32 GameUI::GetEsportsTemplateIndex(ESPORT_ID_LEVEL EsId)
{
	INT32 eSport = -1;

	switch( EsId)
	{
	case ESPORT_ID_LEVEL_ZERO:	eSport = -1;						break;
	case ESPORT_ID_LEVEL_BEGINNER1:		eSport = 0;		break;
	case ESPORT_ID_LEVEL_BEGINNER2:		eSport = 1;		break;
	case ESPORT_ID_LEVEL_BEGINNER3:		eSport = 2;		break;
	case ESPORT_ID_LEVEL_SUBSTITUTE1:	eSport = 3;		break;
	case ESPORT_ID_LEVEL_SUBSTITUTE2:	eSport = 4;		break;
	case ESPORT_ID_LEVEL_STARTER1:		eSport = 5;		break;
	case ESPORT_ID_LEVEL_STARTER2:		eSport = 6;		break;
	case ESPORT_ID_LEVEL_STAR1:			eSport = 7;		break;
	case ESPORT_ID_LEVEL_STAR2:			eSport = 8;		break;
	case ESPORT_ID_LEVEL_STAR3:			eSport = 9;		break;
	case ESPORT_ID_LEVEL_SUPERSTAR1:	eSport = 10;	break;
	case ESPORT_ID_LEVEL_SUPERSTAR2:	eSport = 11;	break;
	case ESPORT_ID_LEVEL_SUPERSTAR3:	eSport = 12;	break;
	case ESPORT_ID_LEVEL_SUPERSTAR4:	eSport = 13;	break;
	case ESPORT_ID_LEVEL_WORLDCLASS1:	eSport = 14;	break;
	case ESPORT_ID_LEVEL_WORLDCLASS2:	eSport = 15;	break;
	case ESPORT_ID_LEVEL_WORLDCLASS3:	eSport = 16;	break;
	case ESPORT_ID_LEVEL_LEGEND1:		eSport = 17;	break;
	case ESPORT_ID_LEVEL_LEGEND2:		eSport = 18;	break;
	case ESPORT_ID_LEVEL_HALLOFFAME:	eSport = 19;	break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "Esports 레벨이 없다.");
	}

	return eSport;
}

INT32 GameUI::GetCharaImageIndex( T_ItemID itemID )
{
	switch( itemID )
	{
	case	601001: return 4;		// 레드불스
	case	602002: return 0;		// 애시드폴
	case	601003: return 1;		// 타란튤라
	case	602004: return 7;		// 킨아이즈
	case	601005: return 3;		// 바이퍼레드
	case	601007: return 6;		// 디폭스
	case	602008: return 2;		// 레오파드
	case	602009: return 5;		// 하이드
	case    601069: return 8;		// 그레이
	case    602070: return 9;		// 울프
	}

	return -1;
}

i3::rc_wstring	GameUI::GetKeymenuString( GAME_KEY_MAP key_map)
{
	switch( key_map )
	{
		// 이동
	case GAME_KEY_MAP_LEFT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_LEFT");
	case GAME_KEY_MAP_RIGHT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_RIGHT");
	case GAME_KEY_MAP_FORWARD: return GAME_RCSTRING("STBL_IDX_GAMEKEY_FORWARD");
	case GAME_KEY_MAP_BACKWARD: return GAME_RCSTRING("STBL_IDX_GAMEKEY_BACKWARD");
	case GAME_KEY_MAP_JUMP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_JUMP");
	case GAME_KEY_MAP_FOOTSTEP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_FOOTSTEP");
	case GAME_KEY_MAP_CROUCH: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CROUCH");

		// 무기사용
	case GAME_KEY_MAP_ATTACK: return GAME_RCSTRING("STBL_IDX_GAMEKEY_ATTACK");
	case GAME_KEY_MAP_SUBFUNC: return GAME_RCSTRING("STBL_IDX_GAMEKEY_SUBFUNC");
	case GAME_KEY_MAP_RELOAD: return GAME_RCSTRING("STBL_IDX_GAMEKEY_RELOAD");
	case GAME_KEY_MAP_TURNBACK: return GAME_RCSTRING("STBL_IDX_GAMEKEY_TURNBACK");
	case GAME_KEY_MAP_DROP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_DROP");
	case GAME_KEY_MAP_WEAPON_EXTENSION: return GAME_RCSTRING("STR_OPT_TAP_CTR_EXT2");

		// 무기교체
	case GAME_KEY_MAP_CHANGEWEAPON: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON");
	case GAME_KEY_MAP_CHANGEWEAPON_1: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_1");
	case GAME_KEY_MAP_CHANGEWEAPON_2: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_2");
	case GAME_KEY_MAP_CHANGEWEAPON_3: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_3");
	case GAME_KEY_MAP_CHANGEWEAPON_4: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_4");
	case GAME_KEY_MAP_CHANGEWEAPON_5: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_5");
	case GAME_KEY_MAP_CHANGEWEAPON_6: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_6");
	case GAME_KEY_MAP_CHANGEWEAPON_QUICK: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_QUICK");
	case GAME_KEY_MAP_CHANGEWEAPON_PREV: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_PREV");
	case GAME_KEY_MAP_CHANGEWEAPON_NEXT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHANGEWEPON_NEXT");

		// 기타
	case GAME_KEY_MAP_SHOWSCORE: return GAME_RCSTRING("STBL_IDX_GAMEKEY_SHOWSCORE");
	case GAME_KEY_MAP_CHAT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHAT");
	case GAME_KEY_MAP_VOICECHAT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_VOICECHAT");
	case GAME_KEY_MAP_RADIOMESSAGE1: return GAME_RCSTRING("STR_OPT_TAP_AUDIO_RADIO_KEY_TEAM");	/*STBL_IDX_GAMEKEY_RADIOMESSAGE1*/
	case GAME_KEY_MAP_RADIOMESSAGE2: return GAME_RCSTRING("STR_OPT_TAP_AUDIO_RADIO_KEY_REQUEST");	/*STBL_IDX_GAMEKEY_RADIOMESSAGE2*/
	case GAME_KEY_MAP_RADIOMESSAGE3: return GAME_RCSTRING("STR_OPT_TAP_AUDIO_RADIO_KEY_REPORTS");	/*STBL_IDX_GAMEKEY_RADIOMESSAGE3*/
	case GAME_KEY_MAP_SPECIALACTION: return GAME_RCSTRING("STBL_IDX_GAMEKEY_SPECIALACTION");
	case GAME_KEY_MAP_MOUSE_SENS_UP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_MOUSESENEUP");
	case GAME_KEY_MAP_MOUSE_SENS_DOWN: return GAME_RCSTRING("STBL_IDX_GAMEKEY_MOUSESENEDOWN");
	case GAME_KEY_MAP_SCREENSHOT: return GAME_RCSTRING("STBL_IDX_GAMEKEY_SCREENSHOT");
	case GAME_KEY_MAP_VIDEOCAPTURE: return GAME_RCSTRING("STBL_IDX_GAMEKEY_VIDEOCAPTURE");
	//case GAME_KEY_MAP_TOGGLE_MAP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_TOGGLEMAP");
	case GAME_KEY_MAP_MINIMAP_ENLARGE: return GAME_RCSTRING("STBL_IDX_GAMEKEY_MINIMAP_ENLARGE");
	case GAME_KEY_MAP_MINIMAP_REDUCTION: return GAME_RCSTRING("STBL_IDX_GAMEKEY_MINIMAP_REDUCTION");
	case GAME_KEY_MAP_CHAT_ALL: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHAT_ALL");
	case GAME_KEY_MAP_CHAT_TEAM: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHAT_TEAM");
	case GAME_KEY_MAP_CHAT_CLAN: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHAT_CLAN");
	case GAME_KEY_MAP_CHAT_HISTORY: return GAME_RCSTRING("STBL_IDX_GAMEKEY_CHAT_HISTORY");

		// 야전 상점
	case GAME_KEY_MAP_INGAME_SHOP: return GAME_RCSTRING("STBL_IDX_GAMEKEY_INGAME_SHOP");
	}

	return i3::rc_wstring();
}

i3::rc_wstring GameUI::GetKeyboardString( GAME_KEY_STRUCT *pGameKey)
{
	I3ASSERT( pGameKey != nullptr);

	if( pGameKey->_nDiviceType == GAME_DEVICE_KEYBOARD)
	{
		switch( pGameKey->_nState)
		{
		case I3I_KEY_UNKNOWN:					return	GAME_RCSTRING("STR_POPUP_NO_USE");/*[사용안함]*/

		case I3I_KEY_0:							return 	L"0";
		case I3I_KEY_1:							return 	L"1";
		case I3I_KEY_2:							return 	L"2";
		case I3I_KEY_3:							return 	L"3";
		case I3I_KEY_4:							return  L"4";
		case I3I_KEY_5:							return 	L"5";
		case I3I_KEY_6:							return 	L"6";
		case I3I_KEY_7:							return 	L"7";
		case I3I_KEY_8:							return 	L"8";
		case I3I_KEY_9:							return 	L"9";

		case I3I_KEY_A:							return 	L"A";
		case I3I_KEY_B:							return  L"B";
		case I3I_KEY_C:							return 	L"C";
		case I3I_KEY_D:							return 	L"D";
		case I3I_KEY_E:							return 	L"E";
		case I3I_KEY_F:							return 	L"F";
		case I3I_KEY_G:							return 	L"G";
		case I3I_KEY_H:							return 	L"H";
		case I3I_KEY_I:							return 	L"I";
		case I3I_KEY_J:							return 	L"J";
		case I3I_KEY_K:							return 	L"K";
		case I3I_KEY_L:							return 	L"L";
		case I3I_KEY_M:							return 	L"M";
		case I3I_KEY_N:							return 	L"N";
		case I3I_KEY_O:							return 	L"O";
		case I3I_KEY_P:							return 	L"P";
		case I3I_KEY_Q:							return 	L"Q";
		case I3I_KEY_R:							return 	L"R";
		case I3I_KEY_S:							return 	L"S";
		case I3I_KEY_T:							return 	L"T";
		case I3I_KEY_U:							return 	L"U";
		case I3I_KEY_V:							return 	L"V";
		case I3I_KEY_W:							return 	L"W";
		case I3I_KEY_X:							return  L"X";
		case I3I_KEY_Y:							return 	L"Y";
		case I3I_KEY_Z:							return 	L"Z";

		case I3I_KEY_ESC:						return 	L"ESC";
		case I3I_KEY_ENTER:						return 	L"Enter";
		case I3I_KEY_LCTRL:						return 	L"L-Ctrl";
		case I3I_KEY_RCTRL:						return 	L"R-Ctrl";
		case I3I_KEY_LSHIFT:					return 	L"L-Shift";
		case I3I_KEY_RSHIFT:					return 	L"R-Shift";
		case I3I_KEY_LALT:						return 	L"L-Alt";
		case I3I_KEY_RALT:						return 	L"R-Alt";
		case I3I_KEY_SPACE:						return  L"Space";
		case I3I_KEY_LEFT:						return 	L"Left";
		case I3I_KEY_RIGHT:						return 	L"Right";
		case I3I_KEY_UP:						return 	L"Up";
		case I3I_KEY_DOWN:						return 	L"Down";
		case I3I_KEY_PGUP:						return 	L"Page up";
		case I3I_KEY_PGDN:						return 	L"Page down";
		case I3I_KEY_HOME:						return 	L"Home";
		case I3I_KEY_END:						return 	L"End";
		case I3I_KEY_DEL:						return 	L"Delete";
		case I3I_KEY_INS:						return 	L"Insert";
		case I3I_KEY_TAB:						return 	L"Tab";
		case I3I_KEY_NUMLOCK:					return 	L"Num Lock";
		case I3I_KEY_CAPSLOCK:					return 	L"Caps Lock";
		case I3I_KEY_SCROLLLOCK:				return 	L"Scroll Lock";
		case I3I_KEY_BREAK:						return 	L"Break";
		case I3I_KEY_PRTSC:						return 	L"Print Screen";
		case I3I_KEY_WIN:						return 	L"Window Key";
		case I3I_KEY_BACK:						return 	L"Back Space";

		case I3I_KEY_F1:						return 	L"F1";
		case I3I_KEY_F2:						return 	L"F2";
		case I3I_KEY_F3:						return 	L"F3";
		case I3I_KEY_F4:						return 	L"F4";
		case I3I_KEY_F5:						return 	L"F5";
		case I3I_KEY_F6:						return 	L"F6";
		case I3I_KEY_F7:						return 	L"F7";
		case I3I_KEY_F8:						return 	L"F8";
		case I3I_KEY_F9:						return 	L"F9";
		case I3I_KEY_F10:						return 	L"F10";
		case I3I_KEY_F11:						return 	L"F11";
		case I3I_KEY_F12:						return 	L"F12";

		case I3I_KEY_NUMPAD0:					return 	L"Numpad 0";
		case I3I_KEY_NUMPAD1:					return 	L"Numpad 1";
		case I3I_KEY_NUMPAD2:					return 	L"Numpad 2";
		case I3I_KEY_NUMPAD3:					return 	L"Numpad 3";
		case I3I_KEY_NUMPAD4:					return 	L"Numpad 4";
		case I3I_KEY_NUMPAD5:					return 	L"Numpad 5";
		case I3I_KEY_NUMPAD6:					return 	L"Numpad 6";
		case I3I_KEY_NUMPAD7:					return 	L"Numpad 7";
		case I3I_KEY_NUMPAD8:					return 	L"Numpad 8";
		case I3I_KEY_NUMPAD9:					return 	L"Numpad 9";
		case I3I_KEY_NUMPADDIVIDE:				return 	L"Numpad Div";
		case I3I_KEY_NUMPADMULTIPLY:			return 	L"Numpad Mul";
		case I3I_KEY_NUMPADMINUS:				return 	L"Numpad Minus";
		case I3I_KEY_NUMPADPLUS:				return 	L"Numpad Plus";
		case I3I_KEY_NUMPADENTER:				return 	L"Numpad Enter";
		case I3I_KEY_NUMPADDEL:					return 	L"Numpad Del";

		case I3I_KEY_MINUS:						return	L"-";
		case I3I_KEY_EQUAL:						return	L"=";
		case I3I_KEY_BACKSLASH:					return	L"\\";
		case I3I_KEY_LBRACKET:					return	L"[";
		case I3I_KEY_RBRACKET:					return	L"]";
		case I3I_KEY_SEMICOLON:					return 	L";";
		case I3I_KEY_APOSTROPHE:				return 	L"'";
		case I3I_KEY_COMMA:						return 	L",";
		case I3I_KEY_PERIOD:					return 	L".";
		case I3I_KEY_SLASH:						return 	L"/";
		case I3I_KEY_GRAVE:						return	L"~";
		default: break;
		}
	}
	else if( pGameKey->_nDiviceType == GAME_DEVICE_MOUSE)
	{
		switch( pGameKey->_nState)
		{
		case I3I_MOUSE_LBUTTON:						return GAME_RCSTRING("STR_POPUP_MOUSE_LEFT_BUTTON");/*마우스 좌측버튼*/
		case I3I_MOUSE_RBUTTON:						return GAME_RCSTRING("STR_POPUP_MOUSE_RIGHT_BUTTON");/*마우스 우측버튼*/
		case I3I_MOUSE_MBUTTON:						return GAME_RCSTRING("STR_POPUP_MOUSE_WHEEL_BUTTON");/*마우스 휠버튼*/
		case I3I_MOUSE_WHEELUP:						return GAME_RCSTRING("STR_POPUP_MOUSE_WHEEL_UP");/*마우스휠 올리기*/
		case I3I_MOUSE_WHEELDOWN:					return GAME_RCSTRING("STR_POPUP_MOUSE_WHEEL_DOWN");/*마우스휠 내리기*/
		default: break;
		}
	}
	
	return GAME_RCSTRING("STR_POPUP_NO_USE");/*[사용안함]*/
}

i3::rc_wstring GameUI::GetKeymapString( GAME_KEY_MAP key_map)
{
	GAME_KEY_STRUCT* gk = g_pEnvSet->getGameKeyMap() +  ptrdiff_t( key_map );
	return GameUI::GetKeyboardString( gk);
}

void GameUI::RemoveColorKeyword( const char * pStr, char * OutStr, INT32 OutStrSize)
{
	string	str = pStr;
	RemoveColorKeyword(str);

	i3::safe_string_copy(OutStr, str.c_str(), OutStrSize);
}

void GameUI::RemoveColorKeyword( std::string & str)
{
	enum
	{
		KEYWORD_BEGIN_SIZE = 5,	// "{col:"
		KEYWORD_END_SIZE = 5,	// "{/col"
	};

	std::string	test_str = str;
	std::transform(test_str.begin(), test_str.end(), test_str.begin(), tolower);

	while (true)
	{
		// 시작 키워드 삭제 (대소문자 구분함)
		size_t sIdx = test_str.find("{col:");
		if ( sIdx == string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_BEGIN_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}

	while (true)
	{
		// 종료 키워드 삭제 (대소문자 구분함)
		size_t sIdx = test_str.find("{/col");
		if ( sIdx == string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_END_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}
}

void	GameUI::RemoveColorKeyword( i3::string& str)
{
	
	for (;;)
	{
		int erase_pos1 = i3::icontain_string(str, "{col:" );
		if (erase_pos1 == -1) 
			break;
		
		int	erase_pos1_end = erase_pos1 + 5;
		i3::iterator_range<const char*> it_rng = i3::make_iterator_range( str.begin() + erase_pos1_end, str.end() );
		int brace_pos = i3::contain_char(it_rng, '}');
		if (brace_pos != -1)
			erase_pos1_end += brace_pos + 1;
		
		str.erase( str.begin() + erase_pos1, str.begin() + erase_pos1_end);
	}
	
	for (;;)
	{
		int erase_pos2 = i3::icontain_string(str, "{/col}");
		if (erase_pos2 == -1)
			break;
		str.erase( str.begin() + erase_pos2, str.begin() + erase_pos2 + 6);
	}
		
}

namespace
{

template<class WCharCtn>
bool	RemoveColorKeywordImpl( WCharCtn& wstr)
{
	bool bChanged = false;
	for (;;)
	{
		int erase_pos1 = i3::icontain_string(wstr, L"{col:" );
		if (erase_pos1 == -1) 
			break;

		int	erase_pos1_end = erase_pos1 + 5;
		i3::const_wchar_range it_rng = i3::make_iterator_range( wstr.begin() + erase_pos1_end, wstr.end() );
		int brace_pos = i3::contain_char(it_rng, L'}');
		if (brace_pos != -1)
			erase_pos1_end += brace_pos + 1;

		wstr.erase( wstr.begin() + erase_pos1, wstr.begin() + erase_pos1_end);
		bChanged = true;
	}

	for (;;)
	{
		int erase_pos2 = i3::icontain_string(wstr, L"{/col}");
		if (erase_pos2 == -1)
			break;
		wstr.erase( wstr.begin() + erase_pos2, wstr.begin() + erase_pos2 + 6);
		bChanged = true;
	}

	return bChanged;
}

}

void	GameUI::RemoveColorKeyword( i3::stack_wstring& wstr)
{
	RemoveColorKeywordImpl(wstr);
}

void	GameUI::RemoveColorKeyword( i3::wstring& wstr)
{
	RemoveColorKeywordImpl(wstr);
}

void	GameUI::RemoveColorKeyword( i3::rc_wstring& wstr)
{
	i3::stack_wstring wstr_stack = wstr;

	if ( RemoveColorKeywordImpl(wstr_stack) )
		wstr = wstr_stack;
}


void	GameUI::RemoveColorKeywordCopy(const i3::string& in_str, i3::string& out_str)
{
	out_str = in_str;
	GameUI::RemoveColorKeyword(out_str);
}


/***********************************/
namespace ctrl
{
	bool g_init_mgr = false;
	i3InputMouse* g_mouse_mgr =0;
	i3InputKeyboard* g_keyboard_mgr = 0;
	bool g_mouse_pressed = false;
	void init_mgr()
	{
		i3InputDeviceManager * pInputManager = g_pFramework->GetViewer()->GetInputDeviceManager();
		ctrl::g_keyboard_mgr = pInputManager->GetKeyboard();
		ctrl::g_mouse_mgr = pInputManager->GetMouse();
		g_init_mgr = true;
	}
}

bool GameUI::IsKeyboardPressed(I3I_KEY key)
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	return ctrl::g_keyboard_mgr->GetPressState( key ) ? true : false;
}

bool GameUI::IsKeyboardStroked(I3I_KEY key)
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	return ctrl::g_keyboard_mgr->GetStrokeState( key ) ? true : false;
}

I3I_KEY GameUI::GetKeyboardPressed()
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	for( int i=0; i<I3I_KEY_MAX; i++)
	{
		if( ctrl::g_keyboard_mgr->GetPressState( (I3I_KEY)i ) )
			return (I3I_KEY)i;
	}
	return I3I_KEY_UNKNOWN;
}

I3I_KEY GameUI::GetKeyboardStroke()
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	for( int i=0; i<I3I_KEY_MAX; i++)
	{
		if( ctrl::g_keyboard_mgr->GetStrokeState( (I3I_KEY)i ) )
			return (I3I_KEY)i;
	}
	return I3I_KEY_UNKNOWN;
}

bool GameUI::IsMousePressed(UINT32 mouse)
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();
	return ((ctrl::g_mouse_mgr->GetButtonState() & mouse) == mouse);
}

bool GameUI::IsMouseStroked(UINT32 mouse)
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();
	return ((ctrl::g_mouse_mgr->GetStrokeButtonState() & mouse) == mouse);
}

void GameUI::ResetMousePressedUp()
{
	ctrl::g_mouse_pressed = false;
}

bool GameUI::IsMousePressedUp(UINT32 mouse)
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	if( !ctrl::g_mouse_pressed && ctrl::g_mouse_mgr->GetButtonState() == mouse )
	{
		ctrl::g_mouse_pressed = true;
	}
	else if(ctrl::g_mouse_pressed && ctrl::g_mouse_mgr->GetButtonState() == 0)
	{
		ctrl::g_mouse_pressed = false;
		return true;
	}

	return false;
}

int GameUI::GetMouseStroke()
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	UINT32 state = ctrl::g_mouse_mgr->GetStrokeButtonState();

	if( (state & I3I_MOUSE_LBUTTON) == I3I_MOUSE_LBUTTON )	return I3I_MOUSE_LBUTTON;
	if( (state & I3I_MOUSE_RBUTTON) == I3I_MOUSE_RBUTTON )	return I3I_MOUSE_RBUTTON;
	if( (state & I3I_MOUSE_MBUTTON) == I3I_MOUSE_MBUTTON )	return I3I_MOUSE_MBUTTON;
	if( (state & I3I_MOUSE_WHEELUP) == I3I_MOUSE_WHEELUP )	return I3I_MOUSE_WHEELUP;
	if( (state & I3I_MOUSE_WHEELDOWN) == I3I_MOUSE_WHEELDOWN )	return I3I_MOUSE_WHEELDOWN;

	return UINT_MAX;
}

POINT GameUI::GetScreenMousePosition()
{
	if(!ctrl::g_init_mgr) ctrl::init_mgr();

	POINT pt;
	pt.x = (LONG)ctrl::g_mouse_mgr->GetX();
	pt.y = (LONG)ctrl::g_mouse_mgr->GetY();
	return pt;
}

POINT GameUI::GetUIMousePosition()
{
	return i3UI::getManager()->getCurrentMousePosition();
}


void GameUI::SetMousePosition(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	::ClientToScreen( g_pViewer->GetWindowHandle(), &pt);

	::SetCursorPos(pt.x, pt.y);
}
void GameUI::SetMouseCenterPosition()
{
	SetMousePosition((int)(g_pViewer->GetViewWidth()* 0.5f), 
		(int)(g_pViewer->GetViewHeight()* 0.5f));
}

/***********************************/
static i3::vector<UILoadingIcon*> s_UILoadingIconList;

UILoadingIcon *	GameUI::EnableLoadingIcon( UIBase * pBase, i3UIControl * pCtrl, bool bVisibleChild)
{
	// List에서 안쓰는 것이 있는지 찾아본다.
	UILoadingIcon * pNewIcon = nullptr;

	for(size_t i = 0; i < s_UILoadingIconList.size(); i++)
	{
		UILoadingIcon * pIcon = (UILoadingIcon*) s_UILoadingIconList[i];
		I3ASSERT( pIcon != nullptr);
		if( pIcon->IsUsed() == false)
		{
			pNewIcon = pIcon;
			break;
		}
	}

	if( pNewIcon == nullptr)
	{
		pNewIcon = UILoadingIcon::new_object();
		s_UILoadingIconList.push_back( pNewIcon);
	}
	
	I3ASSERT( pNewIcon != nullptr);
	pNewIcon->EnableLoadingIcon( pCtrl, bVisibleChild);

	pBase->AddChild( pNewIcon);

	return pNewIcon;
}

void GameUI::DisableLoadingIcon( UILoadingIcon * pIcon)
{
	I3ASSERT( pIcon != nullptr);

	pIcon->DisableLoadingIcon();
	i3GameNode * pParent = pIcon->getParent();
	if( pParent != nullptr)
		pParent->RemoveChild( pIcon);
}

void GameUI::ReleaseLoadingIcon( void)
{
	for(size_t i = 0; i < s_UILoadingIconList.size(); i++)
	{
		UILoadingIcon * pIcon = (UILoadingIcon*) s_UILoadingIconList[i];
		i3GameNode * pParent = pIcon->getParent();
		if( pParent != nullptr)
			pParent->RemoveChild( pIcon);
		I3_SAFE_RELEASE( pIcon);
	}

	s_UILoadingIconList.clear();
}


UINT32 GameUI::GetMaxExp(UINT32 rank)
{
	//만약, 계급이 RANK_NEWBIE, RANK_GAMEMASTER가 될 경우 문제가 된다. 리턴 0을 할 경우,
	//RANK_00 계급도 max exp가 0으로 같은 값으로 인식될 가능성이 높다.
	//RANK_NEWBIE, RANK_GAMEMASTER은 0을 리턴한다.

	if (rank >= RANK_00 && rank <= RANK_51)
	{
		if( g_pEnvSet->IsV2Version())
		return g_ExpValueV2[rank];
		else
			return g_ExpValueV1[rank];
	}

	return 0;
}

UINT32 GameUI::GetExpRate(UINT32 rank, INT32 curExp)
{
	INT32 nMaxExp = 0;
	if (g_pEnvSet->IsV2Version())
		nMaxExp = g_ExpValueV2[rank + 1]- g_ExpValueV2[rank];
	else
		nMaxExp = g_ExpValueV1[rank + 1] - g_ExpValueV1[rank];

	if (nMaxExp == 0)
		nMaxExp = 1;
	INT32 nExpRate = (INT32)((curExp / (REAL32)nMaxExp) * 100);
	
	return MINMAX(0, nExpRate, 100);
}

/*********************************************/
//AutoPosition

namespace detail
{
	enum { precision_factor = 1000, };
	INT32 g_default_scr_rate = (INT32)( (4.f/3.f) * precision_factor );
	INT32 g_scn_width = 0, g_scn_height = 0;
	INT32 g_max_left = 0, g_max_right = 0, g_max_top = 0, g_max_bottom = 0;

	INT32 get_left(INT32 p, INT32 s)	
	{		
		return detail::g_max_left + p;	
	}
	INT32 get_right(INT32 p, INT32 s)	
	{		
		INT32 offset = i3UI::getManager()->getWidth() - (p + s);
		return detail::g_max_right - (offset + s);
	}
	INT32 get_top(INT32 p, INT32 s)	
	{		
		return detail::g_max_top + p;	
	}
	INT32 get_bottom(INT32 p, INT32 s)	
	{		
		INT32 offset = i3UI::getManager()->getHeight() - (p + s);
		return detail::g_max_bottom - (offset + s);
	}
}

INT32 GameUI::AutoPosition::get_max_left()
{
	GameUI::AutoPosition::UpdateRevive();
	return detail::g_max_left;
}
INT32 GameUI::AutoPosition::get_max_right()
{
	GameUI::AutoPosition::UpdateRevive();
	return detail::g_max_right;
}
INT32 GameUI::AutoPosition::get_max_top()
{
	GameUI::AutoPosition::UpdateRevive();
	return detail::g_max_top;
}
INT32 GameUI::AutoPosition::get_max_bottom()
{
	GameUI::AutoPosition::UpdateRevive();
	return detail::g_max_bottom;
}

void GameUI::AutoPosition::set_dir(AutoPosDir dir1, AutoPosDir dir2)
{ 
	aps = (UINT32)dir1;
	aps += (dir2 == APD_NONE) ? 0 : (UINT32)dir2;
}

VEC3D GameUI::AutoPosition::get_pos()
{
	UpdateRevive();

	VEC3D ret;
	ret.x = (REAL32)x;	ret.y = (REAL32)y;	ret.z = 0.f;
	if( g_pFramework->IsBattleStage() )	return ret;

	if( (aps & APD_LEFT) == APD_LEFT )
		ret.x = (REAL32)detail::get_left(x,w);

	if( (aps & APD_RIGHT) == APD_RIGHT )
		ret.x = (REAL32)detail::get_right(x,w);

	if( (aps & APD_TOP) == APD_TOP )
		ret.y = (REAL32)detail::get_top(y,h);

	if( (aps & APD_BOTTOM) == APD_BOTTOM )
		ret.y = (REAL32)detail::get_bottom(y,h);

	return ret;
}

void GameUI::AutoPosition::UpdateRevive()
{
	//INT32 a_w = i3UI::getManager()->getActualWidth();
	//INT32 a_h = i3UI::getManager()->getActualHeight();

	//if( detail::g_scn_width != a_w || detail::g_scn_height != a_h )
	{
		//detail::g_scn_width = a_w;	detail::g_scn_height = a_h;

		INT32 w = g_pViewer->GetViewWidth();
		INT32 h = g_pViewer->GetViewHeight();
		INT32 rate = (INT32)( (w / (REAL32)h) * detail::precision_factor);

		if( detail::g_default_scr_rate < rate )
		{
			INT32 offset = (INT32)(i3UI::getManager()->getDisplayOffsetX() * i3UI::getManager()->getScreenStretchRatioX() );

			detail::g_max_left = -offset;
			detail::g_max_right = i3UI::getManager()->getWidth() + offset;

			detail::g_max_top = 0;
			detail::g_max_bottom = i3UI::getManager()->getHeight();
		}
		else if( detail::g_default_scr_rate > rate )
		{
			detail::g_max_left = 0;
			detail::g_max_right = i3UI::getManager()->getWidth();

			INT32 offset = (INT32)(i3UI::getManager()->getDisplayOffsetY() * i3UI::getManager()->getScreenStretchRatioY() );

			detail::g_max_top = -offset;
			detail::g_max_bottom = i3UI::getManager()->getHeight() + offset;
		}
		else
		{
			detail::g_max_left = 0;
			detail::g_max_right = i3UI::getManager()->getWidth();
			detail::g_max_top = 0;
			detail::g_max_bottom = i3UI::getManager()->getHeight();
		}
	}
}

VEC3D GameUI::AutoPosition::GetUpdatedPos(INT32 x, INT32 y, INT32 w, INT32 h, AutoPosDir dir1, AutoPosDir dir2)
{
	GameUI::AutoPosition::UpdateRevive();

	VEC3D ret;
	ret.x = (REAL32)x;	ret.y = (REAL32)y;	ret.z = 0.f;

	UINT32 aps = (UINT32)dir1;
	aps += (dir2 == APD_NONE) ? 0 : (UINT32)dir2;

	if( (aps & APD_LEFT) == APD_LEFT )
		ret.x = (REAL32)detail::get_left(x,w);

	if( (aps & APD_RIGHT) == APD_RIGHT )
		ret.x = (REAL32)detail::get_right(x,w);

	if( (aps & APD_TOP) == APD_TOP )
		ret.y = (REAL32)detail::get_top(y,h);

	if( (aps & APD_BOTTOM) == APD_BOTTOM )
		ret.y = (REAL32)detail::get_bottom(y,h);

	return ret;
}



/*********************************************/
//AutoOffset
INT32 GameUI::AutoOffset::get_auto_offset_position(AutoOffsetDir dir, int start, UINT32 offset, UINT32 idx, UINT32 max_idx)
{
	INT32 ret = 0;

	switch( dir )
	{
	case AOD_DECREASE:	ret = start - (offset * idx);		break;
	case AOD_INCREASE:	ret = start + (offset * idx);		break;
	case AOD_MIDDLE:
		{
			UINT32 mod = max_idx % 2;
			if( mod == 0 )
			{
				INT32 quotient = max_idx / 2;
				INT32 tmp_idx = idx + 1;
				if( tmp_idx <= quotient ) return get_auto_offset_position(AOD_DECREASE, start - (offset>>1), offset, quotient - tmp_idx);
				return get_auto_offset_position(AOD_INCREASE, start + (offset>>1), offset, idx - quotient);
			}
			else
			{
				UINT32 mid_spot  = max_idx / 2;
				if( idx == mid_spot ) return start;
				if( idx > mid_spot ) return get_auto_offset_position(AOD_INCREASE, start, offset, idx - mid_spot);
				return get_auto_offset_position(AOD_DECREASE, start, offset, mid_spot - idx);
			}
		}
		break;
	}

	return ret;
}

VEC2D GameUI::AutoOffset::get_horizon_position(UINT32 idx, UINT32 max_idx)
{
	VEC2D ret;
	ret.x = (REAL32)x;	ret.y = (REAL32)y;

	ret.x = (REAL32)get_auto_offset_position(dir, x, offset_x, idx, max_idx);

	return ret;
}
VEC2D GameUI::AutoOffset::get_vertical_position(UINT32 idx, UINT32 max_idx)
{
	VEC2D ret;
	ret.x = (REAL32)x;	ret.y = (REAL32)y;

	ret.y = (REAL32)get_auto_offset_position(dir, y, offset_y, idx, max_idx);

	return ret;
}

#include "room_detail.h"
namespace inner
{
	enum { DOMI_SLOT_COUNT = 4};
	enum { BonusPoint = 5, BonusPointUserCnt = 4, };
	enum GradeType { GT_None = -1, GT_S, GT_A, GT_B, GT_C, GT_D, GT_E, GT_MAX, };

	void get_domi_grade_string(i3::wstring & wstr_ret_grade, GradeType gt) //gt에 대한 문자열을 구함
	{
		const wchar_t* grage = 0; 
		i3::rc_wstring wstr_rank = GAME_RCSTRING("STBL_IDX_CLAN_HEADER_RANK");

		if(gt == inner::GT_S) grage = L"S ";
		else if(gt == inner::GT_A) grage = L"A ";
		else if(gt == inner::GT_B) grage = L"B ";
		else if(gt == inner::GT_C) grage = L"C ";
		else if(gt == inner::GT_D) grage = L"D ";
		else if(gt == inner::GT_E) grage = L"E ";

		if (grage)
		wstr_ret_grade.append(grage);
		wstr_ret_grade.append(wstr_rank.begin(), wstr_rank.end());
	}

	INT32 get_grade_point(INT32 rank)
	{
		if( rank == RANK_00 ) return 5; //훈련병
		if( rank <= RANK_12 ) return 3; //이병 1호봉 ~ 병장 3호봉
		if( rank <= RANK_21 ) return 2; //하사 1호봉 ~ 상사 3호봉

		return 0;
	}

	inner::GradeType get_grade_type(INT32 point)
	{
		if(point >= 19) return inner::GT_S; //19 ~ 25
		if(point >= 14) return inner::GT_A; //14 ~ 18
		if(point >= 11) return inner::GT_B; //11 ~ 13
		if(point >= 8) return inner::GT_C; //8 ~ 10
		if(point >= 1) return inner::GT_D; //1 ~ 7
		return inner::GT_E;
	}

	GradeType get_domi_grade_room(i3::wstring & wstr_ret_grade) //현재 방에 대한 등급을 구한다.
	{
		INT32 point = 0; INT32 user_cnt = 0;

		for( INT32 i = 0; i < inner::DOMI_SLOT_COUNT; i++)
		{
			const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( i*2 );
			if( info == nullptr ) continue;

			if( info->_State != SLOT_STATE_CLOSE && info->_State != SLOT_STATE_EMPTY && 
				GameUI::is_over_hero_rank(info->_Rank) == false )
			{
				point += inner::get_grade_point( info->_Rank );
				++user_cnt;
			}
		}

		if( user_cnt == BonusPointUserCnt )
			point += BonusPoint;

		GradeType gt = inner::get_grade_type(point);
		inner::get_domi_grade_string(wstr_ret_grade, gt);
		return gt;
	}
	GradeType get_domi_grade_room_inbattle(i3::wstring & wstr_ret_grade) //현재 방에 대한 등급을 구한다.
	{
		INT32 point = 0; INT32 user_cnt = 0;

		for( INT32 i = 0; i < inner::DOMI_SLOT_COUNT; i++)
		{
			const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( i*2 );
			if( info == nullptr ) continue;

			if( info->_State != SLOT_STATE_CLOSE && info->_State != SLOT_STATE_EMPTY && 
				info->_State == SLOT_STATE_BATTLE &&
				GameUI::is_over_hero_rank(info->_Rank) == false )
			{
				point += inner::get_grade_point( info->_Rank );
				++user_cnt;
			}
		}

		if( user_cnt == BonusPointUserCnt )
			point += BonusPoint;

		GradeType gt = inner::get_grade_type(point);
		inner::get_domi_grade_string(wstr_ret_grade, gt);
		return gt;
	}

	GradeType get_domi_grade_battle(i3::wstring & wstr_ret_grade) //현재 배틀에 대한 등급을 구한다.
	{
		INT32 point = 0; INT32 user_cnt = 0;

		for(INT32 i = 0; i<inner::DOMI_SLOT_COUNT; ++i)
		{
			CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i*2);
			if( pChara == nullptr ) continue;

			UINT8 rank = BattleSlotContext::i()->getSlotInfo(i*2)->_Rank;
			if( UserContext::i()->IsPlayingSlot(i*2) != false && GameUI::is_over_hero_rank(rank) == false )
			{
				point += inner::get_grade_point( rank ); //계급에 따른 포인터를 합산한다.
				++user_cnt;
			}
		}

		if( user_cnt == BonusPointUserCnt )
			point += BonusPoint;

		GradeType gt = inner::get_grade_type(point);
		inner::get_domi_grade_string(wstr_ret_grade, gt);
		return gt;
	}

	void set_color_domi_grade_room(i3::wstring & ret_str, GradeType gt)
	{
		if(gt == inner::GT_S) ::ColorString(ret_str, 128,0,255); //보라
		else if(gt == inner::GT_A) ::ColorString(ret_str, 255,0,0); //빨강
		else if(gt == inner::GT_B) ::ColorString(ret_str, 255,128,64); //주황
		else if(gt == inner::GT_C) ::ColorString(ret_str, 255,255,0); //노랑
		else if(gt == inner::GT_D) ::ColorString(ret_str, 128,255,0); //연두
		else if(gt == inner::GT_E) ::ColorString(ret_str, 0,128,0); //녹색
	}

	void set_color_domi_grade_battle(i3::wstring & ret_str, GradeType gt)
	{
		::ColorString(ret_str, 255,255,0); //노랑
	}
} //end of namespace inner

//---------------------////---------------------//
//domi_util
void domi_util::to_string_reward_grade_room(i3::wstring & ret, INT16& curr_grade)
{
	ret.clear();

	i3::wstring grade; 
	inner::GradeType gt =inner::get_domi_grade_room(grade);
	if( gt == inner::GT_None ) return;
	else if(gt == (inner::GradeType) curr_grade) return;

	i3::sprintf(ret, GAME_RCSTRING("STR2_DOMI_REWARD_GRADE"), grade ); /*현재 방의 보상 아이템 등급은 %s 입니다.*/

	inner::set_color_domi_grade_room(ret, gt);
	ret.append(L"\n");

	i3::wstring tip = GAME_RCSTRING("STR2_DOMI_REWARD_GRADE_TIP"); /*(계급이 소위 이하인 유저와 플레이하게 되면 등급이 상승됩니다.)*/
	::ColorString(tip, 255,255,255);
	ret.append(tip);

	ret.insert(0, L"\n");		// 이건 조금 문제가 있다. (2014.07.17.수빈)

	curr_grade = static_cast<INT16>(gt);
}

void domi_util::to_string_reward_grade_room_inbattle(i3::wstring & ret, INT16& curr_grade)
{
	ret.clear();

	i3::wstring grade; 
	inner::GradeType gt =inner::get_domi_grade_room_inbattle(grade);
	if( gt == inner::GT_None ) return;
	else if(gt == (inner::GradeType) curr_grade) return;

	i3::sprintf(ret, GAME_RCSTRING("STR2_DOMI_REWARD_GRADE"), grade.c_str() ); /*현재 방의 보상 아이템 등급은 %s 입니다.*/

	inner::set_color_domi_grade_room(ret, gt);
	ret.append(L"\n");

	i3::wstring tip = GAME_RCSTRING("STR2_DOMI_REWARD_GRADE_TIP"); /*(계급이 소위 이하인 유저와 플레이하게 되면 등급이 상승됩니다.)*/
	::ColorString(tip, 255,255,255);
	ret.append(tip);

	ret.insert(0, L"\n");	// 이건 조금 문제가 있다. (2014.07.17.수빈)
	
	curr_grade = static_cast<INT16>(gt);
}

void domi_util::to_string_reward_grade_start_battle(i3::wstring& ret)
{
	ret.clear();

	i3::wstring grade; 
	inner::GradeType gt = inner::get_domi_grade_battle(grade);
	if( gt == inner::GT_None ) return;

	i3::sprintf(ret, GAME_RCSTRING("STR2_DOMI_REWARD_GRADE_START_BATTLE"), grade ); /*현재 라운드 클리어 보상 등급은 %s 입니다.*/

	inner::set_color_domi_grade_battle(ret, gt);
}

void domi_util::to_string_reward_grade_in_user(i3::wstring& ret, const i3::rc_wstring& user_name)
{
	ret.clear();

	i3::wstring user = user_name;
	::ColorString(user, 255,255,255);

	i3::wstring grade; 
	inner::GradeType gt = inner::get_domi_grade_battle(grade);
	if( gt == inner::GT_None ) return;

	i3::wstring wstr_msg;
	i3::sprintf(wstr_msg, GAME_RCSTRING("STR2_DOMI_REWARD_GRADE_IN_BATTLE"), grade ); /*님이 입장하여 보상등급이 %s 으로 조정되었습니다.*/

	inner::set_color_domi_grade_battle(wstr_msg, gt);

	ret.append(user);
	ret.append(wstr_msg);
}

void domi_util::to_string_reward_grade_out_user(i3::wstring & ret, const i3::rc_wstring & user_name)
{
	ret.clear();

	i3::wstring user = user_name;
	::ColorString(user, 255,255,255);

	i3::wstring grade; 
	inner::GradeType gt = inner::get_domi_grade_battle(grade);
	if( gt == inner::GT_None ) return;

	i3::wstring wstr_msg;
	i3::sprintf(wstr_msg, GAME_RCSTRING("STR2_DOMI_REWARD_GRADE_OUT_BATTLE"), grade ); /*님이 퇴장하여 보상등급이 %s 으로 조정되었습니다.*/
	inner::set_color_domi_grade_battle(wstr_msg, gt);

	ret.append(user);
	ret.append(wstr_msg);
}

const wchar_t* g_grades[inner::GT_MAX] = { L"S", L"A", L"B", L"C", L"D", L"E"};

INT32 domi_util::reward_item_image_Idx()
{
	INT32 imgidx[inner::GT_MAX] = {5, 4, 3, 0, 1, 2};

	i3::wstring grade; 
	inner::GradeType gt =inner::get_domi_grade_battle(grade);
	if (inner::GT_None == gt) gt = inner::GT_E;

	return imgidx[gt];
}

const wchar_t* domi_util::reward_item_image_name()
{
	const wchar_t* names[inner::GT_MAX] = {L"master", L"diamond", L"platinum", L"gold", L"silver", L"bronze"};

	i3::wstring grade; 
	inner::GradeType gt =inner::get_domi_grade_battle(grade);
	if (inner::GT_None == gt) gt = inner::GT_E;

	return names[gt];
}

const wchar_t* domi_util::round_grade_result()
{
	i3::wstring grade; 
	inner::GradeType gt =inner::get_domi_grade_battle(grade);

	if (gt == inner::GT_None) return L"E";

	return g_grades[gt];
}

//영웅 이상 계급이면 true (RANK_NEWBIE, RANK_GAMEMASTER)
bool GameUI::is_over_hero_rank(INT32 nRank)
{
	if(nRank == RANK_NEWBIE || nRank == RANK_GAMEMASTER || nRank == RANK_MOD)
		return true;

	return false;
}

 //영웅 계급이면 true, include_over_rank : true 일 경우 그 이상 계급 포함
bool GameUI::is_hero_rank(INT32 nRank, bool include_over_rank)
{
	if(nRank == RANK_51)
		return true;

	if( include_over_rank == true )
		return is_over_hero_rank(nRank);

	return false;
}

//장성급&영웅 계급이면 true, include_over_rank : true 일 경우 그 이상 계급 포함
bool GameUI::is_general_rank(INT32 nRank, bool include_over_rank)
{
	if(nRank >= RANK_46 && nRank <= RANK_51)
		return true;

	if( include_over_rank == true )
		return is_over_hero_rank(nRank);

	return false;
}

void GameUI::CheckWeaponEndurance(void)
{
	enum { DT_TROUBLE = 0, DT_FAULTY, DT_NORMAL, DT_GOOD } durability_type = DT_GOOD;

	CShopContext*	pShopContext	= CShopContext::i();

	UINT32 size = CInvenList::i()->GetItemCount(INVEN::DB_WEAPON);

	for( UINT32 i = 0; i < size; i++ )
	{
		const INVEN_BUFFER * pItem = CInvenList::i()->GetItem( INVEN::DB_WEAPON, i);
		if( pShopContext->IsDurabilityItem( pItem->_ui32ItemID ) == false ) continue;

		REAL32 fRate = 0.f;
		if ( pShopContext->GetDurability( pItem->_ui32ItemID, fRate ) == true )
		{
			if( fRate <= 0.0f )
			{
				durability_type = DT_TROUBLE; //고장
				break;
			}
			else if( fRate <= 0.2f )
			{
				durability_type = DT_FAULTY; //불량상태
			}
			else if( fRate <= 0.5f )
			{
				if( durability_type > DT_NORMAL ) durability_type = DT_NORMAL; //보통상태
			}
		}
	}

	if( durability_type == DT_GOOD ) return;

	i3::wstring strType;
	switch( durability_type )
	{
	case DT_TROUBLE: strType = GAME_RCSTRING("STR_POP_REPAIR_TROUBLE_STATE"); ::ColorString(strType, 255, 0, 0); break;
	case DT_FAULTY: strType = GAME_RCSTRING("STR_POP_REPAIR_FAULTY_STATE"); ::ColorString(strType, 255, 127, 0); break;
	case DT_NORMAL: strType = GAME_RCSTRING("STR_POP_REPAIR_NORMAL_STATE"); ::ColorString(strType, 255, 255, 0); break;
	}

	i3::rc_wstring wstrText;
	i3::sprintf( wstrText, GAME_RCSTRING("STR_POP_REPAIR_REMAIN_ITEM"), strType );

	g_pFramework->getChatBox()->PutSystemChatting( wstrText );
}

void GameUI::Update_RankAndNationMark_Swap(REAL32 rDeltaSecond, UserUI_NationMarkData& UserUI_NationMark)
{
	REAL32	_NationAlpha_Rate = 1.3f;
	REAL32	_SwapTimer = UserUI_NationMark.rTimer;
	REAL32	_NationMarkAlpha = UserUI_NationMark.rAlphaValue;
	REAL32	_ElapsedTimeByEffect = UserUI_NationMark.rElapsedTimeByEffect;
	UINT8	_EffectIdx = UserUI_NationMark.ui8EffectIdx;

	_SwapTimer += rDeltaSecond;
	
	if (_SwapTimer > 3.0f)
	{
		UserUI_NationMark.isVisible = !UserUI_NationMark.isVisible;
		
		_ElapsedTimeByEffect = 0.0f;
		_SwapTimer = 0.0f;
	}
	
	//1.5초 ~ 2.5초 사이 1초 간처리
	if (1.5f < _SwapTimer && _SwapTimer < 2.5f)
	{
		_ElapsedTimeByEffect += rDeltaSecond;
		_EffectIdx = static_cast<UINT8>((_ElapsedTimeByEffect * 8.0f) + 1);	// 0.125로 나눈 값, EffectIdx Min 1 ~ Max 4

		if (_EffectIdx == MAX_EFFECT_IDX) _EffectIdx = MAX_EFFECT_IDX;
	}

	if (UserUI_NationMark.isVisible == true)
	{
		_NationMarkAlpha += _NationAlpha_Rate * rDeltaSecond;
		_NationMarkAlpha = i3Math::Min(1.0f, _NationMarkAlpha);
	}
	else
	{
		_NationMarkAlpha -= _NationAlpha_Rate * rDeltaSecond;
		_NationMarkAlpha = i3Math::Max(0.0f, _NationMarkAlpha);
	}

	UserUI_NationMark.rTimer = _SwapTimer;
	UserUI_NationMark.rAlphaValue = _NationMarkAlpha;
	UserUI_NationMark.rElapsedTimeByEffect = _ElapsedTimeByEffect;
	UserUI_NationMark.ui8EffectIdx = _EffectIdx;
}

void GameUI::Update_RankMark_AlphaValue(i3UIListView_Box* LV_BOX, INT32 CellNumber, REAL32 RankMarkAlpha)
{
	if (LV_BOX == nullptr)
		return;

	INT32 VisibleItemCount = LV_BOX->getItemCount();			// 보이는 아이템 갯수

	for (int i = 0; i < VisibleItemCount; ++i)
	{
		i3UIListView_Item* pItem = LV_BOX->getItemList()[i];
		if (pItem != nullptr)
		{
			i3UIListView_Cell* pLV_RankCell = pItem->getCell(CellNumber);
			if (pLV_RankCell != nullptr)
			{
				if (i3::same_of<i3UIButtonImageSet*>(pLV_RankCell->getCellChild()))
				{
					COLOR color;
					i3Color::Set(&color, (REAL32)1, (REAL32)1, (REAL32)1, RankMarkAlpha);
					i3UIButtonImageSet * pBtnImgSet = (i3UIButtonImageSet*)pLV_RankCell->getCellChild();
					if (pBtnImgSet != nullptr)
					{
						pBtnImgSet->setColor(&color);
					}
				}
			}
		}
	}
}