#if !defined( __UI_POPUP_ACQUIRE_SKILL_H__)
#define __UI_POPUP_ACQUIRE_SKILL_H__

#if defined(SKILLSYSTEM)

#include "UIPopupSubBase.h"
#include "UITabProfileAbility.h"
#include "UISkillSelectionSlot.h"



class UIPopupLearnSkill : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupLearnSkill, UIPopupBase);

public:
	UIPopupLearnSkill();
	virtual ~UIPopupLearnSkill();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual bool	OnExitStart();

	virtual void	_InitializeAtLoad(i3UIScene * pScene);

	virtual bool	OnOKButton( void);

	virtual bool	OnCloseButton( void);
public:
	void			InitState();
	void			setNickText();
	INT32			GetCurrentAvatar()	{	return m_SkillProperty._currentAvater;	}
	INT32			GetCurrentTab()		{	return m_SkillProperty._currentTab;		}
	//void			LoadAgreement();

private:
	SKILL_PROPERTY			m_SkillProperty;
	i3UIFrameWnd *			m_pImageDummy;
	UISkillSelectionSlot*	m_SlotSet;
};

#endif
#endif
