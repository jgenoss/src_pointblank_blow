#include "i3FrameworkPCH.h"
#include "i3GameSceneInfo.h"
#include "i3Framework.h"

#include "i3GameResSceneGraph.h"
#include "i3GameResAnimation.h"
#include "i3GameResSound.h"
#include "i3GameResTexture.h"
#include "i3GameResChara.h"
#include "i3GameResWorld.h"
#include "i3GameResBinary.h"
#include "i3GameResObject.h"
#include "i3GameResRespawn.h"

#include "i3GameResPath.h"
#include "i3GameResLight.h"
#include "i3GameResEvent.h"

#include "i3GamePath.h"
#include "i3LightObject.h"
#include "i3CameraObject.h"
#include "i3Trigger.h"

#include "i3Base/itl/container_util.h"
#include "i3Base/smart_ptr/make_shared.h"


UINT32 i3GameSceneInfo::_LoadInstance4( i3Stream * pStream)
{
	bool bValid = false;
	UINT32	nResult, Rc = 0;
	INT32	i;
	i3Stage						*	m_pCurStage = m_pFramework->GetCurrentStage();

	nResult = pStream->Read( &m_Pack, sizeof( m_Pack));
	I3_CHKIO( nResult);

	I3TRACE( "[INSTANCE MANAGER] : Total %d objects\n", m_Pack.m_nInstanceCount);

	//배열 사이즈 
	if( m_spInstList != nullptr)
	{
		i3::for_each(m_spInstList->begin(), m_spInstList->end(), i3::fu::functor_safe( i3::mem_fun( &i3GameObj::Release )) );
		m_spInstList.reset();
//		m_pInstList->SafeReleaseAll();
//		I3_SAFE_RELEASE( m_pInstList);
	}

	m_spInstList = i3::make_shared< i3::vector<i3GameObj*> >();	//List::new_object();
	m_spInstList->reserve( m_Pack.m_nInstanceCount );

//	m_pInstList->SetAllocationUnit( m_Pack.m_nInstanceCount);

	// PreCreate Instances
	{
		I3ASSERT( m_pInstInfo == nullptr);

		m_pInstInfo	= (I3_GAMESCENE_INSTANCE_2 *) i3MemAlloc( sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);

		i3mem::FillZero( m_pInstInfo, sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);

		for( i = 0; i < (INT32)m_Pack.m_nInstanceCount; ++i)
		{
			I3_GAMESCENE_INSTANCE_2 * pInstInfo = m_pInstInfo + i;

			//Offset, index
			pInstInfo->_nIndex	= i;

			// read instance info
			{
				i3::pack::GAME_INSTANCE instInfo;

				Rc = pStream->Read( &instInfo, sizeof( i3::pack::GAME_INSTANCE));
				I3_CHKIO( Rc);
				nResult += Rc;

				pInstInfo->_Instance.CopyFrom( &instInfo);
			
			}

			bValid = true;

			//Create Instance By Meta
			{
				pInstInfo->_pMeta = i3ClassMetaSystem::i()->find_class_meta(pInstInfo->_Instance.nHashCodeInstClass);
					//i3ClassMeta::FindClassMetaByHash( pInstInfo->_Instance.nHashCodeInstClass);

				I3ASSERT( pInstInfo->_pMeta != nullptr);

				pInstInfo->_pObject	= (i3GameObj *)pInstInfo->_pMeta->create_instance();  //CREATEINSTANCE( pInstInfo->_pMeta);

				I3ASSERT( pInstInfo->_pObject != nullptr);
			}

			//find resource by hash
			{
				//UINT32 hash[4];
				UINT128 hash( pInstInfo->_Instance.HashCode[2], pInstInfo->_Instance.HashCode[1], pInstInfo->_Instance.HashCode[0], pInstInfo->_Instance.nHashCodeResPath);

				pInstInfo->_pRes = m_pResManager->FindResourceByHash( & hash);
			}

			//
			pInstInfo->_nOffsetAdditionalData	= pStream->GetPosition();
			if( pInstInfo->_Instance.nGameDataSize + pInstInfo->_Instance.nNameSize > 0)
			{
				pStream->SetPosition( pInstInfo->_Instance.nGameDataSize + pInstInfo->_Instance.nNameSize, STREAM_CURRENT);
			}
		}
	}

	{
		for( i = 0; i < (INT32) m_Pack.m_nInstanceCount; ++i)
		{
			I3_GAMESCENE_INSTANCE_2	* pInstInfo = m_pInstInfo + i;

			i3ClassMeta				* pMeta		= pInstInfo->_pMeta;
			i3GameRes				* pRes		= pInstInfo->_pRes;
			i3GameObj				* pObject	= pInstInfo->_pObject;
			i3::pack::GAME_INSTANCE_2 * pInst	= &pInstInfo->_Instance;

			// load additional data
			if( pInst->nGameDataSize > 0)
			{
				pStream->SetPosition( pInstInfo->_nOffsetAdditionalData, STREAM_BEGIN);

				Rc = pObject->OnLoadGameData( this, nullptr, pInst->nGameDataSize);
				I3_CHKIO( Rc);
				nResult += Rc;

				//게임 데이터로 지정된 영역의 크기와 읽어들인 크기가 동일해야한다.
				//그렇다는 보장은 없지만 초기 버그찾기 쉽게하기위해 일단 넣어둡니다.
				I3ASSERT( Rc == pInst->nGameDataSize);
			}

			char conv[MAX_PATH];
			if( pInst->nNameSize > 0)
			{
				pStream->SetPosition( pInstInfo->_nOffsetAdditionalData + pInst->nGameDataSize, STREAM_BEGIN);
				
				Rc  = pStream->Read( conv, pInst->nNameSize);
				I3_CHKIO( Rc);
				nResult += Rc;
			}
			conv[pInst->nNameSize] = 0;

			//Respawn
			if( i3::same_of<i3RespawnObj*>(pMeta) )
			{
				i3RespawnObj * pObj = (i3RespawnObj *)pObject;

				pObj->setRespawnStyle( pInst->nParam1);

				m_pCurStage->addRespawnObject( pObj);
			}
			else if( i3::same_of<i3GamePath*>(pMeta))
			{
				if( pRes != nullptr)
				{
					i3GamePath		* pNewObject = (i3GamePath*)pObject;
					i3GameResPath * pResObject = (i3GameResPath*)pRes;

					i3GamePath * pSrcPath = pResObject->getPath();

					pSrcPath->CopyTo( pNewObject, I3_COPY_REF);

					m_pCurStage->AddChild( pNewObject);
				}
				else
				{
					I3PRINTLOG(I3LOG_WARN,  "[INSTANCE %d] : %d Object에 대한 Resource를 찾을 수 없습니다. 생성하지 않습니다.", i);
					bValid = false;
				}
			}
			else if( i3::same_of<i3LightObject*>(pMeta))
			{
				if( pRes != nullptr)
				{
					i3GameResLight * pResLight = (i3GameResLight*)pRes;
					i3LightObject  * pLightObj = pResLight->getLightObject();
					i3Light *		pLight = pLightObj->getLightAttr()->GetLightObject();

					m_pFramework->addLight( pLight);

					{
						i3GameObj * pTempObj = i3GameObj::new_object();

						_SetInstanceMatrix( pTempObj, pInst);

						pLight->SetWorldSpaceTransform( pTempObj->GetMatrix());
						I3_SAFE_RELEASE( pTempObj);
					}

					//LIght는 셋팅을 하고나선 없애버립니다.
					I3_SAFE_ADDREF( pObject);
					I3_SAFE_RELEASE( pObject);
				}

				continue;
			}
			else if( (i3::kind_of<i3Object*>(pMeta)) && (i3::kind_of<i3GameResObject*>(pRes)))
			{
				if( pRes != nullptr)
				{
					i3Object		* pNewObject = (i3Object*)pObject;
					i3GameResObject * pResObject = (i3GameResObject*)pRes;

					pNewObject->Create( m_pFramework, pResObject);

					pNewObject->setFirstParam( (I3_OBJECT_PARAM_FIRST)pInst->nParam1);
					pNewObject->setSecondParam( (I3_OBJECT_PARAM_SECOND)pInst->nParam2);
					pNewObject->setThirdParam( (I3_OBJECT_PARAM_THIRD)pInst->nParam3);

					//pNewObject->setJumpPos( Instance.vJumpPos.x, Instance.vJumpPos.y, Instance.vJumpPos.z);
					if( conv[0] != 0)
					{
						pNewObject->SetName( conv);
					}

					m_pCurStage->AddChild( pNewObject);
				}
				else
				{
					I3PRINTLOG(I3LOG_WARN,  "[INSTANCE %d] : %d Object에 대한 Resource를 찾을 수 없습니다. 생성하지 않습니다.", i);
					bValid = false;
				}
			}
			else if( i3::same_of< i3CameraObject* >(pMeta))
			{
				pObject->SetEnable( pInst->bInitEnable != 0);

				//SetTransform
				_SetInstanceMatrix( pObject, pInst);

				i3CameraObject * pCam = (i3CameraObject*)pObject;

				pCam->SetMatrix( pObject->GetMatrix());
				pCam->Create();

				m_pFramework->getSgContext()->addCamera( pCam->getCamera());

				pObject->OnChangeTime( 0.0f);

				AddInstance( pObject);

				continue;
			}
			else if( i3::kind_of<i3Trigger*>(pMeta))
			{
				i3Trigger * pTrigger = (i3Trigger*)pObject;

				MATRIX Mtx;
				
				// Rotate
				{
					QUATERNION	Quat;
					i3Quat::Set( &Quat, pInst->qRot.x, pInst->qRot.y, pInst->qRot.z, pInst->qRot.w);

					i3Matrix::SetRotateQuat( &Mtx, &Quat);
				}

				// Translate
				{
					VEC3D	Zero,Pos,Scale;
					
					//Size 
					i3Vector::Set( &Scale, pInst->vScale.x, pInst->vScale.y, pInst->vScale.z);

					i3Vector::Set( &Pos, pInst->vPos.x, pInst->vPos.y, pInst->vPos.z);
					i3Matrix::PostTranslate( &Mtx, &Pos);

					if( pInst->nTriggerType == 0)
					{
						pTrigger->CreateBoxTrigger( &Zero, &Scale, pInst->nTriggerMatchGroup, false, &Mtx);
					}
					else
					{
						pTrigger->CreateSphereTrigger( &Pos, pInst->vScale.x * 0.5f, pInst->nTriggerMatchGroup);
					}

					pTrigger->SetMatrix( &Mtx);
				}

				pTrigger->setInitState( pInst->bInitEnable != 0);
				
				if( i3::same_of<i3GameResEvent*>(pRes))
				{
					i3GameResEvent  *	pEvent	= (i3GameResEvent*)pRes;
					i3TimeEventGen	*	pGen	= (i3TimeEventGen*)pEvent->getTimeEvent();

					pTrigger->setTimeEventGen( pGen);
					if( pGen != nullptr)
					{
						pGen->setEventReceiver( pTrigger);
					}
				}

				i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);
				if( pAttachParent != nullptr)
				{
					pAttachParent->AddChild( pTrigger->getSceneObject());
				}

				m_pCurStage->AddChild( pTrigger);

				pTrigger->InitTrigger();

				continue;
			}
			else
			{
				//i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);

				pObject->BindSgRes( m_pFramework, (i3GameResSceneGraph *) pRes);

				m_pCurStage->AddChild( pObject);
			}

			if( bValid)
			{
				if((pObject->GetNode() != nullptr) && (pObject->getSgRes() != nullptr))
				{
					#if defined( I3_DEBUG)
					pObject->GetNode()->SetName( pObject->getSgRes()->GetNameString());
					#endif

					pObject->AttachScene();
				}

				pObject->SetEnable( pInst->bInitEnable != 0);

				//SetTransform
				_SetInstanceMatrix( pObject, pInst);

				pObject->Instantiate();

				//Set Dynamic State
				pObject->setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) pInst->nNetSyncMode);

		#if defined( I3_PHYSX)
				pObject->setInitialDynamicActiveState(  pInst->bInitEnable != 0);
				pObject->setInitialDynamicState( (I3GAME_DYNAMIC_STATE)pInst->nDynamicState);
		#endif

				pObject->OnChangeTime( 0.0f);

				if( i3::kind_of<i3World*>(pMeta))
				{
					m_pCurStage->setWorld( (i3World*)pObject);;
				}

				AddInstance( pObject);
			}
			else
			{
				I3TRACE( "[INSTANCE %d] Object를 제거합니다!!!!!!\n", i);
				I3_SAFE_ADDREF( pObject);
				I3_SAFE_RELEASE( pObject);
			}
		}
	}
	
	return nResult;
}

UINT32 i3GameSceneInfo::_LoadInstance3( i3Stream * pStream)
{
	bool bValid = true;
	UINT32	nResult, Rc = 0;
	INT32	i;

	i3Stage						*	m_pCurStage = m_pFramework->GetCurrentStage();

	nResult = pStream->Read( &m_Pack, sizeof( m_Pack));
	I3_CHKIO( nResult);

	I3TRACE( "[INSTANCE MANAGER] : Total %d objects\n", m_Pack.m_nInstanceCount);

	//배열 사이즈 
	if( m_spInstList != nullptr)
	{
		i3::cu::for_each_container(*m_spInstList, i3::fu::mem_fun_safe(&i3GameObj::Release));
		m_spInstList.reset();
//		m_pInstList->SafeReleaseAll();
//		I3_SAFE_RELEASE( m_pInstList);
	}

	m_spInstList =	i3::make_shared<i3::vector<i3GameObj* > >();		//	List::new_object();
	m_spInstList->reserve(m_Pack.m_nInstanceCount);
//	m_pInstList->SetAllocationUnit( m_Pack.m_nInstanceCount);

	// PreCreate Instances
	{
		I3ASSERT( m_pInstInfo == nullptr);

		m_pInstInfo	= (I3_GAMESCENE_INSTANCE_2 *)i3MemAlloc( sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);

		i3mem::FillZero( m_pInstInfo, sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);

		for( i = 0; i < (INT32)m_Pack.m_nInstanceCount; ++i)
		{
			I3_GAMESCENE_INSTANCE_2 * pInstInfo = m_pInstInfo + i;

			//Offset, index
			pInstInfo->_nIndex	= i;

			// read instance info
			{
				i3::pack::GAME_INSTANCE inst;

				Rc = pStream->Read( &inst, sizeof( i3::pack::GAME_INSTANCE) - sizeof( UINT32));
				I3_CHKIO( Rc);
				nResult += Rc;

				pInstInfo->_Instance.CopyFrom( &inst);
			}

			bValid = true;

			//Create Instance By Meta
			{
				pInstInfo->_pMeta = i3ClassMetaSystem::i()->find_class_meta( pInstInfo->_Instance.nHashCodeInstClass);
					//i3ClassMeta::FindClassMetaByHash( pInstInfo->_Instance.nHashCodeInstClass);

				I3ASSERT( pInstInfo->_pMeta != nullptr);

				pInstInfo->_pObject	= (i3GameObj *)	pInstInfo->_pMeta->create_instance();	//	CREATEINSTANCE( pInstInfo->_pMeta);

				I3ASSERT( pInstInfo->_pObject != nullptr);
			}

			//find resource by hash
			{
				//UINT32 hash[4];
				UINT128 hash( pInstInfo->_Instance.HashCode[2], pInstInfo->_Instance.HashCode[1], pInstInfo->_Instance.HashCode[0], pInstInfo->_Instance.nHashCodeResPath);

				pInstInfo->_pRes = m_pResManager->FindResourceByHash( & hash);
			}

			//
			pInstInfo->_nOffsetAdditionalData	= pStream->GetPosition();
			if( pInstInfo->_Instance.nGameDataSize > 0)
			{
				pStream->SetPosition( pInstInfo->_Instance.nGameDataSize, STREAM_CURRENT);
			}
		}
	}

	{
		for( i = 0; i < (INT32) m_Pack.m_nInstanceCount; ++i)
		{
			I3_GAMESCENE_INSTANCE_2	* pInstInfo = m_pInstInfo + i;

			i3ClassMeta				* pMeta		= pInstInfo->_pMeta;
			i3GameRes				* pRes		= pInstInfo->_pRes;
			i3GameObj				* pObject	= pInstInfo->_pObject;
			i3::pack::GAME_INSTANCE_2 * pInst	= &pInstInfo->_Instance;

			// load additional data
			if( pInst->nGameDataSize > 0)
			{
				pStream->SetPosition( pInstInfo->_nOffsetAdditionalData, STREAM_BEGIN);

				Rc = pObject->OnLoadGameData( this, nullptr, pInst->nGameDataSize);
				I3_CHKIO( Rc);
				nResult += Rc;

				//게임 데이터로 지정된 영역의 크기와 읽어들인 크기가 동일해야한다.
				//그렇다는 보장은 없지만 초기 버그찾기 쉽게하기위해 일단 넣어둡니다.
				I3ASSERT( Rc == pInst->nGameDataSize);
			}

			//Respawn
			if( i3::same_of<i3RespawnObj*>(pMeta))
			{
				i3RespawnObj * pObj = (i3RespawnObj *)pObject;

				pObj->setRespawnStyle( pInst->nParam1);

				m_pCurStage->addRespawnObject( pObj);
			}
			else if( i3::same_of<i3GamePath*>(pMeta))
			{

			}
			else if( i3::same_of< i3LightObject* >(pMeta))
			{
				if( pRes != nullptr)
				{
					i3GameResLight * pResLight = (i3GameResLight*)pRes;
					i3LightObject  * pLightObj = pResLight->getLightObject();
					i3Light *		pLight = pLightObj->getLightAttr()->GetLightObject();

					m_pFramework->addLight( pLight);

					{
						i3GameObj * pTempObj = i3GameObj::new_object();

						_SetInstanceMatrix( pTempObj, pInst);

						pLight->SetWorldSpaceTransform( pTempObj->GetMatrix());
						I3_SAFE_RELEASE( pTempObj);
					}

					//LIght는 셋팅을 하고나선 없애버립니다.
					I3_SAFE_ADDREF( pObject);
					I3_SAFE_RELEASE( pObject);
				}

				continue;
			}
			else if( (i3::kind_of<i3Object*>(pMeta)) && (i3::kind_of<i3GameResObject*>(pRes)))
			{
				if( pRes != nullptr)
				{
					i3Object		* pNewObject = (i3Object*)pObject;
					i3GameResObject * pResObject = (i3GameResObject*)pRes;

					pNewObject->Create( m_pFramework, pResObject);

					pNewObject->setFirstParam( (I3_OBJECT_PARAM_FIRST)pInst->nParam1);
					pNewObject->setSecondParam( (I3_OBJECT_PARAM_SECOND)pInst->nParam2);
					pNewObject->setThirdParam( (I3_OBJECT_PARAM_THIRD)pInst->nParam3);
					
					m_pCurStage->AddChild( pNewObject);
				}
				else
				{
					I3PRINTLOG(I3LOG_WARN,  "[INSTANCE %d] : %d Object에 대한 Resource를 찾을 수 없습니다. 생성하지 않습니다.", i);
					bValid = false;
				}
			}
			else if( i3::same_of< i3CameraObject* >(pMeta))
			{
				pObject->SetEnable( pInst->bInitEnable != 0);

				//SetTransform
				_SetInstanceMatrix( pObject, pInst);

				i3CameraObject * pCam = (i3CameraObject*)pObject;

				pCam->SetMatrix( pObject->GetMatrix());
				pCam->Create();

				m_pFramework->getSgContext()->addCamera( pCam->getCamera());

				pObject->OnChangeTime( 0.0f);

				AddInstance( pObject);

				continue;
			}
			else if( i3::kind_of< i3Trigger* >(pMeta))
			{
				i3Trigger * pTrigger = (i3Trigger*)pObject;

				MATRIX Mtx;
				
				// Rotate
				{
					QUATERNION	Quat;
					i3Quat::Set( &Quat, pInst->qRot.x, pInst->qRot.y, pInst->qRot.z, pInst->qRot.w);

					i3Matrix::SetRotateQuat( &Mtx, &Quat);
				}

				// Translate
				{
					VEC3D	Zero,Pos,Scale;
										
					//Size 
					i3Vector::Set( &Scale, pInst->vScale.x, pInst->vScale.y, pInst->vScale.z);

					i3Vector::Set( &Pos, pInst->vPos.x, pInst->vPos.y, pInst->vPos.z);
					i3Matrix::PostTranslate( &Mtx, &Pos);

					if( pInst->nTriggerType == 0)
					{
						pTrigger->CreateBoxTrigger( &Zero, &Scale, pInst->nTriggerMatchGroup, false, &Mtx);
					}
					else
					{
						pTrigger->CreateSphereTrigger( &Pos, pInst->vScale.x * 0.5f, pInst->nTriggerMatchGroup);
					}

					pTrigger->SetMatrix( &Mtx);
				}

				pTrigger->setInitState( pInst->bInitEnable != 0);
				
				if( i3::same_of<i3GameResEvent*>(pRes))
				{
					i3GameResEvent  *	pEvent	= (i3GameResEvent*)pRes;
					i3TimeEventGen	*	pGen	= (i3TimeEventGen*)pEvent->getTimeEvent();

					pTrigger->setTimeEventGen( pGen);
					if( pGen != nullptr)
					{
						pGen->setEventReceiver( pTrigger);
					}
				}

				i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);
				if( pAttachParent != nullptr)
				{
					pAttachParent->AddChild( pTrigger->getSceneObject());
				}

				m_pCurStage->AddChild( pTrigger);

				pTrigger->InitTrigger();

				continue;
			}
			else
			{
				pObject->BindSgRes( m_pFramework, (i3GameResSceneGraph *) pRes);

				m_pCurStage->AddChild( pObject);
			}

			if( bValid)
			{	
				if((pObject->GetNode() != nullptr) && (pObject->getSgRes() != nullptr))
				{
					#if defined( I3_DEBUG)
					pObject->GetNode()->SetName( pObject->getSgRes()->GetName());
					#endif

					pObject->AttachScene();
				}

				pObject->SetEnable( pInst->bInitEnable != 0);

				//SetTransform
				_SetInstanceMatrix( pObject, pInst);

				pObject->Instantiate();

				//Set Dynamic State
				pObject->setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) pInst->nNetSyncMode);

		#if defined( I3_PHYSX)
				pObject->setInitialDynamicActiveState(  pInst->bInitEnable != 0);
				pObject->setInitialDynamicState( (I3GAME_DYNAMIC_STATE)pInst->nDynamicState);
		#endif

				pObject->OnChangeTime( 0.0f);

				if( i3::kind_of<i3World*>(pMeta))
				{
					m_pCurStage->setWorld( (i3World*)pObject);;
				}

				AddInstance( pObject);
			}
			else
			{
				I3TRACE( "[INSTANCE %d] Object를 제거합니다!!!!!!\n", i);
				I3_SAFE_ADDREF( pObject);
				I3_SAFE_RELEASE( pObject);
			}
		}
	}
	
	return nResult;
}


UINT32 i3GameSceneInfo::_LoadInstance2( i3Stream * pStream)
{
	i3::pack::GAME_INSTANCE_PACK	pack;
	i3::pack::GAME_INSTANCE		Instance;
	bool bValid;

	i3Stage						*	m_pCurStage = m_pFramework->GetCurrentStage();

	UINT32	nResult, Rc = 0;
	INT32	i;

	nResult = pStream->Read( &pack, sizeof( pack));
	I3_CHKIO( nResult);

	I3TRACE( "[INSTANCE MANAGER] : Total %d objects\n", pack.m_nInstanceCount);

	//배열 사이즈 
	if( m_spInstList != nullptr)
	{
		i3::cu::for_each_container(*m_spInstList, i3::fu::mem_fun_safe(&i3GameObj::Release));
		m_spInstList.reset();
//		m_pInstList->SafeReleaseAll();
//		I3_SAFE_RELEASE( m_pInstList);
	}

	m_spInstList =	i3::make_shared<i3::vector<i3GameObj*> >();			// List::new_object();
	m_spInstList->reserve( pack.m_nInstanceCount );
	//m_pInstList->SetAllocationUnit( pack.m_nInstanceCount);

	for( i = 0; i < (INT32)pack.m_nInstanceCount; ++i)
	{
		{//
			Rc = pStream->Read( &Instance, sizeof( i3::pack::GAME_INSTANCE));
			I3_CHKIO( Rc);
			nResult += Rc;
		}

		bValid = true;

		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(Instance.nHashCodeInstClass);
			//i3ClassMeta::FindClassMetaByHash( Instance.nHashCodeInstClass);

		I3ASSERT( pMeta != nullptr);

		i3GameObj * pObject = (i3GameObj *)	pMeta->create_instance();	//	CREATEINSTANCE( pMeta);

		UINT128 hash( Instance.HashCode[2], Instance.HashCode[1], Instance.HashCode[0], Instance.nHashCodeResPath);

		i3GameRes * pRes = m_pResManager->FindResourceByHash( &hash);

		if( Instance.nGameDataSize > 0)
		{
			Rc = pObject->OnLoadGameData( this, nullptr, Instance.nGameDataSize);
			I3_CHKIO( Rc);
			nResult += Rc;
		}

		//Respawn
		if( i3::same_of<i3RespawnObj*>(pMeta))
		{
			i3RespawnObj * pObj = (i3RespawnObj *)pObject;

			pObj->setRespawnStyle( Instance.nParam1);

			m_pCurStage->addRespawnObject( pObj);
		}
		else if( i3::same_of<i3GamePath*>(pMeta))
		{

		}
		else if( i3::same_of<i3LightObject*>(pMeta))
		{
			if( pRes != nullptr)
			{
				i3GameResLight * pResLight = (i3GameResLight*)pRes;
				i3LightObject  * pLightObj = pResLight->getLightObject();
				i3Light *		pLight = pLightObj->getLightAttr()->GetLightObject();

				m_pFramework->addLight( pLight);

				{
					i3::pack::GAME_INSTANCE_2 inst;

					i3GameObj * pTempObj = i3GameObj::new_object();

					inst.CopyFrom( &Instance);

					_SetInstanceMatrix( pTempObj, &inst);

					pLight->SetWorldSpaceTransform( pTempObj->GetMatrix());
					I3_SAFE_RELEASE( pTempObj);
				}

				//LIght는 셋팅을 하고나선 없애버립니다.
				I3_SAFE_ADDREF( pObject);
				I3_SAFE_RELEASE( pObject);
			}

			continue;
		}
		else if( i3::kind_of<i3Object*>(pMeta))
		{
			if( pRes != nullptr)
			{
				i3Object		* pNewObject = (i3Object*)pObject;
				i3GameResObject * pResObject = (i3GameResObject*)pRes;

				pNewObject->Create( m_pFramework, pResObject);

				pNewObject->setFirstParam( (I3_OBJECT_PARAM_FIRST)Instance.nParam1);
				pNewObject->setSecondParam( (I3_OBJECT_PARAM_SECOND)Instance.nParam2);
				pNewObject->setThirdParam( (I3_OBJECT_PARAM_THIRD)Instance.nParam3);

				//pNewObject->setJumpPos( Instance.vJumpPos.x, Instance.vJumpPos.y, Instance.vJumpPos.z);

				m_pCurStage->AddChild( pNewObject);
			}
			else
			{
				I3PRINTLOG(I3LOG_WARN,  "[INSTANCE %d] : %d Object에 대한 Resource를 찾을 수 없습니다. 생성하지 않습니다.", i);
				bValid = false;
			}
		}
		else if( i3::kind_of<i3Trigger*>(pMeta))
		{
			i3Trigger * pTrigger = (i3Trigger*)pObject;

			MATRIX Mtx;
			
			// Rotate
			{
				QUATERNION	Quat;
				i3Quat::Set( &Quat, Instance.qRot.x, Instance.qRot.y, Instance.qRot.z, Instance.qRot.w);

				i3Matrix::SetRotateQuat( &Mtx, &Quat);
			}

			// Translate
			{
				VEC3D	Zero,Pos,Scale;
								
				//Size 
				i3Vector::Set( &Scale, Instance.vScale.x, Instance.vScale.y, Instance.vScale.z);

				i3Vector::Set( &Pos, Instance.vPos.x, Instance.vPos.y, Instance.vPos.z);
				i3Matrix::PostTranslate( &Mtx, &Pos);

				if( Instance.nTriggerType == 0)
				{
					pTrigger->CreateBoxTrigger( &Zero, &Scale, Instance.nTriggerMatchGroup, false, &Mtx);
				}
				else
				{
					pTrigger->CreateSphereTrigger( &Pos, Instance.vScale.x * 0.5f, Instance.nTriggerMatchGroup);
				}

				pTrigger->SetMatrix( &Mtx);
			}

			pTrigger->setInitState( Instance.bInitEnable != 0);
			
			if( i3::same_of<i3GameResEvent*>(pRes))
			{
				i3GameResEvent  *	pEvent	= (i3GameResEvent*)pRes;
				i3TimeEventGen	*	pGen	= (i3TimeEventGen*)pEvent->getTimeEvent();

				pTrigger->setTimeEventGen( pGen);
				if( pGen != nullptr)
				{
					pGen->setEventReceiver( pTrigger);
				}
			}

			i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);
			if( pAttachParent != nullptr)
			{
				pAttachParent->AddChild( pTrigger->getSceneObject());
			}

			m_pCurStage->AddChild( pTrigger);

			pTrigger->InitTrigger();

			continue;
		}
		else
		{
		//	i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);

			pObject->BindSgRes( m_pFramework, (i3GameResSceneGraph *) pRes);

			m_pCurStage->AddChild( pObject);
		}

		if( bValid)
		{	
			if((pObject->GetNode() != nullptr) && (pObject->getSgRes() != nullptr))
			{
				#if defined( I3_DEBUG)
				pObject->GetNode()->SetName( pObject->getSgRes()->GetNameString());
				#endif

				pObject->AttachScene();
			}

			pObject->SetEnable( Instance.bInitEnable != 0);

			//SetTransform
			{
				i3::pack::GAME_INSTANCE_2 inst;

				inst.CopyFrom( &Instance);
				_SetInstanceMatrix( pObject, &inst);
			}

			pObject->Instantiate();

			//Set Dynamic State
			pObject->setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) Instance.nNetSyncMode);

	#if defined( I3_PHYSX)
			pObject->setInitialDynamicActiveState(  Instance.bInitEnable != 0);
			pObject->setInitialDynamicState( (I3GAME_DYNAMIC_STATE)Instance.nDynamicState);
	#endif

			pObject->OnChangeTime( 0.0f);

			if( i3::kind_of<i3World*>(pMeta))
			{
				m_pCurStage->setWorld( (i3World*)pObject);;
			}

			AddInstance( pObject);
		}
		else
		{
			I3TRACE( "[INSTANCE %d] Object를 제거합니다!!!!!!\n", i);
			I3_SAFE_ADDREF( pObject);
			I3_SAFE_RELEASE( pObject);
		}
	}

	return nResult;
}

UINT32 i3GameSceneInfo::_LoadInstance( i3Stream * pStream)
{
	i3::pack::GAME_INSTANCE_PACK	pack;
	i3::pack::GAME_INSTANCE	*	pFirstInstance;
	i3::pack::GAME_INSTANCE	*	pInst;

	i3Stage						*	m_pCurStage = m_pFramework->GetCurrentStage();

	UINT32	nResult, Rc = 0;
	INT32	i;

	nResult = pStream->Read( &pack, sizeof( pack));
	I3_CHKIO( nResult);

	//배열 사이즈 
	if( m_spInstList != nullptr)
	{
		i3::cu::for_each_container(*m_spInstList, i3::fu::mem_fun_safe(&i3GameObj::Release) );
		m_spInstList.reset();
//		m_pInstList->SafeReleaseAll();
//		I3_SAFE_RELEASE( m_pInstList);
	}

	m_spInstList =	i3::make_shared< i3::vector<i3GameObj*> >();	// List::new_object();
	m_spInstList->reserve(pack.m_nInstanceCount );
//	m_pInstList->SetAllocationUnit( pack.m_nInstanceCount);

	pFirstInstance = (i3::pack::GAME_INSTANCE*)i3MemAlloc( sizeof( i3::pack::GAME_INSTANCE) * pack.m_nInstanceCount);
	{
		Rc = pStream->Read( pFirstInstance, sizeof( i3::pack::GAME_INSTANCE) * pack.m_nInstanceCount);

		I3_CHKIO( Rc);

		nResult += Rc;
	}

	for( i = 0; i < (INT32)pack.m_nInstanceCount; ++i)
	{
		pInst = pFirstInstance + i;

		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(pInst->nHashCodeInstClass );
			//i3ClassMeta::FindClassMetaByHash( pInst->nHashCodeInstClass);

		I3ASSERT( pMeta != nullptr);

		i3GameObj * pObject = (i3GameObj *)	pMeta->create_instance();	// CREATEINSTANCE( pMeta);

		UINT128 hash( pInst->HashCode[2], pInst->HashCode[1], pInst->HashCode[0], pInst->nHashCodeResPath);

		i3GameRes * pRes = m_pResManager->FindResourceByHash( &hash);

		//Respawn
		if( i3::same_of<i3RespawnObj*>(pMeta))
		{
			i3RespawnObj * pObj = (i3RespawnObj *)pObject;

			pObj->setRespawnStyle( pInst->nParam1);

			m_pCurStage->addRespawnObject( pObj);
		}
		else if( i3::same_of<i3GamePath*>(pMeta))
		{

		}
		else if( i3::same_of<i3LightObject*>(pMeta))
		{
			i3GameResLight * pResLight = (i3GameResLight*)pRes;
			i3LightObject  * pLightObj = pResLight->getLightObject();
			i3Light *		pLight = pLightObj->getLightAttr()->GetLightObject();

			m_pFramework->addLight( pLight);

			{
				i3GameObj * pTempObj = i3GameObj::new_object();
				i3::pack::GAME_INSTANCE_2 inst;

				inst.CopyFrom( pInst);
				_SetInstanceMatrix( pTempObj, &inst);

				pLight->SetWorldSpaceTransform( pTempObj->GetMatrix());
				I3_SAFE_RELEASE( pTempObj);
			}

			//LIght는 셋팅을 하고나선 없애버립니다.
			I3_SAFE_ADDREF( pObject);
			I3_SAFE_RELEASE( pObject);
			continue;
		}
		else if( i3::kind_of< i3Object* >(pMeta))
		{
			i3Object		* pNewObject = (i3Object*)pObject;
			i3GameResObject * pResObject = (i3GameResObject*)pRes;

			pNewObject->Create( m_pFramework, pResObject);

			pNewObject->setFirstParam( (I3_OBJECT_PARAM_FIRST)pInst->nParam1);
			pNewObject->setSecondParam( (I3_OBJECT_PARAM_SECOND)pInst->nParam2);
			pNewObject->setThirdParam( (I3_OBJECT_PARAM_THIRD)pInst->nParam3);

			//pNewObject->setJumpPos( pInst->vJumpPos.x, pInst->vJumpPos.y, pInst->vJumpPos.z);

			m_pCurStage->AddChild( pNewObject);
		}
		else if( i3::kind_of< i3Trigger* >(pMeta))
		{
			i3Trigger * pTrigger = (i3Trigger*)pObject;

			MATRIX Mtx;
			
			// Rotate
			{
				QUATERNION	Quat;
				i3Quat::Set( &Quat, pInst->qRot.x, pInst->qRot.y, pInst->qRot.z, pInst->qRot.w);

				i3Matrix::SetRotateQuat( &Mtx, &Quat);
			}

			// Translate
			{
				VEC3D	Zero,Pos,Scale;
								
				//Size 
				i3Vector::Set( &Scale, pInst->vScale.x, pInst->vScale.y, pInst->vScale.z);

				i3Vector::Set( &Pos, pInst->vPos.x, pInst->vPos.y, pInst->vPos.z);
				i3Matrix::PostTranslate( &Mtx, &Pos);

				if( pInst->nTriggerType == 0)
				{
					pTrigger->CreateBoxTrigger( &Zero, &Scale, pInst->nTriggerMatchGroup, false, &Mtx);
				}
				else
				{
					pTrigger->CreateSphereTrigger( &Pos, pInst->vScale.x * 0.5f, pInst->nTriggerMatchGroup);
				}

				pTrigger->SetMatrix( &Mtx);
			}

			pTrigger->setInitState( pInst->bInitEnable != 0);
			
			if( i3::same_of<i3GameResEvent*>(pRes))
			{
				i3GameResEvent  *	pEvent	= (i3GameResEvent*)pRes;
				i3TimeEventGen	*	pGen	= (i3TimeEventGen*)pEvent->getTimeEvent();

				pTrigger->setTimeEventGen( pGen);
				if( pGen != nullptr)
				{
					pGen->setEventReceiver( pTrigger);
				}

				I3_SAFE_RELEASE( pGen);
			}

			i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);
			if( pAttachParent != nullptr)
			{
				pAttachParent->AddChild( pTrigger->getSceneObject());
			}

			m_pCurStage->AddChild( pTrigger);

			pTrigger->InitTrigger();

			continue;
		}
		else
		{
			//i3Node	*	pAttachParent = m_pFramework->OnQueryAttachNode( pRes);

			pObject->BindSgRes( m_pFramework, (i3GameResSceneGraph *) pRes);

			m_pCurStage->AddChild( pObject);
		}

		if((pObject->GetNode() != nullptr) && (pObject->getSgRes() != nullptr))
		{
			#if defined( I3_DEBUG)
			pObject->GetNode()->SetName( pObject->getSgRes()->GetNameString());
			#endif

			pObject->AttachScene();
		}

		pObject->SetEnable( pInst->bInitEnable != 0);

		//SetTransform
		{
			i3::pack::GAME_INSTANCE_2 inst;

			inst.CopyFrom( pInst);
			_SetInstanceMatrix( pObject, &inst);
		}

		pObject->Instantiate();

		//Set Dynamic State
		pObject->setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) pInst->nNetSyncMode);

#if defined( I3_PHYSX)
		pObject->setInitialDynamicActiveState(  pInst->bInitEnable != 0);
		pObject->setInitialDynamicState( (I3GAME_DYNAMIC_STATE)pInst->nDynamicState);
#endif

		pObject->OnChangeTime( 0.0f);

		if( i3::kind_of<i3World* >(pMeta))
		{
			m_pCurStage->setWorld( (i3World*)pObject);;
		}
		
		AddInstance( pObject);
	}

	I3MEM_SAFE_FREE_POINTER( pFirstInstance);

	return nResult;
}