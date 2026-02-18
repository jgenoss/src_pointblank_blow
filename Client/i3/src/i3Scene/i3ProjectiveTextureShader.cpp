#include "i3SceneDef.h"
#include "i3ProjectiveTextureShader.h"

I3_CLASS_INSTANCE( i3ProjectiveTextureShader);

I3_TRACE_RESULT I3SG_NODE_PROC( i3ProjectiveTextureShader)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3ProjectiveTextureShader * pShader = (i3ProjectiveTextureShader *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	#if NODE_PROFILE
	START_PROFILE( 30);
	#endif

	pShader->Apply( pTracer);

	#if NODE_PROFILE
	END_PROFILE( 30);
	#endif

	return Result;
}

i3ProjectiveTextureShader::i3ProjectiveTextureShader(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3ProjectiveTextureShader));

	m_pTransformMatrixObj = i3MatrixObject::new_object();
	m_pInvTransformMatrixObj = i3MatrixObject::new_object();
	m_pProjectionMatrixObj = i3MatrixObject::new_object();
	
	{
		m_pTexBind = i3TextureBindAttr::new_object();
		I3ASSERT( m_pTexBind != nullptr);
	}

	{
		m_pBlendEnableAttr = i3BlendEnableAttr::new_object();
		I3ASSERT( m_pBlendEnableAttr != nullptr);

		m_pBlendEnableAttr->Set(true);
	}

	{
		m_pBlendModeAttr = i3BlendModeAttr::new_object();
		I3ASSERT( m_pBlendModeAttr != nullptr);
	}

	{
		m_pTexWrap = i3TextureWrapAttr::new_object();
		I3ASSERT( m_pTexWrap != nullptr);

		m_pTexWrap->SetHorzWrap( I3G_TEXTURE_WRAP_CLAMP);
		m_pTexWrap->SetVertWrap( I3G_TEXTURE_WRAP_CLAMP);
	}

	{
		m_pTexFunc = i3TextureFunctionAttr::new_object();
		I3ASSERT( m_pTexFunc != nullptr);
	}

	{
		m_pTextureMatrixAttr = i3TextureMatrixAttr::new_object();
		I3ASSERT( m_pTextureMatrixAttr);

		m_pTextureMatrixAttr->SetOutputElementCount( 4);
		m_pTextureMatrixAttr->SetProjectiveState(true);
	}

	{
		m_pTexCoordSourceAttr = i3TextureCoordSourceAttr::new_object();
		I3ASSERT( m_pTexCoordSourceAttr != nullptr);

		m_pTexCoordSourceAttr->SetMode( I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION);
	}

	{
		m_pTextureEnableAttr  = i3TextureEnableAttr::new_object();
		I3ASSERT( m_pTextureEnableAttr != nullptr);

		m_pTextureEnableAttr->Set(true);
	}

	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object();

		m_pLightingEnableAttr->Set(false);
	}

	{
		m_pZFuncAttr = i3ZFuncAttr::new_object();

		m_pZFuncAttr->Set( I3G_COMP_LEQUAL);
	}

	{
		m_pDispList = i3RenderAttrListAttr::new_object();
		I3ASSERT( m_pDispList != nullptr);
	}

	SetSrcBlend( I3G_BLEND_SRCALPHA);
	SetDestBlend( I3G_BLEND_INVSRCALPHA);
}

i3ProjectiveTextureShader::~i3ProjectiveTextureShader(void)
{
	I3_SAFE_RELEASE( m_pTransformMatrixObj);
	I3_SAFE_RELEASE( m_pInvTransformMatrixObj);
	I3_SAFE_RELEASE( m_pProjectionMatrixObj);
	I3_SAFE_RELEASE( m_pProjectTexture);

	I3_SAFE_RELEASE( m_pScene);

	I3_SAFE_RELEASE( m_pTexBind);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pTexWrap);
	I3_SAFE_RELEASE( m_pTexFunc);
	I3_SAFE_RELEASE( m_pTextureMatrixAttr);
	I3_SAFE_RELEASE( m_pDispList);
	I3_SAFE_RELEASE( m_pTexCoordSourceAttr);
	I3_SAFE_RELEASE( m_pTextureEnableAttr);
	I3_SAFE_RELEASE( m_pZFuncAttr);
	I3_SAFE_RELEASE( m_pLightingEnableAttr);
}

void i3ProjectiveTextureShader::SetSceneGraph( i3Node * pNode)
{
	I3_REF_CHANGE( m_pScene, pNode);
}

void i3ProjectiveTextureShader::SetTransformMatrix( MATRIX * pMatrix, MATRIX * pInvMatrix)
{
	m_pTransformMatrixObj->SetMatrix( pMatrix);

	if( pInvMatrix == nullptr)
	{
		i3Matrix::Inverse( m_pInvTransformMatrixObj->GetMatrix(), nullptr, pMatrix);
	}
	else
	{
		m_pInvTransformMatrixObj->SetMatrix( pInvMatrix);
	}
}

void i3ProjectiveTextureShader::SetTransformMatrixObject( i3MatrixObject * pMatObj, i3MatrixObject * pInvMatObj)
{
	I3_REF_CHANGE( m_pTransformMatrixObj, pMatObj);
	I3_REF_CHANGE( m_pInvTransformMatrixObj, pInvMatObj);
}

void i3ProjectiveTextureShader::SetProjectionMatrix( MATRIX * pMatrix)
{
	m_pProjectionMatrixObj->SetMatrix( pMatrix);
}

void i3ProjectiveTextureShader::SetProjectionMatrixObject( i3MatrixObject * pMatObj)
{
	I3_REF_CHANGE( m_pProjectionMatrixObj, pMatObj);
}

void i3ProjectiveTextureShader::SetProjectiveTexture( i3Texture * pTex)
{
	I3_REF_CHANGE( m_pProjectTexture, pTex);

	m_pTexBind->SetTexture( m_pProjectTexture);
}

void i3ProjectiveTextureShader::SetSrcBlend( I3G_BLEND blend)
{
	m_SrcBlend = blend;

	m_pBlendModeAttr->SetSource( m_SrcBlend);
}

void i3ProjectiveTextureShader::SetDestBlend( I3G_BLEND blend)
{
	m_DestBlend = blend;

	m_pBlendModeAttr->SetDestination( m_DestBlend);
}

void i3ProjectiveTextureShader::Apply( i3SceneTracer * pTracer)
{
	MATRIX OldViewMatrix, OldInvViewMatrix, OldProjectMatrix;
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();

	// 1-Pass
	//		* Diffuse Texture를 설정한다.
	//		* 일반적인 설정으로 해당 Scene Graph를 Rendering 한다.
	//
	// 2-Pass
	//		* Texture를 Projected Texture로 설정한다.
	//		* Texture Coord Source를 CAMERA_SPACE_POSITION으로 설정한다.
	//		* Texture Matrix를 아래와 같이 설정한다.
	//				  [Inverse View Matrix]
	//				* [Projector-Transform Matrix]
	//				* [Projection Matrix]
	//				* [Scale & Translate for Texture Coord. System]
	//		* Texture Wrapping Mode를 Clamp로 설정.
	//		* Blend Enable

	if( m_pScene == nullptr)
		return;

	i3Matrix::Copy( &OldViewMatrix, pTracer->GetViewMatrix());
	i3Matrix::Copy( &OldInvViewMatrix, pTracer->GetInverseViewMatrix());
	i3Matrix::Copy( &OldProjectMatrix, pTracer->GetProjectionMatrix());

	// Scene Graph를 Trace하기 위한 준비
	{
		pTracer->SetViewMatrix( m_pTransformMatrixObj->GetMatrix(), m_pInvTransformMatrixObj->GetMatrix());
		pTracer->SetProjectionMatrix( m_pProjectionMatrixObj->GetMatrix());

		pTracer->GetModelViewMatrixStack()->PushIdentity();

		pTracer->PushOutputList( m_pDispList);
	}

	{
		{
			MATRIX tmp, t;

			i3Matrix::Mul( &tmp, &OldInvViewMatrix, m_pTransformMatrixObj->GetMatrix());
			i3Matrix::Mul( &tmp, &tmp, m_pProjectionMatrixObj->GetMatrix());

			i3Matrix::SetScale( &t, 0.5f, -0.5f, 1.0f);
			i3Matrix::PostTranslate( &t, 0.5f, 0.5f, 0.0f); 

			i3Matrix::Mul( &tmp, &tmp, &t);

			m_pTextureMatrixAttr->SetMatrix( &tmp);
		}

		pManager->PushAttr( m_pTexBind,				I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pBlendEnableAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pBlendModeAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pTexWrap,				I3_DEF_ATTR_PRIORITY - 1);
		//pManager->PushAttr( m_pTexFunc,				I3_DEF_ATTR_PRIORITY + 1);
		pManager->PushAttr( m_pTexCoordSourceAttr,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pTextureMatrixAttr,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pTextureEnableAttr,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pLightingEnableAttr,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pZFuncAttr,			I3_DEF_ATTR_PRIORITY - 1);
	}

	// Traversal
	pTracer->Trace( m_pScene);


	// 원래의 Scene Tracer 상태로 복구
	{
		pManager->PopAttr( m_pTexBind->GetID(),				I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pBlendEnableAttr->GetID(),		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pBlendModeAttr->GetID(),		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pTexWrap->GetID(),				I3_DEF_ATTR_PRIORITY - 1);
		//pManager->PopAttr( m_pTexFunc,				I3_DEF_ATTR_PRIORITY + 1);
		pManager->PopAttr( m_pTexCoordSourceAttr->GetID(),	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pTextureMatrixAttr->GetID(),	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pTextureEnableAttr->GetID(),	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pLightingEnableAttr->GetID(),	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( m_pZFuncAttr->GetID(),			I3_DEF_ATTR_PRIORITY - 1);

		pTracer->PopOutputList();

		//m_pDispList->Dump();

		pTracer->GetModelViewMatrixStack()->Pop();

		pTracer->SetViewMatrix( &OldViewMatrix, &OldInvViewMatrix);
		pTracer->SetProjectionMatrix( &OldProjectMatrix);
	}

	pTracer->AddAttributeToOutput( m_pDispList);
}

void i3ProjectiveTextureShader::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pProjectTexture != nullptr)
	{
		m_pProjectTexture->OnBuildObjectList( List);
	}

	if( m_pScene != nullptr)
	{
		m_pScene->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PROJECTIVE_TEXTURE_SHADER_INFO_1
	{
		UINT8				m_ID[4] = { 'P', 'T', 'S', '1' };
		i3::pack::MATRIX	m_TransformMatrix;
		i3::pack::MATRIX	m_ProjectionMatrix;
		OBJREF				m_Texture = 0;
		OBJREF				m_Scene = 0;
		UINT8				m_SrcBlend = 0;
		UINT8				m_DestBlend = 0;
		UINT8				m_Reserved[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ProjectiveTextureShader::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::PROJECTIVE_TEXTURE_SHADER_INFO_1 Info;
	i3Matrix::Copy( &Info.m_TransformMatrix, m_pTransformMatrixObj->GetMatrix());
	i3Matrix::Copy( &Info.m_ProjectionMatrix, m_pProjectionMatrixObj->GetMatrix());
	Info.m_Texture = (OBJREF) pResFile->GetObjectPersistID( m_pProjectTexture);
	Info.m_Scene = (OBJREF) pResFile->GetObjectPersistID( m_pScene);
	Info.m_SrcBlend = (UINT8) i3Gfx::Convert( (I3G_BLEND) m_SrcBlend);
	Info.m_DestBlend = (UINT8) i3Gfx::Convert( (I3G_BLEND) m_DestBlend);

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveTextureShader::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3ProjectiveTextureShader::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ProjectiveTextureShader::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PROJECTIVE_TEXTURE_SHADER_INFO_1 Info;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveTextureShader::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// ID

	// Matrix
	i3Matrix::Copy( m_pTransformMatrixObj->GetMatrix(), &Info.m_TransformMatrix);
	SetTransformMatrix( m_pTransformMatrixObj->GetMatrix(), nullptr);

	i3Matrix::Copy( m_pProjectionMatrixObj->GetMatrix(), &Info.m_ProjectionMatrix);

	// Texture
	if( Info.m_Texture != (OBJREF)nullptr)
	{
		i3Texture * pTex;

		pTex = (i3Texture *) pResFile->FindObjectByID( Info.m_Texture);
		I3ASSERT( pTex != nullptr);

		SetProjectiveTexture( pTex);
	}

	// Scene
	if( Info.m_Scene != (OBJREF)nullptr)
	{
		i3Node * pNode;

		pNode = (i3Node *) pResFile->FindObjectByID( Info.m_Scene);
		I3ASSERT( pNode != nullptr);

		SetSceneGraph( pNode);
	}

	m_SrcBlend = i3Gfx::Convert( (I3G_PERSIST_BLEND) Info.m_SrcBlend);
	m_DestBlend = i3Gfx::Convert( (I3G_PERSIST_BLEND) Info.m_DestBlend);

	return Result;
}
