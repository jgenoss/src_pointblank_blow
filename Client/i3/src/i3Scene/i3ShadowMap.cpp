#include "i3SceneDef.h"
#include "i3ShadowMap.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "i3SceneUtil.h"

//#define I3L_SAVSM
//#define I3L_VSM
//#define I3L_ESM

I3_CLASS_INSTANCE( i3ShadowMap);

// ShadowMap 구현 (2 Pass)
//
// - Shadow Map RenderTarget으로 전환한다.
// - Shadow Map RenderTarget Clear
// - Light-Space에서 Caster를 Rendering한다. (NoLight, White, Depth, Z->R)
// - Receiver들을 Diffuse Render한다.
// - Receiveer들을 Shadow Map을 입혀 다시 Render한다. (No Diffuse)


I3_TRACE_RESULT I3SG_NODE_PROC( i3ShadowMap)( i3Node * pNode, i3SceneTracer * pTracer)
{
	return I3_TRACE_RESULT_CONTINUE;
}

i3ShadowMap::i3ShadowMap(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3ShadowMap));
}

i3ShadowMap::~i3ShadowMap(void)
{
	I3_SAFE_RELEASE( m_pLightViewMatrixAttr);
	I3_SAFE_RELEASE( m_pLightProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pTexBindAttr);
	I3_SAFE_RELEASE( m_pZWriteEnableAttr);
	I3_SAFE_RELEASE( m_pFaceCullModeAttr);
	I3_SAFE_RELEASE( m_pRenderTargetAttr);
	I3_SAFE_RELEASE( m_pClearRenderTargetAttr);
	I3_SAFE_RELEASE( m_pCasterShaderAttr);
	I3_SAFE_RELEASE( m_pTexEnableAttr);

	I3_SAFE_RELEASE( m_pCasterShader);
	I3_SAFE_RELEASE( m_pDisplayList);

	I3_SAFE_RELEASE( m_pSAT);
	I3_SAFE_RELEASE( m_pBlur);

	I3_SAFE_RELEASE( m_pSg);

	#if defined( I3_DEBUG)
	I3_SAFE_RELEASE( m_pDebugViewShader);
	I3_SAFE_RELEASE( m_pDebugViewShaderAttr);

	I3_SAFE_RELEASE( m_pRectGeoAttr);
	I3_SAFE_RELEASE( m_pViewMatrixAttr);
	I3_SAFE_RELEASE( m_pProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pViewMatrixAttr2);

	I3_SAFE_RELEASE( m_pRectGeoAttr2);
	I3_SAFE_RELEASE( m_pTexBindAttr2);
	#endif
}


void i3ShadowMap::setLight( i3Light * pLight)
{
	m_fCutOff = pLight->getOuterAngle();

	m_pLight = pLight;

	switch( pLight->GetLightType())
	{
		case I3G_LIGHT_DIRECTIONAL :
			// Perspective를 사용하되, 원근감을 매우 미약한 수준으로 잡는다.
				#if 0
					i3Matrix::PerspectiveFOVRH( m_pLightProjectMatrixAttr->GetMatrix(), m_fCutOff, 1.0f, 
						0.01f, 100000.0f);
				#else
					i3Matrix::OrthoRH( m_pLightProjectMatrixAttr->GetMatrix(), 
						2.0f, 2.0f, // m_pShadowRTAttr->GetWidth(), m_pShadowRTAttr->GetWidth(), 
						1.0f, 10000.0f);
				#endif

				#if 1
				{
					MATRIX rot;
					VEC3D axis;

					i3Vector::Copy( &axis, i3Matrix::GetRight( m_pLightProjectMatrixAttr->GetMatrix()));
					i3Vector::Normalize( &axis, &axis);
					i3Vector::Copy( &axis, &I3_XAXIS);

					i3Matrix::SetRotateAxis( &rot, &axis, I3_DEG2RAD( 180.0f));
					i3Matrix::Mul( m_pLightProjectMatrixAttr->GetMatrix(), &rot, m_pLightProjectMatrixAttr->GetMatrix());
				}
				#endif
			break;

		case I3G_LIGHT_SPOT :
		case I3G_LIGHT_POINT :
			i3Matrix::PerspectiveFOVRH( m_pLightProjectMatrixAttr->GetMatrix(), pLight->getOuterAngle(), 1.0f,
				1.0f, 100.0f);
			break;
	}

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

void i3ShadowMap::SetLightTransform( MATRIX * pMtx)
{
	i3Matrix::Copy( &m_mLightTx, pMtx);
	i3Matrix::Inverse( m_pLightViewMatrixAttr->GetMatrix(), nullptr, pMtx);

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

bool i3ShadowMap::CreateCasterShader(void)
{
	m_pCasterShader = i3ShaderCache::LoadShader( "i3ShadowMapCaster.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_MUST_ADDREF( m_pCasterShader);

	return true;
}

void i3ShadowMap::Create(void)
{
	CreateCasterShader();

	m_pDisplayList = i3RenderAttrListAttr::new_object();

	{
		m_pLightViewMatrixAttr = i3ViewMatrixAttr::new_object();
		m_pLightProjectMatrixAttr = i3ProjectMatrixAttr::new_object();
	}

	{
		m_pRenderTargetAttr = i3RenderTargetAttr::new_object();
	}

	{
		COLOR col;

		#if defined( I3L_SAVSM)
			i3Color::Set( &col, 0, 0, 0, 0);
		#else
			i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		#endif
		m_pClearRenderTargetAttr = i3ClearRenderTargetAttr::new_object();
		m_pClearRenderTargetAttr->SetAddClearMask( I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH);
		m_pClearRenderTargetAttr->SetClearColor( &col);
		m_pClearRenderTargetAttr->SetClearZ( 1.0f);
	}

	{
		m_pCasterShaderAttr = i3ShaderAttr::new_object();
		m_pCasterShaderAttr->setAddCacheState( true);
		m_pCasterShaderAttr->setShader( m_pCasterShader);
	}

	{
		i3VertexArray * pVA;

		pVA = i3VertexArray::CreateRect( 2, 2, true, false, false, false, false, false);

		m_pRectGeoAttr = i3GeometryAttr::new_object();
		m_pRectGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
	}

	{
		m_pFaceCullModeAttr = i3FaceCullModeAttr::new_object();
		m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
	}

	#if defined( I3_DEBUG)
	{
		m_pZWriteEnableAttr = i3ZWriteEnableAttr::new_object();
		m_pZWriteEnableAttr->Set(true);

		m_pProjectMatrixAttr = i3ProjectMatrixAttr::new_object();

		m_pViewMatrixAttr = i3ViewMatrixAttr::new_object();
		i3Matrix::SetScale( m_pViewMatrixAttr->GetMatrix(), 0.25f, 0.25, 1.0f);
		i3Matrix::PostTranslate( m_pViewMatrixAttr->GetMatrix(), 0.75f, 0.75f, 0.0f);

		m_pViewMatrixAttr2 = i3ViewMatrixAttr::new_object();
		i3Matrix::SetScale( m_pViewMatrixAttr2->GetMatrix(), 0.25f, 0.25, 1.0f);
		i3Matrix::PostTranslate( m_pViewMatrixAttr2->GetMatrix(), 0.75f, 0.25f, 0.0f);

		m_pTexBindAttr = i3TextureBindAttr::new_object();

		m_pTexEnableAttr = i3TextureEnableAttr::new_object();
		m_pTexEnableAttr->Set(true);

		m_pDebugViewShader = i3ShaderCache::LoadShader( "i3ShadowMapDebug.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
		I3_MUST_ADDREF( m_pDebugViewShader);

		m_pDebugViewShaderAttr = i3ShaderAttr::new_object();
		m_pDebugViewShaderAttr->setAddCacheState( true);
		m_pDebugViewShaderAttr->setShader( m_pDebugViewShader);

		m_pRectGeoAttr2 = i3GeometryAttr::new_object();
		m_pRectGeoAttr2->SetVertexArray( m_pRectGeoAttr->GetVertexArray(), I3G_PRIM_TRILIST, 2, 0, false);

		m_pTexBindAttr2 = i3TextureBindAttr::new_object();
	}
	#endif

	#if defined( I3L_SAVSM)
	{
		m_pSAT = i3SATGenerator::new_object();
		m_pSAT->Create();
	}
	#endif

	#if defined( I3L_VSM) || defined( I3L_ESM)
	{
		m_pBlur = i3VSM_BlurFilter::new_object();
		m_pBlur->Create();
	}
	#endif
}

void i3ShadowMap::CalcShadowMatrix( i3SceneTracer * pTracer)
{
	MATRIX mtxV2L;
	REAL32 fCutoff, tanTheta, Span, distLight;
	VEC3D	vOrigin[8],	vFrusta[8];
	MATRIX mLightTx;
	VEC3D vLightDir, vTarget, vLightPos, vTemp;
	i3BoundBox box;
	VEC3D vmin, vmax;
	INT32 i;
	i3Camera * pCam = pTracer->getCurrentCamera();

	fCutoff = i3Math::cos( m_fCutOff);
	tanTheta = i3Math::tan( m_fCutOff);

	if( getStyle() & I3SG_SHADOWMAP_STYLE_AUTOFIT)
	{
		// Camera의 Frustum Volume을 구한다.
		{
			REAL32 nx, nz, fx, fz;

			nz = - pCam->GetNear();
			nx = nz * tanTheta;
			
			fz = - i3Math::Min( pCam->GetFar(), m_AutoFitCamDist);
			fx = fz * tanTheta;

			i3Vector::Set( &vOrigin[0], -nx,	-nx,	nz);
			i3Vector::Set( &vOrigin[1], -nx,	nx,		nz);
			i3Vector::Set( &vOrigin[2], nx,		-nx,	nz);
			i3Vector::Set( &vOrigin[3], nx,		nx,		nz);

			i3Vector::Set( &vOrigin[4], -fx,	-fx,	fz);
			i3Vector::Set( &vOrigin[5], -fx,	fx,		fz);
			i3Vector::Set( &vOrigin[6], fx,		-fx,	fz);
			i3Vector::Set( &vOrigin[7], fx,		fx,		fz);
			
			// World-Space로의 Transform
			i3Matrix::TransformPoints( vFrusta, vOrigin, 8, pTracer->GetInverseViewMatrix());

			i3Vector::Set( &vTarget, 0.0f, 0.0f, 0.0f);
			i3Vector::Set( &vmin, 0.0f, 0.0f, 0.0f);
			i3Vector::Set( &vmax, 0.0f, 0.0f, 0.0f);

			for( i = 0; i < 8; i++)
			{
				if( i == 0)
				{
					i3Vector::Copy( &vmin, &vFrusta[i]);
					i3Vector::Copy( &vmax, &vFrusta[i]);
				}
				else
				{
					i3Vector::Minimize( &vmin, &vmin, &vFrusta[i]);
					i3Vector::Maximize( &vmax, &vmax, &vFrusta[i]);
				}

				i3Vector::Add( &vTarget, &vTarget, &vFrusta[i]);
			}
			i3Vector::Scale( &vTarget, &vTarget, 1.0f / 8.0f);

			// Light-Space에서 Rendering되어야 할 넓이를 구한다.
			Span = i3Math::Max( i3Math::abs( vmax.x), i3Math::abs( vmin.x));
			Span = i3Math::Max( Span, i3Math::Max( i3Math::abs( vmax.y), i3Math::abs( vmin.y)));

			distLight = Span / tanTheta;

			//i3Vector::SetY( &vTarget, 0.0f);
		}

		// vLightDir을 유지하면서, vTarget 점을 바라보는 임의의 Light Position 계산
		{
			i3Vector::Normalize( &vLightDir, i3Matrix::GetAt( &m_mLightTx));
			i3Vector::Scale( &vTemp, &vLightDir, -distLight);
			i3Vector::Add( &vLightPos, &vTemp, &vTarget);

			i3Matrix::MakeWithAt( &mLightTx, &vLightDir, &I3_YAXIS);
			i3Matrix::SetPos( &mLightTx, &vLightPos);

			SetBoundBox( &vmin, &vmax);
			SetLightTransform( &mLightTx);
		}


		// Near와 Far값을 계산..
		{
			REAL32 minZ = 0.0f, maxZ = 0.0f;
			MATRIX * pProj = m_pLightProjectMatrixAttr->GetMatrix();

			i3Matrix::TransformPoints( vFrusta, vFrusta, 8, m_pLightViewMatrixAttr->GetMatrix());

			for( i = 0; i < 8; i++)
			{
				if( i == 0)
				{
					minZ = maxZ = i3Math::abs( i3Vector::GetZ( &vFrusta[i]));
				}
				else
				{
					minZ = MIN( minZ, i3Math::abs( i3Vector::GetZ( &vFrusta[i])));
					maxZ = MAX( maxZ, i3Math::abs( i3Vector::GetZ( &vFrusta[i])));
				}
			}

			if( minZ < 0.1f)	minZ = 0.1f;

			i3Matrix::PerspectiveFOVRH( pProj, I3_DEG2RAD( 25.0f), 1.0f, minZ, maxZ);	//1.0f, 300.0f);

			#if 1
			{
				MATRIX rot;
				VEC3D axis;

				i3Vector::Copy( &axis, i3Matrix::GetRight( pProj));
				i3Vector::Normalize( &axis, &axis);
				i3Vector::Copy( &axis, &I3_XAXIS);

				i3Matrix::SetRotateAxis( &rot, &axis, I3_DEG2RAD( 180.0f));
				i3Matrix::Mul( pProj, &rot, pProj);
			}
			#endif
		}
	}

	// Pseudo Light Position
	//i3Vector::TransformCoord( (VEC3D*)&m_LightPos, i3Matrix::GetPos( &m_mLightTx), pTracer->GetViewMatrix());

	// Pseudo Light Direction
	//i3Vector::TransformNormal( (VEC3D*)&m_LightDir, i3Matrix::GetAt( &m_mLightTx), pTracer->GetViewMatrix());
	//i3Vector::Normalize( (VEC3D*)&m_LightDir, (VEC3D*)&m_LightDir);

	i3Matrix::Mul( &mtxV2L, pTracer->GetInverseViewMatrix(), m_pLightViewMatrixAttr->GetMatrix());
	i3Matrix::Mul( &mtxV2L, &mtxV2L, m_pLightProjectMatrixAttr->GetMatrix());

	m_pLight->setShadowMapMatrix( &mtxV2L);

	//fMapSz = (REAL32) m_pShadowRTAttr->GetWidth();

	/*
	pTracer->GetRenderContext()->AddShadowMap( m_pShadowRTAttr->GetColorRenderTarget(), &mtxV2L, nullptr, 
		(VEC3D *) &m_LightPos, (VEC3D *) &m_LightDir, 
		&m_ShadowColor, fMapSz, fCutoff);
		*/
}

void i3ShadowMap::Apply( i3Light * pLight, i3SceneTracer * pTracer, i3Node * pSg, i3BoundBox * pBox)
{
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();
	I3SG_TRACER_CONTEXT ctx;

	setLight( pLight);
	SetLightTransform( pLight->GetWorldSpaceTransform());

	I3_REF_CHANGE(m_pSg, pSg);

	if( m_pSg == nullptr)
		return;

	if( pBox->IsEmpty())
		return;

	m_ReceiverBound.SetMinMax( pBox->GetMin(), pBox->GetMax());

	// Shadow Map의 Edge에 걸리는 경우 Wrapping 등으로 인해 문제가
	// 발생할 수 있으므로 

	// [20]  - call:00001,  elapsed:0.002235 ms (min:0.000000, max:0.013968, avg:0.001956)
	// Vectors
	i3Vector::Normalize( &m_vLightDirWS, i3Matrix::GetAt( &m_mLightTx));
	i3Vector::Scale( &m_vLightDirWS, &m_vLightDirWS, -1.0f);

	i3Vector::Normalize( &m_vCamDirWS, i3Matrix::GetAt( pTracer->GetInverseViewMatrix()));
	i3Vector::Scale( &m_vCamDirWS, &m_vCamDirWS, -1.0f);

	i3Vector::Copy( &m_vCamPosWS, i3Matrix::GetPos( pTracer->GetInverseViewMatrix()));

	// Caster에 대한 Output을 구해온다.
	{
		// [21]  - call:00001,  elapsed:0.526883 ms (min:0.000000, max:0.638349, avg:0.466819)
		switch( m_Method)
		{
			case LIGHTSPACE_PERSPECTIVE :		CalcLSPSMShadowMatrix2( pTracer);		break;
			default :							CalcShadowMatrix( pTracer);				break;
		}

		// Scene Graph를 Trace하기 위한 준비
		// [22]  - call:00001,  elapsed:0.012013 ms (min:0.000000, max:0.023467, avg:0.007822)
		pTracer->SaveContext( &ctx);
		pTracer->SetViewMatrix( m_pLightViewMatrixAttr->GetMatrix(), &m_mLightTx);
		pTracer->SetProjectionMatrix( m_pLightProjectMatrixAttr->GetMatrix());
		pTracer->GetModelViewMatrixStack()->PushIdentity();
		pTracer->addState( I3SG_TRACER_STATE_GEN_SHADOW);

		pTracer->PushOutputList( m_pDisplayList);

		m_pRenderTargetAttr->BindRenderTarget( m_pLight->getShadowMap(), m_pLight->getDepthBuffer());

		pManager->FlushPushedAttrs( pTracer->GetOutput());
		pManager->PushAttr( m_pRenderTargetAttr);
		pManager->PushAttr( m_pClearRenderTargetAttr);
		pManager->PushAttr( m_pCasterShaderAttr);
		pManager->PushAttr( m_pLightViewMatrixAttr);
		pManager->PushAttr( m_pLightProjectMatrixAttr);
		pManager->PushAttr( m_pFaceCullModeAttr);

		pTracer->addDisableNodeMask( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

		// Dynamic Caster의 Render
		{
			// [23]  - call:00001,  elapsed:0.808203 ms (min:0.000000, max:1.092876, avg:0.653435)
			pTracer->Trace( m_pSg);
		}

		// Tracer의 Restore
		{
			// [24]  - call:00001,  elapsed:0.002235 ms (min:0.000000, max:0.016483, avg:0.001956)
			pTracer->removeDisableNodeMask( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
			pManager->PopAttr( m_pCasterShaderAttr->GetID());
			pManager->PopAttr( m_pClearRenderTargetAttr->GetID());
			pManager->PopAttr( m_pRenderTargetAttr->GetID());
			pManager->PopAttr( m_pFaceCullModeAttr->GetID());
			pManager->PopAttr( m_pLightViewMatrixAttr->GetID());
			pManager->PopAttr( m_pLightProjectMatrixAttr->GetID());

			pManager->FlushPushedAttrs( pTracer->GetOutput());

			pTracer->PopOutputList();

			pTracer->RestoreContext( &ctx);

			pTracer->GetModelViewMatrixStack()->Pop();
		}

		// Drawing
		pTracer->AddAttributeToOutput( m_pDisplayList);

		#if defined( I3L_SAVSM)
		if( pLight->getShadowMap( 1) != nullptr)
		{
			// Summed-Area Table Generation
			m_pSAT->Bind( pLight->getShadowMap( 0), pLight->getShadowMap( 1), pLight->getDepthBuffer());
			m_pSAT->Process( pTracer);
		}
		#endif

		#if defined( I3L_VSM) || defined( I3L_ESM)
		if( pLight->getShadowMap( 1) != nullptr)
		{
			// Gaussian Blur Filter
			m_pBlur->Bind( pLight->getShadowMap(0), pLight->getShadowMap( 1), pLight->getDepthBuffer());
			m_pBlur->Process( pTracer);
		}
		#endif

		#if 0
		{
			m_pTexBindAttr->SetTexture( pLight->getShadowMap());

			pManager->PushAttr( m_pTexBindAttr);
			pManager->PushAttr( m_pTexEnableAttr);
			pManager->PushAttr( m_pZWriteEnableAttr);
			pManager->PushAttr( m_pProjectMatrixAttr);
			pManager->PushAttr( m_pViewMatrixAttr);
			pManager->PushAttr( m_pDebugViewShaderAttr);
			
			pManager->FlushPushedAttrs( pTracer->GetOutput());
			
			////////// Shadow Map View
			pTracer->AddAttributeToOutput( m_pRectGeoAttr);
			pManager->PopAttr( m_pViewMatrixAttr->GetID());
			pManager->PopAttr( m_pTexBindAttr->GetID());

			///////// SAT Map View
			if( pLight->getShadowMap( 1) != nullptr)
			{
				m_pTexBindAttr2->SetTexture( pLight->getShadowMap( 1));
				pManager->PushAttr( m_pTexBindAttr2);
				pManager->PushAttr( m_pViewMatrixAttr2);

				pManager->FlushPushedAttrs( pTracer->GetOutput());

				pTracer->AddAttributeToOutput( m_pRectGeoAttr2);
				pManager->PopAttr( m_pViewMatrixAttr2->GetID());
				pManager->PopAttr( m_pTexBindAttr2->GetID());
			}

			pManager->PopAttr( m_pDebugViewShaderAttr->GetID());
			pManager->PopAttr( m_pProjectMatrixAttr->GetID());
			pManager->PopAttr( m_pZWriteEnableAttr->GetID());
			pManager->PopAttr( m_pTexEnableAttr->GetID());
			pManager->FlushPushedAttrs( pTracer->GetOutput());
		}
		#endif

		pTracer->removeState( I3SG_TRACER_STATE_GEN_SHADOW);
	}
}

void i3ShadowMap::Reset(void)
{
	m_pLight = nullptr;
	I3_SAFE_RELEASE( m_pSg);
	m_pRenderTargetAttr->BindRenderTarget( nullptr, nullptr);
}

void i3ShadowMap::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

void i3ShadowMap::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
}

UINT32 i3ShadowMap::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShadowMap::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}

void i3ShadowMap::OnLostDevice( bool bLostDevice)
{
	if( m_pRectGeoAttr != nullptr)
	{
		m_pRectGeoAttr->SetVertexArray( nullptr);
	}

#if defined( I3_DEBUG)
	if( m_pRectGeoAttr2 != nullptr)
	{
		m_pRectGeoAttr2->SetVertexArray( nullptr);
	}
#endif
}

void i3ShadowMap::OnRevive( i3RenderContext * pCtx)
{
	if( m_pRectGeoAttr != nullptr)
	{
		i3VertexArray * pVA;

		pVA = i3VertexArray::CreateRect( 2, 2, true, false, false, false, false, false);

		m_pRectGeoAttr->SetVertexArray( pVA);

		#if defined( I3_DEBUG)
		if( m_pRectGeoAttr2 != nullptr)
		{
			m_pRectGeoAttr2->SetVertexArray( pVA);
		}	
		#endif
	}
}