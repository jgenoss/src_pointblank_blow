#if !defined( __I3_TRIGGER_H)
#define __I3_TRIGGER_H
#include "i3GameObj.h"
////////////////////////////////////////////////////////////////////
// i3Trigger State
#define				I3_TRIG_STATE_DISABLE		0x00000001

class i3Action;



class I3_EXPORT_FRAMEWORK i3Trigger : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3Trigger, i3GameObj);

protected:
	// 초기 상태( Activate/Deactivate)
	bool				m_bInitEnable = true;
	UINT32				m_TriggerState = 0;
	i3TimeEventGen	*	m_pTimeEvent = i3TimeEventGen::new_object();
	// 작동 횟수
	UINT32				m_nRepeatCount = 0;
	INT32				m_Param1 = 0;
	INT32				m_Param2 = 0;

	//같은속성을가진 트리거들의 그룹인경우 지정된 한개의 트리거 속성만을 사용한다.
	i3Trigger		*	m_pOwner = nullptr;

	//volatile
	NxActor			*	m_pNxActor = nullptr;

	bool				m_bTriggerGroupModeEnabled = false;

	i3EventReceiver	*	m_pEventReceiver = nullptr;
	UINT32				m_nRemainCount = 0;			// 남은 동작횟수

	i3::vector<struct ALIGN4 I3TRIG_INNER_ITEM*>	m_InnerObjectList;

public:
	virtual ~i3Trigger(void);

	void			Create( NxActorDesc * pDesc = nullptr, bool bUseTool = false);
	void			CreateSphereTrigger( VEC3D * pCenterOff, REAL32 radius, INT32 group);
	void			CreateBoxTrigger( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose = nullptr);
	void			CreateCapsuleTrigger(VEC3D * pCenter, REAL32 radius, REAL32 height, INT32 group);

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

	NxActor *		getNxActor(void)					{ return m_pNxActor; }

	i3TimeEventGen *getTimeEventGen( void)				{ return m_pTimeEvent;}
	void			setTimeEventGen( i3TimeEventGen * pEventGen)	{ I3_REF_CHANGE( m_pTimeEvent, pEventGen);}

	void			setEventReceiver( i3EventReceiver * pRecv)	{	I3_REF_CHANGE( m_pEventReceiver, pRecv);}
	i3EventReceiver *getEventReceiver(void)						{	return m_pEventReceiver;}

	virtual void	SetEnable( bool bTrue = 1, REAL32 rHideTime = 0.0f) override;

	void			setTriggerGroupModeEnabled( bool bEnable)	{ m_bTriggerGroupModeEnabled = bEnable;}

public:

	virtual void	OnInitTrigger();

	virtual void	OnTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status) override;
	virtual void	OnChangeTime( REAL32 tm) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	void			Enter(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);					//활성화된 트리거가 작동
	void			Leave(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);					//활성화된 트리거를 멈춤
	void			Stay(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);

	virtual void	OnStay( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest);
	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest);
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest);

	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
};




#endif
