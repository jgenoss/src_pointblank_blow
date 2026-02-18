#include "pch.h"
#include "GameWorld.h"
#include "GlobalVariables.h"
#include "UserInfoContext.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include <sstream>

//I3_CONCRETE_CLASS_INSTANCE( CGameWorld, i3World);

I3_CLASS_INSTANCE( CGameWorld);

CGameWorld::CGameWorld(void)
{
#if defined (SHOW_COLLISION_MESH)
	m_bCreateRenderCol					= false;
	m_pMeshHeroCol						= nullptr;
	m_pMeshWorldCol						= nullptr;
	m_pMeshHitCol						= nullptr;
	m_pMeshLightCol						= nullptr;
	m_pMeshObjSMCol						= nullptr;

	m_renColliderColorTable				= nullptr;
	m_renColliderLogger					= nullptr;
#endif

	m_pSndGeom = nullptr;
	m_SafeBinded[0] = false;
	m_SafeBinded[1] = false;
	m_SafeBinded[2] = false;
	m_SafeBinded[3] = false;
}

CGameWorld::~CGameWorld(void)
{
	if( g_pFramework != nullptr)
	{
		i3CollisionContext * pCtx = g_pFramework->getCollisionContext();

		if( pCtx != nullptr)
		{
			if( m_pLightVolumeCollisionRoot)
			{
				pCtx->RemoveWorld( m_pLightVolumeCollisionRoot->getInstanceSg());
			}
		}

		//Detach FogAttrs
		{
			//Attach FogAttrs
			i3AttrSet * pLightRoot = (i3AttrSet*)g_pFramework->Get3DRoot();
			{
				pLightRoot->RemoveAttr( m_pFogEnableAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->RemoveAttr( m_pFogColorAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->RemoveAttr( m_pFogDensityAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->RemoveAttr( m_pFogModeAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->RemoveAttr( m_pFogNearFarAttr[ I3FRM_FOG_WORLD]);
			}
		}
	}

	#if defined( SHOW_COLLISION_MESH)
	m_bCreateRenderCol = false;
	I3_SAFE_DELETE( m_renColliderColorTable);
	I3_SAFE_DELETE( m_renColliderLogger);
	#endif

	I3_SAFE_RELEASE( m_pSndGeom);
}

INT32 _ToInt( char* val )
{
	std::istringstream str(val);
	int ret = 0;
	str >> ret;
	return ret;
}

inline INT32 _ToInt(const wchar_t* val)
{
	return val ? _wtoi(val) : 0;
}

void CGameWorld::OnBindResource(void)
{
	PB_VM_S

	i3World::OnBindResource();

	//Collision
	if( g_pFramework != nullptr)
	{
		if( m_pWorldCollisionRoot != nullptr && m_pWorldCollisionRoot->getInstanceSg())
		{
			m_pWorldCollisionRoot->getInstanceSg()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
			InitColliderSet( CGRP_WORLD, (UINT32) CGRP_MASK_WORLD, m_pWorldCollisionRoot->getInstanceSg());
			m_SafeBinded[0] = true;
		}

		if( m_pHitCollisionRoot != nullptr && m_pHitCollisionRoot->getInstanceSg())
		{
			m_pHitCollisionRoot->getInstanceSg()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
			InitColliderSet( CGRP_WORLD_ATTACK, (UINT32) CGRP_MASK_WORLD_ATTACK, m_pHitCollisionRoot->getInstanceSg());
			m_SafeBinded[1] = true;
		}

		if( m_pLightVolumeCollisionRoot != nullptr && m_pLightVolumeCollisionRoot->getInstanceSg())
		{
			m_pLightVolumeCollisionRoot->getInstanceSg()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
			g_pFramework->getCollisionContext()->AddWorld( m_pLightVolumeCollisionRoot->getInstanceSg());
			InitColliderSet( CGRP_LIGHTVOLUME, (UINT32) 0, m_pLightVolumeCollisionRoot->getInstanceSg());
			m_SafeBinded[2] = true;
		}

#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverMe() == false)
#else
		if(UserInfoContext::i()->IsGM_Observer() == false)
#endif
		{
			//Attach FogAttrs
			i3AttrSet * pLightRoot = (i3AttrSet*)g_pFramework->Get3DRoot();
			{
				pLightRoot->AddAttr( m_pFogEnableAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->AddAttr( m_pFogColorAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->AddAttr( m_pFogDensityAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->AddAttr( m_pFogModeAttr[ I3FRM_FOG_WORLD]);
				pLightRoot->AddAttr( m_pFogNearFarAttr[ I3FRM_FOG_WORLD]);
			}
		}

		if( m_pMapSectorTable != nullptr)
		{
			char MapCountString[MAX_PATH] = "";
			char MapLabel[MAX_PATH] = "";
			INT32 nCount;
			const char * pszStageName = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex())->GetName();

			//MapCountString = NPort_Count
			i3::snprintf( MapCountString, MAX_PATH, "%s_Count", pszStageName);

			//chunjong.song 140728 - GAME_WORLD_STRING은 이제 wchar_t로 넘겨준다. 
			const wchar_t* ret = GAME_WORLD_STRING(MapCountString);

			nCount = _ToInt(ret);

			if ( m_pMapSectorTable->getCount() != nCount )
			{
				I3PRINTLOG(I3LOG_FATAL, "맵 구역에 대한 스트링 불일치 [ %s ] [ textcount : %d != map count : %d] - map count [ %d ] 갯수로 맞추면 된다.", 
					pszStageName, nCount, m_pMapSectorTable->getCount(), m_pMapSectorTable->getCount());
			}

			for( INT32 i = 0;i < nCount; ++i)
			{
				i3::pack::MAP_SECTOR_INFO *	SectorTable = (i3::pack::MAP_SECTOR_INFO*)m_pMapSectorTable->getMapSector( i);

				//NPort_0
				sprintf_s( MapLabel, "%s_%d", pszStageName, i);
				
				i3::string strMapName;	i3::utf16_to_mb(GAME_WORLD_RCSTRING(MapLabel), strMapName);

				i3::safe_string_copy( SectorTable->m_szName, strMapName,  sizeof(SectorTable->m_szName) );
			
			}
		}

#if defined ( ENABLE_SOUNDGEOMETRY)
		if( g_pViewer->GetSoundContext() != nullptr)
		{
			I3_SAFE_RELEASE( m_pSndGeom);

			if( i3FileStream::isFile( "World/Library/test.fgf"))
			{
				m_pSndGeom = i3FMODSoundGeometry::new_object();
				m_pSndGeom->Create( g_pViewer->GetSoundContext());

				m_pSndGeom->LoadGeometryGroup( "World/Library/test.fgf");
			}
			/*
			List list;

			pSndCtx->addGeometry( "World/Library/test.fgf", &list);
			
			{
				i3AttrSet * pAttrSet = i3AttrSet::new_object();

				{
					i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
					pAttr->Set( true);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
					pAttr->Set( false);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
					pAttr->SetSource( I3G_BLEND_SRCALPHA);
					pAttr->SetDestination( I3G_BLEND_ONE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
					pAttr->Set( false);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
					pAttr->Set( false);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();
					pAttr->Set( true);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();

					pAttr->setBias( -0.0002f);
					pAttrSet->AddAttr( pAttr);
				}

				for( INT32 i = 0;i < list.GetCount(); ++i)
				{
					i3AttrSet * pChildAttr = i3AttrSet::new_object_ref();

					pAttrSet->AddChild( pChildAttr);
					{
						i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
						COLOR col;
						
						i3Color::Set( &col, i3Math::Rand()%250 + 5, i3Math::Rand()%250 + 5, i3Math::Rand()%250 + 5, 255);

						pAttr->Set( &col);
						pChildAttr->AddAttr( pAttr);
					}
					
					i3Geometry * pGeom = i3Geometry::new_object_ref();

					pChildAttr->AddChild( pGeom);

					i3GeometryAttr * pGeomAttr = i3GeometryAttr::new_object_ref();

					i3VertexArray * pVA = (i3VertexArray*)list.GetItem(i);

					pGeomAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pVA->GetCount() / 3);

					pGeom->AppendGeometryAttr( pGeomAttr);
				}

				{
					i3ResourceFile file;
					i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object();
					pInfo->setInstanceSg( pAttrSet);

					file.setKeyObject(pInfo);
					file.Save( "jkdsla.i3s");
					GetNode()->AddChild( pAttrSet);
				}
			
			}	*/
		}
#endif

		//
		// 월샷 해킹때문에 들어감.
		// 해킹으로 이 함수 시작부분에서 리턴시켜버리면 벽뚫고 총알이 날라다님.
		// 
		m_SafeBinded[3] = true;
	}
	PB_VM_E
}

void CGameWorld::OnUpdate( REAL32 rDeltaSeconds)
{
	if( m_pSkyScene != nullptr)
	{
		VEC3D  vCamPos, vSkyPos;
		i3Vector::Copy( &vCamPos, i3Matrix::GetPos( g_pCamera->getCamMatrix()));

		i3Vector::Copy( &vSkyPos, i3Matrix::GetPos( m_pSkyTrans->GetMatrix()));
		i3Vector::SetY( &vCamPos, getY( &vSkyPos));

		i3Matrix::SetPos( m_pSkyTrans->GetMatrix(), &vCamPos);
	}

	/*
	if( g_pFramework != nullptr )
	{
		i3Viewer * pViewer = g_pFramework->GetViewer();
		i3InputDeviceManager	*	pMng	= pViewer->GetInputDeviceManager();
		i3InputKeyboard			*	pKey	= pMng->GetKeyboard();

		if( pKey->GetStrokeState( I3I_KEY_8 ) )
		{
			i3SoundContext * pCtx = pViewer->GetSoundContext();

			((i3SoundContextFMOD*)pCtx)->showReverbConfigDialog( pViewer->GetWindowHandle(), true);
		}
	}*/
	i3World::OnUpdate( rDeltaSeconds);
}

void CGameWorld::SetEnableViewCol( INT32 nType, UINT32 opt )
{
#if defined (SHOW_COLLISION_MESH)
	if(opt & 0x4)
	{
		i3SceneUtil::RenColliderColorTable::GenerateDefaultIni();
		return;
	}

	if( !m_bCreateRenderCol )
	{
		if( !m_renColliderColorTable)
			m_renColliderColorTable = new i3SceneUtil::RenColliderColorTable();

		if( (opt & 0x2) && !m_renColliderLogger)
			m_renColliderLogger = new i3SceneUtil::BuildLogger();

		if(m_renColliderLogger)
			m_renColliderLogger->Reset();

		getSceneObject()->Instantiate();
		
		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
		GetNode()->AddChild( pAttrSet);

		{
			i3LightingEnableAttr * pLightingAttr = i3LightingEnableAttr::new_object_ref();
			pLightingAttr->Set( false);
			pAttrSet->AddAttr( pLightingAttr);

			COLORREAL		col;
			i3MaterialAttr * pMaterialAttr = i3MaterialAttr::new_object_ref();

			i3Color::Set( &col, 0.8f, 0.8f, 0.8f, 1.0f);
			pMaterialAttr->SetDiffuse( &col);

			pAttrSet->AddAttr( pMaterialAttr);
		}

		if( m_pWorldCollisionRoot != nullptr && m_pWorldCollisionRoot->getInstanceSg() )
		{
			if(m_renColliderColorTable)
				m_renColliderColorTable->SetColorCate(i3SceneUtil::RenColliderColorTable::ECATE_SM);

			if(m_renColliderLogger)
				m_renColliderLogger->SetSection("[World_Col_Hero]");

			m_pMeshWorldCol = i3SceneUtil::BuildRendableCollider( m_pWorldCollisionRoot->getInstanceSg(), 
				pAttrSet, true, true, true, nullptr, nullptr, true, 
				m_renColliderLogger, 0, m_renColliderColorTable);

		#if defined( I3_DEBUG)
			m_pMeshWorldCol->SetName( "World Col Root");
		#endif
			m_pMeshWorldCol->AddFlag( I3_NODEFLAG_DISABLE );
		}
		
		if( m_pHitCollisionRoot != nullptr && m_pHitCollisionRoot->getInstanceSg() )
		{
			if(m_renColliderColorTable)
				m_renColliderColorTable->SetColorCate(i3SceneUtil::RenColliderColorTable::ECATE_HIT);

			if(m_renColliderLogger)
				m_renColliderLogger->SetSection("[World_Col_Hit]");

			m_pMeshHitCol = i3SceneUtil::BuildRendableCollider( m_pHitCollisionRoot->getInstanceSg(), 
				pAttrSet, true, true, true, nullptr, nullptr, true, 
				m_renColliderLogger, 0, m_renColliderColorTable);

#if defined( I3_DEBUG)
			m_pMeshHitCol->SetName( "Hit Col Root");
#endif
			m_pMeshHitCol->AddFlag( I3_NODEFLAG_DISABLE );
		}

		if( m_pLightVolumeCollisionRoot != nullptr && m_pLightVolumeCollisionRoot->getInstanceSg() )
		{
			if(m_renColliderColorTable)
				m_renColliderColorTable->SetColorCate(i3SceneUtil::RenColliderColorTable::ECATE_DEFAULT);

			if(m_renColliderLogger)
				m_renColliderLogger->SetSection("[World_Col_Light]");

			m_pMeshLightCol = i3SceneUtil::BuildRendableCollider( m_pLightVolumeCollisionRoot->getInstanceSg(), 
				pAttrSet, true, true, true, nullptr, nullptr, true, 
				m_renColliderLogger, 0, m_renColliderColorTable);

#if defined( I3_DEBUG)
			m_pMeshLightCol->SetName( "LightVolume Col Root");
#endif
			m_pMeshLightCol->AddFlag( I3_NODEFLAG_DISABLE );
		}

		pAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE);
		
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( pStage != nullptr )
		{
			if( pStage->GetStageObjectList() != nullptr )
			{
				m_pMeshObjSMCol = i3Node::new_object_ref();
#if defined( I3_DEBUG)
				m_pMeshObjSMCol->SetName( "Object SM Col Root");
#endif
				m_pMeshObjSMCol->AddFlag( I3_NODEFLAG_DISABLE);

				pAttrSet->AddChild( m_pMeshObjSMCol);

				i3Node * pCollision = nullptr;

				if(m_renColliderColorTable)
					m_renColliderColorTable->SetColorCate(i3SceneUtil::RenColliderColorTable::ECATE_SM);

				if(m_renColliderLogger)
					m_renColliderLogger->SetSection("[World_Col_Hero]");

				for( size_t i = 0; i < pStage->GetStageObjectList()->size(); i++)
				{
					i3Object * pObj = (*pStage->GetStageObjectList())[i];

					pCollision = i3SceneUtil::BuildRendableCollider( pObj->GetNode(), m_pMeshObjSMCol, 
						true, true, true, nullptr, nullptr, true, nullptr, 0, m_renColliderColorTable);
					
					/*i3TransformRef * pTransRef = i3TransformRef::new_object_ref();
					pTransRef->SetMatrix( pObj->GetMatrix());
					for( INT32 j = 0; j < pCollision->GetChildCount(); j++)
					{
						i3Node * pChild = pCollision->GetChild( j);
						pChild->ChangeParent( pTransform);
					}
					pCollision->AddChild( pTransform);*/
				}
			}
		}

		if(m_renColliderLogger)
			m_renColliderLogger->ShowMsgBox(g_pViewer->GetWindowHandle());

		m_bCreateRenderCol = true;
	}
		

	if( m_pMeshWorldCol != nullptr && (nType & SHOW_COLLISION_TYPE_SM))
	{
		if( m_pMeshWorldCol->IsFlag( I3_NODEFLAG_DISABLE))
			m_pMeshWorldCol->RemoveFlag( I3_NODEFLAG_DISABLE );
		else
			m_pMeshWorldCol->AddFlag( I3_NODEFLAG_DISABLE );
	}

	if( m_pMeshHitCol != nullptr && (nType & SHOW_COLLISION_TYPE_HIT))
	{
		if( m_pMeshHitCol->IsFlag( I3_NODEFLAG_DISABLE))
			m_pMeshHitCol->RemoveFlag( I3_NODEFLAG_DISABLE );
		else
			m_pMeshHitCol->AddFlag( I3_NODEFLAG_DISABLE );
	}

	if( m_pMeshLightCol != nullptr && (nType & SHOW_COLLISION_TYPE_LIGHT))
	{
		if( m_pMeshLightCol->IsFlag( I3_NODEFLAG_DISABLE))
			m_pMeshLightCol->RemoveFlag( I3_NODEFLAG_DISABLE );
		else
			m_pMeshLightCol->AddFlag( I3_NODEFLAG_DISABLE );
	}

	if( m_pMeshObjSMCol != nullptr )
	{
		if( m_pMeshObjSMCol->IsFlag( I3_NODEFLAG_DISABLE))
			m_pMeshObjSMCol->RemoveFlag( I3_NODEFLAG_DISABLE);
		else
			m_pMeshObjSMCol->AddFlag( I3_NODEFLAG_DISABLE);
	}
#endif
}

