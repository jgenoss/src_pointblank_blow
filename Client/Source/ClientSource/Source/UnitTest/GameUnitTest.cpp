#include "pch.h"
#if ENABLE_UNIT_TEST

#include "GameUnitTest.h"

#include "GameUnitTest_UI.h"
#include "GameUnitTest_Hud.hpp"
#include "GameCharaInputContext.h"
#include "UI/UIMainFrame.h"
#include "UtilBattle.h"
#include "../StageBattle/AIModeContext.h"
#include "../StageBattle/IntrudeContext.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"


const static REAL32 AI_DELAY_TIME = 40.0f;
const static REAL32 STAGE_DELAY_TIME = 60.0f;

I3_CLASS_INSTANCE( CGameUnitTest);//, i3ElementBase);

CGameUnitTest::CGameUnitTest(void)
{
	m_pUIUnitTest = CGameUnitTest_UI::new_object();
}

CGameUnitTest::~CGameUnitTest(void)
{	
	I3_SAFE_RELEASE( m_pUIUnitTest);
}

void CGameUnitTest::OnUpdate( REAL32 rDeltaSeconds)
{
	if(m_UnitTestFlag.is(UNIT_TEST_UI_PHASE | UNIT_TEST_UI_POPUP))
	{
		if( m_pUIUnitTest->IsBattleProcess() )
		{
			m_UnitTestFlag.add( UNIT_TEST_AI);
			UpdateStageTest( rDeltaSeconds);
		}
		else
		{
			m_pUIUnitTest->OnUpdate(rDeltaSeconds);
		}
	}
	else if( m_UnitTestFlag.is(UNIT_TEST_JUMP_STAGE | UNIT_TEST_AI) )
	{
		UpdateStageTest( rDeltaSeconds);
	}
	else if( m_UnitTestFlag.is(UNIT_TEST_HUD) )
	{
		if( g_pFramework->IsBattleStage() )
		{
			UpdateHudTest(rDeltaSeconds);
		}
	}

	if( m_UnitTestFlag.is(UNIT_TEST_FULLSCREEN) )
	{
		UpdateFullScreenTest( rDeltaSeconds);
	}
}

void CGameUnitTest::UpdateStageTest( REAL32 rDeltaSeconds)
{
	static REAL32 fLocalTime = 0.0f;
	static REAL32 fLocalTime2 = 0.0f;
	static REAL32 rGiveupTime = 0.f;
	static REAL32 rIntrudeTime = 0.f;
	static bool bStageIn = false;
	static bool bFirst = false;

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( g_pFramework->IsBattleStage() )
	{
		if( !bFirst && fLocalTime2 > 0.5f)
		{// 스테이지 진입시 한번만 실행합니다.
			bFirst = true;

			if( m_UnitTestFlag.is( UNIT_TEST_AI) )
			{				
				// 로컬 캐릭터를 AI화 합니다.
				pPlayer->getInputContext()->OnChangeAI();
				
				AIModeContext::i()->EnemyCount		= 8;
				AIModeContext::i()->FriendCount		= 7;
				AIModeContext::i()->StartLevel		= 1;
				AIModeContext::i()->CurrentLevel	= 1;

				AI_BIND_DATA * pBindData = _SetAiInfo( BattleSlotContext::i()->getMySlotIdx(), AIModeContext::i()->CurrentLevel);
				pPlayer->BindAI(pBindData);

				util_battle::AddAiCharacter();
			}

			i3mem::FillZero( m_CharaRespawnElaps, sizeof( m_CharaRespawnElaps));
		}

		// 특정 시간마다 1인칭 3인칭을 변경합니다.
		const static REAL32 CHANGE_VIEW_TIME = 10.0f;
		if( fLocalTime2 > CHANGE_VIEW_TIME)
		{
			fLocalTime2 = 0.0f;
			//ub::toggle_3p_camera();
		}	

		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			I3ASSERT( pChara->getCharaNetIndex() != -1);

			INT32 idx = pChara->getCharaNetIndex();

			if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
			{
				m_CharaRespawnElaps[ idx] += rDeltaSeconds;

				if( m_CharaRespawnElaps[ idx] > 2.f)
				{
					if( idx != BattleSlotContext::i()->GetMySlotIdxEx())
					{
						util_battle::AddAiChara( idx);
						I3TRACE( "add ai %d\n", idx);
					}
					
					//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE_FOR_AI, idx);
					m_CharaRespawnElaps[ idx] = 0.f;
					I3TRACE( "respawn ai %d\n", idx);
				}
			}
		}

		fLocalTime2 += rDeltaSeconds;

		rGiveupTime += rDeltaSeconds;
		if( rGiveupTime > 8.f + (i3Math::Randf2() * 3.f))
		{
			INT32 userIndex = i3Math::Rand() % SLOT_MAX_COUNT;
			while( userIndex == BattleSlotContext::i()->getMySlotIdx() )
			{
				userIndex = i3Math::Rand() % SLOT_MAX_COUNT;
			}

			rGiveupTime = 0.f;
			if( g_pCharaManager->getCharaByNetIdx( userIndex) != nullptr)
			{
				GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GIVEUP_BATTLE, userIndex);
			}
		}

		rIntrudeTime += rDeltaSeconds;

		if( rIntrudeTime > 7.f + (i3Math::Randf2() * 3.f))
		{
			INT32 idx = 0;
			while( g_pCharaManager->getCharaByNetIdx( idx) != nullptr )
			{
				idx++;
				if( idx >= SLOT_MAX_COUNT)
					break;

				if( g_pCharaManager->getCharaByNetIdx( idx) == nullptr)
				{
					util_battle::AddAiChara( idx);
					IntrudeContext::i()->SetIntrusionMsgFlag( true); 
					GameEventReceiver::i()->PushReceivedGameEvent( EVENT_START_BATTLE, idx);
					//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE_FOR_AI, idx);
					m_CharaRespawnElaps[ idx] = 0.f;
					break;
				}
			}


			rIntrudeTime = 0.f;
		}

	}
	
	fLocalTime += rDeltaSeconds;	

	// Stage Jump처리를 합니다.
	if( bStageIn)
	{
		if( m_UnitTestFlag.is( UNIT_TEST_AI) )
		{
			if( fLocalTime > AI_DELAY_TIME)
			{
				// 스테이지 들어온상태
				bStageIn = false;
				fLocalTime = 0.0f;
				fLocalTime2 = 0.0f;

				if( m_UnitTestFlag.is(UNIT_TEST_UI_PHASE | UNIT_TEST_UI_POPUP) )
				{
					m_UnitTestFlag.remove(UNIT_TEST_AI);

					m_pUIUnitTest->SetBattleProcess(false);
					m_pUIUnitTest->JumpToReady( (i3Math::Rand() % 2) == 0 );
				}
				else
				{
					CGameFramework::JumpToLogin();
				}
			}
		}
		else
		{
			if( fLocalTime > STAGE_DELAY_TIME)
			{
				// 스테이지 들어온상태
				bStageIn = false;
				fLocalTime = 0.0f;
				fLocalTime2 = 0.0f;

				if( m_UnitTestFlag.is(UNIT_TEST_UI_PHASE | UNIT_TEST_UI_POPUP) )
				{
					m_UnitTestFlag.remove(UNIT_TEST_AI);

					m_pUIUnitTest->SetBattleProcess(false);
					m_pUIUnitTest->JumpToReady( (i3Math::Rand() % 2) == 0 );
				}
				else
				{
					CGameFramework::JumpToLogin();
				}
			}
		}
	}
	else
	{
		const static REAL32 MENU_DELAY_TIME = 5.0f;
		if( fLocalTime > MENU_DELAY_TIME)
		{
			bFirst = false;

			// 메뉴상태
			g_pConfig->m_bNetworking = false;
			CInvenList::i()->SetWeaponListDummy();
			USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
			UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

			INVEN_BUFFER buf[5];

			InfoEquipWeapon._noprim		= DEFAULT_WEAPON_ASSAULT;
			buf[0].Reset();
			buf[0]._ui32ItemID = InfoEquipWeapon._noprim;

			InfoEquipWeapon._nosub		= DEFAULT_WEAPON_HANDGUN;
			buf[1].Reset();
			buf[1]._ui32ItemID = InfoEquipWeapon._nosub;

			InfoEquipWeapon._nomelee	= DEFAULT_WEAPON_KNIFE;
			buf[2].Reset();
			buf[2]._ui32ItemID = InfoEquipWeapon._nomelee;

			InfoEquipWeapon._nothrow	= DEFAULT_WEAPON_THROWING1;
			buf[3].Reset();
			buf[3]._ui32ItemID = InfoEquipWeapon._nothrow;

			InfoEquipWeapon._noitem		= DEFAULT_WEAPON_THROWING2;
			buf[4].Reset();
			buf[4]._ui32ItemID = InfoEquipWeapon._noitem;

			UserInfoContext::i()->SetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

			CInvenList::i()->AddItem( buf, 5);

			bStageIn = true;
			fLocalTime = 0.0f;
		}
	}		
}

void CGameUnitTest::UpdateFullScreenTest( REAL32 rDeltaSeconds)
{
	static REAL32 fDeltaTestTime = 0.0f;
	fDeltaTestTime += rDeltaSeconds;

	if( fDeltaTestTime > 120.0f)
	{	
		fDeltaTestTime = 0.0f;

		UIMainFrame * pStage = g_pFramework->GetUIMainframe();
		if( i3::same_of<UIMainFrame*>(pStage))
		{
			if( ( pStage)->IsOpenedPopup( UPW_OPTION))
			{
				( pStage)->ClosePopup( UPW_OPTION);
			}
		}

		i3VideoInfo info;
		g_pViewer->GetVideoInfo( &info);
		info.m_bFullScreen = !info.m_bFullScreen;
		g_pEnvSet->m_bFullScreen = info.m_bFullScreen;
		g_pEnvSet->Save();
		g_pViewer->SetVideoInfo( &info);
	}
}

void CGameUnitTest::ForceChangeHudTest()
{
	inner_hud::change_test_stage_type();
	inner_hud::change_test_factor();
}

void CGameUnitTest::UpdateHudTest( REAL32 rDeltaSeconds)
{
	static REAL32 change_test_hud_time = 0.0f;
	change_test_hud_time += rDeltaSeconds;
	if( change_test_hud_time > 2.5f)
	{	
		change_test_hud_time = 0.0f;
		inner_hud::change_test_stage_type();
	}

	static REAL32 change_test_factor_time = 0.0f;
	change_test_factor_time += rDeltaSeconds;
	if( change_test_factor_time > 0.5f)
	{	
		change_test_factor_time = 0.0f;
		inner_hud::change_test_factor();
	}
}












namespace inner
{
	static CGameUnitTest* game_unit_test = 0;

	UINT32 parser_command_line()
	{
		UINT32 flag = 0;

		if( i3::contain_string( ::GetCommandLine(), "UT_JumpStage") >= 0)
			flag |= UNIT_TEST_JUMP_STAGE;

		if( i3::contain_string( ::GetCommandLine(), "UT_FullScreen") >= 0)
			flag |= UNIT_TEST_FULLSCREEN;

		if( i3::contain_string( ::GetCommandLine(), "UT_AiTest") >= 0)
			flag |= UNIT_TEST_AI;

		if( i3::contain_string( ::GetCommandLine(), "UT_HudTest") >= 0)
			flag |= UNIT_TEST_HUD;

		if( i3::contain_string( ::GetCommandLine(), "UT_All") >= 0)
			flag |= (UNIT_TEST_JUMP_STAGE | UNIT_TEST_FULLSCREEN | UNIT_TEST_AI);

		if(i3::contain_string( ::GetCommandLine(), "UT_UIPhase") >= 0)
		{
			//배틀과 UI를 같이 쓰면 안됩니다.
			//UI팝업과 동시에 사용하지 않습니다.
			flag &= ~UNIT_TEST_JUMP_STAGE;
			flag &= ~UNIT_TEST_AI;
			flag &= ~UNIT_TEST_UI_POPUP;

			flag |= UNIT_TEST_UI_PHASE;
		}

		if(i3::contain_string( ::GetCommandLine(), "UT_UIPopup") >= 0)
		{
			// 배틀과 UI를 같이 쓰면 안됩니다.
			// UI 페이지 테스트와 동시에 사용하지 않습니다.
			flag &= ~UNIT_TEST_JUMP_STAGE;
			flag &= ~UNIT_TEST_AI;
			flag &= ~UNIT_TEST_UI_PHASE;

			flag |= UNIT_TEST_UI_POPUP;
		}

		return flag;
	}
}

void UnitTest::create_unit_test()
{
	UINT32 command_line_flag = inner::parser_command_line();
	if( command_line_flag == UNIT_TEST_NULL ) return;

	inner::game_unit_test = CGameUnitTest::new_object();
	inner::game_unit_test->m_UnitTestFlag.add(command_line_flag);

	if( inner::game_unit_test->m_UnitTestFlag.is(UNIT_TEST_HUD) == true )
		inner::game_unit_test->ForceChangeHudTest();
}
void UnitTest::force_create_unit_test()
{
	inner::game_unit_test = CGameUnitTest::new_object();
}
void UnitTest::release_unit_test()
{
	I3_SAFE_RELEASE( inner::game_unit_test );
}
CGameUnitTest* UnitTest::get_unit_test()
{
	return inner::game_unit_test;
}
void UnitTest::update_unit_test(REAL32 rDeltaSeconds)
{
	if( inner::game_unit_test == 0 ) return;

	inner::game_unit_test->OnUpdate(rDeltaSeconds);
}

#endif //end of ENABLE_UNIT_TEST
