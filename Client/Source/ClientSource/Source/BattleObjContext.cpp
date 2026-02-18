#include "pch.h"
#include "BattleObjContext.h"
#include "StageBattle.h"
#include "./StageObject/ObjectWeaponBox.h"
#include "./StageObject/ObjectEvent.h"
#include "GameMissionManager.h"

BattleObjContext::BattleObjContext()
{
	BattleObjContext::ResetGameInfo_ThrowWeapon();

	for(INT32 i = 0; i < OBJ_TOTAL_COUNT; i++ )
		m_ObjectGameInfo[ i ].ResetGameInfoObject();

	for(INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT; i++)
		m_DropedWeaponGameInfo[ i].Reset();

	m_bInterEnterSmokeSend = false;

	__Register_Object_Function();
}

BattleObjContext::~BattleObjContext()
{
}

void	BattleObjContext::ResetGameInfo_ThrowWeapon()	
{ 
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )	
	{
		for( INT32 j = 0; j < MAX_THROWWEAPON_SLOP; j++)
		{
			m_ThrowWeaponGameInfo[ i][ j].Reset(); 
		}
	}
}


static I3SG_TRAVERSAL_RESULT __s_Traverse_Object_MaxSize( i3Node * pNode, void * pUserData)
{
	if( pNode != nullptr)
	{
		if( pNode->GetBound() )
		{
			REAL32 * pMaxSize = (REAL32*)pUserData;
			*pMaxSize = MAX( *pMaxSize, i3Vector::Length( pNode->GetBound()->GetMax()));
			*pMaxSize = MAX( *pMaxSize, i3Vector::Length( pNode->GetBound()->GetMin()));
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void BattleObjContext::AddObject( i3Object *pObj )
{
	INT32	nIndex = 0;
	INT32	nCount = 0;

	switch( pObj->getNetworkSyncMode() )
	{
	case I3_NETWORK_GAMEOBJECT_TYPE_NONE :
		nIndex = (INT32)m_ObjectCountInfo.m_pNoneNetObjectList.size();
		m_ObjectCountInfo.m_pNoneNetObjectList.push_back( pObj);
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_STATIC :
		// WeaponBox는 리스트를 달리 합니다.. 2010.10.15 swoongoo
		// Sysnc를 맞추기 위해 List를 따로 관리합니다.
		if( i3::same_of<CGameObjectWeaponBox*>(pObj))
		{
			nIndex = (INT32)m_ObjectCountInfo.m_pControledObjectList.size();
			m_ObjectCountInfo.m_pControledObjectList.push_back( pObj);
			if( m_ObjectCountInfo.m_pControledObjectList.size() >= OBJ_CONTROLED_COUNT)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Controled Object Count overflow...");
				return;
			}
		}
		else if( i3::same_of<CGameObjectEvent*>(pObj))
		{
			nIndex = (INT32)m_ObjectCountInfo.m_pEventObjectList.size();
			m_ObjectCountInfo.m_pEventObjectList.push_back( pObj);
			if( m_ObjectCountInfo.m_pEventObjectList.size() >= OBJ_CONTROLED_COUNT)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Controled Object Count overflow...");
				return;
			}
		}
		else
		{
			nIndex = (INT32)m_ObjectCountInfo.m_pStaticObjectList.size();
			m_ObjectCountInfo.m_pStaticObjectList.push_back( pObj );
			
			if( m_ObjectCountInfo.m_pStaticObjectList.size() >= OBJ_STATIC_COUNT )
			{
				I3PRINTLOG(I3LOG_FATAL,  "Static Object count overflow." );
				return;
			}
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_MOVE :
		nIndex = (INT32)m_ObjectCountInfo.m_pMoveObjectList.size();
		m_ObjectCountInfo.m_pMoveObjectList.push_back( pObj );
		if( m_ObjectCountInfo.m_pMoveObjectList.size() >= OBJ_MOVE_COUNT )
		{
			I3PRINTLOG(I3LOG_FATAL,  "Move Object count overflow." );
			return;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC :
		nIndex = (INT32)m_ObjectCountInfo.m_pDynamicObjectList.size();
		m_ObjectCountInfo.m_pDynamicObjectList.push_back( pObj );
		if( m_ObjectCountInfo.m_pDynamicObjectList.size() >= OBJ_DYNAMIC_COUNT )
		{
			I3PRINTLOG(I3LOG_FATAL,  "Dynamic Object count overflow." );
			return;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_ANIM :
		nIndex = m_ObjectCountInfo.m_pAnimObjectList.size();
		m_ObjectCountInfo.m_pAnimObjectList.push_back( pObj );
		if( m_ObjectCountInfo.m_pAnimObjectList.size() >= OBJ_ANIM_COUNT )
		{
			I3PRINTLOG(I3LOG_FATAL,  "Anim Object count overflow." );
			return;
		}
		break;
	default :		I3PRINTLOG(I3LOG_FATAL,  "Invalid Object type." );		break;
	}

	nCount = m_ObjectCountInfo.m_pTotalObjectList.size();
	m_ObjectCountInfo.m_pTotalObjectList.push_back( pObj);

	if( m_ObjectCountInfo.m_pTotalObjectList.size() >= OBJ_TOTAL_COUNT )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Object count overflow." );
		return;
	}

	I3ASSERT( nCount == pObj->getNetIndex());

	{
		GAMEINFO_OBJECT * pObjInfo = BattleObjContext::setGameInfo_Object( nCount );
		pObjInfo->_pObjectRef		= pObj;
		pObjInfo->_nType			= pObj->getNetworkSyncMode();
		pObjInfo->_nInfoSlotIdx		= (UINT8) nCount;
		i3Vector::Copy( &pObjInfo->_tAssistData._vPostPos, pObj->GetPos() );
		pObjInfo->_tNetData.setHP( (UINT16) pObj->getCurrentHP());
		pObjInfo->_tNetData.setPosition( pObj->GetPos() );
		QUATERNION q;
		i3Matrix::GetRotateQuat( &q, pObj->GetMatrix());
		pObjInfo->_tNetData.setRotation( &q);
	}

	// 실제 포지션과 Shape의 중점이 다릅니다.
	// 하나의 모델에서 Shape의 중점을 구합니다.
	{
		// 피직스에 버그가 있어 BoundBox의 Center로 세팅하게 합니다.
		VEC3D vec;
		i3BoundBox box;
		pObj->getSceneObject()->GetWrappingBound(&box);
		box.GetCenter(&vec);
		i3Vector::Sub( &vec, &vec, pObj->GetPos());

		pObj->setShapeCenterPos( &vec );
	}

	/*
	// 오브젝트의 크기에 대한 offset
	// 폭파 무기에 대한 거리 측정에 사용합니다.
	REAL32 rRadius = 0.f;
	if( pObj->getSceneObject()->GetBound() != nullptr)
	{
	rRadius = MAX( i3Vector::Length( pObj->getSceneObject()->GetBound()->GetMin()), i3Vector::Length( pObj->getSceneObject()->GetBound()->GetMax()));
	}
	else
	{
	i3Scene::Traverse( pObj->getSceneObject(), (I3_SG_TRAVERSE_PROC) __s_Traverse_Object_MaxSize, (void*)&rRadius);
	}

	m_ObjectGameInfo[ nCount ]._rRadius = rRadius;
	I3TRACE( "Object Size : %.4f\n", rRadius);
	*/
}

void BattleObjContext::ResetEventHandler( i3GameObj * pObj)
{
	CStageBattle* pStage = g_pFramework->GetStageBattle();
	if( pStage != nullptr)
	{
		StageObjectListPtr	pObjectList = pStage->GetStageObjectList();
		if( pObjectList != nullptr )
		{
			for( UINT32 i = 0; i < pObjectList->size(); i++ )
			{
				i3Object * pGameObject = (*pObjectList)[i];
				if( pGameObject->getEventSender() == pObj)
					pGameObject->setEventSender( nullptr);
			}
		}
	}
}

#if !defined( I3_NO_PROFILE )

void BattleObjContext::CrashAllStageObjects( void)
{
	for( INT32 i = 0; i < OBJ_TOTAL_COUNT; ++i)
	{
		const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::getGameInfo_Object(i); 	// 모든 오브젝트 데이터

		i3Object * pObj = pObjectInfo->_pObjectRef;
		if( pObj != nullptr)
			pObj->Damage( pObj->getMaxHP(), nullptr, nullptr);
	}
}

#endif

void BattleObjContext::CreateBattle( void)
{
	ResetGameInfo_ThrowWeapon();

#ifdef DOMI_STING_POISON_GRENADE
	setDomiDinoGrenades()->DeleteAll();
#endif

	for( INT32 i = 0; i < OBJ_TOTAL_COUNT; i++ )
		setGameInfo_Object(i)->ResetGameInfoObject();

	for( INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT; i++)
		setGameInfo_DropedWeapon(i)->Reset();

	//ISNETWORK
		m_ObjectCountInfo.Reset();
}

void BattleObjContext::getObjectMission( i3::vector<i3GameObj *>& pObject)
{
	CGameObjectEvent* pEvent = nullptr;
	for( size_t i = 0; i < m_ObjectCountInfo.m_pEventObjectList.size(); i++ )
	{
		pEvent = (CGameObjectEvent*)m_ObjectCountInfo.m_pEventObjectList[i];
		if( pEvent->getEventType() == OBJ_EVENT_MISSION_TRIGGER)
			pObject.push_back((i3GameObj *)pEvent);
	}
}

void BattleObjContext::OnObjectMission( bool bEnable)
{
	CGameObjectEvent* pEvent = nullptr;
	for( size_t i = 0; i < m_ObjectCountInfo.m_pEventObjectList.size(); i++ )
	{
		pEvent = (CGameObjectEvent*)m_ObjectCountInfo.m_pEventObjectList[i];
		if( pEvent->getEventType() == OBJ_EVENT_MISSION_TRIGGER)
		{
			GlobalFunc::ChangeObjState_( pEvent, (INT32) bEnable, 0.f, true);
		}
	}
}

void BattleObjContext::ObjectHitWriteFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_ObjFuncMap_HitWrite.find( (UINT32)ObjIdx);

	if ( it != m_ObjFuncMap_HitWrite.end() )
	{
		netFunc & mf = it->second;
		(this->*mf)(pObj, pChara);
	}
	else
		I3TRACE( "잘못된 이벤트 function Event");
}
void BattleObjContext::ObjectHitReceveFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_ObjFuncMap_HitReceve.find( (UINT32)ObjIdx);

	if ( it != m_ObjFuncMap_HitReceve.end() )
	{
		netFunc & mf = it->second;
		(this->*mf)(pObj, pChara);
	}
	else
		I3TRACE( "잘못된 이벤트 function Event");
}
void BattleObjContext::__Register_Object_Function( void)
{
	RegisterFunctionWriteObj( OBJ_EVENT_ESCAPE	,				&BattleObjContext::__Function_Object_Write_Escape);
	RegisterFunctionWriteObj( OBJ_EVENT_MISSION,				&BattleObjContext::__Function_Object_Write_Mission);

	RegisterFunctionReceveObj( OBJ_EVENT_MISSION,				&BattleObjContext::__Function_Object_Receve_Mission);
	RegisterFunctionReceveObj( OBJ_EVENT_MISSION_TRIGGER,		&BattleObjContext::__Function_Object_Write_Mission_Trigger);
}

void BattleObjContext::RegisterFunctionWriteObj( UINT32 eventType, netFunc func)
{
	m_ObjFuncMap_HitWrite.insert(i3::unordered_map< UINT32, netFunc >::value_type( eventType, func));
}
void BattleObjContext::RegisterFunctionReceveObj( UINT32 eventType, netFunc func)
{
	m_ObjFuncMap_HitReceve.insert(i3::unordered_map< UINT32, netFunc >::value_type( eventType, func));
}

void		BattleObjContext::__Function_Object_Write_Mission(i3Object * pObj, CGameCharaBase * pChara)
{
	CGameObjectEvent * ObjEvent = (CGameObjectEvent*) pObj;
	if( ( ObjEvent->getEventUseCount() > 0) && ( pChara->getTeam() == TEAM_RED ))
		GAMEEVENT::Write_Chara_UseObject( pChara, pObj, NET_ACTIONKEY_USEOBJECT);
}

void		BattleObjContext::__Function_Object_Write_Escape(i3Object * pObj, CGameCharaBase * pChara)
{
	if(CGameMissionManager::i()->isMyUsurpatio() == true
		&&  pChara->getTeam() == TEAM_RED )
	{
		UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
		GAMEEVENT::Write_Chara_TouchDown( pChara, ObjectIdx );
	}
}

void		BattleObjContext::__Function_Object_Receve_Mission(i3Object * pObj, CGameCharaBase * pChara)
{
	CGameObjectEvent * ObjEvent = (CGameObjectEvent*) pObj;
	if( 0xFF == CGameMissionManager::i()->getUsurpationIdx())
	{
		ObjEvent->SetEnable(true);
		ObjEvent->setEventUseCount(1);
		CGameMissionManager::i()->CopyMissionObjPos(ObjEvent->GetPos());
	}
	else
	{
		ObjEvent->SetEnable(false);
		ObjEvent->setEventUseCount(0);
	}
}

void		BattleObjContext::__Function_Object_Write_Mission_Trigger(i3Object * pObj, CGameCharaBase * pChara)
{

}


