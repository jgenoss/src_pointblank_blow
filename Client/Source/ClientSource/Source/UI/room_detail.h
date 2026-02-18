#pragma once

#define		COUNTDOWN_COUNT			5


class UIPhaseReadyRoom;
class room_base
{
protected:
	UIPhaseReadyRoom* phase_ready_room;
	i3UIFrameWnd* frame_wnd;
public:
	room_base( UIPhaseReadyRoom* p) : phase_ready_room(p), frame_wnd(nullptr) {}
	virtual ~room_base() {}

	void set_frame_wnd( i3UIFrameWnd* frame_wnd) { this->frame_wnd = frame_wnd;}
	void hide_frame() { if(frame_wnd) frame_wnd->EnableCtrl( false);/*SetEnable( false);*/}
	void show_frame() { if(frame_wnd) frame_wnd->EnableCtrl( true);/*SetEnable( true);*/}

	virtual void update( REAL32 elapsed_time) {}
	virtual void entrance_end() {}
	virtual void exit_start() {}
	virtual void revive() {}

	virtual void event_set_focus_popup() {}
	virtual void event_kill_focus_popup() {}

	void do_config_room();

	//-------------------------------------------------------------------------------
	//공통 기능 이지만 슬롯 제한에 때라 다르게 동작 하는 함수
	virtual void do_ready_game() {}
	virtual bool is_ready() { return false;}
	virtual void set_button_state() {}

	virtual bool is_room_user( const char* pNick) { return false;}
	virtual bool check_room_master_kick( INT32& other_user_count) { return false;}

	//슬롯 업데이트
	virtual void process_update_slot() {}
	virtual	void close_slot( INT32 slot, bool bClose, bool bLock = false) {}
	virtual void empty_slot( INT32 slot) {}
	virtual void set_slot_info( INT32 slot) {}
	virtual void effect_slot( INT32 slot) {}

	virtual bool is_alone() { return false;}
	

	//-------------------------------------------------------------------------------
	//제압 미션 레디룸에서는 필요 업는 함수
	//팀 바꾸기 기능
	virtual void change_team( INT32 team) {}
	virtual void do_swap_team() {}

	//랜덤 맵 기능
	virtual void add_all_ramdom_map() {}
	
	//-------------------------------------------------------------------------------
	//제압 에서만 쓰는 것들..
	virtual void grade_domi_room( bool in_battle) { }
	virtual void release_grade() { }
};




class room_normal : public room_base
{
	REAL32				m_rTeamChangeDelay;
public:
	room_normal( UIPhaseReadyRoom* p) : room_base(p) {}
	~room_normal() {}

	void update( REAL32 elapsed_time);

	void do_ready_game();
	virtual void ready_process();

	bool is_ready();
	void set_button_state();

	bool is_room_user( const i3::wliteral_range& wNickRng);
	//bool check_room_master_kick( INT32& other_user_count);

	void process_update_slot();
	void close_slot( INT32 slot, bool bClose, bool bLock = false);
	void empty_slot( INT32 slot);
	
	void effect_slot( INT32 slot);

	bool is_alone();

	void change_team( INT32 team);
	void do_swap_team();
	void add_all_ramdom_map();
	
	static void callback_season_match(void* pThis,INT32 nParam);

	virtual void set_slot_info( INT32 slot);
};

class room_normal_V10 : public room_normal
{
	REAL32				m_rTeamChangeDelay;
public:
	room_normal_V10( UIPhaseReadyRoom* p) : room_normal(p) {}
	~room_normal_V10() {}

	virtual void set_slot_info( INT32 slot) override;
	virtual void  ready_process() override;
};

class CWebCtrl;
void OnDocumentCompeteInReadyRoom( CWebCtrl* ctl);

class room_domi : public room_base
{
	enum { DOMI_SLOT_COUNT = 4};
	enum DOMI_TEAM { DOMI_RED, DOMI_BLUE};

	bool m_create_web;
	bool m_connected;
	HWND m_hwnd_web;
	static INT16 m_grade;
public:
	
	static INT32 convert_slot( INT32 idx, DOMI_TEAM e)
	{ 
		if(e == DOMI_RED) return (idx*2);
		return (idx*2)+1;
	}

	room_domi( UIPhaseReadyRoom* p) : room_base(p), m_create_web(false), m_connected(false), m_hwnd_web(0) {}
	~room_domi() {}

	void entrance_end();
	void exit_start();
	void revive();

	void event_set_focus_popup() { if( m_create_web) close_rank_web( false);}
	void event_kill_focus_popup() { if( m_create_web) open_rank_web();}

	void do_ready_game();
	bool is_ready();
	void set_button_state();

	bool is_room_user( const i3::wliteral_range& wNickRng);
	//bool check_room_master_kick( INT32& other_user_count);

	void process_update_slot();
	void close_slot( INT32 slot, bool bClose, bool bLock = false);
	void empty_slot( INT32 slot);
	void set_slot_info( INT32 slot);
	void effect_slot( INT32 slot);

	void grade_domi_room( bool in_battle);
	void release_grade() { m_grade = -1; }

	bool is_alone();

	void click_slot( INT32 slot);
	void click_R_slot( INT32 slot);
	void click_close( INT32 slot);


	void open_rank_web();
	void close_rank_web( bool del);
	void calc_rect( i3::pack::RECT& rc);
	void reposition_rank_web();

	friend void OnDocumentCompeteInReadyRoom( CWebCtrl*);
};

extern "C" {
	int room_domi_click_slot( LuaState* L);
	int room_domi_click_R_slot( LuaState* L);
	int room_domi_click_close( LuaState* L);
};