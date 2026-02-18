#pragma once

#include "UIHUD_Domination_Base.h"
#include "UIHudDomi_Combo.h"

#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(flow_state) \
	enum_value(round) \
	enum_value(intermission) \
	enum_value(wave) \
	enum_value(annihilation) \
	enum_value(boss_hp) \
	enum_value(revival) \
	enum_value(etc) \
	enum_value(counter) \
	enum_value(award) \
	enum_value(grade) \
	enum_value(boss_howling) \
	enum_value(boss_dash) \
	enum_value(boss_howl_hp) \
	enum_value(boss_howl_canceled) \
	enum_value(flow_control_max)

#undef DO_ENUM
#define DO_ENUM(e) e,

#undef DO_DESC
#define DO_DESC(e) _T(#e),

namespace DominationUI
{
	enum FLOW_CONTROL
	{ ENUM_LIST(DO_ENUM)};

	static const char* g_flow_desc[] = 
	{ ENUM_LIST(DO_DESC)};

	//------------------------------------------
	//input
	//SetEvent( FLOW, Input, arg)


	//
	enum HUD_FLOW_EVENT
	{
		FLOW_E_CHK_VISIBLE,
		FLOW_E_VISIBLE,
		FLOW_E_INVISIBLE,
		
		FLOW_E_REFRESH,
		FLOW_E_DETAIL,

		FLOW_E_INPUT,

		FLOW_E_MAX
	};
	enum FLOW_DETAIL 
	{ 
		FLOW_D_ROUND, 
		FLOW_D_INTERMISSION, 
		FLOW_D_WAVE, 
		FLOW_D_COUNTER, 
		FLOW_D_BOSS_HP, 
		FLOW_D_AWARD, 
		FLOW_D_ETC, 
		FLOW_D_GRADE, 
		FLOW_D_BOSS_HOWLING,
		FLOW_D_BOSS_DASH,
		FLOW_D_BOSS_HOWL_HP,
		FLOW_D_BOSS_HOWL_CANCELED,
		FLOW_D_MAX
	};

	class time_event
	{
		INT32 m_id;
	protected:
		i3UIControl* m_p;
	public:
		REAL32 m_current, m_target;

		time_event( i3UIControl* p, REAL32 target, INT32 id = -1) : m_p(p), m_target(target), m_id(id) { m_current = 0.f;}
		virtual ~time_event() {}
		virtual void operator()() = 0;
		bool OnUpdate( REAL32 fElpasedTime);
		INT32 get_id() const { return m_id;}
	};

	class time_disable : public time_event
	{
	public:
		time_disable( i3UIControl* p, REAL32 target, INT32 id = -1) : time_event(p, target, id) {}
		void operator()() { if( m_current >= m_target) m_p->EnableCtrl( false);}
	};
	
	class time_event_detail : public time_event
	{
		call_back_ptr cb_ptr;
	public:
		time_event_detail( i3UIControl* p, REAL32 target, call_back_ptr cb, INT32 id = -1) : time_event(p, target, id), cb_ptr(cb) {}
		void operator()() { if( m_current >= m_target) (*cb_ptr)( this);}
	};

	typedef i3::shared_ptr<time_event> time_event_ptr;
	class time_event_list
	{
		i3::vector<time_event_ptr> list;
	public:
		time_event_list() { list.reserve(10);}
		~time_event_list() { list.clear();}
		void push_event( time_event_ptr ptr) { list.push_back(ptr);}
		bool pop_event( time_event_ptr& ptr);
		
		time_event_ptr find( INT32 event_id);

		void OnUpdate( REAL32 fElpasedTime);
	};
};

namespace dui = DominationUI;

class UIImgSetCtrl;
class CUIHudDomi_Flow : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_Flow, UIHUDDominationBase);

	enum 
	{ 
		IDE_GRADE_DISABLE = 5,
		IDE_DASH_DISABLE = 7,
	};

private:
	i3UIFrameWnd*		m_frame[dui::flow_control_max];

	dui::time_event_list	m_time_event_list;

	REAL32	m_refresh_boss_hp;

	UIImgSetCtrl* m_award_img_ctrl;

	bool m_bossHowlCanceled;
	REAL32 m_refresh_boss_howl_canceled;

	//refresh
	typedef void (CUIHudDomi_Flow::*refresh_func)();
	refresh_func refresh[dui::flow_control_max];

	void refresh_flow_state();
	void refresh_round();
	void refresh_intermission();
	void refresh_wave();
	void refresh_annihilation();
	void refresh_boss_hp();
	void refresh_revival();
	void refresh_etc();
	void refresh_counter();
	void refresh_award();
	void refresh_grade();
	void refresh_boss_howling();
	void refresh_boss_dash();
	void refresh_boss_howl_hp();
	void refresh_boss_howl_canceled();

	void update_play_time();

	//detail
	typedef void (CUIHudDomi_Flow::*detail_func)();
	detail_func detail[dui::FLOW_D_MAX];

	void detail_round();
	void detail_intermission();
	void detail_wave();
	void detail_counter();
	void detail_boss_hp();
	void detail_award();
	void detail_etc();
	void detail_grade();
	void detail_boss_howling();
	void detail_boss_dash();
	void detail_boss_howl_hp();
	void detail_boss_howl_canceled();
	


	//input
	void process_input( INT32 key);

public:
	CUIHudDomi_Flow(void);
	~CUIHudDomi_Flow(void);

	virtual void OnCreate( i3GameNode* pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnEntranceEnd() override;

	void OnLoadAllScenes();
	void OnUnloadAllScenes();

	virtual void OnUpdate( REAL32 rDeltaSec) override;

	virtual bool SetHudEvent( INT32 nEventNum, INT32 arg = 0) override;

	//...
	void cb_detail_wave( void* p1, void* p2) { detail_wave();}
	void cb_detail_grade( void* p1, void* p2) { detail_grade();}
};