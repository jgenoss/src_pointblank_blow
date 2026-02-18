#pragma once

#include "UIHUDBase.h"
#include "i3Base/itl/function/function.h"

namespace observer_hud
{
	enum EVENT_TYPE
	{
		EVT_ENABLE,
		EVT_TARGET_CHANGED,
		EVT_GIVEUP_BATTLE,
		EVT_CHATBOX_APPEARED,
		EVT_WEAPON_CHANGED,
		EVT_BULLETCOUNT_CHANGED,
		EVT_HP_CHANGED,
		EVT_KILL,
		EVT_DEATH,
		EVT_HELMETPROTECTION,
		EVT_ASSIST,
		EVT_HAZEMODE,
		EVT_MAX_COUNT,
	};

	class View
	{
	public:
		View(i3UIScene* scene);
		virtual ~View();

		void OnNotify(const EVENT_TYPE evt, const INT32 arg);
		virtual bool SameOfUserIdx(INT32 Idx) { return false; }
	protected:
		void None(INT32 arg) {}

		i3UIScene* m_scene;

		typedef i3::function<void(INT32)> fun;
		fun m_funcList[EVT_MAX_COUNT];
	};

	class View_ActiveUserInfo : public View
	{
	public:
		View_ActiveUserInfo(i3UIScene* scene);
		virtual ~View_ActiveUserInfo();

		void TurnOn(const INT32 arg);
		void SelectUser(const INT32 arg);
		void Rearrange(const INT32 arg);
		void Reposition(const INT32 arg);
		void ChangeWeaponName(const INT32 arg);
		void ChangeHPGauge(const INT32 arg);
		void ChangeOnDead(const INT32 arg);
		void ChangeKillCount(const INT32 arg);

	private:
		i3UIFrameWnd* GetFrameWnd(const CHARACTER_TEAM_TYPE teamType) const;
		void MoveFrameWnd(const bool chatboxAppeared, const CHARACTER_TEAM_TYPE teamType);

		i3UIFrameWnd* m_frameWnd_R;
		i3UIFrameWnd* m_frameWnd_B;
		REAL32 m_origposY;
		INT32 m_currUserIdx;
	};

	class View_ObservedUserInfo : public View
	{
	public:
		View_ObservedUserInfo(i3UIScene* scene);
		virtual ~View_ObservedUserInfo();

		void TurnOn(const INT32 arg);
		void Refresh(const INT32 arg);
		void ChangeWeaponInfoText(const INT32 arg);
		void ChangeBulletCountText(const INT32 arg);
		void ChangeHPGauge(const INT32 arg);
		void ChangeOnDead(const INT32 arg);
		void ChangeKillCount(const INT32 arg);
		void ChangeAssistCount(const INT32 arg);
		void HideHelmetProtectionImg(const INT32 arg);
		void HazeMode(const INT32 arg);
		virtual bool SameOfUserIdx(INT32 Idx) override { return m_currUserIdx == Idx ? true : false; }

	private:
		i3UIFrameWnd* m_frameWnd;
		i3UIImageBoxEx* m_clanmarkCtrl;
		INT32 m_currUserIdx;
	};

	class Context : public i3::shared_ginst<Context>
	{
	public:
		Context();
		virtual ~Context();

		void Register(View* view);
		void Clear();
		void Notify(const EVENT_TYPE evt, const INT32 arg);
		bool SameOfUserIdx(INT32 Idx);
	private:
		i3::vector<View*> m_viewList;
	};
}

class UIHUDObserver : public UIHUDBase
{
	I3_CLASS_DEFINE(UIHUDObserver, UIHUDBase);

public:
	UIHUDObserver();
	virtual ~UIHUDObserver();

	virtual void _InitializeAtLoad(i3UIScene* scene) override;
	virtual void _ClearAtUnload(i3UIScene* scene) override;
	virtual void OnCreate(i3GameNode * parent) override;
	virtual bool OnExitStart() override;
	virtual void OnUpdate(REAL32 tm) override;

private:
	INT32 _CheckSomeoneHPChanged();
	INT32 _CheckBulletCountChanged();

	i3UIScene* m_scene;
	INT32 m_hpList[SLOT_MAX_COUNT];
	INT32 m_bulletCountList[SLOT_MAX_COUNT];
};