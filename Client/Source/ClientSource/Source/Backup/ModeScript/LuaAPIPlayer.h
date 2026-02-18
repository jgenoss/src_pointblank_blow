#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIPlayer
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID	OnDamagePlayer( HPLAYER player, REAL32 damage );			// Player가 공격을 당해 데미지를 입은 경우 호출된다. 공격을 한 Player에 대해서는 호출되지 않는다.
	LVOID	OnEnterTrigger( HPLAYER player, HOBJECT trigger );		// Player가 Trigger에 진입했을 때, 한번 호출된다.
	bool	OnInstall( HPLAYER player, HWEAPON weapon );				// Player가 미션 아이템 및 무기 등을 설치했을 때 호출된다.
	LVOID	OnIntrudePlayer( HPLAYER player );						// 새로운 Player가 해당 방에 난입했을 때 호출되는 함수. 난입의 경우에만 호출되며, 처음 방을 함께 시작한 경우의 Player들은 여기서 호출되지 않는다. 이 함수로 전달된 Player는 이미 난입이 허용된 사람이기 때문에 더 이상 난입 불가 및 조정이 불가능하다.
	LVOID	OnKillPlayer( HPLAYER player );							// Player가 죽은 경우에 호출된다. 죽음 자체는 결정된 것이기 때문에 이 함수를 통해 번복될 수 없다.
	LVOID	OnLeaveTrigger( HPLAYER player, HOBJECT trigger );		// Player가 Trigger를 벗어날 때, 한번 호출된다.
	bool	OnObtainWeapon ( HPLAYER player, HWEAPON weapon );			// Player가 버려진 무기를 습득했을 때 호출된다. 설치 무기를 습득했을 때에도 호출된다.
	LVOID	OnStayTrigger( HPLAYER player, HOBJECT trigger );			// Player가 Trigger에 머물고 있는 동안 매 Frame마다 호출된다.
	LVOID	OnThrowAwayWeapon ( HPLAYER player, HWEAPON weapon );		// Player가 무기를 버렸을 때 호출된다. 설치 무기를 버리는 경우에도 호출된다.
	bool	OnUninstall( HPLAYER player, HWEAPON weapon );				// Player가 미션 아이템 및 무기 등을 해체했을 때 호출된다.


	LVOID	addEXP( HPLAYER player, INT32 exp );				// 지정 Player에게 경험치를 지급하는 함수.
	LVOID	addPoint( HPLAYER player, INT32 exp );				// 지정 Player에게 Point를 지급하는 함수.
	LVOID	addTempWeapon( HPLAYER player, WEAPON_ID weapon );	// 해당 Round를 위해 임시로 특정 무기를 지급해주는 함수. 지급된 무기는 Round가 종료될 때 사라진다.
	LVOID	disableRespawn( HPLAYER player );					// 지정된 Player의 Respawn을 불가능하도록 설정한다. 불가능하게 설정된 Player는 관전 상태로 있으며, 이것은 enableRespawn 함수를 통해 활성화될 때까지 지속된다.
	LVOID	disableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot );	// 해당 Player의 특정 무기류를 사용 불가능하도록 설정한다.
	LVOID	enableRespawn( HPLAYER player );					// 지정 Player의 Respawn을 가능하도록 설정한다. Respawn이 불가능하도록 설정된 Player는 관전만 가능하며 Respawn 단계로 넘어가지 않는다.
	LVOID	enableWeapon( HPLAYER player, WEAPON_USAGE_TYPE slot );	// 해당 Player의 특정 무기류를 사용 가능하도록 설정한다.
	INT32	getAlivePlayerCount( TEAM team );					// 지정 팀의 생존 Player 수를 얻어온다.
	HPLAYER getKiller( HPLAYER player );						// 지정된 player를 죽인 상대 Player를 반환한다.
	CLAN_ID GetPlayerClan( HPLAYER player );					// Player가 속한 Clan의 ID.
	INT32	getPlayerHP( HPLAYER player );						// 특정 Player의 HP를 구해온다.
	const char* GetPlayerName( HPLAYER player );				// 특정 Player의 이름을 반환한다.
	INT32	GetPlayerPoint( HPLAYER player );					// Player의 Point를 반환한다.
	PVEC3D	getPlayerPos( HPLAYER player );						// 지정 Player의 현재 위치를 반환합니다.
	RANK	GetPlayerRank( HPLAYER player );					// Player의 현재 계급
	HWEAPON GetPlayerWeapon( HPLAYER player, WEAPON_USAGE_TYPE usage );// Player의 현재 주무기, 보조무기 등을 얻어올 수 있다.
	TEAM	getTeam( HPLAYER player );							// Player의 팀( RED 또는 BLUE)을 반환한다.
	LVOID	setRole( HPLAYER player, ROLE role );				// 특수한 형태의 MOD를 위해 사용되는 것으로,각 Player에게 특정 역할을 부여하는데 사용되는 함수.													// 각 Timer가 만료되었을 경우, OnTimeOut1, 또는 OnTimeOut2 Event Handler 함수들이 각각 호출된다.
};
