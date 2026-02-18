#if !defined( __QUEST_BASE_MAP_COUNT_ACCM_TEAM_H)
#define __QUEST_BASE_MAP_COUNT_ACCM_TEAM_H

#include "QuestBase.h"

class CQuestBase_Map_Count_Accm_Team : public CQuestBase
{
	I3_CLASS_DEFINE( CQuestBase_Map_Count_Accm_Team, CQuestBase);
protected:
	UINT8				m_nValidKillCount;							//누적 액션에 누적 카운트가 필요한경우(double/triple kill)임무시 내부적으로 kill을 카운트한다.

	ACTION_ID		_getAction(void)						{	return (ACTION_ID) getArg( 0); }
	void			_setAction( ACTION_ID action)			{	setArg( 0, (UINT8)action); }

	STAGE_ID		_getStage(void)							{	return (STAGE_ID) getQuestStageID( getArg( 1)); }
	//void			_setStage( STAGE_ID stage)				{	setArg( 1, (UINT8) stage); }

	UINT8			_getCount(void)							{	return getArg( 2);	}
	void			_setCount( UINT8 cnt)					{	setArg( 2, cnt); }

	bool			_isAccmQuest(void)						{	return getArg( 3) != 0; }
	void			_setAccmQuest( bool bFlag)				{	if( bFlag)	setArg( 3, 1);	else	setArg( 3, 0);	}

	UINT8			_getTeam(void)							{	return getArg( 4); }
	void			_setTeam( UINT8 team)					{	setArg( 4, team); }

	bool			_checkCondition(void);

	void			_setValidKillCount( UINT8 cnt)				{	m_nValidKillCount = cnt;}
	UINT8			_getValidKillCount(void)					{	return m_nValidKillCount;}

public:
	CQuestBase_Map_Count_Accm_Team(void);
	virtual ~CQuestBase_Map_Count_Accm_Team(void);

	// 주인공으로 인해, 다른 캐릭터가 죽었을 때 호출된다.
	// Kill Chara.에 대한 구체적인 정보는 KILL_INFO Structure에 저장되어 있다.
	virtual bool		OnKill( KILL_INFO * pKillInfo);

	// 다른 캐릭터에 의해 주인공 캐릭터가 죽임을 당했을 때에 호출된다.
	// Kill Chara에 대한 구체적인 정보는 KILL_INFO Structure를 통해 전달된다.
	virtual bool		OnDeath( KILL_INFO * pDeathInfo);

	// 케릭터가 공격을 당했을 때 호출된다.
	// 공격 내용에 대한 구체적인 정보는 DAMAGE_INFO Structure를 통해 전달된다.
	virtual bool		OnDamage( DAMAGE_INFO * pDamgeInfo);

	// 새롭게 Stage를 시작했을 때 호출된다.
	// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
	// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
	virtual bool		OnStageStart( STAGE_INFO * pStage);

	// Stage를 종료할 때 호출된다.
	// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
	// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
	virtual bool		OnStageEnd( STAGE_INFO * pStageInfo);

	// 새롭게 Round를 시작했을 때 호출된다.
	// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
	// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
	virtual bool		OnRoundStart( STAGE_INFO * pStageInfo);

	// Round를 종료할 때 호출된다.
	// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
	// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
	virtual bool		OnRoundEnd( STAGE_INFO * pStageInfo);

	// 주인공 캐릭터가 특정 동작을 취했을 때 호출된다.
	// 동작에 대한 구체적인 정보는 ACTION_INFO Structure를 통해 전달된다.
	virtual bool		OnAction( ACTION_INFO * pAction);

	// 화면에 출력할 Mission에 대한 설명문을 요구할 때 호출된다.
	// CQuestBase Class에서 파생된 개별 Mission Class들은
	// 반드시 이 함수를 Override하여 적절한 형식의 문자열을 제공해야 한다.
	virtual void		OnQueryDesc( char * pszBuff, INT32 maxLen);

	virtual void		OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed = true);

	// Quest의 진행 상태를 반환한다.
	virtual QUEST_STATE	OnQueryState(void);

	// Quest강제완료
	virtual	bool		ForceComplete(void);
};

#endif
