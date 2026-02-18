#if !defined( __UI_TAB_PROFILE_ABILITY_H__)
#define __UI_TAB_PROFILE_ABILITY_H__

#include "UITabProfileBase.h"
#include "UISkillSlot.h"

#define MAX_ABILITY_SLOT 11

class UITabProfileAbility : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileAbility);

public:
	UITabProfileAbility();
	virtual ~UITabProfileAbility();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnUpdate(REAL32 rDeltaSeconds);

	void			VerifyMoveScene(INT32);
	void			MoveScene(INT32);
	void			SelectTab(INT32);
	void			EventSkillChange();
	INT32			GetCurrentSelectedTab() {return m_CurrentTab;}
	void			ChangeSkillImage();

	void			LoadText();
	void			SetPlayMovie();

	void			OnLoadSlot(INT32 idx, i3UIScene* scn);
	void			OnEnableSlot(INT32 idx, i3UIScene* scn);

	void			SetMyAbailableSkill();
	void			SetMyFixedSkill();

private:
	INT32					m_CurrentTab;
	UISkillSlot				m_SlotSet;

	/*
	typedef UISkillSlot_Callback<UITabProfileAbility, &UITabProfileAbility::OnLoadSlot,
		&UITabProfileAbility::OnEnableSlot> SlotCallback;
	friend class SlotCallback;
	SlotCallback			m_SlotCallback;

	class UIImgSetCtrl**	m_ppImgSetCtrl;
	*/
};
#endif
