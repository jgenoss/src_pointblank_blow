#pragma once
//이 파일은 GameContext 클래스에서 분리 가능한 함수들을 위해서 만듬.

namespace GameContextUtil {} 

namespace gcu = GameContextUtil;

class CUserLocationInfo;

namespace GameContextUtil
{
	bool	IsSameRoom(const CUserLocationInfo & UserLocInfo);

	i3::rc_wstring	GetRadioMessage( RADIOCHAT_TYPE nType, INT32 nMsgNum );
	i3SoundPlayState * PlayRadioMessageSound( RADIOCHAT_TYPE nType, INT32 nMsgNum, CGameCharaBase * pPlayer);

	//QA팀 요청으로 만든 테스트용 명령문.. 게임 서비스시에는 동작하면 않됨
	//1. 계급 조정 커맨드 
	//2. 피노 조정 
	//3. 경험치 조정 
	//10. 라운드 조정 커맨드 
	//11. 킬수 조정 커맨드 
	//12. 끝없는 배틀 
	void	QA_SetRank(UINT32 rank); 
	void	QA_SetPino(UINT32 Pino); 
	void	QA_SetChallenge(void); 
	void	QA_SetExp(UINT32 Exp); 
	void	QA_SetCash(UINT32 Cash); 
	void	QA_SetRound(UINT8 Red, UINT8 Blue); 
	void	QA_SetKillCount(UINT16 Red, UINT16 Blue); 
	void	QA_SetEndlessBattle(void); 
	void	QA_SetClanRank(UINT32 Pino);	//Clan RankUp
	void	QA_SetClanExp(UINT32 Pino);	//Clan Exp
	void	QA_SetClanMatchPoint( UINT32 Pino);
	void	QA_SetClanMatchWinLose( UINT32 wMatch, UINT32 wWin, UINT32 wLose );
	void	QA_SetClanMatchRoomInsert( UINT32 wCount );
	void	QA_SetMedalCount(UINT8 Type, UINT8 Number, UINT8 Count); 

	// UI팀 요청으로 미니맵 위치 조정 커맨드
#ifdef UITEST_MINIMAP_MOVE
	void	UITest_SetMinimapMove(bool enable);
	bool	UITest_IsMinimapMoveEnable();
#endif

	// debug
	void	CheckWeaponList( const ITEM_INFO * pList );

	INT32	getKillRate(UINT32 iKill, UINT32 iDeath);
	
}
