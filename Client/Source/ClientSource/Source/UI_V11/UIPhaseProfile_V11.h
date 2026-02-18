#if !defined( __UI_PHASE_PROFILE_H__)
#define __UI_PHASE_PROFILE_H__

//----------------------------------------------------------------------------------//
//	개인정보
//	└ 임무
//	└ 호칭
//	└ 전적
//	└ 무기전적
//----------------------------------------------------------------------------------//

#include "UI/UIPhaseSubBase.h"

class UITabProfileDesignation_V11;
class UITabProfileMissionCard_V11;
class UITabProfileRecord;
class UITabProfileWeaponRecord;
class UITabProfileBase_V11;

enum PROFILETAB
{
	PROFILETAB_NONE = -1,
	PROFILETAB_START = 0,
	PROFILETAB_MISSIONCARD = PROFILETAB_START,
	PROFILETAB_DESIGNATION,

	PROFILETAB_MAX
};

class UIPhaseProfile_V11 : public UIPhaseSubBase
{
	I3_CLASS_DEFINE( UIPhaseProfile_V11, UIPhaseSubBase);

private:
	UITabProfileDesignation_V11 *	m_pDesignation;
	UITabProfileMissionCard_V11 *	m_pMissionCard;
	UITabProfileRecord *		m_pRecord;
	UITabProfileWeaponRecord *	m_pWeaponRecord;
	i3UIScene *					m_pCurrentScene;

	i3UIButton*					m_pBtnGeneral;

	PROFILETAB			m_CurrentTab;
	UITabProfileBase_V11 *	m_pProfileTabs[ PROFILETAB_MAX];

public:
	UIPhaseProfile_V11();
	virtual ~UIPhaseProfile_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	SetInputDisable(bool bDisable) override;

	virtual void	OnLoadAllScenes() override;
	virtual bool	OnExitStart() override;
	virtual void	OnExitEnd( void) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	Leave( void) override;

	virtual bool	OnClick( i3UIControl * pControl) override;

	void			InitTab();
	void			ChangeTab( PROFILETAB tab);

	virtual void OnEntranceEnd() override;

	UITabProfileDesignation_V11	*	GetDesignation() { return m_pDesignation; }
	UITabProfileMissionCard_V11 *	GetMissionCard() { return m_pMissionCard; }

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
