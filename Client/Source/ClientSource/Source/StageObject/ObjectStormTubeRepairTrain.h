#if !defined( __OBJECT_STORM_TUBE_REPAIR_TRAIN_H__)
#define __OBJECT_STORM_TUBE_REPAIR_TRAIN_H__

#include "GameControledObject.h"

enum RTSND_STATE
{
	RTSND_STATE_IDLE = 0,
	RTSND_STATE_MOVE,

	RTSND_STATE_COUNT,
};

class CGameObjectStormTubeRepairTrain : public CGameControledObject
{
	I3_EXPORT_CLASS_DEFINE( CGameObjectStormTubeRepairTrain, CGameControledObject);

private:
	enum REPAIR_TRAIN_ANIM
	{
		RTA_START = 0,
		RTA_MOVE,
		RTA_END,

		RTA_COUNT,
	};
	
	REPAIR_TRAIN_ANIM		m_eRTA;
	i3Animation *			m_pAnim[RTA_COUNT];
	REAL32					m_rMovelTime;
	INT32					m_iSendPacketCount;
	VEC3D					m_v3VelTrain;

	i3Node *				m_pRootDummy;
	REAL32					m_rAnimPlayTime;

	i3SoundNode *			m_pSoundNode[ RTSND_STATE_COUNT];
	RTSND_STATE				m_nSoundState;

public:
	CGameObjectStormTubeRepairTrain( void);
	virtual ~CGameObjectStormTubeRepairTrain( void);

	virtual void			InitObject() override;
	virtual void			ResetObject() override;
	VEC3D*					GetVelocityTrain();

	virtual void			ProcessObject( REAL32 rDeltaSeconds) override;
	void					ProcessTest( REAL32 rDeltaSeconds);

	virtual void			EnableObject() override {}
	virtual void			SetControledObjectState( INT32 state) override {}

	void					InitMemberVaribleSTRT();
	void					LoadAnimSTRT();
	void					SetAnimSTRT(REPAIR_TRAIN_ANIM animKey);
	void					SetMoveAnimSTRT();

	void					MoveHostProcess(REAL32 rDeltaSeconds);
	void					MoveProcess(REAL32 rDeltaSeconds);

	// test
	/*void					TraceDebugInfo();
	NxVec3					m_v3OldPos[10];
	NxVec3					m_v3NowPos[10];*/
};

#endif
