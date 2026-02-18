#if !defined( __UI_PHASE_SHOP_H__)
#define __UI_PHASE_SHOP_H__

#include "UIMultiTab.h"
#include "ShopContext.h"


#include "i3Base/itl/signal/signal_common.h"

namespace buy { typedef i3::shared_ptr<class DisplayedGoods> DisplayedGoodsPtr; }

class UIPhaseShop : public iMultiTap
{
	I3_CLASS_DEFINE( UIPhaseShop, iMultiTap );

	friend class ShopSlot;

	enum{ SLIDE_OFFSET = 0, X = 634, WIDTH = 190,	};
	
	MultiSceneInfo m_board_scn_info;
	class PreviewSlot* m_pPreview_slot;

private:
	INT32 m_StartTab;

	i3::scoped_connection_set	m_SigConnection;				// 콜백함수 자동소거에만 씁니다..

public:
	void clicked_cart_board();
	void clicked_shop_Coupon();
	
	virtual void open_menu(INT32 idx) const override;
	void enable_ability_main_info(bool enable);

private:
	void cb_finish_buy_popup(bool, bool);

	void cb_finish_buy_item( bool, const i3::vector<buy::DisplayedGoodsPtr>& );
	void cb_finish_buy_auth_item(bool, const SHOP_GOODS*);
	void cb_finish_preview_auth_item(bool, const SHOP_GOODS*);

	void cb_update_cart_board();
	void cb_update_preview_slot();

	void cb_enable_compare_display_attr(bool bEnable);
	void cb_set_compare_skip_button(bool bSkip);

protected:
	virtual void clicked_menu_impl(INT32 idx) override;
	virtual void clicked_tap_impl(INT32 idx1, INT32 idx2) override;
	virtual void clicked_BuyType(INT32 idx) override;
	virtual void clicked_BuySlot(INT32 idx) override;

public:
	UIPhaseShop();
	virtual ~UIPhaseShop();

public:
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;
	virtual void OnEntranceEnd() override;

	virtual bool OnRevive( i3RenderContext * pCtx) override;
	virtual bool OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual void Enter(void) override;
	virtual void Leave(void) override;

	virtual void ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

public:
	virtual void init_slide_impl() override;
	virtual void update_impl( REAL32 rDeltaSeconds) override;
	virtual void entrance_end_impl() override;

public:
	void get_left_top_position(INT32& left, INT32 & top);
	void SetListFrontItemByItemID(UINT32 ui32ItemID);

	// 상점으로 이동시 무기 탭 외 다른 탭을 띄워줘야하는 경우가 있음
	void SetEnteranceTab(INT32 idx);

};

#endif
