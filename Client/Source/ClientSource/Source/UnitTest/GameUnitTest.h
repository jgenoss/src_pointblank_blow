#pragma once
#if ENABLE_UNIT_TEST

enum UnitTestFlag
{
	UNIT_TEST_NULL = 0,

	UNIT_TEST_JUMP_STAGE = 1<<0,
	UNIT_TEST_FULLSCREEN = 1<<1,
	UNIT_TEST_AI = 1<<2,
	UNIT_TEST_UI_PHASE = 1<<3,
	UNIT_TEST_UI_POPUP = 1<<4,
	UNIT_TEST_HUD = 1<<5,
};

struct FlagTest
{
	void reset() { m_Flag = 0; }
	void add(UINT32 flag) { m_Flag |= flag; }
	void remove(UINT32 flag) { m_Flag &= ~flag; }
	bool is(UINT32 flag) const { return (m_Flag & flag) != 0 ? true : false; }

	UINT32 m_Flag = 0;
};

class CGameUnitTest;
namespace UnitTest
{
	void create_unit_test(); //command line 값이 없으면 생성 안 된다.
	void release_unit_test();
	CGameUnitTest* get_unit_test();
	void update_unit_test(REAL32 rDeltaSeconds);
}

namespace UnitTest
{
	void force_create_unit_test(); //commnad line 값이 없어도 생성 가능하도록..
}

namespace BaTMaN { void BatMan_Ingame_Update(REAL32 rDeltaSeconds); } //전방 선언
class CGameUnitTest :	public i3ElementBase
{
	I3_CLASS_DEFINE( CGameUnitTest, i3ElementBase );

	friend void UnitTest::create_unit_test();

public:
	CGameUnitTest(void);
	~CGameUnitTest(void);

	//bool IsFlag(UINT32 flag) const { return m_UnitTestFlag.is(flag); }
	void OnUpdate( REAL32 rDeltaSeconds);
	void ForceChangeHudTest();

private:
	void UpdateStageTest( REAL32 rDeltaSeconds);
	void UpdateFullScreenTest( REAL32 rDeltaSeconds);
	void UpdateHudTest(REAL32 rDeltaSeconds);

private:
	REAL32	m_CharaRespawnElaps[SLOT_MAX_COUNT];
	class CGameUnitTest_UI * m_pUIUnitTest;

public:
	FlagTest m_UnitTestFlag;
};

namespace UnitTest
{
	inline bool is_act_hud_unit_test()
	{
		if( UnitTest::get_unit_test() == 0 ) return false;
		return UnitTest::get_unit_test()->m_UnitTestFlag.is(UNIT_TEST_HUD);
	}
}

#endif //end of ENABLE_UNIT_TEST