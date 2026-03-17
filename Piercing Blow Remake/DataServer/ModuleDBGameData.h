#ifndef __MODULEDBGAMEDATA_H__
#define __MODULEDBGAMEDATA_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;

// Modulo de operaciones de datos de juego
// Maneja: equipment, medals, medal sets, attendance, skills, quests,
//         cosmetics, mode records, daily records, maps, map stats,
//         rankings, server config, notices, boost events,
//         shop (extended), coupons, gifts
class ModuleDBGameData
{
public:
	ModuleDBGameData(DBConnectionPool* pPool);
	~ModuleDBGameData();

	// Equipment (extendido con IDX linking)
	bool SaveEquipment(int64_t i64UID, IS_EQUIPMENT_SAVE_REQ* pReq);

	// Medals (individual)
	bool SaveMedal(int64_t i64UID, IS_MEDAL_SAVE_REQ* pReq);

	// Medal Sets
	bool SaveMedalSet(int64_t i64UID, IS_MEDAL_SET_SAVE_REQ* pReq);
	int  LoadMedalSets(int64_t i64UID, IS_MEDAL_SET_CURRENT_ENTRY* pOut, int i32MaxCount);

	// Attendance
	bool SaveAttendance(int64_t i64UID, IS_ATTENDANCE_SAVE_REQ* pReq);

	// Skills
	bool SaveSkill(int64_t i64UID, IS_SKILL_SAVE_REQ* pReq);

	// Quests
	bool SaveQuest(int64_t i64UID, IS_QUEST_SAVE_REQ* pReq, const char* pQuestData, int i32DataSize);

	// Account Cosmetics
	bool LoadCosmetics(int64_t i64UID, IS_COSMETICS_LOAD_ACK* pOut);
	bool SaveCosmetics(int64_t i64UID, IS_COSMETICS_SAVE_REQ* pReq);

	// Mode Records (per game mode stats)
	int  LoadModeRecords(int64_t i64UID, IS_MODE_RECORD_ENTRY* pOut, int i32MaxCount);
	bool SaveModeRecord(int64_t i64UID, IS_MODE_RECORDS_SAVE_REQ* pReq);

	// Daily Records
	bool LoadDailyRecord(int64_t i64UID, IS_DAILY_RECORD_ENTRY* pOut);
	bool SaveDailyRecord(int64_t i64UID, IS_DAILY_RECORD_SAVE_REQ* pReq);

	// Map System
	int  LoadMapModes(IS_MAP_MODE_ENTRY* pOut, int i32MaxCount);
	bool SaveMapStats(IS_MAP_STATS_SAVE_REQ* pReq);

	// Rankings
	int  LoadRankingsIndividual(uint8_t ui8TermType, IS_RANKING_INDIVIDUAL_ENTRY* pOut, int i32MaxCount);
	int  LoadRankingsClan(uint8_t ui8TermType, IS_RANKING_CLAN_ENTRY* pOut, int i32MaxCount);
	bool CalculateRankings();

	// Server Config / Notices / Boost Events
	int  LoadServerConfig(IS_SERVER_CONFIG_ENTRY* pOut, int i32MaxCount);
	int  LoadNotices(IS_NOTICE_ENTRY* pOut, int i32MaxCount);
	int  LoadBoostEvents(IS_BOOST_EVENT_ENTRY* pOut, int i32MaxCount);

	// Shop catalog (extendido)
	int  LoadShopItems(IS_SHOP_ITEM_ENTRY* pOut, int i32MaxCount);
	void BuyShopItem(IS_SHOP_BUY_REQ* pReq, IS_SHOP_BUY_ACK* pAck);
	bool UpdateInventory(IS_INVEN_UPDATE_REQ* pReq);

	// Shop Coupons
	bool RedeemCoupon(IS_SHOP_COUPON_REQ* pReq, IS_SHOP_COUPON_ACK* pAck);

	// Gift Delivery
	bool SendGift(IS_GIFT_SEND_REQ* pReq);
	int  LoadGifts(int64_t i64UID, IS_GIFT_ENTRY* pOut, int i32MaxCount);
	bool ReceiveGift(int64_t i64UID, int64_t i64GiftId, IS_GIFT_RECEIVE_ACK* pAck);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBGAMEDATA_H__
