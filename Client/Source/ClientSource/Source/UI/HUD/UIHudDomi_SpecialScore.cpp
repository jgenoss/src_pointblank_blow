#include "pch.h"
#include "UIHudDomi_SpecialScore.h"

#include "../../GameCharaBoneContext.h"

#include "UIHUD_Domination_Manager.h"
#include "../../Mode/Domination/Domination_Defines.h"

#include "../../../../../Common/CommonSource/CommonDediCli.h"
#include "BattleSlotContext.h"

namespace DominationUI
{
	ui_3d_slider::ui_3d_slider( i3UIControl* pControl /*= nullptr*/) : ui_slider(pControl), pos_3d(nullptr), x_d(0.f), y_d(0.f)
	{ 
		if( pControl)   
			width = pControl->getWidth();
	}		
	
	void ui_3d_slider::start( SLIDE_TYPE e, REAL32 fPeriod, bool use_call_back /*= false*/, REAL32 target_time /*= 0.f*/)
	{  
		ui_slider::start( e, fPeriod, use_call_back, target_time);
		
		bool enalbe = calc_3d_to_2d();
		m_pControl->EnableCtrl( enalbe == true);

		if( style == SLIDE_IN) m_pControl->setPos( &m_start);
		else m_pControl->setPos( &m_end);
	}

	bool ui_3d_slider::calc_3d_to_2d()
	{
		VEC3D	temp_out;  
		VEC3D	temp_in;
		MATRIX	camera_mat;
		VEC3D*	camera_at;
		VEC3D	sub;

		camera_at = i3Matrix::GetAt( g_pCamera->getCamMatrix());

		i3Vector::Sub( &sub, i3Matrix::GetPos( g_pCamera->getCamMatrix()), pos_3d);
		i3Vector::Normalize( &sub, &sub);

		REAL32 rDot = i3Vector::Dot( camera_at, &sub);
		if( rDot <= 0.0f) 
			return false;

		i3Matrix::Mul( &camera_mat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

		i3Vector::Set( &temp_in, pos_3d->x, pos_3d->y + 1.5f, pos_3d->z);
		i3Vector::TransformCoord( &temp_out, &temp_in, &camera_mat);

		temp_out.x = temp_out.x * 0.5f + 0.5f;
		temp_out.y = temp_out.y * -0.5f + 0.5f;
		temp_out.z = 0.f;

		//
		VEC3D* camera_pos = i3Matrix::GetPos( g_pCamera->getCamMatrix());
		i3Vector::Sub( &temp_in, camera_pos, pos_3d);
		INT32 nLength = (INT32)i3Vector::Length(&temp_in);
		if( nLength > 9000 || nLength < -9000) nLength = 0;

		i3Viewer* pViewer = g_pFramework->GetViewer();

		temp_out.x = (temp_out.x * (REAL32)pViewer->GetViewWidth()) - (width * 0.5f);
		temp_out.y = (temp_out.y * (REAL32)pViewer->GetViewHeight()) - (nLength);


		REAL32 ui_ratio = (REAL32)i3UI::getManager()->getWidth() / (REAL32)i3UI::getManager()->getHeight();
		REAL32 view_ratio = (REAL32)pViewer->GetViewWidth() / (REAL32)pViewer->GetViewHeight();

		INT32 offset_x = i3UI::getManager()->getDisplayOffsetX();
		INT32 offset_y = i3UI::getManager()->getDisplayOffsetY();

		VEC3D v;
		v.x = (REAL32)pViewer->GetViewWidth() / (REAL32)i3UI::getManager()->getWidth();
		v.y = (REAL32)pViewer->GetViewHeight() / (REAL32)i3UI::getManager()->getHeight();

		REAL32 ratio = v.y;
		if( ui_ratio > view_ratio)
			ratio = v.x;

		temp_out.x = (temp_out.x/ratio) - offset_x;
		temp_out.y = (temp_out.y/ratio) - offset_y;

		//start, end setting
		i3Vector::Copy( &m_start, &temp_out);
		i3Vector::Set( &m_end, temp_out.x+x_d, temp_out.y+y_d, temp_out.z);

		return true;
	}  
    
	void ui_3d_slider::act()
	{
		if( pos_3d == nullptr) return;

		bool enalbe = calc_3d_to_2d();
		m_pControl->EnableCtrl( enalbe == true);

		ui_slider::act();
	}
  
	void special_score::update( REAL32 fEapsedTime)
	{   
		for( INT32 i = 0; i < S_SLIDE_D_MAX; ++i)
			frame_slider[i]->OnUpdate( fEapsedTime);
  
		for( INT32 i = 0; i < S_CONTROL_D_MAX; ++i)
		  	control_fader[i]->OnUpdate( fEapsedTime);
	}     

	void special_score::start( const i3::wliteral_range& state, const i3::wliteral_range& score, COLOR* col, VEC3D* target_pos)
	{
		//data setting  
		i3UIStaticText* con_state = (i3UIStaticText*)control_fader[S_CONTROL_D_STATE]->get_control();  
		i3UIStaticText* con_score = (i3UIStaticText*)control_fader[S_CONTROL_D_SCORE]->get_control();
  
		con_state->SetText( state);
		con_state->SetTextColor( col);
		con_score->SetText( score);

		for( INT32 i = 0; i < S_CONTROL_D_MAX; ++i)
		{
			control_fader[i]->restore();
			control_fader[i]->set_enable( false);
		}

		frame_slider[S_SLIDE_D_UP]->set_enable( true);
		frame_slider[S_SLIDE_D_ADD]->set_enable( false);

		if( me)
		{
			//ui_control setting     
			frame_slider[S_SLIDE_D_UP]->set_src_3d_pos( target_pos, 0.f, -40.f);
			frame_slider[S_SLIDE_D_UP]->start( dui::SLIDE_IN, 3.f, true, 0.8f);
		}
		else
		{
			//ui_control setting     
			frame_slider[S_SLIDE_D_UP]->set_src_3d_pos( target_pos, 0.f, -50.f);
			frame_slider[S_SLIDE_D_UP]->start( dui::SLIDE_IN, 4.f, true, 0.8f);	

			control_fader[S_CONTROL_D_STATE]->start( dui::FADE_OUT, 5.f);
			control_fader[S_CONTROL_D_SCORE]->start( dui::FADE_OUT, 5.f);

			control_fader[S_CONTROL_D_STATE]->set_enable( true);
			control_fader[S_CONTROL_D_SCORE]->set_enable( true);
		}
	}
};

LuaFuncReg UIHUDDomi_SpecialScore_Glue[] = {  
	{nullptr,				nullptr}  
};  

I3_CLASS_INSTANCE(CUIHudDomi_SpecialScore);

CUIHudDomi_SpecialScore::CUIHudDomi_SpecialScore(void)
{
	memset( m_special_ui, 0, sizeof(dui::special_score*)*dui::special_max);  
} 

CUIHudDomi_SpecialScore::~CUIHudDomi_SpecialScore(void)
{
	m_free_list.clear();
	m_active_list.clear();

	for( size_t i = 0; i < dui::special_max; ++i)
	{
		I3_SAFE_DELETE( m_special_ui[i]);
	}
}

void CUIHudDomi_SpecialScore::OnCreate( i3GameNode* pParent)
{
	UIHUDDominationBase::OnCreate( pParent);  

	AddScene( "Scene/Domination/PBRe_Domi_statescore.i3UIs", UIHUDDomi_SpecialScore_Glue, true, false);  
}

void CUIHudDomi_SpecialScore::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDDominationBase::_InitializeAtLoad( pScene);  

	for( size_t i = 0; i < dui::special_max; ++i)  
	{  
		m_special_ui[i] = new dui::special_score();
		MEMDUMP_NEW( m_special_ui[i], sizeof(dui::special_score) );
		
		//slider setting  
		i3UIFrameWnd* frame = (i3UIFrameWnd*)pScene->FindChildByName( dui::g_special_desc[i]);  

		dui::ui_slider * pSlider = new dui::ui_slider( frame);
		MEMDUMP_NEW( pSlider, sizeof( dui::ui_slider));
		dui::ui_slider_ptr add_slide_ptr( pSlider);

		dui::member_call_back<CUIHudDomi_SpecialScore> * pSpecialScore = new dui::member_call_back<CUIHudDomi_SpecialScore>( this,
			&CUIHudDomi_SpecialScore::cb_slider_add, reinterpret_cast<void*>(m_special_ui[i]));
		MEMDUMP_NEW( pSpecialScore, sizeof(dui::member_call_back<CUIHudDomi_SpecialScore>));

		dui::call_back_ptr add_cb_ptr( pSpecialScore);
		add_slide_ptr->set_call_back( add_cb_ptr);  


		dui::ui_3d_slider * p3DSlider = new dui::ui_3d_slider( frame);
		MEMDUMP_NEW( p3DSlider, sizeof( dui::ui_3d_slider));
		dui::ui_slider_ptr up_slide_ptr( p3DSlider);

		pSpecialScore = new dui::member_call_back<CUIHudDomi_SpecialScore>( this,
			&CUIHudDomi_SpecialScore::cb_slider_up, reinterpret_cast<void*>(m_special_ui[i]));
		MEMDUMP_NEW( pSpecialScore, sizeof( dui::member_call_back<CUIHudDomi_SpecialScore>));
		dui::call_back_ptr up_cb_ptr( pSpecialScore);
		up_slide_ptr->set_call_back( up_cb_ptr);  


		char buf[64] = {0,};
		itoa( i, buf, 10);
		i3::string str_state = "textup_"; str_state += buf;
		i3::string str_score = "textdown_"; str_score += buf;


		//fader setting  
		i3UIStaticText* ui_state = (i3UIStaticText*)frame->FindChildByName( str_state.c_str());
		i3UIStaticText* ui_score = (i3UIStaticText*)frame->FindChildByName( str_score.c_str());

		dui::ui_fader * pFader = new dui::ui_fader( ui_state);
		MEMDUMP_NEW( pFader, sizeof( dui::ui_fader));
		dui::ui_fade_ptr f_state_ptr( pFader);

		pFader = new dui::ui_fader( ui_score);
		MEMDUMP_NEW( pFader, sizeof( dui::ui_fader));
		dui::ui_fade_ptr f_score_ptr( pFader);
  
		m_special_ui[i]->set_slider( dui::S_SLIDE_D_UP, up_slide_ptr);
		m_special_ui[i]->set_slider( dui::S_SLIDE_D_ADD, add_slide_ptr);	//<-- 다른 콜백을 등록
		m_special_ui[i]->set_fader( dui::S_CONTROL_D_STATE, f_state_ptr);  
		m_special_ui[i]->set_fader( dui::S_CONTROL_D_SCORE, f_score_ptr);  
	}    
}

bool CUIHudDomi_SpecialScore::OnEntranceStart( void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIHUDDominationBase::OnEntranceStart( pArg1, pArg2) == false) return false;
	
	for( size_t i = 0; i < dui::special_max; ++i)
	{
		i3UIFrameWnd* frame = m_special_ui[i]->get_frame();
		frame->EnableCtrl( false);
		
		m_free_list.push_back( m_special_ui[i]);
	}

	return true;
}

void CUIHudDomi_SpecialScore::OnUpdate( REAL32 rDeltaSec)
{
	for( size_t i = 0; i < m_active_list.size(); ++i)  
		m_active_list[i]->update( rDeltaSec);  
}

void CUIHudDomi_SpecialScore::cb_slider_up( void* ui_actor, void* data)  
{
	dui::special_score* special_ui = reinterpret_cast<dui::special_score*>(data);  

	
	dui::ui_slider_ptr up_slide_ptr = special_ui->get_slider( dui::S_SLIDE_D_UP);  
	up_slide_ptr->set_enable( false);


	dui::ui_fade_ptr state_ptr = special_ui->get_fader( dui::S_CONTROL_D_STATE);
	dui::ui_fade_ptr score_ptr = special_ui->get_fader( dui::S_CONTROL_D_SCORE);

	dui::ui_slider_ptr add_slide_ptr = special_ui->get_slider( dui::S_SLIDE_D_ADD);  

	if( special_ui->me)
	{
		//fade 시작
		state_ptr->start( dui::FADE_OUT, 0.9f);
		score_ptr->start( dui::FADE_OUT, 0.9f);

		state_ptr->set_enable( true);
		score_ptr->set_enable( true);	
	

		//add 슬라이드 시작	
		i3UIFrameWnd* frame = (i3UIFrameWnd*)add_slide_ptr->get_control();
		VEC3D* start = frame->getPos();

		VEC3D end;
		INT32 screen_width = i3UI::getManager()->getWidth();
		INT32 screen_height = i3UI::getManager()->getHeight();  

		i3Vector::Set( &end, (screen_width*0.6f), (screen_height*0.05f), 0.f);  

		add_slide_ptr->set_target( start, &end);
		add_slide_ptr->start( dui::SLIDE_IN, 0.5f, true, 1.f);
		add_slide_ptr->set_enable( true);
	}
	else
	{
		state_ptr->set_enable( false);
		score_ptr->set_enable( false);

		i3::vu::remove( m_active_list, special_ui);
		m_free_list.push_back( special_ui);

		special_ui->get_frame()->EnableCtrl( false);
	}
}

void CUIHudDomi_SpecialScore::cb_slider_add( void* ui_actor, void* data)
{
	//다 사용함.
	dui::special_score* special_ui = reinterpret_cast<dui::special_score*>(data);

	dui::ui_slider_ptr add_slide_ptr = special_ui->get_slider( dui::S_SLIDE_D_ADD);
	add_slide_ptr->set_enable( false);

	dui::ui_fade_ptr state_ptr = special_ui->get_fader( dui::S_CONTROL_D_STATE);
	dui::ui_fade_ptr score_ptr = special_ui->get_fader( dui::S_CONTROL_D_SCORE);

	state_ptr->set_enable( false);
	score_ptr->set_enable( false);

	i3::vu::remove( m_active_list, special_ui);
	m_free_list.push_back( special_ui);

	special_ui->get_frame()->EnableCtrl( false);

	if( special_ui->me)
	{
		domi_state_info::i()->total_score[BattleSlotContext::i()->getMySlotIdx()/2] += special_ui->score;
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);

		//debug
		//domi_state_info::i()->ui_spe_score += special_ui->score;
	}
}

bool CUIHudDomi_SpecialScore::SetHudEvent( INT32 nEventNum, INT32 arg)
{
	switch( nEventNum)
	{
	case dui::SPECIAL_E_IS_WORKING: return !m_active_list.empty();

	case dui::SPECIAL_E_START:
		{
			//reset 한다는 생각으로
			dui::special_score* available;
			if( m_free_list.empty())
			{
				available = *m_active_list.begin();
				
				if( available->me)
				{
					domi_state_info::i()->total_score[BattleSlotContext::i()->getMySlotIdx()/2] += available->score;
					UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);

					//debug
					//domi_state_info::i()->ui_spe_score += available->score;
				}
			}
			else
			{
				available = *m_free_list.begin();
				i3::vu::remove( m_free_list, available);  

				m_active_list.push_back( available);  
			}




			PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK* info = reinterpret_cast<PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK*>(arg);			
						
			CGameCharaBase* dino = g_pCharaManager->getCharaByNetIdx( info->_i8DinoSlot, true);
			if( dino)
			{
				available->me = (info->_i8UserSlot == BattleSlotContext::i()->getMySlotIdx());
				available->score = info->_i16Score;

				dino->getBoneContext()->TransformWorldSpace( 0.016f);
				VEC3D* pos = dino->GetPos();

				COLOR col;
				i3::rc_wstring state;
				i3::stack_wstring score = L"+";

				get_state( info->_i8EffectID, state, col);

				wchar_t buf[64] = {0,};
				_itow( info->_i16Score, buf, 10);
				score += buf;

				if( available->me == false) score.clear();
	
				available->get_frame()->EnableCtrl( true);
				available->start( state, score, &col, pos);
			}
		}
		break;
	}  
  
	return false;  
}

void CUIHudDomi_SpecialScore::get_state( INT8 effect, i3::rc_wstring& out, COLOR& col)
{
	col.a = 255;  
	switch( effect)  
	{  
	case WEAPON_SPECIAL_EFFECT_FATALBLOW:
		out = GAME_RCSTRING( "STR_DOMI_BATTLE_5");
		col.r = 250;  
		col.g = 20;  
		col.b = 5;  
		break;

	case WEAPON_SPECIAL_EFFECT_STIFFEN:
		out = GAME_RCSTRING( "STR_DOMI_BATTLE_2");
		col.r = 70;
		col.g = 100;
		col.b = 250;
		break;

	case WEAPON_SPECIAL_EFFECT_BLIND:
		out = GAME_RCSTRING( "STR_DOMI_BATTLE_6");
		col.r = 50;
		col.g = 246;
		col.b = 150;
		break;

	case WEAPON_SPECIAL_EFFECT_SLOW:
		out = GAME_RCSTRING( "STR_DOMI_BATTLE_7");
		col.r = 190;
		col.g = 246;
		col.b = 32;
		break;

	case WEAPON_SPECIAL_EFFECT_KNOCKDOWN:
		out = GAME_RCSTRING( "STR_DOMI_BATTLE_4");
		col.r = 200;
		col.g = 255;
		col.b = 32;  
		break;

	default:
		out = L"error";
		col.r = 255;
		col.g = 255;
		col.b = 255;
		break;
	}
} 

  