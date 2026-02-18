#if !defined( __UI_TAB_PROFILE_ABILITY_H__)
#define __UI_TAB_PROFILE_ABILITY_H__

#if defined(SKILLSYSTEM)

#include "UIPhaseProfile.h"
#include "UITabProfileBase.h"
#include "UISkillSelectionSlot.h"
#include "UISkillMenu.h"

#define MAX_SKILL_SLOT 6


typedef struct LEARN_SKILL_PROPERTY
{
	INT32	_currentAvater;
	INT32	_currentTab;
	INT32	_class;
	UINT64	_skilltree;
	INT32	_line;
	INT32	_direction;
	
} SKILL_PROPERTY;


class UITabProfileAbility : public UISkillMenu
{
	I3_CLASS_DEFINE( UITabProfileAbility, UISkillMenu);

public:
	UITabProfileAbility();
	virtual ~UITabProfileAbility();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual void	OnMenuStart();
	virtual void	OnMenuExit();
	virtual void	SetInputDisable(bool bDisable);
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData);
protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene);
public:
	void			CreateSelectSlot();
	void			InitScroll();
	void			LoadUserInfo();
	void			LoadSkillSlotDetailInfo(UISkillSelectionSlot *slot, INT32);
	void			LoadSkillSlotInfo(UISkillSelectionSlot *slot = NULL);
	void			LoadPlayPointInfo();
	void			UpdateSlotList();
	void			Learn();
	void			Scroll();

public:				//SkillManager 연동 데이터 받아옴
	INT32			GetSlotState(INT32);
	BOOL			getCheckEnable(INT32);

	i3UIScrollBar *	getScrollBar()	{ return m_pScrollBar;	};

private:
	i3UIFrameWnd *			m_pImageDummy;
	i3UIScrollBar *			m_pScrollBar;

	INT32					m_StartIdx;
	INT32					m_CurrentSlot;
	UISkillSelectionSlot *	m_SlotSet[MAX_SKILL_SLOT];

};

#endif
#endif
