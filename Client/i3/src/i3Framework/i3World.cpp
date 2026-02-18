#include "i3FrameworkPCH.h"
#include "i3World.h"
#include "i3GameResWorld.h"

#include "i3Math/structure/aabb.h"
#include "i3Math/octree/occ_culler.h"
#include "i3Math/octree/octree.h"
#include "i3Math/octree/occ_quad_set.h"

#include "i3Gfx/i3GfxOccCuller.h"

#include "i3Framework.h"
#include "OccDebugInfo.h"

#include "i3EffectManager.h"

I3_CLASS_INSTANCE( i3World);

namespace
{
	class occ_hw_callback_derived : public i3::occ_hw_query_callback
	{
	public:
		virtual ~occ_hw_callback_derived()
		{
			I3_SAFE_RELEASE(m_gfxCuller);
		}

		occ_hw_callback_derived()
		{
			m_gfxCuller = i3GfxOccCuller::new_object();
			m_gfxCuller->Create();
		}
	private:
		virtual int on_additional_process(const i3::vector<i3::occ_quad*>& occluder_list,
			const i3::vector<i3::octree_item*>& occludee_list, const i3::mat4& mat_view_proj)
		{
			return m_gfxCuller->UpdateOcclusions(occluder_list, occludee_list, mat_view_proj);
		}

		i3GfxOccCuller*	m_gfxCuller;
	};

}


i3World::i3World()
{
	INT32 i;
			
	for( i = 0; i < I3FRM_FOG_COUNT; i++)
	{
		m_pFogEnableAttr[ i]	= nullptr;
		m_pFogColorAttr[ i]	= nullptr;
		m_pFogDensityAttr[ i]	= nullptr;
		m_pFogModeAttr[ i]		= nullptr;	
		m_pFogNearFarAttr[ i]	= nullptr;
	}

	InitRoot();

	CreateFogAttr( nullptr, I3FRM_FOG_WORLD);
	CreateFogAttr( m_pSkyRoot, I3FRM_FOG_SKY);
	

	i3Framework* pCurrFramework = i3Framework::getCurrentFramework();
	if (pCurrFramework)
	{
		i3EffectManager* pEffectManager = pCurrFramework->getEffectManager();
		I3ASSERT(pEffectManager);

		pEffectManager->RemoveWorldRenderAttrsFromEffect();
		pEffectManager->AddWorldRenderAttrToEffect(m_pFogEnableAttr[0]);
		pEffectManager->AddWorldRenderAttrToEffect(m_pFogColorAttr[0]);
		pEffectManager->AddWorldRenderAttrToEffect(m_pFogDensityAttr[0]);
		pEffectManager->AddWorldRenderAttrToEffect(m_pFogModeAttr[0]);
		pEffectManager->AddWorldRenderAttrToEffect(m_pFogNearFarAttr[0]);
	}
	
}


i3World::~i3World()
{
//	I3_SAFE_RELEASE(m_occ_query);

	I3_SAFE_RELEASE(m_pRenderOccQuad);

	I3_SAFE_RELEASE( m_pWorldCollisionRoot);	// Chara, Npc등의 이동 관련 Collision
	I3_SAFE_RELEASE( m_pHitCollisionRoot);		// Attack판정에 쓰일 Hit Collision
	I3_SAFE_RELEASE( m_pLightVolumeCollisionRoot);
	I3_SAFE_RELEASE( m_pWorldScene);
	I3_SAFE_RELEASE( m_pSkyScene);

	I3_SAFE_RELEASE( m_pWorldSectionTable);
	I3_SAFE_RELEASE( m_pMapSectorTable);

	if (m_pFogEnableAttr[0])
	{
		i3Framework* pCurrFramework = i3Framework::getCurrentFramework();
		if (pCurrFramework)
		{
			i3EffectManager* pEffectManager = pCurrFramework->getEffectManager();
			I3ASSERT(pEffectManager);
			pEffectManager->RemoveWorldRenderAttrsFromEffect();
		}
	}
	
	for( INT32 i = 0; i < 2; ++i)
	{
		I3_SAFE_RELEASE( m_pFogEnableAttr[ i]);
		I3_SAFE_RELEASE( m_pFogColorAttr[ i]);
		I3_SAFE_RELEASE( m_pFogDensityAttr[ i]);
		I3_SAFE_RELEASE( m_pFogModeAttr[ i]);
		I3_SAFE_RELEASE( m_pFogNearFarAttr[ i]);
	}
}

void i3World::InitRoot()
{
	getAttachNode()->RemoveAllChild();

	m_pSkyRoot		= i3AttrSet::new_object_ref();
	m_pSkyRoot->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
	getAttachNode()->AddChild( m_pSkyRoot);

	#if defined( I3_DEBUG)
	m_pSkyRoot->SetName( "## SkyBranch ##");
	#endif
	
	m_pWorldRoot	= i3AttrSet::new_object_ref();
	getAttachNode()->AddChild( m_pWorldRoot);

	#if defined( I3_DEBUG)
	m_pWorldRoot->SetName( "## WorldBranch ##");
	#endif

	// Disable Z Write
	{
		i3ZWriteEnableAttr *pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pSkyRoot->AddAttr( pAttr );
	}

	// Disable Z Write
	{
		i3LightingEnableAttr *pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pSkyRoot->AddAttr( pAttr );
	}

	m_pSkyTrans = i3Transform::new_object_ref();
	m_pSkyRoot->AddChild( m_pSkyTrans);
}

void i3World::BindSgRes( i3Framework * pFramework, i3GameResSceneGraph * pRes, bool bInstancing, bool bClone)
{
	I3ASSERT( i3::same_of<i3GameResWorld* >(pRes));

	i3GameResWorld * pWorldRes = (i3GameResWorld *) pRes;
	i3World * pSrc = pWorldRes->getWorld();

	pSrc->CopyTo( this, I3_COPY_REF);

	pFramework->getSgContext()->SetOccCuller(m_occ_culler);

	// World는 여러 개의 Instance가 만들어질 필요가 없기때문에...
	// 원본 Resource를 그냥 사용한다.
	i3GameObj::BindSgRes( pFramework, pRes, false, false);
}

void i3World::SetVisible( bool bFlag, REAL32 tm)
{
	i3GameObj::SetVisible( bFlag, tm);

	UINT32 addFlag, removeFlag;

	if( bFlag)
	{
		addFlag = 0;

#if defined( I3_PHYSX)
		removeFlag = I3_PHYSIX_STATE_DISABLE;
#endif
	}
	else
	{
#if defined( I3_PHYSX)
		addFlag = I3_PHYSIX_STATE_DISABLE;
#endif
		removeFlag = 0;
	}

	if( getWorldCollision() != nullptr)
	{
		i3Scene::ModifyDynamicState( getWorldCollision()->getInstanceSg(), addFlag, removeFlag);
	}

	if( getHitCollision() != nullptr)
	{
		i3Scene::ModifyDynamicState( getHitCollision()->getInstanceSg(), addFlag, removeFlag);
	}

	if( getLightVolumeCollision() != nullptr)
	{
		i3Scene::ModifyDynamicState( getLightVolumeCollision()->getInstanceSg(), addFlag, removeFlag);
	}
}

void i3World::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameObj::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3World* >(pDest));

	i3World * pWorld = (i3World *)pDest;

	if( method == I3_COPY_REF)
	{
		pWorld->SetWorld( getWorld());
		pWorld->SetSky( getSky());

		pWorld->SetWorldCollision( getWorldCollision());
		pWorld->SetHitCollision( getHitCollision());
		pWorld->SetLightVolumeCollision( getLightVolumeCollision());
	}
	else
	{
		if( getWorld() != nullptr)
		{
			i3SceneGraphInfo * pNewWorldSg = i3SceneGraphInfo::new_object_ref();
			getWorld()->CopyTo( pNewWorldSg, I3_COPY_INSTANCE);
			pWorld->SetWorld( pNewWorldSg);
		}

		if( getSky() != nullptr)
		{
			i3SceneGraphInfo * pNewSkySg = i3SceneGraphInfo::new_object_ref();
			getSky()->CopyTo( pNewSkySg, I3_COPY_INSTANCE);
			pWorld->SetSky( pNewSkySg);
		}

		if( getWorldCollision() != nullptr)
		{
			i3SceneGraphInfo * pNewWorldColSg = i3SceneGraphInfo::new_object_ref();
			getWorldCollision()->CopyTo( pNewWorldColSg, I3_COPY_INSTANCE);
			pWorld->SetWorldCollision( pNewWorldColSg);
		}

		if( getHitCollision() != nullptr)
		{
			i3SceneGraphInfo * pNewHitColSg = i3SceneGraphInfo::new_object_ref();
			getHitCollision()->CopyTo( pNewHitColSg, I3_COPY_INSTANCE);
			pWorld->SetHitCollision( pNewHitColSg);
		}

		if( getLightVolumeCollision() != nullptr)
		{
			i3SceneGraphInfo * pNewLightVolumeColSg = i3SceneGraphInfo::new_object_ref();
			getLightVolumeCollision()->CopyTo( pNewLightVolumeColSg, I3_COPY_INSTANCE);
			pWorld->SetLightVolumeCollision( pNewLightVolumeColSg);
		}
	}

	pWorld->m_octree = this->m_octree;
	pWorld->m_occ_culler = this->m_occ_culler;
	pWorld->m_query_callback = this->m_query_callback;

	pWorld->m_occ_quad_list = this->m_occ_quad_list;
	I3_REF_CHANGE(pWorld->m_pRenderOccQuad, this->m_pRenderOccQuad);


	// World-Section Table은 언제나 참조로만...
	pWorld->setWorldSectionTable( getWorldSectionTable());
	pWorld->setMapSectorTable( getMapSectorTable());

	for( INT32 i = 0; i < 2; i++)
	{
		pWorld->SetFogEnable(	GetFogEnable( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
		pWorld->SetFogMode(		GetFogMode( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
		pWorld->SetFogColor(	GetFogColor( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
		pWorld->SetFogDensity(	GetFogDensity( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
		pWorld->SetFogNear(		GetFogNear( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
		pWorld->SetFogFar(		GetFogFar( (I3FRM_FOG_INDEX) i), ((I3FRM_FOG_INDEX)i));
	}
}

void i3World::SetWorld( i3SceneGraphInfo * pWorld)
{
	I3ASSERT( m_pWorldRoot != nullptr);

	if( m_pWorldScene != nullptr)
	{
		m_pWorldRoot->RemoveChild( m_pWorldScene->getInstanceSg());
	}

	I3_REF_CHANGE( m_pWorldScene, pWorld);

	if( m_pWorldScene != nullptr)
	{
		m_pWorldRoot->AddChild( m_pWorldScene->getInstanceSg());
	}
}

void i3World::SetSky( i3SceneGraphInfo * pSky)
{
	I3ASSERT( m_pSkyTrans != nullptr);

	if( m_pSkyScene != nullptr)
		m_pSkyTrans->RemoveChild( m_pSkyScene->getInstanceSg());

	I3_REF_CHANGE( m_pSkyScene, pSky);

	if( m_pSkyScene != nullptr)
	{
		m_pSkyTrans->AddChild( m_pSkyScene->getInstanceSg());
	}
}

void i3World::SetWorldCollision( i3SceneGraphInfo * pColNode)
{
	i3Node * pSg;

	I3_REF_CHANGE( m_pWorldCollisionRoot, pColNode);

	//  여러 가지 편리함을 위해, Sg에 추가한다.
	// 그러나, 실질적으로 Traversal에 포함될 필요는 없기 때문에
	// Disable 시켜 둔다.
	if( m_pWorldCollisionRoot != nullptr)
	{
		pSg = m_pWorldCollisionRoot->getInstanceSg();
		getAttachNode()->AddChild( pSg);

		i3Scene::ModifyDynamicState( pSg, 0, I3_PHYSIX_STATE_KINEMATIC);

		pSg->DeactivateNode();
	}
}

void i3World::SetHitCollision( i3SceneGraphInfo * pNode)
{
	i3Node * pSg;

	I3_REF_CHANGE( m_pHitCollisionRoot, pNode);

	//  여러 가지 편리함을 위해, Sg에 추가한다.
	// 그러나, 실질적으로 Traversal에 포함될 필요는 없기 때문에
	// Disable 시켜 둔다.
	if( m_pHitCollisionRoot != nullptr)
	{
		pSg = m_pHitCollisionRoot->getInstanceSg();
		I3ASSERT( pSg != nullptr);

		I3ASSERT( getAttachNode() != nullptr);
		getAttachNode()->AddChild( pSg);

		pSg->DeactivateNode();
	}
}

void i3World::SetLightVolumeCollision( i3SceneGraphInfo * pNode)
{
	I3_REF_CHANGE( m_pLightVolumeCollisionRoot, pNode);
}

bool	i3World::GetFogEnable(I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogEnableAttr[nIdx] != nullptr)
	{
		return m_pFogEnableAttr[nIdx]->Get();
	}
	else
		return false;
}

I3COLOR*	i3World::GetFogColor(I3FRM_FOG_INDEX nIdx)		
{
	if( m_pFogColorAttr[nIdx] != nullptr)
	{
		return m_pFogColorAttr[nIdx]->Get();
	}
	else	return nullptr;
}

REAL32	i3World::GetFogDensity(I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogDensityAttr[nIdx] != nullptr)
	{
		return m_pFogDensityAttr[nIdx]->Get();
	}
	else
		return 0.0f;
}

I3G_FOG_MODE	i3World::GetFogMode(I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogModeAttr[nIdx] != nullptr)
	{
		return m_pFogModeAttr[nIdx]->Get();
	}
	else
		return I3G_FOG_NONE;
}

REAL32	i3World::GetFogNear(I3FRM_FOG_INDEX nIdx)		
{
	if( m_pFogNearFarAttr[nIdx] != nullptr)
	{
		return m_pFogNearFarAttr[nIdx]->GetNear();
	}
	else
		return 0.0f;
}

REAL32	i3World::GetFogFar(I3FRM_FOG_INDEX nIdx)			
{
	if( m_pFogNearFarAttr[nIdx] != nullptr)
	{
		return m_pFogNearFarAttr[nIdx]->GetFar();
	}
	else
		return 0.0f;
}

void	i3World::SetFogEnable( bool bEnable, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogEnableAttr[nIdx] != nullptr)
	{
		m_pFogEnableAttr[nIdx]->Set( bEnable);
	}
}

void	i3World::SetFogColor( I3COLOR * pColor, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogColorAttr[nIdx] != nullptr)
	{
		m_pFogColorAttr[nIdx]->Set( pColor);
	}
}

void	i3World::SetFogDensity( REAL32 rDensity, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogDensityAttr[nIdx] != nullptr)
	{
		m_pFogDensityAttr[nIdx]->Set( rDensity);
	}
}

void	i3World::SetFogMode( I3G_FOG_MODE FogMode, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogModeAttr[nIdx] != nullptr)
	{
		m_pFogModeAttr[nIdx]->Set( FogMode);
	}
}

void	i3World::SetFogNear( REAL32 rNear, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogNearFarAttr[nIdx] != nullptr)
	{
		m_pFogNearFarAttr[nIdx]->SetNear( rNear);
	}
}

void	i3World::SetFogFar( REAL32 rFar, I3FRM_FOG_INDEX nIdx)
{
	if( m_pFogNearFarAttr[nIdx] != nullptr)
	{
		m_pFogNearFarAttr[nIdx]->SetFar( rFar);
	}
}

void	i3World::CreateFogAttr( i3AttrSet * pParentAttr, I3FRM_FOG_INDEX nIndex)
{
	//Fog Enable
	{
		m_pFogEnableAttr[nIndex]	= i3FogEnableAttr::new_object();
		m_pFogEnableAttr[nIndex]->Set(true);

		if( pParentAttr != nullptr)
		{
			pParentAttr->AddAttr( m_pFogEnableAttr[nIndex]);
		}
	}

	//Fog Mode
	{
		m_pFogModeAttr[nIndex] = i3FogModeAttr::new_object();
		m_pFogModeAttr[nIndex]->Set( I3G_FOG_LINEAR);
		if( pParentAttr != nullptr)
		{
			pParentAttr->AddAttr( m_pFogModeAttr[nIndex]);
		}
	}

	//Fog Color
	{
		COLOR Col;
		i3Color::Set( &Col, (UINT8) 255,255,255,255);
		m_pFogColorAttr[nIndex] = i3FogColorAttr::new_object();
		m_pFogColorAttr[nIndex]->Set( &Col);

		if( pParentAttr != nullptr)
		{
			pParentAttr->AddAttr( m_pFogColorAttr[nIndex]);
		}
	}

	//FogDensity
	{
		m_pFogDensityAttr[nIndex]= i3FogDensityAttr::new_object();
		m_pFogDensityAttr[nIndex]->Set( 1.0f);

		if( pParentAttr != nullptr)
		{
			pParentAttr->AddAttr( m_pFogDensityAttr[nIndex]);
		}
	}

	//FogNear, Far
	{
		m_pFogNearFarAttr[nIndex] = i3FogNearFarAttr::new_object();
		m_pFogNearFarAttr[nIndex]->SetNear( 1.0f);
		m_pFogNearFarAttr[nIndex]->SetFar( 1000.0f);

		if( pParentAttr != nullptr)
		{
			pParentAttr->AddAttr( m_pFogNearFarAttr[nIndex]);
		}
	}
}

void	i3World::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pWorldScene != nullptr)
	{
		m_pWorldScene->SetExternRes();
		m_pWorldScene->OnBuildObjectList( List);
	}

	if( m_pSkyScene != nullptr)
	{
		m_pSkyScene->SetExternRes();
		m_pSkyScene->OnBuildObjectList( List);
	}

	if( m_pWorldCollisionRoot != nullptr)
	{
		m_pWorldCollisionRoot->SetExternRes();
		m_pWorldCollisionRoot->OnBuildObjectList(  List);
	}

	if( m_pHitCollisionRoot != nullptr)
	{
		m_pHitCollisionRoot->SetExternRes();
		m_pHitCollisionRoot->OnBuildObjectList( List);
	}

	if( m_pLightVolumeCollisionRoot != nullptr)
	{
		m_pLightVolumeCollisionRoot->SetExternRes();
		m_pLightVolumeCollisionRoot->OnBuildObjectList( List);
	}

	if( getWorldSectionTable() != nullptr)
	{
		getWorldSectionTable()->OnBuildObjectList( List);
	}

	if( getMapSectorTable() != nullptr)
	{
		getMapSectorTable()->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED WORLD1
	{
		UINT8		m_ID[4] = { 'W', 'R', 'D', '1' };
		OBJREF64	m_pWorldScene = 0;
		OBJREF64	m_pSkyScene = 0;

		//Light
		OBJREF64	m_pLightAttr = 0;

		//Fog
		OBJREF64	m_pFogEnableAttr[I3FRM_FOG_COUNT] = { 0 };
		OBJREF64	m_pFogColorAttr[I3FRM_FOG_COUNT] = { 0 };
		OBJREF64	m_pFogDensityAttr[I3FRM_FOG_COUNT] = { 0 };
		OBJREF64	m_pFogModeAttr[I3FRM_FOG_COUNT] = { 0 };
		OBJREF64	m_pFogNearFarAttr[I3FRM_FOG_COUNT] = { 0 };

		OBJREF64	m_pWorldCollisionRoot = 0;
		OBJREF64	m_pHeroCollisionRoot = 0;
		OBJREF64	m_pHitCollisionRoot = 0;

		REAL32			m_LightDist = 0.0f;
		i3::pack::VEC3D	m_vLightRot;
		i3::pack::VEC3D	m_vLightTargetPos;

		OBJREF64	m_pLightVolumeCollisionRoot = 0;

		UINT32		pad[3] = { 0 };
	};


	struct PACKED WORLD_FOGINFO
	{
		UINT8		m_bEnable = 0;
		UINT8		m_Mode = 0;
		UINT16		align_pad0 = 0;
		UINT32		m_Color = 0;
		REAL32		m_Density = 0.0f;
		REAL32		m_Near = 0.0f;
		REAL32		m_Far = 0.0f;
		UINT32		pad[2] = { 0 };
	};

	struct PACKED WORLD2
	{
		UINT8		m_ID[4] = { 'W', 'R', 'D', '2' };

		OBJREF64	m_pWorldScene = 0;
		OBJREF64	m_pSkyScene = 0;
		OBJREF64	m_pWorldCollisionRoot = 0;
		OBJREF64	m_pHitCollisionRoot = 0;
		OBJREF64	m_pLightVolumeCollisionRoot = 0;

		//Fog
		WORLD_FOGINFO	m_Fog[2];

		OBJREF64	m_pWorldSectionTable = 0;
		OBJREF64	m_pMapSectorTable = 0;

		UINT32		pad[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3World::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Result, Rc = 0;
	i3Stream	*	pStream = pResFile->GetStream();
	pack::WORLD2	data;
	INT32 i;
	
	Result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( Result);

	
	data.m_pWorldScene					= (OBJREF64) pResFile->GetObjectPersistID( m_pWorldScene);
	data.m_pSkyScene					= (OBJREF64) pResFile->GetObjectPersistID( m_pSkyScene);
	data.m_pWorldCollisionRoot			= (OBJREF64) pResFile->GetObjectPersistID( m_pWorldCollisionRoot);
	data.m_pHitCollisionRoot			= (OBJREF64) pResFile->GetObjectPersistID( m_pHitCollisionRoot);
	data.m_pLightVolumeCollisionRoot	= (OBJREF64) pResFile->GetObjectPersistID( m_pLightVolumeCollisionRoot);
	data.m_pWorldSectionTable			= (OBJREF64) pResFile->GetObjectPersistID( getWorldSectionTable());
	data.m_pMapSectorTable				= (OBJREF64) pResFile->GetObjectPersistID( getMapSectorTable());

	for( i = 0; i < 2; i++)
	{
		data.m_Fog[i].m_bEnable		= (UINT8) GetFogEnable( (I3FRM_FOG_INDEX) i);
		data.m_Fog[i].m_Mode		= (UINT8) GetFogMode( (I3FRM_FOG_INDEX) i);
		data.m_Fog[i].m_Color		= i3Color::ConvertRGBA32( GetFogColor( (I3FRM_FOG_INDEX) i));
		data.m_Fog[i].m_Density		= GetFogDensity( (I3FRM_FOG_INDEX) i);
		data.m_Fog[i].m_Near		= GetFogNear( (I3FRM_FOG_INDEX) i);
		data.m_Fog[i].m_Far			= GetFogFar( (I3FRM_FOG_INDEX) i);
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3World::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Result, Rc = 0;
	char id[4];

	Result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( id, sizeof(id));
	I3_CHKIO( Rc);
	Result += Rc;

	pResFile->GetStream()->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "WRD1", 4) == 0)
	{
		Rc = OnLoad1( pResFile);
	}
	else
	{
		Rc = OnLoad2( pResFile);
	}

	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32  i3World::OnLoad2( i3ResourceFile * pResFile)
{
	UINT32	Result = 0;
	i3Stream	*	pStream = pResFile->GetStream();
	pack::WORLD2	data;
	INT32 i;

	Result = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Result);

	if( data.m_pWorldScene != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pWorldScene);
		I3ASSERT( pInfo != nullptr);

		SetWorld( pInfo);
	}

	if( data.m_pSkyScene != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pSkyScene);
		I3ASSERT( pInfo != nullptr);

		SetSky( pInfo);
	}
	
	if( data.m_pWorldCollisionRoot	!=  0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pWorldCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetWorldCollision( pInfo);
	}

	if( data.m_pHitCollisionRoot != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pHitCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetHitCollision( pInfo);
	}

	if( data.m_pLightVolumeCollisionRoot != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID(( OBJREF)data.m_pLightVolumeCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetLightVolumeCollision( pInfo);
	}

	if( data.m_pWorldSectionTable != 0)
	{
		i3WorldSectionTable * pTable = (i3WorldSectionTable *) pResFile->FindObjectByID( (OBJREF) data.m_pWorldSectionTable);
		I3ASSERT( pTable != nullptr);

		setWorldSectionTable( pTable);
	}

	if( data.m_pMapSectorTable != 0)
	{
		i3MapSectorTable * pTable = (i3MapSectorTable *) pResFile->FindObjectByID( (OBJREF) data.m_pMapSectorTable);
		I3ASSERT( pTable != nullptr);

		setMapSectorTable( pTable);
	}
	
	for(i = 0; i < 2; i++)
	{
		SetFogEnable( data.m_Fog[i].m_bEnable != 0, (I3FRM_FOG_INDEX) i);
		SetFogMode(		(I3G_FOG_MODE) data.m_Fog[i].m_Mode, (I3FRM_FOG_INDEX) i);

		COLOR col;
		i3Color::ConvertRGBA32( data.m_Fog[i].m_Color, &col);
		SetFogColor( &col, (I3FRM_FOG_INDEX) i);

		SetFogDensity(	data.m_Fog[i].m_Density, (I3FRM_FOG_INDEX) i);
		SetFogNear(		data.m_Fog[i].m_Near, (I3FRM_FOG_INDEX) i);
		SetFogFar(		data.m_Fog[i].m_Far, (I3FRM_FOG_INDEX) i);
	}

	return Result;
}

UINT32  i3World::OnLoad1( i3ResourceFile * pResFile)
{
	UINT32	Result = 0;
	i3Stream	*	pStream = pResFile->GetStream();
	pack::WORLD1	data;
	
	Result = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Result);

	if( data.m_pWorldScene != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pWorldScene);
		I3ASSERT( pInfo != nullptr);

		SetWorld( pInfo);
	}

	if( data.m_pSkyScene != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pSkyScene);
		I3ASSERT( pInfo != nullptr);

		SetSky( pInfo);
	}

	if( data.m_pFogEnableAttr[0] != 0)
	{
		m_pFogEnableAttr[0]		= (i3FogEnableAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogEnableAttr[0]);
		I3ASSERT( m_pFogEnableAttr[0] != nullptr);
		m_pWorldRoot->AddAttr( m_pFogEnableAttr[0]);
	}

	if( data.m_pFogColorAttr[0] != 0)
	{
		m_pFogColorAttr[0]		= (i3FogColorAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogColorAttr[0]);
		I3ASSERT(data.m_pFogColorAttr[0] != 0);
		m_pWorldRoot->AddAttr( m_pFogColorAttr[0]);
	}

	if( data.m_pFogDensityAttr[0] != 0)
	{
		m_pFogDensityAttr[0]	= (i3FogDensityAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogDensityAttr[0]);
		I3ASSERT(m_pFogDensityAttr[0] != nullptr);
		m_pWorldRoot->AddAttr( m_pFogDensityAttr[0]);
	}

	if( data.m_pFogModeAttr[0] != 0)
	{
		m_pFogModeAttr[0]		= (i3FogModeAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogModeAttr[0]);
		I3ASSERT(m_pFogModeAttr[0] != nullptr);
		m_pWorldRoot->AddAttr( m_pFogModeAttr[0]);
	}

	if( data.m_pFogNearFarAttr[0] != 0)
	{
		m_pFogNearFarAttr[0]	= (i3FogNearFarAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogNearFarAttr[0]);
		I3ASSERT(m_pFogNearFarAttr[0] != nullptr);
		m_pWorldRoot->AddAttr( m_pFogNearFarAttr[0]);
	}

	//  1.  월드 루트에 붙이는 순간을 포그 적용시점으로 보고,
	//  2.  이 시점에 일단 파티클쪽 렌더에도 포그 적용이 가능하도록 RenderAttr을 보내도록 한다...
	//  3.  데칼의 경우 포그가 함께 적용되야하는데, 데칼이 아닌 경우도 일단은 포그 적용이 되도록 가정한다.
	//  기타 )  파티클은 i3Viewer의 별도Trace 및 EffectRoot의 파티클루트의 별도 Trace처리가 되어 월드와 완전이 분리된 상태이고, 
	//         월드의 포그가 전혀 적용되지 않기 때문에 별도 적용이 꼭 필요하다..
	

	
	if( data.m_pFogEnableAttr[1] != 0)
	{
		m_pFogEnableAttr[1]		= (i3FogEnableAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogEnableAttr[1]);
		I3ASSERT(m_pFogEnableAttr[1] != nullptr);
		m_pSkyRoot->AddAttr( m_pFogEnableAttr[1]);
	}

	if( data.m_pFogColorAttr[1] != 0)
	{
		m_pFogColorAttr[1]		= (i3FogColorAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogColorAttr[1]);
		I3ASSERT(m_pFogColorAttr[1] != nullptr);
		m_pSkyRoot->AddAttr( m_pFogColorAttr[1]);
	}

	if( data.m_pFogDensityAttr[1] != 0)
	{
		m_pFogDensityAttr[1]	= (i3FogDensityAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogDensityAttr[1]);
		I3ASSERT(m_pFogDensityAttr[1] != nullptr);
		m_pSkyRoot->AddAttr( m_pFogDensityAttr[1]);
	}

	if( data.m_pFogModeAttr[1] != 0)
	{
		m_pFogModeAttr[1]		= (i3FogModeAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogModeAttr[1]);
		I3ASSERT(m_pFogModeAttr[1] != nullptr);
		m_pSkyRoot->AddAttr( m_pFogModeAttr[1]);
	}

	if( data.m_pFogNearFarAttr[1] != 0)
	{
		m_pFogNearFarAttr[1]	= (i3FogNearFarAttr*)pResFile->FindObjectByID( (OBJREF)data.m_pFogNearFarAttr[1]);
		I3ASSERT(m_pFogNearFarAttr[1] != nullptr);
		m_pSkyRoot->AddAttr( m_pFogNearFarAttr[1]);
	}

	if( data.m_pWorldCollisionRoot	!=  0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pWorldCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetWorldCollision( pInfo);
	}

	if( data.m_pHitCollisionRoot != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID( (OBJREF)data.m_pHitCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetHitCollision( pInfo);
	}

	if( data.m_pLightVolumeCollisionRoot != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo*)pResFile->FindObjectByID(( OBJREF)data.m_pLightVolumeCollisionRoot);
		I3ASSERT( pInfo != nullptr);
		SetLightVolumeCollision( pInfo);
	}
	
	return Result;
}

void i3World::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	MATRIX mtx;

	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);

	pInfo->_pMatrixStack->PushPre( &mtx, getSceneObject()->GetMatrix());

	if((m_pWorldCollisionRoot != nullptr) && (m_pWorldCollisionRoot->getInstanceSg() != nullptr))
	{
		m_pWorldCollisionRoot->getInstanceSg()->OnInstancing( pInfo);
	}

	if((m_pHitCollisionRoot != nullptr) && (m_pHitCollisionRoot->getInstanceSg() != nullptr))
	{
		m_pHitCollisionRoot->getInstanceSg()->OnInstancing( pInfo);
	}

	if((m_pLightVolumeCollisionRoot != nullptr) && (m_pLightVolumeCollisionRoot->getInstanceSg() != nullptr))
	{
		m_pLightVolumeCollisionRoot->getInstanceSg()->OnInstancing( pInfo);
	}

	pInfo->_pMatrixStack->Pop();

	i3GameObj::OnInstancing( pInfo);
}

namespace
{
	i3AttrSet*		CreateOccQuadNode(const i3::occ_culler* culler, i3::vector<i3::occ_quad*>& out_quad_list)
	{
		out_quad_list.clear();

		size_t num_quad_set = culler->get_num_occ_quad_set();

		if (num_quad_set == 0)
			return nullptr;
				
		i3AttrSet* pAttrQuadSet = i3AttrSet::new_object();

		pAttrQuadSet->AddFlag(I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL |
			I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
		pAttrQuadSet->SetName("DEBUG_Renderable_OccQuadSet");
		
		{
			COLOR color;
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			i3Color::Set(&color, (UINT8)64, 255, 64, 128);
			pAttr->Set(&color);
			pAttrQuadSet->AddAttr(pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);	pAttrQuadSet->AddAttr(pAttr);
		}
		
		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
			pAttr->Set(false);	pAttrQuadSet->AddAttr(pAttr);
		}

		{
			i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
			pAttr->Set(false);	pAttrQuadSet->AddAttr(pAttr);
		}

		{
			i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
			pAttr->setBias(0.f); 	pAttrQuadSet->AddAttr(pAttr);
		}

		i3VertexArray*	pVA = nullptr;
		COLORREAL col; 
		
		for (size_t i = 0; i < num_quad_set; ++i)
		{
			const i3::occ_quad_set* quad_set = culler->get_occ_quad_set(i);

			const size_t num_quad = quad_set->get_num_face();

			for (size_t j = 0; j < num_quad; ++j)
			{
				i3::occ_quad* quad = quad_set->get_occ_quad(j);
				// 메시와 쿼드를 리니어하게 채워준다...

				const size_t numLine = (quad->is_quad_triangle()) ? 3 : 4;			
			
				i3Node* pRenderLine = i3SceneUtil::BuildLine(pVA, numLine, nullptr);

				INT32 vertex_count = 0;

				pVA->Lock();
			
				for (size_t k = 0; k < numLine; ++k, vertex_count += 2)
				{
					size_t first_idx = k;
					size_t second_idx = (k + 1) % numLine;
					const i3::vec3& first_pos =	quad->get_quad_vertex_pos(first_idx);
					const i3::vec3& second_pos = quad->get_quad_vertex_pos(second_idx);

					pVA->SetPosition(vertex_count, const_cast<VEC3D*>(reinterpret_cast<const VEC3D*>(&first_pos)));
					pVA->SetPosition(vertex_count+1, const_cast<VEC3D*>(reinterpret_cast<const VEC3D*>(&second_pos)));

				}
				pVA->Unlock();

				pAttrQuadSet->AddChild(pRenderLine);			// 순서대로..
				pRenderLine->Release();
				out_quad_list.push_back(quad);
			}
					
		}

		pAttrQuadSet->AddFlag(I3_NODEFLAG_DISABLE);		// 디폴트로 꺼둔 상태임..

		return pAttrQuadSet;
	}
}

void	i3World::CreateOccCull(const i3::occ_mesh_set& occMeshSet)
{
	I3ASSERT(m_pWorldScene != nullptr);

	i3BoundBox i3Box;
	m_pWorldScene->getInstanceSg()->GetWrappingBound(&i3Box);

	const VEC3D* pMin = i3Box.GetMin();
	const VEC3D* pMax = i3Box.GetMax();

	i3::aabb  world_aabb(pMin->x, pMin->y, pMin->z, pMax->x, pMax->y, pMax->z);

	m_octree = i3::make_shared<i3::octree>(world_aabb);
	m_occ_culler = i3::make_shared<i3::occ_culler>(m_octree.get());

	m_query_callback = i3::make_shared<occ_hw_callback_derived>();
	
	m_occ_culler->set_hw_query_callback(m_query_callback.get());

//	m_occ_query = i3GfxOccCuller::new_object();
//	m_occ_query->Create();

	m_occ_culler->register_occ_quads(&occMeshSet);



	i3Framework* pCurrFramework = i3Framework::getCurrentFramework();

	if (pCurrFramework)
	{
		OccDebugInfo* pInfo = pCurrFramework->GetViewer()->getOccDebugInfo();
		if (pInfo)
		{
			m_pRenderOccQuad = CreateOccQuadNode(m_occ_culler.get(), m_occ_quad_list);
		}
	}
}

bool	i3World::HasWorldOccQuads() const
{
	return m_occ_culler.get() != nullptr;
}

void	i3World::AttachScene(void)
{
	if (this->IsAttachedScene()) 
		return;
	
	i3GameObj::AttachScene();

	if (m_pRenderOccQuad && m_pRenderOccQuad->GetParent() == nullptr)
		getAttachNode()->AddChild(m_pRenderOccQuad);
	//	getSgRes()->getAttachRoot()->AddChild(m_pRenderOccQuad);

}

void	i3World::DetachScene(void)
{
	if (!this->IsAttachedScene())
		return;

	i3GameObj::DetachScene();
}

void	i3World::EnableOccCullMesh(bool bEnable)
{
	if (m_pRenderOccQuad == nullptr)
		return;

	if (bEnable)
		m_pRenderOccQuad->RemoveFlag(I3_NODEFLAG_DISABLE);
	else
		m_pRenderOccQuad->AddFlag(I3_NODEFLAG_DISABLE);
}

bool	i3World::IsEnabledOccCullMesh() const
{
	if (m_pRenderOccQuad == nullptr)
		return false;
	return m_pRenderOccQuad->IsFlag(I3_NODEFLAG_DISABLE) == false;
}

