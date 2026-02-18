#if !defined( __UI_OPTION_Audio_H__)
#define __UI_OPTION_Audio_H__

#include "UIOptionBase.h"
#include "UIPopupOption.h"

#include "ProgressEdit.h"
#include "EnvInfoContext.h"

class UIOptionAudio : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionAudio, UIOptionBase );
	
	enum{ MAX_VOL = 2, MAX_KEY = 3 };
	enum { MOUSE_NONE = -1 };	//마우스 키 값 상태
	struct Volume
	{
		bool enable;
		IntProgressEdit edit;
		int vol;
	};

private:
	KeyItemList mKeyItemList;
	Volume mVolume[MAX_VOL];
	edit_slot m_EditSlot[MAX_KEY];

	bool m_bEnableRadioMessage;
	int m_nRadioMessageType;

private:
	void init_keyitem_list();
	void init_control_slot(i3UIScene * pScene, edit_slot & slot, const char* wnd, const char* st, const char* btn);
	void update_keyitem_list();
	bool duplicate_key(UINT32 key, INT8 device_type);

	void start_adjust_key(int idx);
	void finish_adjust_key();
	void show_item_slot(UINT idx, const KeyItem& i);
	void display_adjust_str();
	void hide_item_slot(int idx);

private:
	void cb_edit_change1( INT32 value );
	void cb_edit_change2( INT32 value );
	IntProgress_Callback< UIOptionAudio, &UIOptionAudio::cb_edit_change1 > m_Callback1;
	IntProgress_Callback< UIOptionAudio, &UIOptionAudio::cb_edit_change2 > m_Callback2;

	static void cb_change_keyitem(void* pThis,INT32 nParam);

public:
	void clicked(int idx1, int idx2);

private:
	virtual void load_value() override;
	
	virtual void default_value() override;
	virtual void apply_game() override;
	virtual void apply_lua() override;
	virtual bool is_modified() override;
	virtual void show_scn() override;

	bool is_modified_basic();
	bool is_modified_key();
	

public:
	UIOptionAudio();
	virtual ~UIOptionAudio();

	virtual void save_value() override;
	virtual void _InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual	void OnLoadAllScenes() override;
	virtual bool OnKey_Enter() override;
	virtual bool OnKey_Escape() override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	bool is_empty_keyitem();
};

#endif
