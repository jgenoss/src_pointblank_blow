#include "i3FrameworkPCH.h"
#include "i3Stage.h"
#include "i3StageInfo.h"
#include "i3Framework.h"
#include "i3GameSceneInfo.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3Stage);

i3Stage::i3Stage(void)
{
	m_RespawnList.reserve( 32);
}

i3Stage::~i3Stage(void)
{
	if (m_pFramework && m_pFramework->getSgContext())
	{
		m_pFramework->getSgContext()->SetOccCuller(i3::occ_culler_ptr());
	}
	
	I3_SAFE_RELEASE( m_pWorld);

	i3::cu::for_each_container(m_RespawnList, i3::fu::mem_fun_safe(&i3RespawnObj::Release));

	I3_SAFE_RELEASE( m_pGameSceneInfo);

	// m_InstManagerList.SafeReleaseAll();
	i3::cu::for_each_safe_release_clear(m_InstManagerList, &i3InstanceManager::Release);

	I3MEM_SAFE_FREE( m_pLayer);
	m_LayerCount = 0;
}

void i3Stage::ReleasePhysXScene( void)
{
	g_pPhysixCtx->ReleaseScene();
}

InstanceListPtr i3Stage::GetInstanceList( void)
{
	if( m_pGameSceneInfo == nullptr)
	{
		return InstanceListPtr();
	}

	return m_pGameSceneInfo->getInstanceList();
}

void i3Stage::CreateLayers( INT32 cnt)
{
	m_LayerCount = cnt;

	I3MEM_SAFE_FREE( m_pLayer);

	if( m_LayerCount > 0)
	{
		m_pLayer = (i3::pack::GAME_LAYER *) i3MemAlloc( sizeof(i3::pack::GAME_LAYER) * cnt);
		i3mem::FillZero( m_pLayer, sizeof( i3::pack::GAME_LAYER) * cnt);
	}
}

i3::pack::GAME_LAYER * i3Stage::getLayer( INT32 idx)
{
	I3_BOUNDCHK( idx, m_LayerCount);

	return m_pLayer + idx;
}

StageObjectListPtr	i3Stage::GetStageObjectList( void)
{
	if( m_pGameSceneInfo == nullptr)
		return StageObjectListPtr();

	return m_pGameSceneInfo->getStageObjectList();
}

bool i3Stage::OnQueryLoad( INT32)
{
	i3StageInfo		*	pInfo	= m_pFramework->getStageInfo( m_pFramework->GetCurrentStageIndex());
	
	// 등록된 RSC 파일들을 읽어들인다.
	const i3::vector<i3::rc_string>& RSCList = pInfo->getRSCPath();

	for(size_t i = 0; i < RSCList.size(); i++)
	{
		m_pFramework->LoadRSC( RSCList[i].c_str() );
	}

	CreateGameScene( pInfo);
	
	return true;
}

void i3Stage::addRespawnObject( i3RespawnObj * pRespawn)
{
	if( pRespawn == nullptr)
		return;

	I3_SAFE_ADDREF( pRespawn);

	m_RespawnList.push_back( pRespawn);
}

void i3Stage::OnLoading(void)
{
	
}

void i3Stage::OnLoadingGameData(void)
{
}

void i3Stage::OnLoadEnd(void)
{
}

i3Node * i3Stage::OnQueryAttachNode( i3GameRes *)
{
	return nullptr;
}

void i3Stage::OnChangeVideo( i3VideoInfo * pInfo)
{
}

void i3Stage::OnInput(void)
{
}

/*
void i3Stage::CreateWorld( i3StageInfo * pInfo)
{
	i3ResourceManager * pMng = m_pFramework->GetResourceManager();

	if( pInfo == nullptr)	return;

	char	szWorldFileName[MAX_PATH];

	sprintf( szWorldFileName, "World/%s/%s.i3wrd", pInfo->GetName(), pInfo->GetName());

	i3GameResWorld * pWorldRes = (i3GameResWorld*)pMng->FindResourceByPath( szWorldFileName);

	if( (m_pFramework != nullptr) && (pWorldRes != nullptr))
	{
		I3_SAFE_RELEASE( m_pWorld);

		m_pWorld = i3World::new_object();
		
		m_pWorld->Create( m_pFramework, pWorldRes);

		AddChild( m_pWorld);
	}
}
*/

void i3Stage::CreateGameScene( i3StageInfo * pInfo)
{
	//i3ResourceManager * pMng = m_pFramework->GetResourceManager();

	if( pInfo == nullptr)	return;

	const char * szStageName = pInfo->GetName();

	I3ASSERT( m_pGameSceneInfo == nullptr);
	m_pGameSceneInfo = i3GameSceneInfo::new_object();
	I3ASSERT( m_pGameSceneInfo != nullptr);

	if( (m_pGameSceneInfo->Create( m_pFramework)) && (m_pGameSceneInfo->Load( szStageName)))
	{
	}
	else
	{
		I3_SAFE_RELEASE( m_pGameSceneInfo);
	}

	// Effect의 Loading
	/*
	{
		char szPath[ MAX_PATH];

		sprintf( szPath, "Scene/%s/%s.i3FxList", szStageName, szStageName);

		if( i3System::IsFile( szPath))
		{
			m_pFramework->getEffectManager()->Load( szPath, false);
		}
	}
	*/
}

void i3Stage::OnCreate(void)
{
	I3ASSERT( m_pFramework != nullptr);

	i3StageInfo * pInfo = m_pFramework->getStageInfo( m_pFramework->GetCurrentStageIndex());
	SetName( pInfo->GetNameString());

	if( pInfo != nullptr)
	{
		StageInstanceInfoList*	pList = pInfo->getInstanceManagerInfoListPtr().get();
		if( pList != nullptr)
		{
			for( INT32 i = 0; i < pInfo->getInstanceManagerInfoCount(); ++i)
			{
				i3StageInstanceInfo * pInsMng = (*pList)[i];
				I3ASSERT( pInsMng != nullptr);

				i3InstanceManager * pMng = i3InstanceManager::new_object();
				I3ASSERT( pMng != nullptr);

				if( pMng->Create( pInsMng->GetClassMetaByName(), pInsMng->getInstanceCount()))
				{
					m_InstManagerList.insert( i3::make_pair(pMng->getInstanceMeta(), pMng) );
				}
				else
				{
					I3_SAFE_RELEASE( pMng);
				}
			}
		}
	}

	if( m_pGameSceneInfo != nullptr)
	{
		m_pGameSceneInfo->InitInstance();
	}
	else
	{
		//CreateWorld( pInfo);
	}
}

i3InstanceManager * i3Stage::GetInstanceManager( i3ClassMeta * pMeta)
{
//	INT32 idx;
	// "찾기"용의 Compare Procedure로 임시 변경
//	m_InstManagerList.SetCompareProc( (COMPAREPROC) _ManagerFindProc);

//	idx = m_InstManagerList.FindItem( pMeta);
//	I3ASSERT( idx != -1);

	// "Add"용의 Compare Procedure로 복구
//	m_InstManagerList.SetCompareProc( (COMPAREPROC) _ManagerCompProc);

//	return (i3InstanceManager *) m_InstManagerList.GetItem( idx);

	i3::unordered_map<i3ClassMeta*, i3InstanceManager*>::iterator it = m_InstManagerList.find(pMeta);
	return (it != m_InstManagerList.end()) ? it->second : nullptr;
}

struct LF_INFO
{
	i3Object *	_pObj = nullptr;
	VEC3D *		_pImpulse = nullptr;
	VEC3D *		_pIntersect = nullptr;
	REAL32		_rTorque = 0.0f;
#if defined(I3_PHYSX)
	i3PhysixShapeSet * _pHitShape = nullptr;
#endif
} ;

static I3SG_TRAVERSAL_RESULT _SetLinearForceProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
#if defined(I3_PHYSX)
	i3PhysixShapeSet * pShape;
	MATRIX	mat;
	VEC3D	vR, vTorque;
	LF_INFO * pInfo = (LF_INFO *) pData;

	if( !i3::same_of<i3Transform2*>(pNode))
		return I3SG_TRAVERSAL_CONTINUE;

	i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

	pShape = pTrans2->getShapeSet();
	I3ASSERT( pShape != nullptr );


	if( pInfo->_pHitShape != nullptr && pShape != pInfo->_pHitShape )
		return I3SG_TRAVERSAL_CONTINUE;

	pShape->getShapeGlobalPose( &mat );
	i3Vector::Sub( &vR, pInfo->_pIntersect, i3Matrix::GetPos( &mat ) );
	i3Vector::Cross( &vTorque, &vR, pInfo->_pImpulse );
	pShape->setLinearVelocity( pInfo->_pImpulse );

	i3Vector::Scale( &vTorque, &vTorque, pInfo->_rTorque );
	pShape->setAngularVelocity( &vTorque );
#endif

	return I3SG_TRAVERSAL_CONTINUE;
}

#if defined( I3_PHYSX)
bool i3Stage::PHYSIX_SetLinearForce( i3Object * pObj, VEC3D * pImpulse, VEC3D * pIntersect, REAL32 rTorque, i3PhysixShapeSet * pHitShape )
{	
	LF_INFO info;

	info._pHitShape = pHitShape;
	info._pImpulse = pImpulse;
	info._pIntersect = pIntersect;
	info._pObj = pObj;
	info._rTorque = rTorque;

	i3Scene::FindNodeByExactType( pObj->getSceneObject(), i3Transform2::static_meta(), _SetLinearForceProc, &info, true);

	return false;
}

bool i3Stage::PHYSIX_SetExplosion( i3Object * pObj, REAL32 rRange, REAL32 rPower, VEC3D * pCenterPos )
{
	pObj->PHYSIX_SetExplosionForce( pCenterPos, rRange, rPower );

	return true;
}
#endif

#if defined( I3_WINDOWS)
void i3Stage::OnLButtonDown( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnLButtonUp( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnMButtonDown( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnMButtonUp( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnRButtonUp( UINT32 nFlag, POINT point)
{
}

void i3Stage::OnMouseMove( UINT32 nFlags, POINT point)
{
}

void i3Stage::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
}

void i3Stage::OnKeyDown( UINT32 nKey)
{
}

void i3Stage::OnKeyUp( UINT32 nKey)
{
}
#endif

void i3Stage::RegisterLuaFunction( lua_State * pState)
{
}