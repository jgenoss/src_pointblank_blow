#if !defined( __UI_MULTI_TAB_H__)
#define __UI_MULTI_TAB_H__

#include "UIPhaseSubBase.h"
#include "UIMultiTabSlot.h"

class iMultiTap : public UIPhaseSubBase
{
private:
	i3::vector<SHOP_SORT_TYPE>	m_SortData[SHOP_SORT_CATEGORY_MAX];
	SHOP_SORT_CATEGORY			m_CurrentSortType;
	INT32						m_SortComboSlot;
	i3UIComboBox*				m_SortComboBox;

protected:
	iMultiTabSlot* m_pSlot;
	class UIImgSetCtrl* m_ppImgSet;
	class CharaInfoMgr* m_pCharaInfoMgr;

	UI::tmSlide m_tmSlide;
	INT32 m_slide_offset_x, m_slide_offset_y;
	INT32 m_phase_type;	

	i3::rc_string slot_scene_name;
	MultiSceneInfo m_menu_scn_info, m_tap_scn_info, m_slot_scn_info;

	i3UIButtonComboBox*		m_BtnCombo;
	i3UIEditBox*			m_SearchEditBox;
	i3UIFrameWnd*			m_SearchFrm;
	i3UIFrameWnd*			m_SlotFrm[2];
public:
	iMultiTap();
	virtual ~iMultiTap();

	i3UIScene * GetItemSlotScene(INT32 i32idx) const	{ return m_pSlot->GetSlotScene(i32idx); }
	i3UIScene * GetMainTabScene(void) const				{ return m_menu_scn_info.scn; }

	iMultiTabSlot * GetItemSlot(void) const				{ return m_pSlot; }
	INT32		GetPhaseType(void) const				{ return m_phase_type; }

	void				SetSortCombo(INT32 idx);
	SHOP_SORT_TYPE	GetSortType() const
	{
		return m_SortData[m_CurrentSortType].at(m_SortComboSlot);
	}


public:
	void clicked_menu(INT32 idx) ;
	void clicked_tap(INT32 idx1, INT32 idx2);
	void clicked_tap_combo(INT32 idx);
	void clicked_CancelSearch();
	virtual void clicked_Sort(INT32 idx);
	virtual void open_menu(INT32 idx) const;
	virtual void clicked_UseInven() {};
	virtual void clicked_BuyType(INT32 idx) {};
	virtual void clicked_BuySlot(INT32 idx) {};

	void hide_display_info();
	virtual void show_display_info();
	VEC3D* GetSlotPos()
	{
		return m_SlotFrm[0]->getPos();
	}

	size_t	get_current_menu_idx() const;
	void	set_current_menu_idx(size_t value);

	void	SelectedEnteranceTab(INT32 idx);

protected:
	virtual void clicked_menu_impl(INT32 idx) {}
	virtual void clicked_tap_impl(INT32 idx1, INT32 idx2) {}
	virtual void clicked_tap_combo_impl(INT32 idx) {}

private:
	void init_scene();
	void init_combo();

protected:
	void init_slide();
	virtual void init_slide_impl() {}

public:
	void add_slide(UI::tmSlideAtt* att);

public:
	virtual void OnCreate( i3GameNode * pParent) override;

	/*
	pArg2 타입에 따라 pArg1의 casting이 달라집니다.
	param[in] pArg1 const SHOP_GOODS_PACK* or T_ItemID*
	param[in] pArg2 bool*
	*/
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnEntranceEnd() override;

	virtual bool OnExitStart(void) override;
	virtual void OnUnloadAllScenes() override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;

	virtual bool OnPlayClickSound(i3UIControl * pCtrl) override;

	virtual void _InitializeAtLoad( i3UIScene * pScene) override;

	virtual bool OnRevive(i3RenderContext * pCtx) override;
	virtual bool OnKeyInput(i3InputDeviceManager * pInputMng) override;

	virtual void ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

protected:
	virtual void update_impl( REAL32 rDeltaSeconds) {}
	virtual void entrance_end_impl() {}
};

#endif
