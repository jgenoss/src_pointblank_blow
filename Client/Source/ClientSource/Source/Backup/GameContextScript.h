#if !defined (__GAME_CONTEXT_SCRIPT_H__)
#define __GAME_CONTEXT_SCRIPT_H__

#if defined( USE_LUA_UI)

#include "GameContext.h"

namespace GameContextScript
{
	extern "C"	int		Init(lua_State * L);
				bool	InitGameDefine(lua_State *L);
				void	OnGameEvent(i3UIScene * pUIScene, INT32 nEvent,INT32 arg);

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	//extern "C"	int		OnGameEvent(

	extern "C"	int		setEventOnce(lua_State *L);
	/////////////////////////////////////////////////////////////////////////////////////////
	// Section Lobby Room
	/////////////////////////////////////////////////////////////////////////////////////////
	extern "C"	int		getLobbyRoomCount( lua_State * L);
	extern "C"	int		getLobbyRoomInfo( lua_State * L);

	// Lobby - set selected room
	extern "C"	int		setLobbyRoomSelect(lua_State *L);				// 해당 인덱스의 방을 선택 상태로 설정

	// Lobby-common string
	extern "C"	int		getLobbyRoomNumber_s(lua_State *L);				// 방번호: todo: 스트링에 포맷 입히는거 확인해야해서..일단 그냥 받음
	extern "C"	int		getLobbyRoomMapName_s(lua_State *L);			// 맵이름: 예) "아웃포스트"
	extern "C"	int		getLobbyRoomMode_s( lua_State * L);				// 게임모드: 예) "파괴모드"
	extern "C"	int		getLobbyRoomName_s( lua_State * L);				// 방제목: 예) "니가 짱입니다요"

	extern "C"	int		getLobbyRoomProgressStatus_s(lua_State *L);		// 진행상황: 예) "게임중/대기중"
	extern "C"	int		getLobbyRoomPingStatus(lua_State *L);			// ping상태: 예) 4
	extern "C"	int		getLobbyRoomPlayerStatus(lua_State *L);			// 인원수: 예) 02/16

	extern "C"	int		getLobbySelectedConfig_s( lua_State * L);		// 게임설정: 예) "100kill/10분"
	extern "C"	int		getLobbySelectedRoomMaster_s(lua_State * L);	// 방장이름: 예) "골플레이어"
	extern "C"	int		getLobbySelectedRoomTimeStatus_s(lua_State * L);// 진행상황: 예) "2:37"
	
	extern "C"	int		getLobbySelectedRoomLimitInterEnter_s(lua_State * L);	// 난입제한: 예) "없음"
	extern "C"	int		getLobbySelectedRoomLimitWeapon_s(lua_State * L);		// 무기제한: 예) "있음"
	extern "C"	int		getLobbySelectedRoomObserver_s(lua_State * L);			// 관전기능: 예) "사용함"
	extern "C"	int		getLobbySelectedRoomTeamBalance_s(lua_State * L);		// 팀밸런스: 예) "제한"

	/////////////////////////////////////////////////////////////////////////////////////////
	// Section Lobby User
	/////////////////////////////////////////////////////////////////////////////////////////
	extern "C"	int		getLobbyUserCount( lua_State *L);
	extern "C"	int		getLobbyUserInfo( lua_State *L);

	extern "C"	int		getLobbyUserNick( lua_State *L);
	extern "C"	int		getLobbyUserClanMark( lua_State *L);
	extern "C"	int		getLobbyUserRank( lua_State *L);
	extern "C"	int		getLobbyUserEsportsID( lua_State *L);
};
#endif	//#if defined( USE_LUA_UI)
#endif