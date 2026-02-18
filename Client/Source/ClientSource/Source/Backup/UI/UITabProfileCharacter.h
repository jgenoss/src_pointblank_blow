#if !defined( __UI_TAB_PROFILE_CHARACTER_H__)
#define __UI_TAB_PROFILE_CHARACTER_H__

#include "UIPhaseProfile.h"
#include "UITabProfileBase.h"
#include "UISkillCharacterSlot.h"
#include "UISkillMenu.h"

#define MAX_CHARA_SLOT 5

#if defined( SKILLSYSTEM)

class UITabProfileCharacter : public UISkillMenu
{
	I3_CLASS_DEFINE( UITabProfileCharacter, UISkillMenu);

public:
	UITabProfileCharacter();
	virtual ~UITabProfileCharacter();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual void	OnMenuStart();
	virtual void	OnMenuExit();
	
protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene);
	virtual void	SetInputDisable(bool bDisable);

public:
	void			InitState();
	void			InitText();

	void			CreateCharaSlot();
	void			SelectSlot(INT32,BOOL);
	INT32			FindSlot(LuaState *);

	void			SetCharaName(INT32);
	void			SetDetailInfo(INT32);
	void			DefaultSlot(INT32,BOOL);
	void			PushSetButton();

	void			UpdateSlotList();
	void			UpdateWheel( int nDelta );
	void			UpdateScroll();

private:
	i3UIFrameWnd *				m_pImageDummy;
	UISkillCharacterSlot*		m_SlotSet[MAX_CHARA_SLOT];
	i3UIScrollBar *				m_pScrollBar;

	INT32						m_StartIdx;
	INT32						m_nUserAccount;
	INT32						m_CurrentSlot;
};

#endif
#endif
