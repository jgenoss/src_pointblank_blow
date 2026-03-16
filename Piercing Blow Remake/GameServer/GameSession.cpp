#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"

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
	, m_ui8ActiveCharaSlot(0)
	, m_i32InventoryCount(0)
	, m_i32FriendCount(0)
	, m_i32BlockCount(0)
	, m_dwConnectTime(0)
	, m_dwLastPacketTime(0)
{
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
	uint16_t protocolId = *(uint16_t*)(pPacket + 2);

	if (iSize < packetSize)
		return 0;

	m_dwLastPacketTime = GetTickCount();

	char* pData = pPacket + SOCKET_HEAD_SIZE;
	INT32 dataSize = packetSize - SOCKET_HEAD_SIZE;

	switch (protocolId)
	{
	// ---- Base protocols ----
	case PROTOCOL_BASE_CONNECT_REQ:		OnBaseConnectReq(pData, dataSize);		break;
	case PROTOCOL_BASE_HEART_BIT_REQ:	OnBaseHeartBitReq(pData, dataSize);		break;
	case PROTOCOL_LOGIN_REQ:			OnLoginReq(pData, dataSize);			break;
	case PROTOCOL_BASE_GET_USER_INFO_REQ:	OnGetUserInfoReq(pData, dataSize);	break;

	// ---- Channel (GameSessionChannel.cpp) ----
	case PROTOCOL_BASE_GET_CHANNELLIST_REQ:	OnChannelListReq(pData, dataSize);	break;
	case PROTOCOL_BASE_SELECT_CHANNEL_REQ:	OnChannelEnterReq(pData, dataSize);	break;

	// ---- Lobby (GameSessionChannel.cpp) ----
	case PROTOCOL_LOBBY_ENTER_REQ:			OnLobbyEnterReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_LEAVE_REQ:			OnLobbyLeaveReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ:	OnGetRoomListReq(pData, dataSize);		break;
	case PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ:	OnQuickJoinRoomReq(pData, dataSize);	break;
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

	// ---- Shop (GameSessionShop.cpp) ----
	case PROTOCOL_SHOP_ENTER_REQ:					OnShopEnterReq(pData, dataSize);			break;
	case PROTOCOL_SHOP_LEAVE_REQ:					OnShopLeaveReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ:			OnShopBuyReq(pData, dataSize);				break;
	case PROTOCOL_SHOP_REPAIR_REQ:					OnShopRepairReq(pData, dataSize);			break;
	case PROTOCOL_AUTH_GET_POINT_CASH_REQ:			OnGetPointCashReq(pData, dataSize);			break;

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
	m_ui32EncriptKey = (uint32_t)(GetTickCount() ^ (DWORD)GetIndex());
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

	if (i32Size < (int)(sizeof(uint32_t) + 64))
	{
		SendLoginAck(2);
		return;
	}

	uint32_t authToken = *(uint32_t*)pData;
	memcpy(m_szUsername, pData + sizeof(uint32_t), 64);
	m_szUsername[63] = '\0';

	// TODO: Validate auth token via ModuleConnectServer
	m_ui32AuthToken = authToken;
	m_eMainTask = GAME_TASK_LOGIN;

	// TODO: Request player data from DataServer
	m_i64UID = (int64_t)(GetIndex() + 1);
	strncpy_s(m_szNickname, m_szUsername, _TRUNCATE);
	m_i32Level = 1;
	m_i64Exp = 0;
	m_i32GP = 10000;

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

	m_eMainTask = GAME_TASK_CHANNEL;
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

	m_i32Kills = 0;
	m_i32Deaths = 0;
	m_i32Headshots = 0;
	m_i32Wins = 0;
	m_i32Losses = 0;

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

	m_MedalData.Reset();
	m_AttendanceData.Reset();
	m_DailyRecord.Reset();
	m_SkillData.Reset();

	m_dwConnectTime = 0;
	m_dwLastPacketTime = 0;
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
