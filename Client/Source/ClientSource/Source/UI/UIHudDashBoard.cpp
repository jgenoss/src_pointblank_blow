#include "pch.h"
#include "UIHudDashBoard.h"

#include "UIHudDefine.h"
#include "UIHudUtil.h"
#include "UIMath.h"

#include "UIHudManager.h"
#include "UIHudMinimap.h"

#include "DashBoardTarget.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "../StageBattle/DinoModeContext.h"

#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"

/******************************************/
namespace
{
	enum { TURN_TIME = 1000/4, MAX_WARNING_CNT = 14,};
	enum { NONE_1_5 = -1, 
		HELICOPTER_LEFT_1_5, HELICOPTER_RIGHT_1_5, GIRAN_RIGHT_1_5, GENERATOR_1_5, GIRAN_LEFT_1_5,  //파괴 미션
		TANK_A_1_5, TANK_B_1_5, TANK_DIE_1_5, //방어 미션
	}; //1.5 shape index
	enum { NONE_1_0 = -1,
		HELICOPTER_LEFT_1_0 = 0, HELICOPTER_RIGHT_1_0, HELICOPTER_GENERATOR_1_0, GIRAN_RIGHT_1_0 = 5, GENERATOR_1_0 = 3, GIRAN_LEFT_1_0 = 4, //파괴 미션
		TANK_A_1_0 = 0, TANK_B_1_0, TANK_DIE_1_0, //방머 미션
	}; //1.0 shape index
	enum { BG_NONE = 0, BG_RED, BG_BLUE, };

	HUD::TargetShapeType g_target_shape_type;

	bool g_is_default_shape = false;
	bool g_is_target_mode = false;
	bool g_is_escape_mode = false;
	bool g_is_tutorial_mode = false;
	bool g_is_outpost_helicopter = false;
}

/******************************************/
UIHudDashBoard::UIHudDashBoard(UIHudManager* p) : iHudBase(p)
{
	pf_InitializeAtLoad[HUD::HS_1_5] = &UIHudDashBoard::InitializeAtLoad_1_5;
	pf_InitializeAtLoad[HUD::HS_1_0] = &UIHudDashBoard::InitializeAtLoad_1_0;

	m_enable_update_time = false;
}

UIHudDashBoard::~UIHudDashBoard()
{
	for(size_t i=DEFAULT_SHAPE; i<MAX_SHAPE;++i)
	{
		for(size_t j=RED_TEAM;j<MAX_TEAM;++j)
		{
			I3_SAFE_DELETE( m_target[i][j]);
		}
	}

	I3_SAFE_DELETE( m_neutrality_target);
}

/*********************************/
void UIHudDashBoard::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	for(size_t i=DEFAULT_SHAPE; i<MAX_SHAPE;++i)
	{
		for(size_t j=RED_TEAM;j<MAX_TEAM;++j)
		{
			if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
			{
				m_target[i][j] = new dboard::target_1_5;
				MEMDUMP_NEW( m_target[i][j], sizeof( dboard::target_1_5));
			}
			else
			{
				m_target[i][j] = new dboard::target_1_0;
				MEMDUMP_NEW( m_target[i][j], sizeof( dboard::target_1_0));
			}
		}
	}

	if ( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		m_neutrality_target = new dboard::target_1_5;
		MEMDUMP_NEW( m_neutrality_target, sizeof( dboard::target_1_5));
	}
	else
	{
		m_neutrality_target = new dboard::target_1_0;
		MEMDUMP_NEW( m_neutrality_target, sizeof( dboard::target_1_0));
	}

	InitializeAtLoad_Common(pScene);
	(this->*pf_InitializeAtLoad[g_pEnvSet->m_nGameInterface])(pScene);
}

void UIHudDashBoard::ClearAtUnload( i3UIScene * pScene)
{
	for(size_t i=DEFAULT_SHAPE; i<MAX_SHAPE;++i)
	{
		for(size_t j=RED_TEAM;j<MAX_TEAM;++j)
		{
			I3_SAFE_DELETE( m_target[i][j]);
		}
	}

	I3_SAFE_DELETE( m_neutrality_target);
}

void UIHudDashBoard::InitializeAtLoad_Common( i3UIScene * pScene)
{

	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "DashBoard");

	m_pScore = FindControl<i3UIFrameWnd*>(pScene, "Score");
	m_pTutorial = FindControl<i3UIImageBox*>(pScene, "Tutorial");

	m_TeamBg = FindControl<i3UIButtonImageSet*>(pScene, "ts_ImgSet_Team");

	//Team Score
	m_Score[RED_TEAM].AddNumberControl(pScene, "ts_Img_Cnt0");
	m_Score[RED_TEAM].AddNumberControl(pScene, "ts_Img_Cnt1");
	m_Score[RED_TEAM].AddNumberControl(pScene, "ts_Img_Cnt2");

	m_Score[BLUE_TEAM].AddNumberControl(pScene, "ts_Img_Cnt3");
	m_Score[BLUE_TEAM].AddNumberControl(pScene, "ts_Img_Cnt4");
	m_Score[BLUE_TEAM].AddNumberControl(pScene, "ts_Img_Cnt5");
	if(g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		m_Score[RED_TEAM].EnableDynamicCenter();
		m_Score[BLUE_TEAM].EnableDynamicCenter();
	}

	//Time
	m_Time[MINUTE].AddNumberControl(pScene, "ts_Img_Cnt13");
	m_Time[MINUTE].AddNumberControl(pScene, "ts_Img_Cnt14");
	m_Time[MINUTE].EnableShowFrontZero(true);

	m_Time[SECOND].AddNumberControl(pScene, "ts_Img_Cnt10");
	m_Time[SECOND].AddNumberControl(pScene, "ts_Img_Cnt11");
	m_Time[SECOND].EnableShowFrontZero(true);
	if( g_pEnvSet->m_nGameInterface == HUD::HS_1_0 )
	{
		m_TimeSeparate = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText");
		m_TimeSeparate->SetText(":");
		m_TimeSeparate->EnableCtrl(true);
	}
	

	
	//Round
	m_ShieldGauge = FindControl<i3UIButtonImageSet*>(pScene, "db_ImgSet_Shield");
	m_RoundInfinite = FindControl<i3UIImageBox*>(pScene, "ts_Img_RoundInfinite");
	m_Round.AddNumberControl(pScene, "ts_Img_Round0");
	m_Round.AddNumberControl(pScene, "ts_Img_Round1");
	m_Round.AddNumberControl(pScene, "ts_Img_Round2");

#if ENABLE_UNIT_TEST
	m_RoundCtrl = FindControl<i3UIButtonImageSet*>(pScene, "ts_Img_Round0");
	m_RoundCtrl_Origin_Pos = *m_RoundCtrl->getPos();
#endif

	//Default Target
	dboard::target & default_red = *m_target[DEFAULT_SHAPE][RED_TEAM];
	default_red.init_control(pScene, "TargetShape0", "db_BtnImgSet_Target0", "db_BtnImgSet_Bg0", "db_Prog0");
	default_red.set_minimap_warning_idx(RED_TEAM);
	default_red.set_bg_idx(BG_RED, true);

	dboard::target & default_blue = *m_target[DEFAULT_SHAPE][BLUE_TEAM];
	default_blue.init_control(pScene, "TargetShape0", "db_BtnImgSet_Target1", "db_BtnImgSet_Bg1", "db_Prog1");
	default_blue.set_minimap_warning_idx(BLUE_TEAM);
	default_blue.set_bg_idx(BG_BLUE, true);
	default_blue.set_progress_invert();

	//Parallel Target
	dboard::target & parallel_main = *m_target[PARALLEL_SHAPE][TANK_MAIN];
	parallel_main.init_control(pScene, "TargetShape1", "db_BtnImgSet_Target2", "db_BtnImgSet_Bg2", "db_Prog2");
	parallel_main.set_minimap_warning_idx(TANK_MAIN);
	parallel_main.set_bg_idx(BG_RED, false);
	parallel_main.set_progress_invert();

	dboard::target & parallel_sub = *m_target[PARALLEL_SHAPE][TANK_SUB];
	parallel_sub.init_control(pScene, "TargetShape1", "db_BtnImgSet_Target3", "db_BtnImgSet_Bg3", "db_Prog3");
	parallel_sub.set_minimap_warning_idx(TANK_SUB);
	parallel_sub.set_bg_idx(BG_RED, false);
	parallel_sub.set_progress_invert();

	// Outpost Helicopter
	dboard::target & outpost_helicopter = *m_neutrality_target;
	outpost_helicopter.init_control(pScene, "Neutrality_Target", "db_BtnImgSet_Target4", "db_BtnImgSet_Bg4", "db_Prog4", false);
	outpost_helicopter.set_bg_idx(BG_NONE, false);
	if (g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		i3UIButtonImageSet* pTargetWarning = FindControl<i3UIButtonImageSet*>(pScene, "db_BtnImgSet_Bg4");
		pTargetWarning->SetShapeIdx(1);
	}
	outpost_helicopter.set_enable(false);
}


void UIHudDashBoard::InitializeAtLoad_1_5( i3UIScene * pScene)
{
	i3_prof_func();

	//Team Mark
	m_TeamMark[RED_TEAM] = FindControl<i3UIButtonImageSet*>(pScene, "ts_Img_Mark0");
	m_TeamMark[BLUE_TEAM] = FindControl<i3UIButtonImageSet*>(pScene, "ts_Img_Mark1");

	//Team Score
	m_Score[RED_TEAM].SetWidthOffset(-4);
	m_Score[BLUE_TEAM].SetWidthOffset(-4);

	//Time
	i3UIButtonImageSet* btn = FindControl<i3UIButtonImageSet*>(pScene, "ts_Img_Cnt12");
	btn->SetShapeIdx(10);

	//Round
	m_Round.EnableDynamicCenter();
	m_Round.SetWidthOffset(-3);

	//Parallel Target
	dboard::target & parallel_main = *m_target[PARALLEL_SHAPE][TANK_MAIN];
	parallel_main.set_target_die_shape_idx(TANK_DIE_1_5);

	dboard::target & parallel_sub = *m_target[PARALLEL_SHAPE][TANK_SUB];
	parallel_sub.set_target_die_shape_idx(TANK_DIE_1_5);
}

void UIHudDashBoard::InitializeAtLoad_1_0( i3UIScene * pScene)
{
	i3_prof_func();

	m_Score[RED_TEAM].EnableShowFrontZero(true);
	m_Score[BLUE_TEAM].EnableShowFrontZero(true);

	m_Round.EnableDynamicCenter();
	m_Round.SetWidthOffset(-3);
	m_Round.EnableShowFrontZero(false);

	dboard::target & default_red = *m_target[DEFAULT_SHAPE][RED_TEAM];
	default_red.set_bg_idx(BG_RED-1, true);

	dboard::target & default_blue = *m_target[DEFAULT_SHAPE][BLUE_TEAM];
	default_blue.set_bg_idx(BG_BLUE-1, true);

	//Parallel Target
	dboard::target & parallel_main = *m_target[PARALLEL_SHAPE][TANK_MAIN];
	parallel_main.set_target_die_shape_idx(TANK_DIE_1_0);

	dboard::target & parallel_sub = *m_target[PARALLEL_SHAPE][TANK_SUB];
	parallel_sub.set_target_die_shape_idx(TANK_DIE_1_0);
}

void UIHudDashBoard::init_team_mark()
{
	i3_prof_func();

	if (g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		//team 마크 카운트 설정
		INT32 red, blue;

		TEAM_TYPE my_team = hu::get_team_mark(s_pMyChara, red, blue);

		if (red == -1)	m_TeamMark[RED_TEAM]->EnableCtrl(false);
		else	m_TeamMark[RED_TEAM]->SetShapeIdx(red);

		if (blue == -1)	m_TeamMark[BLUE_TEAM]->EnableCtrl(false);
		else	m_TeamMark[BLUE_TEAM]->SetShapeIdx(blue);

		if (my_team == TEAM_NONE) return;

		//다른 팀은 알파 넣어준다.
		enum { ALPHA = 60, };

		TEAM_TYPE other_team = (my_team == TEAM_RED) ? TEAM_BLUE : TEAM_RED;

		COLOR* col = m_TeamMark[other_team]->getColor();
		col->a = ALPHA;
		m_TeamMark[other_team]->setColor(col); //팀 마크

		// 같은 팀은 진하게 바꾼다.
		col = m_TeamMark[my_team]->getColor();
		col->a = 255;
		m_TeamMark[other_team]->setColor(col); //팀 마크

		m_Score[TEAM_RED].SetAlpha(255); //팀 스코어
		m_Score[TEAM_BLUE].SetAlpha(255); //팀 스코어
		m_Score[other_team].SetAlpha(ALPHA); //팀 스코어
	}
	int TeamType = hu::getMyTeam();
	m_TeamBg->SetShapeIdx((TeamType != CHARACTER_TEAM_RED)); //팀 Bg 선택
}

void	UIHudDashBoard::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 Minute =0, Second =0;
	if( m_enable_update_time == true )
		hu::get_running_time(Minute, Second);
	m_Time[MINUTE].SetNumber(Minute);
	m_Time[SECOND].SetNumber(Second);

	INT32 Red = 0, Blue = 0;
	hu::get_team_score(Red, Blue);
	m_Score[RED_TEAM].SetNumber(Red);
	m_Score[BLUE_TEAM].SetNumber(Blue);

	if( g_is_tutorial_mode == true ) return;

	if( g_is_escape_mode )
	{
		hu::update_escape_block_time(rDeltaSeconds);
		INT32 idx = hu::get_escape_block_time_shape_idx();
		m_ShieldGauge->SetShapeIdx(idx);
	}
	else if( g_is_target_mode )
	{
		INT32 red_hp, max_red_hp, blue_hp, max_blue_hp;
		hu::get_target_object_hp(red_hp, max_red_hp, blue_hp, max_blue_hp);

		dboard::target & obj1 = *m_target[ !g_is_default_shape ][RED_TEAM];
		obj1.set_hp(red_hp, max_red_hp);
		obj1.update(rDeltaSeconds);

		dboard::target & obj2 = *m_target[ !g_is_default_shape ][BLUE_TEAM];
		obj2.set_hp(blue_hp, max_blue_hp);
		obj2.update(rDeltaSeconds);
	}
	else if(g_is_outpost_helicopter)
	{
		dboard::target & obj = *m_neutrality_target;
		obj.update(rDeltaSeconds);
		obj.update_color();
	}
}

const VEC3D* UIHudDashBoard::GetTimeCtrlPos() const
{
	return m_Time[MINUTE].GetNumberPos();
}

REAL32 UIHudDashBoard::GetTimeCtrlHeight() const
{
	return m_Time[MINUTE].GetNumberHeight();
}

/*********************************/
void UIHudDashBoard::change_target_shape()
{
	i3_prof_func();

	UIHudMinimap* mini_map = static_cast< UIHudMinimap* >( HUD::instance()->GetHud(HUD::MINIMAP) );
	mini_map->SetObjectUnderAttack( RED_TEAM, false);
	mini_map->SetObjectUnderAttack( BLUE_TEAM, false);

	INT32 red_type = -1, blue_type = -1;

	if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		switch( g_target_shape_type )
		{
		case HUD::TST_GENERATOR:		red_type = HELICOPTER_LEFT_1_5;		blue_type = GENERATOR_1_5;		break;
		case HUD::TST_HELICOPTER:		red_type = HELICOPTER_LEFT_1_5;		blue_type = HELICOPTER_RIGHT_1_5;		break;
		case HUD::TST_GIRAN:		red_type = GIRAN_LEFT_1_5;		blue_type = GIRAN_RIGHT_1_5;		break;
		case HUD::TST_TANK:
			m_target[PARALLEL_SHAPE][RED_TEAM]->set_target_normal_shape_idx(TANK_A_1_5);
			m_target[PARALLEL_SHAPE][BLUE_TEAM]->set_target_normal_shape_idx(TANK_B_1_5);
			break;
		}
	}
	else
	{
		switch( g_target_shape_type )
		{
		case HUD::TST_GENERATOR:		red_type = HELICOPTER_GENERATOR_1_0;		blue_type = GENERATOR_1_0;		break;
		case HUD::TST_HELICOPTER:		red_type = HELICOPTER_LEFT_1_0;		blue_type = HELICOPTER_RIGHT_1_0;		break;
		case HUD::TST_GIRAN:		red_type = GIRAN_LEFT_1_0;		blue_type = GIRAN_RIGHT_1_0;		break;
		case HUD::TST_TANK:
			m_target[PARALLEL_SHAPE][RED_TEAM]->set_target_normal_shape_idx(TANK_A_1_0);
			m_target[PARALLEL_SHAPE][BLUE_TEAM]->set_target_normal_shape_idx(TANK_B_1_0);
			break;
		}
	}

	if( red_type != -1) m_target[DEFAULT_SHAPE][RED_TEAM]->set_target_normal_shape_idx(red_type);
	if( blue_type != -1) m_target[DEFAULT_SHAPE][BLUE_TEAM]->set_target_normal_shape_idx(blue_type);
}


/*********************************/
void UIHudDashBoard::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	switch(arg1)
	{
	case HUD::DBS_TargetShow:
		{
			if( g_is_default_shape ) return;

			bool enable = arg2 ? true : false;
			m_target[ PARALLEL_SHAPE ][RED_TEAM]->set_enable(enable);
			m_target[ PARALLEL_SHAPE ][BLUE_TEAM]->set_enable(enable);
		}
		break;
	case HUD::DBS_SetGameTimeZero: //게임 시간은 서버에서 관리하기 때문에 임의로 00:00 만들어도 된다.
		{
			m_Time[MINUTE].SetNumber(0);
			m_Time[SECOND].SetNumber(0);
		}
		break;
	case HUD::DBS_SetRunningTime:
		{
			m_enable_update_time = true;
			hu::set_running_time(arg2);
		}
	break;
	case HUD::DBS_SetRidingHelicopter:
		{
			bool enable = arg2 ? true : false;
			g_is_outpost_helicopter = enable;
			m_neutrality_target->set_enable(g_is_outpost_helicopter);
			m_neutrality_target->set_enable_update(g_is_outpost_helicopter);
		}
	break;
	case HUD::DBS_SetUnderAttack:
		{
			m_neutrality_target->set_hp(arg2, 3000);
		}
	break;
	}
}


/*********************************/
void UIHudDashBoard::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	//제압모드는 다른 데시 보드를 사용 하기 때문에.
	if( hu_ut::is_domination_mode() ) 
	{
		iHudBase::SetEnable(false);
		return;
	}

	m_pScore->EnableCtrl(!hu_ut::is_tutorial_mode());
	m_pTutorial->EnableCtrl(hu_ut::is_tutorial_mode());
	//난입 유저일 경우엔 다른 패킷이 안 들어 오기 때문에 true로 설정한다.
	bool is_break_into_user = ( UserContext::i()->IsPlayingSlot( s_pMyChara->getCharaNetIndex() ) == false);
	iHudBase::SetVisible(is_break_into_user);

	g_is_escape_mode = hu_ut::is_dino_escape_mode();
	g_is_tutorial_mode = hu_ut::is_tutorial_mode();
	g_is_target_mode = hu::is_target_mode(g_target_shape_type, g_is_default_shape);

	if( hu_ut::is_test_changing_hud() == true )
	{
		m_Round.HideRound();
		m_Round.ResetDisplay();

#if ENABLE_UNIT_TEST
		//일반적인 battle 일 때 stage mode에 따라 한 번 셋팅을 하지만,
		//hud unit test는 stage mode가 계속적으로 변경되기 때문에 위치가 이동되기 때문에 초기 위치로 설정해야 된다.
		m_RoundCtrl->setPos(&m_RoundCtrl_Origin_Pos); 
#endif
	}

	//여기서 모드 비활성화 상태로 시작하자.
	m_RoundInfinite->EnableCtrl(false);
	m_Round.SetVisible(false);
	m_ShieldGauge->EnableCtrl(false);
	
	//각각의 모드에 따라서 활성화 하자.
	if( g_is_escape_mode )	//탈출 모드
	{
		m_ShieldGauge->EnableCtrl(true);
		m_ShieldGauge->SetShapeIdx(0);

		DinoModeContext::i()->eEscapeBlock	= TOUCHDOWN_BLOCK_STATE_INIT;
		DinoModeContext::i()->rDelayTime		= 0.0f;
	}
	else//라운드 모드, 킬 모드
	{
		bool is_round = false;
		INT32 round_goal_cnt = hu::get_round_goal_count(is_round);
	

		if( round_goal_cnt == -1 )	//AI 모드
		{
			m_RoundInfinite->EnableCtrl(true);	//무한 라운드
		}
		else
		{
			if( is_round )	//라운드 모드
			{
				m_Round.ShowRound();
				if( hu_ut::is_test_changing_hud() == false )
				{
					m_Round.SetWidthOffset(-2);
					m_Round.InitialPosOffset(-1);
				}
			}
			
			m_Round.SetVisible(true);
			m_Round.SetNumber(round_goal_cnt);
		}
	}

	//타켓 hud 설정
	for(size_t target_cnt=0; target_cnt<MAX_TEAM;++target_cnt)
	{
		dboard::target & obj1 = *m_target[ DEFAULT_SHAPE ][target_cnt];
		obj1.set_enable(g_is_target_mode && g_is_default_shape);

		dboard::target & obj2 = *m_target[ PARALLEL_SHAPE ][target_cnt];
		obj2.set_enable(g_is_target_mode && !g_is_default_shape);
	}

	if (g_is_target_mode == true)
		change_target_shape();

	//if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5 )
	init_team_mark();

	dboard::target &obj = *m_neutrality_target;
	obj.set_enable(false);
	obj.set_enable_update(false);
}

void UIHudDashBoard::event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	//각각의 모드에 따라서 활성화 하자.
	if (hu_ut::is_cross_count_mode())	//CC 모드
	{
		bool is_round = false;
		INT32 round_goal_cnt = hu::get_round_goal_count(is_round);
		m_Round.SetNumber(round_goal_cnt);
	}

	//게임 중 캐릭터를 삭제 -> 생성 하기 때문에 다시 세팅 함.
	//if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)// && !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
		init_team_mark();

		dboard::target &obj = *m_neutrality_target;
		obj.set_enable(false);
}

void UIHudDashBoard::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	if( hu_ut::is_domination_mode() == true ) return;

	iHudBase::SetVisible(true);

	//target에 update가 멈춤게 있을 경우 다시 활성화 해준다.
	for(size_t target_cnt=0; target_cnt<MAX_TEAM;++target_cnt)
	{
		INT32 RG_HP[MAX_TEAM] = { 0, };
		INT32 RGMAX_HP[MAX_TEAM] = { 0, };

		hu::get_target_object_hp(RG_HP[RED_TEAM], RGMAX_HP[RED_TEAM], RG_HP[BLUE_TEAM], RGMAX_HP[BLUE_TEAM]);

		if (hu_ut::is_destroy_mission_mode() == true)
		{
			dboard::target & obj = *m_target[DEFAULT_SHAPE][target_cnt];
			obj.set_enable_update(g_is_target_mode && g_is_default_shape);
			if (RG_HP[target_cnt] != 0)
				obj.reset_target_shape();
		}

		if (hu_ut::is_defence_mission_mode() == true)
		{
			dboard::target & obj = *m_target[PARALLEL_SHAPE][target_cnt];
			obj.set_enable_update(g_is_target_mode && !g_is_default_shape);
			if (RG_HP[target_cnt] != 0)
				obj.reset_target_shape();
		}
	}

	if(	MyRoomInfoContext::i()->getStage()->GetStageMode() == STAGE_MODE_CROSSCOUNT
		|| MyRoomInfoContext::i()->getStage()->GetStageMode() == STAGE_MODE_ESCAPE)
	{
		if(s_pMyChara->IsDino())
			s_pMyChara->setCharaInfo()->SetTeam(CHARACTER_TEAM_RED);
		else
			s_pMyChara->setCharaInfo()->SetTeam(CHARACTER_TEAM_BLUE);

		//if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
			init_team_mark();		
	}
}

void UIHudDashBoard::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( !hu_ut::is_domination_mode() );
}

void UIHudDashBoard::SetRidingHelicopter(bool bRiding)
{
	g_is_outpost_helicopter = bRiding;
}