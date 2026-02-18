#include "pch.h"
#include "FullbodyBiStateMapFactory.h"

using namespace fb_fsm;

//--------------------------------------------------------------
// BiStateMap
//--------------------------------------------------------------
BiStateMap::BiStateMap()
{
	m_bistates.clear();
}

BiStateMap::~BiStateMap()
{
	m_bistates.clear();
}

INT32 BiStateMap::Find(fb_fsm::BiStateKey key)
{
	BiStateKeyMap::const_iterator it = m_bistates.find(key);
	return (it != m_bistates.end()) ? it->second : 1;
}

INT32 BiStateMap::Find(UINT8 st, UINT8 subSt)
{
	return Find(fb_fsm::BiStateKey(st, subSt));
}

fb_fsm::BiStateKey BiStateMap::Find(INT32 id)
{
	BiStateKeyMap::const_iterator it;
	for (it = m_bistates.begin(); it != m_bistates.end(); it++)
	{
		if (id == it->second)
			return it->first;
	}

	return fb_fsm::BiStateKey(0, 0);
}

void BiStateMap::Insert(fb_fsm::BiStateKey key, INT32 val)
{
	m_bistates.insert(BiStateKeyMap::value_type(key, val));
}


//--------------------------------------------------------------
// BiStateMap ÆÑÅä¸®
//--------------------------------------------------------------
BiStateMap* BiStateMapFactory::CreateMapDomiAcid()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_acid;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_ING),			BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_SOONBOOM),							BOOMING);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_ACID_SEEK, 0),											LOOKAROUND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiAcidNotBomb()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_acid_not_bomb;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_FRONT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_ING),			BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_ACID_SEEK, 0),											LOOKAROUND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiAcidItem()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_acid_item;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiAcidVulcan()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_acid_vulcan;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							SUICIDE_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_ING),			BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_ACID_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_SOONBOOM),							BOOMING);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_ACID_SEEK, 0),											LOOKAROUND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiMutantRex()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_mutantrex;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_STAGEEND);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_STAGEEND);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_STAGEEND);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_BITE_LOW<<4 | DINO_ATTACK_STATE_ING),				ATTACK_BOTTOM);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_BITE_LOW<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_BITE_MID<<4 | DINO_ATTACK_STATE_ING),				ATTACK_TOP);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_BITE_MID<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_CHOP<<4 | DINO_ATTACK_STATE_ING),					STOMP);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_MUTANTTREX_CHOP<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),		IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, 5),												HOWLING);
	bist->Insert(BiStateKey(DINO_STATE_TREX_HOWL_FAIL, 0),										DAMAGED_BY_DEATHBLOW);
	bist->Insert(BiStateKey(DINO_STATE_TREX_READY, 0),											RUNATTACK_READY);
	bist->Insert(BiStateKey(DINO_STATE_TREX_DASH, 0),											RUNATTACK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUNATTACK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKBACK);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											ROAR);
	bist->Insert(BiStateKey(DINO_STATE_TREX_HOWL, 0),											HOWLING);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;

}

BiStateMap* BiStateMapFactory::CreateMapDomiRaptor()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_raptor;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_MID1);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_ING),				BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_CLAW<<4 | DINO_ATTACK_STATE_ING),				JUMPATTACK);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_CLAW<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_READY),				LEAP_READY_BEND_OVER);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_LEAP_UP),				LEAP_START);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_LEAP_DOWN),			LEAP);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_FINISH),				LEAP_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_FAIL),					LEAP_FAIL);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_RAPTOR_FEED, 0),											FORAGE);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiRaptorMercury()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_raptor_mercury;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_MID1);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_ING),				BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_CLAW<<4 | DINO_ATTACK_STATE_ING),				JUMPATTACK);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_CLAW<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_READY),				LEAP_READY_BEND_OVER);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_LEAP_UP),				LEAP_START);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_LEAP_DOWN),			LEAP);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_FINISH),				LEAP_ATTACK);
	bist->Insert(BiStateKey(DINO_STATE_RAPTORLEAP, DINO_RAPTORLEAP_STATE_FAIL),					LEAP_FAIL);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_RAPTOR_FEED, 0),											FORAGE);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiSting()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_sting;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_MID1);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_SPIT<<4 | DINO_ATTACK_STATE_ING),				SPIT);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_SPIT<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),		IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_GASBAG<<4 | DINO_ATTACK_STATE_ING),				POISON_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_GASBAG<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_STING_SHAKE, 0),											LOOKAROUND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiStingMars()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_sting_mars;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_MID1);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_SPIT<<4 | DINO_ATTACK_STATE_ING),				SPIT);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_SPIT<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),		IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_GASBAG<<4 | DINO_ATTACK_STATE_ING),				POISON_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_STING_GASBAG<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_START),						JUMPSTART);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_UP),							JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_DOWN),						JUMPING);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_JUMP_END),							JUMPEND);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											WARNKILL);
	bist->Insert(BiStateKey(DINO_STATE_STING_SHAKE, 0),											LOOKAROUND);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}

BiStateMap* BiStateMapFactory::CreateMapDomiTank()
{
	BiStateMap* bist = new BiStateMap;
	MEMDUMP_NEW( bist, sizeof( BiStateMap));

	using namespace fb_fsm::domi_tank;
	bist->Insert(BiStateKey(DINO_STATE_NONE, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_NORMAL),							DEATH_MID1);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_HEADSHOT),							DEATH_BY_HEADSHOT);
	bist->Insert(BiStateKey(DINO_STATE_DEAD, DINO_DEAD_STATE_GRENADE),							DEATH_BY_GRENADE);
	bist->Insert(BiStateKey(DINO_STATE_FREEZE, 0),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 0),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 1),												NONE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 2),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_STOP, 3),												IDLE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_ING),				BITE);
	bist->Insert(BiStateKey(DINO_STATE_ATTACK, DINO_ATTACK_WEAPON_RAPTOR_BITE<<4 | DINO_ATTACK_STATE_WAIT_COOLTIME),	IDLE);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_RUN),								RUN);
	bist->Insert(BiStateKey(DINO_STATE_MOVE, DINO_MOVE_STATE_WALK),								WALK);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_LOW),								DAMAGE_LOW);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_MIDDLE),							DAMAGE_MID);
	bist->Insert(BiStateKey(DINO_STATE_PAIN, DINO_PAIN_STATE_HIGH),								DAMAGE_HIGH);
	bist->Insert(BiStateKey(DINO_STATE_S_KNOCKDOWN, 0),											KNOCKDOWN);
	bist->Insert(BiStateKey(DINO_STATE_S_STIFFEN, 0),											GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_S_BLIND, 0),												GROGGY);
	bist->Insert(BiStateKey(DINO_STATE_TANKDASH, DINO_TANKDASH_STATE_READY),					RUSH_READY_BEND_OVER);
	bist->Insert(BiStateKey(DINO_STATE_TANKDASH, DINO_TANKDASH_STATE_RUNNING),					RUSH);
	bist->Insert(BiStateKey(DINO_STATE_TANKDASH, DINO_TANKDASH_STATE_FINISH),					RUSH_FINISH);
	bist->Insert(BiStateKey(DINO_STATE_ROAR, 0),												ROAR);
	bist->Insert(BiStateKey(DINO_STATE_SUCCESSKILL, 0),											ROAR);
	bist->Insert(BiStateKey(DINO_STATE_TANK_HOWL, 0),											HOWL);
	bist->Insert(BiStateKey(DINO_STATE_S_SLOW, 0),												WALK);

	return bist;
}