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

#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/itl/container_util.h"

#include "i3Base/string/compare/generic_is_niequal.h"


I3_CLASS_INSTANCE( i3GameSceneInfo);

/////////////////////////////////////////////////////////////////////////////////////////////////////

i3GameSceneInfo::~i3GameSceneInfo( void)
{
	I3MEM_SAFE_FREE_POINTER( m_pInstInfo);

	m_spResourceList.reset();

	if( m_spInstList != nullptr)
	{
		for( size_t i = 0; i < m_spInstList->size(); ++i)
		{
			i3GameObj * pObj = (*m_spInstList)[i];

			if( pObj != nullptr)
			{
				i3Node * pNode = pObj->GetNode();
				pNode->RemoveFromParent();

				i3GameNode * pParent = (i3GameNode*)pObj->getParent();
				if( pParent != nullptr)
				{
					pParent->RemoveChild( pObj);
				}

				#if !defined( I3_NO_PROFILE)
				if( i3::kind_of<i3Object*>(pObj))
				{
					if( pObj->GetRefCount() != 1)
					{
						I3TRACE("스테이지가 종료되었음에도 불구하고 남아있는 오브젝트가 있습니다\n");
					}
				}
				#endif

				I3_MUST_RELEASE( pObj);
			}
		}
		
	//	I3_SAFE_RELEASE( m_pInstList);
		m_spInstList.reset();
	}

//	I3_SAFE_RELEASE( m_pStageObjectList);
	m_spStageObjectList.reset();


	I3_SAFE_RELEASE( m_pStream);
}

bool	i3GameSceneInfo::Create( i3Framework * pFramework)
{
	m_pFramework = pFramework;
	if( m_pFramework == nullptr)
		return false;

	m_pResManager = m_pFramework->GetResourceManager();
	if( m_pResManager == nullptr)
		return false;

	return true;
}

bool i3GameSceneInfo::Load( const char * pszStageName)
{
	if( pszStageName == nullptr)
	{
		I3TRACE("GameScene Name is nullptr\n");

		return false;
	}

	setStageName( pszStageName);

	char name[MAX_PATH];
	sprintf( name, "Scene/%s/%s.RSC", pszStageName, pszStageName);

	if( !i3FileStream::isFile( name))
	{
		I3TRACE("%s have no SceneInfoResource\n", pszStageName);

		return false;
	}

	return m_pResManager->LoadRSC( name);
}

UINT32 i3GameSceneInfo::InitInstance(void)
{
	if( m_strStageName.empty())
	{
		I3TRACE("스테이지의 이름을 설정하여 주십시오\n");

		return STREAM_ERR;
	}

	char name[MAX_PATH];
	sprintf( name, "Scene/%s/%s.i3ins", m_strStageName.c_str(), m_strStageName.c_str());

	return LoadInstance( name);
}

UINT32 i3GameSceneInfo::Save()
{
	if(  m_strStageName.empty())
	{
		I3TRACE("스테이지의 이름을 설정하여 주십시오\n");

		return false;
	}

	char	conv[MAX_PATH];
	sprintf( conv, "Scene/%s/%s.RSC", m_strStageName.c_str(), m_strStageName.c_str());

	if( SaveResource( conv) == false)
	{
		return false;
	}

	sprintf( conv, "Scene/%s/%s.i3ins", m_strStageName.c_str(), m_strStageName.c_str());
	if( SaveInstance( conv) == STREAM_ERR)
	{
		return false;
	}

	return true;
}

void i3GameSceneInfo::AddResource( i3GameRes	*	pRes)
{
	if( m_spResourceList == nullptr)
	{
		m_spResourceList = i3::make_shared< i3::vector<i3GameRes*> >();		//List::new_object();
		m_spResourceList->reserve( 32);
	}

	m_spResourceList->push_back( pRes);
}

void i3GameSceneInfo::AddInstance( i3GameObj * pObj)
{
	if( m_spInstList == nullptr)
	{
		m_spInstList = i3::make_shared<i3::vector<i3GameObj*> >();		// List::new_object();
		m_spInstList->reserve( 128);
	}

	m_spInstList->push_back( pObj);

	// pObj는 nullptr일 수 있다!!!!
	// Bug 아님.
	if( pObj != nullptr)
	{
		I3_MUST_ADDREF( pObj);

		if( i3::kind_of<i3Object*>(pObj))
		{
			AddStageObject((i3Object*) pObj);
		}
	}
}

//StageObject List는 RefCount를 사용하지 않습니돠~ raja
void i3GameSceneInfo::AddStageObject( i3Object * pObj)
{
	if( m_spStageObjectList == nullptr)
	{
		m_spStageObjectList =	i3::make_shared< i3::vector<i3Object*> >();		// List::new_object();
		m_spStageObjectList->reserve( 32);
	}

	INT32 nObjectIndex = (INT32)m_spStageObjectList->size();

	pObj->setNetIndex( nObjectIndex);

	m_spStageObjectList->push_back( pObj);
}

UINT32 i3GameSceneInfo::SaveInstance( const char * pszFileName)
{
	i3FileStream * pFile = i3FileStream::new_object();

	if( pFile->Create( pszFileName, STREAM_WRITE | STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3TRACE("Couldn't Create file: %s\n", pszFileName);

		return STREAM_ERR;
	}

	UINT32 result = _SaveInstance( pFile);

	I3_SAFE_RELEASE( pFile);

	return result;
}

UINT32 i3GameSceneInfo::LoadInstance( const char * pszFileName)
{
	i3FileStream* pFileStream = i3FileStream::new_object();
	m_pStream = pFileStream;

	if(pFileStream->Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3TRACE("Couldn't find file: %s\n", pszFileName);
		I3_SAFE_RELEASE( m_pStream);

		return STREAM_ERR;
	}

	i3::pack::GAME_INSTANCE_PACK	pack;
	UINT32	nResult = 0;

	nResult = m_pStream->Read( &pack, sizeof( pack));
	I3_CHKIO( nResult);

	m_pStream->SetPosition( 0, STREAM_BEGIN);

	{
		if( i3::generic_is_niequal( (char*)pack.m_ID, "GINS", 4) == false)
		{
			I3_SAFE_RELEASE( m_pStream);

			I3PRINTLOG(I3LOG_NOTICE,  "this file isn't a game instance file or not a valid file");

			return STREAM_ERR;
		}

		if( pack.m_Version == 1)
		{
			nResult = _LoadInstance( m_pStream);
		}
		else if( pack.m_Version == 2)
		{
			nResult = _LoadInstance3( m_pStream);
		}
		else if( pack.m_Version == 3)
		{
			nResult = _LoadInstance4( m_pStream);
		}
		else if( pack.m_Version == 4)
		{
			nResult = _LoadInstance5( m_pStream);
		}
		else
		{
			nResult =  STREAM_ERR;
		}
	}

	I3_SAFE_RELEASE( m_pStream);

	return nResult;
}

//
// 이 함수는 실제 사용되지 않는다.
// i3LevelDesign Tool 쪽에서 직접 저장하고 있다.
UINT32 i3GameSceneInfo::_SaveInstance( i3Stream * pStream)
{
	i3::pack::GAME_INSTANCE_PACK	pack;
	i3::pack::GAME_INSTANCE_2	*	pFirstInstance;
	i3::pack::GAME_INSTANCE_2	*	pInst;

	UINT32	nResult, Rc = 0;
	INT32	i;

	pack.m_nInstanceCount	= (INT32)m_spInstList->size();

	nResult = pStream->Write( &pack, sizeof( pack));
	I3_CHKIO( Rc);

	{
		UINT32 sz = sizeof( i3::pack::GAME_INSTANCE_2) * pack.m_nInstanceCount;

		pFirstInstance = (i3::pack::GAME_INSTANCE_2 *)i3MemAlloc( sz);
		i3mem::FillZero( pFirstInstance, sz);
	}

	for( i = 0; i < (INT32)pack.m_nInstanceCount; ++i)
	{
		i3GameObj * pObj = (*m_spInstList)[i];

		pInst = pFirstInstance + i;

		// Class Meta Hash Code
		pInst->nHashCodeInstClass	=	pObj->static_meta()->class_crc(); 
		// pObj->GetClassMeta()->getHashCode(); //GetMeta()가 아니고 GetClassMeta()인 이유가 있는 듯.(2012.08.10.수빈)

		// Name
		i3::safe_string_copy( pInst->m_szName, pObj->GetNameString(), _countof( pInst->m_szName));

		// Layer
		pInst->m_Layer = pObj->getLayer();

		// Resource 설정.
		{
			i3GameResSceneGraph * pRes		= pObj->getSgRes();

			UINT64 h, l;

			h = pRes->getChecksum( 0);
			l = pRes->getChecksum( 1);

			pInst->nHashCodeResPath		= (UINT32) (l & 0xFFFFFFFF);
			pInst->HashCode[0]			= (UINT32) ((l >> 32) & 0xFFFFFFFF);
			pInst->HashCode[1]			= (UINT32) (h & 0xFFFFFFFF);
			pInst->HashCode[2]			= (UINT32) ((h >> 32) & 0xFFFFFFFF);
		}

		{
			MATRIX * pMat = pObj->GetMatrix();
			
			{
				QUATERNION Quat;

				i3Matrix::GetRotateQuat( &Quat, pMat);

				pInst->qRot.x = i3Quat::GetA( &Quat);
				pInst->qRot.y = i3Quat::GetB( &Quat);
				pInst->qRot.z = i3Quat::GetC( &Quat);
				pInst->qRot.w = i3Quat::GetD( &Quat);
			}

			{
				VEC3D	vScale;

				i3Vector::Copy( &vScale, i3Matrix::GetPos( pMat));

				pInst->vScale.x = getX( &vScale);
				pInst->vScale.y = getY( &vScale);
				pInst->vScale.z = getZ( &vScale);
			}

			{
				VEC3D	vPos;

				i3Vector::Copy( &vPos, i3Matrix::GetPos( pMat));

				pInst->vPos.x = getX( &vPos);
				pInst->vPos.y = getY( &vPos);
				pInst->vPos.z = getZ( &vPos);
			}
		}

		if( pObj->isEnable())
			pInst->bInitEnable			= true;
		else
			pInst->bInitEnable			= false;
	}

	Rc = pStream->Write( pFirstInstance, sizeof( i3::pack::GAME_INSTANCE_2) * pack.m_nInstanceCount);
	I3_CHKIO( Rc);
	nResult += Rc;

	I3MEM_SAFE_FREE_POINTER( pFirstInstance);

	return nResult;
}

void i3GameSceneInfo::_SetInstanceMatrix( i3GameObj * pObject, i3::pack::GAME_INSTANCE_2 * pInst)
{
	MATRIX Mtx;
		
	// Scale
	{
		i3Matrix::SetScale( &Mtx, pInst->vScale.x, pInst->vScale.y, pInst->vScale.z);
	}

	// Rotate
	{
		QUATERNION	Quat;
		MATRIX rot;
		i3Quat::Set( &Quat, pInst->qRot.x, pInst->qRot.y, pInst->qRot.z, pInst->qRot.w);

		i3Matrix::SetRotateQuat( &rot, &Quat);
		i3Matrix::Mul( &Mtx, &Mtx, &rot);
	}

	// Translate
	{
		VEC3D	Pos;
		i3Vector::Set( &Pos, pInst->vPos.x, pInst->vPos.y, pInst->vPos.z);

		i3Matrix::PostTranslate( &Mtx, &Pos);
	}

	pObject->SetInitialMatrix( &Mtx);
}

i3GameObj * i3GameSceneInfo::FindObjectByIndex( UINT32 nIndex)
{
	if( m_pInstInfo == nullptr)
		return nullptr;

	if( nIndex >= m_Pack.m_nInstanceCount)
	{
		I3PRINTLOG(I3LOG_WARN,  "그런거 없는디유?");
		return nullptr;
	}

	I3_GAMESCENE_INSTANCE_2 * pInst = m_pInstInfo + nIndex;

	return pInst->_pObject;
}

INT32 i3GameSceneInfo::GetObjectPersistID( i3GameObj * pObj)
{
//	return m_pInstList->IndexOf( pObj);
	size_t idx = i3::vu::index_of(*m_spInstList, pObj);
	return (idx != m_spInstList->size() ) ? INT32(idx) : -1 ;
}


UINT32 i3GameSceneInfo::_LoadInstance5( i3Stream * pFile)
{
	bool bValid;
	UINT32	nResult, Rc = 0;
	INT32	i;
	i3Stage	*	m_pCurStage = m_pFramework->GetCurrentStage();

	nResult = pFile->Read( &m_Pack, sizeof( m_Pack));
	I3_CHKIO( nResult);

	I3TRACE( "[INSTANCE MANAGER] : Total %d objects\n", m_Pack.m_nInstanceCount);

	// Layer 정보
	if( m_Pack.m_LayerCount > 0)
	{
		m_pCurStage->CreateLayers( m_Pack.m_LayerCount);

		Rc = pFile->Read( m_pCurStage->getLayer( 0), sizeof( i3::pack::GAME_LAYER) * m_Pack.m_LayerCount);
		I3_CHKIO( Rc);
		nResult += Rc;
	}

	//배열 사이즈 
	if( m_spInstList != nullptr)
	{
		i3::cu::for_each_container(*m_spInstList, i3::fu::mem_fun_safe(&i3GameObj::Release));
		m_spInstList.reset();
//		m_pInstList->SafeReleaseAll();
//		I3_SAFE_RELEASE( m_pInstList);
	}

	m_spInstList =	i3::make_shared<i3::vector<i3GameObj*> >();		//List::new_object();
	m_spInstList->reserve(m_Pack.m_nInstanceCount);
//	m_pInstList->SetAllocationUnit( m_Pack.m_nInstanceCount);

	// PreCreate Instances
	{
		I3ASSERT( m_pInstInfo == nullptr);

		m_pInstInfo	= (I3_GAMESCENE_INSTANCE_2 * )i3MemAlloc( sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);
		i3mem::FillZero( m_pInstInfo, sizeof( I3_GAMESCENE_INSTANCE_2) * m_Pack.m_nInstanceCount);

		for( i = 0; i < (INT32)m_Pack.m_nInstanceCount; ++i)
		{
			I3_GAMESCENE_INSTANCE_2 * pInstInfo = m_pInstInfo + i;

			//Offset, index
			pInstInfo->_nIndex	= i;

			// read instance info
			{
				Rc = pFile->Read( &pInstInfo->_Instance, sizeof( i3::pack::GAME_INSTANCE_2));
				I3_CHKIO( Rc);
				nResult += Rc;
			}

			//Create Instance By Meta
			{
				pInstInfo->_pMeta = i3ClassMetaSystem::i()->find_class_meta(pInstInfo->_Instance.nHashCodeInstClass);
					//i3ClassMeta::FindClassMetaByHash( pInstInfo->_Instance.nHashCodeInstClass);

				I3ASSERT( pInstInfo->_pMeta != nullptr);

				pInstInfo->_pObject	= (i3GameObj *)	pInstInfo->_pMeta->create_instance();	//CREATEINSTANCE( pInstInfo->_pMeta);

				I3ASSERT( pInstInfo->_pObject != nullptr);
			}

			// Name
			pInstInfo->_pObject->SetName( pInstInfo->_Instance.m_szName);

			// Layer
			pInstInfo->_pObject->setLayer( pInstInfo->_Instance.m_Layer);

			//find resource by hash
			{
				//UINT32 hash[4];
				UINT128 hash( pInstInfo->_Instance.HashCode[2], pInstInfo->_Instance.HashCode[1], pInstInfo->_Instance.HashCode[0], pInstInfo->_Instance.nHashCodeResPath);

				pInstInfo->_pRes = m_pResManager->FindResourceByHash( & hash);
			}

			//
			pInstInfo->_nOffsetAdditionalData	= pFile->GetPosition();
			if( pInstInfo->_Instance.nGameDataSize + pInstInfo->_Instance.nNameSize > 0)
			{
				pFile->SetPosition( pInstInfo->_Instance.nGameDataSize + pInstInfo->_Instance.nNameSize, STREAM_CURRENT);
			}
		}
	}

	for( i = 0; i < (INT32) m_Pack.m_nInstanceCount; ++i)
	{
		I3_GAMESCENE_INSTANCE_2	* pInstInfo = m_pInstInfo + i;

		i3ClassMeta				* pMeta		= pInstInfo->_pMeta;
		i3GameRes				* pRes		= pInstInfo->_pRes;
		i3GameObj				* pObject	= pInstInfo->_pObject;
		i3::pack::GAME_INSTANCE_2 * pInst	= &pInstInfo->_Instance;

		bValid = true;

		// load additional data
		if( pInst->nGameDataSize > 0)
		{
			pFile->SetPosition( pInstInfo->_nOffsetAdditionalData, STREAM_BEGIN);

			Rc = pObject->OnLoadGameData( this, nullptr, pInst->nGameDataSize);
			I3_CHKIO( Rc);
			nResult += Rc;

			//게임 데이터로 지정된 영역의 크기와 읽어들인 크기가 동일해야한다.
			//그렇다는 보장은 없지만 초기 버그찾기 쉽게하기위해 일단 넣어둡니다.
			I3ASSERT( Rc == pInst->nGameDataSize);
		}

		//Respawn
		if( i3::same_of<i3RespawnObj* >(pMeta))
		{
			i3RespawnObj * pObj = (i3RespawnObj *)pObject;

			pObj->setRespawnStyle( pInst->nParam1);

			m_pCurStage->addRespawnObject( pObj);
		}
		else if( i3::same_of<i3GamePath* >(pMeta))
		{
			// GamePath
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
			// LightObject
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
		else if( i3::kind_of< i3Object*>(pMeta)
			&& i3::kind_of<i3GameResObject*>(pRes))	// i3Object는 게임에 파생됩니다.
		{
			// i3Object
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
		else if( i3::kind_of<i3Trigger*>(pMeta))	// i3Tirgger는 게임에 파생됩니다.
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

				switch(pInst->nTriggerType)
				{
				case 0:
					pTrigger->CreateBoxTrigger( &Zero, &Scale, pInst->nTriggerMatchGroup, false, &Mtx);
					break;
				case 1:
					pTrigger->CreateSphereTrigger( &Pos, pInst->vScale.x * 0.5f, pInst->nTriggerMatchGroup);
					break;
				case 2:
					pTrigger->CreateCapsuleTrigger( &Pos, pInst->vScale.x, pInst->vScale.y * 2.0f, pInst->nTriggerMatchGroup);
					break;
				default:
					I3ASSERT("Wrong nTriggerType number!");
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

			if( i3::kind_of<i3World*>(pMeta))	// i3World는 게임에 파생됩니다.
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

bool	i3GameSceneInfo::SaveResource( const char * pszFileName)
{
	i3FileStream			File;
	if( File.Create( pszFileName, STREAM_WRITE) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Couldn't Create File : %s\n", pszFileName);
		return false;
	}

	UINT32 nResult = _SaveRsc( &File);
	if (nResult == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL, "couldn't save file : %s\n", pszFileName);
		return false;
	}

	return true;
}

bool i3GameSceneInfo::LoadResource(const char * pszFileName)
{
	I3ASSERT( m_pResManager != nullptr);

	return m_pResManager->LoadRSC( pszFileName);
}

UINT32 i3GameSceneInfo::_SaveRsc( i3Stream * pStream)
{
	if( m_spResourceList == nullptr)
	{
		I3TRACE("저장할 Resource의 List가 nullptr입니다.\n");
		return STREAM_ERR;
	}

	INT32 i;
	INT32					nCount		= (INT32)m_spResourceList->size();
	UINT32					Result, Rc	= 0;
	//i3::pack::RSC_HEADER	*pHeader	= nullptr;

	{
		char	ID[4];
		i3mem::Copy( ID, "RSCF", sizeof( UINT8) * 4);

		Result = pStream->Write( ID, 4 * sizeof( UINT8));
		I3_CHKIO( Result);
	}

	{
		Rc = pStream->Write( &nCount, sizeof( INT32));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	for( i = 0; i < nCount; i++)
	{
		i3GameRes * pRes = (*m_spResourceList)[i];
		I3TRACE("save resource : %s\n", pRes->GetName());

		//Rc = __SaveElement( pStream, pRes);
		Rc = pRes->SaveToFile( pStream);

		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

