#include "pch.h"
#include "QuestBase.h"

I3_CLASS_INSTANCE( CQuestBase);//, i3ElementBase);

CQuestBase::CQuestBase(void)
{
	i3mem::FillZero( m_Arg, sizeof( m_Arg));
	m_ID = 0;
}

CQuestBase::~CQuestBase(void)
{
}

void CQuestBase::getProgressColor( INT32 cur, INT32 maxcount, COLOR * pColor)
{
	static UINT8 s_ColorTable[3][4] =
	{
		{	206, 206, 206, 255 },		// 시작도 안했을 때
		{	252, 255, 100, 255 },		// 진행 중일 때
		{	159, 252, 0,	255 }		// 완료했을 때
	};

	if( cur <= 0.0f)
	{
		pColor->r = s_ColorTable[0][0];
		pColor->g = s_ColorTable[0][1];
		pColor->b = s_ColorTable[0][2];
		pColor->a = s_ColorTable[0][3];
	}
	else if( cur == maxcount)
	{
		pColor->r = s_ColorTable[2][0];
		pColor->g = s_ColorTable[2][1];
		pColor->b = s_ColorTable[2][2];
		pColor->a = s_ColorTable[2][3];
	}
	else
	{
		pColor->r = s_ColorTable[1][0];
		pColor->g = s_ColorTable[1][1];
		pColor->b = s_ColorTable[1][2];
		pColor->a = s_ColorTable[1][3];
	}
}

void CQuestBase::OnBind( const QUEST_CTX * pCtx)
{
	m_ID = pCtx->m_ID;

	for( INT32 i = 0; i < MAX_QUEST_ARG; i++)
	{
		m_Arg[i] = pCtx->m_Arg[i];
	}

//	I3TRACE("Arg 1: %d 2: %d 3: %d 4: %d 5: %d 6: %d\n", m_Arg[0],m_Arg[1],m_Arg[2],m_Arg[3],m_Arg[4],m_Arg[5]);
}

// 주인공으로 인해, 다른 캐릭터가 죽었을 때 호출된다.
// Kill Chara.에 대한 구체적인 정보는 KILL_INFO Structure에 저장되어 있다.
bool		CQuestBase::OnKill( KILL_INFO * pKillInfo)
{
	return false;
}

// 다른 캐릭터에 의해 주인공 캐릭터가 죽임을 당했을 때에 호출된다.
// Kill Chara에 대한 구체적인 정보는 KILL_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnDeath( KILL_INFO * pDeathInfo)
{
	return false;
}

// 케릭터가 공격을 당했을 때 호출된다.
// 공격 내용에 대한 구체적인 정보는 DAMAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnDamage( DAMAGE_INFO * pDamgeInfo)
{
	return false;
}

// 새롭게 Stage를 시작했을 때 호출된다.
// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnStageStart( STAGE_INFO * pStage)
{
	return false;
}

// Stage를 종료할 때 호출된다.
// Deathmatch, Mission 등의 종류를 막론하고 각 Stage마다 한번만 호출된다.
// 새로 시작하는 Stage에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnStageEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// 새롭게 Round를 시작했을 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnRoundStart( STAGE_INFO * pStageInfo)
{
	return false;
}

// Round를 종료할 때 호출된다.
// Deathmatch 등과 같이 Round제 Mission이 아닌경우에는 호출되지 않을 수 있다.
// 새로 시작하는 Round에 대한 구체적인 정보는 STAGE_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnRoundEnd( STAGE_INFO * pStageInfo)
{
	return false;
}

// 주인공 캐릭터가 특정 동작을 취했을 때 호출된다.
// 동작에 대한 구체적인 정보는 ACTION_INFO Structure를 통해 전달된다.
bool		CQuestBase::OnAction( ACTION_INFO * pAction)
{
	return false;
}

// 화면에 출력할 Mission에 대한 설명문을 요구할 때 호출된다.
// CQuestBase Class에서 파생된 개별 Mission Class들은
// 반드시 이 함수를 Override하여 적절한 형식의 문자열을 제공해야 한다.
void		CQuestBase::OnQueryDesc( char * pszBuff, INT32 maxLen)
{
	I3ASSERT( pszBuff != NULL);

	*pszBuff = 0;
}

void		CQuestBase::OnQueryHUDDesc( char * pszBuff, INT32 maxLen, bool bLineFeed)
{
	*pszBuff = 0;
}

// Quest의 진행 상태를 반환한다.
QUEST_STATE	CQuestBase::OnQueryState(void)
{
	return QST_NOT_STARTED; 
}

bool	CQuestBase::ForceComplete(void)
{
	return false;
}
