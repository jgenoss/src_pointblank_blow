#if !defined( __UI__SMenu_Slot_Base_H__)
#define __UI__SMenu_Slot_Base_H__

#include "iSlot.h"
#include "UIInvenInfoMgr.h"
#include "UIUtil.h"

class UISideMenu;
class UIImgSetCtrl;
class iSMenu;
class CharaInfoMgr;

class iSMenuSlot : public UI::iSlot
{
protected:
	item_def::SideCategory mCategory;
	iSMenu* mParent;
	CharaInfoMgr* m_pCharaInfoMgr;
	bool m_inven_phase;
	GameUI::AutoPosition m_Pos;

public:
	enum SLOT_MSG {
		SLOT_MSG_OPEN_SLOT_SUB,
		SLOT_MSG_CLOSE_SLOT_SUB,
		SLOT_MSG_ITEM_CHANGE,
		SLOT_MSG_UPDATE_ITEM,
	};

public:
	iSMenuSlot(iSMenu*);
	virtual ~iSMenuSlot();

	i3UIScene * GetSlotScene(INT32 idx) { return mSlot.GetSlotScene(idx);}
	void SetScrollFocus()	{	mSlot.SetScrollFocus();	}
	item_def::SideCategory GetCategory() const {	return mCategory; }

protected:
	UISideMenu* get_base();
	void send_msg( SLOT_MSG msg, void * pArg = nullptr);
	void adjust_position();

public:
	void OnCreate( i3GameNode * pParent);
	void OnLoadAllScenes();
	void OnUnloadAllScenes();
	void OnUpdate( REAL32 rDeltaSeconds);
	void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);

protected:
	virtual void create_impl( i3GameNode * pParent) = 0;
	virtual void load_impl() = 0;
	virtual void unload_impl() {};
	virtual void update_impl( REAL32 rDeltaSeconds) {}
	virtual void event_impl( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) {}

public:
	virtual void Entrance();
	virtual void Exit();
	virtual void Notify( SLOT_MSG msg, void * pArg = nullptr) {}
	virtual void MoveBackScene() { mSlot.MoveBackScene(); }
	bool is_selected_slot(INT32 idx) const { return iSlot::is_selected(idx); }

protected:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn );
};


class SMenuMainSlot : public iSMenuSlot
{
public:
	SMenuMainSlot(iSMenu*);
	virtual ~SMenuMainSlot();

public:
	void clicked();

protected:
	void msg_update_item();
	virtual void Notify( SLOT_MSG msg, void * pArg = nullptr) override;

	virtual void Entrance() override;	//CharaInfoMgr를 매번 갱신한다.
	virtual void load_impl() override;
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
};


class SMenuSubSlot : public iSMenuSlot
{
protected:
	inven_util::SubInfoList mSubInfoList;
	bool m_hit_entered;

public:
	SMenuSubSlot(iSMenu*);
	virtual ~SMenuSubSlot();

public:
	void clicked();
	void double_clicked();

protected:
	void adjust_position();
	virtual void Notify( SLOT_MSG msg, void * pArg = nullptr) override;

	virtual void Entrance() override; //SubInfoList를 매번 갱신한다.
	virtual void Exit() override;
	virtual void load_impl() override;
	virtual void unload_impl() override;
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void update_impl( REAL32 rDeltaSeconds) override;

private:
	void hit_test_enter( UINT32 inx, i3UIControl* pWnd );
	void hit_test_leave( UINT32 inx, i3UIControl* pWnd );

	UI::tmiHover_Callback< SMenuSubSlot, &SMenuSubSlot::hit_test_enter, &SMenuSubSlot::hit_test_leave	> m_HitTestCallback;
	UI::tmHover m_tmHitTest;
};

#endif
