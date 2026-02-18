#if !defined( __QUEST_BASE_H)
#define __QUEST_BASE_H

enum QUEST_STATE
{
	QST_NONE = -1,
	QST_NOT_STARTED = 0,
	QST_DOING,
	QST_COMPLETED
};

class CQuestBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CQuestBase, i3ElementBase);
protected:
	QUEST_ID				m_ID;
	UINT8					m_Arg[ MAX_QUEST_ARG];

protected:
	void					getProgressColor( INT32 current, INT32 maxCount, COLOR * pColor);
public:
	CQuestBase(void);
	virtual ~CQuestBase(void);

	QUEST_ID			getID(void)							{ return m_ID; }
	void				setID( QUEST_ID id)				{ m_ID = id; }

	UINT8				getArg( INT32 idx)					{ I3ASSERT( (idx >= 0) && (idx < MAX_QUEST_ARG));		return m_Arg[ idx]; }
	void				setArg( INT32 idx, UINT8 val)		{ I3ASSERT( (idx >= 0) && (idx < MAX_QUEST_ARG));		m_Arg[ idx] = val; }

	virtual void		OnBind( const QUEST_CTX * pCtx);

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

	// HUD 상에서 출력할 Quest의 진행상태 문자열을 반환한다.
	virtual void		OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed = true);

	// Quest의 진행 상태를 반환한다.
	virtual QUEST_STATE	OnQueryState(void);

	// Quest강제완료
	virtual	bool		ForceComplete(void);
};

#endif
