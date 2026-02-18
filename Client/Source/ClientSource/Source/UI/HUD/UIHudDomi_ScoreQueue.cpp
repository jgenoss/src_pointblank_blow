#include "pch.h"
#include "UIHudDomi_ScoreQueue.h"

#include "UIHUD_Domination_Manager.h"

#include "BattleSlotContext.h"


namespace DominationScoreQueue
{
	static COLOR col_white = COLOR( 255, 255, 255, 255);
	static COLOR col[TEXT_MAX] = 
	{
		COLOR(   0, 255,   0, 255),
		COLOR(   0, 255,   0, 255),
		COLOR( 255, 255, 255, 255),
		COLOR( 255, 255,   0, 255),
		COLOR( 124, 150, 204, 255),
	};
	static INT32 font_size[TEXT_MAX] = { 10, 10, 15, 8, 15};
	static char text_format[TEXT_MAX][64] = 
	{
		"Assist Kill +%d",
		"Assist SP +%d",
		"Kill +%d",
		"(HeadShot +%d)",
		"SP +%d",
	};

	void score_info::update( REAL32 fDeltaTime)
	{ 
		if( m_ui->isEnable())
		{
			m_time += fDeltaTime;
			if( m_time >= 1.f)
			{
				//move to
				m_pScene->move_to_ctrl( this);

				m_ui->EnableCtrl( false);
				m_time = 0.f;

				m_data.combo = 0;
				m_data.score = 0;
				m_data.sp = 0;
				m_data.calculated_score = 0;
			}
		}
	}

	bool score_control::update( REAL32 fDeltaTime)
	{
		m_fader->OnUpdate( fDeltaTime);
		m_slider->OnUpdate( fDeltaTime);

		return m_slider->get_period() <= m_slider->get_progress();
	}
};

 












I3_CLASS_INSTANCE(UIHUDDomi_ScoreQueue);

UIHUDDomi_ScoreQueue::UIHUDDomi_ScoreQueue() :m_frame(0)
{
	m_score_info_list.reserve( dsq::TEXT_MAX);
	m_score_ctrl_list.reserve( UI_TEXT_MAX);
}

UIHUDDomi_ScoreQueue::~UIHUDDomi_ScoreQueue()
{
	m_score_info_list.clear();
	m_score_ctrl_list.clear();
}

void UIHUDDomi_ScoreQueue::OnCreate( i3GameNode* parent)
{
	UIHUDDominationBase::OnCreate(parent);

	AddScene( "Scene/Domination/PBRe_Domi_ScoreQueueDummy.i3UIs", nullptr, true, false);
}

void UIHUDDomi_ScoreQueue::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDDominationBase::_InitializeAtLoad( pScene);

	if( pScene != nullptr)
	{
		m_frame = (i3UIFrameWnd*)pScene->FindChildByName( "i3UIFrameWnd");
		
		m_free.reserve( UI_TEXT_MAX);
		i3UITemplate_StaticText* temp = (i3UITemplate_StaticText*)i3UI::FindTemplateByName("StaticText");
		for( INT32 i = 0; i < UI_TEXT_MAX; ++i)
		{
			i3UIStaticText* ui = i3UIStaticText::new_object();
			ui->SetTemplate( temp);
			m_frame->AddControl( ui);	
			I3_MUST_RELEASE(ui);
			
			ui->setSize( 200.f, 60.f);
			ui->SetTextAlign( I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);

			m_free.push_back( ui);
		} 

		REAL32 width = (REAL32)i3UI::getManager()->getWidth();
		REAL32 height = (REAL32)i3UI::getManager()->getHeight();   
		

		REAL32 y = (height * 0.3f);

		for( INT32 i = 0; i < dsq::TEXT_MAX; ++i)
		{
			m_main_texts[i] = _GetTextUI();

			REAL32 x = (width - m_main_texts[i]->getWidth())/2;
			y -= (_PointToHeight( dsq::font_size[i]));

			if( i == dsq::TEXT_KILL) y += 6;
			if( i == dsq::TEXT_HEAD_SHOT) y += 2;

			m_main_texts[i]->setPos( x, y);
			m_main_texts[i]->SetTextColor( &dsq::col[i]);

			m_main_texts[i]->SetFont( m_main_texts[i]->getFontName(), dsq::font_size[i], FW_SEMIBOLD, false);
			m_main_texts[i]->SetTextShadowType( I3UI_TEXT_SHADOW_THICK);
			
			
			
			//char buf[64] = {0,};
			//StringCchPrintf( buf, 64, dsq::text_format[i], i);
			//m_main_texts[i]->SetText( buf);

			dsq::score_info * pScoreInfo = new dsq::score_info( this, m_main_texts[i]);
			MEMDUMP_NEW( pScoreInfo, sizeof( dsq::score_info));
			dsq::score_info_ptr	obj = dsq::score_info_ptr( pScoreInfo);
			obj->m_type = static_cast<dsq::TEXT_TYPE>(i);
			m_score_info_list.push_back( obj);
		}
	}
}

bool UIHUDDomi_ScoreQueue::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIHUDDominationBase::OnEntranceStart( pArg1, pArg2) == false) return false;

	for( size_t i = 0; i < m_free.size(); ++i)
		m_free[i]->EnableCtrl( false);

	for( size_t i = 0; i < m_score_info_list.size(); ++i)
		m_score_info_list[i]->m_ui->EnableCtrl( false);

	return true;
}

void UIHUDDomi_ScoreQueue::OnUpdate( REAL32 rDeltaSec)
{
	UIHUDDominationBase::OnUpdate( rDeltaSec);

	for( size_t i = 0; i < m_score_info_list.size(); ++i)
		m_score_info_list[i]->update( rDeltaSec);


	i3::vector<dsq::score_control_ptr>::iterator itr = m_score_ctrl_list.begin();
	while( itr != m_score_ctrl_list.end())
	{
		if( (*itr)->update( rDeltaSec))
		{
			(*itr)->m_slider->get_control()->EnableCtrl( false);
			m_free.push_back( (i3UIStaticText*)(*itr)->m_slider->get_control());

			itr = m_score_ctrl_list.erase( itr);
			continue;
		}
		++itr;
	}
}

bool UIHUDDomi_ScoreQueue::SetHudEvent( INT32 nEventNum, INT32 arg /*= nullptr*/)
{
	bool is_ctrl_working = !m_score_ctrl_list.empty();

	if( nEventNum == dsq::SCORE_E_IS_WORKING_SCORE)
		return is_ctrl_working || m_score_info_list[dsq::TEXT_ASSIST_KILL]->m_ui->isEnable() || m_score_info_list[dsq::TEXT_KILL]->m_ui->isEnable();

	if( nEventNum == dsq::SCORE_E_IS_WORKING_SP)
		return is_ctrl_working || m_score_info_list[dsq::TEXT_ASSIST_SP]->m_ui->isEnable() || m_score_info_list[dsq::TEXT_SP]->m_ui->isEnable();

	GAMEINFO_DINO_DEATH* death = reinterpret_cast<GAMEINFO_DINO_DEATH*>(arg);
	INT32 arr_pos = BattleSlotContext::i()->getMySlotIdx()/2;
	
	//I3TRACE("recieve clac score %d\n", death->combo*death->score[arr_pos]);

	char buf[64] = {0,};
	dsq::score_info* ui_obj = nullptr;
	dsq::TEXT_TYPE type = dsq::TEXT_MAX;
	switch( nEventNum)
	{
	case dsq::SCORE_E_KILL_DINO:
		{			
			type = dsq::TEXT_KILL;
		}
		break;

	case dsq::SCORE_E_KILL_SP:
		{		
			type = dsq::TEXT_SP;
		}
		break;

	case dsq::SCORE_E_ASSIST_DINO:
		{	
			type = dsq::TEXT_ASSIST_KILL;
		}
		break;

	case dsq::SCORE_E_ASSIST_SP:
		{
			type = dsq::TEXT_ASSIST_SP;
		}
		break;
	}

	if( type >= dsq::TEXT_ASSIST_KILL && type < dsq::TEXT_MAX)
	{
		ui_obj = m_score_info_list[type].get();
		dsq::score_info* head_shot_ui = m_score_info_list[dsq::TEXT_HEAD_SHOT].get();
		if( ui_obj->m_ui->isEnable())
		{
			if( ui_obj->m_time > 0.01f)
			{
				//move to
				move_to_ctrl( ui_obj);

				if( type == dsq::TEXT_KILL && head_shot_ui->m_ui->isEnable())
				{
					move_to_ctrl( head_shot_ui);
					head_shot_ui->m_ui->EnableCtrl( death->bHeadShot);
				}

				//reset
				ui_obj->reset_score( dsq::score_data( death->sp[arr_pos], death->score[arr_pos], death->combo));
				//I3TRACE("score reset!!\n");
			}
			else
			{//merge
				ui_obj->merge_score( dsq::score_data( death->sp[arr_pos], death->score[arr_pos], death->combo));
				//I3TRACE("score merge!!!!\n");
			}	
		}
		else
		{//start
			ui_obj->m_ui->EnableCtrl( true);
			ui_obj->reset_score( dsq::score_data( death->sp[arr_pos], death->score[arr_pos], death->combo));	
		}

		if( type == dsq::TEXT_KILL || type == dsq::TEXT_ASSIST_KILL)
			StringCchPrintf( buf, 64, dsq::text_format[type], ui_obj->m_data.score);

		if( type == dsq::TEXT_SP || type == dsq::TEXT_ASSIST_SP)
			StringCchPrintf( buf, 64, dsq::text_format[type], ui_obj->m_data.sp);

		ui_obj->m_ui->SetText( buf);
		ui_obj->m_time = 0.f;

		if( type == dsq::TEXT_KILL && death->bHeadShot)
		{
			head_shot_ui->m_ui->EnableCtrl( true);
			head_shot_ui->reset_score( dsq::score_data( death->sp[arr_pos], death->score[arr_pos], death->combo));

			StringCchPrintf( buf, 64, dsq::text_format[dsq::TEXT_HEAD_SHOT], (INT32)(ui_obj->m_data.calculated_score*0.2f));
			head_shot_ui->m_ui->SetText( buf);
			head_shot_ui->m_time = 0.f;

			//I3TRACE(" death->bHeadShot %d\n", death->bHeadShot);
		}
	}

	return false;
}

void UIHUDDomi_ScoreQueue::move_to_ctrl( dsq::score_info* _score_info)
{
	CUIHudDomi_Combo* ui_combo = (CUIHudDomi_Combo*)tinst<UIHUDDominationMgr>()->GetHUD( HUD_DOMI_COMBO);
	VEC3D* frame_pos = ui_combo->m_pFrameWnd->getPos();

	INT32 screen_width = i3UI::getManager()->getWidth();
	INT32 screen_height = i3UI::getManager()->getHeight();

	i3UIStaticText* move_ui = _GetTextUI();
	if( move_ui == nullptr) I3ASSERT_0;
	move_ui->EnableCtrl( true);

	dsq::score_control * pScoreCtrl = new dsq::score_control();
	MEMDUMP_NEW( pScoreCtrl, sizeof(dsq::score_control));
	dsq::score_control_ptr ctrl( pScoreCtrl);
	ctrl->m_data = _score_info->m_data;

	dui::ui_slider * pSlider = new dui::ui_slider(move_ui);
	MEMDUMP_NEW( pSlider, sizeof(dui::ui_slider));
	ctrl->m_slider = dui::ui_slider_ptr( pSlider);

	dui::ui_fader * pFader = new dui::ui_fader(move_ui);
	MEMDUMP_NEW( pFader, sizeof( dui::ui_fader));
	ctrl->m_fader = dui::ui_fade_ptr( pFader);

	dui::call_back_ptr cb;
	VEC3D start, end;
	i3Vector::Copy( &start, _score_info->m_ui->getPos());

	switch( _score_info->m_type)
	{
	case dsq::TEXT_KILL:
		if( _score_info->m_data.combo == 1)
		{
			dui::member_call_back<UIHUDDomi_ScoreQueue> * pCb = new dui::member_call_back<UIHUDDomi_ScoreQueue>( this,
				&UIHUDDomi_ScoreQueue::cb_add_total_score, (void*)ctrl.get());
			MEMDUMP_NEW( pCb, sizeof( dui::member_call_back<UIHUDDomi_ScoreQueue>));
			cb = dui::call_back_ptr( pCb);
			i3Vector::Set( &end, (screen_width*0.6f), (screen_height*0.05f), 0.f);	
		}
		else
		{
			dui::member_call_back<UIHUDDomi_ScoreQueue> * pCb = new dui::member_call_back<UIHUDDomi_ScoreQueue>( this,
				&UIHUDDomi_ScoreQueue::cb_add_combo_score, (void*)ctrl.get());
			MEMDUMP_NEW( pCb, sizeof( dui::member_call_back<UIHUDDomi_ScoreQueue>));
			cb = dui::call_back_ptr( pCb);
			i3Vector::Copy( &end, frame_pos);
		}
		break;

	case dsq::TEXT_ASSIST_KILL:
		{
			dui::member_call_back<UIHUDDomi_ScoreQueue> * pCb = new dui::member_call_back<UIHUDDomi_ScoreQueue>( this,
				&UIHUDDomi_ScoreQueue::cb_add_total_score, (void*)ctrl.get());
			MEMDUMP_NEW( pCb, sizeof( dui::member_call_back<UIHUDDomi_ScoreQueue>));
			cb = dui::call_back_ptr( pCb);
			i3Vector::Set( &end, (screen_width*0.6f), (screen_height*0.05f), 0.f);	
		}
		break;

	case dsq::TEXT_SP:
	case dsq::TEXT_ASSIST_SP:
		{
			dui::member_call_back<UIHUDDomi_ScoreQueue> * pCb = new dui::member_call_back<UIHUDDomi_ScoreQueue>( this,
				&UIHUDDomi_ScoreQueue::cb_add_sp, (void*)ctrl.get());
			MEMDUMP_NEW( pCb, sizeof( dui::member_call_back<UIHUDDomi_ScoreQueue>));
			cb = dui::call_back_ptr( pCb);
			i3Vector::Set( &end, (screen_width*0.5f), (screen_height*0.85f), 0.f);	
		}
		break;

	case dsq::TEXT_HEAD_SHOT:
		if( _score_info->m_data.combo == 1)
		{
			i3Vector::Set( &end, (screen_width*0.6f), (screen_height*0.05f)-_PointToHeight( dsq::font_size[dsq::TEXT_HEAD_SHOT]), 0.f);	
		}
		else
		{
			i3Vector::Copy( &end, frame_pos);
			i3Vector::AddY( &end, (REAL32)-_PointToHeight( dsq::font_size[dsq::TEXT_HEAD_SHOT]));
		}
		//call back은 등록 안함.
		break;
	}


	ctrl->m_slider->set_call_back( cb);
	ctrl->m_slider->set_target( &start, &end);
	ctrl->m_slider->start( dui::SLIDE_IN, 0.5f, true, 1.f);
	ctrl->m_slider->set_enable( true);

	ctrl->m_fader->start( dui::FADE_OUT, 1.f);
	ctrl->m_fader->set_enable( true);


	move_ui->SetTextColor( _score_info->m_ui->getTextColor());
	move_ui->SetFont( _score_info->m_ui->getFontName(), dsq::font_size[_score_info->m_type], FW_NORMAL, false);
	move_ui->SetText( _score_info->m_ui->getText());
	this->m_score_ctrl_list.push_back( ctrl);
}

void UIHUDDomi_ScoreQueue::cb_add_total_score( void* p1, void* p2)
{
	dsq::score_control* ctrl = (dsq::score_control*)p2;
	if( ctrl)
	{
		INT32 arr_pos = BattleSlotContext::i()->getMySlotIdx()/2;
		domi_state_info* info = domi_state_info::i();
		info->total_score[arr_pos] += ctrl->m_data.calculated_score;

		//I3TRACE("add total clac score %d\n", ctrl->m_data.calculated_score);

		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);
	}
}

void UIHUDDomi_ScoreQueue::cb_add_combo_score( void* p1, void* p2)
{
	dsq::score_control* ctrl = (dsq::score_control*)p2;

	if( ctrl)
	{
		dui::combo_info info = { ctrl->m_data.combo, ctrl->m_data.score, ctrl->m_data.calculated_score};

		//I3TRACE("send combo clac score %d\n", ctrl->m_data.calculated_score);

		UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_COMBO, dui::COMBO_E_RECEIVE, reinterpret_cast<INT32>(&info));
	}
}

void UIHUDDomi_ScoreQueue::cb_add_sp( void* p1, void* p2)
{
	dsq::score_control* ctrl = (dsq::score_control*)p2;

	if( ctrl)
	{
		INT32 arr_pos = BattleSlotContext::i()->getMySlotIdx()/2;
		domi_state_info* info = domi_state_info::i();
		info->sp.sp[arr_pos] += ctrl->m_data.sp;

		static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL))->SetSPGauge(info->sp.sp[arr_pos]);
	}
}

i3UIStaticText* UIHUDDomi_ScoreQueue::_GetTextUI()
{
	i3UIStaticText* ui = nullptr;
	if( !m_free.empty())
	{
		ui = *m_free.begin();
		m_free.erase( m_free.begin());
	}
	else
	{
		i3UITemplate_StaticText* temp = (i3UITemplate_StaticText*)i3UI::FindTemplateByName("StaticText");	
		ui = i3UIStaticText::new_object();
		ui->SetTemplate( temp);
		m_frame->AddControl( ui);	
		I3_MUST_RELEASE(ui);

		ui->setSize( 200.f, 100.f);
		ui->SetTextAlign( I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);
		ui->setColor( &dsq::col_white);

		INT32 font_size = _HeightToPoint( ui->getFontSize()) + 5;
		ui->SetFont( ui->getFontName(), /*static_cast<INT32>*/(font_size), FW_NORMAL, false);
	}

	return ui;
}