#include "i3SceneDef.h"
#include "i3PerspectiveShadowMap.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3PerspectiveShadowMap, i3Node);

// ShadowMap 구현 (2 Pass)
//
// - Shadow Map RenderTarget으로 전환한다.
// - Shadow Map RenderTarget Clear
// - Light-Space에서 Caster를 Rendering한다. (NoLight, White, Depth, Z->R)
// - Receiver들을 Diffuse Render한다.
// - Receiveer들을 Shadow Map을 입혀 다시 Render한다. (No Diffuse)

#if 0
	#define	VLOG		i3VLog
#else
	#define VLOG		__noop
#endif


I3_TRACE_RESULT I3SG_NODE_PROC( i3PerspectiveShadowMap)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3PerspectiveShadowMap * pShadowMap = (i3PerspectiveShadowMap *) pNode;
	
	pShadowMap->Apply( pTracer);

	return Result;	
}

i3PerspectiveShadowMap::i3PerspectiveShadowMap(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3PerspectiveShadowMap));

	m_pReceiver = NULL;
	m_pStaticCaster = NULL;
	m_pCaster = NULL;
	m_pLight = NULL;

	i3Matrix::Identity( &m_mLightTx);

	m_pShadowRTAttr = NULL;
	m_pStaticRTAttr = NULL;
	m_pClearRTAttr = NULL;
	m_pCasterRenderListAttr = NULL;
	m_pCasterShaderAttr = NULL;
	m_pCopyShaderAttr = NULL;

	m_pRectGeoAttr = NULL;
	m_pTexBindAttr = NULL;
	m_pZTestEnableAttr = NULL;
	m_pFaceCullModeAttr = NULL;

	m_pLightViewMatrixAttr = NULL;
	m_pLightProjectMatrixAttr = NULL;
	m_pShadowMapEnableAttr = NULL;

	m_fFocusDist = -0.8f;

	m_fCamNear = 1.0f;
	m_fCamFar = 30.0f;

	i3Color::Set( &m_ShadowColor, 0.2f, 0.2f, 0.2f, 0.7f);

	m_Style = 0;
	m_State = 0;
}

i3PerspectiveShadowMap::~i3PerspectiveShadowMap(void)
{
	I3_SAFE_RELEASE( m_pReceiver);
	I3_SAFE_RELEASE( m_pStaticCaster);
	I3_SAFE_RELEASE( m_pCaster);
	I3_SAFE_RELEASE( m_pLight);
	I3_SAFE_RELEASE( m_pShadowRTAttr);
	I3_SAFE_RELEASE( m_pStaticRTAttr);
	I3_SAFE_RELEASE( m_pClearRTAttr);
	I3_SAFE_RELEASE( m_pCasterRenderListAttr);
	I3_SAFE_RELEASE( m_pCasterShaderAttr);
	I3_SAFE_RELEASE( m_pCopyShaderAttr);
	I3_SAFE_RELEASE( m_pLightViewMatrixAttr);
	I3_SAFE_RELEASE( m_pLightProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pShadowMapEnableAttr);
	I3_SAFE_RELEASE( m_pRectGeoAttr);
	I3_SAFE_RELEASE( m_pTexBindAttr);
	I3_SAFE_RELEASE( m_pZTestEnableAttr);
	I3_SAFE_RELEASE( m_pFaceCullModeAttr);
}

void i3PerspectiveShadowMap::setReceiver( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pReceiver);
	m_pReceiver = pNode;
}

void i3PerspectiveShadowMap::setCaster( i3Node * pNode)
{
	if( pNode != NULL)
	{
		pNode->AddRef();

		pNode->GetWrappingBound( &m_BBox);
	}

	I3_SAFE_RELEASE( m_pCaster);
	m_pCaster = pNode;
}

void i3PerspectiveShadowMap::setStaticCaster( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pStaticCaster);
	m_pStaticCaster = pNode;

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

void i3PerspectiveShadowMap::setLight( i3Light * pLight, REAL32 fCutoff, REAL32 fNear, REAL32 fFar)
{
	m_fCutOff = fCutoff;
	m_fNear = fNear;
	m_fFar = fFar;

	if( pLight != NULL)
		pLight->AddRef();

	I3_SAFE_RELEASE( m_pLight);
	m_pLight = pLight;

	switch( pLight->GetLightType())
	{
		case I3G_LIGHT_DIRECTIONAL :
			// Perspective를 사용하되, 원근감을 매우 미약한 수준으로 잡는다.
				#if 1
					i3Matrix::PerspectiveFOVRH( &m_mProject, I3_DEG2RAD( 45.0f), 1.0f, 
						m_fNear, m_fFar);
				#else
					i3Matrix::OrthoRH( &m_mProject, 
						2.0f, 2.0f, // m_pShadowRTAttr->GetWidth(), m_pShadowRTAttr->GetWidth(), 
						m_fNear, m_fFar);
				#endif
			break;

		case I3G_LIGHT_SPOT :
		case I3G_LIGHT_POINT :
			i3Matrix::PerspectiveFOVRH( &m_mProject, pLight->getOuterAngle(), 1.0f,
				m_fNear, pLight->getRange());
			break;
	}

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

void i3PerspectiveShadowMap::SetLightTransform( MATRIX * pMtx)
{
	i3Matrix::Copy( &m_mLightTx, pMtx);

	/*
	{
		MATRIX m;

		i3Matrix::SetRotateXDeg( &m, 180.0f);
		i3Matrix::Mul( &m_mLightTx, &m, &m_mLightTx);
	}
	*/


	i3Matrix::Inverse( &m_mInvLightTx, NULL, pMtx);

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

bool i3PerspectiveShadowMap::CreateCasterShader(void)
{
	i3ShaderCode * pCode;
	i3ShaderContext * pCtx;

	pCtx = i3ShaderContext::NewObjectRef();

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3ShaderDefine.inl");
	pCtx->AddCode( pCode);

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3ShadowMapCaster.fx");
	pCtx->AddCode( pCode);

	pCtx->SetVertexShaderName( "VertScene");
	pCtx->SetSkinVertexShaderName( "VertSkin");
	pCtx->SetPixelShaderName( "PS_i3ShadowMapCaster");

	pCtx->Compile( g_pRenderContext);

	m_pCasterShaderAttr = i3ShaderAttr::NewObject();
	m_pCasterShaderAttr->SetShaderContext( pCtx);

	return true;
}

bool i3PerspectiveShadowMap::CreateCopyShader(void)
{
	i3ShaderCode * pCode;
	i3ShaderContext * pCtx;

	pCtx = i3ShaderContext::NewObjectRef();

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3ShaderDefine.inl");
	pCtx->AddCode( pCode);

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3CopySurface.fx");
	pCtx->AddCode( pCode);

	pCtx->SetVertexShaderName( "VS_NoSkin");
	pCtx->SetPixelShaderName( "PS_Def");

	m_pCopyShaderAttr = i3ShaderAttr::NewObject();
	m_pCopyShaderAttr->SetShaderContext( pCtx);

	return true;
}

void i3PerspectiveShadowMap::Create( INT32 w, INT32 h)
{
	I3G_IMAGE_FORMAT fmt, dfmt;
	COLOR col;

	fmt = I3G_IMAGE_FORMAT_R_32F;
	dfmt = I3G_IMAGE_FORMAT_D24X8;

	m_pStaticRTAttr = i3RenderTargetAttr::NewObject();
	m_pStaticRTAttr->CreateRenderTarget( w, h, fmt, dfmt);

	m_pShadowRTAttr = i3RenderTargetAttr::NewObject();
	m_pShadowRTAttr->CreateRenderTarget( w, h, fmt, dfmt);

	i3Color::Set( &col, 0, 0, 0, 0);
	//i3Color::Set( &col, 255, 255, 255, 255);
	m_pClearRTAttr = i3ClearRenderTargetAttr::NewObject();
	m_pClearRTAttr->SetAddClearMask( I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH );
	m_pClearRTAttr->SetClearZ( 1.0f);
	m_pClearRTAttr->SetClearColor( &col);

	m_pCasterRenderListAttr = i3RenderAttrListAttr::NewObject();

	CreateCasterShader();
	CreateCopyShader();

	m_pLightViewMatrixAttr = i3ViewMatrixAttr::NewObject();
	m_pLightProjectMatrixAttr = i3ProjectMatrixAttr::NewObject();

	m_pShadowMapEnableAttr = i3ShadowMapEnableAttr::NewObject();
	m_pShadowMapEnableAttr->Set( TRUE);

	{
		i3VertexArray * pVA;

		pVA = i3VertexArray::CreateRect( 2, 2, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE);

		m_pRectGeoAttr = i3GeometryAttr::NewObject();
		m_pRectGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2);
	}

	{
		m_pTexBindAttr = i3TextureBindAttr::NewObject();
		m_pTexBindAttr->SetTexture( m_pStaticRTAttr->GetColorRenderTarget());
		m_pTexBindAttr->SetBindType( I3G_TEXTURE_BIND_DIFFUSE);

		m_pZTestEnableAttr = i3ZTestEnableAttr::NewObject();
		m_pZTestEnableAttr->Set( FALSE);

		m_pFaceCullModeAttr = i3FaceCullModeAttr::NewObject();
		m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
	}
}

static INT32	s_EnableAttrTable[] =
{
	I3_ATTRID_GEOMETRY,
	I3_ATTRID_FACECULL_MODE,
	I3_ATTRID_VIEW_MATRIX,
	I3_ATTRID_PROJECT_MATRIX,
	I3_ATTRID_MODEL_VIEW_MATRIX,
	I3_ATTRID_BONEMATRIXLIST,
	I3_ATTRID_RENDER_ATTR_LIST,
	I3_ATTRID_QUAD,
	I3_ATTRID_SPRITE,
	I3_ATTRID_BILLBOARD,
	I3_ATTRID_TEXT,
	I3_ATTRID_SPRITE2D,
	I3_ATTRID_RENDERTARGET,
	I3_ATTRID_CLEARRENDERTARGET,
	I3_ATTRID_SHADER
};

//////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////
//
void i3PerspectiveShadowMap::CalcPostPerspectiveMatrix( i3SceneTracer * pTracer)
{
	VEC3D vOrigin[8], vFrusta[8], vTemp;
	MATRIX mVP, mTemp, mUniProj;
	UINT32 i;
	REAL32 minZ = 0.0f, maxZ = 0.0f;
	VEC3D mid;

	VLOG( "s ");

	// 원래의 Projection Tx 후에 Z는 (0~1)사이의 값을 가진다.
	// 이것은 Shadow Map의 사용성을 떨어뜨리기 때문에
	// (-1~1) 사이의 값으로 나오도록 Projection Matrix를 조절한다.
	i3Matrix::SetScale( &mTemp, 1.0f, 1.0f, 2.0f);
	i3Matrix::SetPos( &mTemp, 0.0f, 0.0f, -1.0f, 1.0f);

	i3Matrix::Mul( &mUniProj, pTracer->GetProjectionMatrix(), &mTemp);
	i3Matrix::Mul( &mVP, pTracer->GetViewMatrix(), &mUniProj);

	{
		//MATRIX mtxV2L;
		REAL32 fMapSz, fCutoff;
		MATRIX mTemp, mLView, mLProj;
		VEC3D vLightDir, vLightPos;

		{
			// Cv * Cp Space에서의 Light Direction을 구한다.
			i3Matrix::Mul( &mTemp, &m_mLightTx, &mVP);
			
			i3Vector::Normalize( &vLightDir, i3Matrix::GetAt( &mTemp));
			i3Vector::Scale( &vLightDir, &vLightDir, -1.0f);
		}

		// Bound Box를 Cv * Cp Tx한다.
		{
			m_BBox.GetCornerVertices( vOrigin);

			i3Matrix::TransformPoints( vFrusta, vOrigin, 8, &mVP);

			i3Vector::Set( &mid, 0.0f, 0.0f, 0.0f);
			for( i = 0; i < 8; i++)
			{
				i3Vector::Add( &mid, &mid, &vFrusta[i]);
			}

			// 중앙점
			i3Vector::Scale( &mid, &mid, 1.0f / 8.0f);

			// 중앙점에서 -Light Direction으로의 Light Position을 잡는다.
			// FOV( 45)로 잡아, 거리가 2 떨어진 거리에 Psuedo Light의 위치를 잡는다.
			i3Vector::Scale( &vTemp, &vLightDir, 3.0f);
			i3Vector::Add( &vLightPos, &vTemp, &mid);
		}

		// vLightPos에 위치하고, vLightDir 방향을 바라보는 mLView Matrix를 만든다.
		{
			i3Vector::Scale( &vTemp, &vLightDir, 1.0f);
			i3Matrix::Identity( &mLView);
			i3Matrix::MakeWithAt( &mLView, &vTemp, &I3_YAXIS);
			i3Matrix::SetPos( &mLView, &vLightPos);

			i3Matrix::Inverse( &mLView, NULL, &mLView);
		}

		// Light의 Projection Matrix를 생성한다.
		{
			// Cv * Cp * Lv
			i3Matrix::TransformPoints( vFrusta, vFrusta, 8, &mLView);	// Post-Perspective Light Space로...

			for( i = 0; i < 8; i++)
			{
				if( i == 0)
				{
					minZ = maxZ = ( i3Vector::GetZ( &vFrusta[i]));
				}
				else
				{
					minZ = MIN( minZ, ( i3Vector::GetZ( &vFrusta[i])));
					maxZ = MAX( maxZ, ( i3Vector::GetZ( &vFrusta[i])));
				}
			}

			i3Matrix::PerspectiveFOVRH( &mLProj, I3_DEG2RAD( 45.0f), 1.0f, 1.5f, 4.0f);	//minZ, maxZ);
		}

		VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[0]), i3Vector::GetY( &vFrusta[0]), i3Vector::GetZ( &vFrusta[0]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[1]), i3Vector::GetY( &vFrusta[1]), i3Vector::GetZ( &vFrusta[1]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[3]), i3Vector::GetY( &vFrusta[3]), i3Vector::GetZ( &vFrusta[3]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[2]), i3Vector::GetY( &vFrusta[2]), i3Vector::GetZ( &vFrusta[2]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[0]), i3Vector::GetY( &vFrusta[0]), i3Vector::GetZ( &vFrusta[0]));
																									
		VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[4]), i3Vector::GetY( &vFrusta[4]), i3Vector::GetZ( &vFrusta[4]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[5]), i3Vector::GetY( &vFrusta[5]), i3Vector::GetZ( &vFrusta[5]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[7]), i3Vector::GetY( &vFrusta[7]), i3Vector::GetZ( &vFrusta[7]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[6]), i3Vector::GetY( &vFrusta[6]), i3Vector::GetZ( &vFrusta[6]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[4]), i3Vector::GetY( &vFrusta[4]), i3Vector::GetZ( &vFrusta[4]));

		for( i = 0; i < 4; i++)
		{
			VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[i]),		i3Vector::GetY( &vFrusta[i]),		i3Vector::GetZ( &vFrusta[i]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[i + 4]), i3Vector::GetY( &vFrusta[i + 4]),	i3Vector::GetZ( &vFrusta[i + 4]));
		}

		VLOG( "c 255 0 0");
		VLOG( "m %f %f %f", i3Vector::GetX( &mid), i3Vector::GetY( &mid), i3Vector::GetZ( &mid));
		VLOG( "l %f %f %f", i3Vector::GetX( &vLightPos), i3Vector::GetY( &vLightPos), i3Vector::GetZ( &vLightPos));

		fCutoff = i3Math::cos( m_fCutOff);
		fMapSz = (REAL32) m_pShadowRTAttr->GetWidth();

		{
			i3Matrix::Mul( &mTemp, &mVP, &mLView);
			i3Matrix::Mul( &mTemp, &mTemp, &mLProj);

			// World 및 View Matrix는 Shader에서 4x3 Format으로 Projective Matrix를
			// 전달할 수 없기 때문에...
			m_pLightViewMatrixAttr->SetMatrix( &I3_IDENTITYMATRIX);			
			m_pLightProjectMatrixAttr->SetMatrix( &mTemp);
		}

		{
			i3Matrix::Mul( &mTemp, &mUniProj, &mLView);
			i3Matrix::Mul( &mTemp, &mTemp, &mLProj);

			pTracer->GetRenderContext()->AddShadowMap( m_pShadowRTAttr->GetColorRenderTarget(), 
				&mTemp, &I3_IDENTITYMATRIX, NULL, NULL,
				&m_ShadowColor, fMapSz, fCutoff);
		}
	}

	VLOG( "u");
}

void i3PerspectiveShadowMap::Apply( i3SceneTracer * pTracer)
{
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();
	I3SG_TRACER_CONTEXT ctx;

	CalcPostPerspectiveMatrix( pTracer);

	// Caster에 대한 Output을 구해온다.
	if( m_pShadowMapEnableAttr->Get())
	{
		// Scene Graph를 Trace하기 위한 준비
		pTracer->SaveContext( &ctx);
		pTracer->SetViewMatrix( m_pLightViewMatrixAttr->GetMatrix(), &m_mLightTx);
		pTracer->SetProjectionMatrix( m_pLightProjectMatrixAttr->GetMatrix());
		pTracer->GetModelViewMatrixStack()->PushIdentity();

		pTracer->PushOutputList( m_pCasterRenderListAttr);
		pTracer->SetTime( 0.0f);

		pManager->FlushPushedAttrs( pTracer->GetOutput());

		pManager->PushAttr( m_pLightViewMatrixAttr);
		pManager->PushAttr( m_pLightProjectMatrixAttr);
		pManager->PushAttr( m_pFaceCullModeAttr);
		pManager->PushAttr( m_pCasterShaderAttr, 0);		// 다른 Shader가 동작하지 못하도록...
		pManager->PushAttr( m_pShadowRTAttr);
		pManager->PushAttr( m_pClearRTAttr);

		// Dynamic Caster의 Render
		if( m_pCaster != NULL)
		{
			pTracer->Trace( m_pCaster);
		}

		// Tracer의 Restore
		pManager->PopAttr( m_pClearRTAttr->GetID());
		pManager->PopAttr( m_pShadowRTAttr->GetID());
		pManager->PopAttr( m_pFaceCullModeAttr->GetID());
		pManager->PopAttr( m_pCasterShaderAttr->GetID(), 0);		// 다른 Shader가 동작하지 못하도록...
		pManager->PopAttr( m_pLightViewMatrixAttr->GetID());
		pManager->PopAttr( m_pLightProjectMatrixAttr->GetID());

		pTracer->RestoreContext( &ctx);
		pTracer->PopOutputList();

		pTracer->AddAttributeToOutput( m_pCasterRenderListAttr);

		pTracer->GetModelViewMatrixStack()->Pop();
	}

	

	// Receiver의 Diffuse Render
	{
		pManager->FlushPushedAttrs( pTracer->GetOutput());
		pManager->PushAttr( m_pShadowMapEnableAttr);

		pTracer->Trace( m_pReceiver);

		pManager->PopAttr( m_pShadowMapEnableAttr->GetID());

		pManager->FlushPushedAttrs( pTracer->GetOutput());

		//pTracer->GetOutput()->Dump();
	}

	// Update Shader Params
}

void i3PerspectiveShadowMap::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

void i3PerspectiveShadowMap::OnBuildObjectList( i3List * pList)
{
}

UINT32 i3PerspectiveShadowMap::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3PerspectiveShadowMap::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}

void	i3PerspectiveShadowMap::TransformWorldToScreen( VEC3D * pOut, VEC3D * pVec)
{
	VEC4D result;

	i3Vector::Transform( &result, pVec, &m_matViewProjPrime);

	if (result.w < 0.001f) 
	{
		// Hm.
		i3Vector::Set( pOut, 0.0f, 0.0f, 0.0f);
	}

	REAL32	recip = i3Math::Reciprocal( i3Vector::GetW( &result));

	i3Vector::Scale( pOut, (VEC3D *) &result, recip);
}
