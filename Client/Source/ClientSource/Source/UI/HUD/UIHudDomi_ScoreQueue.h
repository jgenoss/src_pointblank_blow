#pragma once

#include "UIHudDomi_SpecialScore.h"

#include "i3Base/itl/queue.h"


class UIHUDDomi_ScoreQueue;
namespace DominationScoreQueue
{
	enum TEXT_TYPE
	{
		TEXT_ASSIST_KILL,
		TEXT_ASSIST_SP,
		TEXT_KILL,
		TEXT_HEAD_SHOT,
		TEXT_SP,

		TEXT_MAX
	};

	enum HUD_SCORE_EVENT
	{
		SCORE_E_DEBUG,
		SCORE_E_IS_WORKING_SCORE,
		SCORE_E_IS_WORKING_SP,

		SCORE_E_KILL_DINO,
		SCORE_E_ASSIST_DINO,

		SCORE_E_KILL_SP,
		SCORE_E_ASSIST_SP,
	};

	struct score_data
	{
		UINT16 sp;
		INT16 score;
		INT8  combo;
		INT32 calculated_score;
		score_data() { sp = 0; score = 0; combo = 0; calculated_score = 0;}
		score_data( UINT16 _sp, INT16 _score, INT8 _combo)
		{
			sp = _sp; score = _score; combo = _combo;
		}
	};

	class score_info
	{	
	protected:
		UIHUDDomi_ScoreQueue* m_pScene;

	public:
		score_info( UIHUDDomi_ScoreQueue* scene, i3UIStaticText* ui) : m_pScene(scene), m_ui(ui), m_time(0.f) {}
		virtual ~score_info() {}

		void update( REAL32 fDeltaTime);
		virtual void reset_score( score_data _data)
		{
			m_data = _data;
			m_data.calculated_score = m_data.combo*m_data.score;
		}
		virtual void merge_score( score_data _data)
		{
			INT16 merged_sp = m_data.sp + _data.sp;
			INT16 merged_score = m_data.score + _data.score;
			INT32 merged_calc = m_data.calculated_score + (_data.combo*_data.score);

			m_data = _data;
			m_data.calculated_score = merged_calc;
			m_data.sp = merged_sp;
			m_data.score = merged_score;
		}
		void move_to();
		
		score_data m_data;
		
		TEXT_TYPE m_type;
		REAL32 m_time;
		i3UIStaticText* m_ui;
	};
	typedef i3::shared_ptr<score_info> score_info_ptr;
	
	
	class score_info_assist_kill : public score_info
	{
	public:
		score_info_assist_kill( UIHUDDomi_ScoreQueue* scene, i3UIStaticText* ui) : score_info( scene, ui) {}
		virtual ~score_info_assist_kill() {}

		virtual void reset_score( score_data _data)
		{
			m_data = _data;
			m_data.calculated_score = m_data.score;
		}
		virtual void merge_score( score_data _data)
		{
			INT16 merged_sp = m_data.sp + _data.sp;
			INT16 merged_score = m_data.score + _data.score;
			INT32 merged_calc = m_data.calculated_score + (_data.combo*_data.score);

			m_data = _data;
			m_data.calculated_score = merged_calc;
			m_data.sp = merged_sp;
			m_data.score = merged_score;
		}
	};

	
	class score_control
	{		
	public:
		score_control() {}
		~score_control() {}

		bool update( REAL32 fDeltaTime);

		score_data m_data;

		dui::ui_slider_ptr m_slider;
		dui::ui_fade_ptr m_fader;
	};
	typedef i3::shared_ptr<score_control> score_control_ptr;
};
namespace dsq = DominationScoreQueue;

class UIHUDDomi_ScoreQueue : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( UIHUDDomi_ScoreQueue, UIHUDDominationBase);

	enum { UI_TEXT_MAX = 32};
private:
	i3UIFrameWnd* m_frame;
	i3::vector<i3UIStaticText*> m_free;
	
	i3::vector<dsq::score_info_ptr> m_score_info_list;
	i3::vector<dsq::score_control_ptr> m_score_ctrl_list;
	

	i3UIStaticText* _GetTextUI();
	i3UIStaticText* m_main_texts[dsq::TEXT_MAX];
public:
	UIHUDDomi_ScoreQueue();
	~UIHUDDomi_ScoreQueue();

	virtual void OnCreate( i3GameNode* parent) override;
	virtual void OnUpdate( REAL32 rDeltaSec) override;

	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool SetHudEvent( INT32 nEventNum, INT32 arg = 0) override;

	void move_to_ctrl( dsq::score_info* _score_info);

	void cb_add_total_score( void* p1, void* p2);
	void cb_add_combo_score( void* p1, void* p2);
	void cb_add_sp( void* p1, void* p2);
};