#pragma once

#include "iSlot.h"

class UIPhaseShop;

class PreviewSlot : public UI::iSlot
{
	UIPhaseShop* mParent;
	class ItemImageMgr* mImageMgr;

public:
	PreviewSlot(UIPhaseShop* p);
	virtual ~PreviewSlot();

public:
	void double_clicked();

public:
	void OnCreate( i3GameNode * pParent);
	void OnLoadAllScenes();
	void OnUnloadAllScenes();
	void OnRevive( i3RenderContext * pCtx);
	void OnUpdate( REAL32 rDeltaSeconds);

public:
	void SetEnable(bool enable);
	void MoveBackScene();

private:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
};
