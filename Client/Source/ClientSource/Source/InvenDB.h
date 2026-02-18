#pragma once

class iInvenDB
{
public:
	virtual ~iInvenDB();

protected:
	virtual void ReReferenceUsage_impl() = 0;

public:
	virtual void AddInvenBufferToList(INVEN_BUFFER* pBuffer,INT32 nCount);
	virtual void UpdateInvenBufferToList(INVEN_BUFFER* pBuffer);
	virtual void ReReferenceUsage();

public:
	i3::vector<INVEN_BUFFER*> m_Main_list;
};

/***************/
class WeaponDB : public iInvenDB
{
public:
	virtual ~WeaponDB();

private:
	virtual void ReReferenceUsage_impl() override;

public:
	enum DBType { Primary, Secondary, Melee, Throw, Item, Max };
	i3::vector<INVEN_BUFFER*> m_Ref_list[Max];

	const i3::vector<INVEN_BUFFER*>* get_list(WEAPON_SLOT_TYPE nUsageType) const;
};

/***************/
class CharacterDB : public iInvenDB
{
public:
	virtual ~CharacterDB();

private:
	virtual void ReReferenceUsage_impl() override;

public:
	enum DBType { Chara, Dino, Max };
	i3::vector<INVEN_BUFFER*> m_Ref_list[Max];

	INT32	m_nCharacterSlotCount;			//	디폴트 캐릭터는 하나의 ID지만 레드/블루 2가지로 나뉘므로 이것도 반영된 갯수입니다. (장비는 제외된 갯수) komet

	const i3::vector<INVEN_BUFFER*>* get_list(CHARACTER_CLASS_TYPE nUsageType) const;
};

/***************/
class PartsDB : public iInvenDB
{
public:
	virtual ~PartsDB();

private:
	virtual void ReReferenceUsage_impl() override;

public:
	// 베레모는 헤드기어와 InvenDB 타입을 공유합니다 (차후 기획 변경 시 별도로 분리 필요.)
	enum DBType { Headgear, Facegear, Upper, Lower, Glove, Belt, Holster, Max, Beret = Headgear };	
	i3::vector<INVEN_BUFFER*> m_Ref_list[Max];

	const i3::vector<INVEN_BUFFER*>* get_list(CHARACTER_CLASS_TYPE nUsageType) const;
};

class SkinDB : public iInvenDB
{
private:
	i3::vector<INVEN_BUFFER*>	m_Ref_list;

	virtual void ReReferenceUsage_impl() override;

public:
	virtual ~SkinDB() {}
	const i3::vector<INVEN_BUFFER*>*	get_list( UINT32 usageType) const;
};

/***************/
class CashItemDB : public iInvenDB
{
public:
	virtual ~CashItemDB();

private:
	virtual void ReReferenceUsage_impl() override;

private:
	bool is_spread_item(const INVEN_BUFFER* pInvenBuffer);

public:
	enum DBType { Maintenance, Expendable, CashDino, Max };
	i3::vector<INVEN_BUFFER*> m_Ref_list[Max];

	i3::vector<INVEN_BUFFER*> m_Spreaded_Main_list; //보급상자를 쪼개서 보여 주기 위한 아이템 리스트

	const i3::vector<INVEN_BUFFER*>* get_list(ITEM_TYPE nUsageType) const;
};

/***************/
class CouponBufferDB
{
public:
	struct COUPON_BUFFER : INVEN_BUFFER
	{
		bool m_bUsed;
	};

public:
	virtual ~CouponBufferDB();

public:
	virtual void AddInvenBufferToList(INVEN_BUFFER* pBuffer,INT32 nCount);
	virtual void UpdateInvenBufferToList(INVEN_BUFFER* pBuffer);

public:
	i3::vector<CouponBufferDB::COUPON_BUFFER*> m_Main_list;
};
