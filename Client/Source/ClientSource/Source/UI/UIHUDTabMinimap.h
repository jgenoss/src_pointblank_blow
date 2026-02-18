#ifndef __UI_HUD_TABMINIMAP_H__
#define __UI_HUD_TABMINIMAP_H__

#include "UIHUDBase.h"

#define MAX_TABMINIMAP_CELL_COUNT	10
#define MAX_TABMINIMAP_STRING_CELL_COUNT	5
#define MAX_QUEST_PER_MISSON_CARD	4

namespace minimap_new
{
	class Minimap;
	class Unit;
}

struct MissonCard
{
	i3UIStaticText*			MC_Contents;
	i3UIButtonImageSet*		MC_ClearButton;
};

class UIHUDTabMinimap : public UIHUDBase
{
	I3_CLASS_DEFINE(UIHUDTabMinimap, UIHUDBase);

public:
	UIHUDTabMinimap();
	virtual ~UIHUDTabMinimap();
	
	virtual void	_InitializeAtLoad(i3UIScene* pScene) override;

	virtual void	OnCreate(i3GameNode* nodeParent) override;
	virtual void	OnUpdate(REAL32 deltaSeconds) override;
	virtual bool	OnEntranceStart(void* arg1 = nullptr, void* arg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;
	void			Show(bool activate);

public:
	static void		cbSetClanTexturForTabMinimap(i3UIListView_Cell * pCell, void * pData);

private:
	void	_CreateMinimap(i3UIScene* scene);
	void	_CreateUnits();
	void	_ReserveTeamRankingList();

	void	_InitDefaultInfo();
	void	_SetRoomLocation(i3::stack_wstring& wstrOut);
	
	void	_UpdateUserInfo(bool BATTLE_EVNET_FLAG = false);
	void	_UpdateTeamScore();
	void	_UpdateRestTeamGoalInfo();

	void	_SetIndivisualInfo(INT32 lvSlot, INT32 slotIdx);

	void	_UpdateWhenNecessary();
	void	_UpdateRespawn(INT32 slot);

	bool	_IsMinimapUpdatable() const  { return m_bMinimapUpdatable;  }
	bool	_IsUserInfoUpdatable() const { return m_bUserInfoUpdatable; }
	bool	_IsActivated() const		 { return m_bActivate;			}

	void	_InitMC(i3UIScene* scene);
	void	_UpdateMC();

private:
	bool				m_bActivate;
	bool				m_bMinimapUpdatable;
	bool				m_bUserInfoUpdatable;

	i3UIControl *		m_pRedTeamListView;
	i3UIControl *		m_pBlueTeamListView;

	i3::vector<minimap_new::Unit*> m_units;
	i3::vector<INT32> m_ranking[TEAM_COUNT];

	bool				m_bActivateMC;
	MissonCard			m_MC[MAX_QUEST_PER_MISSON_CARD];
};
 
#endif