#include "pch.h"
#include "WeaponDamage.h"


namespace weapon
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	bool s_critical_chance_100 = false;
#endif

	namespace damage
	{
		REAL32 Normal::operator()( REAL32 val, REAL32 add) { return val;}

		REAL32 Random::operator()( REAL32 val, REAL32 add)
		{
			REAL32 range = (m_weapon_info->GetMaxDamage() + add - val);
			REAL32 rv = val;
			if( range > 0.0f)
			{				
				rv = val + (range * i3Math::Randf());
			}
			return rv;
		}

		REAL32 Critical::operator()( REAL32 val, REAL32 add)
		{ 
			//데미지 증가, (Hited User)방어력 증가 Item 적용후 CriticalDamage 적용.
			return val;/*m_weapon_info->GetCriticalDamageRate();*/
		}
	}

	Damage::Damage( WeaponBase* _weapon) : m_weapon(_weapon), m_is_critical(false), m_critical_table(nullptr)
	{
		m_weapon_info = _weapon->getWeaponInfo();
		INT32 _count = m_weapon_info->GetCriticalDamageChance();
		if( _count > 0)
		{
			m_critical_table = new INT8[_count];
			ZeroMemory( m_critical_table, _count);
			m_critical_table[0] = 1;

			std::random_shuffle( m_critical_table, m_critical_table + (_count - 1));
		}

		m_damages[damage::NORMAL]		= damage::Normal_ptr( new damage::Normal(m_weapon_info));
		m_damages[damage::RANDOM]		= damage::Normal_ptr( new damage::Random(m_weapon_info));
		m_damages[damage::CRITICAL]		= damage::Normal_ptr( new damage::Critical(m_weapon_info));
	}

	Damage::~Damage()
	{
		if( m_critical_table)
		{
			delete[] m_critical_table; 
			m_critical_table = nullptr;
		}
	}

	REAL32 Damage::Get(REAL32 val, REAL32 add, bool random)
	{
		//1 critical 은 hit시 UDP 패킷을 보내기 전 결정 합니다. --> CheckCriticalDamage()

		//2 random 은 외부에서 결정.

		//3. normal

		m_current_type = random ? damage::RANDOM : damage::NORMAL;
		return (*m_damages[m_current_type])( val, add);
	}

	bool Damage::CheckCriticalDamage()
	{
		if( m_critical_table)
		{
			CGameCharaBase* owner = m_weapon->getOwner();
			if( owner)
			{
				INT32 _count = m_weapon_info->GetCriticalDamageChance();
				INT32 _hit_count = owner->GetHitCount();

				if( _hit_count >= 0 && _hit_count < _count)
				{
					m_is_critical = (m_critical_table[_hit_count] == 1);
				}

				_hit_count++;
				if( _hit_count >= _count)
				{
					_hit_count = 0;
					std::random_shuffle( m_critical_table, m_critical_table + (_count - 1));
				}
				owner->SetHitCount(_hit_count);
			}
		}
		return m_is_critical;
	}

	REAL32 Damage::GetCriticalDamage( REAL32 _damage)
	{
		if( m_is_critical)
		{
			REAL32 val = _damage * m_weapon_info->GetCriticalDamageRate();
			return (val >= _damage) ? val : _damage;
		}
		return _damage;
	}
}

