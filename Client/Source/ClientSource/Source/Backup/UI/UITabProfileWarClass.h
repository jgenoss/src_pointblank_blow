#if !defined( __UI_TAB_PROFILE_WARCLASS_H__)
#define __UI_TAB_PROFILE_WARCLASS_H__

#include "UIPhaseProfile.h"
#include "UITabProfileBase.h"
#include "UISkillSelectionSlot.h"
#include "UISkillMenu.h"

class UIMovieSetCtrl;

#if defined( SKILLSYSTEM)

class UITabProfileWarClass : public UISkillMenu
{
	I3_CLASS_DEFINE( UITabProfileWarClass, UISkillMenu);

public:
	UITabProfileWarClass();
	virtual ~UITabProfileWarClass();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual void	OnMenuStart();
	virtual void	OnMenuExit();
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData);

public:
	void			ChangeClass(INT32);
	void			SetMoviePlay(INT32);
	void			SetButtonFrame();
	void			SetMovieText(INT32 idx = -1);
	void			SetExplainText(INT32 idx = -1);
	void			SetTypeText();
	void			SetMovieBG(bool);
	void			Confirm();

	INT32			getSelectedClass(INT32);

private:
	UIMovieSetCtrl*				m_pMovieCtrl;
	INT32						m_Selected;
};

#endif

#endif
