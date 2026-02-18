#include "pch.h"
#include "DomiSkillProperties.h"
#include <limits>

namespace domi
{
	const char* filePath = "Weapon/Domination/Skill_SP_Table.ini";

	SkillProp::SkillProp() : m_isLoaded(false) {}
	SkillProp::~SkillProp()
	{
		Clear();
	}

	void SkillProp::Clear()
	{
		i3::cu::for_each_delete_clear(m_userskills);
		i3::cu::for_each_delete_clear(m_sps);
		i3::cu::for_each_delete_clear(m_dummys);
		i3::cu::for_each_delete_clear(m_sentryguns);

		m_isLoaded = false;
	}

#ifdef max
#undef max
#endif

	void SkillProp::LoadFromFile()
	{
		if (m_isLoaded) return;

		i3IniParser parser;
		if (!parser.OpenFromFile(filePath))
			I3ASSERT_0;

		INT32 commonVal = 0;
		char commonStr[64] = {0, };

		// HP/AMMO 서플라이 소모 비용(SP)
		parser.ReadSection(_T("UseSupply"));

		parser.GetValue("HPSupply_Use_SP", &commonVal);
		m_supplycost.hpsupply_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
		MEMDUMP_NEW( m_supplycost.hpsupply_sp, sizeof(WeightedValue<UINT16>));
		
		parser.GetValue("AmmoSupply_Use_SP", &commonVal);
		m_supplycost.ammosupply_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
		MEMDUMP_NEW( m_supplycost.ammosupply_sp, sizeof(WeightedValue<UINT16>));


		// 센트리건 SP, HP
		parser.ReadSection(_T("InstallObject"));

		m_sentryguns.reserve(MAX_LEVEL_SENTRYGUN+1);

		// level 0은 투명오브젝트. 효과없는 값으로 채워 넣는다.
		SENTRYGUN_ELEM* sentryLv0 = new SENTRYGUN_ELEM;
		MEMDUMP_NEW( sentryLv0, sizeof(SENTRYGUN_ELEM));

		sentryLv0->install_sp = new WeightedValue<UINT16>(numeric_limits<UINT16>::max());
		MEMDUMP_NEW( sentryLv0->install_sp, sizeof(WeightedValue<UINT16>));

		sentryLv0->repair_sp = new WeightedValue<UINT16>(numeric_limits<UINT16>::max());
		MEMDUMP_NEW( sentryLv0->repair_sp, sizeof(WeightedValue<UINT16>));

		sentryLv0->max_hp = new Value<INT16>(numeric_limits<INT16>::max());
		MEMDUMP_NEW( sentryLv0->max_hp, sizeof(Value<INT16>));

		m_sentryguns.push_back(sentryLv0);

		for (INT32 i=1; i<=MAX_LEVEL_SENTRYGUN; i++)
		{
			SENTRYGUN_ELEM* elem = new SENTRYGUN_ELEM;
			MEMDUMP_NEW( elem, sizeof(SENTRYGUN_ELEM));

			i3::snprintf(commonStr, 63, "SentryGunLv%d_Install_SP", i);
			parser.GetValue(commonStr, &commonVal);
			elem->install_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->install_sp, sizeof(WeightedValue<UINT16>));

			i3::snprintf(commonStr, 63, "SentryGunLv%d_Fix_SP", i);
			parser.GetValue(commonStr, &commonVal);
			elem->repair_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->repair_sp, sizeof(WeightedValue<UINT16>));

			i3::snprintf(commonStr, 63, "SentryGunLv%d_HP", i);
			parser.GetValue(commonStr, &commonVal);
			elem->max_hp = new Value<INT16>(static_cast<INT16>(commonVal));
			MEMDUMP_NEW( elem->max_hp, sizeof(Value<INT16>));

			m_sentryguns.push_back(elem);
		}

		// 더미 SP, HP, 폭발 데미지
		m_dummys.reserve(MAX_LEVEL_DUMMY+1);

		DUMMY_ELEM* dummyLv0 = new DUMMY_ELEM;
		MEMDUMP_NEW( dummyLv0, sizeof(DUMMY_ELEM));

		dummyLv0->install_sp = new WeightedValue<UINT16>(numeric_limits<UINT16>::max());
		MEMDUMP_NEW( dummyLv0->install_sp, sizeof(WeightedValue<UINT16>));

		dummyLv0->repair_sp = new WeightedValue<UINT16>(numeric_limits<UINT16>::max());
		MEMDUMP_NEW( dummyLv0->repair_sp, sizeof(WeightedValue<UINT16>));

		dummyLv0->max_hp = new Value<INT16>(numeric_limits<INT16>::max());
		MEMDUMP_NEW( dummyLv0->max_hp, sizeof(Value<INT16>));

		dummyLv0->explosion_damage = new Value<INT16>(numeric_limits<INT16>::max());
		MEMDUMP_NEW( dummyLv0->explosion_damage, sizeof(Value<INT16>));

		m_dummys.push_back(dummyLv0);

		for (INT32 i=1; i<=MAX_LEVEL_DUMMY; i++)
		{
			DUMMY_ELEM* elem = new DUMMY_ELEM;
			MEMDUMP_NEW( elem, sizeof(DUMMY_ELEM));

			i3::snprintf(commonStr, 63, "DummyLv%d_Install_SP", i);
			parser.GetValue(commonStr, &commonVal);
			elem->install_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->install_sp, sizeof(WeightedValue<UINT16>));

			i3::snprintf(commonStr, 63, "DummyLv%d_Fix_SP", i);
			parser.GetValue(commonStr, &commonVal );
			elem->repair_sp = new WeightedValue<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->repair_sp, sizeof(WeightedValue<UINT16>));

			i3::snprintf(commonStr, 63, "DummyLv%d_HP", i);
			parser.GetValue(commonStr, &commonVal);
			elem->max_hp = new Value<INT16>(static_cast<INT16>(commonVal));
			MEMDUMP_NEW( elem->max_hp, sizeof(Value<INT16>));

			i3::snprintf(commonStr, 63, "DummyLv%d_DamageExplosion", i);
			parser.GetValue(commonStr, &commonVal);
			elem->explosion_damage = new Value<INT16>(static_cast<INT16>(commonVal));
			MEMDUMP_NEW( elem->explosion_damage, sizeof(Value<INT16>));

			m_dummys.push_back(elem);
		}

		// SP
		parser.ReadSection(_T("SP"));
		for (INT32 i=1; i<=MAX_LEVEL_DOMI_RANGE; i++)
		{
			SP_ELEM* elem = new SP_ELEM;
			MEMDUMP_NEW( elem, sizeof(SP_ELEM));

			_snprintf(commonStr, 63, "MaxSP%d", i);
			parser.GetValue(commonStr, &commonVal);
			elem->max = new Value<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->max, sizeof(Value<UINT16>));

			_snprintf(commonStr, 63, "BasicSP%d", i);
			parser.GetValue(commonStr, &commonVal);
			elem->default = new Value<UINT16>(static_cast<UINT16>(commonVal));
			MEMDUMP_NEW( elem->default, sizeof(Value<UINT16>));

			m_sps.push_back(elem);
		}

		// USER SKILL
		char* skill[USER_SKILL_COUNT] = 
		{
			"HP",
			"Damage",
			"MoveSpeed",
			"Magazine",
			"WeaponEffectRatio",
			"ReloadSpeed",
			"GrenadeDamage",
			"Aggro",
			"GetSP",
			"MaxSP",
			"SpendSP",
			"GetDamage",
			"MeatBomb"
		};

		INT8 i8MaxSkillLevel[USER_SKILL_COUNT] = 
		{
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,

			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			MAX_LEVEL_USERSKILL,
			2,
			1
		};

		parser.ReadSection( _T("UserSkill"));

		m_userskills.reserve(USER_SKILL_COUNT);

		for (INT32 i=0; i<USER_SKILL_COUNT; i++)
		{
			USERSKILL_ELEM* elem = new USERSKILL_ELEM;
			MEMDUMP_NEW( elem, sizeof(USERSKILL_ELEM));

			elem->level_max = new Value<INT8>(i8MaxSkillLevel[i]);
			MEMDUMP_NEW( elem->level_max, sizeof(Value<INT8>));

			elem->cost_sp.reserve(MAX_LEVEL_USERSKILL+1);
			elem->mininum_round.reserve(MAX_LEVEL_USERSKILL+1);
			elem->incr_ratio.reserve(MAX_LEVEL_USERSKILL+1);

			WeightedValue<INT16> * cost_sp = new WeightedValue<INT16>(numeric_limits<INT16>::max());
			MEMDUMP_NEW( cost_sp, sizeof(WeightedValue<INT16>));
			elem->cost_sp.push_back(cost_sp);

			Value<INT16> * minimum_round = new Value<INT16>(numeric_limits<INT16>::max());
			MEMDUMP_NEW( minimum_round, sizeof( Value<INT16>));
			elem->mininum_round.push_back(minimum_round);

			Value<REAL32> * incr_ratio = new Value<REAL32>(1.0f);
			MEMDUMP_NEW( incr_ratio, sizeof( Value<REAL32>));
			elem->incr_ratio.push_back( incr_ratio);

			for (INT32 j=1; j<=MAX_LEVEL_USERSKILL; j++)
			{
				i3::snprintf(commonStr, 64, "%sLv%d_SpendSP", skill[i], j);
				parser.GetValue(commonStr, &commonVal);
				WeightedValue<INT16> * cost_sp2 = new WeightedValue<INT16>(static_cast<INT16>(commonVal));
				MEMDUMP_NEW(cost_sp2, sizeof(WeightedValue<INT16>));
				elem->cost_sp.push_back(cost_sp2);

				i3::snprintf(commonStr, 64, "%sLv%d_MinRound", skill[i], j);
				parser.GetValue(commonStr, &commonVal);

				Value<INT16> * minmum_round = new Value<INT16>(static_cast<INT16>(commonVal));
				MEMDUMP_NEW(minmum_round, sizeof(Value<INT16>));
				elem->mininum_round.push_back(minmum_round);

				i3::snprintf(commonStr, 64, "%sLv%d_IncreaseRatio", skill[i], j);
				parser.GetValue(commonStr, &commonVal);

				Value<REAL32> * incr_ratio2 = new Value<REAL32>(static_cast<REAL32>(commonVal/100.f + 1.0000001f));
				MEMDUMP_NEW(incr_ratio2, sizeof(Value<REAL32>));
				elem->incr_ratio.push_back( incr_ratio2);
			}

			m_userskills.push_back(elem);
		}

		m_isLoaded = true;
	}


	SUPPLYCOST_ELEM& SkillProp::GetSupplycost()
	{
		return m_supplycost;
	}

	SENTRYGUN_ELEM& SkillProp::GetSentrygun(const INT32 lv)
	{
		return *(m_sentryguns.at(lv));
	}

	DUMMY_ELEM& SkillProp::GetDummy(const INT32 lv)
	{
		return *(m_dummys.at(lv));
	}

	const SP_ELEM& SkillProp::GetSP(const INT32 round)
	{
		INT32 idx = 0;

		if (0 <= round && round <= 9) idx = 0;
		else if (10 <= round && round <= 19) idx = 1;
		else if (20 <= round && round <= 29) idx = 2;
		else if (30 <= round && round <= 39) idx = 3;
		else if (40 <= round && round <= 49) idx = 4;
		else idx = 5;

		return *(m_sps.at(idx));
	}

	USERSKILL_ELEM& SkillProp::GetUserSkill(const INT32 type)
	{
		return *(m_userskills.at(type));
	}


	SENTRYGUN_ELEM::SENTRYGUN_ELEM() : install_sp(nullptr), repair_sp(nullptr), max_hp(nullptr) {}
	SENTRYGUN_ELEM::~SENTRYGUN_ELEM()
	{
		I3_SAFE_DELETE( install_sp);
		I3_SAFE_DELETE( repair_sp);
		I3_SAFE_DELETE( max_hp);
	}

	DUMMY_ELEM::DUMMY_ELEM() : install_sp(nullptr), repair_sp(nullptr), max_hp(nullptr), explosion_damage(nullptr) {}
	DUMMY_ELEM::~DUMMY_ELEM()
	{
		I3_SAFE_DELETE( install_sp);
		I3_SAFE_DELETE( repair_sp);
		I3_SAFE_DELETE( max_hp);
		I3_SAFE_DELETE( explosion_damage);
	}

	SUPPLYCOST_ELEM::SUPPLYCOST_ELEM() : hpsupply_sp(nullptr), ammosupply_sp(nullptr) {}
	SUPPLYCOST_ELEM::~SUPPLYCOST_ELEM()
	{
		I3_SAFE_DELETE( hpsupply_sp);
		I3_SAFE_DELETE( ammosupply_sp);
	}

	SP_ELEM::SP_ELEM() : default(nullptr), max(nullptr) {}
	SP_ELEM::~SP_ELEM()
	{
		I3_SAFE_DELETE( default);
		I3_SAFE_DELETE( max);
	}

	USERSKILL_ELEM::USERSKILL_ELEM() : level_max(nullptr) {}
	USERSKILL_ELEM::~USERSKILL_ELEM()
	{
		I3_SAFE_DELETE( level_max);

		i3::cu::for_each_delete_clear(cost_sp);
		i3::cu::for_each_delete_clear(mininum_round);
		i3::cu::for_each_delete_clear(incr_ratio);
	}

	WeightedValue<INT16>& USERSKILL_ELEM::GetCostSp(const INT32 lv)
	{
		return *(cost_sp.at(lv)); 
	}

	INT16 USERSKILL_ELEM::GetMinRound(const INT32 lv) const 
	{ 
		return mininum_round.at(lv)->GetVal(); 
	}

	REAL32 USERSKILL_ELEM::GetIncrRatio(const INT32 lv) const 
	{ 
		return incr_ratio.at(lv)->GetVal(); 
	}

	UINT32 USERSKILL_ELEM::GetIncrPercentage(const INT32 lv) const 
	{ 
		REAL32 ratio = GetIncrRatio(lv); 

		// 음수일 경우 부동소수점 오차로 버그 발생.
		if ((ratio - 1.0f) < 0.0f)
			return static_cast<UINT32>(fabs((ratio - 1.0f) * 100.0f) + I3_EPS);

		return static_cast<UINT32>(fabs((ratio - 1.0f) * 100.0f));
	}
}