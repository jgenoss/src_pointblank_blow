#ifndef _UIPOPUP_QUICK_JOIN_SLOT_H_
#define _UIPOPUP_QUICK_JOIN_SLOT_H_

#include "../iSlot.h"
#include "../UIDefine.h"

class UIPopupQuickJoin;

namespace Qjoin
{
	class Coll_StartBtn;
	class Coll_ModeCombobox;
	class Coll_StageCombobox;
	class Coll_Imgbox;
	class CollMgr;

	class Slot : public UI::iSlot
	{
		friend class UIPopupQuickJoin;

		enum SLOT_SCENE_TYPE
		{
			SCT_NONE = -1,
			SCT_1ST,
			SCT_2ND,
			SCT_3RD,
			SCT_MAX,
		};

	public:
		explicit Slot(UIPopupQuickJoin* p);
		virtual ~Slot();

		void OnCreate(i3GameNode* pParent);
		void OnLoadAllScenes();
		void OnUnloadAllScenes();
		void OnRevive(i3RenderContext* pCtx);
		void OnUpdate(REAL32 rDeltaSeconds);

		void OnKeyInput(i3InputKeyboard* keyboard);
		void SetVisible(bool visible) { mSlot.SetVisible(visible); }
		void SetEnableMouseOver(bool enable) { mSlot.SetEnableMouseOver(enable); }
		void StartClicked(INT32 idxSlotScene);
		void ModeSelClicked(INT32 idxSlotScene);
		void StageSelClicked(INT32 idxSlotScene);
		void RefreshAllStartButton();

	protected:
		virtual void cb_Enabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
		virtual void cb_Disabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
		virtual void cb_Enter_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
		virtual void cb_Leave_impl(INT32 slot_idx, i3UIScene* slot_scn) override;

		UIPopupQuickJoin* m_parent;

		struct SlotSceneUnits
		{
			CollMgr*			mgr;

			Coll_StartBtn*		startBtn;
			Coll_ModeCombobox*	mode;
			Coll_StageCombobox*	stage;
			Coll_Imgbox*		image;
		};
		SlotSceneUnits m_unit[SCT_MAX];
	};
}

#endif