#pragma once

class GachaContext : public i3::shared_ginst<GachaContext>
{
public:
	enum EANNOUNCE_TYPE
	{
		EANNOUNCE_NOTICE		= 0,
		EANNOUNCE_WINITEMUSER,
	};

	GachaContext();
	~GachaContext();
	
	void						SetGachaData(INT32 iTab, GACHA_GROUP_INFO * pInfo )		
	{ 
		I3_BOUNDCHK(iTab,GACHA_GROUP_COUNT); 
		m_GachaReceived[iTab] = *pInfo;
	}
	GACHA_GROUP_INFO*			SetGachaData(INT32 iTab)		{ I3_BOUNDCHK(iTab,GACHA_GROUP_COUNT); return &m_GachaReceived[iTab];}
	const GACHA_GROUP_INFO*		GetGachaData(INT32 iTab) const	{ I3_BOUNDCHK(iTab,GACHA_GROUP_COUNT); return &m_GachaReceived[iTab];}
	GACHA_GROUP_INFO*			GetGachaData(INT32 iTab)		{ I3_BOUNDCHK(iTab,GACHA_GROUP_COUNT); return &m_GachaReceived[iTab];}
	const GACHA_ITEM_INFO*		GetGachaItemInfo(GACHA_GROUP grp, INT32 itemKey) const;


	// Gacha Result Data

	void						SetGachaRstHeader(const PACKET_GACHA_RESULT_HEADER* inRstHeader) { m_gachaRstHeader = *inRstHeader; }
	const PACKET_GACHA_RESULT_HEADER* GetGachaRstHeader(){ return &m_gachaRstHeader; }

	void						PushGachaRstData(const PACKET_GACHA_RESULT_DATA* inRstData) { m_gachaRstData.push_back(*inRstData); }

	void						GetGachaRstData(INT32 index, PACKET_GACHA_RESULT_DATA* out) { *out = m_gachaRstData[index]; }
	INT32						GetGachaRstDataCount() const {return m_gachaRstData.size();}
	INT32						GetGachaRstGoodsItemKey() const { return GET_ITEMKEY_FROM_GACHA_RST_GOODSID(m_gachaRstHeader.m_TGoodsID); }
	GACHA_GROUP					GetGachaRstGoodsGroup() const { 									
									UINT32 grp = (GACHA_GROUP)GET_GROUP_FROM_GACHA_RST_GOODSID(m_gachaRstHeader.m_TGoodsID);
									if(grp > 0)
									{ grp = grp-1; // GET_GROUP_FROM_GACHA_RST_GOODSID로 얻는 그룹번호는1부터시작
									  if(grp < GACHA_GROUP_COUNT) 
										  return (GACHA_GROUP)grp; 
									}									
									return GACHA_GROUP_NONE;
								}

	struct GACHA_REAWARD
	{
		GACHA_RESULT			m_result = GACHA_RESULT_NONE;
		T_GoodsID				m_rewardGoodsID = 0;
	};

	void						PushGachaReward(const GACHA_REAWARD& in) { m_gachaReward.push(in); }
	bool						PopGachaReward(GACHA_REAWARD& out);
	INT32						GetGachaRewardCnt() { return m_gachaReward.size(); }
	void						ClearGachaReward() 
	{ 
		for(INT32 i=0; i<(INT32)m_gachaReward.size(); i++) 
			m_gachaReward.pop(); // clear()함수는 어디에..
	}

	void						ClearGachaRst();

	void						SetGachaBuyType(UINT8 istype)	{ m_GachaBuyType = istype; }
	UINT8						GetGachaBuyType() const			{ return m_GachaBuyType; }

	void						SetGachaBuyCashPrice(INT32 price)	{ m_GachaBuyCashPrice = price; }
	INT32						GetGachaBuyCashPrice() const		{ return m_GachaBuyCashPrice; }

	void						SetGachaBuyPointPrice(INT32 price)	{ m_GachaBuyPointPrice = price; }
	INT32						GetGachaBuyPointPrice() const		{ return m_GachaBuyPointPrice; }

	void						SetGachaBuyGoodId(UINT32 price)	{ m_GachaBuyGoodId = price; }
	UINT32						GetGachaBuyGoodId() const		{ return m_GachaBuyGoodId; }

	void						SetGachaBuyCoupon(INT32 coupon)	{ m_GachaBuyCoupon = coupon; }
	INT32						GetGachaBuyCoupon()	const		{ return m_GachaBuyCoupon; }

	void						SetGachaNotice(const i3::rc_wstring& wstr_notice)	{ m_wstrGachaNotice = wstr_notice; }
	const i3::rc_wstring&		GetGachaNotice() const			{ return m_wstrGachaNotice; }

	void						SetGachaLuckyUser(const i3::rc_wstring& user)	{ m_wstrGachaLuckyUser = user; }
	const i3::rc_wstring&		GetGachaLuckyUser()	const		{ return m_wstrGachaLuckyUser; }

	void						SetGachaLuckyGoodsId(INT32 itemid) { m_GachaLuckyGoodsId = itemid; }
	INT32						GetGachaLuckyGoodsId()	const		{ return m_GachaLuckyGoodsId; }
	const i3::wstring			GetGachaLuckyGoodsName() const;

	void						SetMyBuy(bool buy) { m_bisMyBuy = buy; }
	bool						GetMyBuy() { return m_bisMyBuy; }
	void						SetJackpotAnnounceChat();
	void						ShowJackpotAnnounceChat(UINT32 itemID);

	
	bool						FindGachaShopFile();
	bool						OpenGachaShopFile();
	void						LoadGachaShopFile(void);

	void						SetGachaShopMD5Key(char* strShopMD5Key);
	bool						CreateGachaShopFile();
	bool						SaveGachaShopFile();
	bool						DeleteGachaShopFile();
	void						MakeGachaShopMD5Key(void);
	char*						GetGachaShopMD5Key(void)
	{  
		// 실행중에 ShopFile이 변경될 수 도 있으니 다시한번 MD5Key를 생성합니다.
		MakeGachaShopMD5Key(); 
		return m_strGachaShopMD5Key; 
	};

private:
	GACHA_GROUP_INFO			m_GachaReceived[GACHA_GROUP_COUNT];

	PACKET_GACHA_RESULT_HEADER	m_gachaRstHeader;
	i3::vector<PACKET_GACHA_RESULT_DATA> m_gachaRstData;
	i3::queue<GACHA_REAWARD>	m_gachaReward;

	i3::vector<i3::rc_wstring>	m_RecvLuckyItemText;
	i3::vector<UINT32>			m_RecvLuckyItemId;

	UINT8						m_GachaBuyType = 0;							//구매 타입
	INT32						m_GachaBuyCashPrice = 0;					//구매 캐시 가격
	INT32						m_GachaBuyPointPrice = 0;					//구매 포인트 가격
	INT32						m_GachaBuyCoupon = 0;						//구매 쿠폰가격
	INT32						m_GachaBuyGoodId = 0;						//구매 굿지아이디
	i3::rc_wstring				m_wstrGachaNotice;						//공지상

	i3::rc_wstring				m_wstrGachaLuckyUser;					//공지사항럭키유져
	UINT32						m_GachaLuckyGoodsId = 0;					//공지사항 럭키 아이템
	bool						m_bisMyBuy = false;								//내가 구매 한 것인지.

	HANDLE						m_hGachaShopFile = INVALID_HANDLE_VALUE;
	char						m_strGachaShopMD5Key[NET_MD5_KEY_SIZE + 1] = { 0 };

};

