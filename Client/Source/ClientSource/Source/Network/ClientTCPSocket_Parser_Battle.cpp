#include "pch.h"
#include "ClientTCPSocket.h"
#include "../GlobalVariables.h"
#include "StageBattle.h"
#include "LogRespawn.h"
#include "GameStateMgr.h"
#include "UtilBattle.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"
#include "UI/UIBattleDef.h"

#include "IGShopContext.h"
#include "IGShopItemController.h"


#include "../Mode/Domination/DominationSkillSystem.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "../Mode/Domination/DomiSkillObjBulletinBoard.h"
#include "../Mode/Domination/GameMode_Domination.h"
#include "../Mode/Domination/DomiSkillProperties.h"
#include "../GameDropItemManager.h"

#include "../UI/HUD/UIHUD_Domination_Manager.h"
#include "../ui/hud/DomiRevivalController.h"


#include "../../../Common/CommonSource/CommonDediCli.h"
#include "../BattleHud.h"
#include "BattleHUD_CountDown.h"
#include "../../UI/UIBattleFrame.h"
#include "../../UI/UIBattlePopupSkillShop.h"
#include "../../UI/UIBattlePopupSkillShopCashSlot.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "../Mode/Domination/DomiSkillObjBulletinBoard.h"
#include "../ObserverBattle.h"


#include "GameCharaWeaponContext.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudGameResult.h"
#include "CommunityContext.h"

#include "GameCharaHUDContext.h"
#include "GameCharaMoveContext.h"

#include "ReadyBoxContext.h"
#include "../StageBattle/AIModeContext.h"
#include "../StageBattle/DinoModeContext.h"
#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"
#include "../StageBattle/ScoreContext.h"
#include "BattleServerContext.h"
#include "KickContext.h"
#include "UserInfoContext.h"
#include "LobbyContext.h"
#include "MyRoomInfoContext.h"
#include "Channelcontext.h"
#include "ClanGameContext.h"
#include "BattleSlotContext.h"
#include "DomiContext.h"
#include "BattleObjContext.h"
#include "BattleResultContext.h"
#include "AbuseContext.h"
#include "TutorialContext.h"
#include "GameMissionManager.h"
#include "HackContext.h"
#include "GameMaster.h"
#include "AssistContext.h"

#include "ValidCheck.h"
#include "WeaponLRU_Manager.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "MissionCard/MCardUtil.h"

#include "ClientStageinfoUtil.h"
#include "CrosshairMgr.h"
namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_BATTLE_READYBATTLE_ACK : return "PROTOCOL_BATTLE_READYBATTLE_ACK";
		case PROTOCOL_BATTLE_START_GAME_ACK : return "PROTOCOL_BATTLE_START_GAME_ACK";
		case PROTOCOL_BATTLE_START_GAME_TRANS_ACK : return "PROTOCOL_BATTLE_START_GAME_TRANS_ACK";
		case PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_ACK : return "PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_ACK";
		case PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_OTHER_ACK : return "PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_OTHER_ACK";
		case PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK : return "PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK";
		case PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_OTHER_ACK : return "PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_OTHER_ACK";
		case PROTOCOL_BATTLE_PRESTARTBATTLE_ACK : return "PROTOCOL_BATTLE_PRESTARTBATTLE_ACK";
		case PROTOCOL_BATTLE_STARTBATTLE_ACK : return "PROTOCOL_BATTLE_STARTBATTLE_ACK";
		case PROTOCOL_BATTLE_COUNT_DOWN_ACK : return "PROTOCOL_BATTLE_COUNT_DOWN_ACK";
		case PROTOCOL_BATTLE_GIVEUPBATTLE_ACK : return "PROTOCOL_BATTLE_GIVEUPBATTLE_ACK";
		case PROTOCOL_BATTLE_RESPAWN_ACK : return "PROTOCOL_BATTLE_RESPAWN_ACK";
		case PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK : return "PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK";
		case PROTOCOL_BATTLE_USER_SKILL_UPGRADE_ACK : return "PROTOCOL_BATTLE_USER_SKILL_UPGRADE_ACK";
		case PROTOCOL_BATTLE_WAVE_PRE_START_ACK : return "PROTOCOL_BATTLE_WAVE_PRE_START_ACK";
		case PROTOCOL_BATTLE_WAVE_INFO_ACK : return "PROTOCOL_BATTLE_WAVE_INFO_ACK";
		case PROTOCOL_BATTLE_WAVE_READY_INFO_ACK : return "PROTOCOL_BATTLE_WAVE_READY_INFO_ACK";
		case PROTOCOL_BATTLE_WAVE_START_ACK : return "PROTOCOL_BATTLE_WAVE_START_ACK";
		case PROTOCOL_BATTLE_WAVE_END_ACK : return "PROTOCOL_BATTLE_WAVE_END_ACK";
		case PROTOCOL_BATTLE_PRINT_MESSAGE_ACK : return "PROTOCOL_BATTLE_PRINT_MESSAGE_ACK";
		case PROTOCOL_BATTLE_FINAL_DOMI_SCORE_ACK : return "PROTOCOL_BATTLE_FINAL_DOMI_SCORE_ACK";
		case PROTOCOL_BATTLE_USE_ITEM_ACK : return "PROTOCOL_BATTLE_USE_ITEM_ACK";
		case PROTOCOL_BATTLE_APPEAR_ACIDITEM_ACK : return "PROTOCOL_BATTLE_APPEAR_ACIDITEM_ACK";
		case PROTOCOL_BATTLE_MUTANTTREX_HOWLING_SLOW_ACK : return "PROTOCOL_BATTLE_MUTANTTREX_HOWLING_SLOW_ACK";
		case PROTOCOL_BATTLE_NAVIMESH_ACK : return "PROTOCOL_BATTLE_NAVIMESH_ACK";
		case PROTOCOL_BATTLE_AI_COLLISION_ACK : return "PROTOCOL_BATTLE_AI_COLLISION_ACK";
		case PROTOCOL_BATTLE_DEATH_ACK : return "PROTOCOL_BATTLE_DEATH_ACK";
		case PROTOCOL_BATTLE_ENDBATTLE_ACK : return "PROTOCOL_BATTLE_ENDBATTLE_ACK";
		case PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK : return "PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK";
		case PROTOCOL_BATTLE_TIMERSYNC_ACK : return "PROTOCOL_BATTLE_TIMERSYNC_ACK";
		case PROTOCOL_BATTLE_HOLE_CHECK_ACK : return "PROTOCOL_BATTLE_HOLE_CHECK_ACK";
		case PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK : return "PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK";
		case PROTOCOL_BATTLE_SENDPING_ACK : return nullptr;
		case PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK : return "PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK";
		case PROTOCOL_BATTLE_MISSION_ROUND_START_ACK : return "PROTOCOL_BATTLE_MISSION_ROUND_START_ACK";
		case PROTOCOL_BATTLE_MISSION_ROUND_END_ACK : return "PROTOCOL_BATTLE_MISSION_ROUND_END_ACK";
		case PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK : return "PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK";
		case PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK : return "PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK";
		case PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK : return "PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK";
		case PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK : return "PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK";
		case PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK : return "PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK";
		case PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK : return "PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK";
		case PROTOCOL_BATTLE_RECORD_ACK : return "PROTOCOL_BATTLE_RECORD_ACK";
		case PROTOCOL_BATTLE_START_COUNTDOWN_ACK : return "PROTOCOL_BATTLE_START_COUNTDOWN_ACK";
		case PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK : return "PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK";
		case PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK : return "PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK";
		case PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK : return "PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK";
		case PROTOCOL_BATTLE_START_KICKVOTE_ACK : return "PROTOCOL_BATTLE_START_KICKVOTE_ACK";
		case PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK : return "PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK";
		case PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK : return "PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK";
		case PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK : return "PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK";
		case PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK : return "PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK";
		case PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK : return "PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK";
		case PROTOCOL_BATTLE_MISSION_TOUCHDOWN_BLOCK_ACK : return "PROTOCOL_BATTLE_MISSION_TOUCHDOWN_BLOCK_ACK";
		case PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK : return "PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK";
		case PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK : return "PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK";
		case PROTOCOL_BATTLE_GM_PAUSE_ACK : return "PROTOCOL_BATTLE_GM_PAUSE_ACK";
		case PROTOCOL_BATTLE_GM_RESUME_ACK : return "PROTOCOL_BATTLE_GM_RESUME_ACK";
		case PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_ACK : return "PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_ACK";
		case PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_OTHER_ACK : return "PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_OTHER_ACK";
		case PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK : return "PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK";
		case PROTOCOL_BATTLE_USER_SOPETYPE_ACK : return "PROTOCOL_BATTLE_USER_SOPETYPE_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_BATTLE_XXX Unknown Protocol Name");
			return "PROTOCOL_BATTLE_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingBattle( i3NetworkPacket * pPacket)
{
	// 배틀이 아닐때 받으면 경고
	if( !g_pFramework->IsBattleStage() && !BattleServerContext::i()->IsMovingStage())
	{
		// 예외 프로토콜 아닐때만 출력
		PROTOCOL ui16Exception = pPacket->GetProtocolID();

		if( ui16Exception != PROTOCOL_BATTLE_READYBATTLE_ACK &&
			ui16Exception != PROTOCOL_BATTLE_START_GAME_ACK &&
			ui16Exception != PROTOCOL_BATTLE_START_GAME_TRANS_ACK &&
			ui16Exception != PROTOCOL_BATTLE_GIVEUPBATTLE_ACK &&			// 대기방에 나올때 이패킷을 받을 수 있다.
			ui16Exception != PROTOCOL_BATTLE_START_COUNTDOWN_ACK &&
			ui16Exception != PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK &&		// 대기방에서 난입시 주는 현재 상황판 정보
			ui16Exception != PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK )		// 대기방에서 주는 다른 유저의 장비 품목
		{
			I3PRINTLOG(I3LOG_WARN, "[WARNING] Recv battle packet - Protocol ID(%d)", pPacket->GetProtocolID());
			return;
		}
	}

	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Battle.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Battle.end() )
	{
		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		const char * pszProtocolName = getProtocolName( pPacket->GetProtocolID());
		if( pszProtocolName != nullptr)
		{
			I3PRINTLOG(I3LOG_QA, "* %s -->", pszProtocolName);
		}
		#endif

		netFunc & mf = it->second;
		(this->*mf)(pPacket);

		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if( pszProtocolName != nullptr)
		{
			I3PRINTLOG(I3LOG_QA, "* %s <--", pszProtocolName);
		}
		#endif
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Resigter_Battle_Function( void)
{
	RegisterParser( PROTOCOL_BATTLE_READYBATTLE_ACK,					&ClientTCPSocket::__Parse_Battle_ReadyBattle);
	RegisterParser( PROTOCOL_BATTLE_START_GAME_ACK,						&ClientTCPSocket::__Parse_Battle_StartGame);
	RegisterParser( PROTOCOL_BATTLE_START_GAME_TRANS_ACK,				&ClientTCPSocket::__Parse_Battle_StartGameTrans);
	RegisterParser( PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_ACK,		&ClientTCPSocket::__Parse_Battle_ReceiveEntrancedUserInfo);			// 구현필요
	RegisterParser( PROTOCOL_BATTLE_NEW_LOAD_EQUIPMENT_INFO_OTHER_ACK,	&ClientTCPSocket::__Parse_Battle_ReceiveAnotherUserInfo);		// 구현필요
	RegisterParser( PROTOCOL_BATTLE_NEW_EQUIPMENT_INFO_OTHER_ACK,		&ClientTCPSocket::__Parse_Battle_ReceiveAnotherEquipInfo);			// 구현필요
	RegisterParser( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK,					&ClientTCPSocket::__Parse_Battle_Prestart);
	RegisterParser( PROTOCOL_BATTLE_STARTBATTLE_ACK,					&ClientTCPSocket::__Parse_Battle_Start);
	RegisterParser( PROTOCOL_BATTLE_COUNT_DOWN_ACK,						&ClientTCPSocket::__Parse_Battle_Start_Count_Down);
	RegisterParser( PROTOCOL_BATTLE_GIVEUPBATTLE_ACK,					&ClientTCPSocket::__Parse_Battle_Giveup);
	RegisterParser( PROTOCOL_BATTLE_RESPAWN_ACK,						&ClientTCPSocket::__Parse_Battle_Respawn);
	RegisterParser( PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK,					&ClientTCPSocket::__Parse_Battle_RespawnForAI);

	RegisterParser( PROTOCOL_BATTLE_USER_SKILL_UPGRADE_ACK,				&ClientTCPSocket::__Parse_Battle_Domination_UserSkillUpgrade);
	RegisterParser( PROTOCOL_BATTLE_WAVE_PRE_START_ACK,					&ClientTCPSocket::__Parse_Battle_Domination_WavePreStart);
	RegisterParser( PROTOCOL_BATTLE_WAVE_INFO_ACK,						&ClientTCPSocket::__Parse_Battle_Domination_WaveInfo);
	RegisterParser( PROTOCOL_BATTLE_WAVE_READY_INFO_ACK,				&ClientTCPSocket::__Parse_Battle_Domination_WaveReadyInfo);
	RegisterParser( PROTOCOL_BATTLE_WAVE_START_ACK,						&ClientTCPSocket::__Parse_Battle_Domination_WaveStart);
	RegisterParser( PROTOCOL_BATTLE_WAVE_END_ACK,						&ClientTCPSocket::__Parse_Battle_Domination_WaveEnd);
	RegisterParser( PROTOCOL_BATTLE_PRINT_MESSAGE_ACK,					&ClientTCPSocket::__Parse_Battle_Domination_PrintMessage);
	RegisterParser( PROTOCOL_BATTLE_FINAL_DOMI_SCORE_ACK,				&ClientTCPSocket::__Parse_Battle_Domination_FinalScore);
	//RegisterParser( PROTOCOL_BATTLE_USE_ITEM_ACK,						&ClientTCPSocket::__Parse_Battle_Domination_UseItem);
	RegisterParser( PROTOCOL_BATTLE_APPEAR_ACIDITEM_ACK,				&ClientTCPSocket::__Parse_Battle_Domination_AppearAcidItem);
	RegisterParser( PROTOCOL_BATTLE_MUTANTTREX_HOWLING_SLOW_ACK,		&ClientTCPSocket::__Parse_Battle_Domination_MutantTrexHowlingSlow);
	RegisterParser( PROTOCOL_BATTLE_NAVIMESH_ACK,						&ClientTCPSocket::__Parse_Battle_Domination_NaviMesh_or_AICollision);
	RegisterParser( PROTOCOL_BATTLE_AI_COLLISION_ACK,					&ClientTCPSocket::__Parse_Battle_Domination_NaviMesh_or_AICollision);

	RegisterParser( PROTOCOL_BATTLE_DEATH_ACK,							&ClientTCPSocket::__Parse_Battle_Death);
	RegisterParser( PROTOCOL_BATTLE_ENDBATTLE_ACK,						&ClientTCPSocket::__Parse_Battle_End);
	RegisterParser( PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK,					&ClientTCPSocket::__Parse_Battle_ChangeHost);
	RegisterParser( PROTOCOL_BATTLE_TIMERSYNC_ACK,						&ClientTCPSocket::__Parse_Battle_TimeSync);
	RegisterParser( PROTOCOL_BATTLE_HOLE_CHECK_ACK,						&ClientTCPSocket::__Parse_Battle_HoleCheck);
	RegisterParser( PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK,					&ClientTCPSocket::__Parse_Battle_Timeout);
	RegisterParser( PROTOCOL_BATTLE_SENDPING_ACK,						&ClientTCPSocket::__Parse_Battle_SendPing);
	RegisterParser( PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK,		&ClientTCPSocket::__Parse_Battle_Round_Prestart);
	RegisterParser( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK,			&ClientTCPSocket::__Parse_Battle_Round_Start);
	RegisterParser( PROTOCOL_BATTLE_MISSION_ROUND_END_ACK,				&ClientTCPSocket::__Parse_Battle_Round_End);
	RegisterParser( PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK,			&ClientTCPSocket::__Parse_Battle_Install_Bomb);
	RegisterParser( PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK,			&ClientTCPSocket::__Parse_Battle_Uninstall_Bomb);
	RegisterParser( PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK,			&ClientTCPSocket::__Parse_Battle_ReceiveDestructionModeInfo);
	RegisterParser( PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK,			&ClientTCPSocket::__Parse_Battle_ReceiveDefenceModeInfo);
	RegisterParser( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK,				&ClientTCPSocket::__Parse_Battle_ReceiveTouchdownModeInfo);
	RegisterParser( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK,		&ClientTCPSocket::__Parse_Battle_TourchdownCount);
	RegisterParser( PROTOCOL_BATTLE_RECORD_ACK,							&ClientTCPSocket::__Parse_Battle_Record);
	RegisterParser( PROTOCOL_BATTLE_START_COUNTDOWN_ACK,				&ClientTCPSocket::__Parse_Battle_CountdownForStart);
	RegisterParser( PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK,		&ClientTCPSocket::__Parse_Battle_ChangeDifficultyLevelForAI);
	RegisterParser( PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK,		&ClientTCPSocket::__Parse_Battle_Tutorial_Round_End);
	RegisterParser( PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK,				&ClientTCPSocket::__Parse_Battle_SuggestKickVote);
	RegisterParser( PROTOCOL_BATTLE_START_KICKVOTE_ACK,					&ClientTCPSocket::__Parse_Battle_StartKickVote);
	RegisterParser( PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK,					&ClientTCPSocket::__Parse_Battle_VoteKickVote);
	RegisterParser( PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK,		&ClientTCPSocket::__Parse_Battle_NotifyKickVote);
	RegisterParser( PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK,			&ClientTCPSocket::__Parse_Battle_CancelKickVote);
	RegisterParser( PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK,			&ClientTCPSocket::__Parse_Battle_KickVoteResult);
	RegisterParser( PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK,	&ClientTCPSocket::__Parse_Battle_BeKickedByVote);
	RegisterParser( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_BLOCK_ACK,		&ClientTCPSocket::__Parse_Battle_BlockTouchdown);
	RegisterParser( PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK,				&ClientTCPSocket::__Parse_Battle_NotifyHackingUser);
	RegisterParser( PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK,					&ClientTCPSocket::__Parse_Battle_EquipFromBattleShop);
	RegisterParser( PROTOCOL_BATTLE_GM_PAUSE_ACK,						&ClientTCPSocket::__Parse_Battle_GMPause_Activate);
	RegisterParser( PROTOCOL_BATTLE_GM_RESUME_ACK,						&ClientTCPSocket::__Parse_Battle_GMPause_Deactivate);
	RegisterParser( PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_ACK,			&ClientTCPSocket::__Parse_Battle_LoadInvenInfo);
	RegisterParser( PROTOCOL_BATTLE_NEW_LOAD_INVEN_INFO_OTHER_ACK,		&ClientTCPSocket::__Parse_Battle_LoadInvenInfo);
	RegisterParser( PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK,			&ClientTCPSocket::__Parse_Battle_RoomScore);
	RegisterParser( PROTOCOL_BATTLE_USER_SOPETYPE_ACK,					&ClientTCPSocket::__Parse_Battle_ScopeType);
}

void ClientTCPSocket::__Parse_Battle_ReadyBattle( i3NetworkPacket * pPacket)
{
	PACKET_BATTLE_READYBATTLE_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_READY_BATTLE, Recv.m_TResult );
}

static void S2_CHAR_BATTLE_INFOCopyToNET_CHARA_INFO( const S2_CHAR_BATTLE_INFO * pSrc)
{
	NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( pSrc->m_ui8SlotIdx );

	// 무기 정보
	CHARA_WEAPON_INFO WeaponInfo;
#if defined(I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	for( UINT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; i++)
	{
		I3ASSERT( ValidCheck::Item::Weapon::isWeaponItemID( (WEAPON_SLOT_TYPE) i, pSrc->m_TBattleEquipWeapon[ i]));
	}
#endif

	for( UINT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; i++)
	{
		WeaponInfo.m_Info[ i ].m_TItemID	= pSrc->m_TBattleEquipWeapon[ i ];
	}
	pNetCharaInfo->SetWeaponList( &WeaponInfo );

	// 무기 내구도
	pNetCharaInfo->setEndurance( (const UINT8*)pSrc->m_ui8UseWeaponEndurance );

	// vv3 - check
	// 캐릭터 파츠 정보
	CHARA_PARTS_INFO CharPartsInfo;

	// vv3 - job 떔빵 코드. 추후 구조 변경 필요
	CharPartsInfo.SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA,		pSrc->m_TCharItemID );
	for( INT32 i = CHAR_EQUIPMENT_PARTS_HEAD ; i < CHAR_EQUIPMENT_PARTS_COUNT ; ++i )
	{
		CharPartsInfo.SetPartsItemID( (CHAR_EQUIPMENT_PARTS)i,		pSrc->m_TBattleEquipParts[ i ] );
	}

	pNetCharaInfo->setCharaParts( &CharPartsInfo );

	COMMON_PARTS_INFO CommonParts;

	// vv3 - job. 공룡 ItemID pSrc의 공용파츠 에서 가져오기.
	if( ValidCheck::Item::isDinoItemID( pSrc->m_TBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA] ) == true)
		CommonParts.SetPartsItemID( pSrc->m_TBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA] );
	else 
		CommonParts.SetPartsItemID( DEFAULT_DINO ); 


	CommonParts.SetPartsIdx( 0 );
	pNetCharaInfo->setCommonParts( &CommonParts );

	// 호칭 정보
	pNetCharaInfo->ApplyEquipUserTitle( pSrc->m_ui8EquipUserTitle );

	// 스킬 정보
	CHARA_SLOT_SKILL skill;
	skill.m_PrimarySkill		= (CHARA_CLASS) pSrc->m_SkillInfo.m_ui8Class;
	skill.m_CommonSkill			= (COMMON_SKILL_TYPE) pSrc->m_SkillInfo.m_ui8CommonSkill;
	if( skill.m_CommonSkill >= MAX_COMMON_SKILL)
		skill.m_CommonSkill = COMMON_SKILL_NONE;
	skill.m_MainSkill			= (MAIN_SKILL_TYPE) pSrc->m_SkillInfo.m_ui8MainSkill;
	if( skill.m_MainSkill >= MAX_MAIN_SKILL)
		skill.m_MainSkill = MAIN_SKILL_NONE;
	skill.m_AssistSkill			= (ASSIST_SKILL_TYPE) pSrc->m_SkillInfo.m_ui8AssistSkill;
	if( skill.m_AssistSkill >= MAX_ASSIST_SKILL)
		skill.m_AssistSkill = ASSIST_SKILL_NONE;
	skill.m_ui8CommonSkillLv	= pSrc->m_SkillInfo.m_ui8CommonSkillLv;
	skill.m_ui8MainSkillLv		= pSrc->m_SkillInfo.m_ui8MainSkillLv;
	skill.m_ui8AssistSkillLv	= pSrc->m_SkillInfo.m_ui8AssistSkillLv;
	pNetCharaInfo->SetSkillsystem( skill );
}

void ClientTCPSocket::__Parse_Battle_StartGame( i3NetworkPacket * pPacket)
{
	PACKET_BATTLE_START_GAME_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	// 추후 정리 예정입니다. 김현우
	bool loadFlag = false;

	// 튜토리얼은 어떤 값이 날아와도 실행
	// 만약 무기를 가지고 있지 않은 상태에서 실행시 잘못된 값이 날아오는데 그래도 바로 실행
	// 여기까지는 처리해야 게임이 무사히 실행됨
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
	{
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_READY_BATTLE, SLOT_STATE_LOAD );
		return;
	}

	// 배틀에 진입하게 된다.
	ROOM_INFO_BASIC	roomInfo;
	loadFlag = true;

	g_pCharaManager->PreStartStage();

	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
	roomInfo._StageID = Recv.m_ui32StageID;

	MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);

	// 이 부분에서 내 슬롯이 아닌 다른 유저는 백그라운드 로딩 해 주세요.
	for( INT32 i = 0 ; i < Recv.m_stBattleInfo.GetCount() ; i++ )
	{
		S2_CHAR_BATTLE_INFO* pEquipInfo = &Recv.m_stBattleInfo[i];

		{
			S2_CHAR_BATTLE_INFOCopyToNET_CHARA_INFO( pEquipInfo);

			// 캐릭터는 로딩을 시작하자
			g_pCharaManager->setCharaResState( pEquipInfo->m_ui8SlotIdx, CGameCharaManager::RESSTATE_LOADCMD);
			I3TRACE( "Req Load chara %d\n", pEquipInfo->m_ui8SlotIdx );
		}
	}

	if (MyRoomInfoContext::i()->getStage()->IsAiMode())
		AssistContext::i()->AllResetAssistor();

	for(INT32 i = 0; i<SLOT_MAX_COUNT; i++)
		CrosshairMgr::i()->setUserScopeTypes( i, Recv.m_aui8UsersScopeType[i]);

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_START_GAME );
}

void ClientTCPSocket::__Parse_Battle_StartGameTrans( i3NetworkPacket * pPacket)
{
	// 난입한 유저에 대한 정보
	PACKET_BATTLE_START_GAME_TRANS_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	S2_CHAR_BATTLE_INFO* pBattleInfo = &Recv.m_stBattleInfo;
	if( pBattleInfo->m_ui8SlotIdx < SLOT_MAX_COUNT )
	{
		I3ASSERT( pBattleInfo->m_ui8SlotIdx != BattleSlotContext::i()->getMySlotIdx() );
		S2_CHAR_BATTLE_INFOCopyToNET_CHARA_INFO( pBattleInfo);

		// 캐릭터는 로딩을 시작하자
		g_pCharaManager->setCharaResState( pBattleInfo->m_ui8SlotIdx, CGameCharaManager::RESSTATE_LOADCMD);
		I3TRACE( "Req Load chara %d\n", pBattleInfo->m_ui8SlotIdx );

		// 공수교대후에 난입하고 관전상태로 가는 유저의경우 BattleServerContext::i()->Battle_EntranceAnotherUser
		// 에서 로딩하면서 팀세팅해준다.

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_ENTERANCE_USER);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN,  "invalid index from entering user.");
	}
}

void ClientTCPSocket::__Parse_Battle_ReceiveEntrancedUserInfo( i3NetworkPacket * pPacket)
{
	bool bReqLoad = false;

	PACKET_H_BATTLE_EQUIPMENT_INFO_ACK	readPacket;

	pPacket->ReadData( &readPacket, sizeof( readPacket));

	I3ASSERT( readPacket.m_ui8PosCharaCount <= SLOT_MAX_COUNT);

	// 스테이지 진입시 로딩해야 하는 캐릭터들의 정보를 받는다.
	for( UINT8 i = 0; i < readPacket.m_ui8PosCharaCount; i++ )
	{
		PACKET_D_BATTLE_EQUIPMENT_INFO info;

		pPacket->ReadData( &info, sizeof( info));

		if( info.m_i32SlotIdx > -1 && info.m_i32SlotIdx < SLOT_MAX_COUNT)
		{
			if( info.m_i32SlotIdx == BattleSlotContext::i()->getMySlotIdx())	// 자신은 제외
				continue;

			// 로딩 상태인지 확인 
			if( g_pCharaManager->getCharaResState( info.m_i32SlotIdx) == CGameCharaManager::RESSTATE_NONE)
			{
				NET_CHARA_INFO * pCharaInfo  = BattleSlotContext::i()->setNetCharaInfo( info.m_i32SlotIdx );
				pCharaInfo->setEndurance( info.m_ui8WeaponEndurance);
		
				if( info.m_PosMultiSlot.m_CharaEquipInfo.m_CharPartsInfo.IsValid() == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "Invalid character equipment! Please contact the server team.");
					continue;
				}

				pCharaInfo->setCharaParts( &info.m_PosMultiSlot.m_CharaEquipInfo.m_CharPartsInfo);

				// vv3 - check
				if( ValidCheck::Item::isDinoItemID( info.m_PosMultiSlot.m_CommonPartsInfo.GetPartsItemID()) == false)
					info.m_PosMultiSlot.m_CommonPartsInfo.SetPartsItemID(DEFAULT_DINO);

				pCharaInfo->setCommonParts( &info.m_PosMultiSlot.m_CommonPartsInfo);
				// 무기 설정
				CHARA_WEAPON_INFO_OTHER* _Info = &info.m_PosMultiSlot.m_CharaEquipInfo.m_CharWeaponInfo;
				// vv3 - check
				for( INT32 k = 0; k < CHAR_EQUIPMENT_WEAPON_COUNT ; ++k )
				{ 
					pCharaInfo->setWeaponItemID( (WEAPON_SLOT_TYPE)k, _Info->m_Info[k], -1, __FILE__, __LINE__);
				}

				//
				// 호칭설정에 필요한 서버 데이터가 없어서 여기서는 pCharaInfo->ApplyUserTitleInfo는 호출하지 못함..(추후 문제가 되면 
				// 이곳에 설정되도록 UINT8 UserTitleInfo[3] 서버데이터 확보해야한다..) 2015.05.22.수빈..
				//

				// 캐릭터 스킬 셋팅..
				pCharaInfo->SetSkillsystem( info.m_PosMultiSlot.m_PosCharaSkill);

				g_pCharaManager->setCharaResState( info.m_i32SlotIdx, CGameCharaManager::RESSTATE_LOADCMD);
				I3TRACE( "EntranceUser Req Chara Load %d\n", info.m_i32SlotIdx);
				
				bReqLoad = true;
			}
		}
	}

	if( bReqLoad )
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_ENTERANCE_USER);
}

void ClientTCPSocket::__Parse_Battle_ReceiveAnotherUserInfo( i3NetworkPacket * pPacket)
{
	PACKET_BATTLE_LOAD_EQUIPMENT_INFO_OTHER_ACK	readPacket;

	pPacket->ReadData( &readPacket, sizeof( readPacket));

	// 스테이지 진입시 로딩해야 하는 캐릭터들의 정보를 받는다.
	if( readPacket.m_i32SlotIdx > -1 && readPacket.m_i32SlotIdx < SLOT_MAX_COUNT)
	{
		if( readPacket.m_i32SlotIdx == BattleSlotContext::i()->getMySlotIdx())	// 자신은 제외
			return;

		// 로딩 상태인지 확인
		if( g_pCharaManager->getCharaResState( readPacket.m_i32SlotIdx) == CGameCharaManager::RESSTATE_NONE)
		{
			NET_CHARA_INFO* pCharaInfo  = BattleSlotContext::i()->setNetCharaInfo( readPacket.m_i32SlotIdx);
			pCharaInfo->setEndurance( readPacket.m_ui8WeaponEndurance);

			if( readPacket.m_PosMultiSlot.m_CharaEquipInfo.m_CharPartsInfo.IsValid() == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Invalid character item information! Please contact the support.");
				return;
			}
			

			// 무기 설정

			pCharaInfo->setCharaParts( &readPacket.m_PosMultiSlot.m_CharaEquipInfo.m_CharPartsInfo);
			// vv3 - check
			if( ValidCheck::Item::isDinoItemID( readPacket.m_PosMultiSlot.m_CommonPartsInfo.GetPartsItemID()) == false)
				readPacket.m_PosMultiSlot.m_CommonPartsInfo.SetPartsItemID(DEFAULT_DINO);
			pCharaInfo->setCommonParts( &readPacket.m_PosMultiSlot.m_CommonPartsInfo);

			CHARA_WEAPON_INFO_OTHER* _Info = &readPacket.m_PosMultiSlot.m_CharaEquipInfo.m_CharWeaponInfo;

			// vv3 - check
			for( INT32 k = 0; k < CHAR_EQUIPMENT_WEAPON_COUNT ; ++k )
			{
				pCharaInfo->setWeaponItemID( (WEAPON_SLOT_TYPE)k, _Info->m_Info[k], -1, __FILE__, __LINE__);
			}
			
			g_pCharaManager->setCharaResState( readPacket.m_i32SlotIdx, CGameCharaManager::RESSTATE_LOADCMD);
			I3TRACE( "Req Chara Load by AnotherUser %d\n", readPacket.m_i32SlotIdx);
		}

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_ENTERANCE_USER);
	}
}

void ClientTCPSocket::__Parse_Battle_ReceiveAnotherEquipInfo( i3NetworkPacket * pPacket)
{
}

void ClientTCPSocket::__Parse_Battle_Prestart( i3NetworkPacket * pPacket)
{
	INT32	result;
	BOOL	bSuccess; 
	UINT32	SlotIdx;
	UINT8  iBattleType; 
	UINT32 wRelayIP;
	UINT16 sPort;
	UINT32 wGroupId; 
	UINT32 wKey;

	pPacket->ReadData( &bSuccess,	sizeof(BOOL));		//성공여부 
	pPacket->ReadData( &SlotIdx,	sizeof(UINT32));	//슬롯 인덱스 

	if(bSuccess)
	{
		if( SlotIdx  < USER_SLOT_COUNT)
		{
			INT32 nMainSlotIdx = BattleSlotContext::i()->getMainSlotIdx();	
			I3_BOUNDCHK( nMainSlotIdx, SLOT_MAX_COUNT);

			pPacket->ReadData( &iBattleType, sizeof(UINT8)); 
			pPacket->ReadData(&wRelayIP,	sizeof(UINT32));
			pPacket->ReadData(&sPort,		sizeof(UINT16));
			pPacket->ReadData(&wGroupId,	sizeof(UINT32));
			pPacket->ReadData(&wKey,		sizeof(UINT32));

			if( BattleSlotContext::i()->getMySlotIdx() == (INT32)SlotIdx )
			{
			#ifdef _SHUFFLE_HIT_PART
				{
					HitPartContext::i()->reset();
					// 원래의 hit part를 찾기 위한 배열입니다. 
					// 이 값을 이용해서 원래의 인덱스를 매핑합니다.
					UINT8	aui8HitPartIndex[ CHARA_HIT_MAXCOUNT ];
					pPacket->ReadData(	aui8HitPartIndex, sizeof(aui8HitPartIndex) );

					for( int i = 0; i < CHARA_HIT_MAXCOUNT; i++ )
					{
						I3ASSERT(aui8HitPartIndex[i]< CHARA_HIT_MAXCOUNT);
						HitPartContext::i()->SetHitPart(i, aui8HitPartIndex[i]);
					}
				}
			#endif
			
				BattleServerContext::i()->SetServerType( (BATTLE_SERVER_TYPE)iBattleType );
				
				switch(iBattleType)
				{
				case BATTLE_SERVER_TYPE_RELAY		: 
				case BATTLE_SERVER_TYPE_DEDICATION	: 
					if ((INT32)SlotIdx == BattleSlotContext::i()->getMySlotIdx())
					{
						NetworkContext::i()->RelayHolePunch( wRelayIP, sPort, wGroupId, wKey);
					}
					break; 
				case BATTLE_SERVER_TYPE_P2P			:
				default:
					break; 
				}

			#if defined( I3_DEBUG)
				switch(iBattleType)
				{
				case BATTLE_SERVER_TYPE_RELAY		:	I3TRACE( "Server Type : Relay\n");			break;
				case BATTLE_SERVER_TYPE_DEDICATION	:	I3TRACE( "Server Type : Dedicated\n");		break; 
				case BATTLE_SERVER_TYPE_P2P			:	I3TRACE( "Server Type : P2P\n");			break;
				default:	break; 
				}
			#endif

				//서버 타입 표시
				g_pFramework->PrintVersion(true);
			}
			
			result = SlotIdx;
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  " *** PRESTART ERROR %d ", SlotIdx);
			return; 
		}
	}
	else
		result = EVENT_ERROR_PRESTART_BATTLE_ALEADY_END;

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PRESTART_BATTLE, result);
}


void ClientTCPSocket::__Parse_Battle_Start( i3NetworkPacket * pPacket)
{
	INT32	result;

	PACKET_BATTLE_STARTBATTLE_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	INT32	SlotIdx = static_cast<INT32>(Recv.m_ui8SlotIdx);

	// 추후 정리 예정
	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		if( BattleSlotContext::i()->getMySlotIdx() == (INT32)SlotIdx)
			IntrudeContext::i()->InterEnter( Recv.m_ui8InterEnter );

		DinoModeContext::i()->DinoSlot.Reset();
		ScoreContext::i()->setCurrentRound(Recv.m_ui8NowRoundCount);


		// 서버에서 보내주는 라운드값은 이제 자동공수전환여부와 상관없이 원래팀순서로 보내주므로 별도 저장해두고,
    	// 팀변경에 유무에따라 클라이언트에서 바꿔서 보여주게됩니다.
		ScoreContext::i()->SetSvrTeamWinRound(Recv.m_ui16TotalRoundCount[TEAM_RED], Recv.m_ui16TotalRoundCount[TEAM_BLUE]);
		
		UserContext::i()->BattleUserFlag				= Recv.m_ui16BattleUser;

		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB)
			|| MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION)
			|| MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )		// 폭파미션
		{
			UserContext::i()->PlayingSlotFlag			= Recv.m_ui16RoundStartUser;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )	//탈출 미션 
		{
			// 2014-09-04 chunjong.song 공룡탈출모드는 자동공수교대가 지원되지않습니다.
			// 라운드가끝나면 공룡가 인간이 교체되는데 이것은 자동공수교대처럼 팀이 교체된것이아니고
			// 캐릭터만 공룡->인간으로 바뀐거뿐입니다.

			DinoModeContext::i()->RedTouchDownCount		= Recv.m_ui16TotalRoundCount[TEAM_RED];
			DinoModeContext::i()->BlueTouchDownCount	= Recv.m_ui16TotalRoundCount[TEAM_BLUE];
			DinoModeContext::i()->CurrentDinoRound		= Recv.m_ui8NowRoundCount;

			i3mem::Copy( &DinoModeContext::i()->DinoSlot, &Recv.m_MiDinoInfo, sizeof(DINO_ROOM_INFO) );
			UserContext::i()->PlayingSlotFlag = (UINT16)0xFFFF;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY))	// 파괴 미션
		{
			//CGameMissionManager::i()->setDestructionInfoNoEffect( &(stReadData.m_MiDestructionInfo));//setDestructionInfo( &info);
			UserContext::i()->PlayingSlotFlag = (UINT16)0xFFFF;
		}
		else if(  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE))	// 방어 미션
		{
			// 오브젝트가 파괴된 상태라면 이팩트는 처리 하지 않습니다.
			//if(BattleSlotContext::i()->getMySlotIdx() == (INT32) SlotIdx)		
			//	CGameMissionManager::i()->setDefenceInfoNoEffect( &(stReadData.m_MiDefenceInfo) );
			//else	// revision 43552 방어미션에서 난입유저의 데이터 초기화
			//	CGameMissionManager::i()->setDefenceInfoIntrude( &stReadData.m_MiDefenceInfo, SlotIdx );	// 난입한 유저 데이터 처리

			UserContext::i()->PlayingSlotFlag = (UINT16)0xFFFF;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))		// CC 모드
		{
			DinoModeContext::i()->CurrentDinoRound		= Recv.m_ui8NowRoundCount;
			
			i3mem::Copy( &DinoModeContext::i()->DinoSlot, &Recv.m_MiDinoInfo, sizeof(DINO_ROOM_INFO) );
			UserContext::i()->PlayingSlotFlag = (UINT16)0xFFFF;
		}
		else
		{//
			UserContext::i()->PlayingSlotFlag = (UINT16)0xFFFF;
		}

		///
		IntrudeContext::i()->SetIntrusionMsgFlag( IntrudeContext::i()->IsInterEnter() ); 

		result = SlotIdx;

		if( BattleSlotContext::i()->getMySlotIdx() == (INT32)SlotIdx)
		{
			MCardUtil::Update_Recv_PROTOCOL_BATTLE_STARTBATTLE_ACK();
		}

		//@eunil. PP-23552 일단 배틀 간 것이기 때문에 기브업이니 정상종료니 복잡할거 없이 여기서 체크하자
		UserInfoContext::i()->SetShowTSRewardMsg(true);
	}
	else
	{
		result = EVENT_ERROR_START_BATTLE_ALEADY_END;
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_START_BATTLE, result);
	if(CHUD_CountDown::instance() != nullptr)
		CHUD_CountDown::instance()->stop_count_down();
}

void ClientTCPSocket::__Parse_Battle_Start_Count_Down( i3NetworkPacket * pPacket)
{
	UINT8 count_time;
	pPacket->ReadData( &count_time, sizeof(UINT8) );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_START_COUNTDOWN, count_time);
}

void ClientTCPSocket::__Parse_Battle_Giveup( i3NetworkPacket * pPacket)
{
	UINT32 UTemp;

	pPacket->ReadData( &UTemp, sizeof(UINT32) );

	if( UTemp < SLOT_MAX_COUNT)
	{
		if( UTemp == (UINT32)BattleSlotContext::i()->getMySlotIdx())
		{
			USER_INFO_BASIC		MyUserInfoBasic;
			UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
			pPacket->ReadData( &MyUserInfoBasic.m_ui32Exp,	sizeof(UINT32) );
			pPacket->ReadData( &MyUserInfoBasic.m_ui32Rank,	sizeof(UINT32) );
			pPacket->ReadData( &MyUserInfoBasic.m_ui32Point,	sizeof(UINT32) );

			pPacket->ReadData( &UserInfoContext::i()->SetMyUserInfoRecord()->_dis,	sizeof(UINT32) );
			pPacket->ReadData( &UserInfoContext::i()->SetMyUserInfoRecord()->_sdis,	sizeof(UINT32) );

			// User Daily Record
			pPacket->ReadData( &UserInfoContext::i()->SetUserInfoDaily()->_DGPlaytime,	sizeof(UINT32) );
			pPacket->ReadData( &UserInfoContext::i()->SetUserInfoDaily()->_ui8GetItem,	sizeof(UINT8) );

			UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
			
			if( g_pFramework->IsBattleStage())
			{
				if (KickContext::i()->IsKickedByVote())	
				{
					
					gstatemgr::i()->setStageState(GAMESTAGE_STATE_NONE);
					LobbyContext::i()->InitLobbyData();
					GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_LOBBY);		// SendPacket이 아닌 리시브 용도이며, 큐를 경유한다..
				}
				else
				{
					gstatemgr::i()->setStageState(GAMESTAGE_STATE_ENDGAME);
				}
			}

			if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
			{
				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
			}

			if(CGameMaster::i()->IsPaused())
				CGameMaster::i()->SetPause(false);
		}
	}

	if (MyRoomInfoContext::i()->getStage()->IsAiMode() && g_pFramework->IsBattleStage())
		AssistContext::i()->UpdateAssistorInfo();

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GIVEUP_BATTLE, UTemp );
}

void ClientTCPSocket::__Parse_Battle_Respawn( i3NetworkPacket * pPacket)
{
	PACKET_BATTLE_NEW_RESPAWN_ACK ReadPacket;

	pPacket->ReadData( &ReadPacket, sizeof(ReadPacket));

	I3PRINTLOG(I3LOG_QA, " ** %d self %d", ReadPacket.m_ui8SlotIdx, BattleSlotContext::i()->getMySlotIdx());

	if( (INT32)ReadPacket.m_ui8SlotIdx  == BattleSlotContext::i()->getMySlotIdx() )
	{
		C4Context::i()->Reset();

		BattleServerContext::i()->setRespawnReq(false);
		BattleServerContext::i()->setRespawnAckTime(0.f);
	}

	if( ReadPacket.m_ui8SlotIdx < SLOT_MAX_COUNT)
	{
		NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( ReadPacket.m_ui8SlotIdx );

		I3ASSERT( BattleSlotContext::i()->getMySlotIdx() > -1);

		if ( ReadPacket.m_ui8SlotIdx == (UINT8) BattleSlotContext::i()->getMySlotIdx())
		{
			I3_BOUNDCHK( ReadPacket.m_i32RespawnCount, 255);
			UserInfoContext::i()->SetMyRespawnCnt( ReadPacket.m_i32RespawnCount );
		}
		
		I3ASSERT( ReadPacket.m_i32TotalRespawnIdx > -1);
		pNetCharaInfo->setRespawnIndex( ReadPacket.m_i32TotalRespawnIdx );
		pNetCharaInfo->setRespawnObjIndex( -1);
		I3ASSERT( sizeof( ReadPacket.m_aUseWeaponEndurance ) == sizeof(UINT8) * CHAR_EQUIPMENT_WEAPON_COUNT);
		pNetCharaInfo->setEndurance( ReadPacket.m_aUseWeaponEndurance  );

		// 튜토리얼 은 AK 풀커스텀을 든다.
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY,  DEFAULT_WEAPON_TUTORIAL);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE,  DEFAULT_WEAPON_KNIFE);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1,  DEFAULT_WEAPON_THROWING1);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2,  DEFAULT_WEAPON_THROWING2);
		}
		else
		{
			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
			{
				//보스미션
				pPacket->ReadData( &DinoModeContext::i()->DinoSlot, sizeof(DINO_ROOM_INFO));

				NET_CHARA_INFO* pNet_Chara_Info = BattleSlotContext::i()->setNetCharaInfo( ReadPacket.m_ui8SlotIdx);
				COMMON_PARTS_INFO CommonPartsInfo;
				// vv3 - check
				if( ValidCheck::Item::isDinoItemID( ReadPacket.m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ]) == true)
					CommonPartsInfo.SetPartsItemID( ReadPacket.m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ] );
				else
					CommonPartsInfo.SetPartsItemID( DEFAULT_DINO );
				pNet_Chara_Info->setCommonParts( &CommonPartsInfo );

				g_pCharaManager->ChangeToDino( &DinoModeContext::i()->DinoSlot);
				
				// 티렉스가 나갔을 때, 다른 공룡을 리스폰시 티렉스로 만들어 준다.
				g_pCharaManager->ChangeToTRex( &DinoModeContext::i()->DinoSlot, BattleSlotContext::i()->getMySlotIdx());
			}
			else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			{
				//CC모드
				pPacket->ReadData( &DinoModeContext::i()->DinoSlot, sizeof(DINO_ROOM_INFO));
						
				NET_CHARA_INFO* pNet_Chara_Info = BattleSlotContext::i()->setNetCharaInfo( ReadPacket.m_ui8SlotIdx);

				COMMON_PARTS_INFO CommonPartsInfo;
				// vv3 - check
				if( ValidCheck::Item::isDinoItemID( ReadPacket.m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ]) == true)
					CommonPartsInfo.SetPartsItemID( ReadPacket.m_TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ] );
				else
					CommonPartsInfo.SetPartsItemID( DEFAULT_DINO );
				pNet_Chara_Info->setCommonParts( &CommonPartsInfo );
			
				g_pCharaManager->ChangeToDino( &DinoModeContext::i()->DinoSlot);
			}
		


			{	//탈출 미션, cc모드도 무기 리스트 설정.(튜토리얼은 제외)
				// 무기 설정
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY,	ReadPacket.m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_PRIMARY ], -1, __FILE__, __LINE__ );
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY,	ReadPacket.m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_SECONDARY ], -1, __FILE__, __LINE__ );
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE,		ReadPacket.m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_MELEE ], -1, __FILE__, __LINE__ );
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1,	ReadPacket.m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING1 ], -1, __FILE__, __LINE__);
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2,	ReadPacket.m_TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_THROWING2 ], -1, __FILE__, __LINE__);

				// 무기 검사
				for( UINT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
				{
					T_ItemID weaponID = pNetCharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i);

					if( weaponID != 0 && GetWeaponSlot_WeaponItemIDFunc( weaponID) != (WEAPON_SLOT_TYPE)i )
					{	// Error 잘못된 슬롯 번호

						INT32 nArg = 0;
						GameEventSender::i()->SetEvent( EVENT_GIVEUP_BATTLE, &nArg );

						CGameFramework::JumpToReady();

						/*네트워크 문제로 리스폰에 실패했습니다.*/
						GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_RESPWAN_BY_NETWORK_PROBLOM"));
						return;
					}
				}
			}
		}

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_RESPAWN_BATTLE, ReadPacket.m_ui8SlotIdx );
		//observer_battle::i()->RefreshObserver();
		gstatemgr::i()->SetChangeChara(true);


	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  " *** RESPAWN - ERROR %d", ReadPacket.m_ui8SlotIdx ); 
	}
}

void ClientTCPSocket::__Parse_Battle_RespawnForAI( i3NetworkPacket * pPacket)
{
	INT32 slotIdx;
	// vv3 - check
	pPacket->ReadData(&slotIdx, sizeof(UINT32));
	
	if (0 <= slotIdx && slotIdx < MAX_COUNT_DOMI_DINO)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE_FOR_AI, slotIdx);
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK - SLOT ERROR SLOT = %d ", slotIdx);
	}
}

void ClientTCPSocket::__Parse_Battle_Death( i3NetworkPacket * pPacket)
{
	DEATH_INFO_HEADER DeathHeader;

	pPacket->ReadData( &DeathHeader, sizeof(DEATH_INFO_HEADER)); 

	if( DeathHeader._nCount > MAX_KILLUSER_COUNT)
	{				
		I3PRINTLOG(I3LOG_FATAL,  "DEATH Packet. Deathcount invalid.%d", DeathHeader._nCount );
		return; 
	}

	SCORE_INFO svrTempScore[SLOT_MAX_COUNT]; 
	i3::vector<DEATH_INFO_CHARA>	vecDeathCharaInfo;	
	
	vecDeathCharaInfo.resize(DeathHeader._nCount);
	pPacket->ReadData( &vecDeathCharaInfo[0], sizeof(DEATH_INFO_CHARA) * DeathHeader._nCount);			

	pPacket->ReadData(&svrTempScore, sizeof(SCORE_INFO) * TEAM_COUNT);	

	log_respawn::i()->SetMyDeathAck(vecDeathCharaInfo[0]);		// DeathCharaInfo의 첫인덱스로 변경해야함..

	if ((MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) ))
	{ 
		TEAM_TYPE team = (TEAM_TYPE)(BattleSlotContext::i()->getMainSlotIdx() % 2);

		//INT32 preKillCount = ScoreContext::i()->GetTeamScore(team)->m_KillCount;	
		// 서버에서보내온값은 서버값이랑비교, 공수교대등으로 팀이바뀌면 ScoreContext::i()->GetTeamScore(team)은 상대편스코어를 준다
		INT32 preKillCount = ScoreContext::i()->GetSvrTeamScore(team)->m_KillCount;
		INT32 nowKillCount = svrTempScore[team].m_KillCount;

		if (preKillCount == nowKillCount)
		{
			AIModeContext::i()->KillCount = 0;
		}
		else
		{
			INT32 totalKillCount = nowKillCount;
			
			if (20 < nowKillCount && preKillCount < 20)			totalKillCount = 20;
			else if (40 < nowKillCount && preKillCount < 40)	totalKillCount = 40;
			else if (60 < nowKillCount && preKillCount < 60)	totalKillCount = 60;
			else if (80 < nowKillCount && preKillCount < 80)	totalKillCount = 80;
			else if (100 < nowKillCount && preKillCount < 100)	totalKillCount = 100;
			else if (120 < nowKillCount && preKillCount < 120)	totalKillCount = 120;
			else if (140 < nowKillCount && preKillCount < 140)	totalKillCount = 140;
			else if (160 < nowKillCount && preKillCount < 160)	totalKillCount = 160;
			else if (180 < nowKillCount && preKillCount < 180)	totalKillCount = 180;
			else if (200 < nowKillCount && preKillCount < 200)	totalKillCount = 200;

			switch(totalKillCount)
			{
			case 20:	// fall throght
			case 40:	// fall throght
			case 60:	// fall throght
			case 80:	// fall throght
			case 100:	// fall throght
			case 120:	// fall throght
			case 140:	// fall throght
			case 160:	// fall throght
			case 180:	// fall throght
			case 200:
				AIModeContext::i()->KillCount = totalKillCount;
				
				if (BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx())
				{
					GameEventSender::i()->SetEvent(EVENT_CHANGE_DIFFICULTY_LEVEL);
				}
				break;
			default:
				AIModeContext::i()->KillCount = 0;
				break;
			}

			// AI 캐릭터들이 들고 있는 무기를 로딩
			AIModeContext::i()->PrepareAIWeapon();
		}
	}

	ScoreContext::i()->SetSvrTeamScore(svrTempScore[TEAM_RED], svrTempScore[TEAM_BLUE]);
	
	SCORE_INFO info[ SLOT_MAX_COUNT];
	pPacket->ReadData( info,	sizeof(info));		//스코어
	ScoreContext::i()->setIndividualScores( info, sizeof(info));

		
	//자신이면 
	UINT16 accTrainingScore = 0;
	UINT8  PreKillCount; 
	pPacket->ReadData( &PreKillCount,		sizeof(UINT8) ); 
	pPacket->ReadData( &accTrainingScore,	sizeof(UINT16)); //변수 이름이 변경 되어야 합니다. 
	
	UINT32 MyKillMsg = 0;
	INT32  MyKillCount = 0;
	INT32  MyPrevKillCount = 0;

	if( DeathHeader._nUserIdx == BattleSlotContext::i()->getMySlotIdx() )
	{
		// 킬 메시지 초기화
		UINT16 message = 0;

		// 킬 메시지 머지
		
		//0x00		0		0
		//message	State	Death

		UINT32 nKillCount = DeathHeader._nCount;
		for(UINT32 i = 0; i < DeathHeader._nCount; i++)
		{
			// 자신은 제외
			if (DeathHeader._nUserIdx == GET_DEATHINFO_IDX_TYPE( vecDeathCharaInfo[i]._DeathInfo ))
			{
				nKillCount--;						
			}
			else
			{
				SETBIT(message, GET_DEATHINFO_MESSAGE_TYPE( vecDeathCharaInfo[i]._DeathInfo ) );
			}
		}
	
		MyKillMsg = (message << 8);
		MyKillCount = nKillCount;
		MyPrevKillCount = PreKillCount;

		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
		{
			ScoreContext::i()->SetAddTrainingScore(accTrainingScore - ScoreContext::i()->GetAccTrainingScore());
			ScoreContext::i()->SetAccTrainingScore(accTrainingScore);
		}
	}

	//훈련전일때 모든 사람의 점수를 계산합니다. 
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{
		AIModeContext::i()->UserScore[DeathHeader._nUserIdx] = accTrainingScore; 
	}
	
	UINT16	pui16TotalRoundCount[ TEAM_COUNT ];
	pPacket->ReadData( pui16TotalRoundCount, (sizeof(UINT16) * TEAM_COUNT) );

	// 공룡 점수 계산 수정
	// 탈출미션일경우 스코어 적용
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
	{
		DinoModeContext::i()->RedTouchDownCount = pui16TotalRoundCount[0];
		DinoModeContext::i()->BlueTouchDownCount = pui16TotalRoundCount[1];
	}

	{
		i3::user_data ud = i3::user_data(RecvData_EVENT_DEATH_BATTLE());
		
		RecvData_EVENT_DEATH_BATTLE& data = i3::value_cast<RecvData_EVENT_DEATH_BATTLE>(ud);
		
		data.DeathCharaInfo.swap(vecDeathCharaInfo);
		data._KillWeaponFlag = DeathHeader._WeaponInfo;
		data._nByObjectIdx   = DeathHeader._nByObjectIdx;
		data.MyKillMsg		 = MyKillMsg;
		data.MyKillCount	 = MyKillCount;
		data.MyPrevKillCount = MyPrevKillCount;
		data.KillIdx		 = DeathHeader._nUserIdx;
		
		GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_DEATH_BATTLE, ud, data.KillIdx);
	}
}

void ClientTCPSocket::__Parse_Battle_End( i3NetworkPacket * pPacket)
{
	I3PRINTLOG(I3LOG_QA, "RECV ENDBATTLE_ACK" );

	//PACKET_BATTLE_ENDBATTLE_ACK Recv;
	//S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );
	UINT16 ui16Data = 0;
	pPacket->ReadData(&ui16Data, sizeof(UINT16));
	BattleResultContext::i()->setBattleEndUserFlag(ui16Data);

	UINT8 ui8Win = 0;
	pPacket->ReadData(&ui8Win, sizeof(UINT8));

	//배틀 결과창 (all user info) 
	BATTLE_RESULT & ResultInfo = BattleResultContext::i()->SetInfo();
	BATTLE_RESULT resultInfo;
	memset(&resultInfo, 0, sizeof(BATTLE_RESULT));

	pPacket->ReadData(&ResultInfo.m_BattleExp, sizeof(UINT16)*SLOT_MAX_COUNT);			// 모든 유저의 기본 Exp
	pPacket->ReadData(&ResultInfo.m_BattlePoint, sizeof(UINT16)*SLOT_MAX_COUNT);			// 모든 유저의 기본 Point
	pPacket->ReadData(&ResultInfo.m_pBattleResultIcon, sizeof(UINT8)*SLOT_MAX_COUNT);	// 모든 유저의 Event Flag ( PCCafe, Item, Event )
	pPacket->ReadData(&ResultInfo.m_BonusExp, sizeof(UINT16)*(SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT));			// 모든 유저의 보너스 Exp
	pPacket->ReadData(&ResultInfo.m_BonusPoint, sizeof(UINT16)*(SLOT_MAX_COUNT * TYPE_BATTLE_RESULT_EVENT_COUNT));			// 모든 유저의 보너스 Point
	pPacket->ReadData(&ResultInfo.m_ui16QuestEndFlag, sizeof(UINT16));

	if( CStageInfoUtil::IsModeRoundType(*MyRoomInfoContext::i()->getStage()) )
	{
		UINT16				aui16TotalRoundCount[TEAM_COUNT];

		pPacket->ReadData(&aui16TotalRoundCount, sizeof(UINT16) * TEAM_COUNT);
		ScoreContext::i()->SetSvrTeamWinRound(aui16TotalRoundCount[TEAM_RED], aui16TotalRoundCount[TEAM_BLUE]);
		pPacket->ReadData(&ResultInfo.m_BattleScore, sizeof(UINT8) * SLOT_MAX_COUNT);
	}
	else if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{
		pPacket->ReadData(&ResultInfo.m_BattleScore, sizeof(UINT16) * SLOT_MAX_COUNT);
	}

	MULTI_KILL_INFO multi_kill_info;
	memset(&multi_kill_info, 0, sizeof(MULTI_KILL_INFO));
	pPacket->ReadData(&multi_kill_info, sizeof(MULTI_KILL_INFO));

	UINT8 ui8Size = 0;
	USER_INFO_BASIC UserInfoBasic;
	USER_INFO_DAILY UserInfoDaily;
	USER_INFO_RECORD UserInfoRecord;
	memset(&UserInfoBasic, 0, sizeof(USER_INFO_BASIC));
	memset(&UserInfoDaily, 0, sizeof(USER_INFO_DAILY));
	memset(&UserInfoRecord, 0, sizeof(USER_INFO_RECORD));

	pPacket->ReadData(&ui8Size, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_strNick, ui8Size);

	pPacket->ReadData(&UserInfoBasic.m_ui32Rank, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32KeepRank, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32Point, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32Exp, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32AbusingCount, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui8TutorialIng, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui64GuideComplete, sizeof(UINT64));
	pPacket->ReadData(&UserInfoBasic.m_ui8AuthLevel, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui8SpecialAbility, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui32Coin, sizeof(UINT32));

	pPacket->ReadData(&UserInfoBasic.m_ui32Cash, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32Clanidx, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32ClanState, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32NCCoin, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui32HappyCoin, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui8PCCafe, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui8ESportID, sizeof(UINT8));

	pPacket->ReadData(&ui8Size, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_strClanName, ui8Size);

	pPacket->ReadData(&UserInfoBasic.m_ui8ClanExpRank, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui8ClanPerRank, sizeof(UINT8));
	pPacket->ReadData(&UserInfoBasic.m_ui32ClanMark, sizeof(UINT32));
	pPacket->ReadData(&UserInfoBasic.m_ui8ClanNamecolor, sizeof(UINT8));

	pPacket->ReadData(&UserInfoRecord, sizeof(USER_INFO_RECORD));
	pPacket->ReadData(&UserInfoDaily, sizeof(USER_INFO_DAILY));

	UINT8 ui8AbusingLevel = 0;
	UINT32 ui32AbusingPopupTime = 0;
	pPacket->ReadData(&ui8AbusingLevel, sizeof(UINT8));
	pPacket->ReadData(&ui32AbusingPopupTime, sizeof(UINT32));

	UserInfoContext::i()->SetMyUserInfoBasic(UserInfoBasic);
	i3mem::Copy(UserInfoContext::i()->SetMyUserInfoRecord(), &UserInfoRecord, sizeof(USER_INFO_RECORD));
	
	
	//// 어뷰징 관련 정보
	if(ui8AbusingLevel <= ABUSING_LEVEL_STRONGEST)
	{
		AbuseContext::i()->SetAbuseInfo((ABUSING_LEVEL)ui8AbusingLevel, ui32AbusingPopupTime);
	}

	UINT8 ui8AvatarSlot =  0;
	UINT32 ui32BattleTime = 0;
	UINT32 ui32SkillRelTime = 0;
	pPacket->ReadData(&ui8AvatarSlot, sizeof(UINT8));
	pPacket->ReadData(&ui32BattleTime, sizeof(UINT32));
	pPacket->ReadData(&ui32SkillRelTime, sizeof(UINT32));

	//오늘의 전적
	UserInfoContext::i()->SetUserInfoDaily(UserInfoDaily);

	// 멀티킬 정보
	if( HUD::instance() != nullptr)
	{
		UIHudGameResult* hud_game_result = (UIHudGameResult*)HUD::instance()->GetHud(HUD::GAME_RESULT);
		i3mem::Copy( &hud_game_result->multi_kill_info,	&multi_kill_info, sizeof(MULTI_KILL_INFO) );
	}

	// 스킬 정보 최신화
	CHARA_INFO * sCharaInfo = const_cast<CHARA_INFO*>( CCharaInfoContext::i()->GetCharaInfoBySlotIdx(ui8AvatarSlot));
	I3ASSERT(sCharaInfo != nullptr);
	sCharaInfo->m_ui32BattleTime = ui32BattleTime;
	sCharaInfo->m_ui32SkillRelTime = ui32SkillRelTime;

	MCardUtil::Update_Recv_PROTOCOL_BATTLE_ENDBATTLE_ACK();

	if (MyRoomInfoContext::i()->getStage()->IsAiMode())
		AssistContext::i()->AllResetAssistor();
	
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_END_BATTLE, (INT32)ui8Win);

	if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		if(ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN)
			ClanGameContext::i()->SetMatchEnd(true);
	}
}

void ClientTCPSocket::__Parse_Battle_ChangeHost( i3NetworkPacket * pPacket)
{
	if( BattleServerContext::i()->IsP2PActivate())		// 내가 게임중이면 
	{
		INT32 MainSlotBuffer; 
		pPacket->ReadData( &MainSlotBuffer,	sizeof(INT32) );
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_HOSTLOST_BATTLE, MainSlotBuffer );
	}
}

void ClientTCPSocket::__Parse_Battle_TimeSync( i3NetworkPacket * pPacket)
{
	if(!AbuseContext::i()->IsAbusing())
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_INVALID_TIMESYNC );
}


void ClientTCPSocket::__Parse_Battle_HoleCheck( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp, sizeof( INT32 ) );	//시작한 유저..
}

void ClientTCPSocket::__Parse_Battle_Timeout( i3NetworkPacket * pPacket)
{	
#if !defined( I3_DEBUG) && !defined( BUILD_RELEASE_QA_VERSION)	// 디버그 모드, QA 빌드 에서 한 컴퓨터에서 2개의 클라를 돌리기 위해 처리하지 않는다.
	if( BattleServerContext::i()->IsP2PHost() )
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TIMEOUT_HOST );
	else
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TIMEOUT_CLIENT );
#endif
}


void ClientTCPSocket::__Parse_Battle_SendPing( i3NetworkPacket * pPacket)
{
	if( !BattleServerContext::i()->IsP2PHost() )
	{
		UINT8 Ping[ SLOT_MAX_COUNT];
		pPacket->ReadData( Ping, sizeof( Ping ));
		BattleServerContext::i()->SetPingInfo()->SetPings( Ping, sizeof( Ping));
	}
}

namespace
{
	void GoNextRound(UINT32 newStageID)
	{
		I3TRACE("[DOMI] JUMP STAGE NEXT ROUND \n");

		gstatemgr::i()->setStageState(GAMESTAGE_STATE_DOMI_NEW_FLOW);
		if (g_pFramework->MoveNextStage(newStageID))
		{
			BattleServerContext::i()->InitRoundData();
			ROOM_INFO_BASIC room; MyRoomInfoContext::i()->getMyRoomInfo(&room);
			
			
			room._StageID = newStageID;
			
			
			MyRoomInfoContext::i()->setMyRoomInfo(&room);
		}
		else // 스테이지 점프가 실패 (서버에서 보내준 id가 잘못된 값일 경우) 하면 배틀 종료
		{
			INT32 arg = 0;
			GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &arg);
			CGameFramework::JumpToReady();
		}
	}
}


void ClientTCPSocket::__Parse_Battle_Round_Prestart( i3NetworkPacket * pPacket)
{
	UINT16 UserPlayFlag;

	// 초기화 코드 순서 중요.. by dfly79
	BattleServerContext::i()->InitRoundData();
	//g_pWeaponManager->PreStartRound();
	CGameMissionManager::i()->ProcessMissionPreStart();

	pPacket->ReadData( &UserPlayFlag, sizeof( UINT16 ) );
	UserContext::i()->PlayingSlotFlag = UserPlayFlag;
	
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
	{
		DinoModeContext::i()->eEscapeBlock	= TOUCHDOWN_BLOCK_STATE_INIT;
		DinoModeContext::i()->rDelayTime		= 0.0f;
	}

	pPacket->ReadData( &DinoModeContext::i()->DinoSlot, sizeof( DINO_ROOM_INFO ) );
	
	UINT8 ui8Change;
	pPacket->ReadData( &ui8Change, sizeof( UINT8 ) );

	/*
		폭파미션, 3라운드 자동공수교대경우
		0  -                        -
        1  -                        -
        2  ATK_DEF_CHANGED    ATK_DEF_CHANGING   공수교대라운드
        3  ATK_DEF_CHANGED          -
        4  ATK_DEF_CHANGED          -
	*/

	// 매라운드마다 공수교대전환여부가 이값에 의해 결정됩니다.
	if (ui8Change & ATK_DEF_CHANGED)  
	{
		// 현재 라운드가 첫 공수교대 라운드인지 판단.
		if (ui8Change & ATK_DEF_CHANGING)
		{
			// 공수전환으로 변경된 캐릭터로 변경해줍니다.
			T_ItemID chara_item_id[SLOT_MAX_COUNT];
			pPacket->ReadData( chara_item_id, sizeof(T_ItemID) * SLOT_MAX_COUNT );

			for ( INT32 idx = 0; idx < SLOT_MAX_COUNT; ++idx )
			{
				NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->setNetCharaInfo(idx);

				ITEM_INFO ItemInfo;
				ItemInfo.m_TItemID = chara_item_id[idx];

				// 자기 슬롯은 DBIdx도 설정합시다.
				if ( idx == BattleSlotContext::i()->getMySlotIdx() )
				{
					// 배틀 진입 시 레드팀 -> 블루팀의 메인 캐릭터의 DBIdx를 가져옴.
					// 배틀 진입 시 블루팀 -> 레드팀의 메인 캐릭터의 DBIdx를 가져옴.
					TEAM_TYPE team = TEAM_RED;
							
					if( CCharaInfoContext::i()->GetTeamSlot() == TEAM_RED)
						team = TEAM_BLUE;

					ItemInfo.m_TItemDBIdx = CCharaInfoContext::i()->GetMainCharaDBIdx(team);
				}

				pCharaInfo->setCharaParts()->SetPartsInfo( CHAR_EQUIPMENT_PARTS_CHARA, &ItemInfo);
			}

			// 공수교대 Effect 출력.
			ScoreContext::i()->ShowTeamAutoChangeHudEffect();
		}

		// 공수 교대 상태 갱신 및 공수 교대 전환이 안된 유저의 RED / BLUE 캐릭터 전환
		ScoreContext::i()->ExecuteAutoTeamChange();
	}


	/*
	UINT16	ui16TeamWinRound;
	pPacket->ReadData( &ui16TeamWinRound,	sizeof(UINT16) );
	ScoreContext::i()->setRedTeamWinRound( ui16TeamWinRound);

	pPacket->ReadData( &ui16TeamWinRound,	sizeof(UINT16) );
	ScoreContext::i()->setBlueTeamWinRound( ui16TeamWinRound);

	최종기획에서 제압모드 스테이지 점프 기능이 제거되었으므로 주석처리

	UINT32 newStageID = 0; 
	pPacket->ReadData(&newStageID, sizeof(UINT32)); // 제압모드 스테이지 점프 때문에 다른 모드에서도 읽음.

#ifdef DOMI_NEW_FLOW
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DOMINATION))
	{
		// 2라운드부터 스테이지 점프
		// 첫 라운드는 로딩한 스테이지와 newStageID가 같으므로 스테이지 점프 안 함.
		// 2라운드부터는 현재 stage와 다음 stage가 반드시 다르므로 스테이지 점프.
		UINT32 currentStageID = static_cast<UINT32>(MyRoomInfoContext::i()->getStageID());
		if (currentStageID != newStageID)
			GoNextRound(newStageID);
	}
#endif
	*/

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_PRE_ROUNDSTART_BATTLE );	
}


void ClientTCPSocket::__Parse_Battle_Round_Start( i3NetworkPacket * pPacket)
{
	UserInfoContext::i()->SetMyRespawnCnt(0);
	UserInfoContext::i()->SetMyTouchDownCnt(0);
	UserInfoContext::i()->SetMyUsurpationIdx(255);

	UINT8	ui8NowRoundCount;
	UINT32	Temp;

	pPacket->ReadData( &ui8NowRoundCount,	sizeof( UINT8 ) );			// 시작하는 라운드 카운트
	pPacket->ReadData( &Temp, sizeof(UINT32));

	UINT16 playingSlot = 0;
	pPacket->ReadData( &playingSlot, sizeof( UINT16 ) );
	UserContext::i()->PlayingSlotFlag = playingSlot;

	UINT8 ui8Change;
	pPacket->ReadData( &ui8Change, sizeof( UINT8 ) );

    // 레디룸에서 난입해서 바로 관전모드로 가는 유저는 __Parse_Battle_Round_Prestart 를 받지못합니다.
    // 그래서 이곳에서 체크를 한번더합니다.
	if(ui8Change & ATK_DEF_CHANGED)
	{
		// 자신이 난입했을 때 이미 공수교대가 진행되있으면 다른팀의 캐릭터로 변경해줍니다.
		{
			INT32 my_slot_idx = BattleSlotContext::i()->getMySlotIdx();

			if ( g_pCharaManager->getCharaTeamChange(my_slot_idx) == false) 
			{
				NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo(my_slot_idx);

				// 배틀 진입 시 레드팀 -> 블루팀의 메인 캐릭터의 DBIdx를 가져옴.
				// 배틀 진입 시 블루팀 -> 레드팀의 메인 캐릭터의 DBIdx를 가져옴.
				TEAM_TYPE team = TEAM_RED;

				if (CCharaInfoContext::i()->GetTeamSlot() == TEAM_RED)
					team = TEAM_BLUE;

				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(team);

				ITEM_INFO ItemInfo;

				ItemInfo.m_TItemID = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
				ItemInfo.m_TItemDBIdx = pPartsInfo->GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA);

				pNetCharaInfo->setCharaParts()->SetPartsInfo( CHAR_EQUIPMENT_PARTS_CHARA, &ItemInfo);
			}
		}

        // 슬롯번호를기준으로 팀이 안바뀐사람만 바뀐다.(난입자등)
		ScoreContext::i()->ExecuteAutoTeamChange();
	}

	// 관전이 없는 모드라면
	if( !CStageInfoUtil::IsObserverModeStageByOnlyMode(*MyRoomInfoContext::i()->getStage()) )
	{
		// 모든 유저가 플레이 중이라고 세팅한다.
		// 그래야 AddChara가 된다.
		UserContext::i()->PlayingSlotFlag = 0xFFFF;
	}

	UserContext::i()->fRemainingBattleTime = (REAL32)Temp;
	if(HUD::instance())
		HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetRunningTime, (INT32)Temp);
	CGameMissionManager::i()->ProcessMissionStart();
	
	ScoreContext::i()->setCurrentDediRound( ui8NowRoundCount);
	ScoreContext::i()->setCurrentRound( ui8NowRoundCount);

	DinoModeContext::i()->CurrentDinoRound	= ui8NowRoundCount;
	DinoModeContext::i()->eEscapeBlock		= TOUCHDOWN_BLOCK_STATE_INIT;
	DinoModeContext::i()->rDelayTime		= 0.0f;

	GrenadeContext::i()->ResetGrenadeCheck();

	for (INT32 i =0; i< OBJ_TOTAL_COUNT;i++)
	{
		BattleObjContext::i()->setGameInfo_Object(i)->_tNetData.Reset();
	}

	// 자신은 관전상태로 플레이중이 아니라면 이벤트 처리를 하지 않는다.
	if( UserContext::i()->IsPlayingSlot( BattleSlotContext::i()->getMySlotIdx()) == false)
	{
		return;
	}

	/*if( ScoreContext::i()->isChangeRound() == TEAM_CHANGE_CHANGING) 
	{
		ScoreContext::i()->setSwapTeamWinRound();
	}*/
	
	MCardUtil::Update_Recv_PROTOCOL_BATTLE_MISSION_ROUND_START_ACK();

	GameEventReceiver::i()->PushReceivedGameEvent(   EVENT_M_ROUNDSTART_BATTLE );
}


void ClientTCPSocket::__Parse_Battle_Round_End( i3NetworkPacket * pPacket)
{
	I3PRINTLOG(I3LOG_QA, "RECV ROUND_END_ACK" );

	UINT8 WinTeam;
	UINT8 Type;
	INT32 Temp;
	UINT16 numRedWin, numBlueWin;

	pPacket->ReadData( &WinTeam,	sizeof( UINT8 ) );		//0, 1, 2 <-Red Blue Draw
	pPacket->ReadData( &Type,		sizeof( UINT8 ) );		//MISSION_END_TIMEOUT ....1,2,3,4,//검사할필요없다. //EVENT_ROUNDEND_BATTLE
	pPacket->ReadData( &numRedWin,	sizeof(UINT16) );
	pPacket->ReadData( &numBlueWin,	sizeof(UINT16) );

	ScoreContext::i()->SetSvrTeamWinRound(numRedWin, numBlueWin);

	if(WinTeam > 2 )
	{
        I3PRINTLOG(I3LOG_FATAL, "Error WinTeam PROTOCOL_BATTLE_MISSION_ROUND_END_ACK "); 
		return; 
	}

	I3PRINTLOG(I3LOG_QA, "RECV ROUND_END_ACK 2" );

	Temp = (INT32)(Type) | ((INT32)WinTeam << 8);
	CGameMissionManager::i()->setMissionState( GAME_MISSION_STATE_ENDBATTLE );

	// Test-Automation을 위해 추가 구현된 기능
	// Round의 종료 조건을 파악하기 위한 상태 설정.
	CGameMissionManager::i()->setLastRoundResult((MISSION_END_TYPE)Type, (TEAM_RESULT_TYPE)WinTeam);

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_M_ROUNDEND_BATTLE, Temp );
}


void ClientTCPSocket::__Parse_Battle_Install_Bomb( i3NetworkPacket * pPacket)
{
	UINT32	nUserIdx;
	UINT8	nArea;
	UINT16	nTime; 
	REAL32	rPos[3];

	pPacket->ReadData( &nUserIdx,	sizeof( UINT32 ) );		//설치한 유저 슬롯 인덱스
	pPacket->ReadData( &nArea,		sizeof( UINT8 ) );		//지역 		
	pPacket->ReadData( &nTime,		sizeof( UINT16 ) );		//폭파 타임
	pPacket->ReadData( rPos,		sizeof( REAL32) * 3 );

	if( BattleSlotContext::i()->getMySlotIdx() == (INT32)(nUserIdx) )
		BattleSlotContext::i()->setGameInfo_Chara( nUserIdx )->_tNetData._tMissionData.Reset();	//설치한 사람이 나이면 설치 패킷(UDP) 안보냅니다.
	
	if(nUserIdx  < SLOT_MAX_COUNT)
	{
		CGameMissionManager::i()->ProcessInstalled();				// 설치팀은 이제 설치할 수 없습니다.
		CGameMissionManager::i()->setExplosionTimer( (REAL32)nTime );// 타이머를 세팅			
		GAME_ROOM_OPTION _option;
		BattleSlotContext::i()->getGameRoomOption(&_option);
		if (_option.m_ui8Mode == STAGE_MODE_BOMB)
		{
			CGameMissionManager::i()->setExplosionTimer(_option.m_r16BombExplosionDelayTime);
		}

		CGameMissionManager::i()->setInstalledArea( nArea);			// 설치된 지역을 세팅


		if( BattleSlotContext::i()->getMySlotIdx() == (INT32)(nUserIdx) )
		{
			C4Context::i()->Reset();
		}

		VEC3D vPos;
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( 0);

			i3Vector::Copy( &vPos, pChara->GetPos() );
			//i3Vector::Add( &vPos, 0.f, 0.1f, 0.f );
			rPos[0] = vPos.x;
			rPos[1] = vPos.y;
			rPos[2] = vPos.z;
		}

		C4Context::i()->setInstalledPos( rPos[0], rPos[1], rPos[2]);			// 설치 위치 세팅 2009.06.22 dfly79
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ARMEDBOMB_BATTLE, nUserIdx );		//
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, " * INSTALL BOMB - ERROR SLOT = %d ", nUserIdx);
	}
}



void ClientTCPSocket::__Parse_Battle_Uninstall_Bomb( i3NetworkPacket * pPacket)
{
	INT32		nUserIdx	= -1;

	pPacket->ReadData( &nUserIdx, sizeof(INT32) );

	if( BattleSlotContext::i()->getMySlotIdx() == (INT32)(nUserIdx) )
		BattleSlotContext::i()->setGameInfo_Chara( nUserIdx )->_tNetData._tMissionData.Reset();	//해제한 사람이 나이면 해제 패킷(UDP) 안보냅니다.

	if( BattleSlotContext::i()->getMySlotIdx() == (INT32)(nUserIdx) )
	{
		C4Context::i()->Reset();
	}

	if(nUserIdx  < SLOT_MAX_COUNT)
	{
		CGameMissionManager::i()->ProcessUninstalled();			// 폭탄이 해제되었습니다.
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_DISARMBOMB_BATTLE, nUserIdx );	
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, " UNINSTALL BOMB - ERROR SLOT = %d ", nUserIdx); 
	}
}



void ClientTCPSocket::__Parse_Battle_ReceiveDestructionModeInfo( i3NetworkPacket * pPacket)
{
	if(BattleServerContext::i()->IsP2PActivate())
	{
		DESTRUCTION_INFO info;
		pPacket->ReadData( &info, sizeof( DESTRUCTION_INFO));
		CGameMissionManager::i()->setDestructionInfo( &info);
	}
}

void ClientTCPSocket::__Parse_Battle_ReceiveDefenceModeInfo( i3NetworkPacket * pPacket)
{
	if( BattleServerContext::i()->IsP2PActivate())
	{
		DEFENCE_INFO info;
		pPacket->ReadData( &info, sizeof( DEFENCE_INFO));
		CGameMissionManager::i()->setDefenceInfo( &info);
	}
}

void ClientTCPSocket::__Parse_Battle_ReceiveTouchdownModeInfo( i3NetworkPacket * pPacket)
{
	//Key :EscapeMission
	UINT16	pui16RoundCount[ TEAM_COUNT ];
	INT32	i32SlotIdx;
	INT16	ui16TouchCount;

	pPacket->ReadData( pui16RoundCount,		sizeof(UINT16) * TEAM_COUNT );	
	pPacket->ReadData( &i32SlotIdx,			sizeof(INT32) );
	pPacket->ReadData( &ui16TouchCount,		sizeof(INT16) );

	DinoModeContext::i()->TouchDownSlotIdx = i32SlotIdx;
	DinoModeContext::i()->TouchDownCount	 = ui16TouchCount;

	DinoModeContext::i()->eEscapeBlock	= TOUCHDOWN_BLOCK_STATE_INIT;
	DinoModeContext::i()->rDelayTime		= 0.0f;

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_TOUCHDOWN, EVENT_ERROR_FAIL);
}

void ClientTCPSocket::__Parse_Battle_TourchdownCount( i3NetworkPacket * pPacket)
{
	UINT16	pui16RoundCount[ TEAM_COUNT ];
	pPacket->ReadData( pui16RoundCount,		sizeof(UINT16) * TEAM_COUNT );	
	DinoModeContext::i()->RedTouchDownCount = pui16RoundCount[0];
	DinoModeContext::i()->BlueTouchDownCount = pui16RoundCount[1];
}

void ClientTCPSocket::__Parse_Battle_Record( i3NetworkPacket * pPacket)
{
	SCORE_INFO TempScore[TEAM_COUNT]; 
	pPacket->ReadData( TempScore, sizeof(TempScore));
	
	ScoreContext::i()->SetSvrTeamScore(TempScore[TEAM_RED], TempScore[TEAM_BLUE]);

	SCORE_INFO userScore[ SLOT_MAX_COUNT];
	pPacket->ReadData( userScore,	sizeof(userScore));
	ScoreContext::i()->setIndividualScores( userScore, sizeof(userScore));
}

void ClientTCPSocket::__Parse_Battle_CountdownForStart( i3NetworkPacket * pPacket)
{
	INT8 i8Countdown;
	pPacket->ReadData(&i8Countdown, sizeof(INT8));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_COUNTDOWN, ((INT32)i8Countdown));
}

void ClientTCPSocket::__Parse_Battle_ChangeDifficultyLevelForAI( i3NetworkPacket * pPacket)
{
	INT32 i;
	INT8 difficultyLevel;
	INT32 nLevel[ SLOT_MAX_COUNT ];

	pPacket->ReadData(&difficultyLevel, sizeof(INT8));
	pPacket->ReadData(nLevel,			sizeof(INT32)*SLOT_MAX_COUNT);

	AIModeContext::i()->CurrentLevel = difficultyLevel;

	for( i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		AIModeContext::i()->CurrentAILevel[i] = nLevel[i];
	}
}

void ClientTCPSocket::__Parse_Battle_Tutorial_Round_End( i3NetworkPacket * pPacket)
{
	UINT8	ui8Temp;
	UINT16	RoundPlayTime = 0;

	USER_INFO_BASIC	MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	
	pPacket->ReadData( &ui8Temp, sizeof(UINT8) );
	MyUserInfoBasic.m_ui8TutorialIng = ui8Temp;	
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	pPacket->ReadData( &RoundPlayTime, sizeof(UINT16) );
	TutorialContext::i()->SetRoundPlayTime((UINT32)RoundPlayTime);

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TUTORIAL_ROUND_END);
}

//- 강제 퇴장 ---//
void ClientTCPSocket::__Parse_Battle_SuggestKickVote( i3NetworkPacket * pPacket)
{
	INT32 Result;
	pPacket->ReadData(&Result, sizeof(INT32) );
	
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_KICK_VOTE_SUGGEST_RESULT, Result);
}

void ClientTCPSocket::__Parse_Battle_StartKickVote( i3NetworkPacket * pPacket)
{
	UINT8 app_slot, cand_slot, reason;
	pPacket->ReadData(&app_slot, sizeof(UINT8));
	pPacket->ReadData(&cand_slot, sizeof(UINT8));
	pPacket->ReadData(&reason, sizeof(INT8));

	INT32 pack = AssembleValue(app_slot, cand_slot, reason);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_KICK_VOTE_START, pack);
}

void ClientTCPSocket::__Parse_Battle_VoteKickVote( i3NetworkPacket * pPacket)
{
	INT32 Result;
	pPacket->ReadData(&Result, sizeof(INT32));

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_KICK_VOTE, Result);
}

void ClientTCPSocket::__Parse_Battle_NotifyKickVote( i3NetworkPacket * pPacket)
{
	UINT8 agree, disagree;
	pPacket->ReadData(&agree, sizeof(UINT8));
	pPacket->ReadData(&disagree, sizeof(UINT8));

	INT32 pack =  AssembleValue(agree, disagree);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_KICK_VOTE_COUNT, pack);
}

void ClientTCPSocket::__Parse_Battle_CancelKickVote( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_KICK_VOTE_CANCEL);
}

void ClientTCPSocket::__Parse_Battle_KickVoteResult( i3NetworkPacket * pPacket)
{
	UINT8 slot, agree, disagree;
	INT32 result;
	pPacket->ReadData(&slot, sizeof(UINT8));
	pPacket->ReadData(&agree, sizeof(UINT8));
	pPacket->ReadData(&disagree, sizeof(UINT8));
	pPacket->ReadData(&result, sizeof(INT32));

	UINT8 res2 = BATTLE_FORCIBLYREMOVE_UNKNOWN_ERROR;
	switch(result)
	{	
	case EVENT_ERROR_SUCCESS:								res2 = BATTLE_FORCIBLYREMOVE_SUCCESS;	break;
	case EVENT_ERROR_KICKVOTE_REJECTED_SHORTAGES:			res2 = BATTLE_FORCIBLYREMOVE_SHORTAGES; break;
	case EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ALLY:		res2 = BATTLE_FORCIBLYREMOVE_NOALLY; break;
	case EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ENEMY:	res2 = BATTLE_FORCIBLYREMOVE_NOENEMY; break;
	}
	
	INT32 pack = AssembleValue(slot, agree, disagree, res2);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_KICK_VOTE_RESULT, pack);
}

void ClientTCPSocket::__Parse_Battle_BeKickedByVote( i3NetworkPacket * pPacket)
{
	// 이 패킷은 강퇴자가 받고, 데이터는 없습니다. 1.1 소스엔 이 함수에 대해서 뭔 처리를 한다. 

	// 배틀 급 종료의 경우 퀘스트 완료 취소하는 함수..
	MCardUtil::Update_Process_GiveupBattle();
	//

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_KICK_VOTE_MY_KICKED);
}

void ClientTCPSocket::__Parse_Battle_BlockTouchdown( i3NetworkPacket * pPacket)
{
	UINT8	ui8LastBlockState = TOUCHDOWN_BLOCK_STATE_INIT;		// TOUCHDOWN_BLOCK_STATE
	UINT16	pui16TotalRoundCount[ TEAM_COUNT ];

	pPacket->ReadData( &ui8LastBlockState, sizeof(UINT8) );
	DinoModeContext::i()->eEscapeBlock = TOUCHDOWN_BLOCK_STATE(ui8LastBlockState);
	

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_ESCAPEBLOCK, EVENT_ERROR_FAIL);
	pPacket->ReadData( pui16TotalRoundCount, (sizeof(UINT16) * TEAM_COUNT) );
	DinoModeContext::i()->RedTouchDownCount = pui16TotalRoundCount[0];
	DinoModeContext::i()->BlueTouchDownCount = pui16TotalRoundCount[1];
}

void ClientTCPSocket::__Parse_Battle_NotifyHackingUser( i3NetworkPacket * pPacket)
{


	INT32	slot = 0;
	UINT8	type = 0;
	INT32	number = 0;
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	pPacket->ReadData( &slot, sizeof(INT32) );
	pPacket->ReadData( &type, sizeof(UINT8) );
	pPacket->ReadData( &number, sizeof(INT32) );
	HackContext::i()->setHackUserSlot( slot , (HACK_CHECK_TYPE)type,number);

#else
	pPacket->ReadData( &slot, sizeof(UINT8) );
	pPacket->ReadData( &type, sizeof(UINT8) );
	pPacket->ReadData( &number, sizeof(INT32) );

#endif

}

void ClientTCPSocket::__Parse_Battle_EquipFromBattleShop( i3NetworkPacket * pPacket)
{
	// 인게임 상점에서 아이템(무기/장비/아이템) 구입 후 바로 캐릭터에 적용
	pPacket->ReadData(&IGShopItemController::GetInstance()->m_Slot_ItemInfo_Buf, 
		sizeof(PACKET_H_PROTOCOL_SLOT_EQUIPMENT_ACK));

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_INGAMESHOP_APPLY_ITEM, EVENT_ERROR_FAIL);
}

void ClientTCPSocket::__Parse_Battle_GMPause_Activate( i3NetworkPacket * pPacket)
{
	INT32 num;
	pPacket->ReadData(&num, sizeof(INT32));

	if( EV_SUCCESSED(num) )
	{
		I3PRINTLOG(I3LOG_NOTICE,"GM Pause On-------");
		INT32 GMSlotNum;
		pPacket->ReadData(&GMSlotNum, sizeof(INT32));
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE,"GM Pause Failed-------");
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_GM_PAUSE, num);
}

void ClientTCPSocket::__Parse_Battle_GMPause_Deactivate( i3NetworkPacket * pPacket)
{
	INT32 num;
	pPacket->ReadData(&num, sizeof(INT32));

	if( EV_SUCCESSED(num) )
	{
		I3PRINTLOG(I3LOG_NOTICE,"GM Resume On-------");
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE,"GM Resume Failed-------");
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_GM_RESUME, num);
}

void ClientTCPSocket::__Parse_Battle_LoadInvenInfo( i3NetworkPacket * pPacket)
{
	PACKET_H_BATTLE_LOAD_INVEN_INFO_ACK		head;

	pPacket->ReadData( &head, sizeof( head));

	INT16 itemCount = head.m_i16InvenCount;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( head.m_i32SlotIdx);
	if( pChara != nullptr)
	{
		for( INT32 i = 0; i < itemCount; i++)
		{
			PACKET_D_BATTLE_LOAD_INVEN_INFO info;
			pPacket->ReadData( &info, sizeof( info));

			if( WeaponLRU_Manager::i()->IsPriorityWeapon( info.m_TItemID))
			{
				g_pWeaponManager->PushLoadWeapon( info.m_TItemID);
				pChara->PreloadAnimationByWeapon( info.m_TItemID);
			}
		}
	}
}

void ClientTCPSocket::__Parse_Battle_RoomScore( i3NetworkPacket * pPacket)
{
	PACKET_BATTLE_NEW_JOIN_ROOM_SCORE_ACK param;
	pPacket->ReadData( &param, sizeof( param) );

	MyRoomInfoContext::i()->setMyRoomScore(param.m_RoomNowScore);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_ROOM_SCORE);
}

void ClientTCPSocket::__Parse_Battle_Domination_UserSkillUpgrade( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_USER_SKILL_UPGRADE_ACK buf;
	pPacket->ReadData(&buf,	sizeof(PACKET_DOMINATION_USER_SKILL_UPGRADE_ACK));

	// 스킬 업그레이드 불가 처리
	if (buf._i8Level < 0)
	{
		const char* message[5] = {"STR_DOMI_OBJECT_9", "NO LEVEL TO UPGRADE", "ROUND NOT ENOUGH", "NOT INTERMISSION", "NOT NEARBY SKILLSHOP"}; 
		INT32 idx = ::abs(buf._i8Level) - 1;
		g_pFramework->PutSystemChatting(GAME_RCSTRING(message[idx]));
		return;
	}

	if (buf._i8Level >= 0)
		DomiUserSkillMgr::i()->ApplyDirect(buf._i8UserIdx, static_cast<USER_SKILL>(buf._i8SkillIdx), buf._i8Level);

	if (UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP))
		static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP))->RefreshPage();
}

void ClientTCPSocket::__Parse_Battle_Domination_WavePreStart( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_WAVE_PRE_START_ACK i;
	pPacket->ReadData(&i, sizeof(PACKET_DOMINATION_WAVE_PRE_START_ACK));

	domi_state_info::i()->round = i._i16Round;
	domi_state_info::i()->max_wave = i._i8MaxWave;
	domi_state_info::i()->wave_count = i._i8Wave;
	domi_state_info::i()->max_dino = i._i16TotalDinoCount;
	domi_state_info::i()->intermission_time = INTERMISSION_WAIT_SECOND;
	domi_state_info::i()->inIntermission = true;

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_WAVE_PRE_START, static_cast<INT32>(i._ui16PlaySlot));
}

void ClientTCPSocket::__Parse_Battle_Domination_WaveInfo( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_WAVE_INFO_ACK i;
	pPacket->ReadData(&i, sizeof(PACKET_DOMINATION_WAVE_INFO_ACK));

	domi_state_info::i()->round = i._i16Round;
	domi_state_info::i()->max_wave = i._i8MaxWave;
	domi_state_info::i()->wave_count = i._i8Wave;
	domi_state_info::i()->max_dino = i._i16TotalDinoCount;
	domi_state_info::i()->inIntermission = i._bInter;
	domi_state_info::i()->intermission_time = static_cast<REAL32>(i._i8RemainInterSec);

	UIHUDDominationMgr::i()->EnableHUD(HUD_DOMI_WAVE_RESULT, false);
	UIHUDDominationMgr::i()->EnableHUD(HUD_DOMI_ROUND_RESULT, false);
	UIBattleFrame::i()->ClosePopup(UBP_SKILL_SHOP);

	// 라운드 시작
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_ROUND);

	if (domi_state_info::i()->inIntermission)
	{
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_INTERMISSION);


		{//인터미션 중 자신이 난입한 경우 등급 표시.
			domi_state_info* info = domi_state_info::i();
			info->grade_state = DOMI_REWARD_GRADE_STATE_USER_ENGER;
			info->grade_user_nick = BattleSlotContext::i()->getNickForSlot( BattleSlotContext::i()->getMySlotIdx());
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_GRADE);
		}
	}

	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);
}

void ClientTCPSocket::__Parse_Battle_Domination_WaveReadyInfo( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_WAVE_READY_INFO_ACK i;
	pPacket->ReadData(&i, sizeof(PACKET_DOMINATION_WAVE_READY_INFO_ACK));

	domi_state_info* info = domi_state_info::i();
	memcpy( info->intermission_ready, i._bPress, sizeof(bool)*MAX_COUNT_DOMI_USER_REAL);

	UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::intermission);
}

void ClientTCPSocket::__Parse_Battle_Domination_WaveStart( i3NetworkPacket * pPacket)
{
	tinst<domi_state_info>()->is_wave_end = false;
	domi_state_info::i()->inIntermission = false;
	UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_COUNTER);
}

void ClientTCPSocket::__Parse_Battle_Domination_WaveEnd( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_WAVE_END_ACK base;
	pPacket->ReadData(&base, sizeof(PACKET_DOMINATION_WAVE_END_ACK));

	if (base._bFinal)
	{
		PACKET_DOMINATION_WAVE_END_ADD_ACK additional;
		pPacket->ReadData(&additional, sizeof(PACKET_DOMINATION_WAVE_END_ADD_ACK));

		for (INT32 i=0; i<MAX_COUNT_DOMI_USER_REAL; i++)
		{
			DomiDefs::ROUND_RESULT_INFO& info = DomiContext::i()->SetRoundResult(i);

			info.base.kill = base._User[i]._i16KillCount;
			info.base.score = base._User[i]._i32Score;
			info.slotIdx = i*2; // 0,1,2,3 -> 0,2,4,6
			info.icon = additional._UserAdd[i]._ui8ResultIcon; 
			info.exp = additional._UserAdd[i]._i16Exp; 
			info.point = additional._UserAdd[i]._i16Point; 
		}
	}
	else
	{
		for (INT32 i=0; i<MAX_COUNT_DOMI_USER_REAL; i++)
		{
			DomiDefs::WAVE_RESULT_INFO& info = DomiContext::i()->SetWaveResult(i);

			info.base.kill = base._User[i]._i16KillCount;
			info.base.score = base._User[i]._i32Score;
			info.slotIdx = i*2; // 0,1,2,3 -> 0,2,4,6
		}
	}

	tinst<domi_state_info>()->is_wave_end = true;

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_WAVE_END, base._bFinal);
}

void ClientTCPSocket::__Parse_Battle_Domination_PrintMessage( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_PRINT_MESSAGE_ACK m;
	pPacket->ReadData(&m, sizeof(PACKET_DOMINATION_PRINT_MESSAGE_ACK));
	
	// TODO : 서버 협업 후 변경 가능성 높음 .(2014.07.14.수빈)
	i3::rc_wstring wstrMessage;		i3::mb_to_utf16(m._szMessage, wstrMessage);

	g_pFramework->PutSystemChatting( wstrMessage );
}

void ClientTCPSocket::__Parse_Battle_Domination_FinalScore( i3NetworkPacket * pPacket)
{
	pPacket->ReadData( &Domination_Util::g_current_score, sizeof(INT32) );	// 방금 획득한 제압모드 스코어

	if( Domination_Util::g_current_score > Domination_Util::g_best_score)
	{
		Domination_Util::g_renewal_best = true;
		Domination_Util::g_best_score = Domination_Util::g_current_score;
	}
}

void ClientTCPSocket::__Parse_Battle_Domination_AppearAcidItem( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_APPEAR_ACIDITEM_ACK a;
	pPacket->ReadData(&a, sizeof(PACKET_DOMINATION_APPEAR_ACIDITEM_ACK));

	domi_state_info::i()->etc_time = (REAL32)a._i16Time;
	domi_state_info::i()->etc_count = a._i8Count;

	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_ETC);
}

void ClientTCPSocket::__Parse_Battle_Domination_MutantTrexHowlingSlow( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_MUTANTTREX_HOWLING_SLOW_ACK data;
	pPacket->ReadData(&data, sizeof(PACKET_DOMINATION_MUTANTTREX_HOWLING_SLOW_ACK));

	REAL32 duration = static_cast<REAL32>(data._i8SlowTime);

	for (INT32 i=0; i<g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase* target = g_pCharaManager->getChara(i);
		if (target == nullptr)	continue;
		if (target->getCharaNetIndex() != BattleSlotContext::i()->getMySlotIdx()) continue;
		if (target->isCharaStateMask(CHARA_STATE_DEATH)) continue;
		if (target->IsDino()) continue;

		target->getHUDContext()->ScreenEffect_Howl(duration);
		target->getMoveContext()->EnableSpeedReduction(0.5f, duration);
	}
}

void ClientTCPSocket::__Parse_Battle_Domination_NaviMesh_or_AICollision( i3NetworkPacket * pPacket)
{
	PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK data;
	pPacket->ReadData(&data, sizeof(PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK));

	du::CNaviMesh* pNaviMesh = gmode::i()->GetNaviMesh();
	if( pNaviMesh)
	{
		pNaviMesh->SetNaviMeshAICollision( &data );
	}
}

void ClientTCPSocket::__Parse_Battle_ScopeType(i3NetworkPacket * pPacket)
{
	UINT32 slotIdx;
	UINT8 scopeType;

	pPacket->ReadData(&slotIdx, sizeof(UINT32));
	pPacket->ReadData(&scopeType, sizeof(UINT8));

	CrosshairMgr::i()->setUserScopeTypes(slotIdx, scopeType);

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(slotIdx);
	if (pChara != NULL)
		pChara->Cmd_ApplyCrosshair();
}