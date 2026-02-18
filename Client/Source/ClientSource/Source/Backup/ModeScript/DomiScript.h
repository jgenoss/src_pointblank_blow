#pragma once

#include "ModeScript.h"

//  TODO : GameContext의 스킬포인트 관련된 모든 접근 함수를 이쪽으로 옮기도록 권장...
//       : 가급적이면 접근함수는 find_modedata_index, get_modedata, calc_modedata, set_modedata를 활용해 단순이 래핑하는게 좋겠다.

class DomiScript : public IModeScript
{
private:
	BOOL		_SetSkillSPTable(void);
	INT32		_GetSkillLevelIndex( INT32 nType, INT32 nLv);

	void		_SetSentrygunMaxHP();

public:
	DomiScript(i3Stage* pStage);
	virtual ~DomiScript();

	virtual void OnOpen();				// 필요한 멤버 변수들을 쎄울 것...다 넣지 말고 필요한것만...

	INT32		GetDay(void) const			{ return m_i32RoomModeData[ m_nIdxDay];			}
	INT32		GetWave(void) const			{ return m_i32RoomModeData[ m_nIdxWave];		}
	INT32		GetFlow(void) const			{ return m_i32RoomModeData[ m_nIdxFlowType];	}
	INT32		GetWaveMission(void) const  { return m_i32RoomModeData[ m_nIdxSubFlowType ];}

	INT32		GetScore( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return m_i32RoomModeData[ m_nIdxScoreStart + nIndex];	}
	REAL32		GetSPCur( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return (REAL32)m_i32RoomModeData[ m_nIdxSPStart + nIndex];	}
	INT32		GetLife( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return m_i32RoomModeData[ m_nIdxLifeStart + nIndex];	}
	REAL32		GetStartTime( INT32 nIndex) const	{ I3_BOUNDCHK( nIndex, SLOT_MAX_COUNT); return (REAL32)m_i32RoomModeData[ m_nIdxSTTimeStart + nIndex];	}

	INT32		GetRemainDino(void) const		{ return m_i32RoomModeData[ m_nIdxRemainDino ];		}
	INT32		GetWaveSetTime(void) const		{ return m_i32RoomModeData[ m_nIdxWaveSetTime ];	}

	REAL32		GetSPMax(void) const			{ return m_rSPMax;				}
	REAL32		GetSPChargeSpd(void) const		{ return m_rSPChargeSpd;		}

	INT32		GetSkillCount(void) const		{ return m_nSkillCount;			}
	INT32		GetMaxLevel(void) const			{ return m_nSkillMaxLevel;		}

	INT32		GetSkillCool(INT32 nType, INT32 nLv);	// 사용시 쿨타임
	REAL32		GetSkillOpen(INT32 nType, INT32 nLv);	// 스킬의 레벨에따른 시작 SP
	REAL32		GetSkillCostSP(INT32 nType, INT32 nLv);	// 스킬의 소모 SP
	UINT32		GetSkillItem(INT32 nType, INT32 nLv);	// 스킬에 사용되는 아이템 ID

	INT32		GetUserGameOverFlag(void) const		{ return m_i32RoomModeData[ m_nIdxUserGameOverFlag ];	}

	INT32		GetSentrygunMaxHP(INT32 level);

private:
	INT32		m_nIdxDay;
	INT32		m_nIdxWave;
	INT32		m_nIdxFlowType;
	INT32		m_nIdxSubFlowType;

	INT32		m_nIdxScoreStart;
	INT32		m_nIdxScoreEnd;
	INT32		m_nIdxSPStart;
	INT32		m_nIdxSPEnd;
	INT32		m_nIdxLifeStart;
	INT32		m_nIdxLifeEnd;
	INT32		m_nIdxSTTimeStart;
	INT32		m_nIdxSTTimeEnd;

	INT32		m_nIdxRemainDino;
	INT32		m_nIdxWaveSetTime;

	REAL32		m_rSPMax;
	REAL32		m_rSPChargeSpd;

	INT32*		m_pnSkillTable;
	INT32		m_nSkillCount;
	INT32		m_nSkillValueSize;		// 스킬 하나당 테이블 크기
	INT32		m_nSkillMaxLevel;
	INT32		m_nSkillValueCount;

	INT32		m_nIdxUserGameOverFlag;

	INT32       m_nSentrygunMaxHP[4];
};


