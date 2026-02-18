#if !defined( __I3_TRIGGER_H)
#define __I3_TRIGGER_H
#include "i3GameObj.h"
////////////////////////////////////////////////////////////////////
// i3Trigger State
#define				I3_TRIG_STATE_DISABLE		0x00000001

class i3Action;

class I3_EXPORT_FRAMEWORK i3Trigger : public i3GameObj
{
	I3_CLASS_DEFINE( i3Trigger);

protected:
	bool				m_bInitEnable;			// 초기 상태( Activate/Deactivate)
	UINT32				m_TriggerState;
	i3TimeEventGen	*	m_pTimeEvent;			// 
	UINT32				m_nRepeatCount;			// 작동 횟수
	INT32				m_Param1;
	INT32				m_Param2;

	i3Trigger		*	m_pOwner;				//같은속성을가진 트리거들의 그룹인경우 지정된 한개의 트리거 속성만을 사용한다.

	//volatile
	NxActor			*	m_pNxActor;

	bool				m_bTriggerGroupModeEnabled;

	i3EventReceiver	*	m_pEventReceiver;
	UINT32				m_nRemainCount;			// 남은 동작횟수

	i3List				m_InnerObjectList;

public:
	i3Trigger(void);
	virtual ~i3Trigger(void);

	void			Create( NxActorDesc * pDesc = NULL, bool bUseTool = false);
	void			CreateSphereTrigger( VEC3D * pCenterOff, REAL32 radius, INT32 group);
	void			CreateBoxTrigger( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose = NULL);

	void			InitTrigger(void);	

	void			ActivateTrigger(void);			//트리거를 활성화
	void			DeactivateTrigger(void);		//트리거를 비활성화

	void			setRepeatCount( UINT32 nCnt)		{ m_nRepeatCount = m_nRemainCount = nCnt;}
	UINT32			getRepeatCount( void)				{ return m_nRepeatCount;}

	bool			getInitState(void)					{ return m_bInitEnable; }
	void			setInitState( bool state)			{ m_bInitEnable = state; }

	INT32			getParam1(void)						{ return m_Param1; }
	void			setParam1( INT32 val)				{ m_Param1 = val; }

	INT32			getParam2(void)						{ return m_Param2; }
	void			setParam2( INT32 val)				{ m_Param2 = val; }

	void			setOwner( i3Trigger * pOwner)		{ I3_REF_CHANGE( m_pOwner, pOwner);}
	i3Trigger	*	getOwner( void)						{ return m_pOwner;}

	i3TimeEventGen *getTimeEventGen( void)				{ return m_pTimeEvent;}
	void			setTimeEventGen( i3TimeEventGen * pEventGen)	{ I3_REF_CHANGE( m_pTimeEvent, pEventGen);}

	void			setEventReceiver( i3EventReceiver * pRecv)	{	I3_REF_CHANGE( m_pEventReceiver, pRecv);}
	i3EventReceiver *getEventReceiver(void)						{	return m_pEventReceiver;}

	//virtual void		SetVisible( BOOL bTrue = TRUE );
	virtual void	SetEnable( BOOL bTrue = 1, RT_REAL32 rHideTime = 0.0f);

	void			setTriggerGroupModeEnabled( bool bEnable)	{ m_bTriggerGroupModeEnabled = bEnable;}

public:
	virtual void	OnChangeTime( REAL32 tm);

	virtual void	OnUpdate( REAL32 rDeltaSeconds);//

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	void			Enter(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);					//활성화된 트리거가 작동
	void			Leave(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);					//활성화된 트리거를 멈춤

	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest);
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest);

	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData);
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz);
};

#endif
