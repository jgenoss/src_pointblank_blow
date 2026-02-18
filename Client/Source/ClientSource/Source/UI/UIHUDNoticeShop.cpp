#include "pch.h"
#include "UIHUDNoticeShop.h"

#include "UIUtil.h"
#include "UIBattleFrame.h"
#include "UIBattlePopupBase.h"
#include "IGShopContext.h"

#include "BattleHud.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
namespace
{
	UIHUDNoticeShop* g_this = 0;
}

UIHUDNoticeShop* UIHUDNoticeShop::instance()
{
	return g_this;
}

LuaFuncReg UIHUDNoticeShop_Glue[] = {
	{nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UIHUDNoticeShop);//, UIHUDBase);

UIHUDNoticeShop::UIHUDNoticeShop() : m_observer_mode(0)
{
	g_this = this;
	GetSlide()->SetTargetTime(false, 1.f / ( UI_SLIDEOUT_SLOP * 2.f) );
	GetSlide()->SetTargetTime(true, 1.f / ( UI_SLIDEIN_SLOP * 2.f) );
	m_visible = true;
}

UIHUDNoticeShop::~UIHUDNoticeShop()
{
}

void	UIHUDNoticeShop::OnCreate( i3GameNode * pParent)
{
	UIHUDBase::OnCreate(pParent);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == false)
	{	// 튜토리얼에서는 로드하지 않도록
		AddScene("Scene/HUD/PBRe_HUDShopIcon.i3UIs", UIHUDNoticeShop_Glue, true, false);		// scn이 바로 생성..
		
		LuaState* L = UILua::CallLuaFunction(GetScene(0), "GetValidTime");
		INT32 sec = UILua::ReturnLuaInteger(L, 0);
		UIBattleFrame* frame = static_cast<UIBattleFrame*>(pParent);
		frame->SetGameShopValidTime(sec);
	}
	
}

bool	UIHUDNoticeShop::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (UIHUDBase::OnEntranceStart(pArg1, pArg2) == false ) return false;
	
	
	i3::rc_wstring wstrKey =	GameUI::GetKeymapString( GAME_KEY_MAP_INGAME_SHOP);
	
	i3::stack_wstring wstr;

	i3::sprintf(wstr, L"%s (%s)", GAME_RCSTRING("STR_INGAME_SHOP"), wstrKey);

	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "SetName");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstr);
		UILua::EndLuaFunction(L, 1);
	}
	
	GetSlide()->AddFrameWnds(GetScene(0), UISlide::R_TO_L);
	GetSlide()->Start(true);

	return true;
}

void	UIHUDNoticeShop::OnExitEnd()
{
	UIHUDBase::OnExitEnd();
	UIHUDNoticeShop::RestorePos();
}

void	UIHUDNoticeShop::ModifyPos()
{
	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "ModifyPos");
	if( L != nullptr)
	{
		UILua::EndLuaFunction(L, 0);
	}
}

void	UIHUDNoticeShop::RestorePos()
{
	i3UIScene* scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(scn, "RestorePos");
	if( L != nullptr)
	{
		UILua::EndLuaFunction(L, 0);
	}
}

// 활성화된 상태에서만 야상입력 가능..
bool UIHUDNoticeShop::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIHUDBase::OnKeyInput( pInputMng) )
		return true;
		
	// 't'키와는 달리 여기서 처리..무기교체의 경우 채팅창조건, 장치설치/해제조건을 따지는데, 채팅창조건은 자동으로 막힘..
	// 여기서 장치설치,해제조건은 일단 뺐다..
	UINT64 stroke = g_pFramework->getKeyStroke();
	if (stroke & GAME_KEY_INGAME_SHOP)
	{	
		UINT64 press  = g_pFramework->getKeyPress();
		if (press & GAME_KEY_CHANGEWEAPON )		// 무기교체 키처리는 StageBattle에 잔존..나중에 다른 곳에서 처리하는게 좋을것이다.
			return false;				

		if( UIBattleFrame::i() != nullptr &&
			UIBattleFrame::i()->IsOpenPopup(UBP_INGAME_SHOP) == false)
		{
			if( !m_observer_mode )
			{
				IGShopContext* cont = g_pFramework->GetIGShopContext();
				cont->SetEventOnce_Open();									// 요청하는동안 HUD가 닫힐수 있음..일단 uibattleFrame에서 패킷처리함..
				return true;
			}
		}
	}
 
	return false;
}

void UIHUDNoticeShop::SetVisible(bool visible)
{
	if( LocaleValue::Enable("FieldShop") == false ) return;
	GetScene(0)->EnableScene( visible );
	m_visible = visible;
}

void UIHUDNoticeShop::SetObserverMode(bool mode)
{
	if( LocaleValue::Enable("FieldShop") == false ) return;

	if( mode == true && m_observer_mode == mode ) return;
	m_observer_mode = mode;

	if( GetScene(0) != nullptr)
		GetScene(0)->EnableScene( !mode );
}
 
 
/////////////////////////////////////////////////////////

namespace
{



igshop_ctrl::get_handler* g_get_handler_array[STAGE_MODE_MAX] = { 0, };


struct handler_none : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner){}
	virtual void SetDeathDelay(igshop_ctrl* owner){}

	virtual void SetRespawn(igshop_ctrl* owner) {}
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// 라운드 종료시 켜지 않는다..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}	
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}

} g_hNone;


struct handler_idle : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);
	virtual void SetDeathDelay(igshop_ctrl* owner);

	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// 라운드 종료시 켜지 않는다..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) { }
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}

} g_hIdle;

struct handler_death :  igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// 이미 뒈진 상태로 처리가 없음..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// 죽은 상태에서 딜레이는 없음..
	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) { }
	virtual void OnUpdate(igshop_ctrl* owner, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.

} g_hDeath;


struct handler_death_delay : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// 단체제와 라운드제 간 충돌이 나면 안됨..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// 중복기능으로 구현생략..
	virtual void SetRespawn(igshop_ctrl* owner);			
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	

} g_hDeathDelay;

// 제압모드는 리스폰상태가 별도로 없고, 리스폰이 즉시 아이들 상태로 처리된다..
struct handler_respawn : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);
	virtual void SetDeathDelay(igshop_ctrl* owner);
	virtual void SetRespawn(igshop_ctrl* owner) { }
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner);
	virtual void OnUpdate(igshop_ctrl* owner, REAL32 delta);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}
	
} g_hRespawn;




struct handler_idle_domi : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner);				// 죽었을때 켜는건 동일..
	virtual void SetDeathDelay(igshop_ctrl* owner);			// 죽었을때 켜는건 동일..

	virtual void SetRespawn(igshop_ctrl* owner)	{}			// 리스폰때 켜지않으므로 구현 비움..
	virtual void SetRoundEnd(igshop_ctrl* owner) {}			// 라운드 종료시 켜지 않는다..
	virtual void SetGameEnd(igshop_ctrl* owner) {}
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl*, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner);
	virtual void SetIntermissionEnd(igshop_ctrl* owner);	

} g_hIdle_domi;

struct handler_death_domi :  igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// 이미 뒈진 상태로 처리가 없음..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// 죽은 상태에서 딜레이는 없음..
	virtual void SetRespawn(igshop_ctrl* owner);
	virtual void SetRoundEnd(igshop_ctrl* owner){}			// 라운드가 종료되어도 죽은 상태임..야전상점마크 유지.
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32) {}
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.

} g_hDeath_domi;

struct handler_death_delay_domi : igshop_ctrl::handler
{
	virtual void SetDeath(igshop_ctrl* owner) {}			// 단체제와 라운드제 간 충돌이 나면 안됨..
	virtual void SetDeathDelay(igshop_ctrl* owner) {}		// 중복기능으로 구현생략..
	virtual void SetRespawn(igshop_ctrl* owner);			
	virtual void SetRoundEnd(igshop_ctrl* owner);
	virtual void SetGameEnd(igshop_ctrl* owner);
	virtual void OnRespawnTimeDone(igshop_ctrl* owner) {}
	virtual void OnUpdate(igshop_ctrl* owner, REAL32);
	virtual void SetIntermissionStart(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.
	virtual void SetIntermissionEnd(igshop_ctrl* owner){}	// 죽은 상태는 변함없다.	

} g_hDeathDelay_domi;




void handler_idle::SetDeath(igshop_ctrl* owner) 
{  
	// 뒈지면 킨다..
	owner->EnableNoticeShop(true);		
	owner->SetHandler_Death(); 
}

void handler_idle::SetDeathDelay(igshop_ctrl* owner)
{
	// 켜지 않고 핸들러를 바꾼다...  딜레이타임값을 반드시 리셋해야함..
	owner->ResetDeathDelayTime();
	owner->SetHandler_Death_Delay(); 
}

void handler_respawn::SetDeath(igshop_ctrl* owner)
{
	// 이 경우에는 켜져있는지 꺼져있는지 잘 모르는데, 켜둔다...다만 타이머를 종료시키는게 좋겠다..
	owner->StopShopTimer();
	owner->EnableNoticeShop(true);
	owner->SetHandler_Death(); 
	//
}

void handler_respawn::SetDeathDelay(igshop_ctrl* owner)
{
	// 일단 타이머는 종료.. 핸들러는 death로 맞춘다..
	owner->StopShopTimer();
	owner->SetHandler_Death(); 
}

// 아마도 죽지 않은 리스폰은 게임시작 리스폰일 것임..
void handler_idle::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(true);		// 리스폰,스타트시 HUD를 킨다...킨건 그대로 유지..이건 표시일뿐...	
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
}

void handler_death::SetRespawn(igshop_ctrl* owner) 
{  
	// 이미 켜져있을 것으로 보이며, 핸들러를 바꾸고, 타이머를 켠다..
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
	//
}

void  handler_death_delay::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(true);		// 딜레이 중엔 켜져있지 않으므로 킨다..
	owner->SetHandler_Respawn(); 
	owner->StartShopTimer();
}

void handler_death_delay::SetRoundEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle(); // hud가 켜져있지 않으며, 상태만 바꾼다...
}

void handler_respawn::SetRoundEnd(igshop_ctrl* owner)
{
	owner->StopShopTimer();					// 타이머 꺼둔다...
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle(); 
}

void handler_death::SetRoundEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle(); 
}


void handler_death::SetGameEnd(igshop_ctrl* owner)			// 기본적으로는 점수표시 구현과 같다..달라질 경우를 대비해 분리..
{
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}

void handler_respawn::SetGameEnd(igshop_ctrl* owner)		// 기본적으로는 점수표시 구현과 같다..달라질 경우를 대비해 분리..
{
	owner->StopShopTimer();
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}

void handler_death_delay::SetGameEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle();	
}



void handler_idle_domi::SetDeath(igshop_ctrl* owner)		// 죽었을때 켜는건 동일..
{
	// 뒈지면 킨다..
	owner->EnableNoticeShop(true);		
	owner->SetHandler_Death(); 
}

void handler_idle_domi::SetDeathDelay(igshop_ctrl* owner)	// 죽었을때 켜는건 동일..
{
	// 켜지 않고 핸들러를 바꾼다...  딜레이타임값을 반드시 리셋해야함..
	owner->ResetDeathDelayTime();
	owner->SetHandler_Death_Delay(); 
}

void handler_death_domi::SetRespawn(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);			// 도미에서는 리스폰시 무조건 끈다..
	owner->SetHandler_Idle();				// 리스폰 상태가 아이들 상태이다..
}

void  handler_death_delay_domi::SetRespawn(igshop_ctrl* owner)		// 아직 켜지 않았으므로 끄지도 않음..
{
	owner->SetHandler_Idle();
}

void handler_death_delay_domi::SetRoundEnd(igshop_ctrl* owner)			// 켜지 않았고, 이건 데스로 바꾸고 켜야함.
{
	owner->EnableNoticeShop(true);
	owner->SetHandler_Death(); // 죽은 것을 확실히 함..
}

void  handler_death_domi::SetGameEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);							// 게임종료시에는 야전상점 끔..
	owner->SetHandler_Idle();
}

void handler_death_delay_domi::SetGameEnd(igshop_ctrl* owner)
{
	owner->SetHandler_Idle();						
}


bool	check_respawn_area( i3RespawnObj* res_obj , VEC3D* my_pos)
{
	VEC3D res_pos;

	i3Vector::Copy(&res_pos, res_obj->GetPos());
ISTEST_S
	{
		res_pos.y += 0.05f;
	}
	else
ISTEST_E
	{
		res_pos.y += 0.2f;
	}

	res_pos.y -= my_pos->y;
	REAL32 dist_y = ::fabs(res_pos.y);

	if ( dist_y > INGAME_SHOP_VALID_DISTANCE_Y ) 
		return false;

	res_pos.x -= my_pos->x;
	res_pos.z -= my_pos->z;
	REAL32 dist_sq_xz = res_pos.x * res_pos.x + res_pos.z * res_pos.z;

	// 리스폰 영역 검사..(통과하게 되면...내부...)
	return (dist_sq_xz <= INGAME_SHOP_VALID_DISTANCE_XZ * INGAME_SHOP_VALID_DISTANCE_XZ );
}


void handler_respawn::OnUpdate(igshop_ctrl* owner, REAL32 delta)
{
// 리스폰후 20초까지는 거리검사를 해야함...	// 이 경우엔 켜고 끄는 것만 처리한다..
	CGameCharaBase* my_chara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	
	if ( my_chara == nullptr )	// 이런 불상사가 생기면...idle로 바꾸고 꺼버린다..
	{
		owner->EnableNoticeShop(false);
		owner->StopShopTimer();
		owner->SetHandler_Idle(); 
		return;
	}
	
	VEC3D* my_pos = my_chara->GetPos();
	const GLOBAL_CHARA_INFO* ch_info = my_chara->getCharaInfo();
	INT32 nTeam = ch_info->GetTeam();
 
	CGameRespawn*	game_respawn = g_pCharaManager->getRespawnPointInfo();

	const INT32 num_res = game_respawn->GetRespawnCount((CHARACTER_TEAM_TYPE)nTeam);

	INT32 tc_idx =	owner->GetTCRespawnIdx();

	bool bCollis = false;

	if (tc_idx > -1 && tc_idx < num_res )
	{
		i3RespawnObj* res_obj =	game_respawn->GetRespawnObject(nTeam, tc_idx);
		bCollis = check_respawn_area( res_obj, my_pos );
	}
	
	if (bCollis == false)
	{
		for (INT32 i = 0 ; i < num_res ; ++i )
		{
		// ..유효범위 체크..	
			if ( i == tc_idx ) continue;

			i3RespawnObj* res_obj =	game_respawn->GetRespawnObject(nTeam, i);
			bCollis = check_respawn_area( res_obj, my_pos) ;
			if (bCollis == true)
			{
				owner->SetTCRespawnIdx(i);	break;
			}
		}
	}
	
	owner->EnableNoticeShop(bCollis);
}

void handler_death_delay::OnUpdate(igshop_ctrl* owner, REAL32 rDelta) 
{
	owner->AddDeathDelayTime(rDelta);	
	
	if (owner->GetCurrDeathDelayTime() >= INGAME_SHOP_DEATHDELAY_ROUND )
	{
		owner->EnableNoticeShop(true);
		owner->SetHandler_Death(); 
		// 여기서 리셋하지말고, 시작할때만 리셋할것..
	}
}

void handler_respawn::OnRespawnTimeDone(igshop_ctrl* owner)
{
// 리스폰시간 종료면 idle로 돌린다... (뒈진 경우엔 아예 이쪽으로 오지 않음)
	owner->EnableNoticeShop(false);
	owner->SetHandler_Idle();
}


void handler_death_delay_domi::OnUpdate(igshop_ctrl* owner, REAL32 rDelta)
{
	owner->AddDeathDelayTime(rDelta);
	if (owner->GetCurrDeathDelayTime() >= INGAME_SHOP_DEATHDELAY_ROUND )
	{
		owner->EnableNoticeShop(true);
		owner->SetHandler_Death(); 
		// 여기서 리셋하지말고, 시작할때만 리셋할것..
	}
}

void handler_idle_domi::SetIntermissionStart(igshop_ctrl* owner)
{
	// 인터미션 시작시에 킨다..

	UIHUDNoticeShop* notice = static_cast<UIHUDNoticeShop*>(UIBattleFrame::i()->GetHUD(UIHUD_NOTICE_SHOP));
	notice->ModifyPos();

	owner->EnableNoticeShop(true);		
	// 죽은 것이 아니고, 리스폰된것도 아니기 때문에, 그대로 놔둔다.....
}

void handler_idle_domi::SetIntermissionEnd(igshop_ctrl* owner)
{
	owner->EnableNoticeShop(false);		// 인터미션중엔 켜져있을것같으므로 이게 끝나면 그냥 꺼준다..
}


struct get_handler_normal : igshop_ctrl::get_handler
{
	virtual igshop_ctrl::handler*		idle() { return &g_hIdle; }
	virtual igshop_ctrl::handler*		death() { return &g_hDeath; }
	virtual igshop_ctrl::handler*		death_delay() { return &g_hDeathDelay; }
	virtual igshop_ctrl::handler*		respawn() { return &g_hRespawn; }
} g_get_handler_normal;


struct get_handler_domi : igshop_ctrl::get_handler
{
	virtual igshop_ctrl::handler*		idle() { return &g_hIdle_domi; }
	virtual igshop_ctrl::handler*		death() { return &g_hDeath_domi; }
	virtual igshop_ctrl::handler*		death_delay() { return &g_hDeathDelay_domi; }
	virtual igshop_ctrl::handler*		respawn() { return &g_hIdle_domi; }	
} g_get_handler_domi;


struct init 
{
	init()
	{
		for (INT32 i = 0 ; i < STAGE_MODE_MAX; ++i)
			g_get_handler_array[i] = &g_get_handler_normal;
	};
}_;

igshop_ctrl::get_handler*	get_handler_mtd() { return g_get_handler_array[MyRoomInfoContext::i()->getStage()->GetStageMode()]; }

}

namespace
{
	bool IsThisGameModeFieldShopEnable()
	{
		return !(	MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SNIPER_ONLY) ||
				MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SHOTGUN_ONLY) ||
				MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY) || 
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)			// 튜터리얼 모드...
				);
	}
}


igshop_ctrl::igshop_ctrl() : m_get_handler(get_handler_mtd()), 
	m_handler(&g_hNone), m_tcRespawnIdx(-1), m_bEnableNoticeShop(false), m_fCurrDeathDelayTime_Round(0.f)
{
	if ( LocaleValue::Enable("FieldShop") &&
		IsThisGameModeFieldShopEnable() )
		m_handler = m_get_handler->idle(); 

	m_sc.SetReverse(true);
	m_sc_cb.SetObj(this);
	m_sc.SetCallback(&m_sc_cb);
	m_sc.SetBaseSec(INGAME_SHOP_VALID_TIME);
}

void	igshop_ctrl::SetGameShopValidTime(INT32 sec)
{
	m_sc.SetBaseSec(sec);	
}

void	igshop_ctrl::OnUpdate(REAL32 delta)
{
	igshop_ctrl::Process_NoticeShopQueue();

	m_sc.Update();
	m_handler->OnUpdate(this, delta);
}

void	igshop_ctrl::Process_NoticeShopQueue()
{
	if ( m_queue_popup.empty() ) 
		return;

	UIBattleFrame* frame = UIBattleFrame::i();
	UIHUDBase*	hud = frame->GetHUD(UIHUD_NOTICE_SHOP);
	bool		hud_enable = (hud->isEnable() != false);

	// 슬라이딩 과정은 한번에 하나씩....일괄처리를 할수 없음...
	bool target_enable = m_queue_popup[0];
	if (target_enable != hud_enable)
	{
		frame->EnableHUD(UIHUD_NOTICE_SHOP, target_enable != false  );
		m_queue_popup.pop_front();
		I3TRACE("-- 슬라이딩 중 HUD enable 변경처리 내용:%d -- \n", target_enable);
	}
	
}

void	igshop_ctrl::SetDeath()
{
	m_handler->SetDeath(this);
}

void	igshop_ctrl::SetDeathDelay()
{
	m_handler->SetDeathDelay(this);
}

void	igshop_ctrl::SetRespawn()
{
	m_handler->SetRespawn(this);
}

void	igshop_ctrl::SetRoundEnd()
{
	m_handler->SetRoundEnd(this);
}

void	igshop_ctrl::SetGameEnd()
{
	// 종료시에는 안전한 처리를 위해 큐를 비움..
	m_queue_popup.clear();
	//
	m_handler->SetGameEnd(this);
}

void	igshop_ctrl::SetIntermissionStart()
{
	m_handler->SetIntermissionStart(this);
}

void	igshop_ctrl::SetIntermissionEnd()
{
	m_handler->SetIntermissionEnd(this);
}

void	igshop_ctrl::StartShopTimer()
{
	m_sc.Start();
}

void	igshop_ctrl::StopShopTimer()
{
	m_sc.Stop();
}

void	igshop_ctrl::OnSec_Start(INT32 result)
{
	
}

void	igshop_ctrl::OnSec_Change(INT32 result)
{

}

void	igshop_ctrl::OnSec_Done(INT32 result)
{
	m_handler->OnRespawnTimeDone(this);
}

void	igshop_ctrl::OnSec_Stop(INT32 result)
{

}

void	igshop_ctrl::EnableNoticeShop(bool bEnable)
{
	if( UIHUDNoticeShop::instance()->m_visible == false ) return;

	// 공룡은 야전상점 사용 못함.
	CGameCharaBase* localPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if (localPlayer->IsDino())
		return;

	if (m_bEnableNoticeShop != bEnable)
	{
		m_bEnableNoticeShop = bEnable;
		
		UIBattleFrame* frame = UIBattleFrame::i();	
		if (frame)
		{
			size_t num_queue = m_queue_popup.size();

			if (num_queue == 0)
			{
				if ( frame->GetHUD(UIHUD_NOTICE_SHOP)->isEnable() == bool(bEnable) )
				{
					// 슬라이드중에 상태변환이 생기면 잘 처리되지 않기 때문에 이 경우에 한해서 업데이트를 굴린다..
					m_queue_popup.push_back(bEnable);	// 다음에 처리할 상황을 기록한다..
					I3TRACE("-- 슬라이딩 중 HUD enable 변경시도 횟수:%d 내용:%d -- \n", num_queue, bool(bEnable));
				}
				else
				{
					frame->EnableHUD(UIHUD_NOTICE_SHOP, bEnable);	
				}
			}
			else		// 큐잉된것이 하나라도 있다면, 계속 큐잉함..
			{
				m_queue_popup.push_back(bEnable);
				I3TRACE("-- 슬라이딩 중 HUD enable 변경시도 횟수:%d 내용:%d -- \n", num_queue, bool(bEnable));
			}
		}
	}
	
}



