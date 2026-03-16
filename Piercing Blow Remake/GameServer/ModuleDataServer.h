#ifndef __MODULEDATASERVER_H__
#define __MODULEDATASERVER_H__

#pragma once
#include "ModuleBase.h"
#include "InterServerProtocol.h"

class GameSession;

// Modulo de comunicacion con DataServer
// Envia requests de carga/guardado de jugadores, recibe responses
class ModuleDataServer : public ModuleBase
{
public:
	ModuleDataServer();
	virtual ~ModuleDataServer();

	// Request operations - Player core
	void		RequestPlayerLoad(int i32SessionIdx, int64_t i64UID);
	void		RequestPlayerSave(int64_t i64UID, int i32Level, int64_t i64Exp, int i32Cash, int i32GP);
	void		RequestStatsSave(int64_t i64UID, int i32Kills, int i32Deaths,
								 int i32Headshots, int i32Wins, int i32Losses);
	void		RequestCreateNick(int64_t i64UID, int i32SessionIdx, const char* pszNickname);
	void		RequestCheckNick(int i32SessionIdx, const char* pszNickname);

	// Request operations - Equipment
	void		RequestEquipmentSave(int64_t i64UID, uint8_t ui8CharaSlot, uint8_t ui8SlotState,
									 uint32_t ui32CharaId, const uint32_t* pWeaponIds, const uint32_t* pPartsIds);

	// Request operations - Medal
	void		RequestMedalSave(int64_t i64UID, uint16_t ui16MedalIdx, uint8_t ui8ActionId,
								 uint16_t ui16Count, uint8_t ui8GetReward);

	// Request operations - Attendance
	void		RequestAttendanceSave(int64_t i64UID, int i32TotalDays, int i32CurrentStreak,
									  uint32_t ui32LastDate, const uint8_t* pDays);

	// Request operations - Skill
	void		RequestSkillSave(int64_t i64UID, uint8_t ui8ClassId,
								 const uint8_t* pMainLevels, const uint8_t* pAssistLevels,
								 const uint8_t* pCommonLevels, uint16_t ui16Points);

	// Request operations - Quest
	void		RequestQuestSave(int64_t i64UID, uint8_t ui8SetIndex, uint8_t ui8SetType,
								 uint8_t ui8ActiveCard, const char* pQuestData, uint16_t ui16DataSize);

	// Request operations - Clan
	void		RequestClanCreate(int64_t i64MasterUID, int i32SessionIdx,
								  const char* pszClanName, const char* pszMasterNick);
	void		RequestClanDisband(int i32ClanId, int64_t i64MasterUID);
	void		RequestClanJoin(int i32ClanId, int64_t i64UID, int i32SessionIdx,
								const char* pszNickname, uint8_t ui8Level);
	void		RequestClanLeave(int i32ClanId, int64_t i64UID);

	// Request operations - Friends
	void		RequestFriendAdd(int64_t i64UID, int64_t i64FriendUID, int i32SessionIdx);
	void		RequestFriendRemove(int64_t i64UID, int64_t i64FriendUID);
	void		RequestFriendList(int64_t i64UID, int i32SessionIdx);

	// Request operations - Block
	void		RequestBlockAdd(int64_t i64UID, int64_t i64BlockedUID, int i32SessionIdx);
	void		RequestBlockRemove(int64_t i64UID, int64_t i64BlockedUID);
	void		RequestBlockList(int64_t i64UID, int i32SessionIdx);

	// Request operations - Shop
	void		RequestShopList();

protected:
	// ModuleBase overrides
	virtual bool	OnConnect() override;
	virtual void	OnDisconnect() override;
	virtual void	OnProcessPacket(char* pData, int i32Size) override;
	virtual void	OnHeartbeat() override;

private:
	// Response handlers - Player core
	void		OnPlayerLoadAck(char* pData, int i32Size);
	void		OnPlayerSaveAck(char* pData, int i32Size);
	void		OnCreateNickAck(char* pData, int i32Size);
	void		OnCheckNickAck(char* pData, int i32Size);
	void		OnStatsSaveAck(char* pData, int i32Size);

	// Response handlers - Game data
	void		OnEquipmentSaveAck(char* pData, int i32Size);
	void		OnMedalSaveAck(char* pData, int i32Size);
	void		OnAttendanceSaveAck(char* pData, int i32Size);
	void		OnSkillSaveAck(char* pData, int i32Size);
	void		OnQuestSaveAck(char* pData, int i32Size);

	// Response handlers - Clan
	void		OnClanCreateAck(char* pData, int i32Size);
	void		OnClanDisbandAck(char* pData, int i32Size);
	void		OnClanJoinAck(char* pData, int i32Size);
	void		OnClanLeaveAck(char* pData, int i32Size);

	// Response handlers - Social
	void		OnFriendAddAck(char* pData, int i32Size);
	void		OnFriendRemoveAck(char* pData, int i32Size);
	void		OnFriendListAck(char* pData, int i32Size);
	void		OnBlockAddAck(char* pData, int i32Size);
	void		OnBlockRemoveAck(char* pData, int i32Size);
	void		OnBlockListAck(char* pData, int i32Size);

	// Response handlers - Shop
	void		OnShopListAck(char* pData, int i32Size);

	// Helper: build and send a packet with a struct body
	void		SendRequest(uint16_t ui16Protocol, const void* pStruct, int i32StructSize);
};

extern ModuleDataServer* g_pModuleDataServer;

#endif // __MODULEDATASERVER_H__
