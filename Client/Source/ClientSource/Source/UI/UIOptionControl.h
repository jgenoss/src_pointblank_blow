#if !defined( __UI_OPTION_Control_H__)
#define __UI_OPTION_Control_H__

#include "UIOptionBase.h"

#include "UILBScroll.h"
#include "ProgressEdit.h"
#include "UIPopupOption.h"

class UIOptionControl : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionControl,UIOptionBase );

public:
	UIOptionControl();
	virtual ~UIOptionControl();

	virtual void _InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void OnCreate( i3GameNode * pParent) override;

	virtual	void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;

	virtual bool OnKey_Enter() override;
	virtual bool OnKey_Escape() override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual void show_scn() override;

	bool is_empty_keyitem();
	bool deactivated_key(UINT32 key, INT8 device_type);
	bool duplicate_key(UINT32 key, INT8 device_type);

private:
	void init_keyitem_list();
	void update_keyitem_list();
	
	void init_control_slot(i3UIScene * pScene, static_slot & slot, const char* wnd, const char* st);
	void init_control_slot(i3UIScene * pScene, edit_slot & slot, const char* wnd, const char* st, const char* btn);
	
	void show_title_slot(int col, int idx, const KeyItem& i);
	void hide_title_slot(int col);

	void show_item_slot(int col, int idx, const KeyItem& item);
	void hide_item_slot(int col, int idx);

	void display_adjust_str();
	
	void start_adjust_key(int idx1, int idx2);		
	void finish_adjust_key();

private:
	static void cb_change_keyitem(void* pThis,INT32 nParam);

private:
	void load_src_key_value();
	void save_key_value_to_src();

public:
	void clicked(int idx1, int idx2);

private:
	virtual void load_value() override;

	virtual void default_value() override;
	virtual void apply_game() override;
	virtual void apply_lua() override;
	virtual bool is_modified() override;

	bool is_modified_basic();
	bool is_modified_key();

public:
	virtual void save_value() override;
	enum {ROW = 9, COL = 2, MAX_TITLE = 3, 	MAX = ROW*COL,};
private:
	static_slot m_TitleSlot[MAX_TITLE];
	edit_slot m_EditSlot[COL][ROW];
	UINT32 m_idxStartSlot;

private:
	bool m_bEnableMouseInvert; // 마우스 반전
	REAL32 m_rMouseSensivity; // 마우스 민감도(0.10~1.20) 값이다. (0~110)으로 계산하자.
	KeyItemList mKeyItemList[COL];	// Ctrl 탭의 key list. uioptionbase에 존재하는 key list에서 값을 받아서 세팅됨

private:
	UILBScroll m_Scroll;

	void cb_scroll( INT32 idx, INT32 old_idx );
	TUILBScroll_Callback< UIOptionControl, &UIOptionControl::cb_scroll > m_ScrollCallback;
	friend class ScrollCallback;

private:
	RealProgressEdit mPEdit;
	void cb_edit_change( REAL32 value );
	RealProgress_Callback< UIOptionControl, &UIOptionControl::cb_edit_change > m_PECallback;
};

#endif
