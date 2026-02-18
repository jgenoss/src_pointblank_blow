#if !defined( __MOD_SCRIPT_GLUE_H)
#define __MOD_SCRIPT_GLUE_H

namespace MODScript
{
	bool		Init(void);
	
	/////////////////////////////////////
	// Stage 관련 함수
	extern "C"	int		finishRound( lua_State * pState);
	extern "C"	int		getConfigRoundCount( lua_State * pState);
	extern "C"	int		getConfigTime( lua_State * pState);
	extern "C"	int		setMODName( lua_State * pState);
	extern "C"	int		setPlayerCount( lua_State * pState);
	extern "C"	int		setRankCondition( lua_State * pState);
	extern "C"	int		setRoundCount( lua_State * pState);


	/////////////////////////////////////
	// Player 관련 함수
	extern "C"	int		addEXP( lua_State * pState);
	extern "C"	int		addPoint( lua_State * pState);
	extern "C"	int		addTempWeapon( lua_State * pState);
	extern "C"	int		disableRespawn( lua_State * pState);
	extern "C"	int		disableWeapon( lua_State * pState);
	extern "C"	int		enableRespawn( lua_State * pState);
	extern "C"	int		enableWeapon( lua_State * pState);
	extern "C"	int		getAlivePlayerCount( lua_State * pState);
	extern "C"	int		getKiller( lua_State * pState);
	extern "C"	int		getPlayerClan( lua_State * pState);
	extern "C"	int		getPlayerHP( lua_State * pState);
	extern "C"	int		getPlayerName( lua_State * pState);
	extern "C"	int		getPlayerPoint( lua_State * pState);
	extern "C"	int		getPlayerPos( lua_State * pState);
	extern "C"	int		getPlayerRank( lua_State * pState);
	extern "C"	int		getPlayerWeapon( lua_State * pState);
	extern "C"	int		getTeam( lua_State * pState);
	extern "C"	int		setRole( lua_State * pState);

	/////////////////////////////////////
	// Object 관련 함수
	extern "C"	int		getObject( lua_State * pState);
	extern "C"	int		getObjectHP( lua_State * pState);
	extern "C"	int		getObjectName( lua_State * pState);


	/////////////////////////////////////
	// UI 관련 함수
	extern "C"	int		showKillScoreUI( lua_State * pState);
	extern "C"	int		hideKillScoreUI( lua_State * pState);
	extern "C"	int		showTeamScoreUI( lua_State * pState);
	extern "C"	int		hideTeamScoreUI( lua_State * pState);
	extern "C"	int		mapHighlight( lua_State * pState);
	extern "C"	int		msgChar( lua_State * pState);
	extern "C"	int		msgHelp( lua_State * pState);
	extern "C"	int		msgPopup( lua_State * pState);
	
	/////////////////////////////////////
	// 기타 함수
	extern "C"	int		addScore( lua_State * pState);
	extern "C"	int		getKillScore( lua_State * pState);
	extern "C"	int		setRoundTimer( lua_State * pState);
	extern "C"	int		killRoundTimer( lua_State * pState);
	extern "C"	int		setTimer( lua_State * pState);
	extern "C"	int		killTimer( lua_State * pState);
	extern "C"	int		setResapwnDelayTime( lua_State * pState);
	
};

#endif
