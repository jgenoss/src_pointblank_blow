#include "pch.h"
#include "GameEventHandler.h"
#include "LogRespawn.h"
#include "Shop.h"
#include "Network/GameNetworkManager.h"
#include "InGameQACommandSet.h"

#include "../../../Common/CommonSource/CommonDediCli.h"
#include "./UI/UIItemInfoUtil.h"
#include "UI/UIUtil.h"

#include "./StageBattle/ScoreContext.h"
#include "./StageBattle/DinoModeContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"
#include "GameMissionManager.h"
#include "EnvInfoContext.h"

#include "MissionCard/MCardUtil.h"

//======================================================================================//
//									Battle Event										//
//======================================================================================//

const char * CGameEventHandler::__getBattleEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_PRESTART_BATTLE : return "EVENT_PRESTART_BATTLE";
	case EVENT_HOLEPUNCH_OK : return "EVENT_HOLEPUNCH_OK";
	case EVENT_START_BATTLE : return "EVENT_START_BATTLE";
	case EVENT_GIVEUP_BATTLE : return "EVENT_GIVEUP_BATTLE";
	case EVENT_RESPAWN_BATTLE : return "EVENT_RESPAWN_BATTLE";
	case EVENT_DEATH_BATTLE : return "EVENT_DEATH_BATTLE";
	case EVENT_RESPAWN_BATTLE_FOR_AI : return "EVENT_RESPAWN_BATTLE_FOR_AI";
	case EVENT_ARMEDBOMB_BATTLE : return "EVENT_ARMEDBOMB_BATTLE";
	case EVENT_DISARMBOMB_BATTLE : return "EVENT_DISARMBOMB_BATTLE";
	case EVENT_CHANGE_DIFFICULTY_LEVEL : return "EVENT_CHANGE_DIFFICULTY_LEVEL";
	case EVENT_BATTLE_TOUCHDOWN : return "EVENT_BATTLE_TOUCHDOWN";
	case EVENT_BATTLE_DEATHBLOW : return "EVENT_BATTLE_DEATHBLOW";
	case EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE : return "EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE";
	case EVENT_KICK_VOTE_START : return "EVENT_KICK_VOTE_START";
	case EVENT_KICK_VOTE : return "EVENT_KICK_VOTE";
	case EVENT_TUTORIAL_ROUND_END : return "EVENT_TUTORIAL_ROUND_END";
	case EVENT_GETINFO_IGSHOP : return "EVENT_GETINFO_IGSHOP";
	case EVENT_OPEN_IGSHOP : return "EVENT_OPEN_IGSHOP";
	case EVENT_INGAMESHOP_APPLY_ITEM : return "EVENT_INGAMESHOP_APPLY_ITEM";
	case EVENT_BATTLE_GM_PAUSE : return "EVENT_BATTLE_GM_PAUSE";
	case EVENT_BATTLE_GM_RESUME : return "EVENT_BATTLE_GM_RESUME";
	case EVENT_DOMI_SKILL : return "EVENT_DOMI_SKILL";
	case EVENT_DOMI_CHEAT_MESSAGE : return "EVENT_DOMI_CHEAT_MESSAGE";
	case EVENT_DOMI_INTERMISSION_READY : return "EVENT_DOMI_INTERMISSION_READY";
	case EVENT_DOMI_INTERMISSION_READY_CANCEL : return "EVENT_DOMI_INTERMISSION_READY_CANCEL";
	case EVENT_DOMI_USE_ITEM : return "EVENT_DOMI_USE_ITEM";
	case EVENT_STAGE_JUMP_ROUND_START : return "EVENT_STAGE_JUMP_ROUND_START";
	case EVENT_SENDPING_N : return "EVENT_SENDPING_N";
	case EVENT_DAMAGEOBJ_BATTLE_N : return "EVENT_DAMAGEOBJ_BATTLE_N";
	case EVENT_HEART_BIT_N : return "EVENT_HEART_BIT_N";
	case EVENT_BATTLE_REMAINTIME_N : return "EVENT_BATTLE_REMAINTIME_N";
	default :					return nullptr;
	}
}

void CGameEventHandler::__register_Battle( void)
{
	regSendEvent( EVENT_SENDPING_N,						&CGameEventHandler::__evtBattle_SendPing);
	regSendEvent( EVENT_PRESTART_BATTLE,				&CGameEventHandler::__evtBattle_Prestart);
	regSendEvent( EVENT_HOLEPUNCH_OK,					&CGameEventHandler::__evtBattle_HolepuchOK);
	regSendEvent( EVENT_START_BATTLE,					&CGameEventHandler::__evtBattle_Start);
	regSendEvent( EVENT_GIVEUP_BATTLE,					&CGameEventHandler::__evtBattle_Giveup);
	regSendEvent( EVENT_RESPAWN_BATTLE,					&CGameEventHandler::__evtBattle_Respawn);
	regSendEvent( EVENT_DEATH_BATTLE,					&CGameEventHandler::__evtBattle_Death);
	regSendEvent( EVENT_RESPAWN_BATTLE_FOR_AI,			&CGameEventHandler::__evtBattle_RespawnAI);

	regSendEvent( EVENT_BATTLE_REMAINTIME_N,			&CGameEventHandler::__evtBattle_RemainTime);
	regSendEvent( EVENT_ARMEDBOMB_BATTLE,				&CGameEventHandler::__evtBattle_Install);
	regSendEvent( EVENT_DISARMBOMB_BATTLE,				&CGameEventHandler::__evtBattle_Uninstall);
	regSendEvent( EVENT_CHANGE_DIFFICULTY_LEVEL,		&CGameEventHandler::__evtBattle_ChangeAILevel);
	regSendEvent( EVENT_BATTLE_TOUCHDOWN,				&CGameEventHandler::__evtBattle_Touchdown);
	regSendEvent( EVENT_BATTLE_DEATHBLOW,				&CGameEventHandler::__evtBattle_DeathBlow);
	regSendEvent( EVENT_DAMAGE_DEFENCE_OBJECT_BATTLE,	&CGameEventHandler::__evtBattle_AttackDefenceObject);
	regSendEvent( EVENT_KICK_VOTE_START,				&CGameEventHandler::__evtVote_Start);
	regSendEvent( EVENT_KICK_VOTE,						&CGameEventHandler::__evtVote_Vote);
	regSendEvent( EVENT_TUTORIAL_ROUND_END,				&CGameEventHandler::__evtBattle_TutorialRoundEnd);
	regSendEvent( EVENT_DAMAGEOBJ_BATTLE_N,				&CGameEventHandler::__evtBattle_DestructionObject);
	regSendEvent( EVENT_HEART_BIT_N,					&CGameEventHandler::__evtBattle_Heartbit);
	regSendEvent( EVENT_GETINFO_IGSHOP,					&CGameEventHandler::__evtFieldShop_GetGoodsList);
	regSendEvent( EVENT_OPEN_IGSHOP,					&CGameEventHandler::__evtFieldShop_Open);
	regSendEvent( EVENT_INGAMESHOP_APPLY_ITEM,			&CGameEventHandler::__evtFieldShop_ApplyItem);

	regSendEvent( EVENT_BATTLE_GM_PAUSE,				&CGameEventHandler::__evtGMPause_Activate);
	regSendEvent( EVENT_BATTLE_GM_RESUME,				&CGameEventHandler::__evtGMPause_Deactivate);

	regSendEvent( EVENT_DOMI_SKILL,						&CGameEventHandler::__evtDomination_Skill);

	regSendEvent( EVENT_DOMI_CHEAT_MESSAGE,				&CGameEventHandler::__evtDomination_CheatKey);
	regSendEvent( EVENT_DOMI_INTERMISSION_READY,		&CGameEventHandler::__evtDomination_WaveReady);
	regSendEvent( EVENT_DOMI_INTERMISSION_READY_CANCEL, &CGameEventHandler::__evtDomination_WaveReadyCancel);
	regSendEvent( EVENT_DOMI_USE_ITEM,					&CGameEventHandler::__evtDomination_UseItem);

#ifdef DOMI_NEW_FLOW
	regSendEvent( EVENT_STAGE_JUMP_ROUND_START,			&CGameEventHandler::__evtBattle_JumpStageRound_LoadFinished);
#endif
	regSendEvent(EVENT_BATTLE_OPTION_SCOPETYPE,		&CGameEventHandler::__evtBattle_Change_UserScopeType);
}

bool CGameEventHandler::__evtBattle_SendPing( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_SENDPING_REQ );
	pInfo->_SendPacket.WriteData( BattleServerContext::i()->GetPingInfo()->m_Pings, sizeof( BattleServerContext::i()->GetPingInfo()->m_Pings ));

#if 0
	I3TRACE( " # SEND PING");
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		I3TRACE( "  %d", BattleServerContext::i()->GetPingInfo()->m_Pings[ i]);
	}
	I3TRACE( " -->\n");
#endif

	return true;
}

bool CGameEventHandler::__evtBattle_Prestart( GAMEEVENTINFO * pInfo)
{
	BattleServerContext::i()->P2PIntrudeBattle();				// P2P 서버 설정
	
	UINT32 StageID = MyRoomInfoContext::i()->GetMyRoomStageID();
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_PRESTARTBATTLE_REQ  );
	pInfo->_SendPacket.WriteData( &StageID, sizeof(UINT32) );

	I3TRACE( " # SEND BATTLE PRESTART -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_HolepuchOK( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_HOLE_CHECK_REQ );
	I3TRACE( " # SEND HOLEPUNCH OK -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_Start( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_STARTBATTLE_REQ );

	// 자신의 인벤에 있는 무기를 로드하자.
	g_pWeaponManager->PreloadMyWeapons();

	// 인벤토리 UI 단과 인게임에서 사용하는 COMMON_PARTS_INFO 분리. 배틀 중 리스폰 시 액티브 파츠 정보를 사용한다. hansoft 11523.
	COMMON_PARTS_INFO* pCommonInfo = CCharaInfoContext::i()->SetCommonPartsInfo();
	COMMON_PARTS_INFO* pActiveCommonInfo = CCharaInfoContext::i()->GetActiveCommonPartsInfo();
	pCommonInfo->CopyTo(pActiveCommonInfo);

	I3TRACE( " # PROTOCOL_BATTLE_STARTBATTLE_REQ -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_Giveup( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_GIVEUPBATTLE_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(T_ItemDBIdx));
	
	// 배틀 중 퀘스트 완료된것을 모두 취소하고 이전 복귀한다..
	MCardUtil::Update_Process_GiveupBattle();

	I3TRACE( " # SEND GIVEUP -->\n");
	return true;
}

#include	"ui/UIBattlePopupSelectWeapon.h"
bool CGameEventHandler::__evtBattle_Respawn( GAMEEVENTINFO * pInfo)
{
	//당신이 보기에 아래 무기 변경 코드가 좋지 못 한 코드라고 생각된다면 변경하시오.. 작성자도 좋지 못 한 코드라고 생각됨.. 
	UIBattlePopupSelectWeapon * pPopup = static_cast<UIBattlePopupSelectWeapon*>(GameUI::GetBattlePopup( UBP_SELECT_WEAPON));
	if( pPopup != nullptr)
		pPopup->WeaponChange(); //리스폰 패킷 날리기전에 무기 교체를 한다.
	

	// revision 31898
	BattleServerContext::i()->setRespawnAckTime( 10.f);
	BattleServerContext::i()->setRespawnReq(true);

	PACKET_BATTLE_NEW_RESPAWN_REQ WritePacket;

	// 리스폰될시 무기정보를 서버에 알려줍니다.
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_RESPAWN_REQ );
	I3TRACE("# PROTOCOL_BATTLE_NEW_RESPAWN_REQ --> \n");

	// REFACTOR_TARGET_SHOTGUN
	const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());

	TEAM_TYPE team = BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED ? TEAM_RED : TEAM_BLUE;
	const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(team);
	//const COMMON_PARTS_INFO * pCommonInfo = CCharaInfoContext::i()->GetCommonPartsInfo();

	// ITEM_INFO 구조체를 바로 넣으려고 하였으나 이미 포인터로 받아 사용하는 부분이 있으며 Common 소스이기 때문에 일단 이렇게 하드코딩 처리

	// vv3 - check
	for( INT32 idx = 0; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx)
	{
		CHAR_EQUIPMENT_PARTS parts = static_cast<CHAR_EQUIPMENT_PARTS>(idx);

		if( parts != CHAR_EQUIPMENT_PARTS_CHARA && pPartsInfo->GetPartsIdx( parts ) > DEFAULT_ITEM_DBIDX_MIN )
		{
			T_ItemID chara_item_id = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
			WritePacket.TBattleEquipParts[ idx ] = item_def::get_default_item(chara_item_id, EQUIP::CommonParts2eParts(parts) );
		}
		else
		{
			WritePacket.TBattleEquipParts[ idx ] = *pPartsInfo->GetPartsInfo( parts );
		}
	}

	// GetModePrimaryDefaultWeapon(). 이 함수 기능 작동이 비정상임.
	//WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_PRIMARY ]	= g_pFramework->GetModePrimaryDefaultWeapon( pNetInfo->getWeaponItemID( WEAPON_SLOT_PRIMARY));

	// 현재 맵모드에따라 사용할 무기를 결정합니다.
	T_ItemID desiredModePrimaryWeapon = g_pFramework->FindWeaponIDForGameMode( pNetInfo->getWeaponItemID( WEAPON_SLOT_PRIMARY ));
	T_ItemID desiredModeMeleeWeapon = g_pFramework->FindWeaponIDForGameMode( pNetInfo->getWeaponItemID( WEAPON_SLOT_MELEE ));

	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_PRIMARY ].m_TItemID		= desiredModePrimaryWeapon;
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_SECONDARY ].m_TItemID	= pNetInfo->getWeaponItemID( WEAPON_SLOT_SECONDARY );
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_MELEE ].m_TItemID		= desiredModeMeleeWeapon;
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING1 ].m_TItemID	= pNetInfo->getWeaponItemID( WEAPON_SLOT_THROWING1 );
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING2 ].m_TItemID	= pNetInfo->getWeaponItemID( WEAPON_SLOT_THROWING2 );

	T_ItemDBIdx desiredModePrimaryWeaponDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID_ForBattle(desiredModePrimaryWeapon);
	T_ItemDBIdx desiredModeMeleeWeaponDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID_ForBattle(desiredModeMeleeWeapon);

	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_PRIMARY ].m_TItemDBIdx		= desiredModePrimaryWeaponDBIdx; // pNetInfo->getWeaponItemDBIdx( WEAPON_SLOT_PRIMARY );
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_SECONDARY ].m_TItemDBIdx	= pNetInfo->getWeaponItemDBIdx( WEAPON_SLOT_SECONDARY );
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_MELEE ].m_TItemDBIdx		= desiredModeMeleeWeaponDBIdx; // pNetInfo->getWeaponItemDBIdx( WEAPON_SLOT_MELEE );

	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING1 ].m_TItemDBIdx	= pNetInfo->getWeaponItemDBIdx( WEAPON_SLOT_THROWING1 );
	WritePacket.TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING2 ].m_TItemDBIdx	= pNetInfo->getWeaponItemDBIdx( WEAPON_SLOT_THROWING2 );
	//WritePacket.TBattleEquipParts[ BATTLE_EQUIPMENT_PARTS_DINO ]		= pCommonInfo->m_Dino;

	// vv3 - check
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == true || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) == true )
	{
		if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED)
		{
			i3mem::Copy(&WritePacket.TBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA], CCharaInfoContext::i()->GetActiveCommonPartsInfo()->getParts(), sizeof(ITEM_INFO));
		}
	}
	
	WritePacket.m_ui8WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();

	pInfo->_SendPacket.WriteData(&WritePacket, sizeof(PACKET_BATTLE_NEW_RESPAWN_REQ));


	log_respawn::i()->Print();

	I3TRACE( " # SEND RESPAWN -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_RespawnAI( GAMEEVENTINFO * pInfo)
{
	INT32 slotIdx = *(INT32*) pInfo->_pArg;

	if ( -1 < slotIdx && slotIdx < SLOT_MAX_COUNT)
	{
		//캐릭터가 로딩중인경우 리스폰 안될수도 있다..(패킷 처리를 따로 해줘야 한다)

		//CGameCharaBase* pCharaBase = g_pCharaManager->getCharaByNetIdx( slotIdx);
		//if( pCharaBase != nullptr)	//"생성되지 않은 AI 캐릭터에 대한 리스폰 요청"
		{
			pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ);
			pInfo->_SendPacket.WriteData(&slotIdx, sizeof(INT32));

			I3TRACE( " # SEND RESPAWN AI %d -->\n", slotIdx);
			return true;
		}
	}
	
	return false;
}

bool CGameEventHandler::__evtBattle_Death( GAMEEVENTINFO * pInfo)
{
	// 클라이언트가 게임서버로 ai의 리스폰을 요청하는 패킷을 보내는 루틴이나 ai의 리스폰은 모두 데디케이션 서버에서 직접 하므로 더 이상 필요없는 이벤트입니다. 안국정
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_DEATH_REQ );
	KILLDATA * pData = (KILLDATA*) pInfo->_pArg;
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(DEATH_INFO_HEADER) + sizeof(DEATH_INFO_CHARA) * pData->head._nCount);
	I3TRACE( " # SEND DEATH -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_RemainTime( GAMEEVENTINFO * pInfo)
{
	UINT8	ui8Ping; 
	UINT16	ui16PingTime;
	UINT8	ui8RoundCount;
	INT32	i32MySlotIdx;

	i32MySlotIdx			= BattleSlotContext::i()->getMySlotIdx();
	const PING* pPingInfo	= BattleServerContext::i()->GetPingInfo();
	ui8RoundCount			= ScoreContext::i()->getCurrentRound();	

	if( 0 > i32MySlotIdx || SLOT_MAX_COUNT <= i32MySlotIdx )
	{
		ui8Ping					= 0;
		ui16PingTime			= 0;
	}
	else
	{
		ui8Ping					= pPingInfo->m_Pings[ i32MySlotIdx ]; 
		ui16PingTime			= pPingInfo->m_PingTimes[ i32MySlotIdx ];
	}

	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_TIMERSYNC_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg,		sizeof( INT32 ) );
	pInfo->_SendPacket.WriteData( pInfo->_pArg1,	sizeof( REAL32) );	
	pInfo->_SendPacket.WriteData( &ui8RoundCount,	sizeof( UINT8 ) );
	pInfo->_SendPacket.WriteData( &ui8Ping,			sizeof( UINT8 ) ); 
	pInfo->_SendPacket.WriteData( pInfo->_pArg2,	sizeof( UINT8) );
    pInfo->_SendPacket.WriteData( &ui16PingTime,	sizeof( UINT16) );

	return true;
}

bool CGameEventHandler::__evtBattle_Install( GAMEEVENTINFO * pInfo)
{
	BOMB_AREA Area;
	INT32 * pIdx = (INT32*) pInfo->_pArg;
	Area = CGameMissionManager::i()->WhoInstallingArea( *pIdx);
	INT32* ui32Weapon = (INT32*) pInfo->_pArg2;

	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ );

	pInfo->_SendPacket.WriteData( pInfo->_pArg,		sizeof( INT32 ) );
	pInfo->_SendPacket.WriteData( &Area,	sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &ui32Weapon,	sizeof(INT32) );

	// 2009.06.22 dfly79 Position 추가
	if( pInfo->_pArg1 != nullptr)
	{
		VEC3D * pPos = (VEC3D*) pInfo->_pArg1;

		REAL32	rPos[ 3];

		rPos[0] = i3Vector::GetX( pPos);
		rPos[1] = i3Vector::GetY( pPos);
		rPos[2] = i3Vector::GetZ( pPos);

		pInfo->_SendPacket.WriteData( rPos, sizeof(REAL32) * 3);
	}

	I3TRACE( " # SEND BATTLE INSTALL BOMB -->\n");

	return true;
}

bool CGameEventHandler::__evtBattle_Uninstall( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg,			sizeof( INT32 ) );

	I3TRACE( " # SEND BATTLE UNINSTALL BOMB -->\n");
	return true;
}

bool CGameEventHandler::__evtBattle_ChangeAILevel( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_REQ);
	return true;
}

bool CGameEventHandler::__evtBattle_Touchdown( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT32 ));	// Slot 번호를 보내주세요.
	return true;
}

bool CGameEventHandler::__evtBattle_DeathBlow( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_DEATHBLOW_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( DEATHBLOW_INFO ));
	return true;
}

bool CGameEventHandler::__evtBattle_AttackDefenceObject( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( DEFENCE_INFO));
	return true;
}

bool CGameEventHandler::__evtVote_Start( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ );
	UINT8 slot = static_cast<UINT8>(*reinterpret_cast<const INT32*>(pInfo->_pArg));
	INT8  reason = static_cast<INT8>(*reinterpret_cast<const FORCEREMOVE_CAUSE*>(pInfo->_pArg1));
	pInfo->_SendPacket.WriteData( &slot, sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &reason, sizeof(INT8));
	return true;
}

bool CGameEventHandler::__evtVote_Vote( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ);
	bool disagree = !(*reinterpret_cast<const bool*>(pInfo->_pArg));
	pInfo->_SendPacket.WriteData(&disagree, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtBattle_TutorialRoundEnd( GAMEEVENTINFO * pInfo)
{
	/*라운드 종료시 REQ 요청은 아래와 같이 보내면 됩니다. */
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_REQ );
	return true;
}

bool CGameEventHandler::__evtBattle_DestructionObject( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( DESTRUCTION_INFO));
	return true;
}

bool CGameEventHandler::__evtBattle_Heartbit( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_HEART_BIT_REQ );
	return true;
}

bool CGameEventHandler::__evtFieldShop_GetGoodsList( GAMEEVENTINFO * pInfo)
{
	// 이때는 야전상점 버전만 전달..
	pInfo->_SendPacket.SetProtocol(PROTOCOL_FIELDSHOP_GOODSLIST_REQ);
	return true;
}

bool CGameEventHandler::__evtFieldShop_Open( GAMEEVENTINFO * pInfo)
{
	const UINT32* version_ingame_shop = reinterpret_cast<const UINT32 *>( pInfo->_pArg);
	UINT8 ui8StageType = UINT8(MyRoomInfoContext::i()->getStage()->GetStageMode());

	char strMD5Key[NET_MD5_KEY_SIZE+1];
	i3::safe_string_copy( strMD5Key, CShop::i()->GetShopMD5Key(), NET_MD5_KEY_SIZE );
	strMD5Key[NET_MD5_KEY_SIZE] = 0;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_FIELDSHOP_OPEN_REQ);
	pInfo->_SendPacket.WriteData( version_ingame_shop,	sizeof(UINT32));
	pInfo->_SendPacket.WriteData( &ui8StageType,		sizeof(UINT8) );
	pInfo->_SendPacket.WriteData(strMD5Key,				sizeof(char) * NET_MD5_KEY_SIZE ); 
	

	return true;
}

bool CGameEventHandler::__evtFieldShop_ApplyItem( GAMEEVENTINFO * pInfo)
{
	const ITEM_INFO* pItemInfo = reinterpret_cast<const ITEM_INFO*>( pInfo->_pArg);
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ);
	pInfo->_SendPacket.WriteData(pItemInfo, sizeof(ITEM_INFO));
	return true;
}

bool CGameEventHandler::__evtGMPause_Activate( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_GM_PAUSE_REQ);
	return true;
}

bool CGameEventHandler::__evtGMPause_Deactivate( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_GM_RESUME_REQ);
	return true;
}

bool CGameEventHandler::__evtDomination_Skill(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_USER_SKILL_UPGRADE_REQ);

	const PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ* send = static_cast<const PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ*>(pInfo->_pArg);

	pInfo->_SendPacket.WriteData( send, sizeof(PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ));

	return true;
}

bool CGameEventHandler::__evtDomination_CheatKey(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_CHEAT_MESSAGE_REQ);

	PACKET_DOMINATION_CHEAT_MESSAGE_REQ* pStruct = (PACKET_DOMINATION_CHEAT_MESSAGE_REQ*)pInfo->_pArg;
	
	if (pStruct)
		pInfo->_SendPacket.WriteData(pStruct, sizeof(PACKET_DOMINATION_CHEAT_MESSAGE_REQ));
	
	return true;
}

bool CGameEventHandler::__evtDomination_WaveReady(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_WAVE_READY_INFO_REQ);

	return true;
}

bool CGameEventHandler::__evtDomination_WaveReadyCancel(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_WAVE_READY_INFO_CANCEL_REQ);

	return true;
}

bool CGameEventHandler::__evtDomination_UseItem(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_USE_ITEM_REQ);

	PACKET_DOMINATION_USE_ITEM_REQ* pStruct = (PACKET_DOMINATION_USE_ITEM_REQ*)pInfo->_pArg;

	if (pStruct)
		pInfo->_SendPacket.WriteData(pStruct, sizeof(PACKET_DOMINATION_USE_ITEM_REQ));

	return true;
}

#ifdef DOMI_NEW_FLOW
bool CGameEventHandler::__evtBattle_JumpStageRound_LoadFinished( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_MISSION_ROUND_START_REQ);
	I3TRACE( "# PROTOCOL_BATTLE_MISSION_ROUND_START_REQ --> \n");

	return true;
}
#endif

bool CGameEventHandler::__evtBattle_Change_UserScopeType(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_USER_SOPETYPE_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(UINT8));

	return true;
}