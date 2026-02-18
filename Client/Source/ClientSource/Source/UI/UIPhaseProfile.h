#if !defined( __UI_PHASE_PROFILE_H__)
#define __UI_PHASE_PROFILE_H__

//----------------------------------------------------------------------------------//
// 하동익 1.5작업 Profile 변경
//----------------------------------------------------------------------------------//

#include "UIPhaseSubBase.h"
#include "UIPopupBase.h"

class UITabBase;
class UITabProfileBase;

class UITabProfileInfo;
class UITAbProfileSkillBase;
class UITabProfileRecord;
class UITabProfileWeaponRecord;
class UILoadingIcon;

namespace GameUnitTest{ namespace Popup { class Profile;}};
class UIPhaseProfile : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPhaseProfile, UIPopupBase );

	friend class GameUnitTest::Popup::Profile;

	enum PROFILETAB
	{
		TAB_NONE		= -1,

		TAB_INFO		= 0,	// 개인정보
		TAB_SKILL,		
		TAB_MEDAL,

		TAB_MAX,
	};

private:
	UITabProfileBase*		m_pTab[ TAB_MAX ];	
	
	PROFILETAB				m_eTab;
	PROFILETAB				m_eNextTab;

	bool					m_EscapeInput;

	i3UITab *				m_pUITab[ TAB_MAX];

public:
	UITabProfileBase*		GetCurrentTab()
	{		
		return GetTab( m_eTab );
	}
	void					SetCurrentTab( PROFILETAB tab);

	UITabProfileBase*		GetTab( PROFILETAB eTab )
	{
		if( eTab < 0 || TAB_MAX <= eTab )
		{
			return nullptr;
		}

		return m_pTab[ eTab ];
	}

	PROFILETAB GetCurrentTabType() const { return m_eTab;}

public:
	bool	ChangeTab( PROFILETAB eTab );
	void	NextTabProcess();

	void	setEscape(bool bDisable){	m_EscapeInput = bDisable;	};
	void	GotoBackTab();

	void	SetBackFrameEnable(bool bEnable);

private:
	void	_CreateTab();
	void	_DestroyTab();

protected:
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload( i3UIScene * pScene) override;

public:
	virtual void		OnCreate( i3GameNode * pParent ) override;

	virtual bool		OnEntranceStart( void * pArg1, void * pArg2 ) override;
	virtual void		OnEntranceEnd() override;

	virtual bool		OnExitStart() override;
	virtual void		OnExitEnd() override;

	virtual void		OnEscapeKey() override;
	virtual bool		OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;

	virtual void		ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData ) override;

	virtual void		SetInputDisable( bool bDisable ) override;
	virtual void		Leave( void) override;

public:
	static void			cbNextTabProcessEvent( void* ptr );

public:
	UIPhaseProfile();
	virtual ~UIPhaseProfile();
};

#endif
