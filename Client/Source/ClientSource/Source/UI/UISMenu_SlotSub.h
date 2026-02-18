#if !defined( __UI__SMenu_SlotSub_H__)
#define __UI__SMenu_SlotSub_H__

#include "UISMenu_SlotBase.h"

class SMenuSlotSubParts : public SMenuSubSlot
{
public:
	SMenuSlotSubParts(iSMenu*);
	virtual ~SMenuSlotSubParts();

public:
	virtual void create_impl( i3GameNode * pParent) override;
};

class SMenuSlotSubWeapon : public SMenuSubSlot
{
public:
	SMenuSlotSubWeapon(iSMenu*);
	virtual ~SMenuSlotSubWeapon();

public:
	virtual void create_impl( i3GameNode * pParent) override;
};


#endif
