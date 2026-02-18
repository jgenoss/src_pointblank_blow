#include "pch.h"
#include "MODScriptSys_Glues.h"

extern "C"
{

//
// Prototype 
//		void addKillScore( TEAM team, INT32 score); 
// 
// Remark
//		지정된 팀의 Kill Count를 증가시킨다. 
// 
// Parameters
//		team		Kill Count를 증가시킬 Team. 
//		score		증가 점수. 일반적으로 1이 주어진다. 음수도 사용 가능하다. 
// 
// Return
//		없음.
// 
int MODScript::addScore( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		INT32 getKillScore( TEAM team); 
// 
// Remark
//		해당 팀의 Kill Count 점수를 반환한다. 
// 
// Parameters
//		team		Kill Count를 얻고자 하는 팀. 
// 
// Return 
//		해당 팀의 Kill Count. 
//
int MODScript::getKillScore( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		void setRoundTimer( INT32 secs) 
//  
// Remark
//		Round Time을 설정한다. 
// 
// Parameters
//		secs		설정할 Round Time을 초 단위로 설정. 
// 
// Return
//		없음. 
//
int MODScript::setRoundTimer( lua_State * pState)
{
	return 0;
}

// 
// Prototype
//		void killRoundTimer( bool ShowUI)
//  
// Remark
//		Round Timer를 종료한다.
// 
// Parameters
//		ShowUI		Round Time Board를 표시할 것인지, 또는 사라지게 할 것인지 설정.
//					true인 경우 UI가 계속 나타나 있으며, false인 경우 사라진다.
// 
// Return
//		없음. 
//
int MODScript::killRoundTimer( lua_State * pState)
{
	return 0;
}

int MODScript::setTimer( lua_State * pState)
{
	return 0;
}

int MODScript::killTimer( lua_State * pState)
{
	return 0;
}

int MODScript::setResapwnDelayTime( lua_State * pState)
{
	return 0;
}


}; // extern "C"