#if !defined( __UI_SKILL_CHARACTER_SLOT_H__)
#define __UI_SKILL_CHARACTER_SLOT_H__

#include "UISlotBase.h"

#if defined( SKILLSYSTEM)

class UISkillCharacterSlot : public UISlotBase
{
	I3_CLASS_DEFINE( UISkillCharacterSlot, UISlotBase);

public:
	UISkillCharacterSlot();
	virtual ~UISkillCharacterSlot();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnSelect();
	virtual void	OnUnselect();

	void			SetSlotInfo(INT32);
	void			SetSlotState(INT32);
	bool			GetSlotState()	{ return m_bSelected;	};

	void			UpdateWheel(INT32);
private:
	bool			m_bSelected;
};

#endif

#endif
