#ifndef _FULLBODY_DEF_H_
#define _FULLBODY_DEF_H_

class CGameCharaFullbody;

namespace fb_fsm
{
	typedef struct BiStateKey
	{
		UINT8 state;
		UINT8 subState;

		BiStateKey(UINT8 _state, UINT8 _subState)
		{
			state = _state;
			subState = _subState;
		}		
	};
}

template<>
struct i3::hash<fb_fsm::BiStateKey>
{
	size_t operator()(const fb_fsm::BiStateKey& k) const
	{
		return reinterpret_cast<const UINT16&>(k);
	}
};

namespace fb_fsm
{
	inline bool operator == (const BiStateKey& lhs, const BiStateKey& rhs)
	{
		if (lhs.state == rhs.state && lhs.subState == rhs.subState) return true;

		return false;
	}

	template<class B, class D, INT32 START_ANIM, INT32 NEXT_ANIM>
	struct static_creator 
	{
		static B* call(CGameCharaFullbody* chara, INT32 id) 
		{ 
			B* p = new D(chara, id, START_ANIM, NEXT_ANIM);
			MEMDUMP_NEW( p, sizeof( D));
			return p;
		}	
	};

#ifdef I3_DEBUG
	//#define ENABLE_FB_FSM_TRACE
#endif

	#define REPEAT -1	// 에디터로 설정한 반복 애니메이션 표시 위해
	namespace domi_acid
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			LOOKAROUND,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			BITE,
			SUICIDE_ATTACK_READY,
			SUICIDE_ATTACK,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			GROGGY,
			BOOMING,

			AI_MAX,
		};
	}

	namespace domi_acid_not_bomb
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			LOOKAROUND,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_FRONT,
			BITE,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			GROGGY,

			AI_MAX,
		};
	}

	namespace domi_acid_item
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			WALK,
			RUN,
			SUICIDE_ATTACK,
			JUMPSTART,
			JUMPING,
			JUMPEND,

			AI_MAX,
		};
	}

	namespace domi_acid_vulcan
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			LOOKAROUND,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			BITE,
			SUICIDE_ATTACK_READY,
			SUICIDE_ATTACK,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			GROGGY,
			BOOMING,

			AI_MAX,
		};
	}

	namespace domi_mutantrex
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			WALK,
			WALK_WITH_LIMP,
			DAMAGE,
			DAMAGED_BY_DEATHBLOW,
			HOWLING_HITTED,
			GROGGY,
			KNOCKBACK,
			DEATH_STAGEEND,
			ATTACK_BOTTOM,
			ATTACK_TOP,
			RUNATTACK_READY,
			RUNATTACK,
			STOMP,
			HOWLING,
			ROAR,

			AI_MAX,
		};
	}

	namespace domi_raptor
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			FORAGE,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_MID1,
			DEATH_MID2,
			DEATH_MID3,
			BITE,
			JUMPATTACK,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			LEAP_READY_BEND_OVER,
			LEAP_READY_QUAKING,
			LEAP_START,
			LEAP,
			LEAP_ATTACK,
			LEAP_FAIL,
			GROGGY,

			AI_MAX,
		};
	}

	namespace domi_raptor_mercury
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			FORAGE,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_MID1,
			DEATH_MID2,
			DEATH_MID3,
			BITE,
			JUMPATTACK,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			LEAP_READY_BEND_OVER,
			LEAP_READY_QUAKING,
			LEAP_START,
			LEAP,
			LEAP_ATTACK,
			LEAP_FAIL,
			GROGGY,

			AI_MAX,
		};
	}


	namespace domi_sting
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			LOOKAROUND,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_MID1,
			DEATH_MID2,
			DEATH_MID3,
			SPIT,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			POISON_GRENADE,
			GROGGY,

			AI_MAX,
		};
	}

	namespace domi_sting_mars
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			LOOKAROUND,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_MID1,
			DEATH_MID2,
			DEATH_MID3,
			SPIT,
			JUMPSTART,
			JUMPING,
			JUMPEND,
			WARNKILL,
			ROAR,
			POISON_GRENADE,
			GROGGY,

			AI_MAX,
		};
	}

	namespace domi_tank
	{
		enum STATE_TYPE
		{
			NONE,
			IDLE,
			ROAR,
			WALK,
			RUN,
			DAMAGE_MID,
			DAMAGE_HIGH,
			DAMAGE_LOW,
			KNOCKDOWN,
			STANDUP,
			DEATH_BY_GRENADE,
			DEATH_BY_HEADSHOT,
			DEATH_MID1,
			DEATH_MID2,
			DEATH_MID3,
			BITE,
			HOWL,
			RUSH_READY_BEND_OVER,
			RUSH_READY_QUAKING,
			RUSH,
			RUSH_FINISH,
			GROGGY,

			AI_MAX,
		};
	}
}

#endif