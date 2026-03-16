#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"
#include "ClanDef.h"
#include "GameServerContext.h"
#include "ModuleConnectServer.h"

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

	// ---- Equipment (GameSessionEquipment.cpp) ----
	case PROTOCOL_BASE_GET_EQUIPMENT_INFO_REQ:		OnGetEquipmentInfoReq(pData, dataSize);		break;
	case PROTOCOL_BASE_EQUIPMENT_REQ:				OnEquipmentReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GET_CHARA_INFO_REQ:			OnGetCharaInfoReq(pData, dataSize);			break;
	case PROTOCOL_CHAR_CREATE_CHARA_REQ:			OnCharaCreateReq(pData, dataSize);			break;
	case PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_REQ:		OnCharaShiftPosReq(pData, dataSize);		break;
	case PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ:		OnRoomGetUserEquipmentReq(pData, dataSize);	break;

	// ---- Inventory (GameSessionInventory.cpp) ----
	case PROTOCOL_BASE_GET_INVEN_INFO_REQ:			OnGetInvenInfoReq(pData, dataSize);			break;
	case PROTOCOL_INVENTORY_ENTER_REQ:				OnInventoryEnterReq(pData, dataSize);		break;
	case PROTOCOL_INVENTORY_LEAVE_REQ:				OnInventoryLeaveReq(pData, dataSize);		break;

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
	case PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ:			OnBattleSuggestKickVoteReq(pData, dataSize);		break;
	case PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ:				OnBattleVoteKickVoteReq(pData, dataSize);			break;
	case PROTOCOL_BATTLE_SENDPING_REQ:					OnBattleSendPingReq(pData, dataSize);				break;

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
	case PROTOCOL_AUTH_SEND_WHISPER_REQ:			OnWhisperReq(pData, dataSize);				break;
	case PROTOCOL_AUTH_BLOCK_INSERT_REQ:			OnBlockInsertReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_BLOCK_DELETE_REQ:			OnBlockDeleteReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_FIND_USER_REQ:				OnFindUserReq(pData, dataSize);				break;

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

	// TODO: Request player data from DataServer (for now use defaults)
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

	m_eMainTask = GAME_TASK_INFO;
	SendLoginAck(0);

	printf("[GameSession] Login success - Index=%d, User=%s, UID=%lld\n",
		GetIndex(), m_szUsername, m_i64UID);
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

	// TODO: Forward to DataServer for duplicate check
	// For now, always return available
	SendSimpleAck(PROTOCOL_BASE_CHECK_NICK_NAME_ACK, 0);	// 0 = available

	printf("[GameSession] CheckNickName - Index=%d, Nick=%s\n", GetIndex(), szNickname);
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

	// TODO: Forward to DataServer
	// For now, accept directly
	OnCreateNickResult(0, szNickname);

	printf("[GameSession] CreateNick - Index=%d, Nick=%s\n", GetIndex(), szNickname);
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

	m_eMainTask = GAME_TASK_INFO;

	printf("[GameSession] Player data loaded - UID=%lld, Nick=%s, Level=%d\n",
		m_i64UID, m_szNickname, m_i32Level);
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
	m_ui8AuthLevel = 0;

	m_i32Kills = 0;
	m_i32Deaths = 0;
	m_i32Headshots = 0;
	m_i32Wins = 0;
	m_i32Losses = 0;
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
