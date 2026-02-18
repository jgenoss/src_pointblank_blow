#include "pch.h"
#include "EdgeTrailer.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CEdgeTrailer);

CEdgeTrailer::~CEdgeTrailer(void)
{
	if( m_pAttrSet != nullptr)
	{
		m_pAttrSet->RemoveFromParent();
		I3_MUST_RELEASE(m_pAttrSet);
		m_pAttrSet = nullptr;
	}

	I3_SAFE_RELEASE( m_pShader);
	I3MEM_SAFE_FREE( m_pTrails);
}

#define SHADER_EDGE_TRAIL

static i3Transform * s_pNewTrans = nullptr;
bool CEdgeTrailer::Create( INT32 cnt)
{
	cnt = 300;

	m_MaxCount = cnt;


	// Geometry Attribute
	{
		m_pGeoAttr = i3GeometryAttr::new_object_ref();
	}

	// Geometry Node
	{
		m_pGeo = i3Geometry::new_object_ref();

		m_pGeo->AppendGeometryAttr( m_pGeoAttr);
	}

	_CreateVertex();

	{
		m_pAttrSet = i3AttrSet::new_object();
		SetNode( m_pAttrSet);

#if defined( SHADER_EDGE_TRAIL)
		{
			m_pShader = i3ShaderCache::LoadShader( "i3Refract.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
			I3ASSERT( m_pShader != nullptr );

			if (m_pShader)
			{
				m_pShader->setRequestedPSVersion(I3G_SHADER_VERSION_2);

				I3_MUST_ADDREF(m_pShader);

				i3ShaderAttr * pAttr = i3ShaderAttr::new_object_ref();

				pAttr->setShader(m_pShader);

				m_pAttrSet->AddAttr(pAttr);
			}
		}
#endif

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
			pAttr->Set( true);

			m_pAttrSet->AddAttr( pAttr);
		}

		
		{
			m_pTexBindAttr = i3TextureBindAttr::new_object_ref();
			i3Texture * pTex = nullptr;

			#if defined( SHADER_EDGE_TRAIL)
			i3RenderContext * pCtx = g_pViewer->GetRenderContext();
			pTex = pCtx->getLastFrameTargetTexture();
			#else
			pTex = g_pFramework->GetResourceManager()->LoadTexture("Effect/Tex_KnifeFlow.i3i");
			#endif

			m_pTexBindAttr->SetTexture( pTex);
			#if !defined( SHADER_EDGE_TRAIL)
			pTex->Release();
			#endif

			m_pAttrSet->AddAttr( m_pTexBindAttr);
		}

#if defined( SHADER_EDGE_TRAIL)
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( false);

			m_pAttrSet->AddAttr( pAttr);
		}
#else
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
			pAttr->Set( true);

			m_pAttrSet->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);

			m_pAttrSet->AddAttr( pAttr);
		}
#endif
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( true);

			m_pAttrSet->AddAttr( pAttr);
		}

		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

			pAttr->Set( I3G_FACE_CULL_NONE);

			m_pAttrSet->AddAttr( pAttr);
		}

		m_pAttrSet->AddChild( m_pGeo);

		// Attach to Scene 
		{
			i3LayerRootNode * pAttachRoot = g_pFramework->GetLayerRoot( "EffectRoot");

			pAttachRoot->AddChild( m_pAttrSet);
		}
	}

	{
		m_pTrails = (TRAILPOSINFO *) i3MemAlloc( sizeof(TRAILPOSINFO) * cnt);
	}

	return true;
}

void CEdgeTrailer::_CreateVertex(void)
{
	INT32 i, idx;

	INT32 patchCount;

	patchCount = (m_MaxCount - 1);

	// Vertex Array
	{
		i3VertexFormat fmt;

		fmt.SetHasPosition();
		fmt.SetHasNormal();
		fmt.SetHasColor(true);
		fmt.SetTextureCoordSetCount( 1);
		fmt.SetTextureCoordCount( 2);

		m_pVA = i3VertexArray::new_object_ref();
		m_pVA->Create( &fmt, m_MaxCount * 2, 0);
		// 위치 좌표는 현재 없음.
	}

	// Index Array
	{
		m_pIA = i3IndexArray::new_object_ref();

		m_pIA->Create( patchCount * 6, 0, m_pVA->GetCount());

		m_pIA->Lock();

		for( i = 0, idx = 0; i < (patchCount * 6); i += 6, idx += 2)
		{
			m_pIA->SetIndex( i + 0, (idx + 0));
			m_pIA->SetIndex( i + 1, (idx + 1));
			m_pIA->SetIndex( i + 2, (idx + 2));

			m_pIA->SetIndex( i + 3, (idx + 2));
			m_pIA->SetIndex( i + 4, (idx + 1));
			m_pIA->SetIndex( i + 5, (idx + 3));
		}

		m_pIA->Unlock();
	}

	m_pGeoAttr->SetIndexArray( m_pIA);
	m_pGeoAttr->SetVertexArray( m_pVA, I3G_PRIM_TRILIST, 0);
}

void CEdgeTrailer::RotateSph(REAL32 delta)
{
	/*
	if( delta > I3_EPS)
	{
		MATRIX * pMtx = s_pNewTrans->GetMatrix();
		i3Matrix::PreRotateY( pMtx, I3_DEG2RAD( 1.5f) * delta / 0.016f);
	}*/
}

bool CEdgeTrailer::Bind( WeaponBase * pWeapon)
{
	I3ASSERT( pWeapon != nullptr);

	m_pWeapon = pWeapon;

	if( pWeapon != nullptr)
	{
		i3Body * pBody = pWeapon->getSceneObject()->GetBody();
		I3ASSERT( pBody != nullptr);

		// EdgeTraceStartFXDummy
		m_idxBoneStart = pBody->FindBoneByName( "EdgeTraceStartFXDummy");
		
		// EdgeTraceEndFXDummy
		m_idxBoneEnd = pBody->FindBoneByName( "EdgeTraceEndFXDummy");
	}
	else
	{
		m_idxBoneStart = -1;
		m_idxBoneEnd = -1;
	}

	if( m_idxBoneStart == -1 || m_idxBoneEnd == -1)
		return false;

	return true;
}

void CEdgeTrailer::Start(WeaponBase * pKnife)
{
	Reset();

	#if defined( SHADER_EDGE_TRAIL)
	i3RenderContext * pCtx = g_pViewer->GetRenderContext();
	pCtx->SaveLastFrameTexture();
	#endif


	if( Bind( pKnife) == true)
	{
		SetEnable();
	}
}

void CEdgeTrailer::Stop(void)
{
	m_rElapsedStopTime = 0.0f;
	m_bEnableStop = true;
}

void CEdgeTrailer::Reset(void)
{
	m_bEnableStop		= false;
	m_rElapsedStopTime	= 0.0f;

	if( m_pVA != nullptr)
	{
		INT32 TrailCount = 0;
		if( m_idxHead > m_idxTail)
			TrailCount = m_idxHead - m_idxTail;
		else
			TrailCount = (m_idxHead + m_MaxCount) - m_idxTail;

		m_pVA->Lock( I3G_LOCK_NONE, 0, TrailCount * 2);
		for( INT32 i = 0; i < TrailCount; i++)
		{
			VEC2D uv;
			i3Vector::Set( &uv, 0.99f, 0.99f);

			m_pVA->SetTextureCoord(0,(i *2),&uv);
			m_pVA->SetTextureCoord(0,(i *2)+1,&uv);
		}
		m_pVA->Unlock();
	}

	m_idxHead = m_idxTail = 0;
	m_PatchCount = 0;

	SetEnable( false);
}

void CEdgeTrailer::OnUpdate( REAL32 tm)
{
	if( (m_idxBoneStart == -1) || (m_idxBoneEnd == -1))
		return;

	I3ASSERT( m_pWeapon != nullptr);
	i3Body * pBody = m_pWeapon->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	INT32 idxStart, TrailCount;
	
	INT32 dividecnt = 3;

	if( m_idxHead == 0)
	{
		TRAILPOSINFO * pHead	= m_pTrails + m_idxHead;

		i3Vector::Copy( &pHead->m_Pos0, i3Matrix::GetPos( pBody->getCacheMatrix( m_idxBoneStart)));
		i3Vector::Copy( &pHead->m_Pos1, i3Matrix::GetPos( pBody->getCacheMatrix( m_idxBoneEnd)));
		//i3Vector::Copy( &pHead->m_Pos1, &pHead->m_Pos0);
		//i3Vector::AddY( &pHead->m_Pos1, 0.05f);	
		
		pHead->m_State = TPIS_VALID;
		pHead->m_time = 0.0f;

		idxStart = m_idxHead;

		m_idxHead ++;
		if( m_idxHead >= m_MaxCount)
			m_idxHead = 0;

		if( m_idxHead == m_idxTail)
		{
			m_idxTail ++;
			if( m_idxTail >= m_MaxCount)
				m_idxTail = 0;
		}
	}
	else
	{
		VEC3D	vStart,vEnd,vDir0, vDir1, vDirTmp;

		i3Vector::Copy( &vStart, i3Matrix::GetPos( pBody->getCacheMatrix( m_idxBoneStart)));

		//i3Vector::Copy( &vEnd, &vStart);
		//i3Vector::AddY( &vEnd, 0.05f);	

		i3Vector::Copy( &vEnd, i3Matrix::GetPos( pBody->getCacheMatrix( m_idxBoneEnd)));
		
		TRAILPOSINFO *	pPrev	= m_pTrails + (m_idxHead - 1);

		i3Vector::Sub( &vDir0, &vStart, &pPrev->m_Pos0);
		i3Vector::Sub( &vDir1, &vEnd, &pPrev->m_Pos1);

		for( INT32 i = 0;i < dividecnt; ++i)
		{
			REAL32 ratio = (i+1.0f)/dividecnt;
			// 이번 Frame의 Position을 Update한다.
			TRAILPOSINFO * pHead	= m_pTrails + m_idxHead;

			i3Vector::Scale( &vDirTmp, &vDir0, ratio);
			if( i3Vector::LengthSq( &vDirTmp) < I3_EPS)
			{
				continue;
			}

			i3Vector::Add( &pHead->m_Pos0, &pPrev->m_Pos0, &vDirTmp);

			i3Vector::Scale( &vDirTmp, &vDir1, ratio);
			if( i3Vector::LengthSq( &vDirTmp) < I3_EPS)
			{
				continue;
			}

			i3Vector::Add( &pHead->m_Pos1, &pPrev->m_Pos1, &vDirTmp);
			
			pHead->m_State = TPIS_VALID;
			pHead->m_time = ( 1.0f - 0.3333f) * tm;

			idxStart = m_idxHead;

			m_idxHead ++;
			if( m_idxHead >= m_MaxCount)
				m_idxHead = 0;

			if( m_idxHead == m_idxTail)
			{
				m_idxTail ++;
				if( m_idxTail >= m_MaxCount)
					m_idxTail = 0;
			}
		}
	}


	if( m_idxHead > m_idxTail)
		TrailCount = m_idxHead - m_idxTail;
	else
		TrailCount = (m_idxHead + m_MaxCount) - m_idxTail;

	// 처음 들어왔을 때에는 사각형을 만들 수 없다.
	if( TrailCount >= 2)
	{
		m_PatchCount = TrailCount - 1;

		// Vertex를 Update한다.
		m_pVA->Lock( I3G_LOCK_NONE, 0, TrailCount * 2);

		INT32 idx;
		VEC3D pos[2];
		COLOR col;
		REAL32 alphaStep = 1.0f / TrailCount;
		REAL32 alpha = alphaStep;

		idx = m_idxTail;

		for( INT32 i = 0; i < TrailCount; i++)
		{
			VEC2D uv;

			i3Vector::Copy( &pos[0], &m_pTrails[idx].m_Pos0);
			i3Vector::Copy( &pos[1], &m_pTrails[idx].m_Pos1);

			if((m_pTrails[idx].m_State & TPIS_VALID) == 0)
			{
				i3Color::Set( &col, (UINT8) 0, 0, 0, 0);
				m_pVA->SetColor( (i * 2), 2, &col);

				setUV( &uv, 0.99f, 0.99f);
				m_pVA->SetTextureCoord(0,(i *2),&uv);
				m_pVA->SetTextureCoord(0,(i *2)+1,&uv);
			}
			else
			{
				REAL32 al = MINMAX( 0.0f, (1.0f - m_pTrails[idx].m_time / 0.15f), 0.99f);

				i3Color::Set( &col, 255, 255, 255, (UINT8)(al * 255));

				m_pVA->SetPositions( (i * 2), 2, pos);
				m_pVA->SetColor( (i * 2), 2, &col);

				setUV( &uv, (1.0f - al), 0.99f);
				m_pVA->SetTextureCoord(0,(i *2),&uv);

				setV( &uv, 0.01f);
				m_pVA->SetTextureCoord(0,(i *2) + 1,&uv);

				m_pTrails[idx].m_time += tm;
				if( m_pTrails[idx].m_time >= 0.15f)
				{
					m_pTrails[idx].m_State = 0;
				}
			}
		
/*
			if( TrailCount - numTails > i)
			{
				i3Color::Set( &col, 0, 0, 0, 0);
				m_pVA->SetColor( (i * 2), 2, &col);

				setUV( &uv, 0.99f, 0.99f);
				m_pVA->SetTextureCoord(0,(i *2),&uv);
				m_pVA->SetTextureCoord(0,(i *2)+1,&uv);
			}
			else
			{
				REAL32 al = alpha;
				if( TrailCount >= numTails)
				{
					al = (REAL32)(i - (TrailCount - numTails))/numTails;
				}

				i3Color::Set( &col, 0, 0, 0, (UINT8)(al * 255));

				m_pVA->SetPositions( (i * 2), 2, pos);
				m_pVA->SetColor( (i * 2), 2, &col);

				setUV( &uv, (1.0f - al), 1.0f);
				m_pVA->SetTextureCoord(0,(i *2),&uv);

				setV( &uv, 0.0f);
				m_pVA->SetTextureCoord(0,(i *2) + 1,&uv);
			}
*/
			if( idx > 1)
			{
				VEC3D	nor1, nor2;

				i3Vector::Normal( &nor1, &m_pTrails[idx-1].m_Pos0, &m_pTrails[idx-1].m_Pos1, &pos[0]);	//abc
				i3Vector::Normal( &nor2,  &pos[0], &m_pTrails[idx-1].m_Pos1, &pos[1]);					//cbd

				m_pVA->SetNormal((i *2),&nor1);
				m_pVA->SetNormal((i *2)+1,&nor2);
			}

			idx++;
			if( idx >= m_MaxCount)
				idx = 0;

			alpha += alphaStep;
		}

		I3ASSERT( idx == m_idxHead);

		m_pVA->Unlock();

		m_pGeoAttr->SetPrimitiveCount( m_PatchCount * 2);
	}

	if( m_bEnableStop)
	{
		m_rElapsedStopTime += tm;
		if( m_rElapsedStopTime > 0.3f)
		{
			m_bEnableStop = false;
			Reset();
		}
	}
}

bool CEdgeTrailer::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pGeoAttr->SetVertexArray( nullptr);
	m_pGeoAttr->SetIndexArray( nullptr);
	return true;
}

bool CEdgeTrailer::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	_CreateVertex();
	return true;
}
