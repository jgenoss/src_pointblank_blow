#pragma once

#ifdef	DOMI_FLOW

class gmode_domination;

class CDominationFlowBase
{
public:
	CDominationFlowBase( gmode_domination* pmode);
	virtual ~CDominationFlowBase(void);

	virtual void StartFlow();
	virtual void EndFlow();

	virtual void OnUpdate( REAL32 rDeltaSeconds) {}

	virtual void ResetObject(void) {}


	virtual void ProcessInput( REAL32 rDeltaSeconds) {};
	virtual void ProcessModeData( INT32 nIndex, INT32 nData) {};
	

	virtual void ProcessEventKey(INT32 arg)	{}
	virtual void ProcessEventTimer( UINT8 iTimerID, UINT16 iTime, bool bSetTimer) {}
	virtual bool ProcessEventTrigger(i3ElementBase* pObj)	{ return false; }


	virtual bool CheckCrossHitMissionObject( CGameCharaBase* pCharacter) { return false;}
	virtual bool IsPressedMissionkey() const { return false;}
	virtual REAL32 GetPressedMissionKeyTime() const { return 0.f;}

protected:
	gmode_domination*	m_pmode;
};

class CDomi_Flow_None : public CDominationFlowBase
{
public:
	CDomi_Flow_None( gmode_domination* pmode) : CDominationFlowBase( pmode) {}
	virtual ~CDomi_Flow_None(void) {}
	
	/*virtual*/ void StartFlow() {}
	/*virtual*/ void EndFlow() {}
};

#endif