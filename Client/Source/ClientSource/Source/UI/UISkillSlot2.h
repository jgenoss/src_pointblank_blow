#if !defined( __UI_SKILL_SLOT2_H__)
#define __UI_SKILL_SLOT2_H__

#include "UISlotBase.h"

class UISkillSlot2 : public UISlotBase
{
	I3_CLASS_DEFINE( UISkillSlot2);

public:
	UISkillSlot2();
	virtual ~UISkillSlot2();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief 선택에 대한 처리 */
	virtual void	OnSelect( void);
	/** \brief 선택 해제에 대한 처리 */
	virtual void	OnUnselect( void);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
public:

	void			OnClickProcess();

	BOOL			CheckEventInSkillSlot(i3UIControl*);

	void			SetSkillImage(INT32);

	void			SetSelectionSlot(INT32);

	void			SetOutLine(INT32);

	void			SetLockCtrl(BOOL);

private:
	i3UICheckBox*	m_pCheckBox[2];
	i3UICheckBox*	m_ActiveCtrl;
	i3UICheckBox*	m_DeActiveCtrl;

};

#endif
