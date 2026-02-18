#pragma once

#include "iHudBase.h"

enum BATTLEUSEITEM_ICON
{
	BATTLEUSEITEM_NONE = -1,

	BATTLEUSEITEM_GRAVITY_RESTORE = 0,
	BATTLEUSEITEM_UNBEATABLE,
	BATTLEUSEITEM_HP_RECOVERY,
	BATTLEUSEITEM_BARRET,
	BATTLEUSEITEM_RPG7,

	BATTLEUSEITEM_MAX,

};

class UIHudManager;

class UIHudBattleUseItemNotice : public iHudBase
{
public:
	UIHudBattleUseItemNotice(UIHudManager *p);
	virtual ~UIHudBattleUseItemNotice();

public:
	virtual void InitializeAtLoad(i3UIScene * pScene) override;
	virtual void Update(REAL32 rDeltaSeconds) override;

private:
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

private:
	i3::rc_wstring			_GetItemNameAndItemType(T_ItemID itemID, BATTLEUSEITEM_ICON& shapeIdx);

private:
	i3UIButtonImageSet*		m_pBoxImage;
	i3UIStaticText*			m_pText;

	i3::rc_wstring			m_String;

	REAL32					m_rNoticeTime;
	REAL32					m_rImageTime;
	bool					m_bNoticeEnable;
};