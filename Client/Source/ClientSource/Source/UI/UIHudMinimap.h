#pragma once

#include "iHudBase.h"

#include "BattleHUD_Define.h"

class UIHudManager;
class CBattleHUD_MiniMap;
class CHud;

class UIHudMinimap : public iHudBase
{
public:
	UIHudMinimap(UIHudManager* p);
	virtual ~UIHudMinimap();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;

private:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) override;

public:
	void	SetMiniMapLocationText( const char* pszLocation);
	void SetObjectUnderAttack(INT32 idx, bool bFlag);

private:
	i3UIFrameWnd*	m_pAreaFrmWnd;
	i3UIStaticText* m_pAreaName;
};