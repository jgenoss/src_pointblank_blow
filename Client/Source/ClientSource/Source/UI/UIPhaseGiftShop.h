#if !defined( __UI_PHASE_GIFT_SHOP_H__)
#define __UI_PHASE_GIFT_SHOP_H__

#include "UIMultiTab.h"

class UIPhaseGiftShop : public iMultiTap
{
	I3_CLASS_DEFINE( UIPhaseGiftShop, iMultiTap );

	friend class ShopSlot;

	enum{ SLIDE_OFFSET = 0, X = 634, WIDTH = 190, };
	enum{ MAX_RANKING_COUNT = 6, };

	MultiSceneInfo m_gift_scn_info, m_deco_scn_info;
	UI::tmSlide	 m_tmSlide2;

	UIImgSetCtrl * m_pImgSet[MAX_RANKING_COUNT];

	i3::scoped_connection_set	m_SigConnection;

public:
	void clicked_gift_rank_slot(INT32 idx);

private:
	void set_gift_shop_display_info(void);

	void set_weekly_best_item(INT32 idx);
	void set_weekly_best_item_image(INT32 idx, T_ItemID itemID);
	void set_weekly_best_item_string(INT32 idx, INT32 rank, T_ItemID itemID);

	void get_item_ranking_amount_text(i3::stack_wstring & out_wstr, const INT32 i32Type, const INT32 i32Amount);
	void get_item_ranking_amount_text_color(COLOR & out_color, const INT32 i32Type);

private:
	void cb_finish_buy_popup(bool, bool);
	void cb_finish_buy_gift(bool);									// 상점과 인수가 달라서 함수 네이밍을 아예 바꿉니다..
	void cb_is_ignore_shared(bool& out_is_ignore_shared);			// 아웃풋값..
	void cb_enable_compare_display_attr(bool bEnable);

protected:
	virtual void clicked_menu_impl(INT32 idx) override;
	virtual void clicked_tap_impl(INT32 idx1, INT32 idx2) override;

public:
	UIPhaseGiftShop();
	virtual ~UIPhaseGiftShop();

public:
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;
	virtual void OnEntranceEnd() override;

	virtual bool OnRevive( i3RenderContext * pCtx) override;

	virtual bool OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	virtual void Enter(void) override;
	virtual void Leave(void) override;

	virtual void ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual void open_menu(INT32 idx) const override;
	void enable_ability_main_info(bool enable);

protected:
	virtual void init_slide_impl() override;
	virtual void entrance_end_impl() override;
	virtual void update_impl( REAL32 rDeltaSeconds) override;
};

#endif // __UI_PHASE_GIFT_SHOP_H__
