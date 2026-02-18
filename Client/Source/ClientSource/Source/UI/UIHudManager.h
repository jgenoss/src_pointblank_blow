#pragma once

#include "UIHUDBase.h"
#include "UIHudDefine.h"
#include "iHudBase.h"

namespace hud_util //hud manager util
{
	struct Context
	{
		Context() {} //stl::vector에 resize 때문에 기본 생성자 생성
		Context(iHudBase*p, const char* n);
		Context(iHudBase*p, const char* n1, const char* n2);
		Context(iHudBase*p1, const char* n1, iHudBase*p2, const char* n2);
		
		Context(const Context& rhs) 
		{
			i3::copy( rhs.i3uie_name, rhs.i3uie_name + HUD::HS_MAX, i3uie_name);   
			i3::copy( rhs.hud, rhs.hud + HUD::HS_MAX, hud);
		}

		Context& operator=(const Context& rhs) 
		{  
			i3::copy( rhs.i3uie_name, rhs.i3uie_name + HUD::HS_MAX, i3uie_name);   
			i3::copy( rhs.hud, rhs.hud + HUD::HS_MAX, hud);
			return *this;
		}
		
		i3::string i3uie_name[HUD::HS_MAX];	//i3uie 파일명.
		iHudBase* hud[HUD::HS_MAX];	//해당 클래스
	};

	bool compare_i3uie_name(hud_util::Context & c, const i3::string & n);
}

#if ENABLE_UNIT_TEST
	namespace inner_hud { void change_test_stage_type(); }
#endif

class UIHudManager : public UIHUDBase
{
	I3_CLASS_DEFINE( UIHudManager, UIHUDBase );

	friend class UIHudFirst;
	friend class UIHudRespawnTimer;
#if ENABLE_UNIT_TEST
	friend void inner_hud::change_test_stage_type();
#endif

public:
	UIHudManager();
	virtual ~UIHudManager();

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData) override;

public:
	void Click(const char* hud_name, INT32 value1, INT32 value2); //lua쪽 함수 받는 부분이다.
	iHudBase* GetHud(HUD::UIType type);

	void EnableHud(HUD::UIType type, bool enable);
	bool IsEnableHud(HUD::UIType type) const;
		
	void Show();
	void Hide();

	void SetParameter(HUD::UIType type, INT32 arg1, INT32 arg2 =-1);	//특정 HUD::UIType 만 호출
	void SetParameter(const HUD::UITypeList & list, INT32 arg1, INT32 arg2 = -1);	//UITypeList HUD::UIType 호출

private:
	void SetUsableAllHud(); //Hud Uint Test 함수에서 사용한다. 함부로 사용하지 말것

public:
	void OnEvent(GAME_EVENT type, INT32 , const i3::user_data& UserData);
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

private:	
	typedef i3::vector< hud_util::Context > HudContextList;
	HudContextList m_hudContextList;
	bool is_complete_respawn; //게임 시작후 첫번째 리스폰(update 함수 기능 작동 시작한다.)
	bool is_load_complete_resource; //리소스 읽기전에 외부 작동 함수 호출에 대한 방어 변수
	bool is_block_msg_by_visible; //visible 유무에 따라 외부 작동 함수 호출에 대한 방어 변수

public:
	MultiSceneInfo round_result_scene_info, end_result_scene_info;

	void SetRespawn(bool bState) { is_complete_respawn = bState; }
};

namespace HUD
{
	UIHudManager* instance();
}
