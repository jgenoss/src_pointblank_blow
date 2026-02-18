/**
 * \file	UI\UISlotStage.h
 *
 * Declares the user interface slot stage class.
 */


#if !defined( __UI_SLOT_STAGE_H__)
#define __UI_SLOT_STAGE_H__

#include "UISlotBase.h"

class UISlotStage : public UISlotBase
{
	I3_CLASS_DEFINE( UISlotStage);

public:
	UISlotStage();
	virtual ~UISlotStage();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	SetEmptySlot(void);

	/** \brief 선택에 대한 처리 */
	virtual void	OnSelect( void);

	/** \brief 선택 해제에 대한 처리 */
	virtual void	OnUnselect( void);

	/** \brief UI Event Mouse Wheel */
	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta);

	void			SetInfo( const char * pszName, INT32 imageIdx, bool bNew, bool bEvent);

	/** \brief 선택 여부 반환 */
	bool			IsSelect();

	/** \brief 활성화 여부 반환 */
	bool			IsEmpty();
};

#endif
