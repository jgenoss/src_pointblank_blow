#ifndef __GAMESESSION_H__
#define __GAMESESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "RoomDef.h"
#include "CharacterDef.h"
#include "InventoryDef.h"
#include "QuestDef.h"
#include "MedalDef.h"
#include "EventDef.h"
#include "SkillDef.h"
#include "ClanDef.h"

class Room;

// Friend/Block limits
#define MAX_FRIEND_COUNT	50
#define MAX_BLOCK_COUNT		50

// Note/Mail system
#define MAX_NOTE_COUNT		50
#define NOTE_SUBJECT_LEN	64
#define NOTE_BODY_LEN		256

struct GameNoteData
{
	uint32_t	ui32NoteId;
	int64_t		i64SenderUID;
	char		szSenderNick[64];
	char		szSubject[NOTE_SUBJECT_LEN];
	char		szBody[NOTE_BODY_LEN];
	DWORD		dwTimestamp;		// GetTickCount() when received
	uint8_t		ui8Type;			// 0=normal, 1=system, 2=gift
	bool		bRead;

	void Reset()
	{
		ui32NoteId = 0;
		i64SenderUID = 0;
		szSenderNick[0] = '\0';
		szSubject[0] = '\0';
		szBody[0] = '\0';
		dwTimestamp = 0;
		ui8Type = 0;
		bRead = false;
	}
};

// Title system (Phase 4D)
#define MAX_TITLE_COUNT		44
#define MAX_EQUIPPED_TITLES	3

struct GameTitleData
{
	uint8_t		ui8OwnedTitles[MAX_TITLE_COUNT];	// 0=not owned, 1=owned
	uint8_t		ui8EquippedSlots[MAX_EQUIPPED_TITLES];	// Title ID per slot (0=empty)
	int			i32OwnedCount;

	void Reset()
	{
		memset(ui8OwnedTitles, 0, sizeof(ui8OwnedTitles));
		memset(ui8EquippedSlots, 0, sizeof(ui8EquippedSlots));
		i32OwnedCount = 0;
	}
};

// Friend info stored per session
struct GameFriendInfo
{
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Level;
	int			i32RankId;

	void Reset()
	{
		i64UID = 0;
		szNickname[0] = '\0';
		i32Level = 0;
		i32RankId = 0;
	}
};

// Block info stored per session
struct GameBlockInfo
{
	int64_t		i64UID;
	char		szNickname[64];

	void Reset()
	{
		i64UID = 0;
		szNickname[0] = '\0';
	}
};

// GAME_TASK state machine (from Server/Source/Game/Game/ServerDef.h)
// GAME_TASK_NONE -> CONNECT -> LEAVE -> LOGIN_ING -> LOGIN -> INFO -> CHANNEL -> LOBBY -> READY_ROOM -> BATTLE
enum GameTask
{
	GAME_TASK_NONE = 0,
	GAME_TASK_CONNECT,
	GAME_TASK_LEAVE,
	GAME_TASK_LOGIN_ING,
	GAME_TASK_LOGIN,
	GAME_TASK_INFO,
	GAME_TASK_CHANNEL,
	GAME_TASK_LOBBY,
	GAME_TASK_READY_ROOM,
	GAME_TASK_BATTLE,
	GAME_TASK_COUNT,
};

// Timeout definitions (from ServerDef.h)
#define SESSION_CHECK_TIMEOUT_NORMAL		120		// 2 minutes
#define SESSION_CHECK_TIMEOUT_CHANNEL		3600	// 1 hour
#define SESSION_CHECK_TIMEOUT_LOGIN			600		// 10 minutes
#define SESSION_CHECK_TIMEOUT_NOT_LOGIN		30		// 30 seconds

// Simplified port of CUserSession from Server/Source/Game/Game/UserSession.h
// State machine, packet handlers for all GAME_TASK states
class GameSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(GameSession);

public:
	GameSession();
	virtual ~GameSession();

	// i3NetworkSession overrides
	virtual BOOL	Create() override;
	virtual void	Destroy() override;
	virtual BOOL	OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL	OnConnectInit() override;
	virtual BOOL	OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32	PacketParsing(char* pPacket, INT32 iSize) override;

	// State
	GameTask		GetTask() const				{ return m_eMainTask; }
	void			SetTask(GameTask eTask)		{ m_eMainTask = eTask; }

	// Identity
	int64_t			GetUID() const				{ return m_i64UID; }
	void			SetUID(int64_t uid)			{ m_i64UID = uid; }
	const char*		GetUsername() const			{ return m_szUsername; }
	const char*		GetNickname() const			{ return m_szNickname; }
	uint32_t		GetXorKey() const			{ return m_ui32EncriptKey; }
	uint32_t		GetAuthToken() const		{ return m_ui32AuthToken; }
	void			SetAuthToken(uint32_t token){ m_ui32AuthToken = token; }

	// Channel & Room
	int				GetChannelNum() const		{ return m_i32ChannelNum; }
	void			SetChannelNum(int ch)		{ m_i32ChannelNum = ch; }
	int				GetRoomIdx() const			{ return m_i32RoomIdx; }
	void			SetRoomIdx(int idx)			{ m_i32RoomIdx = idx; }
	int				GetSlotIdx() const			{ return m_i32SlotIdx; }
	void			SetSlotIdx(int idx)			{ m_i32SlotIdx = idx; }
	Room*			GetRoom() const				{ return m_pRoom; }
	void			SetRoom(Room* pRoom)		{ m_pRoom = pRoom; }

	// Player data (loaded from DataServer)
	int				GetLevel() const			{ return m_i32Level; }
	int64_t			GetExp() const				{ return m_i64Exp; }
	int				GetCash() const				{ return m_i32Cash; }
	int				GetGP() const				{ return m_i32GP; }
	int				GetRankId() const			{ return m_i32RankId; }
	int				GetClanId() const			{ return m_i32ClanId; }
	void			SetClanId(int id)			{ m_i32ClanId = id; }
	const char*		GetClanName() const			{ return m_szClanName; }

	// Stats
	int				GetKills() const			{ return m_i32Kills; }
	int				GetDeaths() const			{ return m_i32Deaths; }
	int				GetHeadshots() const		{ return m_i32Headshots; }
	int				GetWins() const				{ return m_i32Wins; }
	uint32_t		GetColorNick() const		{ return 0; }	// TODO: store color nick from inventory
	int				GetLosses() const			{ return m_i32Losses; }
	GameTask		GetMainTask() const			{ return m_eMainTask; }

	// Equipment (7E)
	uint8_t			GetActiveCharaSlot() const				{ return m_ui8ActiveCharaSlot; }
	const GameCharaEquip& GetActiveEquipment() const		{ return m_CharaSlots[m_ui8ActiveCharaSlot].equip; }
	const GameCharaSlot& GetCharaSlot(int idx) const		{ return m_CharaSlots[idx]; }

	// Inventory (7F)
	int					GetInventoryCount() const			{ return m_i32InventoryCount; }
	const GameInventoryItem* GetInventory() const			{ return m_Inventory; }
	GameInventoryItem*	FindInventoryItem(uint32_t itemId);
	GameInventoryItem*	FindInventoryItemByDBIdx(uint32_t dbIdx);
	bool				HasInventoryItem(uint32_t itemId) const;
	int					AddInventoryItem(const GameInventoryItem& item);
	bool				RemoveInventoryItem(uint32_t dbIdx);

	// Load player data from DataServer response
	void			OnPlayerDataLoaded(const char* pPayload, int i32PayloadSize);

	// DataServer callback results (called from ModuleDataServer response handlers)
	void			OnCreateNickResult(int i32Result, const char* pszNickname);
	void			OnCheckNickResult(int i32Result);
	void			OnClanCreateResult(int i32ClanId, int i32Result);
	void			OnClanJoinResult(int i32ClanId, int i32Result);
	void			OnFriendAddResult(int64_t i64FriendUID, int i32Result);
	void			OnFriendListLoaded(struct IS_FRIEND_ENTRY* pEntries, int i32Count);
	void			OnBlockAddResult(int64_t i64BlockedUID, int i32Result);
	void			OnBlockListLoaded(struct IS_BLOCK_ENTRY* pEntries, int i32Count);

	// Battle results from BattleServer (applied via ModuleBattleServer)
	void			ApplyBattleResult(int i32Kills, int i32Deaths, int i32Headshots, bool bWin);

	// SLOT_BONUS tracking (Phase 1C) - per-battle EXP/Point bonuses
	int				GetLastBattleExpBonus() const	{ return m_i32LastBattleExpBonus; }
	int				GetLastBattleGPBonus() const	{ return m_i32LastBattleGPBonus; }

	// Item expiration (Phase 5B)
	void			CheckExpiredItems();
	int				RemoveExpiredItems();

	// Inventory boost items (Phase 10A)
	uint16_t		GetInventoryExpMultiplier() const;	// 100-based (100 = 1x)
	uint16_t		GetInventoryPointMultiplier() const;

	// Item durability (Phase 14C)
	void			DecreaseEquippedDurability();
	int				RepairItem(uint32_t ui32ItemDBIdx);
	int				RepairAllItems();

	// Save all player stats at end of battle
	void			SaveAllPlayerStats();

	// Timeout
	bool			IsTimedOut() const;
	DWORD			GetLastPacketTime() const	{ return m_dwLastPacketTime; }

private:
	// Packet handlers - Connect phase
	void			OnBaseConnectReq(char* pData, INT32 i32Size);
	void			OnBaseHeartBitReq(char* pData, INT32 i32Size);

	// Packet handlers - Login phase (auth token validation)
	void			OnLoginReq(char* pData, INT32 i32Size);

	// Packet handlers - Info phase
	void			OnGetUserInfoReq(char* pData, INT32 i32Size);
	void			OnGetUserInfoLobbyReq(char* pData, INT32 i32Size);
	void			OnGetUserInfoRoomReq(char* pData, INT32 i32Size);

	// Packet handlers - System/Login flow (Phase 4A - GameSession.cpp)
	void			OnGetSystemInfoReq(char* pData, INT32 i32Size);
	void			OnGetOptionReq(char* pData, INT32 i32Size);
	void			OnOptionSaveReq(char* pData, INT32 i32Size);
	void			OnCheckNickNameReq(char* pData, INT32 i32Size);
	void			OnCreateNickReq(char* pData, INT32 i32Size);
	void			OnRankUpReq(char* pData, INT32 i32Size);
	void			OnGameGuardReq(char* pData, INT32 i32Size);
	void			OnLogoutReq(char* pData, INT32 i32Size);
	void			OnQuestBuyCardSetReq(char* pData, INT32 i32Size);
	void			OnGuideCompleteReq(char* pData, INT32 i32Size);
	void			OnGetUserInfoByUIDReq(char* pData, INT32 i32Size);
	void			OnGetUserInfoByNickReq(char* pData, INT32 i32Size);
	void			OnNewSpecialitySetReq(char* pData, INT32 i32Size);
	void			OnNewSpecialityInitReq(char* pData, INT32 i32Size);
	void			OnNewSpecialitySetClassReq(char* pData, INT32 i32Size);
	void			OnQuestDeleteCardSetReq(char* pData, INT32 i32Size);
	void			OnGetRecordInfoSeasonReq(char* pData, INT32 i32Size);
	void			OnGetRecordInfoSessionReq(char* pData, INT32 i32Size);

	// Packet handlers - User Info (Phase 4C - GameSession.cpp)
	void			OnGetMyInfoRecordReq(char* pData, INT32 i32Size);
	void			OnGetMyInfoBasicReq(char* pData, INT32 i32Size);
	void			OnGetMyInfoAllReq(char* pData, INT32 i32Size);
	void			OnGetRecordInfoDBReq(char* pData, INT32 i32Size);

	// Packet handlers - Title (Phase 4D - GameSession.cpp)
	void			OnTitleEquipReq(char* pData, INT32 i32Size);
	void			OnTitleReleaseReq(char* pData, INT32 i32Size);
	void			OnTitleChangeReq(char* pData, INT32 i32Size);

	// Packet handlers - Map/Stage data (Phase 4B - GameSessionChannel.cpp)
	void			OnMapVersionReq(char* pData, INT32 i32Size);
	void			OnMapListReq(char* pData, INT32 i32Size);
	void			OnMapRuleListReq(char* pData, INT32 i32Size);
	void			OnMapMatchingListReq(char* pData, INT32 i32Size);
	void			OnMapRandomListReq(char* pData, INT32 i32Size);

	// Packet handlers - Channel (GameSessionChannel.cpp)
	void			OnChannelListReq(char* pData, INT32 i32Size);
	void			OnChannelEnterReq(char* pData, INT32 i32Size);
	void			OnChannelLeaveReq(char* pData, INT32 i32Size);

	// Packet handlers - Lobby (GameSessionChannel.cpp)
	void			OnLobbyEnterReq(char* pData, INT32 i32Size);
	void			OnLobbyLeaveReq(char* pData, INT32 i32Size);
	void			OnGetRoomListReq(char* pData, INT32 i32Size);
	void			OnLobbyChatReq(char* pData, INT32 i32Size);
	void			OnQuickJoinRoomReq(char* pData, INT32 i32Size);
	void			OnQuickJoinStartReq(char* pData, INT32 i32Size);
	void			OnQuickJoinEnter2ndBestReq(char* pData, INT32 i32Size);
	void			OnLobbyViewUserItemReq(char* pData, INT32 i32Size);
	void			OnLobbyFindNickNameReq(char* pData, INT32 i32Size);
	void			OnMegaphoneReq(char* pData, INT32 i32Size);
	void			OnLobbyGetRoomInfoReq(char* pData, INT32 i32Size);
	void			OnLobbyGetRoomInfoAddReq(char* pData, INT32 i32Size);
	void			OnLobbyCreateTrainingReq(char* pData, INT32 i32Size);
	void			OnLobbyQuickJoinSettingReq(char* pData, INT32 i32Size);
	void			OnLobbySetBirthdayReq(char* pData, INT32 i32Size);
	void			OnLobbyGetUIDByNickNameReq(char* pData, INT32 i32Size);

	// Packet handlers - Room (GameSessionRoom.cpp)
	void			OnRoomCreateReq(char* pData, INT32 i32Size);
	void			OnRoomJoinReq(char* pData, INT32 i32Size);
	void			OnRoomLeaveReq(char* pData, INT32 i32Size);
	void			OnRoomGetSlotInfoReq(char* pData, INT32 i32Size);
	void			OnRoomGetPlayerInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChangeRoomInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChangePasswdReq(char* pData, INT32 i32Size);
	void			OnRoomChangeSlotReq(char* pData, INT32 i32Size);
	void			OnRoomTeamChangeReq(char* pData, INT32 i32Size);
	void			OnRoomRequestMainReq(char* pData, INT32 i32Size);
	void			OnRoomRequestMainChangeReq(char* pData, INT32 i32Size);
	void			OnRoomChangeOptionInfoReq(char* pData, INT32 i32Size);
	void			OnRoomChatReq(char* pData, INT32 i32Size);
	void			OnRoomGetUserEquipmentReq(char* pData, INT32 i32Size);
	void			OnRoomInviteLobbyUserReq(char* pData, INT32 i32Size);
	void			OnRoomChangeObserverSlotReq(char* pData, INT32 i32Size);
	void			OnRoomLoadingStartReq(char* pData, INT32 i32Size);
	void			OnRoomInfoEnterReq(char* pData, INT32 i32Size);
	void			OnRoomInfoLeaveReq(char* pData, INT32 i32Size);
	void			OnRoomGetLobbyUserListReq(char* pData, INT32 i32Size);
	void			OnRoomCheckMainReq(char* pData, INT32 i32Size);
	void			OnRoomMasterTeamChangeReq(char* pData, INT32 i32Size);
	void			OnRoomGetNicknameReq(char* pData, INT32 i32Size);
	void			OnRoomGetRankReq(char* pData, INT32 i32Size);
	void			OnRoomChangeRoomAllInfoReq(char* pData, INT32 i32Size);
	void			OnRoomGetColorNickReq(char* pData, INT32 i32Size);
	void			OnRoomGetColorMuzzleFlashReq(char* pData, INT32 i32Size);
	void			OnRoomCharaShiftPosReq(char* pData, INT32 i32Size);
	void			OnRoomEquipmentTempWeaponReq(char* pData, INT32 i32Size);
	void			OnRoomInviteLobbyUserListReq(char* pData, INT32 i32Size);
	void			OnRoomTsEventReq(char* pData, INT32 i32Size);
	void			OnRoomRequestMainChangeWhoReq(char* pData, INT32 i32Size);

	// Packet handlers - Battle flow (GameSessionBattle.cpp)
	void			OnBattleReadyBattleReq(char* pData, INT32 i32Size);
	void			OnBattlePreStartBattleReq(char* pData, INT32 i32Size);
	void			OnBattleStartBattleReq(char* pData, INT32 i32Size);
	void			OnBattleGiveUpBattleReq(char* pData, INT32 i32Size);
	void			OnBattleDeathReq(char* pData, INT32 i32Size);
	void			OnBattleRespawnReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundPreStartReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundStartReq(char* pData, INT32 i32Size);
	void			OnBattleMissionRoundEndReq(char* pData, INT32 i32Size);
	void			OnBattleMissionBombInstallReq(char* pData, INT32 i32Size);
	void			OnBattleMissionBombUninstallReq(char* pData, INT32 i32Size);
	void			OnBattleMissionGeneratorDestroyReq(char* pData, INT32 i32Size);
	void			OnBattleMissionGeneratorInfoReq(char* pData, INT32 i32Size);
	void			OnBattleMissionTouchdownReq(char* pData, INT32 i32Size);
	void			OnBattleMissionDefenceInfoReq(char* pData, INT32 i32Size);
	void			OnBattleEquipmentInfoReq(char* pData, INT32 i32Size);
	void			OnBattlePickupWeaponReq(char* pData, INT32 i32Size);
	void			OnBattleTimerSyncReq(char* pData, INT32 i32Size);
	void			OnBattleTimeoutClientReq(char* pData, INT32 i32Size);
	void			OnBattleLeaveP2PServerReq(char* pData, INT32 i32Size);
	void			OnBattleUseItemReq(char* pData, INT32 i32Size);
	void			OnBattleEndBattleReq(char* pData, INT32 i32Size);
	void			OnBattleHoleCheckReq(char* pData, INT32 i32Size);
	void			OnBattleWaveReadyInfoReq(char* pData, INT32 i32Size);
	void			OnBattleWaveReadyInfoCancelReq(char* pData, INT32 i32Size);
	void			OnBattleRecordReq(char* pData, INT32 i32Size);
	void			OnBattleSuggestKickVoteReq(char* pData, INT32 i32Size);
	void			OnBattleVoteKickVoteReq(char* pData, INT32 i32Size);
	void			OnBattleSendPingReq(char* pData, INT32 i32Size);
	void			OnBattleChangeDifficultyReq(char* pData, INT32 i32Size);
	void			OnBattleMissionTouchdownCountReq(char* pData, INT32 i32Size);
	void			OnBattleMissionDeathblowReq(char* pData, INT32 i32Size);
	void			OnBattleSupplyBoxResultReq(char* pData, INT32 i32Size);
	void			OnBattleUserSkillUpgradeReq(char* pData, INT32 i32Size);
	void			OnBattleSlotEquipmentReq(char* pData, INT32 i32Size);
	void			OnBattleMissionTutorialRoundEndReq(char* pData, INT32 i32Size);
	void			OnBattleUserScopeTypeReq(char* pData, INT32 i32Size);
	void			OnBattleTimeoutMainClientReq(char* pData, INT32 i32Size);
	void			OnBattleNewJoinRoomScoreReq(char* pData, INT32 i32Size);
	void			OnBattleReportScreenshotReq(char* pData, INT32 i32Size);

	// Packet handlers - Equipment (GameSessionEquipment.cpp)
	void			OnGetEquipmentInfoReq(char* pData, INT32 i32Size);
	void			OnEquipmentReq(char* pData, INT32 i32Size);
	void			OnGetCharaInfoReq(char* pData, INT32 i32Size);
	void			OnCharaCreateReq(char* pData, INT32 i32Size);
	void			OnCharaDeleteReq(char* pData, INT32 i32Size);
	void			OnCharaChangeNickReq(char* pData, INT32 i32Size);
	void			OnCharaChangeEquipReq(char* pData, INT32 i32Size);
	void			OnCharaShiftPosReq(char* pData, INT32 i32Size);

	// Packet handlers - Inventory (GameSessionInventory.cpp)
	void			OnGetInvenInfoReq(char* pData, INT32 i32Size);
	void			OnInventoryEnterReq(char* pData, INT32 i32Size);
	void			OnInventoryLeaveReq(char* pData, INT32 i32Size);
	void			OnInventoryGetInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Shop (GameSessionShop.cpp)
	void			OnShopEnterReq(char* pData, INT32 i32Size);
	void			OnShopLeaveReq(char* pData, INT32 i32Size);
	void			OnShopBuyReq(char* pData, INT32 i32Size);
	void			OnShopRepairReq(char* pData, INT32 i32Size);
	void			OnShopGiftReq(char* pData, INT32 i32Size);
	void			OnShopItemExtendReq(char* pData, INT32 i32Size);
	void			OnGetPointCashReq(char* pData, INT32 i32Size);
	void			OnShopVersionReq(char* pData, INT32 i32Size);
	void			OnShopListReq(char* pData, INT32 i32Size);
	void			OnShopGoodsListReq(char* pData, INT32 i32Size);
	void			OnShopItemListReq(char* pData, INT32 i32Size);
	void			OnShopMatchingListReq(char* pData, INT32 i32Size);
	void			OnShopRepairListReq(char* pData, INT32 i32Size);
	void			OnShopItemAuthReq(char* pData, INT32 i32Size);
	void			OnShopInsertItemReq(char* pData, INT32 i32Size);
	void			OnShopDeleteItemReq(char* pData, INT32 i32Size);
	void			OnShopCapsuleReq(char* pData, INT32 i32Size);
	void			OnShopJackpotReq(char* pData, INT32 i32Size);
	void			OnShopGetSaleListReq(char* pData, INT32 i32Size);
	void			OnShopPlusPointReq(char* pData, INT32 i32Size);

	// Packet handlers - Social (GameSessionSocial.cpp)
	void			OnFriendInfoReq(char* pData, INT32 i32Size);
	void			OnFriendInsertReq(char* pData, INT32 i32Size);
	void			OnFriendDeleteReq(char* pData, INT32 i32Size);
	void			OnWhisperReq(char* pData, INT32 i32Size);
	void			OnBlockInsertReq(char* pData, INT32 i32Size);
	void			OnBlockDeleteReq(char* pData, INT32 i32Size);
	void			OnFindUserReq(char* pData, INT32 i32Size);

	// Note/Mail system (GameSessionSocial.cpp)
	void			OnNoteSendReq(char* pData, INT32 i32Size);
	void			OnNoteListReq(char* pData, INT32 i32Size);
	void			OnNoteDeleteReq(char* pData, INT32 i32Size);
	void			OnNoteCheckReadedReq(char* pData, INT32 i32Size);

	// Additional Auth handlers (GameSessionSocial.cpp)
	void			OnFriendAcceptReq(char* pData, INT32 i32Size);
	void			OnFriendInviteReq(char* pData, INT32 i32Size);
	void			OnCommunityUserInviteReq(char* pData, INT32 i32Size);
	void			OnChangeNicknameReq(char* pData, INT32 i32Size);
	void			OnChangeColorNickReq(char* pData, INT32 i32Size);
	void			OnGiftShopEnterReq(char* pData, INT32 i32Size);
	void			OnGiftShopLeaveReq(char* pData, INT32 i32Size);

	// Social helpers
	GameFriendInfo*	FindFriend(const char* nickname) const;
	GameFriendInfo*	FindFriendByUID(int64_t uid) const;
	bool			IsBlocked(int64_t uid) const;

	// Note receive (called from another session's OnNoteSendReq)
	bool			ReceiveNote(int64_t senderUID, const char* senderNick,
							const char* subject, const char* body, uint8_t type);

	// Friend notifications (Phase 7A)
	void			NotifyFriendsStatusChange(uint8_t ui8NewState);
	void			NotifyFriendLobbyEnter();
	void			NotifyFriendLobbyLeave();

	// Packet handlers - Quest (GameSessionQuest.cpp)
	void			OnQuestGetReq(char* pData, INT32 i32Size);
	void			OnQuestGetInfoReq(char* pData, INT32 i32Size);
	void			OnQuestGetItemReq(char* pData, INT32 i32Size);
	void			OnQuestChangeReq(char* pData, INT32 i32Size);
	void			OnQuestActiveIdxChangeReq(char* pData, INT32 i32Size);

	// Quest helpers
	void			UpdateQuestProgress(int kills, int deaths, int headshots, bool won);

	// Packet handlers - Medal (GameSessionMedal.cpp)
	void			OnGetMedalSystemReq(char* pData, INT32 i32Size);
	void			OnRefreshMedalSystemReq(char* pData, INT32 i32Size);
	void			OnMedalGetInfoReq(char* pData, INT32 i32Size);
	void			OnGetCurMedalSetInfoReq(char* pData, INT32 i32Size);
	void			OnGetComMedalSetInfoReq(char* pData, INT32 i32Size);
	void			OnGetMedalRewardReq(char* pData, INT32 i32Size);
	void			OnSetNotifyMedalReq(char* pData, INT32 i32Size);

	// Packet handlers - Event/Attendance (GameSessionEvent.cpp)
	void			OnAttendanceReq(char* pData, INT32 i32Size);
	void			OnAttendanceClearItemReq(char* pData, INT32 i32Size);
	void			OnDailyRecordReq(char* pData, INT32 i32Size);

	// Packet handlers - Skill (GameSessionSkill.cpp)
	void			OnSkillSetReq(char* pData, INT32 i32Size);
	void			OnSkillResetReq(char* pData, INT32 i32Size);
	void			OnSkillClassSetReq(char* pData, INT32 i32Size);
	void			OnSkillGetInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Clan (GameSessionClan.cpp)
	void			OnClanCreateReq(char* pData, INT32 i32Size);
	void			OnClanCloseReq(char* pData, INT32 i32Size);
	void			OnClanDetailInfoReq(char* pData, INT32 i32Size);
	void			OnClanMemberListReq(char* pData, INT32 i32Size);
	void			OnClanJoinReq(char* pData, INT32 i32Size);
	void			OnClanLeaveReq(char* pData, INT32 i32Size);
	void			OnClanEnterReq(char* pData, INT32 i32Size);
	void			OnClanChatReq(char* pData, INT32 i32Size);
	void			OnClanListReq(char* pData, INT32 i32Size);
	void			OnClanDeportationReq(char* pData, INT32 i32Size);
	void			OnClanCommissionMasterReq(char* pData, INT32 i32Size);
	void			OnClanCommissionStaffReq(char* pData, INT32 i32Size);
	void			OnClanCommissionRegularReq(char* pData, INT32 i32Size);
	void			OnClanReplaceNoticeReq(char* pData, INT32 i32Size);
	void			OnClanReplaceIntroReq(char* pData, INT32 i32Size);
	void			OnClanReplaceMarkReq(char* pData, INT32 i32Size);
	void			OnClanAcceptRequestReq(char* pData, INT32 i32Size);
	void			OnClanDenialRequestReq(char* pData, INT32 i32Size);
	void			OnClanInviteReq(char* pData, INT32 i32Size);
	void			OnClanRequestListReq(char* pData, INT32 i32Size);
	void			OnClanCheckDuplicateReq(char* pData, INT32 i32Size);
	void			OnClanMemberDetailInfoReq(char* pData, INT32 i32Size);
	void			OnClanPageChattingReq(char* pData, INT32 i32Size);
	void			OnClanListDetailInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Clan Match (GameSessionClanMatch.cpp)
	void			OnClanMatchTeamContextReq(char* pData, INT32 i32Size);
	void			OnClanMatchTeamCreateReq(char* pData, INT32 i32Size);
	void			OnClanMatchTeamJoinReq(char* pData, INT32 i32Size);
	void			OnClanMatchTeamLeaveReq(char* pData, INT32 i32Size);
	void			OnClanMatchAllTeamListReq(char* pData, INT32 i32Size);
	void			OnClanMatchFightRequestReq(char* pData, INT32 i32Size);
	void			OnClanMatchFightAcceptReq(char* pData, INT32 i32Size);
	void			OnClanMatchChatReq(char* pData, INT32 i32Size);
	void			SendClanMatchTeamInfo(int teamIdx);
	void			CreateClanMatchRoom(struct ClanMatchTeam* pTeam1, struct ClanMatchTeam* pTeam2);
	void			SendClanMatchRoomNotify(struct ClanMatchTeam* pTeam, int channel, int roomIdx);

	// Packet handlers - Clan War 1.5 (GameSessionClanWar.cpp)
	void			OnClanWarCreateTeamReq(char* pData, INT32 i32Size);
	void			OnClanWarJoinTeamReq(char* pData, INT32 i32Size);
	void			OnClanWarLeaveTeamReq(char* pData, INT32 i32Size);
	void			OnClanWarTeamListReq(char* pData, INT32 i32Size);
	void			OnClanWarMatchmakingReq(char* pData, INT32 i32Size);
	void			OnClanWarCancelMatchmakingReq(char* pData, INT32 i32Size);
	void			OnClanWarTeamChatReq(char* pData, INT32 i32Size);
	void			OnClanWarChangeMaxPerReq(char* pData, INT32 i32Size);
	void			OnClanWarMercenaryListReq(char* pData, INT32 i32Size);
	void			OnClanWarResultReq(char* pData, INT32 i32Size);
	void			OnClanWarRegistMercenaryReq(char* pData, INT32 i32Size);
	void			OnClanWarRemoveMercenaryReq(char* pData, INT32 i32Size);
	void			OnClanWarInviteMercenaryReq(char* pData, INT32 i32Size);
	void			OnClanWarInviteAcceptReq(char* pData, INT32 i32Size);
	void			OnClanWarInviteDenialReq(char* pData, INT32 i32Size);
	void			OnClanWarCreateRoomReq(char* pData, INT32 i32Size);
	void			OnClanWarJoinRoomReq(char* pData, INT32 i32Size);
	void			OnClanWarLeaveRoomReq(char* pData, INT32 i32Size);
	void			OnClanWarMercenaryDetailInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Clan extras (GameSessionClan.cpp)
	void			OnClanInviteAcceptReq(char* pData, INT32 i32Size);
	void			OnClanCancelRequestReq(char* pData, INT32 i32Size);
	void			OnClanReplaceNameReq(char* pData, INT32 i32Size);
	void			OnClanNoteReq(char* pData, INT32 i32Size);
	void			OnClanReplacePersonmaxReq(char* pData, INT32 i32Size);
	void			OnClanCheckJoinAuthorityReq(char* pData, INT32 i32Size);
	void			OnClanCheckMarkReq(char* pData, INT32 i32Size);

	// Packet handlers - Auth/Social extras (GameSessionSocial.cpp)
	void			OnAuthGetMyInfoReq(char* pData, INT32 i32Size);
	void			OnAuthFriendRoomEnterReq(char* pData, INT32 i32Size);
	void			OnAuthFriendRoomLeaveReq(char* pData, INT32 i32Size);
	void			OnWhisperFindUIDReq(char* pData, INT32 i32Size);
	void			OnAuthRecvWhisperReq(char* pData, INT32 i32Size);
	void			OnAuthUsedWeaponReq(char* pData, INT32 i32Size);
	void			OnAuthFCMInfoReq(char* pData, INT32 i32Size);

	// Packet handlers - Base extras (GameSession.cpp)
	void			OnBaseUrlListReq(char* pData, INT32 i32Size);
	void			OnBaseServerInfoReq(char* pData, INT32 i32Size);
	void			OnBaseClientEndTypeReq(char* pData, INT32 i32Size);
	void			OnBaseUserTitleInfoReq(char* pData, INT32 i32Size);
	void			OnBaseQuestFinishNoticeReq(char* pData, INT32 i32Size);
	void			OnBaseRegistTrainingScoreReq(char* pData, INT32 i32Size);
	void			OnBaseGetUserListReq(char* pData, INT32 i32Size);
	void			OnBaseChangeClanNameReq(char* pData, INT32 i32Size);
	void			OnBaseChangeClanMarkReq(char* pData, INT32 i32Size);

	// Packet handlers - Shop extras (GameSessionShop.cpp)
	void			OnShopGetGiftListReq(char* pData, INT32 i32Size);
	void			OnShopExpireDeleteItemReq(char* pData, INT32 i32Size);
	void			OnShopRewardItemReq(char* pData, INT32 i32Size);
	void			OnShopSaleCouponListReq(char* pData, INT32 i32Size);
	void			OnShopAuthGiftReq(char* pData, INT32 i32Size);
	void			OnShopNotifyGiftReq(char* pData, INT32 i32Size);

	// Packet handlers - Battle extras (GameSessionBattle.cpp)
	void			OnBattleStartKickVoteReq(char* pData, INT32 i32Size);
	void			OnBattleCheatMessageReq(char* pData, INT32 i32Size);

	// Packet handlers - Roulette/Gacha (GameSessionRoulette.cpp)
	void			OnRouletteEnterReq(char* pData, INT32 i32Size);
	void			OnRouletteLeaveReq(char* pData, INT32 i32Size);
	void			OnRouletteItemInfoReq(char* pData, INT32 i32Size);
	void			OnRouletteStartReq(char* pData, INT32 i32Size);
	void			OnRouletteJackpotNotifyReq(char* pData, INT32 i32Size);
	void			OnFieldShopOpenReq(char* pData, INT32 i32Size);
	void			OnFieldShopGoodsListReq(char* pData, INT32 i32Size);

	// Packet handlers - Gacha (0x1400) (GameSessionRoulette.cpp)
	void			OnGachaItemInfoReq(char* pData, INT32 i32Size);
	void			OnGachaShopStateReq(char* pData, INT32 i32Size);
	void			OnGachaEnterReq(char* pData, INT32 i32Size);
	void			OnGachaLeaveReq(char* pData, INT32 i32Size);
	void			OnGachaPurchaseReq(char* pData, INT32 i32Size);
	void			OnGachaGetPurchaseCountReq(char* pData, INT32 i32Size);

	// Packet handlers - MyInfo (0x1900) (GameSessionRoulette.cpp)
	void			OnMyInfoEnterReq(char* pData, INT32 i32Size);
	void			OnMyInfoLeaveReq(char* pData, INT32 i32Size);

	// Packet handlers - GM Commands (GameSessionGM.cpp - Phase 11A)
	// GM Chat handlers (GameSessionGMChat.cpp)
	void			OnGMChatStartReq(char* pData, INT32 i32Size);
	void			OnGMChatSendReq(char* pData, INT32 i32Size);
	void			OnGMChatEndReq(char* pData, INT32 i32Size);
	void			OnGMChatPenaltyReq(char* pData, INT32 i32Size);

	void			OnGMKickUserReq(char* pData, INT32 i32Size);
	void			OnGMExitUserReq(char* pData, INT32 i32Size);
	void			OnGMDestroyRoomReq(char* pData, INT32 i32Size);
	void			OnLobbyGMExitUserReq(char* pData, INT32 i32Size);
	void			OnGMBlockUserReq(char* pData, INT32 i32Size);
	void			OnGMPauseBattleReq(char* pData, INT32 i32Size);
	void			OnGMResumeBattleReq(char* pData, INT32 i32Size);

	// GM Cheat/Debug commands (Phase 13)
	void			OnCheatIncreaseKillReq(char* pData, INT32 i32Size);
	void			OnCheatPlaySoloReq(char* pData, INT32 i32Size);
	void			OnCheatReduceRoundTimeReq(char* pData, INT32 i32Size);
	void			OnCheatTeleportReq(char* pData, INT32 i32Size);

	// GM helpers
	bool			IsGMUser() const;
	void			SendServerAnnounce(const char* pszMessage, uint16_t ui16MsgLen);
	bool			ProcessAdminCommand(const char* pszMessage, int i32MsgLen);

	// Cheat detection (Phase 9C - GameSessionCheat.cpp)
	bool			ValidateDamage(int i32KillerSlot, int i32VictimSlot, uint32_t ui32WeaponId,
								uint8_t ui8HitPart, int i32Damage);
	bool			ValidateSpeedHack(DWORD dwClientTime);
	bool			ValidatePosition(float fX, float fY, float fZ);
	bool			ValidateGrenadeUse(uint32_t ui32WeaponId, float fX, float fY, float fZ);
	void			OnCheatDetected(int i32CheatType, const char* pszDescription);

	// Boost event info (Phase 14B)
	void			SendBoostEventInfo();

	// Attendance login check (Phase 14A)
	void			CheckAttendanceOnLogin();

	// Title info (Phase 4D)
	void			SendTitleInfo();

	// Rank helpers (Phase 10)
	void			CheckRankUp();

	// Helpers
	void			SendConnectAck();
	void			SendHeartBitAck();
	void			SendLoginAck(int i32Result);
	void			SendSimpleAck(uint16_t protocolAck, int32_t result);
	void			SendGamePacket(char* pBuffer, int i32Size);	// Encrypt + send
	void			ResetSessionData();

	// Battle helpers
	void			SendSlotInfoToAll();
	void			SendBattleEndToAll(int i32WinnerTeam);

private:
	// State machine
	GameTask		m_eMainTask;

	// Identity
	int64_t			m_i64UID;
	char			m_szUsername[64];
	char			m_szNickname[64];
	uint32_t		m_ui32EncriptKey;		// XOR encryption key
	uint32_t		m_ui32AuthToken;		// Token from ConnectServer

	// Location
	int				m_i32ChannelNum;		// Current channel (-1 = none)
	int				m_i32RoomIdx;			// Current room index (-1 = none)
	int				m_i32SlotIdx;			// Slot in room (-1 = none)
	Room*			m_pRoom;

	// Player data
	int				m_i32Level;
	int64_t			m_i64Exp;
	int				m_i32Cash;
	int				m_i32GP;
	int				m_i32RankId;
	int				m_i32ClanId;
	char			m_szClanName[64];
	uint8_t			m_ui8AuthLevel;			// 0=normal, 1+=GM
	bool			m_bDamageConsole;		// GM debug: show damage events

	// Stats
	int				m_i32Kills;
	int				m_i32Deaths;
	int				m_i32Headshots;
	int				m_i32Wins;
	int				m_i32Losses;

	// Battle state
	uint32_t		m_stUsedWeapon;		// Last used weapon ID in battle

	// SLOT_BONUS tracking (Phase 1C)
	int				m_i32LastBattleExpBonus;		// Last battle EXP bonus breakdown total
	int				m_i32LastBattleGPBonus;		// Last battle GP bonus breakdown total

	// Equipment (7E) - Character slots with weapon/parts loadouts
	uint8_t			m_ui8ActiveCharaSlot;					// Current active character slot (0-4)
	GameCharaSlot	m_CharaSlots[MAX_CHARA_SLOT];			// Up to 5 character loadouts

	// Inventory (7F) - Full inventory
	GameInventoryItem	m_Inventory[MAX_INVEN_COUNT];
	int				m_i32InventoryCount;

	// Quest (7I) - Quest card sets and progress
	GameQuestData	m_QuestData;

	// Social (7K) - Friends and block list
	GameFriendInfo	m_Friends[MAX_FRIEND_COUNT];
	int				m_i32FriendCount;
	GameBlockInfo	m_BlockList[MAX_BLOCK_COUNT];
	int				m_i32BlockCount;

	// Notes/Mail
	GameNoteData	m_Notes[MAX_NOTE_COUNT];
	int				m_i32NoteCount;
	uint32_t		m_ui32NextNoteId;

	// Medal (7J) - Medal progress and sets
	GameMedalData	m_MedalData;

	// Event/Attendance (7N)
	GameAttendanceData	m_AttendanceData;
	GameDailyRecord		m_DailyRecord;

	// Skill (7O) - Character class skills
	GameSkillData	m_SkillData;

	// Title (4D) - Titles/Designations
	GameTitleData	m_TitleData;

	// Timing
	DWORD			m_dwConnectTime;
	DWORD			m_dwLastPacketTime;

	// Rate limiting (Phase 9A)
	DWORD			m_dwRateLimitWindow;
	uint16_t		m_ui16PacketCount;

	// Replay detection (Phase 9A) - track last N protocol IDs+timestamps
	#define REPLAY_HISTORY_SIZE		16
	struct ReplayEntry
	{
		uint16_t	ui16Protocol;
		DWORD		dwTime;
		uint32_t	ui32DataHash;	// Simple hash of first 8 bytes of data
	};
	ReplayEntry		m_ReplayHistory[REPLAY_HISTORY_SIZE];
	int				m_i32ReplayIdx;
	int				m_i32ReplayDetections;

	// Cheat detection (Phase 9C)
	DWORD			m_dwCheatLastClientTime;
	DWORD			m_dwCheatLastServerTime;
	int				m_i32SpeedHackCount;
	int				m_i32CheatViolations;
	float			m_fLastPosX;
	float			m_fLastPosY;
	float			m_fLastPosZ;
};

#endif // __GAMESESSION_H__
