#ifndef __SHOP_CONTEXT_H
#define __SHOP_CONTEXT_H

#include "UI_V11/ShopContextBasket.h"

namespace i3 { class user_data; }

enum SHOP_SORT_CATEGORY
{
	SHOP_SORT_CATEGORY_NEWHOT = 0,
	SHOP_SORT_CATEGORY_WEAPON,
	SHOP_SORT_CATEGORY_ITEM,
	SHOP_SORT_CATEGORY_MAX,
};
enum SHOP_SORT_TYPE
{
	SHOP_SORT_TYPE_NORMAL = 0,
	SHOP_SORT_TYPE_NEW,
	SHOP_SORT_TYPE_HOT,
	SHOP_SORT_TYPE_NAME,
	SHOP_SORT_TYPE_DAMAGE,
	SHOP_SORT_TYPE_RAPID,
	SHOP_SORT_TYPE_SPEED,
	SHOP_SORT_TYPE_POINT,
	SHOP_SORT_TYPE_CASH,
	SHOP_SORT_TYPE_MAX,
};

enum
{
	FILTER_CHARA_TYPE = 0,
	FILTER_CHARA_TYPE_NORMAL,
	FILTER_CHARA_TYPE_RED,
	FILTER_CHARA_TYPE_BLUE,
	FILTER_CHARA_TYPE_HEADGEAR = 7,
};

class CShopContext : public i3::shared_ginst<CShopContext>
{
private:
	UINT32 m_nUserSelectedGoodsId;

	//	서버에서 받아온 상품 목록을 Weapon, Chara, Item 별로 분류하여 복사해 둡니다.
	//	서버로부터 상품 목록이 변경될 경우 상품 목록을 다시 만들어야 합니다.
	//	앞으로 상점GUI에서는 이 목록으로만 가지고 처리를 합니다.
	//	SHOP_GOODS_PACK* 가 들어있음.
	i3::vector<SHOP_GOODS_PACK*> m_pGoodsList[SHOP_GOODS_MAX];

	GAME_EVENT	m_nGameEvent;
	INT32 m_nEventArg;

	bool m_bEventLock;
	bool m_bInServer;
	
	bool m_bChangedChara;
	bool m_bChangedWeapon;

	SHOP_GOODS * m_pSelectedGoods;

	// 캡슐 아이템 인덱스
	INT32				m_nCapsuleItemIdx;				// 게임컨텍스트에서 일단 이리로 이동한다..

	bool	m_bisBuildGoodsList;	// revision 53188 BuildGoodsList에 있던 static local 변수 제거, 멤버 변수로 빼냄

	// 상점 태그 정렬 순서.
	i3::vector<UINT8> m_tagPriorities;
	i3::vector<UINT8> m_tagPrioritiesType;
	SHOP_ITEM_TYPE m_ItemTagType;


#ifdef I3_DEBUG
	void	DumpAllGoodsInGameContext();
	void	DumpGoodsList(SHOP_GOODS_TYPE goodsType = SHOP_GOODS_UNKNOWN);
	void	_DumpGoodsList( SHOP_GOODS_TYPE goodsType);
#endif

public:
	CShopContext(void);
	virtual ~CShopContext(void);

	void EnterServer(void);
	void LeaveServer(void);
	bool InServer(void)	const		{ return m_bInServer; }

	void BuildGoodsList(void);
	void ClearGoodsList(void);
	void SetGoodsList( bool bBuild)	{ m_bisBuildGoodsList = bBuild; }	// revision 54070

	const i3::vector<SHOP_GOODS_PACK*>& GetShopList(SHOP_GOODS_TYPE type)	{ I3ASSERT(0 <= type && SHOP_GOODS_MAX > type); return m_pGoodsList[type]; }
	i3::vector<SHOP_GOODS_PACK*>& SetShopList(SHOP_GOODS_TYPE type)			{ I3ASSERT(0 <= type && SHOP_GOODS_MAX > type); return m_pGoodsList[type]; }

	const SHOP_GOODS_PACK * GetGoodsPack(SHOP_GOODS_TYPE type, INT32 index);
	const SHOP_GOODS_PACK * GetGoodsPack( T_ItemID itemID);	
	const SHOP_GOODS * GetSelectedGoods(void) const				{ return m_pSelectedGoods; }
	const SHOP_GOODS * GetGoods( T_ItemID itemID);

	//1.1 상점을 위하여 const 안붙어 있는 함수를 만듭니다.
	//

	INT32	GetGoodsCount(SHOP_GOODS_TYPE type);
	INT32	GetGoodsPackCount(SHOP_GOODS_TYPE type);

	//	상품을 구입합니다.
	bool	CheckBuy(UINT32 GoodsID, UINT32 PriceType, UINT8 price_idx, GOODS_PRICE_TYPE tabPrice = GOODS_PRICE_ALL);
	
	/** \brief 상품 장비
		\return 1 : Chara, 2 : Weapon*/
	EQUIP::GOODS	EquipGoods(const SHOP_GOODS * pGoods, bool bUpdateViewModel = true);

	void AddTagPriority(const UINT8 priority);
	void SetTagPriorityType(const SHOP_ITEM_TYPE priority);
	UINT8 GetTagPriority(const SHOP_ITEM_TYPE type) const;

	// 수리 관련
public:
	bool	IsDurabilityItem( T_ItemID ItemID) const;

	bool	GetDurability( T_ItemID ItemID, REAL32& DurableRate) const;
	bool	GetDurability( T_ItemID ItemID, T_ItemDBIdx ware_idx, REAL32& DurableRate) const;
	// 내구도 비율때문에 내부에서 계산해주는 방식을 사용해야함
	bool GetDurability( T_ItemID ItemID, UINT32 & Current, UINT32 & Max) const; //Parts가 두개 이상일 경우 유효한 값을 보장 못 함
	bool GetDurability( T_ItemID ItemID,T_ItemDBIdx ware_idx, UINT32 & Current, UINT32 & Max) const; //인벤용

	bool	GetRepairPrice( T_ItemID ItemID, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const;
	bool	GetRepairPrice( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const; //인벤용

	REPAIR_TYPE	CheckCanRepair( T_ItemID ItemID) const;
	REPAIR_TYPE	CheckCanRepair( T_ItemID ItemID, T_ItemDBIdx ware_idx) const;

	bool RepairItem( T_ItemID ItemID);
	bool RepairItem( T_ItemID ItemID, T_ItemDBIdx ware_idx);

	void OnGameEvent(INT32 event,INT32 arg, const i3::user_data& UserData);
	void PopGameEvent(GAME_EVENT * pOutEvent, INT32 * pOutArg)	
	{ 
		*pOutEvent = m_nGameEvent; 
		*pOutArg = m_nEventArg;

		m_nGameEvent = EVENT_NOTHING; 
		m_nEventArg = 0;
	}

	INT32				getCapsuleItemIdx(void)								{ return m_nCapsuleItemIdx;	}
	void				setCapsuleItemIdx(INT32 idx)						{ m_nCapsuleItemIdx = idx;	}

	/** \brief 아이템 아이디나 굿즈아이디를 아이템 이름으로 컨버트합니다. */
	void GetGoodsName(i3::rc_wstring& Name, UINT32 nGoodsid, T_ItemID nItemid);
	/** \brief 아이템 아이디나 굿즈아이디를 아이템 이름으로 컨버트합니다. */
	INT32 GetGoodsUIShapeIdx(UINT32 nGoodsid, T_ItemID nItemid);
	/** \brief 굿즈 아이디를 아이템 아이디로 컨버트합니다. */
	INT32	ConvertGoodsID(UINT32 nGoodsId);
	INT32	FindItemCountOrPeriod(const UINT32 goodsID) const;

protected:
	void _AddGoodsToShopList(SHOP_GOODS_TYPE type, UINT32 nAddGoodsId);

private:
	void getClanInfo();

};


#endif