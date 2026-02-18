#if !defined( __UI__SMenu_SlotMain_H__)
#define __UI__SMenu_SlotMain_H__

#include "UISMenu_SlotBase.h"

class iSMenu;

class SMenuSlot5 : public SMenuMainSlot
{
public:
	SMenuSlot5(iSMenu*);
	virtual ~SMenuSlot5();

public:
	virtual void create_impl( i3GameNode * pParent) override;
	virtual void Entrance() override;
};

class SMenuSlot7Parts : public SMenuMainSlot
{
public:
	SMenuSlot7Parts(iSMenu*);
	virtual ~SMenuSlot7Parts();

public:
	virtual void create_impl( i3GameNode * pParent) override;
};

class SMenuSlot7Weapon : public SMenuMainSlot
{
public:
	SMenuSlot7Weapon(iSMenu*);
	virtual ~SMenuSlot7Weapon();

public:
	virtual void create_impl( i3GameNode * pParent) override;
};

#endif
