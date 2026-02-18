#if !defined( __UI_PROFILE_INFO_SLOT_H__)
#define __UI_PROFILE_INFO_SLOT_H__

#include "UISlotBase.h"

class UIProfileInfoSlot : public UISlotBase
{
	I3_CLASS_DEFINE( UIProfileInfoSlot);

public:
	UIProfileInfoSlot();
	virtual ~UIProfileInfoSlot();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
};

#endif
