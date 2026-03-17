#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"
#include "ClanDef.h"
#include "GameServerContext.h"
#include "ModuleConnectServer.h"
#include "ModuleDataServer.h"

I3_CLASS_INSTANCE(GameSession);

GameSession::GameSession()
	: m_eMainTask(GAME_TASK_NONE)
	, m_i64UID(0)
	, m_ui32EncriptKey(0)
	, m_ui32AuthToken(0)
	, m_i32ChannelNum(-1)
	, m_i32RoomIdx(-1)
	, m_i32SlotIdx(-1)
	, m_pRoom(nullptr)
	, m_i32Level(1)
	, m_i64Exp(0)
	, m_i32Cash(0)
	, m_i32GP(0)
	, m_i32RankId(0)
	, m_i32ClanId(0)
	, m_i32Kills(0)
	, m_i32Deaths(0)
	, m_i32Headshots(0)
	, m_i32Wins(0)
	, m_i32Losses(0)
	, m_stUsedWeapon(0)
	, m_ui8ActiveCharaSlot(0)
	, m_i32InventoryCount(0)
	, m_i32FriendCount(0)
	, m_i32BlockCount(0)
	, m_dwConnectTime(0)
	, m_dwLastPacketTime(0)
	, m_dwRateLimitWindow(0)
	, m_ui16PacketCount(0)
	, m_i32ReplayIdx(0)
	, m_i32ReplayDetections(0)
{
	memset(m_ReplayHistory, 0, sizeof(m_ReplayHistory));
	m_szUsername[0] = '\0';
	m_szNickname[0] = '\0';
	for (int i = 0; i < MAX_CHARA_SLOT; i++)
		m_CharaSlots[i].Reset();
	memset(m_Inventory, 0, sizeof(m_Inventory));
	m_QuestData.Reset();
	for (int i = 0; i < MAX_FRIEND_COUNT; i++)
		m_Friends[i].Reset();
	for (int i = 0; i < MAX_BLOCK_COUNT; i++)
		m_BlockList[i].Reset();
	m_MedalData.Reset();
	m_AttendanceData.Reset();
	m_DailyRecord.Reset();
	m_SkillData.Reset();
	m_TitleData.Reset();
}

GameSession::~GameSession()
{
}

BOOL GameSession::Create()
{
	ResetSessionData();
	return TRUE;
}

void GameSession::Destroy()
{
	ResetSessionData();
}

BOOL GameSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	m_eMainTask = GAME_TASK_CONNECT;
	m_dwConnectTime = GetTickCount();
	m_dwLastPacketTime = m_dwConnectTime;

	printf("[GameSession] Client connected - Index=%d\n", GetIndex());
	return TRUE;
}

BOOL GameSession::OnConnectInit()
{
	return TRUE;
}

BOOL GameSession::OnDisconnect(BOOL bForceMainThread)
{
	printf("[GameSession] Client disconnected - Index=%d, UID=%lld, Task=%d\n",
		GetIndex(), m_i64UID, m_eMainTask);

	// Notify friends we went offline (Phase 7A)
	if (m_i32FriendCount > 0 && m_eMainTask >= GAME_TASK_CHANNEL)
		NotifyFriendsStatusChange(0);	// 0 = offline

	if (m_pRoom && m_i32RoomIdx >= 0 && g_pRoomManager)
		g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);

	// Decrement channel user count
	if (m_i32ChannelNum >= 0 && m_i32ChannelNum < MAX_GAME_CHANNELS && g_pContextMain)
	{
		GameChannelInfo& ch = g_pContextMain->GetChannelInfoMut(m_i32ChannelNum);
		if (ch.ui16CurrentUsers > 0)
			ch.ui16CurrentUsers--;
	}

	ResetSessionData();
	return TRUE;
}

// ============================================================================
// Packet Router - dispatches to handlers in GameSessionChannel/Room/Battle.cpp
// ============================================================================

INT32 GameSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return 0;

	uint16_t packetSize = *(uint16_t*)pPacket;

	// Track packet count for metrics
	if (g_pContextMain)
		g_pContextMain->IncrementPacketsIn();

	// Phase 9A: Packet encryption validation
	// Bit 0x8000 in size field indicates the packet is encrypted
	bool bEncrypted = (packetSize & 0x8000) != 0;
	if (bEncrypted)
	{
		packetSize &= 0x7FFF;	// Clear encryption flag to get actual size

		if (m_ui32EncriptKey > 0)
		{
			// Decrypt: skip first 2 bytes (size field), decrypt protocol+payload
			// PACKETENCRIPTSIZE = 2 (size field excluded from encryption)
			uint32_t decryptLen = packetSize;	// size includes header, encrypt covers [2..size)
			if (decryptLen > 2)
				BitRotateDecript((UINT8*)&pPacket[2], decryptLen - 2, m_ui32EncriptKey);
		}

		// Write back the decrypted size (without 0x8000 flag)
		*(uint16_t*)pPacket = packetSize;
	}

	uint16_t protocolId = *(uint16_t*)(pPacket + 2);

	if (iSize < (int)packetSize)
		return 0;

	// Phase 9A: Rate limiting - max 100 packets per second
	DWORD dwNow = GetTickCount();
	if (dwNow - m_dwRateLimitWindow > 1000)
	{
		m_dwRateLimitWindow = dwNow;
		m_ui16PacketCount = 0;
	}
	m_ui16PacketCount++;
	if (m_ui16PacketCount > 100)
	{
		// Rate limit exceeded - drop packet (except heartbeat)
		if (protocolId != PROTOCOL_BASE_HEART_BIT_REQ)
			return packetSize;
	}

	// Phase 9A: Replay detection - detect identical packets within short window
	// Skip heartbeat and high-frequency protocols
	if (protocolId != PROTOCOL_BASE_HEART_BIT_REQ)
	{
		// Simple hash of first 8 bytes of payload
		uint32_t dataHash = 0;
		char* pPayload = pPacket + 4;	// Skip size(2) + proto(2)
		int payloadLen = (int)packetSize - 4;
		if (payloadLen > 0)
		{
			int hashLen = (payloadLen > 8) ? 8 : payloadLen;
			for (int i = 0; i < hashLen; i++)
				dataHash = dataHash * 31 + (uint8_t)pPayload[i];
		}

		// Check if this exact packet was seen recently (within 100ms)
		for (int i = 0; i < REPLAY_HISTORY_SIZE; i++)
		{
			if (m_ReplayHistory[i].ui16Protocol == protocolId &&
				m_ReplayHistory[i].ui32DataHash == dataHash &&
				(dwNow - m_ReplayHistory[i].dwTime) < 100)
			{
				// Duplicate detected - drop packet
				m_i32ReplayDetections++;
				if (m_i32ReplayDetections > 50)
				{
					printf("[GameSession] Replay flood detected - UID=%lld, Proto=0x%04X, Count=%d\n",
						m_i64UID, protocolId, m_i32ReplayDetections);
				}
				return packetSize;
			}
		}

		// Record this packet in history
		m_ReplayHistory[m_i32ReplayIdx].ui16Protocol = protocolId;
		m_ReplayHistory[m_i32ReplayIdx].dwTime = dwNow;
		m_ReplayHistory[m_i32ReplayIdx].ui32DataHash = dataHash;
		m_i32ReplayIdx = (m_i32ReplayIdx + 1) % REPLAY_HISTORY_SIZE;
	}

	// Phase 9A: State validation - reject packets from wrong states
	// Always allow: connect, heartbeat, login
	if (protocolId != PROTOCOL_BASE_CONNECT_REQ &&
		protocolId != PROTOCOL_BASE_HEART_BIT_REQ &&
		protocolId != PROTOCOL_LOGIN_REQ &&
		protocolId != PROTOCOL_BASE_GAMEGUARD_REQ)
	{
		// Must be at least logged in for most packets
		if (m_eMainTask < GAME_TASK_LOGIN)
			return packetSize;

		// Battle packets require GAME_TASK_BATTLE or GAME_TASK_READY_ROOM
		if (protocolId >= PROTOCOL_BATTLE_READYBATTLE_REQ &&
			protocolId <= PROTOCOL_BATTLE_SENDPING_ACK)
		{
			if (m_eMainTask != GAME_TASK_BATTLE && m_eMainTask != GAME_TASK_READY_ROOM)
				return packetSize;
		}

		// Room packets require at least GAME_TASK_LOBBY
		if (protocolId >= PROTOCOL_ROOM_CREATE_REQ &&
			protocolId <= PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK)
		{
			if (m_eMainTask < GAME_TASK_LOBBY)
				return packetSize;
		}

		// Lobby packets require GAME_TASK_LOBBY or higher
		if (protocolId >= PROTOCOL_LOBBY_ENTER_REQ &&
			protocolId <= PROTOCOL_LOBBY_FIND_NICK_NAME_ACK)
		{
			if (m_eMainTask < GAME_TASK_CHANNEL)
				return packetSize;
		}

		// Shop packets require at least GAME_TASK_CHANNEL
		if (protocolId >= PROTOCOL_SHOP_ENTER_REQ &&
			protocolId <= PROTOCOL_AUTH_GET_POINT_CASH_ACK)
		{
			if (m_eMainTask < GAME_TASK_CHANNEL)
				return packetSize;
		}

		// Clan packets require at least GAME_TASK_CHANNEL
		if (protocolId >= PROTOCOL_CS_CREATE_CLAN_REQ &&
			protocolId <= PROTOCOL_CS_CLIENT_CLAN_LIST_ACK)
		{
			if (m_eMainTask < GAME_TASK_CHANNEL)
				return packetSize;
		}
	}

	m_dwLastPacketTime = dwNow;

	char* pData = pPacket + SOCKET_HEAD_SIZE;
	INT32 dataSize = packetSize - SOCKET_HEAD_SIZE;

	switch (protocolId)
	{
	// ---- Base protocols ----
	case PROTOCOL_BASE_CONNECT_REQ:		OnBaseConnectReq(pData, dataSize);		break;
	case PROTOCOL_BASE_HEART_BIT_REQ:	OnBaseHeartBitReq(pData, dataSize);		break;
	case PROTOCOL_LOGIN_REQ:			OnLoginReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GET_USER_INFO_REQ:	OnGetUserInfoReq(pData, dataSize);	break;

	// ---- System/Login Flow (Phase 4A) ----
	case PROTOCOL_BASE_GET_SYSTEM_INFO_REQ:	OnGetSystemInfoReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_OPTION_REQ:		OnGetOptionReq(pData, dataSize);		break;
	case PROTOCOL_BASE_OPTION_SAVE_REQ:		OnOptionSaveReq(pData, dataSize);		break;
	case PROTOCOL_BASE_CHECK_NICK_NAME_REQ:	OnCheckNickNameReq(pData, dataSize);	break;
	case PROTOCOL_BASE_CREATE_NICK_REQ:		OnCreateNickReq(pData, dataSize);		break;
	case PROTOCOL_BASE_RANK_UP_REQ:			OnRankUpReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GAMEGUARD_REQ:		OnGameGuardReq(pData, dataSize);		break;
	case PROTOCOL_BASE_LOGOUT_REQ:			OnLogoutReq(pData, dataSize);			break;
	case PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ:	OnQuestBuyCardSetReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GUIDE_COMPLETE_REQ:	OnGuideCompleteReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_USER_INFO_UID_REQ:	OnGetUserInfoByUIDReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_USER_INFO_NICK_REQ:	OnGetUserInfoByNickReq(pData, dataSize);break;
	case PROTOCOL_BASE_NEW_SPECIALITY_SET_REQ:	OnNewSpecialitySetReq(pData, dataSize);	break;
	case PROTOCOL_BASE_NEW_SPECIALITY_INIT_REQ:	OnNewSpecialityInitReq(pData, dataSize);break;
	case PROTOCOL_BASE_NEW_SPECIALITY_SET_CLASS_REQ:	OnNewSpecialitySetClassReq(pData, dataSize);break;
	case PROTOCOL_BASE_QUEST_DELETE_CARD_SET_REQ:	OnQuestDeleteCardSetReq(pData, dataSize);break;
	case PROTOCOL_BASE_GET_RECORD_INFO_SEASON_REQ:	OnGetRecordInfoSeasonReq(pData, dataSize);break;
	case PROTOCOL_BASE_GET_RECORD_INFO_SESSION_REQ:	OnGetRecordInfoSessionReq(pData, dataSize);break;

	// ---- User Detail Info (Phase 4B) ----
	case PROTOCOL_BASE_GET_USER_INFO_LOBBY_REQ:	OnGetUserInfoLobbyReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_USER_INFO_ROOM_REQ:	OnGetUserInfoRoomReq(pData, dataSize);	break;

	// ---- User Info (Phase 4C) ----
	case PROTOCOL_BASE_GET_MYINFO_RECORD_REQ:	OnGetMyInfoRecordReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_MYINFO_BASIC_REQ:	OnGetMyInfoBasicReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_MYINFO_ALL_REQ:		OnGetMyInfoAllReq(pData, dataSize);		break;
	case PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ:	OnGetRecordInfoDBReq(pData, dataSize);	break;

	// ---- Title (Phase 4D) ----
	case PROTOCOL_BASE_USER_TITLE_EQUIP_REQ:	OnTitleEquipReq(pData, dataSize);		break;
	case PROTOCOL_BASE_USER_TITLE_RELEASE_REQ:	OnTitleReleaseReq(pData, dataSize);		break;
	case PROTOCOL_BASE_USER_TITLE_CHANGE_REQ:	OnTitleChangeReq(pData, dataSize);		break;

	// ---- Map/Stage Data (Phase 4B, GameSessionChannel.cpp) ----
	case PROTOCOL_BASE_MAP_VERSION_REQ:		OnMapVersionReq(pData, dataSize);		break;
	case PROTOCOL_BASE_MAP_LIST_REQ:		OnMapListReq(pData, dataSize);			break;
	case PROTOCOL_BASE_MAP_RULELIST_REQ:	OnMapRuleListReq(pData, dataSize);		break;
	case PROTOCOL_BASE_MAP_MATCHINGLIST_REQ:OnMapMatchingListReq(pData, dataSize);	break;
	case PROTOCOL_BASE_MAP_RANDOM_LIST_ACK:	OnMapRandomListReq(pData, dataSize);	break;	// Client can request refresh

	// ---- Channel (GameSessionChannel.cpp) ----
	case PROTOCOL_BASE_GET_CHANNELLIST_REQ:	OnChannelListReq(pData, dataSize);	break;
	case PROTOCOL_BASE_SELECT_CHANNEL_REQ:	OnChannelEnterReq(pData, dataSize);	break;

	// ---- Lobby (GameSessionChannel.cpp) ----
	case PROTOCOL_LOBBY_ENTER_REQ:			OnLobbyEnterReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_LEAVE_REQ:			OnLobbyLeaveReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ:	OnGetRoomListReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ:		OnQuickJoinRoomReq(pData, dataSize);		break;
	case PROTOCOL_QUICKJOIN_START_REQ:			OnQuickJoinStartReq(pData, dataSize);		break;
	case PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ:	OnQuickJoinEnter2ndBestReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_VIEW_USER_ITEM_REQ:		OnLobbyViewUserItemReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_FIND_NICK_NAME_REQ:		OnLobbyFindNickNameReq(pData, dataSize);	break;
	case PROTOCOL_BASE_MEGAPHONE_REQ:			OnMegaphoneReq(pData, dataSize);			break;
	case PROTOCOL_LOBBY_GET_ROOMINFO_REQ:		OnLobbyGetRoomInfoReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_GET_ROOMINFOADD_REQ:	OnLobbyGetRoomInfoAddReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_CREATE_TRAINING_REQ:	OnLobbyCreateTrainingReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_QUICKJOIN_SETTING_REQ:	OnLobbyQuickJoinSettingReq(pData, dataSize);break;
	case PROTOCOL_LOBBY_SET_BIRTHDAY_REQ:		OnLobbySetBirthdayReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_REQ:	OnLobbyGetUIDByNickNameReq(pData, dataSize);break;
	case PROTOCOL_BASE_CHATTING_REQ:
		if (m_eMainTask == GAME_TASK_READY_ROOM || m_eMainTask == GAME_TASK_BATTLE)
			OnRoomChatReq(pData, dataSize);
		else
			OnLobbyChatReq(pData, dataSize);
		break;

	// ---- Room (GameSessionRoom.cpp) ----
	case PROTOCOL_ROOM_CREATE_REQ:				OnRoomCreateReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_JOIN_REQ:				OnRoomJoinReq(pData, dataSize);				break;
	case PROTOCOL_ROOM_LEAVE_ROOM_REQ:			OnRoomLeaveReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_GET_SLOTINFO_REQ:
	case PROTOCOL_ROOM_GET_SLOTONEINFO_REQ:		OnRoomGetSlotInfoReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_PLAYERINFO_REQ:		OnRoomGetPlayerInfoReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_CHANGE_ROOMINFO_REQ:		OnRoomChangeRoomInfoReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_CHANGE_PASSWD_REQ:		OnRoomChangePasswdReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_CHANGE_SLOT_REQ:			OnRoomChangeSlotReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_REQ:	OnRoomTeamChangeReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_REQUEST_MAIN_REQ:		OnRoomRequestMainReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_REQ:	OnRoomRequestMainChangeReq(pData, dataSize);break;
	case PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_REQ:	OnRoomChangeOptionInfoReq(pData, dataSize);break;
	case PROTOCOL_ROOM_INVITE_LOBBY_USER_REQ:		OnRoomInviteLobbyUserReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_REQ:	OnRoomChangeObserverSlotReq(pData, dataSize);break;
	case PROTOCOL_ROOM_LOADING_START_REQ:			OnRoomLoadingStartReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_INFO_ENTER_REQ:				OnRoomInfoEnterReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_INFO_LEAVE_REQ:				OnRoomInfoLeaveReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_LOBBY_USER_LIST_REQ:		OnRoomGetLobbyUserListReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_CHECK_MAIN_REQ:				OnRoomCheckMainReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_MASTER_TEAM_CHANGE_REQ:		OnRoomMasterTeamChangeReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_GET_NICKNAME_REQ:			OnRoomGetNicknameReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_RANK_REQ:				OnRoomGetRankReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_REQ:		OnRoomChangeRoomAllInfoReq(pData, dataSize);break;
	case PROTOCOL_ROOM_GET_COLOR_NICK_REQ:			OnRoomGetColorNickReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_REQ:	OnRoomGetColorMuzzleFlashReq(pData, dataSize);break;
	case PROTOCOL_ROOM_CHARA_SHIFT_POS_REQ:			OnRoomCharaShiftPosReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_REQ:	OnRoomEquipmentTempWeaponReq(pData, dataSize);break;
	case PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_REQ:	OnRoomInviteLobbyUserListReq(pData, dataSize);break;
	case PROTOCOL_ROOM_TS_EVENT_REQ:				OnRoomTsEventReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_REQ:	OnRoomRequestMainChangeWhoReq(pData, dataSize);break;

	// ---- Equipment (GameSessionEquipment.cpp) ----
	case PROTOCOL_BASE_GET_EQUIPMENT_INFO_REQ:		OnGetEquipmentInfoReq(pData, dataSize);		break;
	case PROTOCOL_BASE_EQUIPMENT_REQ:				OnEquipmentReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GET_CHARA_INFO_REQ:			OnGetCharaInfoReq(pData, dataSize);			break;
	case PROTOCOL_CHAR_CREATE_CHARA_REQ:			OnCharaCreateReq(pData, dataSize);			break;
	case PROTOCOL_CHAR_DELETE_CHARA_REQ:			OnCharaDeleteReq(pData, dataSize);			break;
	case PROTOCOL_CHAR_CHANGE_CHARA_NICK_REQ:		OnCharaChangeNickReq(pData, dataSize);		break;
	case PROTOCOL_CHAR_CHANGE_EQUIP_REQ:			OnCharaChangeEquipReq(pData, dataSize);		break;
	case PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_REQ:		OnCharaShiftPosReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ:		OnRoomGetUserEquipmentReq(pData, dataSize);	break;

	// ---- Inventory (GameSessionInventory.cpp) ----
	case PROTOCOL_BASE_GET_INVEN_INFO_REQ:			OnGetInvenInfoReq(pData, dataSize);			break;
	case PROTOCOL_INVENTORY_ENTER_REQ:				OnInventoryEnterReq(pData, dataSize);		break;
	case PROTOCOL_INVENTORY_LEAVE_REQ:				OnInventoryLeaveReq(pData, dataSize);		break;
	case PROTOCOL_INVENTORY_GET_INFO_REQ:			OnInventoryGetInfoReq(pData, dataSize);		break;

	// ---- Battle (GameSessionBattle.cpp) ----
	case PROTOCOL_BATTLE_READYBATTLE_REQ:				OnBattleReadyBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_REQ:			OnBattlePreStartBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_STARTBATTLE_REQ:				OnBattleStartBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_GIVEUPBATTLE_REQ:				OnBattleGiveUpBattleReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_DEATH_REQ:						OnBattleDeathReq(pData, dataSize);					break;
	case PROTOCOL_BATTLE_RESPAWN_REQ:					OnBattleRespawnReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_REQ:	OnBattleMissionRoundPreStartReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_MISSION_ROUND_START_REQ:		OnBattleMissionRoundStartReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_ROUND_END_REQ:			OnBattleMissionRoundEndReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ:		OnBattleMissionBombInstallReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_REQ:	OnBattleMissionBombUninstallReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_MISSION_GENERATOR_DESTROY_REQ:	OnBattleMissionGeneratorDestroyReq(pData, dataSize);break;
	case PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_REQ:	OnBattleMissionGeneratorInfoReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_MISSION_TOUCHDOWN_REQ:			OnBattleMissionTouchdownReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ:		OnBattleMissionDefenceInfoReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_EQUIPMENT_INFO_REQ:			OnBattleEquipmentInfoReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_PICKUP_WEAPON_REQ:				OnBattlePickupWeaponReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_TIMERSYNC_REQ:					OnBattleTimerSyncReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ:				OnBattleTimeoutClientReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_LEAVEP2PSERVER_REQ:			OnBattleLeaveP2PServerReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_USE_ITEM_REQ:					OnBattleUseItemReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_ENDBATTLE_REQ:					OnBattleEndBattleReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_HOLE_CHECK_REQ:				OnBattleHoleCheckReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_WAVE_READY_INFO_REQ:			OnBattleWaveReadyInfoReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_WAVE_READY_INFO_CANCEL_REQ:	OnBattleWaveReadyInfoCancelReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_RECORD_REQ:					OnBattleRecordReq(pData, dataSize);					break;
	case PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ:			OnBattleSuggestKickVoteReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ:				OnBattleVoteKickVoteReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_SENDPING_REQ:					OnBattleSendPingReq(pData, dataSize);				break;
	case PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_REQ:	OnBattleChangeDifficultyReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_REQ:	OnBattleMissionTouchdownCountReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_MISSION_DEATHBLOW_REQ:			OnBattleMissionDeathblowReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_REQ:			OnBattleSupplyBoxResultReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_USER_SKILL_UPGRADE_REQ:		OnBattleUserSkillUpgradeReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ:			OnBattleSlotEquipmentReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_REQ:	OnBattleMissionTutorialRoundEndReq(pData, dataSize);break;
	case PROTOCOL_BATTLE_USER_SOPETYPE_REQ:			OnBattleUserScopeTypeReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_TIMEOUTMAINCLIENT_REQ:		OnBattleTimeoutMainClientReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_REQ:	OnBattleNewJoinRoomScoreReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_REPORTSCREENSHOT_REQ:		OnBattleReportScreenshotReq(pData, dataSize);		break;

	// ---- Shop (GameSessionShop.cpp) ----
	case PROTOCOL_SHOP_ENTER_REQ:					OnShopEnterReq(pData, dataSize);			break;
	case PROTOCOL_SHOP_LEAVE_REQ:					OnShopLeaveReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ:			OnShopBuyReq(pData, dataSize);				break;
	case PROTOCOL_SHOP_REPAIR_REQ:					OnShopRepairReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ:			OnShopGiftReq(pData, dataSize);				break;
	case PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ:		OnShopItemExtendReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_GET_POINT_CASH_REQ:			OnGetPointCashReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_VERSION_REQ:			OnShopVersionReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_LIST_REQ:				OnShopListReq(pData, dataSize);				break;
	case PROTOCOL_AUTH_SHOP_GOODSLIST_REQ:			OnShopGoodsListReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_ITEMLIST_REQ:			OnShopItemListReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_MATCHINGLIST_REQ:		OnShopMatchingListReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_REPAIRLIST_REQ:		OnShopRepairListReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ:			OnShopItemAuthReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ:		OnShopInsertItemReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ:		OnShopDeleteItemReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_CAPSULE_REQ:		OnShopCapsuleReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_JACKPOT_REQ:		OnShopJackpotReq(pData, dataSize);			break;
	case PROTOCOL_SHOP_GET_SAILLIST_REQ:		OnShopGetSaleListReq(pData, dataSize);		break;
	case PROTOCOL_SHOP_PLUS_POINT_REQ:			OnShopPlusPointReq(pData, dataSize);		break;

	// ---- Quest (GameSessionQuest.cpp) ----
	case PROTOCOL_BASE_QUEST_GET_REQ:				OnQuestGetReq(pData, dataSize);				break;
	case PROTOCOL_BASE_QUEST_GET_INFO_REQ:			OnQuestGetInfoReq(pData, dataSize);			break;
	case PROTOCOL_BASE_QUEST_GET_ITEM_REQ:			OnQuestGetItemReq(pData, dataSize);			break;
	case PROTOCOL_BASE_QUEST_CHANGE_REQ:			OnQuestChangeReq(pData, dataSize);			break;
	case PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ:	OnQuestActiveIdxChangeReq(pData, dataSize);	break;

	// ---- Social (GameSessionSocial.cpp) ----
	case PROTOCOL_AUTH_FRIEND_INFO_REQ:				OnFriendInfoReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FRIEND_INSERT_REQ:			OnFriendInsertReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FRIEND_DELETE_REQ:			OnFriendDeleteReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FRIEND_ACCEPT_REQ:			OnFriendAcceptReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FRIEND_INVITED_REQ:			OnFriendInviteReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SEND_WHISPER_REQ:			OnWhisperReq(pData, dataSize);				break;
	case PROTOCOL_AUTH_BLOCK_INSERT_REQ:			OnBlockInsertReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_BLOCK_DELETE_REQ:			OnBlockDeleteReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FIND_USER_REQ:				OnFindUserReq(pData, dataSize);				break;
	case PROTOCOL_AUTH_CHANGE_NICKNAME_REQ:			OnChangeNicknameReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_CHANGE_COLOR_NICK_REQ:		OnChangeColorNickReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ:			OnGiftShopEnterReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ:			OnGiftShopLeaveReq(pData, dataSize);		break;
	case PROTOCOL_COMMUNITY_USER_INVITED_REQ:		OnCommunityUserInviteReq(pData, dataSize);	break;

	// ---- Notes/Mail (GameSessionSocial.cpp) ----
	case PROTOCOL_MESSENGER_NOTE_SEND_REQ:			OnNoteSendReq(pData, dataSize);				break;
	case PROTOCOL_MESSENGER_NOTE_LIST_REQ:			OnNoteListReq(pData, dataSize);				break;
	case PROTOCOL_MESSENGER_NOTE_DELETE_REQ:		OnNoteDeleteReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ:	OnNoteCheckReadedReq(pData, dataSize);		break;

	// ---- Medal (GameSessionMedal.cpp) ----
	case PROTOCOL_BASE_GET_MEDALSYSTEM_REQ:			OnGetMedalSystemReq(pData, dataSize);		break;
	case PROTOCOL_BASE_REFRESH_MEDALSYSTEM_REQ:		OnRefreshMedalSystemReq(pData, dataSize);	break;
	case PROTOCOL_BASE_MEDAL_GET_INFO_REQ:			OnMedalGetInfoReq(pData, dataSize);			break;
	case PROTOCOL_GET_MEDAL_INFO_REQ:				OnMedalGetInfoReq(pData, dataSize);			break;
	case PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ:		OnGetCurMedalSetInfoReq(pData, dataSize);	break;
	case PROTOCOL_GET_COM_MEDAL_SET_INFO_REQ:		OnGetComMedalSetInfoReq(pData, dataSize);	break;
	case PROTOCOL_GET_MEDAL_REWARD_REQ:				OnGetMedalRewardReq(pData, dataSize);		break;
	case PROTOCOL_SET_NOTIFY_MEDAL_REQ:				OnSetNotifyMedalReq(pData, dataSize);		break;

	// ---- Event/Attendance (GameSessionEvent.cpp) ----
	case PROTOCOL_BASE_ATTENDANCE_REQ:				OnAttendanceReq(pData, dataSize);			break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ:	OnAttendanceClearItemReq(pData, dataSize);	break;
	case PROTOCOL_BASE_DAILY_RECORD_REQ:			OnDailyRecordReq(pData, dataSize);			break;

	// ---- Skill (GameSessionSkill.cpp) ----
	case PROTOCOL_SKILL_SET_REQ:					OnSkillSetReq(pData, dataSize);				break;
	case PROTOCOL_SKILL_RESET_REQ:					OnSkillResetReq(pData, dataSize);			break;
	case PROTOCOL_SKILL_CLASS_SET_REQ:				OnSkillClassSetReq(pData, dataSize);		break;

	// ---- Clan (GameSessionClan.cpp) ----
	case PROTOCOL_CS_CREATE_CLAN_REQ:				OnClanCreateReq(pData, dataSize);			break;
	case PROTOCOL_CS_CLOSE_CLAN_REQ:				OnClanCloseReq(pData, dataSize);			break;
	case PROTOCOL_CS_DETAIL_INFO_REQ:				OnClanDetailInfoReq(pData, dataSize);		break;
	case PROTOCOL_CS_MEMBER_LIST_REQ:				OnClanMemberListReq(pData, dataSize);		break;
	case PROTOCOL_CS_CLIENT_ENTER_REQ:				OnClanEnterReq(pData, dataSize);			break;
	case PROTOCOL_CS_CLIENT_LEAVE_REQ:				OnClanLeaveReq(pData, dataSize);			break;
	case PROTOCOL_CS_CLIENT_CLAN_LIST_REQ:			OnClanListReq(pData, dataSize);				break;
	case PROTOCOL_CS_JOIN_REQUEST_REQ:				OnClanJoinReq(pData, dataSize);				break;
	case PROTOCOL_CS_SECESSION_CLAN_REQ:			OnClanLeaveReq(pData, dataSize);			break;
	case PROTOCOL_CS_DEPORTATION_REQ:				OnClanDeportationReq(pData, dataSize);		break;
	case PROTOCOL_CS_COMMISSION_MASTER_REQ:			OnClanCommissionMasterReq(pData, dataSize);	break;
	case PROTOCOL_CS_COMMISSION_STAFF_REQ:			OnClanCommissionStaffReq(pData, dataSize);	break;
	case PROTOCOL_CS_COMMISSION_REGULAR_REQ:		OnClanCommissionRegularReq(pData, dataSize);break;
	case PROTOCOL_CS_CHATTING_REQ:					OnClanChatReq(pData, dataSize);				break;
	case PROTOCOL_CS_REPLACE_NOTICE_REQ:			OnClanReplaceNoticeReq(pData, dataSize);	break;
	case PROTOCOL_CS_REPLACE_INTRO_REQ:				OnClanReplaceIntroReq(pData, dataSize);		break;
	case PROTOCOL_CS_REPLACE_MARK_REQ:				OnClanReplaceMarkReq(pData, dataSize);		break;
	case PROTOCOL_CS_ACCEPT_REQUEST_REQ:			OnClanAcceptRequestReq(pData, dataSize);		break;
	case PROTOCOL_CS_DENIAL_REQUEST_REQ:			OnClanDenialRequestReq(pData, dataSize);	break;
	case PROTOCOL_CS_INVITE_REQ:					OnClanInviteReq(pData, dataSize);			break;
	case PROTOCOL_CS_REQUEST_LIST_REQ:				OnClanRequestListReq(pData, dataSize);		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_REQ:			OnClanCheckDuplicateReq(pData, dataSize);	break;
	case PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ:		OnClanMemberDetailInfoReq(pData, dataSize);	break;
	case PROTOCOL_CS_PAGE_CHATTING_REQ:				OnClanPageChattingReq(pData, dataSize);		break;
	case PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ:		OnClanListDetailInfoReq(pData, dataSize);	break;

	// ---- Roulette/Gacha (GameSessionRoulette.cpp) ----
	case PROTOCOL_RS_ENTER_REQ:						OnRouletteEnterReq(pData, dataSize);		break;
	case PROTOCOL_RS_LEAVE_REQ:						OnRouletteLeaveReq(pData, dataSize);		break;
	case PROTOCOL_RS_ITEM_INFO_REQ:
	case PROTOCOL_RS_HIDDEN_ITEM_INFO_REQ:
	case PROTOCOL_RS_LOSE_ITEM_INFO_REQ:
	case PROTOCOL_RS_GRADE1_ITEM_INFO_REQ:
	case PROTOCOL_RS_GRADE2_ITEM_INFO_REQ:
	case PROTOCOL_RS_GRADE3_ITEM_INFO_REQ:			OnRouletteItemInfoReq(pData, dataSize);		break;
	case PROTOCOL_RS_ROULETTE_START_REQ:			OnRouletteStartReq(pData, dataSize);		break;
	case PROTOCOL_RS_JACKPOT_NOTIFY_REQ:			OnRouletteJackpotNotifyReq(pData, dataSize);break;
	case PROTOCOL_FIELDSHOP_OPEN_REQ:				OnFieldShopOpenReq(pData, dataSize);		break;
	case PROTOCOL_FIELDSHOP_GOODSLIST_REQ:			OnFieldShopGoodsListReq(pData, dataSize);	break;

	// ---- Clan Match (GameSessionClanMatch.cpp - 0x0800) ----
	case PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ:		OnClanMatchTeamContextReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_TEAM_CREATE_REQ:			OnClanMatchTeamCreateReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_TEAM_JOIN_REQ:			OnClanMatchTeamJoinReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ:			OnClanMatchTeamLeaveReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ:		OnClanMatchAllTeamListReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ:		OnClanMatchFightRequestReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ:		OnClanMatchFightAcceptReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_CHATING_REQ:				OnClanMatchChatReq(pData, dataSize);		break;

	// ---- Clan War 1.5 (GameSessionClanWar.cpp - 0x1B00) ----
	case PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ:			OnClanWarCreateTeamReq(pData, dataSize);	break;
	case PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ:			OnClanWarJoinTeamReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ:			OnClanWarLeaveTeamReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ:		OnClanWarTeamListReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_MATCHMAKING_REQ:			OnClanWarMatchmakingReq(pData, dataSize);	break;
	case PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ:	OnClanWarCancelMatchmakingReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ:		OnClanWarTeamChatReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ:		OnClanWarChangeMaxPerReq(pData, dataSize);	break;
	case PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ:		OnClanWarMercenaryListReq(pData, dataSize);	break;
	case PROTOCOL_CLAN_WAR_RESULT_REQ:				OnClanWarResultReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ:	OnClanWarRegistMercenaryReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ:	OnClanWarRemoveMercenaryReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ:	OnClanWarInviteMercenaryReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ:		OnClanWarInviteAcceptReq(pData, dataSize);	break;
	case PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ:		OnClanWarInviteDenialReq(pData, dataSize);	break;

	// ---- Gacha (GameSessionRoulette.cpp - 0x1400) ----
	case PROTOCOL_GACHA_ITEM_INFO_REQ:				OnGachaItemInfoReq(pData, dataSize);		break;
	case PROTOCOL_GACHA_SHOP_STATE_REQ:				OnGachaShopStateReq(pData, dataSize);		break;
	case PROTOCOL_GACHA_ENTER_REQ:					OnGachaEnterReq(pData, dataSize);			break;
	case PROTOCOL_GACHA_LEAVE_REQ:					OnGachaLeaveReq(pData, dataSize);			break;
	case PROTOCOL_GACHA_PURCHASE_REQ:				OnGachaPurchaseReq(pData, dataSize);		break;
	case PROTOCOL_GACHA_GET_PURCHASE_COUNT_REQ:		OnGachaGetPurchaseCountReq(pData, dataSize);break;

	// ---- MyInfo (GameSessionRoulette.cpp - 0x1900) ----
	case PROTOCOL_MYINFO_ENTER_REQ:					OnMyInfoEnterReq(pData, dataSize);			break;
	case PROTOCOL_MYINFO_LEAVE_REQ:					OnMyInfoLeaveReq(pData, dataSize);			break;

	// ---- GM Commands (GameSessionGM.cpp - Phase 11A) ----
	case PROTOCOL_ROOM_GM_KICK_USER_REQ:			OnGMKickUserReq(pData, dataSize);			break;
	case PROTOCOL_ROOM_GM_EXIT_USER_REQ:			OnGMExitUserReq(pData, dataSize);			break;
	case PROTOCOL_LOBBY_GM_EXIT_USER_REQ:			OnLobbyGMExitUserReq(pData, dataSize);		break;

	case PROTOCOL_ROOM_GM_BLOCK_USER_REQ:			OnGMBlockUserReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_GM_PAUSE_REQ:				OnGMPauseBattleReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_GM_RESUME_REQ:				OnGMResumeBattleReq(pData, dataSize);		break;

	// ---- GM Chat (GameSessionGMChat.cpp - Phase 7C) ----
	case PROTOCOL_GMCHAT_START_CHAT_REQ:			OnGMChatStartReq(pData, dataSize);			break;
	case PROTOCOL_GMCHAT_SEND_CHAT_REQ:				OnGMChatSendReq(pData, dataSize);			break;
	case PROTOCOL_GMCHAT_END_CHAT_REQ:				OnGMChatEndReq(pData, dataSize);			break;
	case PROTOCOL_GMCHAT_APPLY_PENALTY_REQ:			OnGMChatPenaltyReq(pData, dataSize);		break;

	// ---- Cheat/Debug Commands (GameSessionGM.cpp - Phase 13) ----
	case PROTOCOL_CHEAT_INCREASE_KILL_COUNT_REQ:	OnCheatIncreaseKillReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_PLAY_SOLO_REQ:				OnCheatPlaySoloReq(pData, dataSize);		break;
	case PROTOCOL_CHEAT_REDUCE_ROUND_TIME_REQ:		OnCheatReduceRoundTimeReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_CHARACTER_TELEPORT_REQ:		OnCheatTeleportReq(pData, dataSize);		break;

	// ---- Clan War extras (GameSessionClanWar.cpp - Batch 17) ----
	case PROTOCOL_CLAN_WAR_CREATE_ROOM_REQ:			OnClanWarCreateRoomReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_JOIN_ROOM_REQ:			OnClanWarJoinRoomReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_LEAVE_ROOM_REQ:			OnClanWarLeaveRoomReq(pData, dataSize);		break;
	case PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ:OnClanWarMercenaryDetailInfoReq(pData, dataSize);break;

	// ---- Clan extras (GameSessionClan.cpp - Batch 17) ----
	case PROTOCOL_CS_INVITE_ACCEPT_REQ:				OnClanInviteAcceptReq(pData, dataSize);		break;
	case PROTOCOL_CS_CANCEL_REQUEST_REQ:			OnClanCancelRequestReq(pData, dataSize);	break;
	case PROTOCOL_CS_REPLACE_NAME_REQ:				OnClanReplaceNameReq(pData, dataSize);		break;
	case PROTOCOL_CS_NOTE_REQ:						OnClanNoteReq(pData, dataSize);				break;
	case PROTOCOL_CS_REPLACE_PERSONMAX_REQ:			OnClanReplacePersonmaxReq(pData, dataSize);	break;
	case PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ:		OnClanCheckJoinAuthorityReq(pData, dataSize);break;
	case PROTOCOL_CS_CHECK_MARK_REQ:				OnClanCheckMarkReq(pData, dataSize);		break;

	// ---- Auth/Social extras (GameSessionSocial.cpp - Batch 17) ----
	case PROTOCOL_AUTH_GET_MYINFO_REQ:				OnAuthGetMyInfoReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_FRIEND_ROOM_ENTER_REQ:		OnAuthFriendRoomEnterReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_REQ:		OnAuthFriendRoomLeaveReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_SEND_WHISPER_FIND_UID_REQ:	OnWhisperFindUIDReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_RECV_WHISPER_REQ:			OnAuthRecvWhisperReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_USED_WEAPON_REQ:				OnAuthUsedWeaponReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_FCM_INFO_REQ:				OnAuthFCMInfoReq(pData, dataSize);			break;

	// ---- Base extras (GameSession.cpp - Batch 17) ----
	case PROTOCOL_BASE_URL_LIST_REQ:				OnBaseUrlListReq(pData, dataSize);			break;
	case PROTOCOL_BASE_SERVER_INFO_REQ:				OnBaseServerInfoReq(pData, dataSize);		break;
	case PROTOCOL_BASE_CLIENT_END_TYPE_REQ:			OnBaseClientEndTypeReq(pData, dataSize);	break;
	case PROTOCOL_BASE_USER_TITLE_INFO_REQ:			OnBaseUserTitleInfoReq(pData, dataSize);		break;
	case PROTOCOL_BASE_QUEST_FINISH_NOTICE_REQ:		OnBaseQuestFinishNoticeReq(pData, dataSize);break;
	case PROTOCOL_BASE_REGIST_TRAINING_SCORE_REQ:	OnBaseRegistTrainingScoreReq(pData, dataSize);break;
	case PROTOCOL_BASE_GET_USER_LIST_REQ:			OnBaseGetUserListReq(pData, dataSize);		break;
	case PROTOCOL_BASE_CHANGE_CLAN_NAME_REQ:		OnBaseChangeClanNameReq(pData, dataSize);	break;
	case PROTOCOL_BASE_CHANGE_CLAN_MARK_REQ:		OnBaseChangeClanMarkReq(pData, dataSize);	break;

	// ---- Shop extras (GameSessionShop.cpp - Batch 17) ----
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ:		OnShopGetGiftListReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_EXPIRE_DELETE_ITEM_REQ:	OnShopExpireDeleteItemReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_SHOP_REWARD_ITEM_REQ:		OnShopRewardItemReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_SALECOUPONLIST_REQ:		OnShopSaleCouponListReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ:			OnShopAuthGiftReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_REQ:		OnShopNotifyGiftReq(pData, dataSize);		break;

	// ---- Battle extras (GameSessionBattle.cpp - Batch 17) ----
	case PROTOCOL_BATTLE_START_KICKVOTE_REQ:		OnBattleStartKickVoteReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_CHEAT_MESSAGE_REQ:			OnBattleCheatMessageReq(pData, dataSize);	break;

	// ---- Battle extras (GameSessionBattle.cpp - Batch 18) ----
	case PROTOCOL_BATTLE_START_COUNTDOWN_REQ:		OnBattleStartCountdownReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_START_LOADING_REQ:			OnBattleStartLoadingReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_DSERVER_REQ:OnBattlePrestartBattleDServerReq(pData, dataSize);break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_RELAY_REQ:	OnBattlePrestartBattleRelayReq(pData, dataSize);break;
	case PROTOCOL_BATTLE_RESULT_REQ:				OnBattleResultReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ:		OnBattleRespawnForAIReq(pData, dataSize);	break;
	case PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_REQ:OnBattleNotifyKickVoteResultReq(pData, dataSize);break;
	case PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_REQ:OnBattleNotifyKickVoteCancelReq(pData, dataSize);break;

	// ---- Room extras (GameSessionRoom.cpp - Batch 18) ----
	case PROTOCOL_ROOM_NEW_GET_SLOTINFO_REQ:		OnRoomNewGetSlotInfoReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_NEW_GET_SLOTONEINFO_REQ:	OnRoomNewGetSlotOneInfoReq(pData, dataSize);break;
	case PROTOCOL_ROOM_NEW_GET_PLAYERINFO_REQ:		OnRoomNewGetPlayerInfoReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_REQ:		OnRoomChangeHiddenSlotReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_TEAM_BALANCE_REQ:			OnRoomTeamBalanceReq(pData, dataSize);		break;

	// ---- Lobby extras (GameSessionLobby.cpp - Batch 18) ----
	case PROTOCOL_LOBBY_CHECK_NICK_NAME_REQ:		OnLobbyCheckNickNameReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_CREATE_NICK_NAME_REQ:		OnLobbyCreateNickNameReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_NEW_CREATE_ROOM_REQ:		OnLobbyNewCreateRoomReq(pData, dataSize);	break;
	case PROTOCOL_LOBBY_NEW_JOIN_ROOM_REQ:			OnLobbyNewJoinRoomReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_REQ:	OnLobbyNewViewUserItemReq(pData, dataSize);	break;

	// ---- Base extras (GameSession.cpp - Batch 18) ----
	case PROTOCOL_BASE_USER_ENTER_REQ:				OnBaseUserEnterReq(pData, dataSize);		break;
	case PROTOCOL_BASE_USER_LEAVE_REQ:				OnBaseUserLeaveReq(pData, dataSize);		break;
	case PROTOCOL_BASE_GAME_SERVER_STATE_REQ:		OnBaseGameServerStateReq(pData, dataSize);	break;
	case PROTOCOL_BASE_READY_HEART_BIT_REQ:			OnBaseReadyHeartBitReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_MYCLAN_REQ:				OnBaseGetMyClanReq(pData, dataSize);		break;
	case PROTOCOL_BASE_GET_USERINFO_ALL_REQ:		OnBaseGetUserInfoAllReq(pData, dataSize);	break;
	case PROTOCOL_BASE_GET_USERINFO_ALL_DB_REQ:		OnBaseGetUserInfoAllDBReq(pData, dataSize);	break;
	case PROTOCOL_BASE_ENTER_PASS_REQ:				OnBaseEnterPassReq(pData, dataSize);		break;
	case PROTOCOL_BASE_GET_UID_LOBBY_REQ:			OnBaseGetUidLobbyReq(pData, dataSize);		break;
	case PROTOCOL_BASE_GET_UID_ROOM_REQ:			OnBaseGetUidRoomReq(pData, dataSize);		break;

	// ---- Cheat extras (GameSessionGM.cpp - Batch 18) ----
	case PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ:OnCheatClanWarMatchingPointReq(pData, dataSize);break;
	case PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ:		OnCheatClanWarResultReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_REQ:	OnCheatDamageGameObjectReq(pData, dataSize);break;
	case PROTOCOL_CHEAT_DETECT_HACK_OFF_REQ:		OnCheatDetectHackOffReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_ITEM_AUTH_REQ:				OnCheatItemAuthReq(pData, dataSize);		break;
	case PROTOCOL_CHEAT_MEDAL_COMMAND_REQ:			OnCheatMedalCommandReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_REDUCE_TS_EVENT_REQ:		OnCheatReduceTsEventReq(pData, dataSize);	break;
	case PROTOCOL_CHEAT_TIMER_GM_PAUSE_REQ:			OnCheatTimerGMPauseReq(pData, dataSize);	break;

	// ---- Batch 19 - Roulette/RS/FieldShop (GameSessionShop.cpp) ----
	case PROTOCOL_AUTH_RS_ENTER_REQ:				OnAuthRsEnterReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_RS_ITEM_INFO_REQ:			OnAuthRsItemInfoReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_RS_JACKPOT_REQ:				OnAuthRsJackpotReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_RS_START_REQ:				OnAuthRsStartReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FCM_LOGOUT_REQ:				OnAuthFcmLogoutReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ:			OnAuthShopAuthGiftReq(pData, dataSize);		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ:		OnAuthShopItemAuthDataReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_REQ:	OnAuthShopItemChangeDataReq(pData, dataSize);break;
	case PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_REQ:		OnAuthShopUseGiftcouponReq(pData, dataSize);break;
	case PROTOCOL_AUTH_USE_GIFTCOUPON_REQ:			OnAuthUseGiftcouponReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_REQ:		OnAuthUseItemCheckNickReq(pData, dataSize);	break;
	case PROTOCOL_AUTH_USER_NOW_INFO_REQ:			OnAuthUserNowInfoReq(pData, dataSize);		break;
	case PROTOCOL_FIELDSHOP_SEND_NEWVERSION_REQ:	OnFieldshopSendNewversionReq(pData, dataSize);break;

	// ---- Batch 19 - ClanWar extras (GameSessionClanWar.cpp) ----
	case PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ:	OnClanWarChangeOperationReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ:OnClanWarCheckPreseasonRankingReq(pData, dataSize);break;
	case PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_REQ:OnClanWarMercenaryPenaltyLoadReq(pData, dataSize);break;

	// ---- Batch 19 - Clan CS_NEW (GameSessionClan.cpp) ----
	case PROTOCOL_CS_NEW_DETAIL_INFO_REQ:			OnClanNewDetailInfoReq(pData, dataSize);	break;
	case PROTOCOL_CS_NEW_CREATE_CLAN_REQ:			OnClanNewCreateClanReq(pData, dataSize);	break;
	case PROTOCOL_CS_NEW_REQUEST_LIST_REQ:			OnClanNewRequestListReq(pData, dataSize);	break;

	// ---- Batch 19 - Base extras (GameSession.cpp) ----
	case PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_REQ:	OnBaseRendezvousNatStateReq(pData, dataSize);break;
	case PROTOCOL_BASE_RENDEZVOUS_HOLEPUNCH_REQ:	OnBaseRendezvousHolepunchReq(pData, dataSize);break;
	case PROTOCOL_BASE_HACKING_CHECK_KICK_REQ:		OnBaseHackingCheckKickReq(pData, dataSize);	break;
	case PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_REQ:	OnBaseChangePccafeStatusReq(pData, dataSize);break;
	case PROTOCOL_BASE_GET_USER_SUBTASK_REQ:		OnBaseGetUserSubtaskReq(pData, dataSize);	break;
	case PROTOCOL_BASE_LOGIN_THAI_REQ:				OnBaseLoginThaiReq(pData, dataSize);		break;

	// ---- Batch 19 - Character/Chara (GameSession.cpp) ----
	case PROTOCOL_DELETE_CHARA_REQ:					OnDeleteCharaReq(pData, dataSize);			break;
	case PROTOCOL_NEW_SET_CURRENT_CHARA_REQ:		OnNewSetCurrentCharaReq(pData, dataSize);	break;
	case PROTOCOL_NEW_GAME_SERVER_STATE_REQ:		OnNewGameServerStateReq(pData, dataSize);	break;

	// ---- Batch 19 - Skill (GameSession.cpp) ----
	case PROTOCOL_SET_SPECIALITY_REQ:				OnSetSpecialityReq(pData, dataSize);		break;
	case PROTOCOL_INIT_SPECIALITY_REQ:				OnInitSpecialityReq(pData, dataSize);		break;

	// ---- Batch 19 - Quest extras (GameSession.cpp) ----
	case PROTOCOL_GET_ALL_QUEST_INFO_REQ:			OnGetAllQuestInfoReq(pData, dataSize);		break;
	case PROTOCOL_GET_HONOR_QUEST_INFO_REQ:			OnGetHonorQuestInfoReq(pData, dataSize);	break;
	case PROTOCOL_PERIOD_QUEST_SETTING_REQ:			OnPeriodQuestSettingReq(pData, dataSize);	break;

	// ---- Batch 19 - Room/Lobby GM (GameSessionGM.cpp / GameSessionChannel.cpp) ----
	case PROTOCOL_ROOM_GM_DESTROY_ROOM_REQ:			OnRoomGmDestroyRoomReq(pData, dataSize);	break;
	case PROTOCOL_ROOM_GM_GET_UID_REQ:				OnRoomGmGetUidReq(pData, dataSize);			break;
	case PROTOCOL_LOBBY_GM_GET_UID_REQ:				OnLobbyGmGetUidReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ:		OnLobbyAbusingPopupEndReq(pData, dataSize);	break;

	// ---- Batch 20 - MESSENGER_* aliases to existing social handlers ----
	case PROTOCOL_MESSENGER_FRIEND_INSERT_REQ:		OnFriendInsertReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_FRIEND_DELETE_REQ:		OnFriendDeleteReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ:		OnFriendAcceptReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_FRIEND_INFO_REQ:		OnFriendInfoReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_FRIEND_INVITED_REQ:	OnFriendInviteReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_SEND_WHISPER_REQ:		OnWhisperReq(pData, dataSize);				break;
	case PROTOCOL_MESSENGER_FIND_USER_REQ:			OnFindUserReq(pData, dataSize);				break;
	case PROTOCOL_MESSENGER_BLOCK_INSERT_REQ:		OnBlockInsertReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_BLOCK_DELETE_REQ:		OnBlockDeleteReq(pData, dataSize);			break;
	case PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ:OnMessengerWhisperByUidReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_FUSER_INFO_REQ:			OnMessengerFuserInfoReq(pData, dataSize);	break;
	case PROTOCOL_MESSENGER_KICKUSER_REQ:			OnMessengerKickUserReq(pData, dataSize);	break;
	case PROTOCOL_MESSENGER_FRIEND_INVITE_FAIL_REQ:	OnMessengerFriendInviteFailReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_NOTE_RECEIVE_REQ:		OnMessengerNoteReceiveReq(pData, dataSize);	break;
	case PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ:OnMessengerNoteSendFindUidReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_NOTE_SEND_GIFT_NOTICE_REQ:OnMessengerNoteGiftNoticeReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_NOTE_SEND_USERGIFT_NOTICE_REQ:OnMessengerNoteGiftNoticeReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ:	OnMessengerClanNoteSendReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ:OnMessengerClanGroupNoteSendReq(pData, dataSize);break;
	case PROTOCOL_MESSENGER_FRIEND_CHANGE_STATE_REQ:OnMessengerFriendChangeStateReq(pData, dataSize);break;

	// ---- Batch 20 - CS_* clan context/operations (GameSessionClan.cpp) ----
	case PROTOCOL_CS_CLAN_CONTEXT_REQ:				OnClanContextReq(pData, dataSize);			break;
	case PROTOCOL_CS_CLIENT_CLAN_CONTEXT_REQ:		OnClanClientContextReq(pData, dataSize);	break;
	case PROTOCOL_CS_CREATE_CLAN_CONDITION_REQ:		OnClanCreateConditionReq(pData, dataSize);	break;
	case PROTOCOL_CS_REQUEST_CONTEXT_REQ:			OnClanRequestContextReq(pData, dataSize);	break;
	case PROTOCOL_CS_REQUEST_INFO_REQ:				OnClanRequestInfoReq(pData, dataSize);		break;
	case PROTOCOL_CS_MEMBER_CONTEXT_REQ:			OnClanMemberContextReq(pData, dataSize);	break;
	case PROTOCOL_CS_STAFF_AUTHORITY_REQ:			OnClanStaffAuthorityReq(pData, dataSize);	break;
	case PROTOCOL_CS_ROOM_INVITED_REQ:				OnClanRoomInvitedReq(pData, dataSize);		break;
	case PROTOCOL_CS_EXPUP_REQ:						OnClanExpUpReq(pData, dataSize);			break;
	case PROTOCOL_CS_REPLACE_MEMBER_NICK_REQ:		OnClanReplaceMemberNickReq(pData, dataSize);break;
	case PROTOCOL_CS_REPLACE_MEMBER_RANK_REQ:		OnClanReplaceMemberRankReq(pData, dataSize);break;
	case PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_REQ:	OnClanReplaceMemberColorNickReq(pData, dataSize);break;
	case PROTOCOL_CS_REPLACE_MANAGEMENT_REQ:		OnClanReplaceManagementReq(pData, dataSize);break;
	case PROTOCOL_CS_CLAN_LIST_STREAM_REQ:			OnClanListStreamReq(pData, dataSize);		break;
	case PROTOCOL_CS_CLAN_LIST_GS_REQ:				OnClanListGsReq(pData, dataSize);			break;
	case PROTOCOL_CS_CLAN_INO_RANK_UPDATE_REQ:		OnClanInoRankUpdateReq(pData, dataSize);	break;
	case PROTOCOL_CS_CLAN_ITEM_DELETE_REQ:			OnClanItemDeleteReq(pData, dataSize);		break;
	case PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ:	OnClanCheckDuplicateAzitUrlReq(pData, dataSize);break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ:	OnClanMatchResultContextReq(pData, dataSize);break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ:	OnClanMatchResultListReq(pData, dataSize);	break;
	case PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ:OnClanMatchResultEmotionSaveReq(pData, dataSize);break;
	case PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ:OnClanRequestAutoAcceptReq(pData, dataSize);break;

	// ---- Batch 20 - CS_MATCH_* clan match extras (GameSessionClanWar.cpp) ----
	case PROTOCOL_CS_MATCH_CHANNEL_REQ:				OnClanMatchChannelReq(pData, dataSize);		break;
	case PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ:	OnClanMatchFightQuickRequestReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ:	OnClanMatchTeamDetailInfoReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ:	OnClanMatchTeamChangePerReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_TEAM_LIST_REQ:			OnClanMatchTeamListReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_NEW_CLAN_INFO_REQ:		OnClanMatchNewClanInfoReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_NEW_FIGHT_QUICK_REQUEST_REQ:OnClanMatchNewFightQuickReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_REQ:OnClanMatchNewFightOtherReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ:OnClanMatchNewGetRoomInfoReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_NEW_HONOR_REQ:			OnClanMatchNewHonorReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_REQ:	OnClanMatchNewHonorAcceptReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_NEW_OPERATION_REQ:		OnClanMatchNewOperationReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_REQ:		OnClanMatchRoomCreateGsReq(pData, dataSize);break;
	case PROTOCOL_CS_MATCH_ROOM_JOIN_GS_REQ:		OnClanMatchRoomJoinGsReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ:		OnClanMatchRoomLeaveGsReq(pData, dataSize);	break;
	case PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ:			OnClanMatchRoomLeaveReq(pData, dataSize);	break;

	default:
		printf("[GameSession] Unknown protocol 0x%04X from Index=%d\n", protocolId, GetIndex());
		break;
	}

	return packetSize;
}

// ============================================================================
// Base & Login Handlers
// ============================================================================

void GameSession::OnBaseConnectReq(char* pData, INT32 i32Size)
{
	// Key must be 1-7 for BitRotateEncript/Decript (bit rotation count)
	m_ui32EncriptKey = (GetIndex() % 7) + 1;
	m_eMainTask = GAME_TASK_CONNECT;
	SendConnectAck();
}

void GameSession::OnBaseHeartBitReq(char* pData, INT32 i32Size)
{
	SendHeartBitAck();
}

void GameSession::OnLoginReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask != GAME_TASK_CONNECT)
	{
		SendLoginAck(1);
		return;
	}

	m_eMainTask = GAME_TASK_LOGIN_ING;

	// Track login attempts
	if (g_pContextMain)
		g_pContextMain->IncrementLogins();

	if (i32Size < (int)(sizeof(uint32_t) + 64))
	{
		SendLoginAck(2);
		return;
	}

	uint32_t authToken = *(uint32_t*)pData;
	memcpy(m_szUsername, pData + sizeof(uint32_t), 64);
	m_szUsername[63] = '\0';

	// Validate auth token via ModuleConnectServer
	m_ui32AuthToken = authToken;
	int64_t validatedUID = 0;
	char validatedUsername[64] = {};

	ModuleConnectServer* pModCS = g_pGameServer ? g_pGameServer->GetModuleConnectServer() : nullptr;
	if (pModCS && pModCS->IsConnected())
	{
		if (!pModCS->ValidateToken(authToken, &validatedUID, validatedUsername))
		{
			printf("[GameSession] Auth token validation failed - Index=%d, Token=%u\n",
				GetIndex(), authToken);
			SendLoginAck(3);	// 3 = invalid token
			m_eMainTask = GAME_TASK_CONNECT;
			return;
		}
		m_i64UID = validatedUID;
		strncpy_s(m_szUsername, validatedUsername, _TRUNCATE);
	}
	else
	{
		// No ConnectServer connection - development mode, accept all
		m_i64UID = (int64_t)(GetIndex() + 1);
	}

	m_eMainTask = GAME_TASK_LOGIN;

	// Request player data from DataServer
	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
	{
		g_pModuleDataServer->RequestPlayerLoad(GetIndex(), m_i64UID);
		// Response will arrive in OnPlayerDataLoaded() which calls SendLoginAck
		printf("[GameSession] Login - requesting player data - Index=%d, User=%s, UID=%lld\n",
			GetIndex(), m_szUsername, m_i64UID);
	}
	else
	{
		// No DataServer - use defaults (development mode)
		InitDefaultPlayerData();
		m_eMainTask = GAME_TASK_INFO;
		SendLoginAck(0);

		printf("[GameSession] Login success (defaults) - Index=%d, User=%s, UID=%lld\n",
			GetIndex(), m_szUsername, m_i64UID);
	}
}

void GameSession::OnGetUserInfoReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &m_i32Level, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i64Exp, sizeof(int64_t));	offset += sizeof(int64_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32GP, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	// Send boost event info (Phase 14B)
	SendBoostEventInfo();

	// Send title info (Phase 4D)
	SendTitleInfo();

	// Check attendance on login (Phase 14A)
	CheckAttendanceOnLogin();

	m_eMainTask = GAME_TASK_CHANNEL;
}

// ============================================================================
// User Detail Info (Phase 4B)
// ============================================================================

// Helper: fill user detail info into buffer, returns bytes written
static int FillUserDetailInfo(char* buf, GameSession* pTarget)
{
	int off = 0;

	// S2_USER_DETAIL_INFO layout (from S2MODefine.h):
	// UID(8) + NickName(64) + ClanName(64) + ClanMark(4) + Rank(1)
	// + Exp(4) + Match(4) + Win(4) + Draw(4) + Lose(4) + DisConnect(4)
	// + Kill(4) + Death(4) + HeadShot(4)
	// + State(4) + ServerIdx(4) + ChannelNum(4) + RoomIdx(4)

	int64_t uid = pTarget->GetUID();
	memcpy(buf + off, &uid, 8);						off += 8;
	memcpy(buf + off, pTarget->GetNickname(), 64);	off += 64;

	// Clan name (empty for now)
	char clanName[64] = {0};
	memcpy(buf + off, clanName, 64);				off += 64;

	uint32_t clanMark = 0;
	memcpy(buf + off, &clanMark, 4);				off += 4;

	uint8_t rank = (uint8_t)pTarget->GetRankId();
	memcpy(buf + off, &rank, 1);					off += 1;

	uint32_t exp32 = (uint32_t)pTarget->GetExp();
	memcpy(buf + off, &exp32, 4);					off += 4;

	// Match count (wins + losses)
	uint32_t wins = (uint32_t)pTarget->GetWins();
	uint32_t losses = (uint32_t)pTarget->GetLosses();
	uint32_t match = wins + losses;
	memcpy(buf + off, &match, 4);					off += 4;
	memcpy(buf + off, &wins, 4);					off += 4;

	uint32_t draw = 0;
	memcpy(buf + off, &draw, 4);					off += 4;
	memcpy(buf + off, &losses, 4);					off += 4;

	uint32_t disconnect = 0;
	memcpy(buf + off, &disconnect, 4);				off += 4;

	uint32_t kills = (uint32_t)pTarget->GetKills();
	uint32_t deaths = (uint32_t)pTarget->GetDeaths();
	uint32_t headshots = (uint32_t)pTarget->GetHeadshots();
	memcpy(buf + off, &kills, 4);					off += 4;
	memcpy(buf + off, &deaths, 4);					off += 4;
	memcpy(buf + off, &headshots, 4);				off += 4;

	// State, ServerIdx, ChannelNum, RoomIdx
	uint32_t state = (uint32_t)pTarget->GetMainTask();
	uint32_t serverIdx = 1;
	uint32_t channelNum = (uint32_t)pTarget->GetChannelNum();
	uint32_t roomIdx = (pTarget->GetRoomIdx() >= 0) ? (uint32_t)pTarget->GetRoomIdx() : 0xFFFFFFFF;
	memcpy(buf + off, &state, 4);					off += 4;
	memcpy(buf + off, &serverIdx, 4);				off += 4;
	memcpy(buf + off, &channelNum, 4);				off += 4;
	memcpy(buf + off, &roomIdx, 4);				off += 4;

	return off;
}

void GameSession::OnGetUserInfoLobbyReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_LOBBY)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK, 1);
		return;
	}

	int32_t targetSessionIdx = *(int32_t*)pData;
	int32_t result = 0;

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Find target session
	GameSession* pTarget = nullptr;
	if (g_pGameServerContext)
	{
		GameSessionManager* pMgr = (GameSessionManager*)g_pGameServerContext->GetSessionManager();
		if (pMgr)
			pTarget = pMgr->GetSession(targetSessionIdx);
	}

	if (pTarget && pTarget->GetMainTask() >= GAME_TASK_CHANNEL)
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
		offset += FillUserDetailInfo(buffer + offset, pTarget);
	}
	else
	{
		result = 1;
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
		// Zero-fill the detail info
		memset(buffer + offset, 0, 200);
		offset += 200;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetUserInfoRoomReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK, 1);
		return;
	}

	int32_t targetSlotIdx = *(int32_t*)pData;
	int32_t result = 0;

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	GameSession* pTarget = m_pRoom->GetSlotSession(targetSlotIdx);

	if (pTarget)
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
		offset += FillUserDetailInfo(buffer + offset, pTarget);
	}
	else
	{
		result = 1;
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
		memset(buffer + offset, 0, 200);
		offset += 200;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Boost Event Info (Phase 14B)
// ============================================================================

void GameSession::SendBoostEventInfo()
{
	if (!g_pContextMain)
		return;

	uint16_t expMult = g_pContextMain->GetCurrentExpMultiplier();
	uint16_t pointMult = g_pContextMain->GetCurrentPointMultiplier();

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_BOOSTEVENT_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// EXP multiplier (percentage, 100 = normal)
	memcpy(buffer + offset, &expMult, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Point/GP multiplier (percentage, 100 = normal)
	memcpy(buffer + offset, &pointMult, sizeof(uint16_t));	offset += sizeof(uint16_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	if (expMult > 100 || pointMult > 100)
	{
		printf("[GameSession] Boost event info sent - UID=%lld, EXP=%d%%, Point=%d%%\n",
			m_i64UID, expMult, pointMult);
	}
}

// ============================================================================
// Phase 4A - System/Login Flow Handlers
// ============================================================================

void GameSession::OnGetSystemInfoReq(char* pData, INT32 i32Size)
{
	// Send server system info: version, MD5 key, GameGuard type
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_SYSTEM_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Server version string
	char szVersion[32] = "1.0.0";
	memcpy(buffer + offset, szVersion, 32);					offset += 32;

	// MD5 key (16 bytes, zeroed = no check)
	memset(buffer + offset, 0, 16);							offset += 16;

	// GameGuard type: 0 = disabled
	uint8_t ggType = 0;
	memcpy(buffer + offset, &ggType, 1);					offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetOptionReq(char* pData, INT32 i32Size)
{
	// Send player options (controls, sensitivity, key bindings)
	// Currently a stub - send empty options
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_OPTION_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Option data (128 bytes, zeroed = defaults)
	memset(buffer + offset, 0, 128);						offset += 128;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnOptionSaveReq(char* pData, INT32 i32Size)
{
	// Accept and acknowledge option save
	// TODO: Forward to DataServer for persistent storage
	SendSimpleAck(PROTOCOL_BASE_OPTION_SAVE_ACK, 0);
}

void GameSession::OnCheckNickNameReq(char* pData, INT32 i32Size)
{
	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_BASE_CHECK_NICK_NAME_ACK, 2);	// Invalid
		return;
	}

	char szNickname[64];
	memcpy(szNickname, pData, 64);
	szNickname[63] = '\0';

	// Validate nickname length
	size_t len = strlen(szNickname);
	if (len < 2 || len > 16)
	{
		SendSimpleAck(PROTOCOL_BASE_CHECK_NICK_NAME_ACK, 2);	// Invalid length
		return;
	}

	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
	{
		g_pModuleDataServer->RequestCheckNick(GetIndex(), szNickname);
		printf("[GameSession] CheckNickName via DataServer - Index=%d, Nick=%s\n", GetIndex(), szNickname);
	}
	else
	{
		// No DataServer - always return available
		SendSimpleAck(PROTOCOL_BASE_CHECK_NICK_NAME_ACK, 0);
		printf("[GameSession] CheckNickName (no DS) - Index=%d, Nick=%s\n", GetIndex(), szNickname);
	}
}

void GameSession::OnCreateNickReq(char* pData, INT32 i32Size)
{
	if (i32Size < 64)
	{
		OnCreateNickResult(2, nullptr);
		return;
	}

	char szNickname[64];
	memcpy(szNickname, pData, 64);
	szNickname[63] = '\0';

	size_t len = strlen(szNickname);
	if (len < 2 || len > 16)
	{
		OnCreateNickResult(2, nullptr);	// Invalid
		return;
	}

	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
	{
		g_pModuleDataServer->RequestCreateNick(m_i64UID, GetIndex(), szNickname);
		printf("[GameSession] CreateNick via DataServer - Index=%d, Nick=%s\n", GetIndex(), szNickname);
	}
	else
	{
		// No DataServer - accept directly
		OnCreateNickResult(0, szNickname);
		printf("[GameSession] CreateNick (no DS) - Index=%d, Nick=%s\n", GetIndex(), szNickname);
	}
}

void GameSession::OnRankUpReq(char* pData, INT32 i32Size)
{
	// Client requests rank-up check
	CheckRankUp();
}

// Rank-up reward table (Phase 10B): GP bonus + optional item per rank
struct RankUpReward
{
	int			i32BonusGP;
	uint32_t	ui32RewardItemId;	// 0 = no item
};

static const RankUpReward s_RankUpRewards[] =
{
	{     0, 0 },	// Rank 0: Trainee (no reward)
	{  1000, 0 },	// Rank 1: Private
	{  1500, 0 },	// Rank 2: Private First Class
	{  2000, 0 },	// Rank 3: Corporal
	{  3000, 0 },	// Rank 4: Sergeant
	{  4000, 0 },	// Rank 5: Staff Sergeant
	{  5000, MAKE_ITEM_ID(ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, 2) },	// Rank 6: SFC + Kukri
	{  6000, 0 },	// Rank 7: Master Sergeant
	{  7000, 0 },	// Rank 8: Sergeant Major
	{  8000, MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 2) },	// Rank 9: 2LT + Desert Eagle
	{ 10000, 0 },	// Rank 10: First Lieutenant
	{ 12000, 0 },	// Rank 11: Captain
	{ 15000, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 3) },	// Rank 12: Major + SG550
	{ 18000, 0 },	// Rank 13: Lieutenant Colonel
	{ 20000, 0 },	// Rank 14: Colonel
	{ 25000, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, 2) },	// Rank 15: BG + PSG-1
	{ 30000, 0 },	// Rank 16: Major General
	{ 35000, 0 },	// Rank 17: Lieutenant General
	{ 50000, MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_MG, 1) },		// Rank 18: General + MG36
};
static const int RANKUP_REWARD_COUNT = sizeof(s_RankUpRewards) / sizeof(s_RankUpRewards[0]);

void GameSession::CheckRankUp()
{
	if (!g_pContextMain)
		return;

	int newRank = g_pContextMain->GetRankForExp(m_i64Exp);
	if (newRank <= m_i32RankId)
		return;	// No rank up

	int oldRank = m_i32RankId;
	m_i32RankId = newRank;

	// Phase 10B: Grant rank-up rewards for each rank gained
	int totalBonusGP = 0;
	uint32_t rewardItemId = 0;
	for (int r = oldRank + 1; r <= newRank && r < RANKUP_REWARD_COUNT; r++)
	{
		totalBonusGP += s_RankUpRewards[r].i32BonusGP;
		if (s_RankUpRewards[r].ui32RewardItemId != 0)
			rewardItemId = s_RankUpRewards[r].ui32RewardItemId;	// Last reward item
	}

	if (totalBonusGP > 0)
		m_i32GP += totalBonusGP;

	// Grant reward item if applicable
	uint32_t grantedDBIdx = 0;
	if (rewardItemId != 0 && !HasInventoryItem(rewardItemId) && m_i32InventoryCount < MAX_INVEN_COUNT)
	{
		GameInventoryItem rewardItem;
		rewardItem.ui32ItemDBIdx = (uint32_t)(GetTickCount() & 0xFFFFFF) | ((uint32_t)m_i32InventoryCount << 24);
		rewardItem.ui32ItemId = rewardItemId;
		rewardItem.ui8ItemType = ITEM_ATTR_UNUSED;	// Permanent
		rewardItem.ui32ItemArg = 0;
		rewardItem.ui8Durability = DURABILITY_MAX;
		AddInventoryItem(rewardItem);
		grantedDBIdx = rewardItem.ui32ItemDBIdx;
	}

	// Send rank up notification with reward info
	i3NetworkPacket packet;
	char buffer[48];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_RANK_UP_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);

	// Bonus GP and reward item
	memcpy(buffer + offset, &totalBonusGP, sizeof(int));	offset += sizeof(int);
	memcpy(buffer + offset, &rewardItemId, 4);				offset += 4;

	// Updated GP balance
	uint32_t gpBalance = (uint32_t)m_i32GP;
	memcpy(buffer + offset, &gpBalance, 4);					offset += 4;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	printf("[GameSession] Rank up! UID=%lld, %d -> %d, BonusGP=%d, RewardItem=%u\n",
		m_i64UID, oldRank, newRank, totalBonusGP, rewardItemId);
}

// ============================================================================
// Phase 4C - User Info Handlers
// ============================================================================

void GameSession::OnGetMyInfoRecordReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_MYINFO_RECORD_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// K/D/Headshot/Win/Loss stats
	memcpy(buffer + offset, &m_i32Kills, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Deaths, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Headshots, sizeof(int));	offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Wins, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Losses, sizeof(int));		offset += sizeof(int);

	// Total matches
	int totalMatches = m_i32Wins + m_i32Losses;
	memcpy(buffer + offset, &totalMatches, sizeof(int));	offset += sizeof(int);

	// K/D ratio * 100 (integer percentage)
	int kdRatio = (m_i32Deaths > 0) ? (m_i32Kills * 100 / m_i32Deaths) : (m_i32Kills * 100);
	memcpy(buffer + offset, &kdRatio, sizeof(int));			offset += sizeof(int);

	// Headshot rate * 100
	int hsRate = (m_i32Kills > 0) ? (m_i32Headshots * 100 / m_i32Kills) : 0;
	memcpy(buffer + offset, &hsRate, sizeof(int));			offset += sizeof(int);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetMyInfoBasicReq(char* pData, INT32 i32Size)
{
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_MYINFO_BASIC_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &m_i32Level, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i64Exp, sizeof(int64_t));	offset += sizeof(int64_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32GP, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32ClanId, sizeof(int));		offset += sizeof(int);

	// Title slots
	for (int i = 0; i < MAX_EQUIPPED_TITLES; i++)
		memcpy(buffer + offset, &m_TitleData.ui8EquippedSlots[i], 1), offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetMyInfoAllReq(char* pData, INT32 i32Size)
{
	// Combines basic + record in one response
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GET_MYINFO_ALL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Basic info
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	memcpy(buffer + offset, &m_i32Level, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i64Exp, sizeof(int64_t));	offset += sizeof(int64_t);
	memcpy(buffer + offset, &m_i32Cash, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32GP, sizeof(int));			offset += sizeof(int);
	memcpy(buffer + offset, &m_i32RankId, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32ClanId, sizeof(int));		offset += sizeof(int);

	// Record
	memcpy(buffer + offset, &m_i32Kills, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Deaths, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Headshots, sizeof(int));	offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Wins, sizeof(int));		offset += sizeof(int);
	memcpy(buffer + offset, &m_i32Losses, sizeof(int));		offset += sizeof(int);

	// Title slots
	for (int i = 0; i < MAX_EQUIPPED_TITLES; i++)
		memcpy(buffer + offset, &m_TitleData.ui8EquippedSlots[i], 1), offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetRecordInfoDBReq(char* pData, INT32 i32Size)
{
	// Same as MyInfoRecord but fetched from DB (for now identical)
	OnGetMyInfoRecordReq(pData, i32Size);
}

// ============================================================================
// Phase 4D - Title System Handlers
// ============================================================================

void GameSession::OnTitleEquipReq(char* pData, INT32 i32Size)
{
	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK, 1);
		return;
	}

	uint8_t titleId = *(uint8_t*)pData;
	uint8_t slotIdx = *(uint8_t*)(pData + 1);

	if (titleId == 0 || titleId > MAX_TITLE_COUNT || slotIdx >= MAX_EQUIPPED_TITLES)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK, 2);	// Invalid
		return;
	}

	if (!m_TitleData.ui8OwnedTitles[titleId - 1])
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK, 3);	// Not owned
		return;
	}

	// Check if title is already equipped in another slot
	for (int i = 0; i < MAX_EQUIPPED_TITLES; i++)
	{
		if (m_TitleData.ui8EquippedSlots[i] == titleId)
		{
			SendSimpleAck(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK, 4);	// Already equipped
			return;
		}
	}

	m_TitleData.ui8EquippedSlots[slotIdx] = titleId;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_USER_TITLE_EQUIP_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &titleId, 1);				offset += 1;
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnTitleReleaseReq(char* pData, INT32 i32Size)
{
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_RELEASE_ACK, 1);
		return;
	}

	uint8_t slotIdx = *(uint8_t*)pData;
	if (slotIdx >= MAX_EQUIPPED_TITLES)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_RELEASE_ACK, 2);
		return;
	}

	m_TitleData.ui8EquippedSlots[slotIdx] = 0;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_USER_TITLE_RELEASE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnTitleChangeReq(char* pData, INT32 i32Size)
{
	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_CHANGE_ACK, 1);
		return;
	}

	uint8_t slotIdx = *(uint8_t*)pData;
	uint8_t newTitleId = *(uint8_t*)(pData + 1);

	if (slotIdx >= MAX_EQUIPPED_TITLES)
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_CHANGE_ACK, 2);
		return;
	}

	if (newTitleId > 0 && (newTitleId > MAX_TITLE_COUNT || !m_TitleData.ui8OwnedTitles[newTitleId - 1]))
	{
		SendSimpleAck(PROTOCOL_BASE_USER_TITLE_CHANGE_ACK, 3);
		return;
	}

	m_TitleData.ui8EquippedSlots[slotIdx] = newTitleId;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_USER_TITLE_CHANGE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &slotIdx, 1);				offset += 1;
	memcpy(buffer + offset, &newTitleId, 1);			offset += 1;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// Send title info to client (Phase 4D)
void GameSession::SendTitleInfo()
{
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_USER_TITLE_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Owned title count
	uint8_t ownedCount = (uint8_t)m_TitleData.i32OwnedCount;
	memcpy(buffer + offset, &ownedCount, 1);				offset += 1;

	// Owned title bitmask (MAX_TITLE_COUNT bytes)
	memcpy(buffer + offset, m_TitleData.ui8OwnedTitles, MAX_TITLE_COUNT);
	offset += MAX_TITLE_COUNT;

	// Equipped slots
	memcpy(buffer + offset, m_TitleData.ui8EquippedSlots, MAX_EQUIPPED_TITLES);
	offset += MAX_EQUIPPED_TITLES;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Phase 9B - GameGuard Stub
// ============================================================================

void GameSession::OnGameGuardReq(char* pData, INT32 i32Size)
{
	// GameGuard stub - respond with dummy valid data so client doesn't disconnect
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_GAMEGUARD_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Echo back the client's GameGuard data (16 bytes typically)
	int copySize = min(i32Size, 32);
	if (copySize > 0)
		memcpy(buffer + offset, pData, copySize);
	else
		memset(buffer + offset, 0, 16);
	offset += max(copySize, 16);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Base Extended Handlers (Batch 15)
// ============================================================================

void GameSession::OnLogoutReq(char* pData, INT32 i32Size)
{
	// Client requests logout (back to server select or exit)
	if (m_eMainTask < GAME_TASK_LOGIN)
		return;

	printf("[GameSession] Logout - Index=%d, User=%s, UID=%lld\n",
		GetIndex(), m_szUsername, m_i64UID);

	// Save player data before logout
	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected() && m_i64UID > 0)
	{
		g_pModuleDataServer->RequestPlayerSave(m_i64UID,
			m_i32Level, m_i64Exp, m_i32Cash, m_i32GP);
	}

	SendSimpleAck(PROTOCOL_BASE_LOGOUT_ACK, 0);

	// Reset session state (will disconnect or go back to connect state)
	// Don't force disconnect - let client decide
}

void GameSession::OnQuestBuyCardSetReq(char* pData, INT32 i32Size)
{
	// Buy a quest card set
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK, -1);
		return;
	}

	uint8_t setIndex = *(uint8_t*)pData;
	uint8_t setType = *(uint8_t*)(pData + 1);

	// Check if slot is available (max 4 active quest sets)
	if (setIndex >= 4)
	{
		SendSimpleAck(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK, -2);
		return;
	}

	// Check cost (quest card sets cost GP)
	const int QUEST_CARD_SET_COST = 1000;
	if (m_i32GP < QUEST_CARD_SET_COST)
	{
		SendSimpleAck(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK, -3);
		return;
	}

	m_i32GP -= QUEST_CARD_SET_COST;

	// Initialize the quest set
	if (setIndex < 4)
	{
		m_QuestData.sets[setIndex].ui8SetType = setType;
		m_QuestData.sets[setIndex].ui8ActiveCard = 0;
		memset(m_QuestData.sets[setIndex].ui16Progress, 0, sizeof(m_QuestData.sets[setIndex].ui16Progress));
	}

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	memcpy(buffer + offset, &setIndex, 1);	offset += 1;
	memcpy(buffer + offset, &setType, 1);	offset += 1;

	int32_t remainGP = m_i32GP;
	memcpy(buffer + offset, &remainGP, 4);	offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGuideCompleteReq(char* pData, INT32 i32Size)
{
	// Tutorial/guide completion
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	// Just ACK - mark guide as completed
	SendSimpleAck(PROTOCOL_BASE_GUIDE_COMPLETE_ACK, 0);
}

void GameSession::OnGetUserInfoByUIDReq(char* pData, INT32 i32Size)
{
	// Get brief user info by UID
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < (int)sizeof(int64_t))
		return;

	int64_t targetUID = *(int64_t*)pData;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = -1;
	GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);

	if (pTarget)
	{
		result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int64_t uid = pTarget->GetUID();
		memcpy(buffer + offset, &uid, 8);	offset += 8;

		const char* nick = pTarget->GetNickname();
		int nickLen = (int)strlen(nick);
		if (nickLen > 32) nickLen = 32;
		memcpy(buffer + offset, nick, nickLen);
		memset(buffer + offset + nickLen, 0, 32 - nickLen);
		offset += 32;

		int32_t level = pTarget->GetLevel();
		int32_t rank = pTarget->GetRankId();
		int32_t kills = pTarget->GetKills();
		int32_t deaths = pTarget->GetDeaths();
		int32_t wins = pTarget->GetWins();
		int32_t losses = pTarget->GetLosses();
		memcpy(buffer + offset, &level, 4);		offset += 4;
		memcpy(buffer + offset, &rank, 4);		offset += 4;
		memcpy(buffer + offset, &kills, 4);		offset += 4;
		memcpy(buffer + offset, &deaths, 4);	offset += 4;
		memcpy(buffer + offset, &wins, 4);		offset += 4;
		memcpy(buffer + offset, &losses, 4);	offset += 4;
	}
	else
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetUserInfoByNickReq(char* pData, INT32 i32Size)
{
	// Get brief user info by nickname
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
		return;

	char nickname[33] = {};
	int copyLen = (i32Size > 32) ? 32 : i32Size;
	memcpy(nickname, pData, copyLen);
	nickname[32] = '\0';

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = -1;
	GameSession* pTarget = g_pGameSessionManager->FindSessionByNickname(nickname);

	if (pTarget)
	{
		result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

		int64_t uid = pTarget->GetUID();
		memcpy(buffer + offset, &uid, 8);	offset += 8;

		const char* nick = pTarget->GetNickname();
		int nickLen = (int)strlen(nick);
		if (nickLen > 32) nickLen = 32;
		memcpy(buffer + offset, nick, nickLen);
		memset(buffer + offset + nickLen, 0, 32 - nickLen);
		offset += 32;

		int32_t level = pTarget->GetLevel();
		int32_t rank = pTarget->GetRankId();
		int32_t kills = pTarget->GetKills();
		int32_t deaths = pTarget->GetDeaths();
		int32_t wins = pTarget->GetWins();
		int32_t losses = pTarget->GetLosses();
		memcpy(buffer + offset, &level, 4);		offset += 4;
		memcpy(buffer + offset, &rank, 4);		offset += 4;
		memcpy(buffer + offset, &kills, 4);		offset += 4;
		memcpy(buffer + offset, &deaths, 4);	offset += 4;
		memcpy(buffer + offset, &wins, 4);		offset += 4;
		memcpy(buffer + offset, &losses, 4);	offset += 4;
	}
	else
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Quest Card Set Delete (Protocol_Base)
// ============================================================================

void GameSession::OnQuestDeleteCardSetReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK, -1);
		return;
	}

	uint8_t setIndex = *(uint8_t*)pData;
	int32_t result = 0;

	if (setIndex >= 4)
	{
		result = -2;
	}
	else
	{
		// Clear the quest set
		m_QuestData.sets[setIndex].ui8SetType = 0;
		m_QuestData.sets[setIndex].ui8ActiveCard = 0;
		memset(m_QuestData.sets[setIndex].ui16Progress, 0,
			sizeof(m_QuestData.sets[setIndex].ui16Progress));
	}

	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &setIndex, 1);				offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Record Info (Protocol_Base - season/session/db stats)
// ============================================================================

void GameSession::OnGetRecordInfoSeasonReq(char* pData, INT32 i32Size)
{
	// Season record info - cumulative stats for current season
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	int64_t targetUID = m_i64UID;
	if (i32Size >= 8)
		memcpy(&targetUID, pData, 8);

	// Find player (could be self or another player)
	GameSession* pTarget = this;
	if (targetUID != m_i64UID)
		pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_RECORD_INFO_SEASON_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	if (!pTarget)
	{
		int32_t result = -1;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
		memcpy(buffer + offset, &targetUID, 8);				offset += 8;

		int32_t kills = pTarget->GetKills();
		int32_t deaths = pTarget->GetDeaths();
		int32_t headshots = pTarget->GetHeadshots();
		int32_t wins = pTarget->GetWins();
		int32_t losses = pTarget->GetLosses();
		int32_t level = pTarget->GetLevel();
		int32_t rank = pTarget->GetRankId();
		int64_t exp = pTarget->GetExp();

		memcpy(buffer + offset, &kills, 4);		offset += 4;
		memcpy(buffer + offset, &deaths, 4);	offset += 4;
		memcpy(buffer + offset, &headshots, 4);	offset += 4;
		memcpy(buffer + offset, &wins, 4);		offset += 4;
		memcpy(buffer + offset, &losses, 4);	offset += 4;
		memcpy(buffer + offset, &level, 4);		offset += 4;
		memcpy(buffer + offset, &rank, 4);		offset += 4;
		memcpy(buffer + offset, &exp, 8);		offset += 8;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetRecordInfoSessionReq(char* pData, INT32 i32Size)
{
	// Session record info - stats for current play session (since login)
	if (m_eMainTask < GAME_TASK_INFO)
		return;

	i3NetworkPacket packet;
	char buffer[96];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Daily record tracks session stats
	memcpy(buffer + offset, &m_DailyRecord.i32Kills, 4);		offset += 4;
	memcpy(buffer + offset, &m_DailyRecord.i32Deaths, 4);		offset += 4;
	memcpy(buffer + offset, &m_DailyRecord.i32GamesPlayed, 4);	offset += 4;
	memcpy(buffer + offset, &m_DailyRecord.i32Wins, 4);		offset += 4;
	memcpy(buffer + offset, &m_DailyRecord.i32Losses, 4);		offset += 4;
	memcpy(buffer + offset, &m_DailyRecord.i64ExpGained, 8);	offset += 8;
	memcpy(buffer + offset, &m_DailyRecord.i32GPGained, 4);	offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetRecordInfoDBReq(char* pData, INT32 i32Size)
{
	// DB record info - all-time stats (same as season for now)
	OnGetRecordInfoSeasonReq(pData, i32Size);
}

// ============================================================================
// Player Data Load (from DataServer)
// ============================================================================

void GameSession::OnPlayerDataLoaded(const char* pPayload, int i32PayloadSize)
{
	if (!pPayload || i32PayloadSize < (int)sizeof(IS_PLAYER_LOAD_DATA))
		return;

	const IS_PLAYER_LOAD_DATA* pData = (const IS_PLAYER_LOAD_DATA*)pPayload;

	m_i64UID = pData->i64UID;
	strncpy_s(m_szNickname, pData->szNickname, _TRUNCATE);
	m_i32Level = pData->i32Level;
	m_i64Exp = pData->i64Exp;
	m_i32Cash = pData->i32Cash;
	m_i32GP = pData->i32GP;
	m_i32RankId = pData->i32RankId;
	m_i32ClanId = pData->i32ClanId;
	if (m_i32ClanId > 0 && g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (pClan)
			strncpy_s(m_szClanName, pClan->szName, _TRUNCATE);
	}

	m_i32Kills = pData->i32Kills;
	m_i32Deaths = pData->i32Deaths;
	m_i32Headshots = pData->i32Headshots;
	m_i32Wins = pData->i32Wins;
	m_i32Losses = pData->i32Losses;

	m_i32InventoryCount = pData->i32InventoryCount;
	if (m_i32InventoryCount > MAX_INVEN_COUNT)
		m_i32InventoryCount = MAX_INVEN_COUNT;

	const IS_PLAYER_INVENTORY_ITEM* pItems =
		(const IS_PLAYER_INVENTORY_ITEM*)(pPayload + sizeof(IS_PLAYER_LOAD_DATA));

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		m_Inventory[i].ui32ItemDBIdx = (uint32_t)(i + 1);
		m_Inventory[i].ui32ItemId = (uint32_t)pItems[i].i32ItemId;
		m_Inventory[i].ui8ItemType = pItems[i].ui8IsEquipped ? ITEM_ATTR_UNUSED : ITEM_ATTR_UNUSED;
		m_Inventory[i].ui32ItemArg = (uint32_t)pItems[i].i32ItemCount;
	}

	// Initialize default equipment if no character data loaded
	if (m_CharaSlots[0].ui32CharaId == 0)
	{
		InitDefaultPlayerData();
		// Keep loaded stats/level/exp/gp, just ensure defaults for equipment
	}

	m_eMainTask = GAME_TASK_INFO;
	SendLoginAck(0);

	// Load friend, block, and note lists from DataServer
	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
	{
		g_pModuleDataServer->RequestFriendList(m_i64UID, GetIndex());
		g_pModuleDataServer->RequestBlockList(m_i64UID, GetIndex());
		g_pModuleDataServer->RequestNoteList(m_i64UID, GetIndex());
	}

	printf("[GameSession] Player data loaded - UID=%lld, Nick=%s, Level=%d, Items=%d\n",
		m_i64UID, m_szNickname, m_i32Level, m_i32InventoryCount);
}

void GameSession::InitDefaultPlayerData()
{
	strncpy_s(m_szNickname, m_szUsername, _TRUNCATE);
	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32GP = g_pContextMain ? g_pContextMain->m_i32StartingGP : 10000;
	m_i32Cash = g_pContextMain ? g_pContextMain->m_i32StartingCash : 0;

	// Initialize default character slot 0 with default equipment
	m_ui8ActiveCharaSlot = 0;
	m_CharaSlots[0].ui8State = MULTI_SLOT_NORMAL;
	m_CharaSlots[0].ui32CharaId = MAKE_ITEM_ID(ITEM_TYPE_CHARA, 1, 1);	// Default RED team char
	m_CharaSlots[0].equip.ui32WeaponIds[EQUIP_WEAPON_PRIMARY]		= MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, 4);		// K-2
	m_CharaSlots[0].equip.ui32WeaponIds[EQUIP_WEAPON_SECONDARY]	= MAKE_ITEM_ID(ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, 3);	// K5
	m_CharaSlots[0].equip.ui32WeaponIds[EQUIP_WEAPON_MELEE]		= MAKE_ITEM_ID(ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, 1);			// M7
	m_CharaSlots[0].equip.ui32PartsIds[EQUIP_PARTS_CHARA] = m_CharaSlots[0].ui32CharaId;

	// Initialize tutorial quest cardset
	m_QuestData.InitTutorial();

	// Initialize default medals and skills
	m_MedalData.InitDefaults();
	m_SkillData.Reset();
}

// ============================================================================
// DataServer Callback Results
// ============================================================================

void GameSession::OnCreateNickResult(int i32Result, const char* pszNickname)
{
	if (i32Result == 0 && pszNickname)
	{
		strncpy_s(m_szNickname, pszNickname, _TRUNCATE);
		printf("[GameSession] Nickname created - UID=%lld, Nick=%s\n", m_i64UID, m_szNickname);
	}

	// Send result to client
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_CREATE_NICK_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (i32Result == 0 && pszNickname)
		memcpy(buffer + offset, pszNickname, 64);
	else
		memset(buffer + offset, 0, 64);
	offset += 64;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnCheckNickResult(int i32Result)
{
	SendSimpleAck(PROTOCOL_BASE_CHECK_NICK_NAME_ACK, i32Result);
}

void GameSession::OnClanCreateResult(int i32ClanId, int i32Result)
{
	if (i32Result == 0 && i32ClanId > 0)
		m_i32ClanId = i32ClanId;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_CS_CREATE_CLAN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &i32ClanId, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnClanJoinResult(int i32ClanId, int i32Result)
{
	if (i32Result == 0 && i32ClanId > 0)
		m_i32ClanId = i32ClanId;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_CS_CLIENT_ENTER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &i32ClanId, sizeof(int32_t));	offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnFriendAddResult(int64_t i64FriendUID, int i32Result)
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_FRIEND_INSERT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &i64FriendUID, sizeof(int64_t));	offset += sizeof(int64_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnFriendListLoaded(IS_FRIEND_ENTRY* pEntries, int i32Count)
{
	m_i32FriendCount = 0;

	for (int i = 0; i < i32Count && i < MAX_FRIEND_COUNT; i++)
	{
		m_Friends[i].i64UID = pEntries[i].i64FriendUID;
		strncpy_s(m_Friends[i].szNickname, pEntries[i].szNickname, _TRUNCATE);
		m_Friends[i].i32Level = pEntries[i].i32Level;
		m_i32FriendCount++;
	}

	printf("[GameSession] Friend list loaded - UID=%lld, Count=%d\n", m_i64UID, m_i32FriendCount);
}

void GameSession::OnBlockAddResult(int64_t i64BlockedUID, int i32Result)
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_AUTH_BLOCK_INSERT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &i64BlockedUID, sizeof(int64_t));	offset += sizeof(int64_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBlockListLoaded(IS_BLOCK_ENTRY* pEntries, int i32Count)
{
	m_i32BlockCount = 0;

	for (int i = 0; i < i32Count && i < MAX_BLOCK_COUNT; i++)
	{
		m_BlockList[i].i64UID = pEntries[i].i64BlockedUID;
		strncpy_s(m_BlockList[i].szNickname, pEntries[i].szNickname, _TRUNCATE);
		m_i32BlockCount++;
	}

	printf("[GameSession] Block list loaded - UID=%lld, Count=%d\n", m_i64UID, m_i32BlockCount);
}

void GameSession::OnNoteListLoaded(IS_NOTE_ENTRY* pEntries, int i32Count)
{
	m_i32NoteCount = 0;

	for (int i = 0; i < i32Count && i < MAX_NOTE_COUNT; i++)
	{
		GameNoteData& note = m_Notes[m_i32NoteCount];
		note.ui32NoteId = (uint32_t)pEntries[i].i64NoteId;
		note.i64SenderUID = pEntries[i].i64SenderUID;
		strncpy_s(note.szSenderNick, pEntries[i].szSenderNick, _TRUNCATE);
		strncpy_s(note.szSubject, pEntries[i].szSubject, NOTE_SUBJECT_LEN - 1);
		strncpy_s(note.szBody, pEntries[i].szBody, NOTE_BODY_LEN - 1);
		note.dwTimestamp = pEntries[i].ui32Timestamp;
		note.ui8Type = pEntries[i].ui8Type;
		note.bRead = (pEntries[i].ui8Read != 0);
		m_i32NoteCount++;
	}

	printf("[GameSession] Note list loaded - UID=%lld, Count=%d\n", m_i64UID, m_i32NoteCount);
}

void GameSession::ApplyBattleResult(int i32Kills, int i32Deaths, int i32Headshots, bool bWin)
{
	m_i32Kills += i32Kills;
	m_i32Deaths += i32Deaths;
	m_i32Headshots += i32Headshots;

	if (bWin)
		m_i32Wins++;
	else
		m_i32Losses++;

	// Use config-driven rewards
	int killGP = 50, winGP = 200, loseGP = 50;
	int killExp = 100, winExp = 500, loseExp = 100;
	int headshotGP = 20, headshotExp = 50;
	if (g_pContextMain)
	{
		killGP = g_pContextMain->m_i32KillGPReward;
		winGP = g_pContextMain->m_i32WinGPReward;
		loseGP = g_pContextMain->m_i32LoseGPReward;
		killExp = g_pContextMain->m_i32KillExpReward;
		winExp = g_pContextMain->m_i32WinExpReward;
		loseExp = g_pContextMain->m_i32LoseExpReward;
	}

	// Phase 1C: SLOT_BONUS breakdown tracking
	int baseGP = bWin ? winGP : loseGP;
	int killBonusGP = i32Kills * killGP;
	int headshotBonusGP = i32Headshots * headshotGP;
	int64_t baseExp = (int64_t)(bWin ? winExp : loseExp);
	int64_t killBonusExp = (int64_t)(i32Kills * killExp);
	int64_t headshotBonusExp = (int64_t)(i32Headshots * headshotExp);

	int gpReward = baseGP + killBonusGP + headshotBonusGP;
	int64_t expReward = baseExp + killBonusExp + headshotBonusExp;

	// Apply boost multipliers: max of (server event boost, inventory boost)
	int boostGP = 0, boostExp = 0;
	{
		// Server-wide event multiplier (Phase 14B)
		uint16_t eventExpMult = g_pContextMain ? g_pContextMain->GetCurrentExpMultiplier() : 100;
		uint16_t eventPtMult = g_pContextMain ? g_pContextMain->GetCurrentPointMultiplier() : 100;

		// Per-player inventory boost item multiplier (Phase 10A)
		uint16_t itemExpMult = GetInventoryExpMultiplier();
		uint16_t itemPtMult = GetInventoryPointMultiplier();

		// Use the higher of event or inventory boost (they don't stack)
		uint16_t expMult = (eventExpMult > itemExpMult) ? eventExpMult : itemExpMult;
		uint16_t pointMult = (eventPtMult > itemPtMult) ? eventPtMult : itemPtMult;

		if (expMult != 100)
		{
			int64_t boosted = expReward * expMult / 100;
			boostExp = (int)(boosted - expReward);
			expReward = boosted;
		}
		if (pointMult != 100)
		{
			int boosted = gpReward * pointMult / 100;
			boostGP = boosted - gpReward;
			gpReward = boosted;
		}
	}

	// Store SLOT_BONUS breakdown for client display (Phase 1C)
	m_i32LastBattleExpBonus = (int)expReward;
	m_i32LastBattleGPBonus = gpReward;

	m_i32GP += gpReward;
	m_i64Exp += expReward;

	// Update daily record
	m_DailyRecord.i32Kills += i32Kills;
	m_DailyRecord.i32Deaths += i32Deaths;
	m_DailyRecord.i32GamesPlayed++;
	if (bWin) m_DailyRecord.i32Wins++; else m_DailyRecord.i32Losses++;
	m_DailyRecord.i64ExpGained += expReward;
	m_DailyRecord.i32GPGained += gpReward;

	printf("[GameSession] Battle result - UID=%lld, K=%d D=%d H=%d Win=%d, GP+%d(base=%d kill=%d hs=%d boost=%d), EXP+%lld\n",
		m_i64UID, i32Kills, i32Deaths, i32Headshots, bWin ? 1 : 0,
		gpReward, baseGP, killBonusGP, headshotBonusGP, boostGP, expReward);

	// Check for rank up after EXP gain
	CheckRankUp();

	// Decrease durability of equipped items (Phase 14C)
	DecreaseEquippedDurability();

	// Update quest progress (Phase 7I)
	UpdateQuestProgress(i32Kills, i32Deaths, i32Headshots, bWin);

	// Award clan EXP from battle results
	if (m_i32ClanId > 0 && g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (pClan)
		{
			// Clan EXP: 10 per kill + 50 for win + 5 per headshot
			int clanExp = i32Kills * 10 + i32Headshots * 5 + (bWin ? 50 : 10);
			pClan->i32ClanExp += clanExp;

			if (bWin)
				pClan->i32Wins++;
			else
				pClan->i32Losses++;

			// Check clan rank up (every 1000 EXP = 1 rank, max 49)
			int newRank = pClan->i32ClanExp / 1000;
			if (newRank > 49) newRank = 49;
			if (newRank > pClan->i32ClanRank)
			{
				pClan->i32ClanRank = newRank;
				printf("[Clan] Clan '%s' ranked up to %d (EXP=%d)\n",
					pClan->szName, newRank, pClan->i32ClanExp);
			}

			// Check unit size upgrade based on rank
			uint8_t newUnit = CLAN_UNIT_SQUAD;
			if (pClan->i32ClanRank >= 40) newUnit = CLAN_UNIT_CORPS;
			else if (pClan->i32ClanRank >= 35) newUnit = CLAN_UNIT_DIVISION;
			else if (pClan->i32ClanRank >= 30) newUnit = CLAN_UNIT_BRIGADE;
			else if (pClan->i32ClanRank >= 25) newUnit = CLAN_UNIT_REGIMENT;
			else if (pClan->i32ClanRank >= 20) newUnit = CLAN_UNIT_BATTALION;
			else if (pClan->i32ClanRank >= 15) newUnit = CLAN_UNIT_COMPANY;
			else if (pClan->i32ClanRank >= 10) newUnit = CLAN_UNIT_PLATOON;

			if (newUnit > pClan->ui8Unit)
			{
				pClan->ui8Unit = newUnit;
				// Increase max members based on unit
				static const int unitMaxMembers[] = { 10, 30, 50, 100, 150, 200, 250, 250 };
				pClan->i32MaxMembers = unitMaxMembers[newUnit];
				printf("[Clan] Clan '%s' unit upgraded to %d, max members=%d\n",
					pClan->szName, newUnit, pClan->i32MaxMembers);
			}
		}
	}
}

// ============================================================================
// Inventory Boost Items (Phase 10A)
// Scan inventory for active ITEM_TYPE_MAINTENANCE boost items
// ============================================================================

uint16_t GameSession::GetInventoryExpMultiplier() const
{
	uint16_t maxMult = 100;
	DWORD dwNow = (DWORD)time(nullptr);

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		const GameInventoryItem& item = m_Inventory[i];
		if (!item.IsValid())
			continue;

		int itemType = GET_ITEM_TYPE(item.ui32ItemId);
		if (itemType != ITEM_TYPE_MAINTENANCE)
			continue;

		// Period-based items: check expiration
		if (item.ui8ItemType == ITEM_ATTR_PERIOD && item.ui32ItemArg > 0)
		{
			if (dwNow > item.ui32ItemArg)
				continue;	// Expired
		}

		int groupType = GET_ITEM_NUMBER(item.ui32ItemId);
		uint16_t mult = GetExpMultiplierFromCashGroup(groupType);
		if (mult > maxMult)
			maxMult = mult;
	}
	return maxMult;
}

uint16_t GameSession::GetInventoryPointMultiplier() const
{
	uint16_t maxMult = 100;
	DWORD dwNow = (DWORD)time(nullptr);

	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		const GameInventoryItem& item = m_Inventory[i];
		if (!item.IsValid())
			continue;

		int itemType = GET_ITEM_TYPE(item.ui32ItemId);
		if (itemType != ITEM_TYPE_MAINTENANCE)
			continue;

		// Period-based items: check expiration
		if (item.ui8ItemType == ITEM_ATTR_PERIOD && item.ui32ItemArg > 0)
		{
			if (dwNow > item.ui32ItemArg)
				continue;	// Expired
		}

		int groupType = GET_ITEM_NUMBER(item.ui32ItemId);
		uint16_t mult = GetPointMultiplierFromCashGroup(groupType);
		if (mult > maxMult)
			maxMult = mult;
	}
	return maxMult;
}

uint32_t GameSession::GetColorNick() const
{
	// Check override first (set by OnChangeColorNickReq)
	if (m_ui32ColorNick > 0)
		return m_ui32ColorNick;

	// Scan inventory for active color nick maintenance item
	DWORD dwNow = (DWORD)time(nullptr);
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		const GameInventoryItem& item = m_Inventory[i];
		if (!item.IsValid())
			continue;

		int itemType = GET_ITEM_TYPE(item.ui32ItemId);
		if (itemType != ITEM_TYPE_MAINTENANCE)
			continue;

		int groupType = GET_ITEM_NUMBER(item.ui32ItemId);
		if (groupType != CASHITEM_GROUP_COLOR_NICK)
			continue;

		// Period-based: check expiration
		if (item.ui8ItemType == ITEM_ATTR_PERIOD && item.ui32ItemArg > 0)
		{
			if (dwNow > item.ui32ItemArg)
				continue;
		}

		// Color value encoded in subtype
		return (uint32_t)GET_ITEM_SUBTYPE(item.ui32ItemId);
	}

	return 0;
}

// ============================================================================
// Timeout & Reset
// ============================================================================

bool GameSession::IsTimedOut() const
{
	DWORD dwNow = GetTickCount();
	DWORD dwElapsed = (dwNow - m_dwLastPacketTime) / 1000;

	switch (m_eMainTask)
	{
	case GAME_TASK_NONE:
	case GAME_TASK_CONNECT:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_NOT_LOGIN);
	case GAME_TASK_LOGIN_ING:
	case GAME_TASK_LOGIN:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_LOGIN);
	case GAME_TASK_CHANNEL:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_CHANNEL);
	default:
		return (dwElapsed > SESSION_CHECK_TIMEOUT_NORMAL);
	}
}

void GameSession::ResetSessionData()
{
	m_eMainTask = GAME_TASK_NONE;
	m_i64UID = 0;
	m_szUsername[0] = '\0';
	m_szNickname[0] = '\0';
	m_ui32EncriptKey = 0;
	m_ui32AuthToken = 0;

	m_i32ChannelNum = -1;
	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_pRoom = nullptr;

	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32Cash = 0;
	m_i32GP = 0;
	m_i32RankId = 0;
	m_i32ClanId = 0;
	m_szClanName[0] = '\0';
	m_ui8AuthLevel = 0;
	m_bDamageConsole = false;

	m_i32Kills = 0;
	m_i32Deaths = 0;
	m_i32Headshots = 0;
	m_i32Wins = 0;
	m_i32Losses = 0;
	m_ui32ColorNick = 0;
	m_stUsedWeapon = 0;
	m_i32LastBattleExpBonus = 0;
	m_i32LastBattleGPBonus = 0;

	m_dwRateLimitWindow = 0;
	m_ui16PacketCount = 0;
	m_i32ReplayIdx = 0;
	m_i32ReplayDetections = 0;
	memset(m_ReplayHistory, 0, sizeof(m_ReplayHistory));

	m_ui8ActiveCharaSlot = 0;
	for (int i = 0; i < MAX_CHARA_SLOT; i++)
		m_CharaSlots[i].Reset();

	m_i32InventoryCount = 0;
	memset(m_Inventory, 0, sizeof(m_Inventory));

	m_QuestData.Reset();

	m_i32FriendCount = 0;
	for (int i = 0; i < MAX_FRIEND_COUNT; i++)
		m_Friends[i].Reset();
	m_i32BlockCount = 0;
	for (int i = 0; i < MAX_BLOCK_COUNT; i++)
		m_BlockList[i].Reset();

	m_i32NoteCount = 0;
	m_ui32NextNoteId = 1;
	for (int i = 0; i < MAX_NOTE_COUNT; i++)
		m_Notes[i].Reset();

	m_MedalData.Reset();
	m_AttendanceData.Reset();
	m_DailyRecord.Reset();
	m_SkillData.Reset();
	m_TitleData.Reset();

	m_dwConnectTime = 0;
	m_dwLastPacketTime = 0;

	// Cheat detection
	m_dwCheatLastClientTime = 0;
	m_dwCheatLastServerTime = 0;
	m_i32SpeedHackCount = 0;
	m_i32CheatViolations = 0;
	m_fLastPosX = 0.0f;
	m_fLastPosY = 0.0f;
	m_fLastPosZ = 0.0f;
}

// ============================================================================
// Inventory Helpers
// ============================================================================

GameInventoryItem* GameSession::FindInventoryItem(uint32_t itemId)
{
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemId == itemId)
			return &m_Inventory[i];
	}
	return nullptr;
}

GameInventoryItem* GameSession::FindInventoryItemByDBIdx(uint32_t dbIdx)
{
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemDBIdx == dbIdx)
			return &m_Inventory[i];
	}
	return nullptr;
}

bool GameSession::HasInventoryItem(uint32_t itemId) const
{
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemId == itemId)
			return true;
	}
	return false;
}

int GameSession::AddInventoryItem(const GameInventoryItem& item)
{
	if (m_i32InventoryCount >= MAX_INVEN_COUNT)
		return -1;

	m_Inventory[m_i32InventoryCount] = item;
	return m_i32InventoryCount++;
}

bool GameSession::RemoveInventoryItem(uint32_t dbIdx)
{
	for (int i = 0; i < m_i32InventoryCount; i++)
	{
		if (m_Inventory[i].ui32ItemDBIdx == dbIdx)
		{
			// Shift remaining items
			for (int j = i; j < m_i32InventoryCount - 1; j++)
				m_Inventory[j] = m_Inventory[j + 1];

			m_i32InventoryCount--;
			m_Inventory[m_i32InventoryCount].Reset();
			return true;
		}
	}
	return false;
}

// ============================================================================
// Send Helpers
// ============================================================================

void GameSession::SendConnectAck()
{
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_CONNECT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));				offset += sizeof(uint16_t);
	memcpy(buffer + offset, &m_ui32EncriptKey, sizeof(uint32_t));	offset += sizeof(uint32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::SendGamePacket(char* pBuffer, int i32Size)
{
	// Encrypt outgoing packet if encryption key is set (post-handshake)
	if (m_ui32EncriptKey > 0 && i32Size > 2)
	{
		// Encrypt bytes [2..size) - skip size field, encrypt protocol+payload
		BitRotateEncript((UINT8*)&pBuffer[2], i32Size - 2, m_ui32EncriptKey);
		// Set encryption flag (bit 0x8000) on size
		*(uint16_t*)pBuffer = (uint16_t)(i32Size | 0x8000);
	}

	i3NetworkPacket packet;
	packet.SetPacketData(pBuffer, i32Size);
	SendMessage(&packet);
}

void GameSession::SendHeartBitAck()
{
	i3NetworkPacket packet;
	char buffer[8];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_BASE_HEART_BIT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::SendLoginAck(int i32Result)
{
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_LOGIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &i32Result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (i32Result == 0)
	{
		memcpy(buffer + offset, &m_i64UID, sizeof(int64_t));	offset += sizeof(int64_t);
		memcpy(buffer + offset, m_szNickname, 64);				offset += 64;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Batch 17 - Base extras
// ============================================================================

void GameSession::OnBaseUrlListReq(char* pData, INT32 i32Size)
{
	// Return list of URLs (website, forum, support, etc.)
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_URL_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// URL count
	uint8_t urlCount = 0;
	memcpy(buffer + offset, &urlCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseServerInfoReq(char* pData, INT32 i32Size)
{
	// Return current server info (player count, channel count, etc.)
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_SERVER_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Server name
	char serverName[64] = {};
	if (g_pContextMain)
		strncpy(serverName, g_pContextMain->m_szServerName, 63);
	memcpy(buffer + offset, serverName, 64);	offset += 64;

	// Online count
	int32_t onlineCount = 0;
	if (g_pGameSessionManager)
		onlineCount = g_pGameSessionManager->GetActiveCount();
	memcpy(buffer + offset, &onlineCount, 4);	offset += 4;

	// Channel count
	uint8_t channelCount = 0;
	if (g_pContextMain)
		channelCount = g_pContextMain->m_ui8ChannelCount;
	memcpy(buffer + offset, &channelCount, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseClientEndTypeReq(char* pData, INT32 i32Size)
{
	// Client reports disconnect reason (fire-and-forget, no ACK needed)
	if (i32Size < (int)sizeof(uint8_t))
		return;

	uint8_t endType = *(uint8_t*)pData;
	printf("[GameSession] Client end type %d from Index=%d UID=%lld\n",
		endType, GetIndex(), m_i64UID);
}

void GameSession::OnBaseUserTitleInfoReq(char* pData, INT32 i32Size)
{
	// Return player's equipped title/designation
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_USER_TITLE_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Title ID (0 = none) - use first equipped slot
	uint16_t titleId = (uint16_t)m_TitleData.ui8EquippedSlots[0];
	memcpy(buffer + offset, &titleId, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseQuestFinishNoticeReq(char* pData, INT32 i32Size)
{
	// Client notifies server that quest completion notice was shown
	// Fire-and-forget, no meaningful ACK needed
	SendSimpleAck(PROTOCOL_BASE_QUEST_FINISH_NOTICE_ACK, 0);
}

void GameSession::OnBaseRegistTrainingScoreReq(char* pData, INT32 i32Size)
{
	// Register training/shooting range score
	if (i32Size < (int)(sizeof(int32_t) + sizeof(int32_t)))
	{
		SendSimpleAck(PROTOCOL_BASE_REGIST_TRAINING_SCORE_ACK, -1);
		return;
	}

	int offset = 0;
	int32_t mapIdx = 0;
	int32_t score = 0;
	memcpy(&mapIdx, pData + offset, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(&score, pData + offset, sizeof(int32_t));	offset += sizeof(int32_t);

	printf("[GameSession] Training score: map=%d, score=%d, UID=%lld\n",
		mapIdx, score, m_i64UID);

	SendSimpleAck(PROTOCOL_BASE_REGIST_TRAINING_SCORE_ACK, 0);
}

void GameSession::OnBaseGetUserListReq(char* pData, INT32 i32Size)
{
	// Get list of users in current channel/lobby
	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USER_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// User count (simplified - return 0 for now, full implementation needs channel user iteration)
	uint16_t userCount = 0;
	memcpy(buffer + offset, &userCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseChangeClanNameReq(char* pData, INT32 i32Size)
{
	// Change clan name via base protocol (fire-and-forget to DataServer)
	if (m_i32ClanId <= 0)
		return;

	if (i32Size < 4)
		return;

	if (!g_pClanManager)
		return;

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
		return;

	char newName[MAX_CLAN_NAME_LEN] = {};
	int nameLen = (i32Size > MAX_CLAN_NAME_LEN - 1) ? MAX_CLAN_NAME_LEN - 1 : i32Size;
	memcpy(newName, pData, nameLen);

	// Update in-memory
	strncpy(pClan->szName, newName, MAX_CLAN_NAME_LEN - 1);

	printf("[GameSession] Clan name changed to '%s' by UID=%lld\n", newName, m_i64UID);
}

void GameSession::OnBaseChangeClanMarkReq(char* pData, INT32 i32Size)
{
	// Change clan mark via base protocol (fire-and-forget to DataServer)
	if (m_i32ClanId <= 0)
		return;

	if (i32Size < (int)(sizeof(uint16_t) + sizeof(uint8_t)))
		return;

	if (!g_pClanManager)
		return;

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
		return;

	int offset = 0;
	uint16_t markId = 0;
	uint8_t markColor = 0;
	memcpy(&markId, pData + offset, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(&markColor, pData + offset, sizeof(uint8_t));

	pClan->ui16MarkId = markId;
	pClan->ui8MarkColor = markColor;

	printf("[GameSession] Clan mark changed: markId=%d, color=%d by UID=%lld\n",
		markId, markColor, m_i64UID);
}

// ============================================================================
// Batch 18 - Base extras
// ============================================================================

void GameSession::OnBaseUserEnterReq(char* pData, INT32 i32Size)
{
	// Server transfer: player enters from another server
	// In distributed architecture, this comes with an auth token
	SendSimpleAck(PROTOCOL_BASE_USER_ENTER_ACK, 0);
}

void GameSession::OnBaseUserLeaveReq(char* pData, INT32 i32Size)
{
	// Server transfer: player wants to leave to another server
	// Save data before allowing transfer
	if (m_i64UID > 0 && g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestPlayerSave(m_i64UID, m_i32Level,
			m_i64Exp, m_i32Cash, m_i32GP);
	}

	SendSimpleAck(PROTOCOL_BASE_USER_LEAVE_ACK, 0);
}

void GameSession::OnBaseGameServerStateReq(char* pData, INT32 i32Size)
{
	// Return current game server state (online count, channels, etc.)
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GAME_SERVER_STATE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Server state info
	int32_t onlineCount = g_pGameSessionManager ? g_pGameSessionManager->GetActiveCount() : 0;
	uint8_t channelCount = g_pContextMain ? g_pContextMain->m_ui8ChannelCount : 4;
	uint8_t serverState = 1;  // 1 = online

	memcpy(buffer + offset, &onlineCount, 4);	offset += 4;
	memcpy(buffer + offset, &channelCount, 1);	offset += 1;
	memcpy(buffer + offset, &serverState, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseReadyHeartBitReq(char* pData, INT32 i32Size)
{
	// Ready-state heartbeat (client is in loading/ready screen)
	SendSimpleAck(PROTOCOL_BASE_READY_HEART_BIT_ACK, 0);
}

void GameSession::OnBaseGetMyClanReq(char* pData, INT32 i32Size)
{
	// Get player's own clan info
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_MYCLAN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = (m_i32ClanId > 0) ? 0 : -1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (m_i32ClanId > 0 && g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (pClan)
		{
			memcpy(buffer + offset, &pClan->i32ClanId, 4);				offset += 4;
			memcpy(buffer + offset, pClan->szName, MAX_CLAN_NAME_LEN);	offset += MAX_CLAN_NAME_LEN;
			memcpy(buffer + offset, &pClan->i32MemberCount, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->i32ClanRank, 4);			offset += 4;
			memcpy(buffer + offset, &pClan->i32ClanExp, 4);			offset += 4;
			memcpy(buffer + offset, &pClan->ui16MarkId, 2);			offset += 2;
			memcpy(buffer + offset, &pClan->ui8MarkColor, 1);			offset += 1;
		}
		else
		{
			result = -1;
			memcpy(buffer + 4, &result, sizeof(int32_t));
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseGetUserInfoAllReq(char* pData, INT32 i32Size)
{
	// Get detailed user info (self or target)
	int64_t targetUID = m_i64UID;
	if (i32Size >= (int)sizeof(int64_t))
		memcpy(&targetUID, pData, sizeof(int64_t));

	GameSession* pTarget = (targetUID == m_i64UID) ? this :
		(g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr);

	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_BASE_GET_USERINFO_ALL_ACK, -1);
		return;
	}

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_USERINFO_ALL_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	int64_t uid = pTarget->GetUID();
	memcpy(buffer + offset, &uid, sizeof(int64_t));		offset += sizeof(int64_t);

	char nick[64] = {};
	strncpy(nick, pTarget->GetNickname(), 63);
	memcpy(buffer + offset, nick, 64);					offset += 64;

	int32_t level = pTarget->GetLevel();
	int32_t rankId = pTarget->GetRankId();
	int64_t exp = pTarget->m_i64Exp;
	int32_t gp = pTarget->m_i32GP;
	int32_t cash = pTarget->m_i32Cash;
	int32_t kills = pTarget->GetKills();
	int32_t deaths = pTarget->GetDeaths();
	int32_t headshots = pTarget->GetHeadshots();
	int32_t wins = pTarget->GetWins();
	int32_t losses = pTarget->GetLosses();
	int32_t clanId = pTarget->GetClanId();

	memcpy(buffer + offset, &level, 4);		offset += 4;
	memcpy(buffer + offset, &rankId, 4);	offset += 4;
	memcpy(buffer + offset, &exp, 8);		offset += 8;
	memcpy(buffer + offset, &gp, 4);		offset += 4;
	memcpy(buffer + offset, &cash, 4);		offset += 4;
	memcpy(buffer + offset, &kills, 4);		offset += 4;
	memcpy(buffer + offset, &deaths, 4);	offset += 4;
	memcpy(buffer + offset, &headshots, 4);	offset += 4;
	memcpy(buffer + offset, &wins, 4);		offset += 4;
	memcpy(buffer + offset, &losses, 4);	offset += 4;
	memcpy(buffer + offset, &clanId, 4);	offset += 4;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseGetUserInfoAllDBReq(char* pData, INT32 i32Size)
{
	// Get user info from DB (same as above for now, DB-backed version)
	OnBaseGetUserInfoAllReq(pData, i32Size);
}

void GameSession::OnBaseEnterPassReq(char* pData, INT32 i32Size)
{
	// Server entrance password check
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_BASE_ENTER_PASS_ACK, -1);
		return;
	}

	// In simplified version, no server password required
	SendSimpleAck(PROTOCOL_BASE_ENTER_PASS_ACK, 0);
}

void GameSession::OnBaseGetUidLobbyReq(char* pData, INT32 i32Size)
{
	// Find a user's UID in the lobby by some identifier
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_BASE_GET_UID_LOBBY_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_UID_LOBBY_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = pTarget ? 0 : -1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	memcpy(buffer + offset, &targetUID, sizeof(int64_t));	offset += sizeof(int64_t);

	if (pTarget)
	{
		int32_t channel = pTarget->GetChannelNum();
		int32_t roomIdx = pTarget->GetRoomIdx();
		memcpy(buffer + offset, &channel, 4);	offset += 4;
		memcpy(buffer + offset, &roomIdx, 4);	offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseGetUidRoomReq(char* pData, INT32 i32Size)
{
	// Find what room a UID is in
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_BASE_GET_UID_ROOM_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_BASE_GET_UID_ROOM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = (pTarget && pTarget->GetRoom()) ? 0 : -1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	memcpy(buffer + offset, &targetUID, sizeof(int64_t));	offset += sizeof(int64_t);

	if (pTarget && pTarget->GetRoom())
	{
		int32_t channel = pTarget->GetChannelNum();
		int32_t roomIdx = pTarget->GetRoomIdx();
		int32_t slotIdx = pTarget->GetSlotIdx();
		memcpy(buffer + offset, &channel, 4);	offset += 4;
		memcpy(buffer + offset, &roomIdx, 4);	offset += 4;
		memcpy(buffer + offset, &slotIdx, 4);	offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Batch 19 - Base extras
// ============================================================================

void GameSession::OnBaseRendezvousNatStateReq(char* pData, INT32 i32Size)
{
	// Client reports its NAT type state
	// Parse NAT type and echo back as ACK
	if (i32Size < 4) return;

	int32_t natType = 0;
	memcpy(&natType, pData, sizeof(int32_t));

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &natType, sizeof(int32_t));	offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseRendezvousHolepunchReq(char* pData, INT32 i32Size)
{
	// Client requests hole-punch relay
	// In distributed architecture, BattleServer handles UDP relay
	// Echo back the client's external IP and port for NAT traversal
	if (i32Size < 8) return;

	uint32_t clientIP = 0;
	uint16_t clientPort = 0;
	memcpy(&clientIP, pData, sizeof(uint32_t));
	memcpy(&clientPort, pData + 4, sizeof(uint16_t));

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_BASE_RENDEZVOUS_HOLEPUNCH_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &clientIP, sizeof(uint32_t));	offset += sizeof(uint32_t);
	memcpy(buffer + offset, &clientPort, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseHackingCheckKickReq(char* pData, INT32 i32Size)
{
	// Client-side hack detection reported a violation
	// Log and optionally kick
	if (i32Size < 4) return;

	int32_t hackType = 0;
	memcpy(&hackType, pData, sizeof(int32_t));

	printf("[GameSession] HackingCheckKick Index=%d UID=%lld HackType=%d\n",
		GetIndex(), m_i64UID, hackType);

	// Fire-and-forget - no ACK protocol defined
	// Could add kick logic here if hack type is severe
}

void GameSession::OnBaseChangePccafeStatusReq(char* pData, INT32 i32Size)
{
	// PC cafe status change notification
	if (i32Size < 4) return;

	int32_t pccafeStatus = 0;
	memcpy(&pccafeStatus, pData, sizeof(int32_t));

	// Echo back the status change
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);
	int32_t result = 0;	// success
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &pccafeStatus, sizeof(int32_t));	offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseGetUserSubtaskReq(char* pData, INT32 i32Size)
{
	// Query what a user is currently doing (channel, room, battle state)
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_BASE_GET_USER_SUBTASK_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_BASE_GET_USER_SUBTASK_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = pTarget ? 0 : 1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &targetUID, sizeof(int64_t));	offset += sizeof(int64_t);

	if (pTarget)
	{
		int32_t mainTask = (int32_t)pTarget->GetMainTask();
		int32_t channel = pTarget->GetChannelNum();
		int32_t roomIdx = pTarget->GetRoomIdx();
		memcpy(buffer + offset, &mainTask, sizeof(int32_t));	offset += sizeof(int32_t);
		memcpy(buffer + offset, &channel, sizeof(int32_t));		offset += sizeof(int32_t);
		memcpy(buffer + offset, &roomIdx, sizeof(int32_t));		offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnBaseLoginThaiReq(char* pData, INT32 i32Size)
{
	// Thailand Garena-specific login - not applicable to remake
	// Send failure to redirect to normal login
	SendSimpleAck(PROTOCOL_BASE_LOGIN_THAI_REQ + 1, 1);	// +1 = ACK enum offset, result=fail
}

// ============================================================================
// Batch 19 - Character/Chara
// ============================================================================

void GameSession::OnDeleteCharaReq(char* pData, INT32 i32Size)
{
	// Delete a character slot (reset to empty)
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_DELETE_CHARA_ACK, 1);
		return;
	}

	uint8_t slotIdx = (uint8_t)pData[0];

	if (slotIdx >= MAX_CHARA_SLOT)
	{
		SendSimpleAck(PROTOCOL_DELETE_CHARA_ACK, 2);
		return;
	}

	// Cannot delete active slot
	if (slotIdx == m_ui8ActiveCharaSlot)
	{
		SendSimpleAck(PROTOCOL_DELETE_CHARA_ACK, 3);
		return;
	}

	// Reset the slot
	m_CharaSlots[slotIdx].Reset();

	// Save via DataServer
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestEquipmentSave(m_i64UID, slotIdx,
			MULTI_SLOT_NONE, 0, nullptr, nullptr);
	}

	SendSimpleAck(PROTOCOL_DELETE_CHARA_ACK, 0);
}

void GameSession::OnNewSetCurrentCharaReq(char* pData, INT32 i32Size)
{
	// Set the representative character (3 slots for display)
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_NEW_SET_CURRENT_CHARA_ACK, 1);
		return;
	}

	uint8_t slotIdx = (uint8_t)pData[0];

	if (slotIdx >= MAX_CHARA_SLOT)
	{
		SendSimpleAck(PROTOCOL_NEW_SET_CURRENT_CHARA_ACK, 2);
		return;
	}

	// Check slot has a character
	if (m_CharaSlots[slotIdx].ui8State == MULTI_SLOT_NONE)
	{
		SendSimpleAck(PROTOCOL_NEW_SET_CURRENT_CHARA_ACK, 3);
		return;
	}

	m_ui8ActiveCharaSlot = slotIdx;
	SendSimpleAck(PROTOCOL_NEW_SET_CURRENT_CHARA_ACK, 0);
}

void GameSession::OnNewGameServerStateReq(char* pData, INT32 i32Size)
{
	// Client requests current server state info
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_NEW_GAME_SERVER_STATE_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Server state info
	int32_t onlineCount = g_pGameSessionManager ? g_pGameSessionManager->GetActiveCount() : 0;
	int32_t maxCount = g_pContextMain ? g_pContextMain->m_i32SessionCount : 1000;
	uint8_t channelCount = g_pContextMain ? g_pContextMain->m_ui8ChannelCount : 4;

	memcpy(buffer + offset, &onlineCount, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &maxCount, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &channelCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Batch 19 - Skill (Speciality)
// ============================================================================

void GameSession::OnSetSpecialityReq(char* pData, INT32 i32Size)
{
	// Set skill levels for the active class
	// Expected: classId(1) + skillSlot(1) + skillType(1) + level(1)
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_SET_SPECIALITY_ACK, 1);
		return;
	}

	uint8_t classId = (uint8_t)pData[0];
	uint8_t skillSlot = (uint8_t)pData[1];		// 0=main, 1=assist, 2=common
	uint8_t skillIdx = (uint8_t)pData[2];
	uint8_t newLevel = (uint8_t)pData[3];

	if (classId >= CHARA_CLASS_MAX)
	{
		SendSimpleAck(PROTOCOL_SET_SPECIALITY_ACK, 2);
		return;
	}

	GameClassSkills& skills = m_SkillData.classSkills[classId];

	// Validate skill index and level
	bool valid = false;
	uint8_t maxLevel = 0;
	uint8_t* pLevel = nullptr;

	if (skillSlot == 0 && skillIdx < MAX_MAIN_SKILLS)
	{
		maxLevel = MAX_SKILL_LEVEL;
		pLevel = &skills.ui8MainLevels[skillIdx];
		valid = true;
	}
	else if (skillSlot == 1 && skillIdx < MAX_ASSIST_SKILLS)
	{
		maxLevel = MAX_ASSIST_SKILL_LEVEL;
		pLevel = &skills.ui8AssistLevels[skillIdx];
		valid = true;
	}
	else if (skillSlot == 2 && skillIdx < MAX_COMMON_SKILLS)
	{
		maxLevel = MAX_SKILL_LEVEL;
		pLevel = &skills.ui8CommonLevels[skillIdx];
		valid = true;
	}

	if (!valid || newLevel > maxLevel)
	{
		SendSimpleAck(PROTOCOL_SET_SPECIALITY_ACK, 3);
		return;
	}

	// Check skill points
	if (newLevel > *pLevel)
	{
		uint8_t cost = newLevel - *pLevel;
		if (skills.ui16SkillPoints < cost)
		{
			SendSimpleAck(PROTOCOL_SET_SPECIALITY_ACK, 4);	// Not enough points
			return;
		}
		skills.ui16SkillPoints -= cost;
	}

	*pLevel = newLevel;

	// Save via DataServer
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestSkillSave(m_i64UID, classId,
			skills.ui8MainLevels, skills.ui8AssistLevels, skills.ui8CommonLevels,
			skills.ui16SkillPoints);
	}

	SendSimpleAck(PROTOCOL_SET_SPECIALITY_ACK, 0);
}

void GameSession::OnInitSpecialityReq(char* pData, INT32 i32Size)
{
	// Reset all skills for a class
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_INIT_SPECIALITY_ACK, 1);
		return;
	}

	uint8_t classId = (uint8_t)pData[0];

	if (classId >= CHARA_CLASS_MAX)
	{
		SendSimpleAck(PROTOCOL_INIT_SPECIALITY_ACK, 2);
		return;
	}

	GameClassSkills& skills = m_SkillData.classSkills[classId];

	// Refund all spent points
	int totalSpent = skills.GetTotalSkillPoints();
	skills.Reset();
	skills.ui8Class = classId;
	skills.ui16SkillPoints = (uint16_t)(totalSpent + 10);	// Refund + base 10

	// Save via DataServer
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestSkillSave(m_i64UID, classId,
			skills.ui8MainLevels, skills.ui8AssistLevels, skills.ui8CommonLevels,
			skills.ui16SkillPoints);
	}

	SendSimpleAck(PROTOCOL_INIT_SPECIALITY_ACK, 0);
}

// ============================================================================
// Batch 19 - Quest extras
// ============================================================================

void GameSession::OnGetAllQuestInfoReq(char* pData, INT32 i32Size)
{
	// Send all quest cardset data to client
	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_GET_ALL_QUEST_INFO_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Active set index
	memcpy(buffer + offset, &m_QuestData.ui8ActiveSetIndex, 1);	offset += 1;

	// Count active sets
	uint8_t activeCount = 0;
	for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		if (m_QuestData.cardSets[i].IsActive())
			activeCount++;
	}
	memcpy(buffer + offset, &activeCount, 1);	offset += 1;

	// Send each active cardset
	for (int i = 0; i < MAX_CARDSET_PER_USER; i++)
	{
		const GameQuestCardSet& cs = m_QuestData.cardSets[i];
		if (!cs.IsActive()) continue;

		uint8_t setIdx = (uint8_t)i;
		memcpy(buffer + offset, &setIdx, 1);			offset += 1;
		memcpy(buffer + offset, &cs.ui8Type, 1);		offset += 1;
		memcpy(buffer + offset, &cs.ui8ActiveCard, 1);	offset += 1;

		// Send card data for active card
		const GameQuestCard& card = cs.cards[cs.ui8ActiveCard];
		for (int q = 0; q < MAX_QUEST_PER_CARD; q++)
		{
			memcpy(buffer + offset, &card.quests[q].ui8Condition, 1);		offset += 1;
			memcpy(buffer + offset, &card.quests[q].ui32Target, 4);			offset += 4;
			memcpy(buffer + offset, &card.quests[q].ui32Current, 4);		offset += 4;
			memcpy(buffer + offset, &card.quests[q].ui32RewardGP, 4);		offset += 4;
			memcpy(buffer + offset, &card.quests[q].ui32RewardExp, 4);		offset += 4;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnGetHonorQuestInfoReq(char* pData, INT32 i32Size)
{
	// Honor quest info - send empty/stub response
	// Honor quests are a special subset tracked separately
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	// No explicit ACK defined - use REQ+1 convention
	uint16_t proto = PROTOCOL_GET_HONOR_QUEST_INFO_REQ + 1;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	uint8_t count = 0;	// No honor quests yet
	memcpy(buffer + offset, &count, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnPeriodQuestSettingReq(char* pData, INT32 i32Size)
{
	// Set period quest (daily/event quest) configuration
	if (i32Size < 3)
	{
		SendSimpleAck(PROTOCOL_PERIOD_QUEST_SETTING_ACK, 1);
		return;
	}

	uint8_t setIndex = (uint8_t)pData[0];
	uint8_t setType = (uint8_t)pData[1];
	uint8_t activeCard = (uint8_t)pData[2];

	if (setIndex >= MAX_CARDSET_PER_USER)
	{
		SendSimpleAck(PROTOCOL_PERIOD_QUEST_SETTING_ACK, 2);
		return;
	}

	// Update the quest cardset
	GameQuestCardSet& cs = m_QuestData.cardSets[setIndex];
	cs.ui8Type = setType;
	cs.ui8ActiveCard = (activeCard < MAX_CARD_PER_CARDSET) ? activeCard : 0;

	// Save via DataServer
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestQuestSave(m_i64UID, setIndex, setType,
			cs.ui8ActiveCard, (const char*)&cs, sizeof(GameQuestCardSet));
	}

	SendSimpleAck(PROTOCOL_PERIOD_QUEST_SETTING_ACK, 0);
}

void GameSession::SendSimpleAck(uint16_t protocolAck, int32_t result)
{
	i3NetworkPacket packet;
	char buffer[16];
	int offset = 0;

	uint16_t size = 0;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &protocolAck, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));			offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
