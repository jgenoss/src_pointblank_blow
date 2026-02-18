#if !defined( __TARGET_TRIGGER_H)
#define __TARGET_TRIGGER_H

#include "TargetObject.h"
#include "StageDef.h"

class TargetTrigger : public i3Trigger
{
	I3_CLASS_DEFINE( TargetTrigger, i3Trigger);
protected:
	i3::vector<TargetObject*>			m_ObjList;
	TEAM_TYPE		m_Team;

public:
	TargetTrigger(void);
	virtual ~TargetTrigger(void);

	INT32			getTargetCount(void)					{ return (INT32)m_ObjList.size(); }
	TargetObject *	getTarget( INT32 idx)					{ return m_ObjList[ idx]; }
	void			addTarget( TargetObject * pObj);
	void			removeTarget( TargetObject * pObj);
	void			removeAllTarget(void);

	TEAM_TYPE		getTeam(void)							{ return m_Team; }
	void			setTeam( TEAM_TYPE team)				{ m_Team = team; }

	virtual void	OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	//
	// Level Tool에서 게임 의존적인 추가 Data를 저장하기 위해 호출한다.
	// OnQueryGameData() 함수 내에서는 필요한 메모리를 할당해 반환해야 하며
	// 실제로 할당된 메모리의 크기를 반환한다.
	//
	// Level Tool은 반환된 메모리를 i3MemFree() 함수를 통해 해제하기 때문에
	// 반드시 i3MemAlloc() 함수를 통해 할당된 메모리여야 한다.
	//
	// Paramters
	//		pData	[OUT]		저장할 데이터를 위해 할당한 메모리 버퍼. 함수 내에서 할당된다.
	//
	// Return
	//		할당된 메모리 버퍼의 크기를 Byte 단위로 반환한다.
	//
	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;

	//
	// i3GameSceneInfo Class에서 게임용 Instance를 Loading하는 중에 호출한다.
	// OnQueryGameData() 함수에 전달된 것과 동일한 처리를 해야한다.
	// 실제로 읽어들인 메모리를 반환해야 하며, 만약 처리 중 Error가 발생했다면
	// STREAM_ERR을 반환해야 한다.
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
};

#endif
