//***************************//
// Hud Unit Test
#include "UIHudUtil.h"
#include "UnitTest/GameUnitTest.h"
#include "UIHudDefine.h"
#include "MyRoomInfoContext.h"
#include "ClientStageInfoUtil.h"

#if ENABLE_UNIT_TEST
	hu_ut::_test_factor hu_ut::test_factor;
#endif

bool hu_ut::is_test_changing_hud()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.changing_hud;
#endif

	return false;
}

STAGE_ID hu_ut::get_stage_id()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		return hu_ut::test_factor.stage_id;
#endif

	return MyRoomInfoContext::i()->getStage()->m_ui32StageID;
}

STAGE_MODE hu_ut::get_stage_type()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return StageID::GetStageMode( hu_ut::test_factor.stage_id );
#endif
	
	return MyRoomInfoContext::i()->getStage()->GetStageMode();
}

UINT8 hu_ut::get_my_room_sub_type()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.room_sub_type;
#endif
	return MyRoomInfoContext::i()->GetMyRoomSubType();
}

bool hu_ut::is_kill_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
	{
		STAGE_MODE stageType = StageID::GetStageMode(hu_ut::get_stage_id());
		return	stageType == STAGE_MODE_DEATHMATCH 
				|| StageID::IsHeadShotOnly(hu_ut::get_stage_id())
				|| StageID::IsHudOff(hu_ut::get_stage_id());
	}
#endif

	return CStageInfoUtil::IsKillMode(*MyRoomInfoContext::i()->getStage());
}

bool hu_ut::is_stage_mask_training()
{
#if ENABLE_UNIT_TEST
	if (UnitTest::is_act_hud_unit_test() == true)
		return StageID::GetAIMode(hu_ut::test_factor.stage_id) == AI_USE_CHALLENGE;
#endif

	return MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE);
}

bool hu_ut::is_stage_mask_madness()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return StageID::GetAIMode(hu_ut::test_factor.stage_id) == AI_USE_MADNESS;
#endif

	return MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS);
}

bool hu_ut::is_stage_mask_diehard()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return StageID::GetAIMode(hu_ut::test_factor.stage_id) == AI_USE_DIEHARD;
#endif

	return MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD);
}

bool hu_ut::is_domination_mode()
{
#if ENABLE_UNIT_TEST
	if (UnitTest::is_act_hud_unit_test() == true)
		return false;
#endif

	return false;
}

bool hu_ut::is_dino_escape_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_ESCAPE;
#endif

	return MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE);
}

bool hu_ut::is_cross_count_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_CROSSCOUNT;
#endif

	return MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT);
}

bool hu_ut::is_annihilation_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_ANNIHILATION;
#endif

	return MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION);
}

bool hu_ut::is_tutorial_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_TUTORIAL;
#endif
	
	return MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL);
}

bool hu_ut::is_destroy_mission_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_DESTROY;
#endif

	return  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY);
}

bool hu_ut::is_defence_mission_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_DEFENCE;
#endif

	return  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE);
}

bool hu_ut::is_bomb_mission_mode()
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::get_stage_type() == STAGE_MODE_BOMB;
#endif

	return MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB);
}

bool hu_ut::is_dino(CGameCharaBase* pMyChara)
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.chara_dino_type;
#endif

	return pMyChara->IsDino();
}