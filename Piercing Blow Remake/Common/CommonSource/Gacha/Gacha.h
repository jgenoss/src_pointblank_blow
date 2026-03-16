#pragma once

#include "../../CommonStructure/ShareClient/ProtocolGacha.h"
#include "../../../Server/Source/CommonStructure/ShareServer/ProtocolServerGacha.h"


class CRandomItem
{
public:
	UINT32		GetGoodsID()					{ return m_ui32GoodsID; }
	INT32		GetProbability()				{ return m_i32Probability; }

	UINT32		SetGoodsID(UINT32 ui32GoodsID)	{ m_ui32GoodsID = ui32GoodsID; }
	INT32		SetProbability(INT32 i32Prob)	{ m_i32Probability = i32Prob; }

	void		SetRandomItemInfo(GACHA_RANDOM_ITEM* pRandomItem);
	void		MakeRandomItemPacket(GACHA_RANDOM_ITEM* pRandomItem);


public:
	CRandomItem() : m_ui32GoodsID(INVALID_GOODS_ID), m_i32Probability(0)	{}
	virtual ~CRandomItem()	{}


private:
	UINT32		m_ui32GoodsID;
	INT32		m_i32Probability;
};



//////////////////////////////////////////////////////////////////////////
//	CGachaItem
class CGachaItem
{
public:
	void				ResetWinningIndex();
	bool				IsWinningWin();
	UINT32				GetWinningRandomGoodsID();

	INT32				GetItemKey()										{ return m_i32ItemKey; }
	INT32				GetRemainAmount()									{ return m_i32RemainAmount; }
	UINT32				GetGoodsID()										{ return m_ui32GoodsID; }
	INT32				GetRandomItemCount()								{ return m_i32RandomItemCnt; }

	void				SetRemainAmount(INT32 i32RemainCount)				{ m_i32RemainAmount = i32RemainCount; }	
	
	void				SetGachaItemInfo(SS_GACHA_ITEM_INFO* pGachaItem, GACHA_GROUP eGroup);

	void				MakeGachaItemInfoPacket(GACHA_ITEM_INFO* pGachaItem);
	void				MakeGachaItemInfoPacket(SS_GACHA_ITEM_INFO* pGachaItem);

	bool				IsWinningGlory(INT32 i32Purchase)						{ return m_i32GloryCount != INVALID_GLORY_CNT && m_i32GloryCount <= i32Purchase; }	

public:
	CGachaItem();
	virtual ~CGachaItem();


private:
	INT32				m_i32ItemKey;
	UINT32				m_ui32GoodsID;
	
	INT32				m_i32RemainAmount;
	INT32				m_i32TotalAmount;
	INT32				m_i32WinningIdx;

	INT32				m_arWinningProbs[PROB_SECTION_COUNT];

	INT32				m_i32RandomItemCnt;
	CRandomItem			m_arRandomItems[RANDOM_ITEM_COUNT_BY_ITEM];

	INT32				m_i32GloryCount;
};



//////////////////////////////////////////////////////////////////////////
//	CGachaGroup
class CGachaGroup
{
public:
 	UINT32				IsWinningLucky();
	GACHA_RESULT		IsWinningGacha(INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32Purchase, OUT UINT32& ui32WinningGoodsID);

	GACHA_GROUP			GetGachaGroupType()										{ return m_eGachaItemGroup; }
	INT32				GetItemKey(INT32 i32ItemIdx);
	INT32				GetRemainAmount(INT32 i32ItemIdx);

	void				SetRemainAmount(INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32RemainCount);

	void				MakeGachaInfoPacket(SS_GACHA_GROUP_INFO* pGachaGroup);
	void				MakeGachaInfoPacket(GACHA_GROUP_INFO* pGachaGroup);
	void				MakeGachaStatePacket(GACHA_GROUP_STATE* pPacket);
	void				MakeGachaStatePacketForClient(i3NetworkPacket* pPacket);
 
	void				SetGachaGroupInfo(SS_GACHA_GROUP_INFO* stGachaGroup);
	void				UpdateGachaState(GACHA_GROUP_STATE* pGachaState);

	bool				IsValidRequest(INT32 i32ItemIdx, INT32 i32ItemKey);
	void				Initialize();
	
private:
	//bool				IsWinningGlory(INT32 i32Purchase)						{ return m_i32GloryItemCount != INVALID_GLORY_CNT && m_i32GloryItemCount <= i32Purchase; }	
	void				ResetLuckyWinningIdx();

	bool				CheckItemIndexValidation(INT32 i32ItemIndex);

public:
	CGachaGroup();
	virtual ~CGachaGroup();


private:
	
	GACHA_GROUP			m_eGachaItemGroup;

	UINT32				m_ui32LuckyGoodsID;
	INT32				m_i32LuckySalesVolume;
	INT32				m_i32LuckyProbability;
	INT32				m_i32LuckyWinningIdx;

	//INT32				m_i32GloryItemCount;

	INT32				m_i32WinItemCount;
	CGachaItem			m_arWinItems[WIN_ITEM_COUNT_BY_TAB];

	INT32				m_i32RandomItemCnt;
	UINT32				m_arRandomGoodsIDs[RANDOM_ITEM_COUNT_BY_TAB];
};



//////////////////////////////////////////////////////////////////////////
//	CGachaShop
class CGachaShop
{
public:
	GACHA_RESULT		IsWinningGacha(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32Purchase, OUT UINT32& ui32WinningGoodsID);
 	UINT32				IsWinningLucky(GACHA_GROUP eGachaGroup, TTCHAR* szCharName);
	UINT32				GetWinningRandomGoodsID(GACHA_GROUP eGachaGroup, UINT32 ui32GoodsID);	

	INT32				GetItemKey(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx);
	INT32				GetRemainAmount(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx);

	INT32				GetNoticeCount()						{ return m_i32NoticeCnt; }
	void				InsertNotice(TTCHAR* szNotice);

	void				SetLuckyUser(TTCHAR* szUserName, UINT32 ui32LuckyGoodsID);
	TTCHAR*				GetLuckyUser(void)						{ return m_szLuckyUser; }
	T_GoodsID			GetLuckyGoodsID(void)					{ return m_ui32LuckGoodsID; }

	void				SetLuckyTime(void) { m_ui32LuckyTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(); }
	UINT32				GetLuckyTime(void) { return m_ui32LuckyTime; }

	bool				IsValidRequest(T_GoodsID TGoodsID, INT32 i32ItemIdx);
	void				Initialize(GACHA_GROUP eGachaGroup);

	GACHA_GROUP			GetGachaGroupType(GACHA_GROUP eGachaGroup) { return m_arGachaGroups[eGachaGroup].GetGachaGroupType(); }

	// Packet functions
	void				MakeGachaStatePacket(PACKET_GACHA_SHOP_STATE* pPacket);
	void				MakeGachaStatePacketForClient(GACHA_GROUP eGroup, i3NetworkPacket* pPacket);
	void				MakePacketGachaInfo(GACHA_GROUP eGachaGroup, SS_GACHA_GROUP_INFO* pGachaGroup);
	void				MakePacketGachaInfo(GACHA_GROUP eGachaGroup, GACHA_GROUP_INFO* pGachaGroup);
	void				MakePacketGachaNotice(INT32 i32Index, PACKET_GACHA_NOTICE* pGachaNotice);
	
	void				SetGachaGroupInfo(SS_GACHA_GROUP_INFO* pGachaGroup);
	void				UpdateGachaState(PACKET_GACHA_SHOP_STATE* pShopState);
	void				SetGachaRemainCount(GACHA_GROUP eGachaGroup, INT32 i32ItemIdx, INT32 i32ItemKey, INT32 i32RemainCount);

	BOOL				CreateGachaShopFile();
	BOOL				SaveGachaShopFile();
	BOOL				DeleteGachaShopFile();
	void				MakeGachaShopMD5Key(void);
	char*				GetGachaShopMD5Key(void)					{  return m_strGachaShopMD5Key; };

	void				SetGachaShopMD5Key(char* strShopMD5Key)
	{
		i3String::Copy( m_strGachaShopMD5Key, strShopMD5Key, NET_MD5_KEY_SIZE+1); 
		m_strGachaShopMD5Key[NET_MD5_KEY_SIZE] = 0;
	}

	GACHA_GROUP			GetGachaGroupByGoodsID( T_GoodsID TGoodsID)
	{
		INT32 i32SubType = GET_GOODS_SUBTYPE( TGoodsID ) - 1;
		if( i32SubType < GACHA_GROUP_POOR ) return GACHA_GROUP_NONE;
		if( i32SubType >= GACHA_GROUP_COUNT ) return GACHA_GROUP_NONE;
		return (GACHA_GROUP)i32SubType;
	}

private:
	bool				CheckGroupValidation(GACHA_GROUP eGachaGroup);


public:
	CGachaShop();
	virtual ~CGachaShop()	{}


private:
	HANDLE				m_hGachaShopFile;
	char				m_strGachaShopMD5Key[NET_MD5_KEY_SIZE+1];

	CGachaGroup			m_arGachaGroups[GACHA_GROUP_COUNT];

	INT32				m_i32NoticeCnt;
	TTCHAR				m_szNotices[GACHA_NOTICE_COUNT][GACHA_NOTICE_LENGTH];

	TTCHAR				m_szLuckyUser[NET_CHARA_NICK_NAME_SIZE];
	T_GoodsID			m_ui32LuckGoodsID;
	UINT32				m_ui32LuckyTime;			// ¿Ë∆Ã¥Á√∑Ω√∞£
};
