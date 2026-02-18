#include "pch.h"
#include "DotLazer.h"
#include "GameCharaCameraContext.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( CDotLazer);//, i3ElementBase);

CDotLazer::CDotLazer()
{
	m_pAttrSetDot = nullptr;
	m_pVADot = nullptr;
	m_pBillboardGlow = nullptr;	
	m_pTargetTextureAttr = nullptr;
	m_pDotStamp = nullptr;

	m_fThickness = 0.004f;
	m_fDistance = 0.5f;

	m_pGeoAttr = nullptr;

	i3mem::FillZero( m_pos, sizeof( m_pos));
	i3mem::FillZero( m_tex, sizeof( m_tex));
}

void CDotLazer::CreateLazerDot(void)
{
	if( m_pDotStamp) DotStampDisable();	

	UINT32 idx = g_pEffectManager->FindEffect( "Env_TargetLaser_01");
	m_pDotStamp = g_pEffectManager->AddEffect( idx, &I3_YAXIS);

	I3ASSERT( m_pDotStamp != nullptr);
	
	m_pDotStamp->AddFlag(I3_NODEFLAG_DISABLE_RENDER_OFFSCREEN);			// 서브 카메라 렌더타켓에는 반영되지 않도록 처리...(도트사이트의 파티클 최대갯수가 작어서 문제가 될수 있었다.)
	m_pDotStamp->RemoveFlag(I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
}

void CDotLazer::DotStampDisable()
{
	if( m_pDotStamp != nullptr)
		m_pDotStamp->AddFlag(I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
}

void CDotLazer::DotStampEnable()
{
	if (m_pDotStamp != nullptr)
		m_pDotStamp->RemoveFlag(I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
}

void CDotLazer::SetEnableLaserBar( bool bFlag)				
{
	if( m_pAttrSetDot != nullptr)
	{
		if(bFlag == false)
			m_pAttrSetDot->AddFlag(I3_NODEFLAG_DISABLE);
		else
			m_pAttrSetDot->RemoveFlag(I3_NODEFLAG_DISABLE);
	}
}

void CDotLazer::SetEnableDotLaser( bool bFlag)				
{ 
	if( m_pAttrSetDot != nullptr)
	{
		if(bFlag == false)
		{
			m_pAttrSetDot->AddFlag(I3_NODEFLAG_DISABLE);
			DotStampDisable();
		}
		else
		{
			m_pAttrSetDot->RemoveFlag(I3_NODEFLAG_DISABLE);
			if( m_pDotStamp)
			{
				m_pDotStamp->RemoveFlag(I3_NODEFLAG_DISABLE| I3_NODEFLAG_INVISIBLE);
			}
		}
	}
}


void CDotLazer::UpdateDotStamp()
{
	if( m_pDotStamp != nullptr)
		i3Matrix::SetPos( m_pDotStamp->GetMatrix(), &m_vecDotPos);
}

void CDotLazer::CreateLazerSight(i3SceneObject* pSceneObj)
{
	i3Texture * pTex = nullptr;
	
	i3ResourceManager * pMng = g_pFramework->GetResourceManager();

	{	
		pTex = pMng->LoadTexture( "Effect/Tex_LaserBeam01.i3i");
	}

	i3Node * pFXLaserSightDummy = i3Scene::FindNodeByName( pSceneObj, "FXLaserSightDummy");

	if(pFXLaserSightDummy == nullptr)
	{
		i3Skeleton * pFrameSkel = pSceneObj->GetBody()->getFrameLOD()->getSkeleton();
		i3AttrSet* pRenderableBone = i3AttrSet::new_object_ref();
		INT32 iRenderableBoneCount = pFrameSkel->getBoneCount();
		INT32 i = 0;

		for( i = 0; i < iRenderableBoneCount; i++)
		{
			if(i3::generic_is_iequal(pFrameSkel->getBone( i)->getName(), "FXLaserSightDummy") )
			{	
				i3BoneRef * pBoneTrans = i3BoneRef::new_object_ref();
				I3ASSERT( pBoneTrans != nullptr);

				pBoneTrans->SetName( pFrameSkel->getBone( i)->getName());
				pBoneTrans->setBoneIndex( i);

				pRenderableBone->AddChild( pBoneTrans);
				pFXLaserSightDummy = (i3Node*)pBoneTrans;
			}
		}

		pSceneObj->GetBody()->AddChild( pRenderableBone);

		I3ASSERT_RETURN(pFXLaserSightDummy);
	}

	m_pAttrSetDot = i3AttrSet::new_object();	

	// 레이져 사이트 Attrs
	{
		{
			i3TextureEnableAttr * pTexEnableAttr = i3TextureEnableAttr::new_object_ref();
			pTexEnableAttr->Set(true);
			m_pAttrSetDot->AddAttr(pTexEnableAttr);
		}

		{	
			m_pTargetTextureAttr = i3TextureBindAttr::new_object();
			m_pAttrSetDot->AddAttr( m_pTargetTextureAttr);
			m_pTargetTextureAttr->SetTexture( pTex);
			I3_MUST_RELEASE(pTex);
		}

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( true);
			m_pAttrSetDot->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_ONE);
			m_pAttrSetDot->AddAttr( pAttr);
		}

		{
			i3AlphaTestEnableAttr * pAlphaTestAttr = i3AlphaTestEnableAttr::new_object_ref();
			pAlphaTestAttr->Set( true);
			m_pAttrSetDot->AddAttr( pAlphaTestAttr);
		}

		{
			i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();
			pAttr->SetFunction( I3G_COMP_GREATER);
			pAttr->SetRefValue( 50);
//			i3::safe_string_copy(pAttr->m_szDebugName, "Dot i3AlphaFuncAttr");
			m_pAttrSetDot->AddAttr( pAttr);

		}
		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();
			pAttr->Set( I3G_FACE_CULL_NONE);
//			i3::safe_string_copy(pAttr->m_szDebugName, "Dot I3G_FACE_CULL_NONE");
			m_pAttrSetDot->AddAttr( pAttr);
		}
	}
	
	{
		i3Geometry * pGeo = i3Geometry::new_object_ref();

		m_pGeoAttr = i3GeometryAttr::new_object_ref();

		//i3::safe_string_copy(m_pGeoAttr->m_szDebugName, "Dot i3GeometryAttr");

		CreateVertex();

		pGeo->AppendGeometryAttr( m_pGeoAttr);
		pGeo->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

		m_pAttrSetDot->AddChild( pGeo);
	}

	
	VEC3D vecDir;

	i3Vector::Set(&m_StartVector, 0.f, 0.f, 0.f);
	i3Vector::Set(&vecDir, 0.f, 1.f, 0.f);
	i3Vector::Scale( &vecDir, &vecDir, m_fDistance);
	i3Vector::Add( &m_EndVector, &m_StartVector, &vecDir);

	//   [0]           [1]
	//   +-------------+
	//   |             |
	//   |             |
	//   |             |
	//   +-------------+
	//   [2]           [3]

	// 0 2 1 - 1 2 3

	
	//	Position
	i3Vector::Set( &m_pos[0], i3Vector::GetX( &m_StartVector)+m_fThickness, i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector));
	i3Vector::Set( &m_pos[1], i3Vector::GetX( &m_EndVector)+m_fThickness, i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector));
	i3Vector::Set( &m_pos[2], i3Vector::GetX( &m_StartVector)-m_fThickness, i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector));
	i3Vector::Set( &m_pos[3], i3Vector::GetX( &m_StartVector)-m_fThickness, i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector));
	i3Vector::Set( &m_pos[4], i3Vector::GetX( &m_EndVector)+m_fThickness, i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector));
	i3Vector::Set( &m_pos[5], i3Vector::GetX( &m_EndVector)-m_fThickness, i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector));

	i3Vector::Set( &m_pos[6], i3Vector::GetX( &m_StartVector), i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector)-m_fThickness);
	i3Vector::Set( &m_pos[7], i3Vector::GetX( &m_EndVector), i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector)-m_fThickness);
	i3Vector::Set( &m_pos[8], i3Vector::GetX( &m_StartVector), i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector)+m_fThickness);
	i3Vector::Set( &m_pos[9], i3Vector::GetX( &m_StartVector), i3Vector::GetY( &m_StartVector), i3Vector::GetZ( &m_StartVector)+m_fThickness);
	i3Vector::Set( &m_pos[10], i3Vector::GetX( &m_EndVector), i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector)-m_fThickness);
	i3Vector::Set( &m_pos[11], i3Vector::GetX( &m_EndVector), i3Vector::GetY( &m_EndVector), i3Vector::GetZ( &m_EndVector)+m_fThickness);

	//	Texture
	REAL32 rTemp = 1.f/128.f;

	i3Vector::Set( &m_tex[0], 1.f, 0.0f);
	i3Vector::Set( &m_tex[1], rTemp, 0.0f);
	i3Vector::Set( &m_tex[2], 1.f, 1.0f);			
	i3Vector::Set( &m_tex[3], 1.f, 1.0f);
	i3Vector::Set( &m_tex[4], rTemp, 0.0f);			
	i3Vector::Set( &m_tex[5], rTemp, 1.0f);

	i3Vector::Set( &m_tex[6], 1.f, 0.0f);
	i3Vector::Set( &m_tex[7], rTemp, 0.0f);
	i3Vector::Set( &m_tex[8], 1.f, 1.0f);			
	i3Vector::Set( &m_tex[9], 1.f, 1.0f);
	i3Vector::Set( &m_tex[10], rTemp, 0.0f);			
	i3Vector::Set( &m_tex[11], rTemp, 1.0f);
	

	UpdateVertex();
	m_pAttrSetDot->SetPriority( 0);
	pFXLaserSightDummy->AddChild( m_pAttrSetDot);
}

void CDotLazer::CreateVertex(void)
{
	i3VertexFormat fmt;

	fmt.SetHasColor( false);
	fmt.SetHasPosition( true);
	fmt.SetTextureCoordSetCount( 1);
	fmt.SetTextureCoordCount( 2);	

	m_pVADot = i3VertexArray::new_object();
	m_pVADot->Create( &fmt, 6*2, I3G_USAGE_FREQUENT_UPDATE | I3G_USAGE_WRITEONLY);
	//m_pVADot->setRestoreType( I3G_RESTORE_MEM);

	m_pGeoAttr->SetVertexArray( m_pVADot, I3G_PRIM_TRILIST, 4);
}

void CDotLazer::UpdateVertex(void)
{
	m_pVADot->Lock( 0);
	m_pVADot->SetPositions( 0, 12, m_pos);
	m_pVADot->SetTextureCoords( 0, 0, 12, m_tex);
	m_pVADot->Unlock();
}

void CDotLazer::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart, bool bReloaded, REAL32 rRange, CGameCharaBase* pOwner, MATRIX* pFX_Mat)
{
	i3CollideeLine line;
	VEC3D at;
	
	MATRIX mtx;
	VEC3D pos;
	I3_PHYSIX_HIT_RESULT * pLastReult = nullptr;
	REAL32 fFarValue = 100000.0f;

	if( !bReloaded)
	{
		pOwner->getCameraContext()->getVirtualCamPosition( &pos, &mtx);
		i3Vector::Scale( &at, i3Matrix::GetAt( &mtx), - rRange);
		i3Vector::Add( pDest, &at, &pos);

		line.SetStart( &pos);
		line.SetEnd( pDest);
	}
	else
	{
		VEC3D vecDir;
		
		i3Vector::Copy( &m_StartVector, i3Matrix::GetPos( pFX_Mat));

		i3Vector::Normalize( &vecDir, i3Matrix::GetUp( pFX_Mat));
		i3Vector::Scale( &vecDir, &vecDir, 1000.0f);

		i3Vector::Add( &m_EndVector, &m_StartVector, &vecDir);

		line.SetStart( &m_StartVector);
		line.SetEnd( &m_EndVector );
	}

	if( pOwner->is1PV() == true)
	{
		I3_PHYSIX_HIT_RESULT * pResultWorld = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
		g_pFramework->reserveRaycastResult( 1);
		I3_PHYSIX_HIT_RESULT * pResultBlue = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_CHARA_RED);
		g_pFramework->reserveRaycastResult( 2);
		I3_PHYSIX_HIT_RESULT * pResultRed = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_CHARA_BLUE);

		if( pResultWorld != nullptr)
		{				
			if( fFarValue > pResultWorld->m_T)
			{
				pLastReult = pResultWorld;
				fFarValue = pResultWorld->m_T;
			}
		}

		if( pResultBlue != nullptr)
		{				
			if(pResultBlue->m_pDestShapeSet->getEventReceiver() != pOwner)
			{
				if( fFarValue > pResultBlue->m_T)
				{
					pLastReult = pResultBlue;
					fFarValue = pResultBlue->m_T;
				}
			}
		}

		if( pResultRed != nullptr)
		{	
			if(pResultRed->m_pDestShapeSet->getEventReceiver() != pOwner)
			{
				if( fFarValue > pResultRed->m_T)
				{
					pLastReult = pResultRed;
				//	fFarValue = pResultRed->m_T;		// 맨 마지막 fFarValue는 쓰이지 않아서 주석...뒤로 추가되는 것이 있으면 주석을 풀어야한다.. (2015.03.24.수빈)
				}
			}
		}

		if( pLastReult != nullptr)
		{
			VEC3D vPos;
			VEC3D vAt;
			i3Vector::Copy( &vPos, &pLastReult->m_Intersect);

			REAL32 rScale = 1.0f;
			if( pLastReult->m_T > rScale)
			{
				rScale = pLastReult->m_T - rScale;

				if( !bReloaded)
				{
					i3Vector::Scale( &vAt, i3Matrix::GetAt( &mtx), rScale);
				}
				else
				{
					i3Vector::Scale( &vAt, i3Matrix::GetUp(pFX_Mat), -rScale);
				}

				i3Vector::Add( &vPos, &vPos, &vAt);
			}

			i3Vector::Copy( &m_vecDotPos, &vPos);

			if( g_pEnvSet->m_bEnableLaserSight)
			{
				DotStampEnable();			
			}
		}
		else
		{
			DotStampDisable();
		}

		g_pFramework->resetRaycastResult();			
	}
}

void CDotLazer::OnLostDevice( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pVADot);

	if( m_pGeoAttr)	m_pGeoAttr->SetVertexArray( nullptr);
}

void CDotLazer::OnRevive( i3RenderContext * pCtx)
{
	if( m_pGeoAttr)
	{
		CreateVertex();
		UpdateVertex();
	}
}

void CDotLazer::ReturnInstance( void)
{
	I3_SAFE_RELEASE( m_pBillboardGlow);
	I3_SAFE_RELEASE( m_pAttrSetDot);
	I3_SAFE_RELEASE( m_pVADot);
	I3_SAFE_RELEASE( m_pTargetTextureAttr);
	m_pGeoAttr = nullptr;
}