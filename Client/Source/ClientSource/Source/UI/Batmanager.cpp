#include "pch.h"
#include "Batmanager.h"

#include "UIUtil.h"
#include "VideoUtil.h" 
#include "UIMath.h"
#include "UnitTest/GameUnitTest.h"
#include "GlobalInfoDef.h"

bool BaTMaN::use_batman = false;
bool BaTMaN::disable_ai_attack = false;
bool BaTMaN::gm_observer_mode = false;
bool BaTMaN::enable_input_away_checker = true;
bool BaTMaN::enable_native_language = true;
bool BaTMaN::enable_jump_booster = false;

bool BaTMaN::enable_debug_item_name = false;
bool BaTMaN::enable_move_inven_wnd = true;

REAL32 BaTMaN::magnification_respawn = 1.f;
INT32 BaTMaN::jump_booster_factor = 1;

namespace
{
	//BaTMaN::init_batman() 이 함수를 호출하기 위해 만든 구조체 이다.
	struct init
	{
		init()
		{
			BaTMaN::init_batman();
		};
	} ___ginit;

	bool is_file_exist = false;
}

void BaTMaN::init_batman()
{
#if 0 
	#ifdef DEV_BUILD
		is_file_exist = true;
		use_batman = true;
		return;
	#endif
#endif
	is_file_exist = FileUtil::is_exist_file("PointBlank.dbg");
	use_batman = FileUtil::get_file_size("PointBlank.dbg") ? 1 : 0;

#ifdef NC_BUILD
	//is_file_exist = false;		//문제가 생길 시에 이 부분 주석 제거 하면 된다.
	//use_batman = false;
#endif
}

void BaTMaN::BatMan_Update(REAL32 rDeltaSeconds)
{
	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_RCTRL);

	//Batmat 사용 유무
	if( batman )
	{
		if(  GameUI::IsKeyboardStroked(I3I_KEY_B) )
		{
			use_batman = !use_batman;

			if( use_batman )
				GameUI::PrintSystemChat(L"BatMan 활성화");
			else
				GameUI::PrintSystemChat(L"BatMan 비활성화");

			g_pFramework->PrintUserInfo();
		}
	}

	batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_RALT) )
		{
			enable_native_language = !enable_native_language;

			if( enable_native_language )
			{
				if( CGameStringMap::Get()->CreateForPointBlank() == true )
					GameUI::PrintSystemChat(L"Setting Original Language");
				else
					GameUI::PrintSystemChat(L"Setting Original Language Fail...");
			}
			else
			{
				if( CGameStringMap::Get()->LoadingEnglishLanguage() == true )
					GameUI::PrintSystemChat(L"Setting English Language");
				else
					GameUI::PrintSystemChat(L"Setting English Language Fail...");
			}
		}
	}

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_SPACE) )
		{
			g_pFramework->GotoNextStage("Exit");
		}
	}
}

void BaTMaN::BatMan_Ingame_Update(REAL32 rDeltaSeconds)
{
	if( is_file_exist == false ) return;
	if( use_batman == false ) return;

	BatMan_Update(rDeltaSeconds);

	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	//일반 AI 케릭터 업데이트 안합니다.
	if( batman )
	{
		if(  GameUI::IsKeyboardStroked(I3I_KEY_A) )
		{
			disable_ai_attack = !disable_ai_attack;
			if( disable_ai_attack )
				GameUI::PrintSystemChat(L"AI 캐릭터 바보");
			else
				GameUI::PrintSystemChat(L"AI 캐릭터 천재");
		}
	}

	//리스폰 속도
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_R) )
		{
			if( magnification_respawn == 1.f)
			{
				magnification_respawn = 1000.f;
				GameUI::PrintSystemChat(L"리스폰 최대 속도");
			}
			else
			{
				magnification_respawn = 1.f;
				GameUI::PrintSystemChat(L"리스폰 원래 속도");
			}
		}
	}

	//무 입력 강제 퇴장
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_I) )
		{
			enable_input_away_checker = !enable_input_away_checker;
			if( enable_input_away_checker )
				GameUI::PrintSystemChat(L"무 입력 강제 퇴장 활성화");
			else
				GameUI::PrintSystemChat(L"무 입력 강제 퇴장 비활성화");
		}
	}

	//무 입력 강제 퇴장
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_J) )
		{
			enable_jump_booster = !enable_jump_booster;
			if( enable_jump_booster )
			{
				i3::rc_wstring wstr;
				i3::sprintf(wstr, L"Jump Booster 활성화, Booster Rate : %d", jump_booster_factor);
				GameUI::PrintSystemChat( wstr );
			}
			else
			{
				GameUI::PrintSystemChat(L"Jump Booster 비활성화");
			}
		}

		if( enable_jump_booster && GameUI::IsKeyboardStroked(I3I_KEY_B) )
		{
			++jump_booster_factor;
			jump_booster_factor = uiMath::roundlimited(jump_booster_factor, 1, 9);
			i3::rc_wstring wstr;
			i3::sprintf(wstr, L"Booster Rate : %d", jump_booster_factor);

			GameUI::PrintSystemChat( wstr );
		}
	}

#if ENABLE_UNIT_TEST
	//hud unit test
	if(batman)
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_Q) )
		{
			CGameUnitTest* hud_unit_test = UnitTest::get_unit_test();
			if(hud_unit_test == 0) { UnitTest::force_create_unit_test(); hud_unit_test = UnitTest::get_unit_test(); } //강제로 생성

			if( hud_unit_test->m_UnitTestFlag.is(UNIT_TEST_HUD) == true )
				hud_unit_test->m_UnitTestFlag.remove(UNIT_TEST_HUD);
			else
				hud_unit_test->m_UnitTestFlag.add(UNIT_TEST_HUD);

			hud_unit_test->ForceChangeHudTest();
		}
	}
#endif
}

void BaTMaN::BatMan_Outgame_Update(REAL32 rDeltaSeconds)
{
	if( is_file_exist == false ) return;
	if( use_batman == false ) return;

	BatMan_Update(rDeltaSeconds);

	bool batman = GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT);

	//GM 계정
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_G) )
		{
			gm_observer_mode = !gm_observer_mode;
			if(gm_observer_mode)
				GameUI::PrintSystemChat(L"GM 옵져버 모드");
			else
				GameUI::PrintSystemChat(L"GM 옵져버 비 모드");
		}
	}

	//debug item name
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_N) )
		{
			enable_debug_item_name = !enable_debug_item_name;
			if(enable_debug_item_name)
				GameUI::PrintSystemChat(L"디버그 아이템 이름");
			else
				GameUI::PrintSystemChat(L"평상시 아이템 이름");
		}
	}

	//move inven wnd
	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_S) )
		{
			enable_move_inven_wnd = !enable_move_inven_wnd;
			if(enable_move_inven_wnd)
				GameUI::PrintSystemChat(L"인벤 윈도우 움직임");
			else
				GameUI::PrintSystemChat(L"인벤 윈도우 멈춤");
		}
	}

	if( batman )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_W) )	//윈도우 창 모드로 변경
		{
			VideoUtil::VideoContext cxt;

			cxt.m_AA = g_pEnvSet->m_AA;
			cxt.m_AAQ = g_pEnvSet->m_AAQ;
			cxt.m_b16BitsDepth = g_pEnvSet->m_b16BitsDepth;
			cxt.m_bFullScreen = false;
			cxt.m_bTriLinearFilter = g_pEnvSet->m_bTriLinearFilter;
			cxt.m_bVSync = g_pEnvSet->m_bVSync;
			cxt.m_nScreenHeight = g_pEnvSet->m_nScreenHeight;
			cxt.m_nScreenHz = g_pEnvSet->m_nScreenHz;
			cxt.m_nScreenWidth = g_pEnvSet->m_nScreenWidth;
			cxt.m_ScreenDepth = g_pEnvSet->m_ScreenDepth;

			VideoUtil::ApplyVideo(cxt);
		}
	}
}