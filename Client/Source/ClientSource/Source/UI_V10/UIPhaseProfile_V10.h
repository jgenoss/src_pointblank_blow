#if !defined( __UI_PHASE_PROFILE_V10_H__)
#define __UI_PHASE_PROFILE_V10_H__

#include "UI/UIPhaseSubBase.h"

class UITabProfileDesignation_V10;
class UITabProfileMissionCard_V10;
class UITabProfileBase_V10;

enum PROFILETAB_V10
{
	PROFILETAB_NONE_V10 = -1,
	PROFILETAB_START_V10 = 0,
	PROFILETAB_MISSIONCARD_V10 = PROFILETAB_START_V10,
	PROFILETAB_DESIGNATION_V10,

	PROFILETAB_MAX
};

class UIPhaseProfile_V10 : public UIPhaseSubBase
{
	I3_CLASS_DEFINE( UIPhaseProfile_V10, UIPhaseSubBase);

private:
	UITabProfileDesignation_V10 *	m_pDesignation;
	UITabProfileMissionCard_V10 *	m_pMissionCard;
	i3UIScene *						m_pCurrentScene;
	i3UIButton*						m_pBtnGeneral;
	PROFILETAB_V10					m_CurrentTab;
	
public:
	UIPhaseProfile_V10();
	virtual ~UIPhaseProfile_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	SetInputDisable(bool bDisable) override;

	virtual void	OnLoadAllScenes() override;
	virtual bool	OnExitStart() override;
	virtual void	OnExitEnd( void) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnKey_Escape() override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	Leave( void) override;

	virtual bool	OnClick( i3UIControl * pControl);

public:
	void			InitTab();
	void			ChangeTab( PROFILETAB_V10 tab);

	UITabProfileDesignation_V10	*	GetDesignation() { return m_pDesignation; }
	UITabProfileMissionCard_V10 *	GetMissionCard() { return m_pMissionCard; }

	//--------------------------------------------------------------------------//
	//								Mission card								//
	//--------------------------------------------------------------------------//
	//void			ClickMissionCard( INT32 pack);
	//void			ViewMissionCardInfo( INT32 s

	//--------------------------------------------------------------------------//
	//								Designation									//
	//--------------------------------------------------------------------------//
	//void			ClickDesignation( INT32 slot);


};

#endif
