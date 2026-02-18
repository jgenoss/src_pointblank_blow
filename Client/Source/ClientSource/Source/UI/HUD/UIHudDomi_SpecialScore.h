#pragma once

#include "UIHUD_Domination_Base.h"
#include "UIHudDomi_Combo.h"


#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(score_0) \
	enum_value(score_1) \
	enum_value(score_2) \
	enum_value(special_max)

#undef DO_ENUM
#define DO_ENUM(e) e,

#undef DO_DESC
#define DO_DESC(e) _T(#e),

namespace DominationUI
{
	enum SPECIAL_CONTROL 
	{ ENUM_LIST(DO_ENUM)};

	static const char* g_special_desc[] =   
	{ ENUM_LIST(DO_DESC)};

	class ui_3d_slider : public ui_slider
	{
		VEC3D* pos_3d;	//3d ±‚¡ÿ ¡¬«•
		REAL32 x_d;
		REAL32 y_d;
		REAL32 width;  
		bool calc_3d_to_2d();  
	public:
		ui_3d_slider( i3UIControl* pControl = nullptr);
		~ui_3d_slider() {}

		void set_src_3d_pos( VEC3D* pos, REAL32 x_dist = 0.f, REAL32 y_dist = 0.f) { pos_3d = pos; x_d = x_dist; y_d = y_dist;}
		void start( SLIDE_TYPE e, REAL32 fPeriod, bool use_call_back = false, REAL32 target_time = 0.f);
		void act();
	};
	typedef i3::shared_ptr<ui_3d_slider> ui_3d_slider_ptr;

	enum SPECIAL_CONTROL_DETAIL { S_CONTROL_D_STATE, S_CONTROL_D_SCORE, S_CONTROL_D_MAX};
	enum SPECIAL_SLIDE_DETAIL { S_SLIDE_D_UP, S_SLIDE_D_ADD, S_SLIDE_D_MAX};
	class special_score
	{
		ui_slider_ptr		frame_slider[S_SLIDE_D_MAX];	//frame 
		ui_fade_ptr			control_fader[S_CONTROL_D_MAX];	//

	public:
		special_score() : me(false), score(0) {}
		~special_score() {}

		void set_slider( SPECIAL_SLIDE_DETAIL e, ui_slider_ptr& s) { I3_BOUNDCHK(e, S_SLIDE_D_MAX); frame_slider[e] = s;}
		void set_fader( SPECIAL_CONTROL_DETAIL e,ui_fade_ptr& f) { I3_BOUNDCHK(e, S_CONTROL_D_MAX); control_fader[e] = f;}

		ui_slider_ptr get_slider( SPECIAL_SLIDE_DETAIL e) { I3_BOUNDCHK(e, S_SLIDE_D_MAX); return frame_slider[e];}
		ui_fade_ptr get_fader( SPECIAL_CONTROL_DETAIL e) { I3_BOUNDCHK(e, S_CONTROL_D_MAX); return control_fader[e];}

		void update( REAL32 fEapsedTime);
		void start( const i3::wliteral_range& state, const i3::wliteral_range& score, COLOR* col, VEC3D* target_pos);

		i3UIFrameWnd* get_frame() { return (i3UIFrameWnd*)frame_slider[S_SLIDE_D_UP]->get_control();}

		bool me;
		INT16 score;
	};

	enum HUD_SPECIAL_EVENT
	{
		SPECIAL_E_START,
		SPECIAL_E_IS_WORKING,

		SPECIAL_E_MAX
	};
};
namespace dui = DominationUI;

class CUIHudDomi_SpecialScore : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_SpecialScore, UIHUDDominationBase);

private:
	dui::special_score*	m_special_ui[dui::special_max];

	i3::vector<dui::special_score*>	m_free_list;
	i3::vector<dui::special_score*>	m_active_list;

	void get_state( INT8 effect, i3::rc_wstring& out, COLOR& col);

public:
	CUIHudDomi_SpecialScore(void);
	~CUIHudDomi_SpecialScore(void);

	virtual void OnCreate( i3GameNode* pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool OnEntranceStart( void * pArg1  = nullptr , void * pArg2  = nullptr) override;

	virtual void OnUpdate( REAL32 rDeltaSec) override;

	void cb_slider_up( void* ui_actor, void* data);
	void cb_slider_add( void* ui_actor, void* data);

	virtual bool SetHudEvent( INT32 nEventNum, INT32 arg = 0) override;
};
