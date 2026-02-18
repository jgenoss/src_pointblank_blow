#pragma once
//#if !defined(__OBJECT_ITEMBOX_H__)
//#define __OBJECT_ITEMBOX_H__

#include "GameControledObject.h"

//#define ITEMBOX_ITEM_MAXCOUNT 255
#define ITEMBOX_ITEM_MAXCOUNT 5

class CGameObjectBattleUseItem : public CGameControledObject
{
private:
	I3_EXPORT_CLASS_DEFINE(CGameObjectBattleUseItem, CGameControledObject);
	
public:
	enum OBJECT_USE_ITEM_TYPE
	{
		OBJECT_USE_ITEM_NONE = 0,
		OBJECT_USE_ITEM_WEAPON,
		OBJECT_USE_ITEM_HPUP,
		OBJECT_USE_ITEM_RECOVERY_GRAVITY,
		OBJECT_USE_ITEM_UNBEATABLE,

		OBJECT_USE_ITEM_MAX,
	};

	enum OBJECT_USE_ITEM_STATE
	{
		OBJECT_USE_ITEM_STATE_OFF = 0,
		OBJECT_USE_ITEM_STATE_ON = 1,

		OBJECT_USE_ITEM_STATE_MAX,
	};

public:
	CGameObjectBattleUseItem();
	virtual ~CGameObjectBattleUseItem();

	virtual void	CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnQueryGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;

	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;

#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject(REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState(INT32 state) override;
	virtual INT32	GetControledObjectState(void) override { return -1; }

public:
	void			UseItem(const UINT8 nItemNumber, CGameCharaBase * pChara, UINT8 nSyncNumber = 0);

	void			SetItemCount(INT32 count);
	void			SetItemID(INT32 idx, INT32 itemid);
	void			SetItemProbability(INT32 idx, INT32 prob);
	void			SetStartTime(REAL32 starttime);
	void			SetRespawnTime(REAL32 resTime);

public:
	INT32			GetItemCount() { return m_iItemCount; }
	INT32			GetItemId(INT32 idx) { return (0 <= idx && idx < ITEMBOX_ITEM_MAXCOUNT) ? m_ItemID[idx] : -1; }
	INT32			GetItemProbability(INT32 idx) { return (0 <= idx && idx < ITEMBOX_ITEM_MAXCOUNT) ? m_ItemProbability[idx] : -1; }
	
	REAL32			GetStartTime() { return m_rStartTime;  }
	REAL32			GetRespawnTime() { return m_rRespawnTime;  }

private:
	void					_ChangeWeapon(T_ItemID itemID, CGameCharaBase * pChara, UINT8 nSyncNumber);
	OBJECT_USE_ITEM_TYPE	_GetUseItemType(INT32 UseItemidx);

	void					_AnotherTeamGravityRecovery(TEAM_TYPE myteam);

	void					_SystemMessage(INT32 SlotIdx, const T_ItemID ItemID);

protected:
	// Item Á¤º¸µé
	INT32				m_iItemCount;

	INT32				m_ItemID[ITEMBOX_ITEM_MAXCOUNT];
	INT32				m_ItemProbability[ITEMBOX_ITEM_MAXCOUNT];

	// Respawn
	REAL32				m_rStartTime;
	REAL32				m_rRespawnTime;

	INT32				m_iEffectidx;

	UINT8				m_bOnOff;
};

//#endif