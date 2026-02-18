#include "i3FrameworkPCH.h"
#include "i3Trigger.h"
#include "i3GameSceneInfo.h"
#include "i3ActionSetWorldSection.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3Trigger);

static bool bInitTriggerCallBack = false;

struct ALIGN4 I3TRIG_INNER_ITEM
{
	INT32	nCnt = 0;
	i3ElementBase * pItem = nullptr;

	void Reset()
	{
		nCnt  = 0;
		pItem = nullptr;
	}
};

I3TRIG_INNER_ITEM * __Find( i3ElementBase * pItem, const i3::vector<I3TRIG_INNER_ITEM*>& List)
{
	I3ASSERT( pItem != nullptr);

	const size_t cnt = List.size();
	for( size_t i = 0;i < cnt; ++i)
	{
		I3TRIG_INNER_ITEM * p = List[i];
		if( p->pItem == pItem)
			return p;
	}

	return nullptr;
}

I3TRIG_INNER_ITEM * __Add( i3ElementBase * pItem, i3::vector<I3TRIG_INNER_ITEM*>& List)
{
	I3ASSERT( pItem != nullptr);

	I3TRIG_INNER_ITEM * pNew = (I3TRIG_INNER_ITEM*)i3MemAlloc( sizeof( I3TRIG_INNER_ITEM));
	pNew->Reset();

	pNew->nCnt	= 1;
	pNew->pItem = pItem;

	List.push_back(pNew);

	return pNew;
}

INT32 __Remove( i3ElementBase * pItem, i3::vector<I3TRIG_INNER_ITEM*>& List)
{
	I3ASSERT( pItem != nullptr);

	I3TRIG_INNER_ITEM * p = __Find( pItem, List);

	if( p != nullptr)
	{
		i3::vu::remove(List, p);
		
		I3MEM_SAFE_FREE( p);
	}

	return (INT32)List.size();
}

const i3ElementBase * __GetItem( INT32 idx, const i3::vector<I3TRIG_INNER_ITEM*>& List)
{
	
	INT32 cnt  = (INT32)List.size();
	if( idx < 0 || idx >= cnt)
		return nullptr;

	I3TRIG_INNER_ITEM * p = List[idx];

	return p->pItem;
}

void __ClearList( i3::vector<I3TRIG_INNER_ITEM*>& List)
{
	const size_t cnt = List.size();
	for( size_t i = 0;i < cnt; ++i)
	{
		I3TRIG_INNER_ITEM * p = List[i];
		I3MEM_SAFE_FREE( p);
	}

	List.clear();
}

i3Trigger::~i3Trigger(void)
{
	DeactivateTrigger();

	if( m_pNxActor != nullptr)
	{
		g_pPhysixCtx->CMD_releaseActor( m_pNxActor);
	}

	I3_SAFE_RELEASE( m_pOwner);
	I3_SAFE_RELEASE( m_pTimeEvent);

	__ClearList( m_InnerObjectList);
}

void i3Trigger::OnTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if(status & NX_TRIGGER_ON_ENTER)
	{
		Enter( triggerShape, otherShape, status);
	}

	if(status & NX_TRIGGER_ON_STAY) 
	{
		Stay( triggerShape, otherShape, status);
	}
		
	if(status & NX_TRIGGER_ON_LEAVE) 
	{
		Leave( triggerShape, otherShape, status);
	}
}

void i3Trigger::OnChangeTime( REAL32 tm)
{
	if( m_pTimeEvent != nullptr)
	{
		m_pTimeEvent->OnChangeTime( tm);
	}

	i3GameObj::OnChangeTime( tm);
}

void i3Trigger::Create( NxActorDesc * pDesc, bool bUseTool )
{
	NxBodyDesc bodyDesc;
	NxActorDesc desc;

	if( pDesc == nullptr)
	{
		desc.body = &bodyDesc;
		pDesc = &desc;
	}
	else if( !bUseTool )
	{
		//NxShapeDesc * pShapeDesc = pDesc->shapes[0];	//C4189
	}

	I3ASSERT( pDesc->isValid());

	m_pNxActor = g_pPhysixCtx->createActor( pDesc);
	I3ASSERT( m_pNxActor != nullptr);
}

void i3Trigger::SetEnable( bool bTrue, REAL32 rHideTime)
{
	i3GameObj::SetEnable( bTrue);

	if( bTrue)
	{
		ActivateTrigger();
	}
	else
	{
		DeactivateTrigger();
	}
}
/*
void i3Trigger::SetVisible( bool bTrue)
{
	i3GameObj::SetVisible( bTrue);

	if( bTrue)
	{
		ActivateTrigger();
	}
	else
	{
		DeactivateTrigger();
	}
}
*/

void i3Trigger::CreateBoxTrigger( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose)
{
	NxBoxShapeDesc boxDesc;
	NxActorDesc actorDesc;

	boxDesc.dimensions.set( pSize->x * 0.5f, pSize->y * 0.5f, pSize->z * 0.5f);
	boxDesc.localPose.t.set( pCenter->x, pCenter->y, pCenter->z);

	boxDesc.group = (NxCollisionGroup) group;
	boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
	
	actorDesc.setToDefault();
	
	actorDesc.shapes.pushBack( &boxDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	if( pGlobalPose != nullptr)
	{
		NXU::Copy( &actorDesc.globalPose, pGlobalPose);
	}

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc);
}

void i3Trigger::CreateSphereTrigger( VEC3D * pCenterOff, REAL32 radius, INT32 group)
{
	NxSphereShapeDesc shapeDesc;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	
	shapeDesc.group = (NxCollisionGroup) group;
	shapeDesc.localPose.t.set( pCenterOff->x, pCenterOff->y, pCenterOff->z);

	shapeDesc.radius		= radius;
	shapeDesc.shapeFlags	|= NX_TRIGGER_ENABLE;

	actorDesc.shapes.pushBack( &shapeDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc );
}

void i3Trigger::CreateCapsuleTrigger(VEC3D * pCenter, REAL32 radius, REAL32 height, INT32 group)
{
	NxCapsuleShapeDesc capsuleDesc;
	NxActorDesc actorDesc;

	capsuleDesc.localPose.t.x = pCenter->x;
	capsuleDesc.localPose.t.y = pCenter->y;
	capsuleDesc.localPose.t.z = pCenter->z;
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	capsuleDesc.group = (NxCollisionGroup) group;

	capsuleDesc.shapeFlags	|= NX_TRIGGER_ENABLE;

	actorDesc.shapes.push_back(&capsuleDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc);
}

void i3Trigger::InitTrigger( void)
{
	m_TriggerState = I3_TRIG_STATE_DISABLE;

	if( m_bInitEnable == false)
	{
		DeactivateTrigger();
	}
	else
	{
		ActivateTrigger();
	}
	
	this->OnInitTrigger();			// 사용자 가상함수 처리..
}

void	i3Trigger::OnInitTrigger()
{
	// Non Impl..
}

void i3Trigger::ActivateTrigger(void)
{
	__ClearList( m_InnerObjectList);

	if( m_pNxActor != nullptr)
	{
		m_pNxActor->clearActorFlag( (NxActorFlag) (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE));

		//m_pNxActor->modifyActorFlag( (NxActorFlag) 0, (NxActorFlag) (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE));
	}
}

void i3Trigger::DeactivateTrigger(void)
{
	__ClearList( m_InnerObjectList);

	if( m_pNxActor != nullptr)
	{
		m_pNxActor->raiseActorFlag( (NxActorFlag) (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE));
		//m_pNxActor->modifyActorFlag( (NxActorFlag)(NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE), (NxActorFlag)0);
	}
}

void i3Trigger::OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
}

void i3Trigger::OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
}

void i3Trigger::OnStay( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
}

void i3Trigger::Stay(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if( m_pOwner != nullptr)
	{
		I3ASSERT( i3::kind_of<i3Trigger* >(m_pOwner))

		m_pOwner->Stay( triggerShape, otherShape, status);
		return;
	}

	// Trigger에 진입한 Object에게 Event들을 Notify해야 한다.
	i3PhysixShapeSet * pTriggerShapeSet = (i3PhysixShapeSet *) triggerShape.userData;
	i3ElementBase *	pDest = (i3ElementBase *) otherShape.userData;

	if( pDest == nullptr)	return;
	if( !i3::kind_of<i3EventReceiver* >(pDest))
		return;

	if( m_pTimeEvent != nullptr)
	{
		i3EventReceiver * pRecv = this;

		if( i3::same_of<i3PhysixShapeSet* >(pDest))
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pDest;

			if( (pShapeSet != nullptr) && (pShapeSet->getEventReceiver() != nullptr))
			{
				pRecv = pShapeSet->getEventReceiver();
			}
		}
		else
		{
			pRecv = (i3EventReceiver*)pDest;
		}
		
		UINT32 i;
		for( i = 0;i < m_pTimeEvent->getEventCount(); ++i)
		{
			i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( i);
			if( pInfo != nullptr)
			{
				i3Action * pAction = pInfo->getAction();

				i3GameObj* pGameOj = NULL;
				if (i3::kind_of< i3GameObj*>(pRecv))
					pGameOj = static_cast<i3GameObj*>(pRecv);

				if ((pGameOj && i3::same_of< i3ActionSetWorldSection*>(pAction) && pGameOj->IsDropObj() == true)
					|| i3::same_of< i3ActionSetMapSector* >(pAction))
				{
					pRecv->OnEvent( I3_EVT_ACTION, pAction, pInfo->getParam());
				}
			}
		}
	}

	OnStay( pTriggerShapeSet, pDest);
}

void i3Trigger::Enter( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if( m_pOwner != nullptr)
	{
		I3ASSERT( i3::kind_of<i3Trigger*>(m_pOwner));

		m_pOwner->Enter( triggerShape, otherShape, status);
		return;
	}

	// Trigger에 진입한 Object에게 Event들을 Notify해야 한다.
	i3PhysixShapeSet * pTriggerShapeSet = (i3PhysixShapeSet *) triggerShape.userData;
	i3ElementBase *	pDest = (i3ElementBase *) otherShape.userData;

//	if( pDest == nullptr)	return;
	if( !i3::kind_of<i3EventReceiver* >(pDest))
		return;

	if( m_bTriggerGroupModeEnabled)
	{
		I3TRIG_INNER_ITEM * p = __Find( pDest, m_InnerObjectList);

		if( p == nullptr)
		{
			__Add( pDest, m_InnerObjectList);
		}
		else
		{
			p->nCnt++;
			return;
		}
	}

	// Trigger에 진입했으므로, OnUpdate가 동작하도록...
	m_TriggerState &= ~I3_TRIG_STATE_DISABLE;

	if( m_pTimeEvent != nullptr)
	{
		i3EventReceiver * pRecv = this;

		if( i3::same_of<i3PhysixShapeSet* >(pDest))
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pDest;

			if( (pShapeSet != nullptr) && (pShapeSet->getEventReceiver() != nullptr))
			{
				pRecv = pShapeSet->getEventReceiver();
			}
		}
		else
		{
			UINT32 i;
			for( i = 0;i < m_pTimeEvent->getEventCount(); ++i)
			{
				i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( i);
				if( pInfo != nullptr)
				{
					i3Action * pAction = pInfo->getAction();
					if( i3::same_of< i3ActionSetWorldSection*>(pAction)
						|| i3::same_of<i3ActionSetMapSector* >(pAction))
					{
						pRecv = (i3EventReceiver*)pDest;

						i = m_pTimeEvent->getEventCount();
					}
				}
			}
		}

		m_pTimeEvent->setEventReceiver( pRecv);

		m_pTimeEvent->OnChangeTime( 0.0f);
		m_pTimeEvent->OnUpdate( 0.0f);
	}

	OnEnter( pTriggerShapeSet, pDest);
}

void i3Trigger::Leave( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if( m_pOwner != nullptr)
	{
		I3ASSERT( i3::kind_of<i3Trigger* >(m_pOwner));

		m_pOwner->Leave( triggerShape, otherShape, status);
		return;
	}

	i3PhysixShapeSet * pTriggerShapeSet = (i3PhysixShapeSet *) triggerShape.userData;
	i3ElementBase *	pDest = (i3ElementBase *) otherShape.userData;	

//	if( pDest == nullptr)	return;
	if( !i3::kind_of<i3EventReceiver*>(pDest))
		return;

	if( m_bTriggerGroupModeEnabled)
	{
		I3TRIG_INNER_ITEM * p = __Find( pDest, m_InnerObjectList);
		I3ASSERT( p != nullptr);

		p->nCnt--;

		if( p->nCnt == 0)
		{
			__Remove( pDest, m_InnerObjectList);
		}
		else
		{
			return;
		}
	}
	
	// Trigger에서 탈출했으므로 OnUpdate가 동작하지 않도록..
	m_TriggerState |= I3_TRIG_STATE_DISABLE;

	if( m_pTimeEvent != nullptr)
	{
		i3EventReceiver * pRecv = this;

		if( i3::same_of<i3PhysixShapeSet* >(pDest))
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pDest;

			if( (pShapeSet != nullptr) && (pShapeSet->getEventReceiver() != nullptr))
			{
				pRecv = pShapeSet->getEventReceiver();
			}
		}
		else
		{
			UINT32 i;
			for( i = 0;i < m_pTimeEvent->getEventCount(); ++i)
			{
				i3TimeEventInfo * pInfo = m_pTimeEvent->getEvent( i);
				if( pInfo != nullptr)
				{
					i3Action * pAction = pInfo->getAction();
					if( i3::same_of<i3ActionSetWorldSection* >(pAction)
						|| i3::same_of<i3ActionSetMapSector* >(pAction))
					{
						pRecv = (i3EventReceiver*)pDest;

						i = m_pTimeEvent->getEventCount();
					}
				}
			}
		}

		m_pTimeEvent->setEventReceiver( pRecv);
		m_pTimeEvent->Stop();
	}

	OnLeave( pTriggerShapeSet, pDest);
}

void i3Trigger::OnUpdate( REAL32 rDeltaSeconds)
{
	if( (m_TriggerState & I3_TRIG_STATE_DISABLE) == 0) // isEnable() && --> 중복 체크로 인해 해당 코드는 뺍니다. soon9 2013.09.10
	{
		if( m_pTimeEvent != nullptr)
		{
			m_pTimeEvent->OnUpdate( rDeltaSeconds);
		}

		i3GameObj::OnUpdate( rDeltaSeconds);
	}
}

void i3Trigger::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTimeEvent != nullptr)
	{
		m_pTimeEvent->OnBuildObjectList( List);
	}

	if( m_pOwner != nullptr)
	{
		m_pOwner->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN1 TRIGGER
	{
		UINT8		nID[4] = { 'T', 'R', 'G', '1' };
		UINT32		nVersion = 0;

		UINT32		nInitState = 0;
		OBJREF64	pTimeEvent = 0;
		INT32		m_Param1 = 0;
		INT32		m_Param2 = 0;
		OBJREF64	pOwner = 0;
		UINT32		pad[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3Trigger::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( Result);

	pack::TRIGGER data;
	
	data.nVersion	= 1;

	data.nInitState	= (UINT32) m_bInitEnable;
	data.pTimeEvent	= (OBJREF64)pResFile->GetObjectPersistID( getTimeEventGen());
	data.m_Param1	= getParam1();
	data.m_Param2	= getParam2();
	data.pOwner		= (OBJREF64)pResFile->GetObjectPersistID( getOwner());
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3Trigger::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::TRIGGER data;

	Result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3::generic_is_niequal( (char*)data.nID, "TRG1", 4) == false)
	{
		I3TRACE("Not a Valid Trigger Info\n");
		return STREAM_ERR;
	}

	if( data.nVersion != 1)
	{
		return STREAM_ERR;
	}

	m_bInitEnable = (data.nInitState != 0);

	if( data.pTimeEvent > 0)
	{
		i3TimeEventGen * pEvent = (i3TimeEventGen*)pResFile->FindObjectByID( (OBJREF)data.pTimeEvent);

		I3ASSERT( pEvent);

		setTimeEventGen( pEvent);
	}

	if( data.pOwner > 0)
	{
		i3Trigger * pTrig = (i3Trigger*)pResFile->FindObjectByID( (OBJREF)data.pOwner);

		I3ASSERT( pTrig);

		setOwner( pTrig);
	}

	setParam1( data.m_Param1);
	setParam2( data.m_Param2);

	return Result;
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)			// [test required] 팩 사이즈가 명시되지 않아 추가합니다.. 2017.02.09 soon9
#endif

namespace pack
{
	struct TRIGGER_GAMEDATA
	{
		OBJREF64	m_pOwner = 0;
		UINT32		pad[10] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3Trigger::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	if( m_pOwner == nullptr || pInfo == nullptr)
	{
		return 0;
	}

	pack::TRIGGER_GAMEDATA * pData;

	UINT32 sz = sizeof( pack::TRIGGER_GAMEDATA);

	*ppData = pData = (pack::TRIGGER_GAMEDATA*)i3MemAlloc( sz);

	i3mem::FillZero( pData, sz);

	pData->m_pOwner = (OBJREF64)pInfo->GetObjectPersistID( m_pOwner);

	return sz;
}

UINT32 i3Trigger::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	if( sz == 0)	return 0;

	i3Stream * pStream = pInfo->getStream();
	UINT32 result = 0;

	pack::TRIGGER_GAMEDATA data;
	result = pStream->Read( &data, sizeof(data));
	I3_CHKIO( result);

	i3Trigger * pTrig = (i3Trigger*)pInfo->FindObjectByIndex((UINT32)data.m_pOwner);
	if( pTrig != nullptr)
	{
		setOwner( pTrig);

		pTrig->setTriggerGroupModeEnabled( true);

		setTriggerGroupModeEnabled( true);
	}
	else
	{
		setTriggerGroupModeEnabled( false);
	}

	return result;
}