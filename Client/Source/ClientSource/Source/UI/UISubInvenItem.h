#if !defined( __UI_PHASE_Sub_Equip_INVENTORY_H__)
#define __UI_PHASE_Sub_Equip_INVENTORY_H__

#include "UISubInven.h"

class SubInvenItem : public SubInven
{
public:
	SubInvenItem(UIPhaseInventory*, UISlide::slider_type);
	virtual ~SubInvenItem();

public:
	virtual void OnCreate( i3GameNode * pParent);
	virtual void OnLoadAllScenes();
	virtual void OnUnloadAllScenes();
	virtual void OnUpdate( REAL32 rDeltaSeconds);

	virtual bool OnEntranceStart( void * pArg1 = NULL, void * pArv2 = NULL);
	virtual void OnEntranceEnd() {}

	virtual bool OnExitStart(void) {return true;};
	virtual void OnExitEnd(void) {}

protected:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn );
};

#endif
