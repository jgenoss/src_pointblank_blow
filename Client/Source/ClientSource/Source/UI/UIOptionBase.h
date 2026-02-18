#if !defined( __UI_OPTION_Base_H__)
#define __UI_OPTION_Base_H__

#include "UITabBase.h"
#include "UIUtil.h"

enum UI_KEYCTRL_LIST_TYPE
{
	UI_KEYCTRL_TYPE_GAMEKEY_LEFT,
	UI_KEYCTRL_TYPE_GAMEKEY_RIGHT,
	UI_KEYCTRL_TYPE_AUDIO,

	UI_KEYCTRL_TYPE_MAX
};

enum SLOT_TYPE { TITLE, ITEM, DUMMY_TITLE };

struct edit_slot
{
	i3UIFrameWnd* wnd;
	i3UIStaticText* st_ctrl;
	i3UIButton* btn_ctrl;
};

struct static_slot
{
	i3UIFrameWnd* wnd;
	i3UIStaticText* st_ctrl;
};

class KeyItem
{
public:
	GAME_KEY_STRUCT value;
	GAME_KEY_MAP menu;
	SLOT_TYPE type;
	i3::rc_wstring wstr_name;

	void set_default();
	void load_value();
	void save_value();
	bool match_value();
	bool duplicate_keyItem(UINT32 key, INT8 device_type);
};

class UIOptionBase : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIOptionBase, UITabBase );

	enum SLOT_TYPE;

	public:
	struct SlotIndex{
		int col;	int row;
		int ctrl_row;
	} base_dupl_slot_idx, base_slot_idx; 

public:	
	typedef i3::vector< KeyItem > KeyItemList;
	enum 
	{
		INIT = 0x10000000,
		BASIC = 0x00000001,
		KEY = 0x00000002,
		MACRO = 0x00000004
	};

public:

	virtual void load_value() = 0;
	virtual void save_value() = 0;
	virtual void default_value() = 0;//option 값을 기본 값으로 변경한다.
	virtual void apply_game() = 0;//option 값을 게임에 적용한다.
	virtual void apply_lua() = 0; //option 값을 lua에 적용한다.
	virtual bool is_modified() = 0; //option 값 변경 유무를 알려준다. 변경시 true
	
	//editbox에 focus가 없는 경우에 들어온다.
	virtual bool key_escape() {return false; } //escape 키 입력시 처리 한다.(리턴값이 false 이면 uibase 루틴으로 간다.)
	virtual bool key_enter() {return false;} //true 키 입력시 처리 한다.(리턴값이 false 이면 uibase 루틴으로 간다.)

public:
	UIOptionBase();
	virtual ~UIOptionBase();

	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool OnExitStart(void) override;

	virtual void entrance_scn();
	virtual void show_scn();
	virtual void hide_scn();
	virtual void exit_scn();

	virtual void clicked(int idx1, int idx2) {}

	static bool g_bKeyItemInit;		// 키 리스트가 구성 되었는지 여부

	void init_key_list();

	GAME_KEY_STRUCT base_none_key;

};

#endif
