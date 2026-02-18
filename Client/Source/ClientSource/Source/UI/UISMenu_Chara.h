#pragma once

#if 0

#include "UISMenu_SlotBase.h"
#include "UICharaInfoMgr.h"

class iSMenu;

class SMenuSlotChara : public iSMenuSlot
{
public:
	SMenuSlotChara(iSMenu*);
	virtual ~SMenuSlotChara();

public:
	void clicked(INT32 idx);

protected:
	virtual void update_impl( REAL32 rDeltaSeconds);

private:
	virtual void create_impl( i3GameNode * pParent);
	virtual void load_impl();

private:
	virtual void Entrance();
	virtual void Exit();

private:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn );

	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn );

	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn );

public:
	virtual void update_position();

	void	SetIgnoreInput( bool bIgnore);
};

#endif
