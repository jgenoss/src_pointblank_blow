#pragma once

#include "UIHUD_Domination_Base.h"
#include "../UIMath.h"

#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(bg) \
	enum_value(combo) \
	enum_value(sum) \
	enum_value(pts) \
	enum_value(sum_frame) \
	enum_value(add) \
	enum_value(scroll_main) \
	enum_value(scroll_sub) \
	enum_value(combo_control_max)

#undef DO_ENUM
#define DO_ENUM(e) e,

#undef DO_DESC
#define DO_DESC(e) _T(#e),

namespace DominationUI
{
	enum COMBO_CONTROL 
	{ ENUM_LIST(DO_ENUM)};

	static const char* g_combo_desc[] = 
	{ ENUM_LIST(DO_DESC)};

	class call_back
	{
	public:
		REAL32 target_time;
		bool use_call;

		call_back() : target_time(0.f), use_call(false) {}
		virtual ~call_back() {}
		virtual void operator()( void* caller) = 0;
	};
	typedef i3::shared_ptr<call_back> call_back_ptr;

	template<class T>
	class member_call_back : public call_back
	{
		T* obj;
		typedef void (T::*cb_func)( void*, void*);
		cb_func cb;

		void* data;
	public:
		member_call_back( T* o, cb_func f, void* d) : obj(o), cb(f), data(d) {}
		void operator()( void* caller) { (obj->*cb)(caller, data);}
	};
	

	class ui_actor
	{
	protected:
		REAL32 m_fPeriod;
		REAL32 m_fProgressTime;
		i3UIControl* m_pControl;

		call_back_ptr m_cb;
		bool m_enable;
	public:
		ui_actor( i3UIControl* pControl = nullptr)
			: m_pControl(pControl), m_enable(false)
		{ if( pControl) pControl->EnableCtrl( false); m_cb.reset();}
		virtual ~ui_actor() {}

		virtual void act() {}
		void OnUpdate( REAL32 fDeltaTime);

		i3UIControl* get_control() const { return m_pControl;}
		void enable_control( bool b) { m_pControl->EnableCtrl( (b == true));}

		REAL32 get_period() const { return m_fPeriod;}
		REAL32 get_progress() const { return m_fProgressTime;}

		void set_enable( bool b) { m_enable = b;}
		bool is_enalbe() const { return m_enable;}

		void set_call_back( call_back_ptr& cb) { m_cb = cb;}
		const call_back_ptr& get_call_back() const { return m_cb;}
	};

	
	enum FADE_CONTROL_TYPE { FADE_IN = 0, FADE_OUT, FADE_MAX}; 
	class ui_fader : public ui_actor
	{
	private:
		i3UIStaticText* m_pText;
		REAL32 m_fMaxApha, m_fMinApha;
		
		FADE_CONTROL_TYPE style;
	public:
		ui_fader( i3UIControl* pControl = nullptr);
		virtual ~ui_fader() {}
		void start( FADE_CONTROL_TYPE eType, REAL32 fPeriod, bool use_call_back = false, REAL32 target_time = 0.f);
		void act();

		void restore();
	};

	enum SLIDE_TYPE { SLIDE_IN = 0, SLIDE_OUT, SLIDE_MAX};
	class ui_slider : public ui_actor
	{
	protected:
		VEC3D m_origin;
		VEC3D m_start;
		VEC3D m_end;
		
		SLIDE_TYPE style;
	public:
		ui_slider( i3UIControl* pControl = nullptr);
		virtual ~ui_slider() {}	
		virtual void start( SLIDE_TYPE e, REAL32 fPeriod, bool use_call_back = false, REAL32 target_time = 0.f);
		virtual void act();
		void stop();

		VEC3D* get_origin() { return &m_origin;}
		void restore() { m_pControl->setPos( &m_origin);}
		void set_target( VEC3D* start, VEC3D* end) { i3Vector::Copy( &m_start, start); i3Vector::Copy( &m_end, end);}

		virtual void set_src_3d_pos( VEC3D* pos, REAL32 x_dist = 0.f, REAL32 y_dist = 0.f) {}	//3d로 인해 만듬.
	};

	
	enum TEXT_SCALE_TYPE { TEXT_SCALE_NORMAL = 0, TEXT_SCALE_RESTORE, TEXT_SCALE_MAX};
	class ui_text_scaler : public ui_actor
	{
	private:
		i3UIStaticText* m_pText;
		INT32 m_origin_size;
		INT32 m_target_size;

		TEXT_SCALE_TYPE style;
	public:
		ui_text_scaler( i3UIControl* pControl = nullptr);
		virtual ~ui_text_scaler() {}
		void start( TEXT_SCALE_TYPE e, REAL32 fPeriod, bool use_call_back = false, REAL32 target_time = 0.f);
		void act();

		void restore() { m_pText->SetFont( m_pText->getFontName(), m_origin_size, FW_NORMAL, false);}
		void set_target( INT32 size) { m_target_size = size;}
	};

	typedef i3::shared_ptr<ui_slider> ui_slider_ptr;
	typedef i3::shared_ptr<ui_fader> ui_fade_ptr;
	typedef i3::shared_ptr<ui_text_scaler> ui_text_scaler_ptr;


	//i3UIEditBox Scroll 기능..
	class combo_scroll_updater
	{
		enum UPDATE_STATE
		{
			UPDATE_EMPTY = 0, UPDATE_CHANGE, UPDATE_END, UPDATE_MAX
		};

		void update_empty( REAL32 fDeltaSec) {}
		void update_change( REAL32 fDeltaSec);
		void update_end( REAL32 fDeltaSec);


		typedef void (combo_scroll_updater::*func)(REAL32);
		func updates[UPDATE_MAX];

		UPDATE_STATE current_state;
		i3UIEditBox* m_main_scroll;
		i3UIEditBox* m_sub_scroll;
		INT32 m_combo_level;
	public:
		combo_scroll_updater();
		~combo_scroll_updater();

		void update( REAL32 fDeltaSec) { (this->*updates[current_state])(fDeltaSec);}
		void set_scrolls( i3UIEditBox* main, i3UIEditBox* sub)
		{ 
			m_main_scroll = main; m_sub_scroll = sub;
			
			m_main_scroll->setContinueScrollPos( false);
			m_main_scroll->setScrollMoveSpeed( 5.f);
			m_main_scroll->setScrollState( i3UIEditBox::UBSS_END);

			m_sub_scroll->setContinueScrollPos( false);
			m_sub_scroll->setScrollMoveSpeed( 5.f);
			m_sub_scroll->setScrollState( i3UIEditBox::UBSS_END);
		}
		void set_combo( INT32 combo);
	};
	

	//combo event
	enum HUD_COMBO_EVENT
	{
		COMBO_E_START,
		COMBO_E_ADD,
		COMBO_E_END,

		COMBO_E_RECEIVE,	//서버에서 정보를 받음.

		COMBO_E_IS_COMBOING,
		COMBO_E_IS_WORKING,

		COMBO_E_MAX
	};

	struct combo_info
	{
		INT8 combo;
		INT16 score;
		INT32 calculated_score;
	};
};
namespace dui = DominationUI;

class CUIHudDomi_Combo : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_Combo, UIHUDDominationBase);
private:
	//연출 관련...
	dui::ui_fader*		m_FadeList[dui::combo_control_max];
	dui::ui_slider*			m_SlideList[dui::combo_control_max];
	dui::ui_text_scaler*	m_ScaleList[dui::combo_control_max];

	i3::vector<dui::ui_actor*>	m_ActiveList;

	bool push_active_list( dui::ui_actor* p);
	void pop_active_list( dui::ui_actor* p);

	void setup_fade_slide_scale();
	//




	//combo 관련 데이터
	REAL32	m_combo_time;	//combo 시간

	bool	m_have_add;		//add 할 점수를 가지고 있는 중
	bool	m_is_comboing;	//combo 중 (combo start ~ combo end) // m_sum 이 더하는 시간은 포함되지 않음.

	bool	m_is_working;	//(combo start ~ sum add to total score) // combo 시작 부터 m_sum이 더하는 시간 까지 포함.
	

	UINT32	m_combo;		//combo 횟수
	UINT32	m_add;			//add 되는 점수
	UINT32  m_sum;			//total 점수
	//

	dui::combo_scroll_updater m_scroller;	//
public:
	//control..
	i3UIFrameWnd*		m_pFrameWnd;
	//

	CUIHudDomi_Combo(void);
	~CUIHudDomi_Combo(void);

	virtual void OnCreate( i3GameNode* pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnUpdate( REAL32 rDeltaSec) override;

	virtual bool SetHudEvent( INT32 nEventNum, INT32 arg = 0) override;

	void cbFade( void* caller, void* pData);
	void cbSilde( void* caller, void* pData);
	void cbScale( void* caller, void* pData);
};


#ifdef apply_soon
{
	typedef i3::scoped_ptr<class actor_pattern> actor_pattern_ptr;	
	typedef i3::shared_ptr<actor_rsc_data>		actor_rsc_data_ptr;

	class actor_pattern
	{
	public:
		virtual void OnProgress(REAL32 ratio) = 0;
		virtual ~actor_pattern() {}
	};

	class actor_pattern_UIControl : public actor_pattern		//
	{
	protected:
		i3UIControl* m_pControl;
	};

	//	REAL32 m_fSlope;

	class sequence_callback
	{
	public: virtual ~sequence_callback() {}
			virtual void OnDone() = 0;
	};

	struct actor_sequence_rsc 
	{
		i3::vector<actor_rsc_data_ptr>		m_data_list;
	};

	class actor_sequence
	{
	public:
		actor_sequence(const actor_sequence_rsc& seq_rsc, sequence_callback* cb ) : m_done_count(0), m_callback(cb) 
		{
			const size_t num = seq_rsc.m_data_list.size();
			m_actor_list.reserve(num);
			for (size_t i = 0; i < num ; ++i )
			{
				m_actor_list.push_back(new actor(seq_rsc.m_data_list[i].get(), this) );
			}
		}

		void						OnUpdate(REAL32 fDelta)
		{
			const size_t num = m_actor_list.size();
			for (size_t i = 0; i < num ; ++i )
			{
				m_actor_list[i]->OnUpdate(fDelta);
			}
		}

		void						increment_done_count() 
		{   
			if ( ++m_done_count >= m_actor_list.size() )
			{
				m_callback->OnDone();	return;
			}
		}
	private:
		i3::vector<actor*>	m_actor_list;
		std::size_t			m_done_count;
		sequence_callback*	m_callback;
	};

	class actor_group : public sequence_callback
	{
	public:
		void	AddActor( INT32 seq_idx, actor_pattern* new_pattern, REAL32 start_time, REAL32 end_time ) 
		{    
			if ( seq_idx >= m_seq_rsc_list.size() )
			{
				m_seq_rsc_list.resize(seq_idx + 1);
			}

			actor_sequence_rsc& seq	= m_seq_rsc_list[seq_idx];
			seq.m_data_list.push_back(new actor_rsc_data(start_time, end_time, new_pattern) );
		}

		void	Start() 
		{ 
			m_curr_seq_list.clear();

			for (size_t i = 0; i < m_seq_rsc_list.size() ; ++i )
			{
				const actor_sequence_rsc& seq_rsc = m_seq_rsc_list[i]; 

				if (!seq_rsc.m_data_list.empty())
					m_curr_seq_list.push_back( new actor_sequence(seq_rsc, this) );

			}
		}

		void	OnUpdate(REAL32 fDelta)
		{
			if (!m_curr_seq_list.empty())
			{
				actor_sequence* seq = m_curr_seq_list.front();

				seq->OnUpdate(fDelta);

			}
		}

		virtual void OnDone() 
		{  
			I3ASSERT( !m_curr_seq_list.empty());
			m_curr_seq_list.pop_front();
		}

	private:
		i3::vector<actor_sequence_rsc>	m_seq_rsc_list;
		i3::deque<actor_sequence*>		m_curr_seq_list;
	};

	struct actor_rsc_data 
	{
		actor_rsc_data(REAL32 start_time, REAL32 end_time, actor_pattern* new_pattern) : start_time(start_time), end_time(end_time), pattern_data(new_pattern) {}
		REAL32 start_time;
		REAL32 end_time;
		actor_pattern_ptr pattern_data;
	};

	struct actor_data
	{
		actor_data(actor_rsc_data* rsc_data, actor_sequence* seq) : 
	m_rsc_data(rsc_data), m_sequence(seq), m_curr_state(&g_actor_state_delay), m_fProgressTime(0.f) {}
	actor_rsc_data*			m_rsc_data;
	actor_state*			m_curr_state;
	REAL32					m_fProgressTime;
	actor_sequence*			m_sequence;
	}

	class actor
	{
	public:
		actor(actor_rsc_data* rsc_data, actor_pattern* new_pattern) : m_actor_data( rsc_data, new_pattern) {}

		void OnUpdate(REAL32 fDeltaTime) 
		{
			m_actor_data.m_curr_state->OnUpdate( &m_actor_data, fDeltaTime);
		}
	private:
		actor_data			m_actor_data;
	}

	////////////////////////////////////

	////////////////////////////////////

	class actor_state
	{
	public:
		virtual void OnUpdate(actor_data* data, REAL32 fDeltaTime) = 0;
		virtual ~actor_state() {}
	};

	class actor_state_end : public actor_state
	{
	public:
		virtual void OnUpdate(actor_data* data, REAL32 fDeltaTime) {}

	} g_actor_state_end;

	class actor_state_delay : public actor_state
	{
		virtual void OnUpdate(actor_data* data, REAL32 fDeltaTime) 
		{
			actor_rsc_data*	rsc_data = data->m_rsc_data;

			data->m_fProgressTime += fDeltaTime;

			if (data->m_fProgressTime >= rsc_data->start_time)
			{
				data->m_curr_state = &g_actor_state_active;

				g_actor_state_active.OnUpdate(data, data->m_fProgressTime - rsc_data->start_time );
				return;
			}
		}
	} g_actor_state_delay;

	class actor_state_active : public actor_state
	{
		virtual void OnUpdate(actor_data* data, REAL32 fDeltaTime)
		{
			actor_rsc_data*			rsc_data = data->m_rsc_data;
			const REAL32			period = rsc_data->end_time - rsc_data->start_time;
			const REAL32			elapsed = data->m_fProgressTime - rsc_data->start_time;

			data->m_fProgressTime += fDeltaTime;

			REAL32 ratio = elapsed / period;

			if (data->m_fProgressTime < rsc_data->end_time) 
			{
				rsc_data->pattern_data->OnProgress(ratio);

			}
			else
			{
				rsc_data->pattern_data->OnProgress( 1.f );
				data->m_curr_state = &g_actor_state_end;
				data->m_sequence->increment_done_count();
				return;
			}
		}
	} g_actor_state_active;

}
#endif