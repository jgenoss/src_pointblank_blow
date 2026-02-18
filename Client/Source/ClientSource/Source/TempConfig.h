#ifndef _TEMP_CONFIG_H__
#define _TEMP_CONFIG_H__

// 이 클래스는 개발 필요에 따라 임시적으로 사용하는 클래스입니다.
// 외부파일로 부터 값을 읽어올때 사용합니다. (ini형식 사용)

//AI Mode에 사용(DieHard)
#define MAX_AI_COUNT	10

enum EVADE_TYPE
{
	EVADE_TYPE_MOVE = 0,
	EVADE_TYPE_JUMP,	

	EVADE_TYPE_COUNT,
};

class CTempConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CTempConfig, i3ElementBase );

public:
	////////////////////////////////////////////////////////
	REAL32		m_Speed[MAX_AI_COUNT+1];
	REAL32		m_Damage[MAX_AI_COUNT+1];
	REAL32		m_PlayerCountPerDamage[4];
	INT32		m_Weapon[MAX_AI_COUNT+1];
	INT32		m_Evade[EVADE_TYPE_COUNT];
	REAL32		m_RespawnTime;
	REAL32		m_JumpHeight;
	REAL32		m_JumpTime;

public:
	CTempConfig(void);
	~CTempConfig(void);

	void		Load();
};

extern CTempConfig * g_pTempConfig;

#endif //_TEMP_CONFIG_H__