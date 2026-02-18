#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIUI
{
	void	Init(lua_State* L);

	//
	// Called : Client
	// 
	LVOID	hideKillScoreUI(void);			// Kill Count Score board를 사라지게 한다.
	LVOID	hideTeamScoreUI(void);			// Team Score Board를 사라지게 한다.
	LVOID	mapHighlight( PVEC3D pos );		// 해당 위치를 지도 상에서 Highlight 시킨다.
	LVOID	msgChat( const char* msg );		// 채팅 창에 System Message로 출력하는 함수.
	LVOID	msgHelp( const char* msg );		// 게임의 진행을 도와주는 메시지를 출력.이 함수로 출력되는 메시지의 위치는 전투 중이냐, 아니냐에 따라 다를 수 있다. 실제로 전투 진행 중에 이 함수는 메시지를 화면 중단에 표시한다. 그러나 여타 화면에서는 별도의 공간이 마련된 UI 영역에 표시될 수 있다.
	LVOID	msgPopup( const char* msg );	// Popup Message 창을 띄운다. User가 OK 버튼을 클릭해야 하는 UI.
	LVOID	showKillScoreUI(void);			// 화면 최상단의 Kill Count Score를 보이도록 설정한다. 이 UI는 항상 Kill Count로만 사용되며, Kill Counting은 Script가 아닌, Game Engine에서 자동으로 갱신된다.
	LVOID	showTeamScoreUI(void);			// Team Score Board를 나타나게 한다. Team Score는 finishRound 함수에 의해 승리한 회수가 자동으로 증가하고 표현된다.
};
