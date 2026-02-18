#if !defined( __UI_SKILL_SELECTION_SLOT_H__)
#define __UI_SKILL_SELECTION_SLOT_H__

#if defined(SKILLSYSTEM)

#include "UISlotBase.h"

class UISkillSelectionSlot : public UISlotBase
{
	I3_CLASS_DEFINE( UISkillSelectionSlot, UISlotBase);

	enum SkillSlotColorType
	{
		SLOT_TYPE_RED	= 1,
		SLOT_TYPE_GREEN,
		SLOT_TYPE_BLUE,
		SLOT_TYPE_YELLOW,
		SLOT_TYPE_PURPLE,
		SLOT_TYPE_GRAY,
	};

public:
	UISkillSelectionSlot();
	virtual ~UISkillSelectionSlot();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	virtual void	_InitializeAtLoad(i3UIScene * pScene);

	virtual void	SetEnable( BOOL bTrue = TRUE, REAL32 tm = 0.f );
public:

	void			OnClickProcess();

	BOOL			CheckEventInSkillSlot(i3UIControl*);

	void			SetSkillImage(INT32);

	void			SetSkillText(INT32);

	void			SetSelectionSlot(INT32);

	void			SetOutLine(INT32);

	void			SetBG(BOOL);

	void			SetSlotEnable(BOOL);

	void			SetCheckBoxEnable(INT32);

	void			SetArrowDirection(INT32,INT32);

	INT32			GetDirection();

private:
	BOOL					m_bCanSelect;
	i3UIButtonImageSet*		m_pImageSet;
	i3UICheckBox*			m_pCheckBox[2];
	i3UICheckBox*			m_ActiveCtrl;
	i3UICheckBox*			m_DeActiveCtrl;
};
#endif
#endif
