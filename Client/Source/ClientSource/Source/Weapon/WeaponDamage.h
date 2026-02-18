#pragma once

namespace weapon
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	extern bool s_critical_chance_100;
#endif

	namespace damage
	{
		enum TYPE 
		{ NORMAL, RANDOM, CRITICAL, MAX};

		class Normal
		{
		protected:
			CWeaponInfo* m_weapon_info;

		public:
			Normal( CWeaponInfo* info) : m_weapon_info(info) {}
			virtual ~Normal() {}
			virtual REAL32 operator()( REAL32 val, REAL32 add);
		};
		typedef i3::shared_ptr<Normal> Normal_ptr;

		class Random : public Normal
		{
		public:
			Random( CWeaponInfo* info) : Normal(info) {}
			REAL32 operator()( REAL32 val, REAL32 add);
		};

		class Critical : public Normal
		{
		public:
			Critical( CWeaponInfo* info) : Normal(info) {}
			REAL32 operator()( REAL32 val, REAL32 add);
		};
	}

	class Damage
	{
		WeaponBase* m_weapon;
		CWeaponInfo* m_weapon_info;
		damage::Normal_ptr m_damages[damage::MAX];
		damage::TYPE m_current_type;				// damage::NORMAL, damage::RANDOM
		bool m_is_critical;

		INT8* m_critical_table;
	public:
		Damage( WeaponBase* _weapon);
		~Damage();

		REAL32 Get( REAL32 val, REAL32 add, bool random);
		bool CheckCriticalDamage();
		REAL32 GetCriticalDamage( REAL32 _damage);

		void SetCriticalDamageForClient( UINT8 val) { m_is_critical = (val != 0);}
		bool IsCritical() const { return m_is_critical;}
	};
};