#ifndef __SHOP_ITEM_INFO_H
#define __SHOP_ITEM_INFO_H

#include "i3Base/string/rc_string.h"
#include "UIGachaDef.h"

enum PURCHASE_AUTHORITY
{
	PURCHASE_AUTH_ANYONE,
	PURCHASE_AUTH_CLAN_MASTER,

	MAX_PURCHASE_AUTHORITY,
};

enum USE_POSITION
{
	USE_POS_ANYWHERE,
	USE_POS_GAME,
	USE_POS_CLAN_MANAGEMENT,
	USE_POS_READYROOM,
	USE_POS_CHATTING,

	MAX_USE_POSITION,
};

enum DataBaseType
{
	DBT_ShopItem, //무기, 캐릭터, 파츠, 뽑기(상점 구매 가능)
	DBT_SetItem, //세트 아이템(상점 구매 가능)

	DBT_Domi_Item, //탈출 미션용 아이템(상점 구매 가능)
	DBT_Domi_Cashltem, //탈출 미션용 아이템(상점 구매 가능)
	DBT_Domi_Spltem, //탈출 미션용 아이템(상점 구매  불가능)

	DBT_UITemplate,
	
	DBT_Max,
};

//------------------------------------////------------------------------------//
namespace Capsule
{
	struct PrizeInfo
	{
		i3::vector<i3::rc_wstring> prize_string_list;
	};
	Capsule::PrizeInfo* read_info(i3RegKey* pKey);
}

//------------------------------------////------------------------------------//
class ShopBaseInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( ShopBaseInfo, i3NamedElement);

public:
	ShopBaseInfo(void);
	virtual ~ShopBaseInfo(void);

public:
	virtual void ReadInfo(i3RegKey * pKey);
	
	const i3::rc_wstring&	GetScriptName() const { return m_Name; }
	const i3::rc_wstring&	GetDescription(void) const { return m_Description; }
	i3::rc_wstring			GetCapsuleItemResultName(INT32 Idx) const;
	bool					IsCouponType() const { return m_isCouponType;  }

protected:
	virtual void ReadInfo_impl(i3RegKey * pKey) {}

protected:
	i3::rc_wstring m_Name, m_Description;
	Capsule::PrizeInfo* pCapsulePrizeInfo;
	bool m_isCouponType;
};


//------------------------------------////------------------------------------//
class ShopItemInfo : public ShopBaseInfo
{
	I3_CLASS_DEFINE( ShopItemInfo, ShopBaseInfo);

	friend class ShopItemInfoDataBase;

protected:
	virtual void ReadInfo_impl( i3RegKey * pKey) override;

public:
	ShopItemInfo(void);
	virtual ~ShopItemInfo(void);

public:
	ITEM_TYPE GetType(void) const
	{
		if( m_Type == 0 ) return ITEM_TYPE_WRAP_PERIOD; //CASHITEM_USAGE_MAINTENANCE(0)->ITEM_TYPE_WRAP_PERIOD(17)
		if( m_Type == 1 ) return ITEM_TYPE_WRAP_COUNT; //CASHITEM_USAGE_EXPENDABLES(1)->ITEM_TYPE_WRAP_COUNT(16)
		return m_Type;				
	}
	INT32					GetGroupIndex(void) const	{ return m_GroupIndex;			}
	INT32					GetTexIndex(void) const		{ return m_UITextureIndex;		}
	INT32					GetShapeIndex(void) const		{ return m_UIShapeIndex;		}

	const i3::rc_wstring&	GetResultStatic(void) const	{ return m_ResultStatic; }
	const i3::rc_wstring&	GetResultValue(void) const	{ return m_ResultValue; }
	const i3::rc_wstring&	GetResultNotice(void) const	{ return m_ResultNotice; }

	const i3::rc_wstring&	GetResultMethod() const		{ return m_ResultMethod; }
	const i3::rc_wstring&	GetUseMethod() const		{ return m_UseMethod; }

	PURCHASE_AUTHORITY			GetPurchaseAuthority() const			{ return m_PurchaseAuthority; }
	const i3::rc_wstring&		GetPurchaseAuthorityDesc() const		{ return m_PurchaseAuthorityDesc; }
	const i3::rc_wstring&		GetCertificationAuthorityDesc() const	{ return m_CertificationAuthDesc; }

	USE_POSITION				GetUsePosition() const				{ return m_UsePosition; }
	const i3::rc_wstring&		GetUsePositionDesc() const			{ return m_UsePositionDesc; }

	bool				IsViewOtherUser() const				{ return m_ViewOtherUser; }
	INT32	GetCatagory(void)	const	{ return m_Catagory;		}

private:
	ITEM_TYPE m_Type;						//	타입
	INT32 m_GroupIndex;				// 그룹인덱스 CASHITEM_GROUP_TYPE가 저장됩니다.
	INT32 m_UITextureIndex;
	INT32 m_UIShapeIndex;
	INT32 m_Catagory;	//상품 Type Catagory

	i3::rc_wstring					m_ResultStatic;	//	아이템 적용 내용
	i3::rc_wstring					m_ResultValue;	//	아이템 적용 값
	i3::rc_wstring					m_ResultNotice;	//	아이템 결과 메시지

	i3::rc_wstring					m_ResultMethod;
	i3::rc_wstring					m_UseMethod;

	PURCHASE_AUTHORITY				m_PurchaseAuthority;
	i3::rc_wstring					m_PurchaseAuthorityDesc;
	i3::rc_wstring					m_CertificationAuthDesc;
	
	USE_POSITION					m_UsePosition;
	i3::rc_wstring					m_UsePositionDesc;

	bool m_ViewOtherUser;
};

//------------------------------------////------------------------------------//
class ShopPackageGoodsInfo: public ShopBaseInfo
{
	I3_CLASS_DEFINE( ShopPackageGoodsInfo, ShopBaseInfo);

	friend class ShopItemInfoDataBase;

private:
	virtual void ReadInfo_impl( i3RegKey * pKey) override;

public:
	ShopPackageGoodsInfo(void);
	virtual ~ShopPackageGoodsInfo(void);

	const i3::rc_wstring& GetType(void) const 		{ return m_TexType;		}
	INT32	GetGoodsId(void)	const	{ return m_GoodsID;		}
	INT32	GetImageIndex(void)	const	{ return m_ShapeIndex;		}
	INT32	GetTexIndex(void)	const	{ return m_TexIndex;		}
	INT32	GetCatagory(void)	const	{ return m_Catagory;		}

private:
	i3::rc_wstring m_TexType;	// 상품 Type
	INT32 m_GoodsID;	// 상품 Id
	INT32	m_ShapeIndex;	// 상품 Image Index
	INT32	m_TexIndex;	// 상품 Tex Index
	INT32 m_Catagory;	//상품 Type Catagory
};

//------------------------------------////------------------------------------//
class DomiShopItemInfo : public ShopItemInfo
{
	I3_CLASS_DEFINE( DomiShopItemInfo, ShopItemInfo);

	friend class ShopItemInfoDataBase;

	enum { Resurrect_ItemType = 0, };

private:
	virtual void ReadInfo_impl( i3RegKey * pKey) override;

public:
	DomiShopItemInfo(void) : m_StartRound(0) {}
	virtual ~DomiShopItemInfo(void) {}

	INT32 GetDisplayIndex() const { return m_DisplayIndex;}
	INT32 GetStartRound() const { return m_StartRound;}
	INT32 GetItemType() const { return m_ItemType;}

private:
	INT32 m_StartRound, m_DisplayIndex, m_ItemType;
};

//------------------------------------////------------------------------------//
class DomiSpItemInfo : public ShopBaseInfo
{
	I3_CLASS_DEFINE( DomiSpItemInfo, ShopBaseInfo);

	friend class ShopItemInfoDataBase;

private:
	virtual void ReadInfo_impl( i3RegKey * pKey) override;

public:
	DomiSpItemInfo(void) {}
	virtual ~DomiSpItemInfo(void) {}

	INT32 GetDisplayIndex() const { return m_DisplayIndex;}
	INT32 GetShapeIndex() const { return m_ShapeIndex;}

private:
	INT32 m_DisplayIndex, m_ShapeIndex;
};

//------------------------------------////------------------------------------//
class DomiCashItemInfo : public ShopBaseInfo
{
	I3_CLASS_DEFINE( DomiCashItemInfo, ShopBaseInfo);

	friend class ShopItemInfoDataBase;

	enum { SpCash_ItemType = 0, };

private:
	virtual void ReadInfo_impl( i3RegKey * pKey) override;

public:
	DomiCashItemInfo(void) {}
	virtual ~DomiCashItemInfo(void) {}

	INT32 GetDisplayIndex() const { return m_DisplayIndex;}
	INT32 GetShapeIndex() const { return m_ShapeIndex;}
	INT32 GetSpIncrease() const { return m_Sp_Increase;}
	INT32 GetGroupIndex() const { return m_GroupIndex; }
	INT32 GetItemType() const { return m_ItemType;}
	INT32 GetMinRemain() const { return m_MinRemain;}

private:
	INT32 m_DisplayIndex, m_ShapeIndex, m_GroupIndex;
	INT32 m_Sp_Increase, m_MinRemain;
	INT32 m_ItemType;
};

//------------------------------------////------------------------------------//
class CUITemplateInfo_Item : public ShopBaseInfo
{
	I3_CLASS_DEFINE(CUITemplateInfo_Item, ShopBaseInfo);

	friend class ShopItemInfoDataBase;

public:
	CUITemplateInfo_Item();
	virtual ~CUITemplateInfo_Item();

	INT32 GetType() const;
	const i3::rc_wstring& GetName() const;

private:
	virtual void ReadInfo_impl(i3RegKey* rk) override;
	INT32 m_type;
	i3::rc_wstring m_name;
};

//------------------------------------////------------------------------------//

class ShopItemInfoDataBase : public i3GameNode
{
	I3_CLASS_DEFINE( ShopItemInfoDataBase, i3GameNode);

	friend class DomiShopItemInfo;
	friend class DomiCashItemInfo;

public:
	ShopItemInfoDataBase();
	virtual ~ShopItemInfoDataBase();

	bool Create();

	//Shop Item, Set Item, Domi Item 사용
	ShopItemInfo* getInfo( T_ItemID item_id);
	void GetItemName( T_ItemID item_id, i3::rc_wstring& outName);
	void GetItemDescription( T_ItemID item_id, i3::rc_wstring& outName);

	//Set Item 전용
	ShopPackageGoodsInfo *	GetPackageGoodsInfo(INT32 GoodsID);
	bool CheckPackageGoodsGroupInfo( INT32 GoodsID);

	DomiCashItemInfo* find_domi_cash_item_info(INT32 display_idx);
	DomiSpItemInfo* find_domi_sp_item_info(INT32 display_idx);

	const i3::vector<DomiShopItemInfo*> & get_insg_domi_item_info_list() const { return m_Insg_Domi_ItemInfo_List; }
	const i3::vector<DomiCashItemInfo*> & get_insg_domi_cash_item_info_list() const { return m_Insg_Domi_Cash_ItemInfo_List; }
	
	size_t GetUITemplateCount() const;
	ShopBaseInfo* GetUITemplateInfo(size_t i) const;

private:
	//pef파일 정보엔 ItemId가 없기 때문에 data를 찾기 위해서는 type, group_index가 필요하다.
	void change_condition_for_finding_in_pef_data( T_ItemID item_id, ITEM_TYPE & type, INT32 &group_index); 

private:
	i3::vector<ShopBaseInfo*> m_InfoList[DBT_Max];

private:
	i3::vector<DomiShopItemInfo*> m_Insg_Domi_ItemInfo_List; //단순 포인터 참조용이라 release 안 해도 된다.
	i3::vector<DomiCashItemInfo*> m_Insg_Domi_Cash_ItemInfo_List; //단순 포인터 참조용이라 release 안 해도 된다.

public:
#if !defined( I3_NO_PROFILE)
	virtual void	OnReloadProperty() override;
#endif
};

//------------------------------------////------------------------------------//
class ColorNickPalette : public i3::shared_ginst<ColorNickPalette>
{
public:
	ColorNickPalette();
	virtual ~ColorNickPalette();

	void Load();

	const I3COLOR& GetColor(const size_t row, const size_t col) const;
	const I3COLOR& GetColor(const UINT8 colorIndex) const;

	size_t GetNumOfRow() const { return m_numOfRow; }
	size_t GetNumOfCol() const { return m_numOfCol; }

private:
	size_t m_numOfRow;
	size_t m_numOfCol;
	i3::vector<I3COLOR> m_colors;
};

class ColorFireMuzzleFlash : public i3::shared_ginst<ColorFireMuzzleFlash>
{
public:
	ColorFireMuzzleFlash();
	virtual ~ColorFireMuzzleFlash();
	
	void Load();
	const I3COLOR& GetColor(const size_t type, const size_t Index) const;
	const I3COLOR& GetColor(const UINT16 colorIndex) const;

private:
	size_t m_numOfRow;
	size_t m_numOfCol;
	i3::vector<I3COLOR> m_colors;
}; 


class GachaItem : public i3::shared_ginst<GachaItem>
{
public:
	enum EUISPRITE_SIZ
	{
		EUISPRITE_SIZ_1024,
		EUISPRITE_SIZ_512,
		EUISPRITE_SIZ_MAX
	};

	GachaItem();
	virtual ~GachaItem();

	void Load();

	const VEC2D& GetItemSprScale(const size_t idx, T_ItemID id) const;
	const VEC3D& GetItemSprRotation(const size_t idx, T_ItemID id) const;

	static EUISPRITE_SIZ ResolveItemSprSizType(T_ItemID id);

private:
	struct ItemSprInfo
	{
		VEC2D m_scaleGachaView[GACHA_POS_MAX];
		VEC3D m_rotGachaView[GACHA_POS_MAX];
	};

	typedef i3::vector<ItemSprInfo*>		ItemSprInfoVec;
	ItemSprInfoVec							m_itemSprInfoAry;
};
#endif