#include "i3SceneDef.h"
#include "i3Math.h"
#include "i3Particle.h"
#include "i3SceneTracer.h"
#include "i3Sprite.h"
#include "i3Quad.h"
#include "i3Billboard.h"
#include "i3Node.h"
#include "i3SceneFile.h"
#include "i3SceneDebug.h"
#include "i3LightingEnableAttr.h"
#include "i3ColorAttr.h"
#include "i3MaterialAttr.h"

#include "i3MeshParticle.h"
#include "i3ParticleGroup.h"
#include "i3ParticleGroupForTriangles.h"
#include "i3ParticleRender.h"
#include "i3DecalManager.h"
#include "i3Decal.h"

//#define I3_TEST_NO_DECAL

static i3FixedMemoryPool *		s_pParticlePool = nullptr;

I3_CLASS_INSTANCE( i3Particle);

static REAL32		g_GravityDir[3] = {	0.0f,	-1.0f,	0.0f };
static REAL32		g_GravityFactor = 98.0f;

I3_TRACE_RESULT I3SG_NODE_PROC( i3Particle)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Particle * pParticle = (i3Particle *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	pParticle->Drive( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

static bool ParticleCloseProc( void * pUserData)
{
	I3_SAFE_DELETE( s_pParticlePool);

	return true;
}

i3Particle::i3Particle(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Particle));

	if( s_pParticlePool == nullptr)
	{
		s_pParticlePool = new i3FixedMemoryPool;
		NetworkDump_Malloc( s_pParticlePool, sizeof( i3FixedMemoryPool), __FILE__, __LINE__);
		s_pParticlePool->Create( "Particle Object Pool", sizeof(I3PARTICLEOBJECT), 0xFFFF);
		s_pParticlePool->SetAutoCompactEnable(false);

		i3System::AddTerminateProc( 0, ParticleCloseProc, nullptr);
	}

	m_pScene = i3AttrSet::new_object();
	m_pScene->AddFlag( I3_NODEFLAG_VOLATILE);		// 함께 저장되어서는 안된다.
	#if defined( I3_DEBUG)
		m_pScene->SetName( "m_pScene");
	#endif

	addParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE | I3_PARTICLE_STYLE_FACETOCAM_ENABLE);

	AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);

	AddChild( m_pScene);
}

i3Particle::~i3Particle(void)
{
	I3_SAFE_RELEASE( m_pGroup);
	I3_SAFE_RELEASE( m_pCustomInfo);
	I3_SAFE_RELEASE( m_pOrigInfo);
	I3_SAFE_RELEASE( m_pInfo);
	I3_SAFE_RELEASE( m_pTexSeq);
	I3_SAFE_RELEASE( m_pMeshInfo);
	I3_SAFE_RELEASE( m_pScene);
	I3_SAFE_RELEASE( m_pBoundBox);
	
	I3_SAFE_RELEASE( m_pTexture);

	// Stop() 혹은 FreeObject() 함수 콜이 필요할듯 싶은데 누락 이유는 아직 파악이 안되었다..  
	// 아마도... OnDeactivate()함수 호출이 항상 존재한다고 가정됨?

	I3MEM_SAFE_FREE_POINTER( m_ppObj ); 
}

void i3Particle::SetGravityDirection( VEC3D * pG)
{
	g_GravityDir[0] = i3Vector::GetX( pG);
	g_GravityDir[1] = i3Vector::GetY( pG);
	g_GravityDir[2] = i3Vector::GetZ( pG);
}

void i3Particle::SetGravityFactor( REAL32 val)
{
	g_GravityFactor = val; 
}

void i3Particle::SetParticleInfo( i3ParticleInfo * pInfo)
{
	I3_REF_CHANGE( m_pInfo, pInfo);
	I3_REF_CHANGE(m_pOrigInfo, pInfo);
}

void i3Particle::SetCustomParticleInfo( i3ParticleInfo * pInfo)
{
	I3_REF_CHANGE( m_pCustomInfo, pInfo);
}

void	i3Particle::EnableCustomParticleInfo(bool bEnable)
{
	m_bCustomParticleInfoUse = bEnable;
	
	i3ParticleInfo* pInfoActive = (bEnable) ? m_pCustomInfo : m_pOrigInfo;
	I3_REF_CHANGE( m_pInfo, pInfoActive);
}

bool	i3Particle::IsEnabledCustomParticleInfo() const
{
	return m_bCustomParticleInfoUse;
}


void i3Particle::setParticleGroup( i3ParticleGroup * pGrp)
{
	I3_REF_CHANGE( m_pGroup, pGrp);
}

void i3Particle::setTexture( i3Texture * pTex)
{
	if( pTex != nullptr)	addParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE);
	else				removeParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE);

	I3_REF_CHANGE( m_pTexture, pTex);
}

void i3Particle::setNormalMap( i3Texture * pTex)
{
	if( pTex != nullptr)	addParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE);
	else				removeParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE);

	I3_REF_CHANGE( m_pNormalMap, pTex);
}

void i3Particle::SetTextureSequence( i3TextureSequence * pTex)
{
	I3_REF_CHANGE( m_pTexSeq, pTex);
}

void i3Particle::SetMesh( i3SceneGraphInfo * pInfo)
{
	INT32 i;
	i3Node * pNode;

	I3_REF_CHANGE( m_pMeshInfo, pInfo);

	for( i = 0; i < m_pScene->GetChildCount(); i++)
	{
		pNode = m_pScene->GetChild( i);
		
		if( !i3::same_of<i3AttrSet*>(pNode))
			break;

		pNode = pNode->GetChild( 0);			// i3Transform

		pNode->RemoveAllChild();				// 이하의 모든 Child Node를 제거한다.

		if( m_pMeshInfo != nullptr)
			pNode->AddChild( GetMesh());
	}
}

void i3Particle::Create( INT32 maxInstance)
{
	INT32 i;
	INT32 maxCnt;

	m_pScene->RemoveAllChild();
	m_pScene->DeactivateNode();

	removeParticleStyle( I3_PARTICLE_STYLE_ROTATE_ENABLE | I3_PARTICLE_STYLE_START | I3_PARTICLE_STYLE_STOP);

	if( isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL) )
		m_pInfo->SetType( i3ParticleInfo::PARTICLE_DECAL);

//	maxCnt = maxInstance * m_pInfo->GetMaxObjectCount();
	maxCnt = maxInstance * m_pInfo->GetMaxObjectCapacity();

	switch( m_pInfo->GetType())
	{
		case i3ParticleInfo::PARTICLE_POINTSPRITE:

			this->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);

			if( m_pGroup == nullptr)
			{
				//	m_pGroup은 addparticle함수 내에서 세팅하도록 되어있음
				g_pParticleRender->AddParticle( this, maxCnt);

				I3ASSERT( m_pGroup != nullptr);
			}
			else if( m_pGroup->getCapacity() <= maxCnt)
			{
				m_pGroup->SetCapacity( maxCnt);
			}

			m_pScene->RemoveFromParent();
			break;

		case i3ParticleInfo::PARTICLE_SPRITE:

			this->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);

			if( m_pGroup == nullptr)
			{
				g_pParticleRender->AddParticle( this, maxCnt);

				I3ASSERT( m_pGroup != nullptr);
			}
			else if( m_pGroup->getCapacity() <= maxCnt)
			{
				m_pGroup->SetCapacity( maxCnt);
			}

			addParticleStyle( I3_PARTICLE_STYLE_ROTATE_ENABLE);			// 회전 가능.
			m_pScene->RemoveFromParent();
			break;

		case i3ParticleInfo::PARTICLE_BILLBOARD:
		case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :

			this->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);

			if( m_pGroup == nullptr)
			{
				g_pParticleRender->AddParticle( this, maxCnt);

				I3ASSERT( m_pGroup != nullptr);
			}
			else if( m_pGroup->getCapacity() <= maxCnt)
			{
				m_pGroup->SetCapacity( maxCnt);
			}
			m_pScene->RemoveFromParent();
			break;

		case i3ParticleInfo::PARTICLE_DECAL :

			this->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);

			if( m_pGroup == nullptr)
			{
				g_pParticleRender->AddParticle( this, maxCnt * 4);

				I3ASSERT( m_pGroup != nullptr);
			}
			else if( m_pGroup->getCapacity() <= maxCnt)
			{
				m_pGroup->SetCapacity( maxCnt);
			}
			m_pScene->RemoveFromParent();
			break;

		case i3ParticleInfo::PARTICLE_SCENE:

			// 파티클씬의 경우 캐스터옵션은 OnLoad를 따르고 일단 방치함...

			for( i = 0; i < m_pInfo->GetMaxObjectCount(); i++)
			{
				i3Transform * pTrans = i3Transform::new_object_ref();
				i3AttrSet *	pAttrSet = i3AttrSet::new_object_ref();

				pAttrSet->SetPriority( 2);

				#if defined( I3_DEBUG)
				pTrans->SetName( "Model Trans");
				pAttrSet->SetName( "Model AttrSet");
				#endif

				{
					i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
					pAttr->Set(isParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE));
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
					pAttr->SetSource( m_BlendSrc);
					pAttr->SetDestination( m_BlendDest);
					pAttr->SetBlendOperation( m_BlendOP);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE));
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE));
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE));
					pAttrSet->AddAttr( pAttr);
				}

				m_pScene->AddChild( pAttrSet);
				pAttrSet->AddChild( pTrans);

				if( m_pMeshInfo != nullptr)
				{
					i3Node * pNode = i3Scene::CreateClone( GetMesh(), I3_COPY_REF);

					pTrans->AddChild( pNode);
				}
			}

			if( m_pScene->GetParent() == nullptr)
			{
				AddChild( m_pScene);
			}

			addParticleStyle( I3_PARTICLE_STYLE_ROTATE_ENABLE);
			break;
	}

	if( m_AllocatedCount != m_pInfo->GetMaxObjectCount())
	{
		if(m_ppObj == nullptr)
		{
			//메모리 할당이 되지 않은 초기 상태.
			AllocateObject();
		}
		else
		{
			// AllocatedCount와 총 오브젝트 갯수가 다른 경우 해제 후 재할당해준다.
			FreeObject();
			// 메모리 재할당. 최대 오브젝트 갯수가 변경될 수 있으므로 이에 맞춰서 메모리 할당해준다.
			AllocateObject();
		}
	}

	// Emit Right & Up Vector
	{
		MATRIX tmx;
		VEC3D zero;

		i3Vector::Set( &zero, 0.0f, 0.0f, 0.0f);

		if( i3Vector::LengthSq( m_pInfo->GetEmitDirection()) > I3_EPS)
		{
			i3Matrix::MakeWithAt( &tmx,m_pInfo->GetEmitDirection(), m_pInfo->GetUpVector());

			i3Vector::Copy( &m_EmitRightVector, i3Matrix::GetRight( &tmx));
			i3Vector::Copy( &m_EmitUpVector, i3Matrix::GetUp( &tmx));
		}
		else
		{
			i3Vector::Copy( &m_EmitRightVector, &zero);
			i3Vector::Copy( &m_EmitUpVector, &zero);	
		}
	}

	// Up Right & At Vector
	{
		MATRIX tmx;
		VEC3D zaxis;

		i3Vector::Set( &zaxis, 0.0f, 0.0f, 1.0f);

		i3Matrix::MakeWithUp( &tmx, m_pInfo->GetUpVector(), &zaxis);

		i3Vector::Copy( &m_UpRightVector, i3Matrix::GetRight( &tmx));
		i3Vector::Copy( &m_UpAtVector, i3Matrix::GetAt( &tmx));
	}

	for( i = FUNC_COUNT - 1, m_PosArgCount = 0; i >= 0; i--)
	{
		if( m_pInfo->GetPositionFunc( i) != i3ParticleInfo::FUNC_NONE)
		{
			m_PosArgCount = i + 1;
			break;
		}
	}

	for( i = FUNC_COUNT - 1, m_ScaleArgCount = 0; i >= 0; i--)
	{
		if( m_pInfo->GetScaleFunc( i) != i3ParticleInfo::FUNC_NONE)
		{
			m_ScaleArgCount = i + 1;
			break;
		}
	}

	if( m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE)
		m_PosInterp = i3KeyframeTable::LERP;
	else
		m_PosInterp = i3KeyframeTable::NONE;

	if( m_pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE)
		m_ScaleInterp = i3KeyframeTable::LERP;
	else
		m_ScaleInterp = i3KeyframeTable::NONE;

	if( m_pInfo->GetColorStyle() & I3_PARTICLE_PARAM_STYLE_INTERPOLATE)
		m_ColorInterp = i3KeyframeTable::LERP;
	else
		m_ColorInterp = i3KeyframeTable::NONE;

	m_RObjectLifeTime = i3Math::div( 1.0f, m_pInfo->GetObjectLifeTime());
}

void i3Particle::Start(void)
{
	INT32 i;

	m_pScene->ActivateNode();

	for( i = 0; i < m_AllocatedCount; i++)
	{
		if( m_ppObj[i] == nullptr)
		{
			m_ppObj[i] = (I3PARTICLEOBJECT *) s_pParticlePool->Alloc( sizeof(I3PARTICLEOBJECT), __FILE__, __LINE__);
		}
		
		m_ppObj[i]->m_Owner = this;			// 일단 할당된 상태에선 바뀌어선 안된다..
		m_ppObj[i]->m_State = 0;			// 
		m_ppObj[i]->m_pTransMtx = nullptr;		// 포인터만 가져다 쓴다.. (렌더러에서 모아놓고, 모두 일괄삭제함)
		m_ppObj[i]->m_PosFromView.x = 0.f;
		m_ppObj[i]->m_PosFromView.y = 0.f;
		m_ppObj[i]->m_PosFromView.z = 1.f;	// 양수이면 카메라 뒷쪽이다..
	}

	for( i = 0; i < m_pInfo->GetInitialObjectCount(); i++)
	{
		_Emit( m_ppObj[i]);
	}

	m_EmitInterval = m_pInfo->GetEmitInterval();
	
	if( m_pInfo->GetPositionUpdateInterval() > 0.0f)
		m_PosRInterval = i3Math::div( 1.0f, m_pInfo->GetPositionUpdateInterval());
	else
		m_PosRInterval = 0.0f;

	if( m_pInfo->GetScaleUpdateInterval() > 0.0f)
		m_ScaleRInterval = i3Math::div( 1.0f, m_pInfo->GetScaleUpdateInterval());
	else
		m_ScaleRInterval = 0.0f;

	if( m_pInfo->GetRotateUpdateInterval() > 0.0f)
		m_RotateRInterval = i3Math::div( 1.0f, m_pInfo->GetRotateUpdateInterval());
	else
		m_RotateRInterval = 0.0f;

	if( m_pInfo->GetColorUpdateInterval() > 0.0f)
		m_ColorRInterval = i3Math::div( 1.0f, m_pInfo->GetColorUpdateInterval());
	else
		m_ColorRInterval = 0.0f;

	addParticleStyle( I3_PARTICLE_STYLE_START);
	removeParticleStyle( I3_PARTICLE_STYLE_STOP);

	m_timeLocal = 0.0f;
}

void i3Particle::Stop(void)
{
	addParticleStyle( I3_PARTICLE_STYLE_STOP);
	m_timeLocal = 0.0f;

	INT32 i;

	for( i = 0; i < m_AllocatedCount; i++)
	{
		if( m_ppObj[i] != nullptr)
		{
			s_pParticlePool->Free( m_ppObj[i]);

			m_ppObj[i] = nullptr;
		}
	}

	_FreeDecal();
}

void i3Particle::_Emit( I3PARTICLEOBJECT * pObj)
{
	REAL32 MassRange;
	MATRIX mtx1;
	REAL32 rnd1, rnd2;
	
	pObj->m_State = I3_PARTICLE_STATE_ENABLE | I3_PARTICLE_STATE_FIRED | I3_PARTICLE_STATE_FIRSTTIME;
	pObj->m_Time = 0.0f;
	//pObj->m_T = 0.0f;

	// Emit된 다음 무조건 값을 초기화할 수 있는 기회를 주기 위해...
	pObj->m_PosInterval = m_pInfo->GetPositionUpdateInterval();
	pObj->m_ScaleInterval = m_pInfo->GetScaleUpdateInterval();
	pObj->m_TexSeqInterval = 0.0f;
	pObj->m_Color = 0xFFFFFFFF;

	{
		i3Vector::Set( &pObj->m_UV1, 0.0f, 0.0f);
		i3Vector::Set( &pObj->m_UV2, 1.0f, 1.0f);
	}


	MassRange = m_pInfo->GetMassMax() - m_pInfo->GetMassMin();
	pObj->m_Mass = m_pInfo->GetMassMin() + i3Math::Mul(i3Math::Randf(), MassRange);
	i3Vector::Zero( & pObj->m_Gravity);

	pObj->m_PosRandomInterval = m_pInfo->GetPositionRandomInterval();

	switch( m_pInfo->GetEmitType())
	{
		case i3ParticleInfo::EMIT_CONE :
			//  기본 EmitDirection에서 Random하게 약간씩 돌린다.
			#if !defined( I3_PSP)
				rnd1 = i3Math::Mul(i3Math::Randf2(), m_pInfo->GetEmitAngle());
				rnd2 = i3Math::Mul(i3Math::Randf2(), m_pInfo->GetEmitAngle());
			#else
				// psp-gcc의 Optimization Bug를 피하기 위해, 직접 Inline Assembly어로 대체
				__asm__ (
						".set			push\n"
						".set			noreorder\n"
						"vfim.s			s000, 3.0\n"		// s002 = 0.998047
						"vrndf2.s		s001\n"
						"vsub.s			s001, s001, s000\n"
						"mfv			$8,  s001\n"			// t0   = s000
						"mtc1			$8,  %0\n"				// d    = t0
						"vnop\n"
						"vrndf2.s		s002\n"
						"vsub.s			s002, s002, s000\n"
						"mfv			$8,  s002\n"			// t0   = s000
						"mtc1			$8,  %1\n"				// d    = t0
						".set			pop\n"
						: "=&f"(rnd1), "=&f"(rnd2)
						:
						: "$8"
					);

					rnd1 *= m_pInfo->GetEmitAngle();
					rnd2 *= m_pInfo->GetEmitAngle();
			#endif
 
			i3Matrix::SetRotateAxis( &mtx1, &m_EmitRightVector, I3_DEG2RAD(rnd1));
			i3Matrix::PostRotateAxis( &mtx1, &m_EmitUpVector, I3_DEG2RAD(rnd2));

			i3Vector::TransformNormal( &pObj->m_EmitDir, m_pInfo->GetEmitDirection(), &mtx1);

			i3Vector::Copy( &pObj->m_StartPos, m_pInfo->GetEmitPosition());
			break;

		case i3ParticleInfo::EMIT_SPHERE :
			// 완전히 Random 방향으로의 Emit Direction을 만들어 낸다.
			i3Vector::Set( &pObj->m_EmitDir, i3Math::Randf2(), i3Math::Randf2(), i3Math::Randf2());
			i3Vector::Normalize( &pObj->m_EmitDir, &pObj->m_EmitDir);

			i3Vector::Copy( &pObj->m_StartPos, m_pInfo->GetEmitPosition());
			break;

		case i3ParticleInfo::EMIT_DISK :
			// CONE과 기본적으로는 같으나,
			// (단, EmitUpVector를 축으로 하여 회전하는 것은 제외) 
			// UpVector를 축으로 하여 Random하게 돌린
			// Direction을 만든다. 

			rnd1 = i3Math::Mul( i3Math::Randf2(), m_pInfo->GetEmitAngle());

			i3Matrix::SetRotateAxis( &mtx1, &m_EmitRightVector, I3_DEG2RAD(rnd1));
			i3Matrix::PostRotateAxis( &mtx1, m_pInfo->GetUpVector(), i3Math::Randf() * 2.0f * I3_PI);			// 360도 Random 회전 

			i3Vector::TransformNormal( &pObj->m_EmitDir, m_pInfo->GetEmitDirection(), &mtx1);

			i3Vector::Copy( &pObj->m_StartPos, m_pInfo->GetEmitPosition());
			break;

		case i3ParticleInfo::EMIT_RECT_PLANE :
			// EmitAngle을 각도가 아닌, 정사각영역의 크기로 반영한다. 
			// EmitDirection은 항상 일정하다.
			i3Vector::Copy( &pObj->m_EmitDir, m_pInfo->GetEmitDirection());

			rnd1 = i3Math::Mul( i3Math::Randf2() * 0.5f, m_pInfo->GetEmitAngle());
			rnd2 = i3Math::Mul( i3Math::Randf2() * 0.5f, m_pInfo->GetEmitAngle());
			i3Vector::Scale( &pObj->m_Pos[0], &m_UpRightVector, rnd1);
			i3Vector::ScaleAdd( &pObj->m_Pos[0], &m_UpAtVector, rnd2);

			i3Vector::Add( &pObj->m_StartPos, &pObj->m_Pos[0], m_pInfo->GetEmitPosition());
			break;

		case i3ParticleInfo::EMIT_CIRCLE_PLANE :
			// EmitAngle을 반지름으로 하는 원형 영역.
			i3Vector::Copy( &pObj->m_EmitDir, m_pInfo->GetEmitDirection());

			rnd1 = i3Math::Mul( i3Math::Randf(), m_pInfo->GetEmitAngle());
			rnd2 = i3Math::Randf() * I3_PI * 2.0f;				// 360도 범위

			i3Vector::Scale( &pObj->m_Pos[0], &m_UpRightVector, rnd1);

			i3Matrix::SetRotateAxis( &mtx1, m_pInfo->GetUpVector(), rnd2);

			i3Vector::TransformCoord( &pObj->m_Pos[0], &pObj->m_Pos[0], &mtx1);

			i3Vector::Add( &pObj->m_StartPos, &pObj->m_Pos[0], m_pInfo->GetEmitPosition());
			break;

		case i3ParticleInfo::EMIT_MESH :
			//Mesh Particle일 경우 처리.
			//얻어온 Mesh 정보를 바탕으로 랜덤하게 Mesh 면에서 포인트를 골라 이펙트를 분출한다.
			i3MeshParticle * pMeshParticle = m_pInfo->GetMeshParticle();
			if(pMeshParticle != nullptr)
			{
				VEC3D pos, normal;

				pMeshParticle->GetMeshPosAndNormal( &pos, &normal);

				i3Vector::Copy(&pObj->m_EmitDir, &normal);
				i3Vector::Copy(&pObj->m_StartPos, &pos);
			}

			break;
	}

	i3Vector::Copy( &pObj->m_Pos[0], &pObj->m_StartPos);

	i3Vector::Zero( &pObj->m_Scale);
	
	pObj->m_PosRandom = 1.0f + i3Math::Mul( i3Math::Randf2() * 0.5f, m_pInfo->GetPositionInitialRandomFactor());
	pObj->m_ScaleRandom = 1.0f + i3Math::Mul( i3Math::Randf2() * 0.5f, m_pInfo->GetScaleInitialRandomFactor());
	pObj->m_RotateRandom = i3Math::Randf2();

	switch( m_pInfo->GetType())
	{
		case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :
			i3Vector::Copy( &pObj->m_Pos[1], &pObj->m_Pos[0]);
			break;

		case i3ParticleInfo::PARTICLE_SCENE :
			{
				#if defined( I3_FIXED_POINT)
					VEC3D temp;

					i3Vector::GetRandVector( &temp);
					i3Vector::Copy( &pObj->m_RotateDir, &temp);
				#else
					i3Vector::GetRandVector( &pObj->m_RotateDir);
				#endif

					m_pScene->ActivateNode();
			}
			break;
	}

	{
		INT32 i;

		for( i = 0; i < m_PosArgCount; i++)
		{
			pObj->m_PosArg[i] = m_pInfo->GetPositionInit( i);
		}

		for( i = 0; i < m_ScaleArgCount; i++)
		{
			pObj->m_ScaleArg[i] = m_pInfo->GetScaleInit( i);
		}

		pObj->m_RotateAccm = m_pInfo->GetRotationInit() + i3Math::Mul( i3Math::Randf2(), m_pInfo->GetRotateRandomFactor());
	}
}

namespace
{
/*
	void CalcCamAtPlane( i3SceneTracer* pTracer, VEC4D&	planeCamAt)
	{
		if (pTracer == nullptr)
		{
			planeCamAt.x = 0.f;		planeCamAt.y = 0.f;		planeCamAt.z = 0.f;		planeCamAt.w = -1.f;
			return;
		}
		MATRIX* camMat = pTracer->GetInverseViewMatrix();
		VEC3D* vCamAt =	i3Matrix::GetAt(camMat);
		VEC3D* vCamPos = i3Matrix::GetPos(camMat);

		// 카메라 평면 완성..
		planeCamAt.x = -vCamAt->x;	planeCamAt.y = -vCamAt->y;	planeCamAt.z = -vCamAt->z;
		planeCamAt.w = VEC3D::Dot(*vCamAt, *vCamPos);		// 카메라 At벡터가 전향의 반대방향인듯 하여 모두 음수처리..
	}
*/

	void UpdateParticleObjectDepth( i3ParticleInfo::PARTICLE_TYPE ParticleType, 
		const MATRIX& matWorldView, I3PARTICLEOBJECT* pObj)
	{
		VEC3D   pos_ViewCenter;

		switch(ParticleType)
		{
		case i3ParticleInfo::PARTICLE_POINTSPRITE:
		case i3ParticleInfo::PARTICLE_SPRITE:
			pos_ViewCenter = pObj->m_Pos[0].TransformCoord(matWorldView);
			break;

		case i3ParticleInfo::PARTICLE_BILLBOARD:
			pos_ViewCenter = pObj->m_Pos[0].TransformCoord(matWorldView);
		
			if (pos_ViewCenter.z > 0.f)		// 양수라면.. 1인덱스를 써 본다..
			{	
				pos_ViewCenter = pObj->m_Pos[1].TransformCoord(matWorldView);
			}
			break;

		case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :
			pos_ViewCenter = pObj->m_Pos[1].TransformCoord(matWorldView);		// 앵커의 경우 변경되지 않는 원점이 인덱스 1이다..
		
			if (pos_ViewCenter.z > 0.f)		// 양수라면.. 0인덱스를 써 본다..
			{	
				pos_ViewCenter = pObj->m_Pos[0].TransformCoord(matWorldView);
			}
			break;

		case i3ParticleInfo::PARTICLE_DECAL:
			pos_ViewCenter = pObj->m_Pos[0].TransformCoord(matWorldView);	
			break;

		case i3ParticleInfo::PARTICLE_SCENE:	// 씬은 일단 소팅에서 제외한다.. (오브젝트와의 소팅 구현이 된다 해도 
			pos_ViewCenter.x = 0.f;				// 여기서 처리하지 않는다.) 막연하게 1.f로 일단 셋팅..(사용되어서는 안된다.)					
			pos_ViewCenter.y = 0.f;
			pos_ViewCenter.z = 1.f;
			break;
		}

		pObj->m_PosFromView = pos_ViewCenter;
	}
}


#define		UPDATE_FLAG_POS		0x00000001
#define		UPDATE_FLAG_SCL		0x00000002
#define		UPDATE_FLAG_ROT		0x00000004
#define		UPDATE_FLAG_CLR		0x00000008
#define		UPDATE_FLAG_UV		0x00000010

void i3Particle::Drive( i3SceneTracer * pTracer)
{
	if ( g_pRenderContext->IsRenderFlag(I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) )		// 이 경우 진행하지 않아본다..
		return;

	INT32 i, EmitCount, AliveCount;//, idx;
	REAL32 tm, s;
	REAL32 _t;
	VEC3D dir;
	I3PARTICLEOBJECT * pObj;
	i3KeyframeTable * pTable;
	INT32 accm;
	bool bEnable;
	bool bEmit = false;
	bool bInterpolate;
	MATRIX mtx;
	VEC3D GravDir;
	bool bVolatile = m_pInfo->IsVolatile();
	UINT8 r, g, b, a;
	UINT32 update = 0;
	bool bNewFrame = true;

	if( pTracer != nullptr)
	{
		tm = pTracer->GetTime();		// 시간차

		bNewFrame = (m_lastFrameID != pTracer->getFrameID());
		m_lastFrameID = pTracer->getFrameID();
	}
	else
	{
		tm = 0.016666f;
		m_lastFrameID = 0;
	}

	if( bNewFrame)
	{
		// 이미 해당 Frame에 대응한 Update를 한 상태라면 처리하지 않도록 처리..
		if((m_timeLocal == 0.0f) && !isParticleStyle( I3_PARTICLE_STYLE_START))
		{
			Start();
		}

		if( m_pInfo->GetLifeTime() > 0.0f)
		{
			// Lifetime이 0이면 무한(Infinite)의 의미로 해석한다.
			m_timeLocal += tm;

			if(m_timeLocal > m_pInfo->GetLifeTime())
			{
				// Lifetime을 Over했다면, 정지시키기 시작한다.
				// 모든 Object가 완전히 사라질 때까지 실제로 정지하지 않는다.
				Stop();
			}
		}

		m_EmitInterval -= tm;
		if( !isParticleStyle( I3_PARTICLE_STYLE_STOP) && (m_EmitInterval <= 0.0f))
		{
			// 정지중이 아니고 Emit Interval이 경과했다면
			// 새로운 Object르 Emit한다.
			bEmit = true;
			m_EmitInterval = m_pInfo->GetEmitInterval();
		}
	}

	EmitCount = 0;
	AliveCount = 0;

	i3ParticleTransformMatrix* matParticleTrans = nullptr;
	MATRIX		matWorldView;

	if( pTracer != nullptr)
	{
		i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

		matParticleTrans = g_pParticleRender->AllocParticleTransformMatrix(pStack->GetTop(), pTracer->GetViewMatrix());		// 해제는 파티클렌더러에서 매번 일괄적으로 적용..
		
		// 3x3의 역행렬 구하기
		if( pTracer->IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
			i3Matrix::Mul( &mtx, pStack->GetTop(), pTracer->GetInverseViewMatrix());
			i3Matrix::Transpose3x3( &mtx, &mtx);
			matWorldView = *pStack->GetTop();
		}
		else
		{
			i3Matrix::Transpose3x3( &mtx, pStack->GetTop());
			i3Matrix::Mul(&matWorldView, pStack->GetTop(), pTracer->GetViewMatrix());
		}

		// World Transform과 무관하게 중력이 일정한 방향으로
		// 적용될 수 있도록 Inverse Model-View Matrix를 적용한다.
		i3Vector::TransformNormal( &GravDir, (VEC3D *) g_GravityDir, &mtx);
	}
	else
	{
		i3Vector::Set( &GravDir, 0.0f, -1.0f, 0.0f);
	}
	
	// 더이상 AtPlane을 사용치 않고, 대신에 WorldViewMat을 미리 구해서...View좌표계 매트릭스로 삼도록 한다.
	//VEC4D	planeCamAt;
	//CalcCamAtPlane(pTracer, planeCamAt);

	//__SP( 10);

	for( i = 0, accm = 0; i < m_AllocatedCount; i++)
	{
		pObj = m_ppObj[i];

		if( pObj == nullptr)
			continue;

		pObj->m_pTransMtx = matParticleTrans;		

		bEnable = true;
		update = r = g = b = a = 0;
		_t = 0.0f;

		if(!( pObj->m_State & I3_PARTICLE_STATE_ENABLE))
		{	// Object가 비활성화 상태라면...
			if(( bNewFrame) && (bEmit == true) && (EmitCount < m_pInfo->GetEmitCount()))
			{	// 새롭게 분출시킨다.
				if( !((bVolatile == true) && (pObj->m_State & I3_PARTICLE_STATE_FIRED)))
				{
					EmitCount++;
					_Emit( pObj);
				}
				else
				{	// IsVolatile이 true이고, 이미 한번 분출된 Object는 다시 분출시키지 않는다.
					bEnable = false;
				}
			}
			else
			{
				bEnable = false;
			}
		}
		else
		{
			// 활성화 상태라면...
			if( bNewFrame)
			{
				pObj->m_Time += tm;
				_t = i3Math::Mul( pObj->m_Time, m_RObjectLifeTime);
				if( _t >= 1.0f)
				{
					// Object의 수명이 만기되었다.
					if((bEmit == true) && (m_pInfo->IsVolatile() == false) && (EmitCount < m_pInfo->GetEmitCount()))
					{
						// 완료된 Particle Object를 재개시킨다.
						EmitCount++;
						_Emit( pObj);
						_t = 0.0f;
					}
					else
					{	// 비활성화 상태로 만든다.
						pObj->m_State &= ~I3_PARTICLE_STATE_ENABLE;
						bEnable = false;
					}
				}
			}
		}

		if( bEnable == false)
		{
			switch( m_pInfo->GetType())
			{
				case i3ParticleInfo::PARTICLE_SCENE :				
					{
						i3AttrSet * pAttrSet;

						pAttrSet = (i3AttrSet *) m_pScene->GetChild( i);
						pAttrSet->DeactivateNode();
					}
					break;

				case i3ParticleInfo::PARTICLE_DECAL:
					break;
			}
			
			// 안 그려질 것으로 보인다..
			pObj->m_PosFromView.x = 0.f;
			pObj->m_PosFromView.y = 0.f;
			pObj->m_PosFromView.z = 1.f;			// 렌더링 범위에 들어오더라도 안그리기 위한 값..
		}
		else
		{
			if( bNewFrame)
			{
				register REAL32 _interval;
				bool bApplyTime = true;

				//__SP( 11);
				bInterpolate = true;
				AliveCount ++;

				////////////////////////////////////////////////////////////////////
				// Position
				pObj->m_PosInterval += tm;
				_interval = pObj->m_PosInterval;

				if( _interval >= m_pInfo->GetPositionUpdateInterval())
				{
					register INT32 last = m_PosArgCount - 1;

					if( m_pInfo->GetPositionUpdateInterval() < tm)
					{
						_interval = pObj->m_PosInterval = tm;
						bInterpolate = false;
					}

					if(last != -1)
					{
						// 가장 마지막 Arg의 값을 결정한다.
						{
							// 속도일 수도, 가속도일 수도 있다.
							switch( m_pInfo->GetPositionFunc( last))
							{
							case i3ParticleInfo::FUNC_CONSTANT :	pObj->m_PosArg[last] = m_pInfo->GetPositionInit( last);	break;
							case i3ParticleInfo::FUNC_SIN :			pObj->m_PosArg[last] = i3Math::Mul( i3Math::vsin( _t), m_pInfo->GetPositionFactor( last));	break;
							case i3ParticleInfo::FUNC_COS :			pObj->m_PosArg[last] = i3Math::Mul( i3Math::vcos( _t), m_pInfo->GetPositionFactor( last));	break;
							case i3ParticleInfo::FUNC_RND :			pObj->m_PosArg[last] = i3Math::Randf2();	break;
							case i3ParticleInfo::FUNC_TABLE :
								pTable = m_pInfo->GetPositionTable( last);
								pObj->m_PosArg[last] = pTable->GetKeyframe( _t, m_PosInterp);	

								bApplyTime = false;
								break;
							default :
								//I3TRACE( "i3Particle::Drive() - Invalid position function : %x\n", m_pInfo->GetPositionFunc(last));
								break;
							}

							pObj->m_PosArg[last] *=  pObj->m_PosRandom;
						}

						if( last == 1)
						{	// 가속도이므로...  v = v0 + at
							pObj->m_PosArg[0] += i3Math::Mul( pObj->m_PosArg[1], _interval);
						}

						if( m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP)
						{
							pObj->m_PosArg[0] = i3Math::Clamp( pObj->m_PosArg[0], m_pInfo->GetPositionMin(), m_pInfo->GetPositionMax());
						}
					}
					else
					{
						pObj->m_PosArg[0] = 0.0f;
					}

					if((pObj->m_PosArg[0] != 0.0f) || ( m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_GRAVITY))
					{
						// 이동 거리.... s = vt
						s = i3Math::Mul( pObj->m_PosArg[0], _interval);

						// 최종적으로 Direction으로의 이동량을 계산한다.
						i3Vector::Scale( &dir, & pObj->m_EmitDir, s);

						// 중력가속도의 적용
						if( m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_GRAVITY)
						{
							REAL32 _fact;
							
							_fact = i3Math::Mul( g_GravityFactor, i3Math::Mul( pObj->m_Mass, i3Math::Mul( _interval, _interval)));
							i3Vector::ScaleAdd( &pObj->m_Gravity, (VEC3D *) &GravDir, _fact);
							i3Vector::Add( &dir, &dir, &pObj->m_Gravity);
						}

						i3Vector::Add( &pObj->m_TargetPos, &pObj->m_Pos[0], &dir);
						i3Vector::Copy( &pObj->m_StartPos, &pObj->m_Pos[0]);

						update |= UPDATE_FLAG_POS;
					}
					else
					{
						i3Vector::Copy( &pObj->m_TargetPos, &pObj->m_Pos[0]);
					}

					pObj->m_PosInterval -= m_pInfo->GetPositionUpdateInterval();
				}
				//__EP( 11);

				//__SP( 12);
				if( m_pInfo->GetPositionStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM)
				{
					// 매 Frame마다 Random을 적용한 경우....
					pObj->m_PosRandomInterval -= tm;

					if( pObj->m_PosRandomInterval <= 0.0f)
					{
						VEC3D rndvec;

						#if defined( I3_FIXED_POINT)
							VEC3D tmp;

							i3Vector::GetRandVector( &tmp);
							i3Vector::Copy( &rndvec, &tmp);
						#else
							i3Vector::GetRandVector( &rndvec);
						#endif

						// XGKICK
						i3Vector::Mul( &rndvec, &rndvec, m_pInfo->GetPositionRandomFactor());
						i3Vector::Add( &pObj->m_TargetPos, &pObj->m_TargetPos, &rndvec);
						//i3Vector::MulAdd( &pObj->m_TargetPos, &rndvec, m_pInfo->GetPositionRandomFactor());

						pObj->m_PosRandomInterval = m_pInfo->GetPositionRandomInterval();

						update |= UPDATE_FLAG_POS;
					}
				}

				if( m_pInfo->GetType() & I3_PARTICLE_TYPE_MASK_TRACE_POS)
				{
					// 이전 위치를 Tracking하기 위해 이동시킨다.
					i3Vector::Copy( &pObj->m_Pos[1], &pObj->m_Pos[0]);
				}

				// Position의 Interpolate
				if( bInterpolate)
				{
					i3Vector::Lerp( &pObj->m_Pos[0], &pObj->m_StartPos, &pObj->m_TargetPos, i3Math::Mul( pObj->m_PosInterval, m_PosRInterval));
					update |= UPDATE_FLAG_POS;
				}
				else
					i3Vector::Copy( &pObj->m_Pos[0], &pObj->m_TargetPos);

				//__EP( 12);

				////////////////////////////////////////////////////////////////////
				// Scale
				//__SP( 13);
				bInterpolate = true;

				pObj->m_ScaleInterval += tm;
				_interval = pObj->m_ScaleInterval;

				if( _interval >= m_pInfo->GetScaleUpdateInterval())
				{
					register int last = m_ScaleArgCount - 1;
					REAL32 oldArg = pObj->m_ScaleArg[0];

					if( m_pInfo->GetScaleUpdateInterval() < tm)
					{
						_interval = pObj->m_ScaleInterval = tm;
						bInterpolate = false;
					}

					if(last != -1)
					{
						// 가장 마지막 Arg의 값을 결정한다.
						switch( m_pInfo->GetScaleFunc( last))
						{
						case i3ParticleInfo::FUNC_CONSTANT :	pObj->m_ScaleArg[last] = m_pInfo->GetScaleInit( last);	break;

						case i3ParticleInfo::FUNC_SIN :		// 0~90도의 sin()값을 0~1로 표현한다.
							pObj->m_ScaleArg[last] = i3Math::vsin( _t) * m_pInfo->GetScaleFactor( last);
							break;	

						case i3ParticleInfo::FUNC_COS :
							pObj->m_ScaleArg[last] = i3Math::vcos( _t) * m_pInfo->GetScaleFactor( last);
							break;

						case i3ParticleInfo::FUNC_RND :			pObj->m_ScaleArg[last] = i3Math::Randf2();	break;

						case i3ParticleInfo::FUNC_TABLE :
							pTable = m_pInfo->GetScaleTable( last);
							pObj->m_ScaleArg[last] = pTable->GetKeyframe( _t, m_ScaleInterp);
							break;
						}

						// Random의 적용 (선형적인 변화만을 지원한다.)
						pObj->m_ScaleArg[last] = i3Math::Mul( pObj->m_ScaleArg[last], pObj->m_ScaleRandom);

						// 가장 마지막 변화량을 하위 Arg에 적용한다.
						if( last == 1)
							pObj->m_ScaleArg[0] += i3Math::Mul( pObj->m_ScaleArg[1], _interval);

						if( m_pInfo->GetScaleStyle() & I3_PARTICLE_PARAM_STYLE_CLAMP)
							pObj->m_ScaleArg[0] = i3Math::Clamp( pObj->m_ScaleArg[0], m_pInfo->GetScaleMin(), m_pInfo->GetScaleMax());

					}
					else
					{
						pObj->m_ScaleArg[0] = 0.0f;
					}

					i3Vector::Scale( &pObj->m_TargetScale, m_pInfo->GetScaleMask(), pObj->m_ScaleArg[0]);
					i3Vector::Copy( &pObj->m_StartScale, &pObj->m_Scale);		// 이전 Scale 값을 보관하여 둔다.
				
					if( oldArg != pObj->m_ScaleArg[0])
					{
						update |= UPDATE_FLAG_SCL;
					}
				
					pObj->m_ScaleInterval -= m_pInfo->GetScaleUpdateInterval();
				}

				if( bInterpolate)
				{
					i3Vector::Lerp( &pObj->m_Scale, &pObj->m_StartScale, &pObj->m_TargetScale, i3Math::Mul( pObj->m_ScaleInterval, m_ScaleRInterval));
					update |= UPDATE_FLAG_SCL;
				}
				else
					i3Vector::Copy( &pObj->m_Scale, &pObj->m_TargetScale);

				//__EP( 13);

				////////////////////////////////////////////////////////////////////
				// Rotation
				//__SP( 14);
				if( isParticleStyle( I3_PARTICLE_STYLE_ROTATE_ENABLE))
				{
					REAL32 RotateArg = 0;

					switch( m_pInfo->GetRotationFunc())
					{
						case i3ParticleInfo::FUNC_NONE :		RotateArg = 0.0f;	break;
						case i3ParticleInfo::FUNC_CONSTANT :	RotateArg = i3Math::Mul( m_pInfo->GetRotationInit(), tm);	break;
						case i3ParticleInfo::FUNC_SIN :			RotateArg = i3Math::Mul( i3Math::vsin( _t), m_pInfo->GetRotationFactor());	break;
						case i3ParticleInfo::FUNC_COS :			RotateArg = i3Math::Mul( i3Math::vcos( _t), m_pInfo->GetRotationFactor());	break;
						case i3ParticleInfo::FUNC_RND :			RotateArg = i3Math::Randf2();	break;
						case i3ParticleInfo::FUNC_TABLE :		
							pTable = m_pInfo->GetRotationTable();
							RotateArg = pTable->GetKeyframe( _t, i3KeyframeTable::LERP);
							break;
					}

					if( RotateArg != 0.0f)
					{
						pObj->m_RotateAccm += i3Math::Mul( RotateArg, pObj->m_RotateRandom);
						update |= UPDATE_FLAG_ROT;
					}
				}

				// Color
				{
					UINT32 oldCol = pObj->m_Color;

					if( !(m_pInfo->GetColorStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM) ||
						(pObj->m_State & I3_PARTICLE_STATE_FIRSTTIME))
					{
						switch( m_pInfo->GetRGBFunc())
						{
							case i3ParticleInfo::FUNC_CONSTANT :	pObj->m_Color = m_pInfo->GetRGBInit();	break;
							case i3ParticleInfo::FUNC_TABLE :
								{
									REAL32 rt;
									UINT8 temp;

									if( m_pInfo->GetColorStyle() & I3_PARTICLE_PARAM_STYLE_RANDOM)
										rt = i3Math::Randf();
									else
										rt = _t;

									temp = (UINT8)m_pInfo->GetRTable()->GetKeyframe( rt, m_ColorInterp);
									pObj->m_Color = temp;

									temp = (UINT8)m_pInfo->GetGTable()->GetKeyframe( rt, m_ColorInterp);
									pObj->m_Color |= (temp << 8);

									temp = (UINT8)m_pInfo->GetBTable()->GetKeyframe( rt, m_ColorInterp);
									pObj->m_Color |= (temp << 16);
								}
								break;

							default :	pObj->m_Color = 0x00FFFFFF;	break;
						}
					}

					pObj->m_Color &= 0x00FFFFFF;

					switch( m_pInfo->GetAlphaFunc())
					{
						case i3ParticleInfo::FUNC_CONSTANT :	pObj->m_Color |= (UINT32) (m_pInfo->GetAlphaInit() << 24);	break;

						case i3ParticleInfo::FUNC_SIN :	// 0~90도의 sin()값을 0~1로 표현한다.	
							pObj->m_Color |= ((UINT32) i3Math::Mul( i3Math::vsin( _t), 255.0f) << 24);
							break;

						case i3ParticleInfo::FUNC_COS :	
							pObj->m_Color |= ((UINT32) i3Math::Mul(i3Math::vcos( _t), 255.0f) << 24);
							break;

						case i3ParticleInfo::FUNC_TABLE :
							pObj->m_Color |= ((UINT32)m_pInfo->GetAlphaTable()->GetKeyframe( _t, m_ColorInterp) << 24);
							break;

						default :	pObj->m_Color |= 0xFFFFFFFF;			break;
					}

					if( oldCol != pObj->m_Color)
					{
						update |= UPDATE_FLAG_CLR;
					}
				}

				//__EP( 14);

				////////////////////////////////////////////////////////////////////////////////
				// Texture Sequence
				//__SP( 15);
				if( m_pTexSeq != nullptr)
				{
					pObj->m_TexSeqInterval -= tm;

					// Texture Sequence를 계산한다.
					if(	(pObj->m_TexSeqInterval <= 0.0f) && 
						((m_pTexSeq->IsFixedFrameMode() == false) || (pObj->m_State & I3_PARTICLE_STATE_FIRSTTIME)))
					{
						pObj->m_TexSeqInterval = m_pTexSeq->GetStaticInterval();

						m_pTexSeq->GetUV( pObj->m_Time, &pObj->m_UV1, &pObj->m_UV2);
					}
				}
				//__EP( 15);

			
			}
			
			//  소팅 거리를 구해야한다.. ( bNewFrame이 아닌 경우엔 pObj내에서 ZDepth 값보존은 있어야한다)
			UpdateParticleObjectDepth(m_pInfo->GetType(), matWorldView, pObj);
			//

			//if( update != 0)
			switch( m_pInfo->GetType())
			{
				case i3ParticleInfo::PARTICLE_POINTSPRITE :
				case i3ParticleInfo::PARTICLE_SPRITE :
				case i3ParticleInfo::PARTICLE_BILLBOARD :
				case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :
					g_pParticleRender->AddParticleObject(pObj);
				break;
				/*
				case i3ParticleInfo::PARTICLE_POINTSPRITE :
					I3ASSERT( m_pGroup != nullptr);

					idx = m_pGroup->AddForQuad( & pObj->m_Pos[0], &pObj->m_Scale);

					if( idx != -1)
					{
						m_pGroup->SetColor( idx, pObj->m_Color);

						if( isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							m_pGroup->SetTextureCoord( idx, &pObj->m_UV1, &pObj->m_UV2);
					}
					break;
*/
/*
				case i3ParticleInfo::PARTICLE_SPRITE :
					I3ASSERT( m_pGroup != nullptr);
					idx = m_pGroup->AddForSprite( & pObj->m_Pos[0], &pObj->m_Scale, pObj->m_RotateAccm, isParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE));

					if( idx != -1)
					{
						m_pGroup->SetColor( idx, pObj->m_Color);

						if( isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							m_pGroup->SetTextureCoord( idx, &pObj->m_UV1, &pObj->m_UV2);
					}
					break;
*/

/*
				case i3ParticleInfo::PARTICLE_BILLBOARD :
					{
						I3ASSERT( m_pGroup != nullptr);

						VEC3D diff;

						i3Vector::Sub( &diff, &pObj->m_Pos[0], &pObj->m_Pos[1]);
						i3Vector::Scale( &diff, &diff, m_pInfo->GetScaleBillboardFactor());
						i3Vector::Add( &diff, &diff, &pObj->m_Pos[0]);

						idx = m_pGroup->AddForBillboard( &pObj->m_Pos[0], &diff, &pObj->m_Scale);

						if( idx != -1)
						{
							
							m_pGroup->SetColor( idx, pObj->m_Color);

							if( isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
								m_pGroup->SetTextureCoordForBillboard( idx, &pObj->m_UV1, &pObj->m_UV2);
						}
					}
					break;
*/
/*
				case i3ParticleInfo::PARTICLE_ANCHOR_BILLBOARD :
					I3ASSERT( m_pGroup != nullptr);

					idx = m_pGroup->AddForBillboard( & pObj->m_Pos[1], &pObj->m_Pos[0], &pObj->m_Scale);

					if( idx != -1)
					{
						m_pGroup->SetColor( idx, pObj->m_Color);

						if( isParticleStyle( I3_PARTICLE_STYLE_TEX_ENABLE))
							m_pGroup->SetTextureCoordForBillboard( idx, &pObj->m_UV1, &pObj->m_UV2);
					}
					break;
*/

			case i3ParticleInfo::PARTICLE_DECAL :
				//__SP( 20);
				if( (pTracer != nullptr) && bNewFrame && ((pObj->m_State & I3_PARTICLE_STATE_FIRE_DECAL) == 0))
				{
					m_EmitWaitCount--;
					// 여기서 HitBoxShape을 세팅하고 컬리젼된 TriMesh를 체크하도록 합니다.
					if( m_pDrawDecalInfo == nullptr && m_EmitWaitCount < 0)
					{
						// EmitDecal 내부를 보면 pObj->Pos[0]를 로컬 중심점으로 쓰는 것을 알수 있다.

						I3SG_DECAL_HITBOX * pHitBox = g_pParticleRender->EmitDecal( pTracer, this, pObj);

						if( pHitBox != nullptr)
						{
							m_pDecal = ((I3SG_DECAL_HITBOX*) pHitBox)->m_pDecal;
							m_pDrawDecalInfo = ((I3SG_DECAL_HITBOX*) pHitBox)->m_pDrawDecalInfo;
							m_EmitWaitCount = 0;
						}
						else
						{
							I3TRACE( "[[[[[[[[[ no more decal hit box. ]]]]]]]]\n" );
							// 준비된 Decal Hit Box가 모자라는 경우
							//m_pDecal->setDecalState( I3SG_DECAL_STATE_NOHIT);

							m_EmitWaitCount = 5;	// 5frame을 기다려본다.
						}	
					}

					//__EP( 31);
				}
				else
				{
					I3ASSERT( m_pGroup != nullptr);
					I3ASSERT( i3::same_of<i3ParticleGroupForTriangles*>(m_pGroup));

					//__SP( 32);

					if( m_pDecal != nullptr)
					{	// Hit용 i3Decal을 반환합니다.
						g_pParticleRender->FreeDecal( m_pDecal);
						m_pDecal = nullptr;
					}

					if( m_pDrawDecalInfo != nullptr)
					{
						if (m_pDrawDecalInfo->isEnable() == false)
						{
							// 일반적인 Sprite으로 간주한다.
							_FreeDecal();
						}
						else
						{
							g_pParticleRender->AddParticleObject(pObj);
						}
					}

					//__EP( 32);
				}
				//__EP( 20);
				break;

/*
				case i3ParticleInfo::PARTICLE_DECAL :
					//__SP( 20);
					if( (pTracer != nullptr) && bNewFrame && ((pObj->m_State & I3_PARTICLE_STATE_FIRE_DECAL) == 0))
					{
						m_EmitWaitCount--;
						// 여기서 HitBoxShape을 세팅하고 컬리젼된 TriMesh를 체크하도록 합니다.
						if( m_pDrawDecalInfo == nullptr && m_EmitWaitCount < 0)
						{
							// EmitDecal 내부를 보면 pObj->Pos[0]를 로컬 중심점으로 쓰는 것을 알수 있다.

							I3SG_DECAL_HITBOX * pHitBox = g_pParticleRender->EmitDecal( pTracer, this, pObj);

							if( pHitBox != nullptr)
							{
								m_pDecal = ((I3SG_DECAL_HITBOX*) pHitBox)->m_pDecal;
								m_pDrawDecalInfo = ((I3SG_DECAL_HITBOX*) pHitBox)->m_pDrawDecalInfo;
								m_EmitWaitCount = 0;
							}
							else
							{
								I3TRACE( "[[[[[[[[[ no more decal hit box. ]]]]]]]]\n" );
								// 준비된 Decal Hit Box가 모자라는 경우
								//m_pDecal->setDecalState( I3SG_DECAL_STATE_NOHIT);

								m_EmitWaitCount = 5;	// 5frame을 기다려본다.
							}	
						}

						//__EP( 31);
					}
					else
					{
						I3ASSERT( m_pGroup != nullptr);
						I3ASSERT( i3::same_of<i3ParticleGroupForTriangles*>(m_pGroup));

						//__SP( 32);

						if( m_pDecal != nullptr)
						{	// Hit용 i3Decal을 반환합니다.
							g_pParticleRender->FreeDecal( m_pDecal);
							m_pDecal = nullptr;
						}

						if( m_pDrawDecalInfo != nullptr)
						{
							if( m_pDrawDecalInfo->isEnable())
							{
								((i3ParticleGroupForTriangles *) m_pGroup)->AddForDecal( m_pDrawDecalInfo, pObj->m_Color);

								// 이 부분은 매 Frame마다 실행되어야 하며, Decal 역시 Free되지 않고
								// 보존되고 있어야 한다.
							}
							else
							{
								// 일반적인 Sprite으로 간주한다.
								_FreeDecal();
							}
						}

						//__EP( 32);
					}
					//__EP( 20);
					break;
*/
				case i3ParticleInfo::PARTICLE_SCENE :			// 씬은 ParticleGroup을 쓰지 않고, 노드에 붙은 Geometry를 직접 그리게 된다.
					//__SP( 21);								// 오브젝트간 소팅처리될때까지는 알파소팅에서 제외처리..
					{
						i3Transform * pTrans;
						i3AttrSet * pAttrSet;
						MATRIX * pMat;

						pAttrSet = (i3AttrSet *) m_pScene->GetChild( i);
						pAttrSet->ActivateNode();
						pTrans = (i3Transform *) pAttrSet->GetChild(0);
						pMat = pTrans->GetMatrix();

						// Rotation
 						i3Matrix::SetRotateAxis( pMat, &pObj->m_RotateDir, pObj->m_RotateAccm);

						// Scale
						i3Matrix::PostScale( pMat, &pObj->m_Scale);

						// Position
						i3Matrix::SetPos( pMat, &pObj->m_Pos[0]);

						{
							COLORREAL rcol;
							COLOR col;

							i3Color::ConvertARGB32( pObj->m_Color, &col);
							i3Color::Set( &rcol, &col);

							{
								i3ColorAttr * pAttr = (i3ColorAttr *) pAttrSet->GetAttr( 0);								
								pAttr->Set( &rcol);
							}

							{
								i3MaterialAttr * pAttr = (i3MaterialAttr *) pAttrSet->GetAttr( 1);

								pAttr->SetDiffuse( &rcol);
							}
						}
					}
					//__EP( 21);
					break;

				#if defined( I3_DEBUG)
				default :
					I3TRACE( "i3Particle::Drive() - Invalid particle type : %x\n", m_pInfo->GetType());
				#endif
			}
		}

		if( bNewFrame)
		{
			pObj->m_State &= ~I3_PARTICLE_STATE_FIRSTTIME;
		}
	}

	//__EP( 10);

	if( isParticleStyle(I3_PARTICLE_STYLE_STOP) && (AliveCount == 0))
	{
		// 정지 명령이 내려졌고, Particle Object도 살아있는 것이 없다면
		// 종료!
		m_pScene->DeactivateNode();

		_FreeDecal();
	}
}

void i3Particle::OnChangeTime( REAL32 tm)
{
	if( tm == 0.0f)
	{
		Stop();
		Start();
	}

	m_timeLocal = tm;

	i3Node::OnChangeTime( tm);
}

void	i3Particle::OnDeactivate(void)
{
	Stop();
}

void i3Particle::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pInfo != nullptr)
	{
		m_pInfo->OnBuildObjectList( List);
	}

	// Texture
	if( getTexture() != nullptr)
	{
		i3Texture * pTex = getTexture();
		pTex->OnBuildObjectList( List);
	}

	// Normal Map
	if( getNormalMap() != nullptr)
	{
		i3Texture * pTex = getNormalMap();
		pTex->OnBuildObjectList( List);
	}

	// Texture Sequence
	if( m_pTexSeq != nullptr)
	{
		m_pTexSeq->OnBuildObjectList( List);
	}

	// Mesh
	if( m_pMeshInfo != nullptr)
	{
		m_pMeshInfo->OnBuildObjectList( List);
	}

	//
	if( m_pBoundBox != nullptr)
	{
		m_pBoundBox->OnBuildObjectList( List);
	}

	// i3Node::OnBuildObjectList( pList);

	// Particle의 경우, 모든 Child는 Runtime 시에 생성되는 것이기 때문에
	// 저장할 필요가 없다.
	List.push_back( this);
}

void i3Particle::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	if( !i3::same_of< i3Particle*>(pDest))
		return;

	i3Particle * pParticle = (i3Particle *) pDest;

	pParticle->setParticleGroup(  m_pGroup);

	pParticle->SetMesh( m_pMeshInfo);

	pParticle->setParticleStyle( getParticleStyle());
	
	pParticle->setTexture( getTexture());

	pParticle->setSrcBlend( getSrcBlend());
	pParticle->setDestBlend( getDestBlend());
	pParticle->setBlendOperation( getBlendOperation());

	pParticle->setNormalMap( getNormalMap());

	pParticle->setZBias( getZBias());
	pParticle->setZBiasSlope( getZBiasSlope());

	I3_REF_CHANGE( pParticle->m_pBoundBox, m_pBoundBox);

	// 메모리 사용율을 낮추기 위해, 무조건 공유하는 방식으로 전환
	pParticle->SetParticleInfo( m_pOrigInfo);

	pParticle->SetTextureSequence( m_pTexSeq);

	pParticle->Create();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 PARTICLE_10
	{
		OBJREF		m_pParticleInfo = 0;
		OBJREF		m_pTexture = 0;
		OBJREF		m_pTexSeq = 0;
		OBJREF		m_pMeshInfo = 0;
		UINT8		m_bBlend = 0;
		UINT8		m_SrcBlend = 0;
		UINT8		m_DestBlend = 0;
		UINT8		m_BlendOP = 0;
		UINT8		m_Reserved[16] = { 0 };
	};

	struct ALIGN4 PARTICLE_20
	{
		OBJREF		m_pParticleInfo = 0;
		OBJREF		m_pTexture = 0;
		OBJREF		m_pTexSeq = 0;
		OBJREF		m_pMeshInfo = 0;
		UINT8		m_bBlend = 0;
		UINT8		m_SrcBlend = 0;
		UINT8		m_DestBlend = 0;
		UINT8		m_BlendOP = 0;

		OBJREF		m_pNormalMap = 0;
		UINT8		m_bFaceToCamera = 0;
		UINT8		m_bLighting = 0;
		REAL32		m_fZBias = 0.0f;
		REAL32		m_fZBiasSlope = 0.0f;
		UINT8		m_Reserved[2] = { 0 };
	};

	struct ALIGN4 PARTICLE_30
	{
		OBJREF		m_pParticleInfo = 0;
		OBJREF		m_pTexture = 0;
		OBJREF		m_pTexSeq = 0;
		OBJREF		m_pMeshInfo = 0;
		UINT8		m_bBlend = 0;
		UINT8		m_SrcBlend = 0;
		UINT8		m_DestBlend = 0;
		UINT8		m_BlendOP = 0;

		OBJREF		m_pNormalMap = 0;
		UINT8		m_bFaceToCamera = 0;
		UINT8		m_bLighting = 0;
		REAL32		m_fZBias = 0.0f;
		REAL32		m_fZBiasSlope = 0.0f;

		OBJREF		m_pBoundBox = 0;
		UINT8		m_bDecal = 0;

		UINT8		m_ZWrite = 0;			// 0:값 세팅이 된적 없었음.		2: false	1: true
		UINT8		m_ZTest = 0;			// 0:값 세팅이 된적 없었음.		2: false	1: true

		UINT8		pad8[1] = { 0 };
		UINT32		pad[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Particle::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// ID
	{
		Rc = pResFile->Write( "PTC3", 4);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	pack::PARTICLE_30 Info;

	Info.m_pParticleInfo	= (OBJREF) pResFile->GetObjectPersistID( m_pInfo);
	Info.m_pTexture			= (OBJREF) pResFile->GetObjectPersistID( getTexture());
	Info.m_pTexSeq			= (OBJREF) pResFile->GetObjectPersistID( m_pTexSeq);
	Info.m_pMeshInfo		= (OBJREF) pResFile->GetObjectPersistID( m_pMeshInfo);

	Info.m_bBlend			= isParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE)?1:0;
	Info.m_SrcBlend			= (UINT8) getSrcBlend();
	Info.m_DestBlend		= (UINT8) getDestBlend();
	Info.m_BlendOP			= (UINT8) getBlendOperation();
	Info.m_bLighting		= isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE)?1:0;
	Info.m_fZBiasSlope		= getZBiasSlope();
	Info.m_fZBias			= getZBias();

	Info.m_ZWrite			= isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE)?1:2;
	Info.m_ZTest			= isParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE)?1:2;
	Info.m_bDecal			= isParticleStyle( I3_PARTICLE_STYLE_IS_DECAL)?1:0;

	if( isParticleStyle( I3_PARTICLE_STYLE_NMMAP_ENABLE))
		Info.m_pNormalMap	= (OBJREF)	pResFile->GetObjectPersistID( getNormalMap());

	Info.m_bFaceToCamera = isParticleStyle( I3_PARTICLE_STYLE_FACETOCAM_ENABLE)?0:1;

	Info.m_pBoundBox	= (OBJREF)	pResFile->GetObjectPersistID( m_pBoundBox);

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Particle::OnSave()", "Could not write particle data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3Particle::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PARTICLE_30 Info;
	INT32 version;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Bug Fix를 위한 임시 코드 1!!!!!!
	AddChild( m_pScene);
	I3_MUST_RELEASE( m_pScene);

	{
		char id[4];

		Rc = pResFile->Read( &id, 4);
		I3_CHKIO( Rc);
		Result += Rc;

		if( memcmp( id, "PTC3", 4) == 0)
			version = 3;
		else if( memcmp( id, "PTC1", 4) == 0)
			version = 2;
		else
			version = 1;
	}

	switch( version)
	{
		case 1 :
			{
				pack::PARTICLE_10 OldInfo;

				pStream->SetPosition( -4, STREAM_CURRENT);

				Rc = pStream->Read( &OldInfo, sizeof(OldInfo));
				I3_CHKIO( Rc);
				Result += Rc;
				
				Info.m_pParticleInfo	= OldInfo.m_pParticleInfo;
				Info.m_pTexture			= OldInfo.m_pTexture;
				Info.m_pTexSeq			= OldInfo.m_pTexSeq;
				Info.m_pMeshInfo		= OldInfo.m_pMeshInfo;
				Info.m_bBlend			= OldInfo.m_bBlend;
				Info.m_SrcBlend			= OldInfo.m_SrcBlend;
				Info.m_DestBlend		= OldInfo.m_DestBlend;
				Info.m_BlendOP			= OldInfo.m_BlendOP;
			}
			break;

		case 2 :
			{
				pack::PARTICLE_20 info2;

				Rc = pStream->Read( &info2, sizeof(info2));
				I3_CHKIO( Rc);
				Result += Rc;

				Info.m_pParticleInfo	=	info2.m_pParticleInfo;
				Info.m_pTexture			=	info2.m_pTexture	;
				Info.m_pTexSeq			=	info2.m_pTexSeq		;
				Info.m_pMeshInfo		=	info2.m_pMeshInfo	;
				Info.m_bBlend			=	info2.m_bBlend		;
				Info.m_SrcBlend			=	info2.m_SrcBlend	;
				Info.m_DestBlend		=	info2.m_DestBlend	;
				Info.m_BlendOP			=	info2.m_BlendOP		;
				Info.m_pNormalMap		=	info2.m_pNormalMap	;
				Info.m_bFaceToCamera	=	info2.m_bFaceToCamera;
				Info.m_bLighting		=	info2.m_bLighting	;
				Info.m_fZBias			=	info2.m_fZBias		;
				Info.m_fZBiasSlope		=	info2.m_fZBiasSlope	;
			}
			break;

		case 3 :
			{
				Rc = pStream->Read( &Info, sizeof(Info));
				I3_CHKIO( Rc);
				Result += Rc;
			}
			break;
	}


	// Particle Info
	if( Info.m_pParticleInfo != 0)
	{
		i3ParticleInfo * pParticleInfo;

		pParticleInfo = (i3ParticleInfo *) pResFile->FindObjectByID( Info.m_pParticleInfo);
		I3ASSERT( pParticleInfo != nullptr);

		SetParticleInfo( pParticleInfo);
	}

	// Texture
	if( Info.m_pTexture != 0)
	{
		i3Texture * pTex = (i3Texture *) pResFile->FindObjectByID( Info.m_pTexture);

		if( pTex != nullptr)
		{
			setTexture( pTex);
		}
	}

	// Texture Sequence
	if( Info.m_pTexSeq != 0)
	{
		i3TextureSequence * pTexSeq = (i3TextureSequence *) pResFile->FindObjectByID( Info.m_pTexSeq);
		I3ASSERT( pTexSeq != nullptr);

		SetTextureSequence( pTexSeq);
	}

	// Mesh
	if( Info.m_pMeshInfo != 0)
	{
		i3PersistantElement * pObj = pResFile->FindObjectByID( Info.m_pMeshInfo);

		if( pObj != nullptr)
		{
			if( i3::same_of<i3SceneGraphInfo* >(pObj))
			{
				SetMesh( (i3SceneGraphInfo *) pObj);
			}
			else
			{
				i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();

				pInfo->setInstanceSg( (i3Node *) pObj);

				SetMesh( pInfo);
			}
		}
	}

	// Normal Map
	if( Info.m_pNormalMap != 0)
	{
		i3Texture * pTex = (i3Texture *) pResFile->FindObjectByID( Info.m_pNormalMap);
		I3ASSERT( pTex != nullptr);

		setNormalMap( pTex);
	}

	//
	if( Info.m_pBoundBox != 0)
	{
		i3BoundBox * pBox = (i3BoundBox *) pResFile->FindObjectByID( Info.m_pBoundBox);
		I3ASSERT( pBox != nullptr);

		I3_REF_CHANGE( m_pBoundBox, pBox);

		m_pScene->SetBound( m_pBoundBox);
	}

	if( Info.m_bBlend != 0)
	{
		addParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE);
		setSrcBlend( (I3G_BLEND) i3Gfx::Convert( (I3G_PERSIST_BLEND) Info.m_SrcBlend));
		setDestBlend( (I3G_BLEND) i3Gfx::Convert( (I3G_PERSIST_BLEND) Info.m_DestBlend));
		setBlendOperation( (I3G_BLEND_OP) i3Gfx::Convert( (I3G_PERSIST_BLEND_OP) Info.m_BlendOP));
	}


	if( Info.m_bDecal != 0)		addParticleStyle( I3_PARTICLE_STYLE_IS_DECAL);
	else						removeParticleStyle( I3_PARTICLE_STYLE_IS_DECAL);

	// Z-Test
	if( Info.m_ZTest == 0)		addParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE);
	else
	{
		if( Info.m_ZTest == 1)	addParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE);
		else					removeParticleStyle( I3_PARTICLE_STYLE_ZTEST_ENABLE);
	}

	if (Info.m_bFaceToCamera == 0)	addParticleStyle(I3_PARTICLE_STYLE_FACETOCAM_ENABLE);
	else							removeParticleStyle(I3_PARTICLE_STYLE_FACETOCAM_ENABLE);

	if (Info.m_bLighting != 0)		addParticleStyle(I3_PARTICLE_STYLE_LIGHTING_ENABLE);
	else							removeParticleStyle(I3_PARTICLE_STYLE_LIGHTING_ENABLE);

	// Z-Write
	if (Info.m_ZWrite == 0)			removeParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
	else
	{
		if (Info.m_ZWrite == 1)		addParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
		else						removeParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
	}

	Create();

	setZBiasSlope( Info.m_fZBiasSlope);
	setZBias( Info.m_fZBias);

	return Result;
}

bool i3Particle::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3Node::OnSaveXML(pFile, pXML);

	if (bResult == false)
		return false;

	pXML->addAttr("ParticleInfo", OBJREF(pFile->GetObjectPersistID(m_pInfo)));
	pXML->addAttr("Texture", OBJREF(pFile->GetObjectPersistID(getTexture())));
	pXML->addAttr("TexSeq", OBJREF(pFile->GetObjectPersistID(m_pTexSeq)));
	pXML->addAttr("MeshInfo", OBJREF(pFile->GetObjectPersistID(m_pMeshInfo)));
	if (isParticleStyle(I3_PARTICLE_STYLE_NMMAP_ENABLE))
		pXML->addAttr("NormalMap", OBJREF(pFile->GetObjectPersistID(getNormalMap())));
	pXML->addAttr("BoundBox", OBJREF(pFile->GetObjectPersistID(m_pBoundBox)));

	pXML->addAttr("Blend", UINT32(isParticleStyle(I3_PARTICLE_STYLE_BLEND_ENABLE) ? 1 : 0));
	pXML->addAttr("SrcBlend", UINT32(getSrcBlend()));
	pXML->addAttr("DestBlend", UINT32(getDestBlend()));
	pXML->addAttr("BlendOP", UINT32(getBlendOperation()));
	pXML->addAttr("Lighting", UINT32(isParticleStyle(I3_PARTICLE_STYLE_LIGHTING_ENABLE) ? 1 : 0));
	pXML->addAttr("ZBiasSlope", getZBiasSlope());
	pXML->addAttr("ZBias", getZBias());
	pXML->addAttr("ZWrite", UINT32(isParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE) ? 1 : 2));
	pXML->addAttr("ZTest", UINT32(isParticleStyle(I3_PARTICLE_STYLE_ZTEST_ENABLE) ? 1 : 2));
	pXML->addAttr("Decal", UINT32(isParticleStyle(I3_PARTICLE_STYLE_IS_DECAL) ? 1 : 0));
	pXML->addAttr("FaceToCamera", isParticleStyle(I3_PARTICLE_STYLE_FACETOCAM_ENABLE) ? 0 : 1);
	
	return bResult;
}

bool i3Particle::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3Node::OnLoadXML(pFile, pXML);

	if (bResult == false)
		return false;

	// Bug Fix를 위한 임시 코드 1!!!!!!
	AddChild(m_pScene);
	I3_MUST_RELEASE(m_pScene);

	OBJREF objref;
	pXML->getAttr("ParticleInfo", &objref);
	if (objref > 0)
	{
		i3ParticleInfo * pParticleInfo = (i3ParticleInfo *)pFile->FindObjectByID(objref);
		I3ASSERT(pParticleInfo != nullptr);

		SetParticleInfo(pParticleInfo);
	}

	pXML->getAttr("Texture", &objref);
	if (objref > 0)
	{
		i3Texture * pTex = (i3Texture *)pFile->FindObjectByID(objref);
		
		if (pTex != nullptr)
		{
			setTexture(pTex);
		}
	}

	pXML->getAttr("TexSeq", &objref);
	if (objref > 0)
	{
		i3TextureSequence * pTexSeq = (i3TextureSequence *)pFile->FindObjectByID(objref);
		I3ASSERT(pTexSeq != nullptr);

		SetTextureSequence(pTexSeq);
	}

	pXML->getAttr("MeshInfo", &objref);
	if (objref > 0)
	{
		i3PersistantElement * pObj = (i3PersistantElement *)pFile->FindObjectByID(objref);
		if (pObj != nullptr)
		{
			if (i3::same_of<i3SceneGraphInfo* >(pObj))
			{
				SetMesh((i3SceneGraphInfo *)pObj);
			}
			else
			{
				i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();

				pInfo->setInstanceSg((i3Node *)pObj);

				SetMesh(pInfo);
			}
		}
	}

	pXML->getAttr("NormalMap", &objref);
	if (objref > 0)
	{
		i3Texture * pTex = (i3Texture *)pFile->FindObjectByID(objref);
		I3ASSERT(pTex != nullptr);

		setNormalMap(pTex);
	}

	pXML->getAttr("BoundBox", &objref);
	if (objref > 0)
	{
		i3BoundBox * pBox = (i3BoundBox *)pFile->FindObjectByID(objref);
		I3ASSERT(pBox != nullptr);

		I3_REF_CHANGE(m_pBoundBox, pBox);

		m_pScene->SetBound(m_pBoundBox);
	}

	UINT32 uTemp;
	pXML->getAttr("Blend", &uTemp);

	if (uTemp > 0)
	{
		addParticleStyle(I3_PARTICLE_STYLE_BLEND_ENABLE);
		pXML->getAttr("SrcBlend", &uTemp);
		setSrcBlend((I3G_BLEND)i3Gfx::Convert((I3G_PERSIST_BLEND)uTemp));

		pXML->getAttr("DestBlend", &uTemp);
		setDestBlend((I3G_BLEND)i3Gfx::Convert((I3G_PERSIST_BLEND)uTemp));

		pXML->getAttr("BlendOP", &uTemp);
		setBlendOperation((I3G_BLEND_OP)i3Gfx::Convert((I3G_PERSIST_BLEND_OP)uTemp));
	}

	pXML->getAttr("Decal", &uTemp);
	if (uTemp != 0)		addParticleStyle(I3_PARTICLE_STYLE_IS_DECAL);
	else				removeParticleStyle(I3_PARTICLE_STYLE_IS_DECAL);

	// Z-Test
	pXML->getAttr("ZTest", &uTemp);
	if (uTemp == 0)		addParticleStyle(I3_PARTICLE_STYLE_ZTEST_ENABLE);
	else
	{
		if (uTemp == 1)	addParticleStyle(I3_PARTICLE_STYLE_ZTEST_ENABLE);
		else			removeParticleStyle(I3_PARTICLE_STYLE_ZTEST_ENABLE);
	}

	pXML->getAttr("FaceToCamera", &uTemp);
	if (uTemp == 0)	addParticleStyle(I3_PARTICLE_STYLE_FACETOCAM_ENABLE);
	else			removeParticleStyle(I3_PARTICLE_STYLE_FACETOCAM_ENABLE);

	pXML->getAttr("Lighting", &uTemp);
	if (uTemp != 0)		addParticleStyle(I3_PARTICLE_STYLE_LIGHTING_ENABLE);
	else				removeParticleStyle(I3_PARTICLE_STYLE_LIGHTING_ENABLE);

	pXML->getAttr("ZWrite", &uTemp);
	if (uTemp == 0)			removeParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
	else
	{
		if (uTemp == 1)		addParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
		else				removeParticleStyle(I3_PARTICLE_STYLE_ZWRITE_ENABLE);
	}

	Create();

	REAL32 fZBiasSlope;
	pXML->getAttr("ZBiasSlope", &fZBiasSlope);
	setZBiasSlope(fZBiasSlope);

	REAL32 fZBias;
	pXML->getAttr("ZBias", &fZBias);
	setZBias(fZBias);

	
	return bResult;
}

void i3Particle::CalcBoundBox(void)
{
	REAL32 timeLength;
	INT32 i, j, cnt;

	if( m_pInfo == nullptr)
		return;

	if( m_pBoundBox == nullptr)
	{
		m_pBoundBox = i3BoundBox::new_object();

		m_pScene->SetBound( m_pBoundBox);
	}

	m_pBoundBox->Reset();

	if( m_pInfo->GetLifeTime() == 0.0f)
	{
		// 무한 시간
		timeLength = 3.0f;		// 무한 시간에 대한 경우 3초 정도에 대한 Bound만 확인한다.
	}
	else
	{
		timeLength = m_pInfo->GetLifeTime();
	}

	cnt = (INT32)(timeLength / 0.016666f);

	for( i = 0; i < cnt; i++)
	{
		Drive( nullptr);

		for( j = 0; j < m_AllocatedCount; j++)
		{
			if((m_ppObj[j]->m_State & I3_PARTICLE_STATE_ENABLE) == 0)
				continue;

			VEC3D temp;
			REAL32 sz;

			sz = MAX( i3Vector::GetX( &m_ppObj[j]->m_Scale),	i3Vector::GetY( &m_ppObj[j]->m_Scale));
			sz = MAX( sz,	i3Vector::GetZ( & m_ppObj[j]->m_Scale));

			temp.x = m_ppObj[j]->m_Pos[0].x + sz;
			temp.y = m_ppObj[j]->m_Pos[0].y + sz;
			temp.z = m_ppObj[j]->m_Pos[0].z + sz;
			m_pBoundBox->ExtendByVec( &temp);

			temp.x = m_ppObj[j]->m_Pos[0].x - sz;
			temp.y = m_ppObj[j]->m_Pos[0].y - sz;
			temp.z = m_ppObj[j]->m_Pos[0].z - sz;
			m_pBoundBox->ExtendByVec( &temp);
		}
	}

	Stop();
}

void i3Particle::RefreshChanges(void)
{
	if( m_pGroup != nullptr)
	{
		m_pGroup->SetParticle( this, true);
	}

	if( m_pInfo->GetType() == i3ParticleInfo::PARTICLE_SCENE)
	{
		INT32 i, j;
		i3AttrSet * pAttrSet;

		for( i = 0; i < m_pScene->GetChildCount(); i++)
		{
			pAttrSet = (i3AttrSet *) m_pScene->GetChild( i);

			for( j = 0; j < pAttrSet->GetAttrCount(); j++)
			{
				i3RenderAttr * pTemp = pAttrSet->GetAttr( j);

				if( i3::same_of<i3BlendEnableAttr* >(pTemp))
				{
					i3BlendEnableAttr * pAttr = (i3BlendEnableAttr *) pTemp;
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_BLEND_ENABLE));
				}
				else if( i3::same_of<i3BlendModeAttr* >(pTemp))
				{
					i3BlendModeAttr * pAttr = (i3BlendModeAttr *) pTemp;
					pAttr->SetSource( getSrcBlend());
					pAttr->SetDestination( getDestBlend());
					pAttr->SetBlendOperation( getBlendOperation());
				}
				else if( i3::same_of<i3ZWriteEnableAttr* >(pTemp))
				{
					i3ZWriteEnableAttr * pAttr = (i3ZWriteEnableAttr *) pTemp;
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_ZWRITE_ENABLE));
				}
				else if( i3::same_of<i3LightingEnableAttr* >(pTemp))
				{
					i3LightingEnableAttr * pAttr = (i3LightingEnableAttr *) pTemp;
					pAttr->Set( isParticleStyle( I3_PARTICLE_STYLE_LIGHTING_ENABLE));
				}
			}
		}
	}
}

void i3Particle::AllocateObject()
{
	UINT32 sz = sizeof(I3PARTICLEOBJECT *) * m_pInfo->GetMaxObjectCount();
	I3ASSERT( sz > 0);
	m_ppObj		= (I3PARTICLEOBJECT **) i3MemReAlloc(m_ppObj, sz);
	I3ASSERT( m_ppObj != nullptr);

	i3mem::FillZero( m_ppObj, sz);
	m_AllocatedCount = m_pInfo->GetMaxObjectCount();
}

void i3Particle::FreeObject()
{
	INT32 i;
	for( i = 0; i < m_AllocatedCount; i++)
	{
		if( m_ppObj[i] != nullptr)
		{
			s_pParticlePool->Free( m_ppObj[i]);

			m_ppObj[i] = nullptr;
		}
	}
	_FreeDecal();
}

//------------------------------------------------------------------------------------//
//									Decal
//------------------------------------------------------------------------------------//
void i3Particle::_FreeDecal( void)
{
	if( m_pDecal != nullptr)
	{
		g_pParticleRender->FreeDecal( m_pDecal);
		m_pDecal = nullptr;
	}

	if( m_pDrawDecalInfo != nullptr)
	{
		g_pParticleRender->FreeDrawDecal( m_pDrawDecalInfo);
		m_pDrawDecalInfo = nullptr;
	}
}
