#include "pch.h"

#include "UIUtil.h"
#include "UIMainFrame.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/format_string.h"
namespace fly
{
	void open_fly(); void close_fly(); void update_fly(REAL32 rDeltaSeconds);

	i3UIFrameWnd* main_wnd, *bg_wnd;
	void _InitializeAtLoad( i3UIScene * pScene);

	i3UIStaticText *score_text, *title_text; 
	UINT32 high_score = 0, score = 0, life = 3, stage_num = 1;
}

namespace fly
{
	enum { stage_y = 3, stage_x = 10, stage_xy = stage_y*stage_x, };
	enum { max_level = 6, };
	enum { disable_idx = -1, };

	REAL32 player_speed = 2.f, player_missile_speed = 2.5f, enemy_missile_speed = 2.f, enemy_speed = 0.5f;
	REAL32 enemy_fire_speed = 1.f;
	REAL32 player_missile_fire_delay = 0.f, enemy_missile_fire_delay = 0.f;
	bool direction = false, go_bottom = false; //false :left, true : right
	
	struct _obj
	{
	public:
		void init(i3UIScene * pScene, const char* name);
		virtual void update(REAL32 rDeltaSeconds) = 0;

	public:
		i3UICaptionControl* object;
		VEC2D* object_size;
		VEC2D area;
	};

	enum { max_enemy = 15, max_enemy_level = 6, };
	struct _enemy_obj : public _obj
	{
		virtual void update(REAL32 rDeltaSeconds) override;
		INT32 level;
	} enemy[max_enemy];

	struct _player_obj : public _obj
	{
		virtual void update(REAL32 rDeltaSeconds) override;
	} player;

	enum { max_player_missile = 3, };
	struct _player_missile_obj : public _obj
	{
		virtual void update(REAL32 rDeltaSeconds) override;
	} player_missile[max_player_missile];

	enum { max_enemy_missile = 3, };
	struct _enemy_missile_obj : public _obj
	{
		virtual void update(REAL32 rDeltaSeconds) override;
	} enemy_missile[max_enemy_missile];
}


namespace fly_util
{
	bool is_in_pt(REAL32 x1, REAL32 y1, REAL32 u1, REAL32 v1, REAL32 pt_x, REAL32 pt_y);
	bool is_in_rect(REAL32 x1, REAL32 y1, REAL32 u1, REAL32 v1, REAL32 x2, REAL32 y2, REAL32 u2, REAL32 v2);
	bool is_collide(const fly::_obj & lhs, const fly::_obj & rhs);
	void resize_bg(i3UIFrameWnd* bg);
}

namespace fly_detail
{
	void update_null_impl(REAL32 rDeltaSeconds) {}
	void update_ready_impl(REAL32 rDeltaSeconds);
	void update_normal_impl(REAL32 rDeltaSeconds);
	void (*pf_update)(REAL32 rDeltaSeconds);

	void init_stage(bool is_next_stage);
	void hide_object();
	
	void update_ready_text();
	void update_normal_text();

	INT32 find_fire_enemy_idx();
	INT32 find_fire_player_missile_idx();

	void next_stage();
	void end_game();
}


/***********************//***********************/
void fly::_InitializeAtLoad( i3UIScene * pScene)
{
	fly::main_wnd = static_cast<i3UIFrameWnd*>( pScene->FindChildByName( "main") );
	fly::bg_wnd = static_cast<i3UIFrameWnd*>( pScene->FindChildByName( "bg") );
	fly::title_text = static_cast<i3UIStaticText*>( pScene->FindChildByName( "i3UIStaticText3") );
	fly::score_text = static_cast<i3UIStaticText*>( pScene->FindChildByName( "i3UIStaticText") );
	fly_util::resize_bg( fly::bg_wnd );

	i3UIFrameWnd* area = static_cast<i3UIFrameWnd*>( pScene->FindChildByName( "i3UIFrameWnd2") );
	VEC2D* size = area->getSize();

	i3::stack_string str_stack;

	for(INT32 i=0;i<fly::max_enemy;++i)
	{
		i3::string str = "i3UIButtonImageSet";
		i3::itoa(i, str_stack);
		str += str_stack;

		fly::enemy[i].init(pScene, str.c_str());
		fly::enemy[i].area = *size;
		fly::enemy[i].level = 0;
	}

	for(INT32 i=0;i<fly::max_enemy_missile;++i)
	{
		i3::string str = "i3UIButtonImageSet";
		i3::itoa(i + 15, str_stack);
		str += str_stack;

		fly::enemy_missile[i].init(pScene, str.c_str());
		fly::enemy_missile[i].area = *size;
	}

	for(INT32 i=0;i<fly::max_player_missile;++i)
	{
		i3::string str = "i3UIImageBox";
		i3::itoa( i + 1, str_stack);
		str += str_stack;

		fly::player_missile[i].init(pScene, str.c_str());
		fly::player_missile[i].area = *size;
	}

	fly::player.init(pScene, "i3UIImageBox0");
	fly::player.area = *size;
}

void fly::open_fly()
{
	fly::main_wnd->EnableCtrl(true); fly::bg_wnd->EnableCtrl(true);
	fly::score = 0;
	fly_detail::update_ready_text();
	fly_detail::hide_object();
	fly_detail::pf_update = &fly_detail::update_ready_impl;
}
void fly::close_fly()
{
	fly::main_wnd->EnableCtrl(false); fly::bg_wnd->EnableCtrl(false);
}

void fly_detail::update_ready_impl(REAL32 rDeltaSeconds)
{
	if( GameUI::IsKeyboardStroked(I3I_KEY_SPACE) == true )
	{
		fly_detail::init_stage(false);
		fly_detail::update_normal_text();
		fly_detail::pf_update = &fly_detail::update_normal_impl;
	}
	if( GameUI::IsKeyboardStroked(I3I_KEY_ESC) == true )
	{
		GameUI::CloseAllMsgBox();
		g_pFramework->GetUIMainframe()->CloseAllPopup();
		fly::close_fly();
	}
}

void fly_detail::update_normal_impl(REAL32 rDeltaSeconds)
{
	fly::player_missile_fire_delay += rDeltaSeconds;
	fly::enemy_missile_fire_delay += rDeltaSeconds;
	
	if( fly::go_bottom == true )
	{
		for(INT32 i=0;i<fly::max_enemy;++i)
		{
			if( fly::enemy[i].object->isEnable() == true )
			{
				VEC3D* pos = fly::enemy[i].object->getPos();
				fly::enemy[i].object->setPos( pos->x, pos->y + 9.5f);
			}
		}
		fly::go_bottom = false; fly::direction = !fly::direction; return;
	}

	INT32 live_enemy_cnt = 0;
	for(INT32 i=0;i<fly::max_enemy;++i)
	{
		if( fly::enemy[i].object->isEnable() == true )
		{
			if( fly::go_bottom == true )
			{
				VEC3D* pos = fly::enemy[i].object->getPos();
				fly::enemy[i].object->setPos( pos->x, pos->y + 19.f);
			}
			++live_enemy_cnt;
		}
	}
	if( live_enemy_cnt == 0 ) { next_stage(); return; }

	for(INT32 i=0;i<fly::max_enemy;++i)
		fly::enemy[i].update(rDeltaSeconds);
	for(INT32 i=0;i<fly::max_enemy_missile;++i)
		fly::enemy_missile[i].update(rDeltaSeconds);
	for(INT32 i=0;i<fly::max_player_missile;++i)
		fly::player_missile[i].update(rDeltaSeconds);
	fly::player.update(rDeltaSeconds);

	if( GameUI::IsKeyboardStroked(I3I_KEY_ESC) == true )
	{
		GameUI::CloseAllMsgBox();
		g_pFramework->GetUIMainframe()->CloseAllPopup();

		fly_detail::update_ready_text();
		fly_detail::hide_object();
		fly_detail::pf_update = &fly_detail::update_ready_impl;
	}
}

void fly::update_fly(REAL32 rDeltaSeconds)
{
	if( fly::main_wnd->isEnable() == false ) return;
	(*fly_detail::pf_update)(rDeltaSeconds);
}


//-----------------------------////-----------------------------//
//fly::_obj
void fly::_obj::init(i3UIScene * pScene, const char* name)
{
	i3GameNode* node = pScene->FindChildByName( name );
	if( !node)
	{
		I3PRINTLOG(I3LOG_FATAL, "[%s] i3Uie 파일 확인", name);
	}
	object = static_cast<i3UICaptionControl*>(node);
	object_size = object->getSize();
}
//fly::_enemy_obj
void fly::_enemy_obj::update(REAL32 rDeltaSeconds)
{
	if( object->isEnable() == false ) return;

	VEC3D* pos = object->getPos();
	if( pos->y >= 285.f ) { fly_detail::end_game(); return; }

	if( fly::direction == true) //right
	{
		if( pos->x == area.x - object_size->x )
		{ fly::go_bottom = true; return; }
		object->setPos( pos->x + fly::enemy_speed, pos->y);

		pos = object->getPos();
		if( pos->x + object_size->x > area.x ) object->setPos( area.x - object_size->x, pos->y);
	}
	else
	{
		if( pos->x == 0.f ) { fly::go_bottom = true; return; }
		object->setPos( pos->x - fly::enemy_speed, pos->y);

		pos = object->getPos();
		if( pos->x < 0.f) object->setPos( 0.f, pos->y);
	}
}
//fly::_player_obj
void fly::_player_obj::update(REAL32 rDeltaSeconds)
{
	if( object->isEnable() == false ) return;

	if( GameUI::IsKeyboardPressed(I3I_KEY_LEFT) == true )
	{
		VEC3D* pos = object->getPos();
		object->setPos( pos->x - fly::player_speed, pos->y);
		pos = object->getPos();
		if( pos->x < 0.f ) object->setPos( 0.f, pos->y);
	}
	else if(GameUI::IsKeyboardPressed(I3I_KEY_RIGHT) == true)
	{
		VEC3D* pos = object->getPos();
		object->setPos( pos->x + fly::player_speed, pos->y);
		pos = object->getPos();
		if( pos->x + object_size->x > area.x ) object->setPos( area.x - object_size->x, pos->y);
	}

	if( GameUI::IsKeyboardPressed(I3I_KEY_SPACE) == true )
	{
		if( fly::player_missile_fire_delay > 0.5f )
		{
			INT32 fire_missile_idx = fly_detail::find_fire_player_missile_idx();

			if( fire_missile_idx != fly::disable_idx )
			{
				VEC3D* pos = object->getPos();
				fly::player_missile[fire_missile_idx].object->setPos( pos->x + (object_size->x / 2), pos->y);
				fly::player_missile[fire_missile_idx].object->EnableCtrl(true);
			}

			fly::player_missile_fire_delay = 0.f;
		}
	}
}
//fly::_player_missile_obj
void fly::_player_missile_obj::update(REAL32 rDeltaSeconds)
{
	if( object->isEnable() == false ) return;

	VEC3D* pos = object->getPos();
	if( pos->y == 0.f ) { object->EnableCtrl(false); return; }
	object->setPos( pos->x, pos->y - fly::player_missile_speed);
	pos = object->getPos();
	if( pos->y < 0.f ) { object->setPos( pos->x, 0.f); return; }

	for(INT32 i=0;i<fly::max_enemy;++i)
	{
		if( fly::enemy[i].object->isEnable() == true )
		{
			if( fly_util::is_collide(fly::enemy[i], *this) == true ) 
			{
				object->EnableCtrl(false);

				--fly::enemy[i].level;
				if( fly::enemy[i].level >= 0 )
					((i3UIButtonImageSet*)fly::enemy[i].object)->SetShapeIdx(fly::enemy[i].level);
				else fly::enemy[i].object->EnableCtrl(false);
				fly::score += 10;
				fly_detail::update_normal_text();
			}
		}
	}
}

//fly::_enemy_missile_obj
void fly::_enemy_missile_obj::update(REAL32 rDeltaSeconds)
{
	if( object->isEnable() == false )
	{
		if( fly::enemy_missile_fire_delay > fly::enemy_fire_speed )
		{
			INT32 enemy_idx = fly_detail::find_fire_enemy_idx();
			if( enemy_idx != fly::disable_idx)
			{
				VEC3D* pos = fly::enemy[enemy_idx].object->getPos();
				object->setPos( pos->x + (object_size->x / 2), pos->y);
				object->EnableCtrl(true);
			}
			fly::enemy_missile_fire_delay = 0.f;
		}
		return;
	}

	VEC3D* pos = object->getPos();
	if( pos->y == area.y - object_size->y ) { object->EnableCtrl(false); return; }

	object->setPos( pos->x, pos->y + fly::enemy_missile_speed);

	pos = object->getPos();
	if( pos->y + object_size->y > area.y ) { object->setPos( pos->x, area.y - object_size->y); return; }

	if( fly_util::is_collide(fly::player, *this) == true )
	{
		if( object->isEnable() == true )
		{
			object->EnableCtrl(false);

			--fly::life;
			if( fly::life == 0 ) fly_detail::end_game();
			else fly_detail::update_normal_text();
		}
	}
}

//-----------------------------////-----------------------------//
//fly_util
void fly_util::resize_bg(i3UIFrameWnd* bg)
{
	enum { Add_Width = 10 };
	bg->setSize( (REAL32) g_pViewer->GetViewWidth(), (REAL32)(g_pViewer->GetViewHeight() + Add_Width) );
	bg->setPos( -((REAL32)i3UI::getManager()->getDisplayOffsetX() * i3UI::getManager()->getScreenStretchRatioX()), 0.f);

	i3UIImageBox * pImgBox = (i3UIImageBox*) bg->FindChildByName( "i3UIImageBox");
	if( pImgBox != nullptr) pImgBox->setSize( (REAL32) g_pViewer->GetViewWidth(), (REAL32)(g_pViewer->GetViewHeight() + Add_Width) );
}
bool fly_util::is_in_pt(REAL32 x, REAL32 y, REAL32 w, REAL32 h, REAL32 pt_x, REAL32 pt_y)
{
	return (x <= pt_x && pt_x < (x + w)) && (y <= pt_y && pt_y < (y + h) );
}
bool fly_util::is_in_rect(REAL32 x1, REAL32 y1, REAL32 u1, REAL32 v1, REAL32 x2, REAL32 y2, REAL32 u2, REAL32 v2)
{
	if( fly_util::is_in_pt(x1, y1, u1, v1, x2, y2) == true ) return true;
	if( fly_util::is_in_pt(x1, y1, u1, v1, x2, y2+v2) == true ) return true;
	if( fly_util::is_in_pt(x1, y1, u1, v1, x2+u2, y2) == true ) return true;
	if( fly_util::is_in_pt(x1, y1, u1, v1, x2+u2, y2+v2) == true ) return true;
	return false;
}
bool fly_util::is_collide(const fly::_obj & lhs, const fly::_obj & rhs)
{
	VEC3D* lhs_pos = lhs.object->getPos(); VEC3D* rhs_pos = rhs.object->getPos();
	return fly_util::is_in_rect(lhs_pos->x, lhs_pos->y, lhs.object_size->x, lhs.object_size->y, rhs_pos->x, rhs_pos->y, 
		rhs.object_size->x, rhs.object_size->y);
}

//-----------------------------////-----------------------------//
//fly_detail
void fly_detail::init_stage( bool is_next_stage )
{
	if( is_next_stage == false ) { fly::life = 3; fly::stage_num = 1; }

	INT32 enemy_cnt = fly::stage_num, boss_enemy_cnt = fly::stage_num;
	if( fly::stage_num >= fly::max_enemy ) { enemy_cnt  = boss_enemy_cnt = fly::max_enemy; }

	i3::vector<bool> list( fly::stage_xy, false );
	for(INT32 i=0;i<enemy_cnt;++i) list[i] = true;
	std::random_shuffle(list.begin(), list.end());
	INT32 enemy_obj_cnt = 0, boss_enemy_obj_cnt = 0;
	for(INT32 i=0;i<fly::stage_xy;++i)
	{
		REAL32 x = 13.f + ((i % fly::stage_x) * 35.f);
		REAL32 y = 10.f + ((i / fly::stage_x) * 19.f);
		fly::enemy[enemy_obj_cnt].object->setPos(x, y);
		fly::enemy[enemy_obj_cnt].object->EnableCtrl(list[i]);
		if( list[i] == true )
		{
			if( boss_enemy_obj_cnt < boss_enemy_cnt )
			{
 				fly::enemy[enemy_obj_cnt].level = (i3Math::Rand() % fly::max_level);
				((i3UIButtonImageSet*)fly::enemy[enemy_obj_cnt].object)->SetShapeIdx(fly::enemy[enemy_obj_cnt].level);
				++boss_enemy_obj_cnt;
			}
			++enemy_obj_cnt;
		}
	}
	for(INT32 i=0;i<fly::max_enemy_missile;++i)
		fly::enemy_missile[i].object->EnableCtrl(false);
	for(INT32 i=0;i<fly::max_player_missile;++i)
		fly::player_missile[i].object->EnableCtrl(false);
	fly::player.object->EnableCtrl(true);
	fly::player.object->setPos(204.f, 209.f, 0.f);
	fly::direction = (i3Math::Rand() % 2) ? true : false;
	fly::go_bottom = false;
}
void fly_detail::hide_object()
{
	for(INT32 i=0;i<fly::max_enemy;++i)
		fly::enemy[i].object->EnableCtrl(false);
	for(INT32 i=0;i<fly::max_enemy_missile;++i)
		fly::enemy_missile[i].object->EnableCtrl(false);
	for(INT32 i=0;i<fly::max_player_missile;++i)
		fly::player_missile[i].object->EnableCtrl(false);
	fly::player.object->EnableCtrl(false);
}

void fly_detail::update_ready_text()
{
	fly::title_text->SetText(L"Press the SpaceBar!!");

	i3::wstring wstr_score;
	i3_locale::itoa_comma(fly::high_score, wstr_score);
	wstr_score.insert(0, L"High Score : ");
	fly::score_text->SetText(wstr_score);
}

void fly_detail::update_normal_text()
{
	i3::wstring title_str = i3::format_string(L"Enjoy!! Rest Time!!    Stage :%d Life :%d", fly::stage_num, fly::life);
	fly::title_text->SetText(title_str);

	i3::wstring wstr_score;
	i3_locale::itoa_comma(fly::score, wstr_score);

	wstr_score.insert(0, L"Score : ");
	fly::score_text->SetText(wstr_score);
}

INT32 fly_detail::find_fire_enemy_idx()
{
	i3::vector< INT32 > list;
	for(INT32 i=0;i<fly::max_enemy;++i)
	{ if( fly::enemy[i].object->isEnable() == true ) list.push_back(i); }
	if( list.empty() == true ) return fly::disable_idx;
	std::random_shuffle(list.begin(), list.end());
	return list.front();
}
INT32 fly_detail::find_fire_player_missile_idx()
{
	for(INT32 i=0;i<fly::max_player_missile;++i)
	{ if( fly::player_missile[i].object->isEnable() == false ) return i; }
	return fly::disable_idx;
}

void fly_detail::end_game()
{
	if( fly::score > fly::high_score) fly::high_score = fly::score;

	fly::score = 0;
	fly::enemy_missile_speed = 2.f;
	fly::enemy_speed = 0.5f;

	fly_detail::update_ready_text();
	fly_detail::hide_object();
	fly_detail::pf_update = &fly_detail::update_ready_impl;
}

void fly_detail::next_stage()
{
	++fly::stage_num;

	fly::enemy_missile_speed += 0.25f;
	fly::enemy_speed += 0.25f;

	fly_detail::init_stage(true);
	fly_detail::update_normal_text();
	fly_detail::pf_update = &fly_detail::update_normal_impl;
}