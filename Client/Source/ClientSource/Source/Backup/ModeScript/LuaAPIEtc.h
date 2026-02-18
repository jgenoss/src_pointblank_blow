#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIEtc
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnTimeOut( TIMER timerIdx );	// setTimer 함수에 의해 설정된 Timer가 Time-Out되었을 때 한번 호출된다. Time-Out이 된 Timer는 기본적으로 리셋이 되며, 동작하지 않는다.

	LVOID addKillScore( TEAM team, INT32 score );	// 지정된 팀의 Kill Count를 증가시킨다.
	INT32 getKillScore( TEAM team );				// 해당 팀의 Kill Count 점수를 반환한다.
	LVOID killRoundTimer( bool ShowUI );			// Round Timer를 종료한다.
	LVOID killTimer( INT32 idTimer, bool showUI );	// PB Engine이 제공하는 2개의 User-Timer를 해제한다.
	LVOID setRespawnDelayTime( INT32 secs );		// Respawn 대기 시간을 설정한다.
	LVOID setRoundTimer( INT32 secs );				// Round Time을 설정한다.
	LVOID setTimer( INT32 idTimer, INT32 secs );	// PB Engine이 제공하는 2개의 User Timer를 설정한다. 설정된 Timer는 자동으로 시간이 감소하며, 관련 UI를 표시한다. 각 Timer는 자신만의 UI를 가지고 있으며, 표현 형식은 고정되어 있다. 개별 Timer에 대한 자세한 설명은 아래의 Parameter 항목을 참조하기 바란다.
													// 각 Timer가 만료되었을 경우, OnTimeOut1, 또는 OnTimeOut2 Event Handler 함수들이 각각 호출된다.
};
