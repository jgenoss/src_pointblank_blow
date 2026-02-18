#pragma once

#ifdef	DOMI_FLOW

#include "Domination_Defines.h"


#define	DEFENCE_OBJECT_ACTIVATE_DELAY	2


using namespace ModeScript;

class gmode_domination;

class CWaveMissionBase
{
protected:
	enum MISSION_BASE_EVENT
	{
		BASE_EVENT_110 = 110,
		BASE_EVENT_111,
		BASE_EVENT_112,
		BASE_EVENT_113,
		BASE_EVENT_114,
		BASE_EVENT_115
	};

	gmode_domination*			m_pGameMode_Domination;

public:
	CWaveMissionBase( gmode_domination* pmode);
	virtual ~CWaveMissionBase(void);

	//virtual void				
	virtual void				ProcessInput( REAL32 rDeltaSeconds) {};
	virtual void				ProcessUpdate( REAL32 rDeltaTime) {};
	
	virtual void				ProcessModeData( INT32 nIndex, INT32 nData) {};
	virtual void				ProcessEventKey( INT32 Arg) {};
	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer);
	
	//object
	virtual	void				ResetMissionObject() {};
	

	//Mission Object Func
	virtual bool				CheckCrossHitMissionObject( CGameCharaBase* pCharacter) { return false; }
	
	virtual bool				IsPressedMissionkey()	{ return false; }
	virtual REAL32				GetPressedMissionKeyTime()	{ return 0.f; }

	//Guide
	virtual i3GameObj*			GetGuildeObject();  //가장 가까운 AI케릭터 리턴
};


class CWaveMission_None : public CWaveMissionBase
{
public:
	CWaveMission_None( gmode_domination* pmode);
	~CWaveMission_None(void);

	void					ProcessModeData( INT32 nIndex, INT32 nData) { I3TRACE("  ## Call Mode Script CallBack Func - WaveMission Type is None    \n"); }
	
	virtual void			ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject() { return CWaveMissionBase::GetGuildeObject(); }
};



#define WAVE_MISSION_LIMIT_TEXT_BUFFER	64
#define WAVE_MISSION_TIME_LIMIT_WARNING_RRMAIN_TIME 10.f
#define WAVE_MISSION_TIME_LIMIT_WARNING_COLOR_TIME 0.35f

class CWaveMission_TimeLimit : public CWaveMissionBase
{

private:
	REAL32				m_rLimitTime;

	REAL32				m_rWarningTime;
	bool				m_bColor;
public:
	CWaveMission_TimeLimit( gmode_domination* pmode);
	~CWaveMission_TimeLimit(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	void						ProcessUpdate( REAL32 rDeltaTime);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject() { return CWaveMissionBase::GetGuildeObject(); }
};

class CWaveMission_Eliminate : public CWaveMissionBase
{
private:
	INT32						m_iKillCount;

public:
	CWaveMission_Eliminate( gmode_domination* pmode);
	~CWaveMission_Eliminate(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }
	
	virtual i3GameObj*			GetGuildeObject() { return CWaveMissionBase::GetGuildeObject(); }
};

class CWaveMission_Recovery : public CWaveMissionBase
{
	enum MISSION_RECOVERY_EVENT
	{
		OBJECT_COUNT = 110,
		RECOVERY_COUNT,
		COMPLETE_FLAG,
		RECOVERY_TARGET
	};
	
	i3Object*			m_pDestinationObj;		//도착 지점.
public:
	CWaveMission_Recovery( gmode_domination* pmode);
	~CWaveMission_Recovery(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject();
};

class CWaveMission_Recovery2 : public CWaveMissionBase
{	
	enum MISSION_RECOVERY2_EVENT
	{
		OBJECT_COUNT = 110,
		RECOVERY_COUNT
	};

public:
	CWaveMission_Recovery2( gmode_domination* pmode);
	~CWaveMission_Recovery2(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }
};

class CWaveMission_Defence : public CWaveMissionBase
{	
	enum { DEFENCE_OBJECT_MAX = 4 };

	std::map<INT32, INT32> m_mapObjectIdx; /*<Used Object Idx, Not Used Object Idx>*/

	i3Object*			m_pDefenceObj;
	
	bool				m_bEnableCrossHitOnObjecct;			//현재 미션 오브젝트에 Cross Hit이 적용 됐을 때.
	bool				m_bObjectActivated;					//서버로 부터 활성화 신호를 받았을 때.


	bool				m_bPressedKey;						//Special Key로 활성화 중 일때.
	REAL32				m_rPressedKeyTime;					

	UINT8				m_iDefenceCount;

	void				_ResetKeyState();					//'e' 버튼 눌렀던 것을 초기화.

	void				_Activate();			
	void				_Deactivate();
public:
	CWaveMission_Defence( gmode_domination* pmode);
	~CWaveMission_Defence(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	void						ProcessInput( REAL32 rDeltaSeconds);
	void						ProcessEventKey( INT32 Arg);

	virtual void				ProcessUpdate( REAL32 rDeltaTime);
	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual	void				ResetMissionObject();

	bool						CheckCrossHitMissionObject( CGameCharaBase* pCharacter);

	bool						IsPressedMissionkey()	{ return m_bPressedKey; }
	REAL32						GetPressedMissionKeyTime()	{ return m_rPressedKeyTime; }

	virtual i3GameObj*			GetGuildeObject();
};

class CWaveMission_Destroy : public CWaveMissionBase
{
	enum MISSION_DESTROY_EVENT
	{
		EGG_SUM_COUNT = 110,
		EGG_DESTROY_COUNT,
		EGG_OBJECT_0,
		EGG_OBJECT_1,
		EGG_OBJECT_2,
		EGG_OBJECT_3,
		EGG_OBJECT_4,
		EGG_OBJECT_5
	};

	i3Object*			m_pDestroyObj;					
public:
	CWaveMission_Destroy( gmode_domination* pmode);
	~CWaveMission_Destroy(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	virtual	void				ResetMissionObject();
	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject();
};

class CWaveMission_Bomb : public CWaveMissionBase //Flow Type Boss..
{
	enum MISSION_BOMB_EVENT
	{
		BOMB_SUM_COUNT = 110,
		BOMB_COUNT
	};

	i3GameObj*					m_pGuideObject;
public:
	CWaveMission_Bomb( gmode_domination* pmode);
	~CWaveMission_Bomb(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject();
};

class CWaveMission_Block : public CWaveMissionBase
{
	UINT16						m_iLimit;

	i3RespawnObj*				m_pAiRespawnObj;
	i3RespawnObj*				m_pAiDestObj;
	
	std::vector<INT32>			m_vecTriggerAI;
	INT32						m_nCurrentTriggerAI;
public:
	CWaveMission_Block( gmode_domination* pmode);
	~CWaveMission_Block(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	virtual void				ProcessUpdate( REAL32 rDeltaTime);

	virtual void				ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) { CWaveMissionBase::ProcessEventTimer( iTimerID, iTime, bSetTimer); }

	virtual i3GameObj*			GetGuildeObject();
};

#endif