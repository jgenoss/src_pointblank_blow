#include "i3SceneDef.h"
#include "i3ProjectiveShadowShader.h"

I3_CLASS_INSTANCE( i3ProjectiveShadowShader);

I3_TRACE_RESULT I3SG_NODE_PROC( i3ProjectiveShadowShader)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3ProjectiveShadowShader * pShader = (i3ProjectiveShadowShader *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	#if NODE_PROFILE
	START_PROFILE( 31);
	#endif

	pShader->Apply( pTracer);

	#if NODE_PROFILE
	END_PROFILE( 31);
	#endif

	return Result;
}

i3ProjectiveShadowShader::i3ProjectiveShadowShader(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3ProjectiveShadowShader));

	{
		m_pShader = i3ProjectiveTextureShader::new_object();
		I3ASSERT( m_pShader);

		m_pShader->SetSrcBlend( I3G_BLEND_SRCALPHA);
		m_pShader->SetDestBlend( I3G_BLEND_INVSRCALPHA);

		m_pViewMatrixObj = m_pShader->GetTransformMatrixObject();
		I3_SAFE_ADDREF( m_pViewMatrixObj);

		m_pInvViewMatrixObj = m_pShader->GetInverseTransformMatrixObject();
		I3_SAFE_ADDREF( m_pInvViewMatrixObj);

		m_pProjectMatrixObj = m_pShader->GetProjectionMatrixObject();
		I3_SAFE_ADDREF( m_pProjectMatrixObj);
	}

	{
		m_pRenderTargetAttr = i3RenderTargetAttr::new_object();
		m_pRenderTargetAttr->SetWidth( 256);
		m_pRenderTargetAttr->SetHeight( 256);
		m_pRenderTargetAttr->SetFormat( I3G_IMAGE_FORMAT_DEFAULT_COLOR);
		m_pRenderTargetAttr->SetMode( I3SG_RENDERTARGET_MODE_CREATE);

		m_pRenderTargetAttr->CreateRenderTarget();

		m_pShader->SetProjectiveTexture( m_pRenderTargetAttr->GetColorRenderTarget());
	}

	{
		COLOR col;

		m_pClearRenderTargetAttr = i3ClearRenderTargetAttr::new_object();

		i3Color::Set( &col, (UINT8) 0, 0, 0, 0);

		m_pClearRenderTargetAttr->SetClearColor( &col);
		m_pClearRenderTargetAttr->SetAddClearMask( I3G_CLEAR_COLOR);
		m_pClearRenderTargetAttr->SetRemoveClearMask( I3G_CLEAR_STENCIL | I3G_CLEAR_DEPTH);
	}

	{
		COLORREAL col;

		i3Color::Set( &col, 0.0f, 0.0f, 0.0f, 0.5f);

		m_pColorAttr = i3ColorAttr::new_object();
		m_pColorAttr->Set( &col);
	}

	{
		m_pBlendModeAttr = i3BlendModeAttr::new_object();

		m_pBlendModeAttr->SetSource( I3G_BLEND_ONE);
		m_pBlendModeAttr->SetDestination( I3G_BLEND_ZERO);
	}

	{
		m_pBlendEnableAttr = i3BlendEnableAttr::new_object();
		m_pBlendEnableAttr->Set(false);
	}

	{
		m_pTextureEnableAttr = i3TextureEnableAttr::new_object();
		m_pTextureEnableAttr->Set(false);
	}

	{
		m_pLightingEnableAttr = i3LightingEnableAttr::new_object();
		m_pLightingEnableAttr->Set(false);
	}

	{
		m_pProjectMatrixAttr = i3ProjectMatrixAttr::new_object();
	}

	{
		m_pViewMatrixAttr = i3ViewMatrixAttr::new_object();
	}

	{
		m_pZWriteEnableAttr = i3ZWriteEnableAttr::new_object();
		m_pZWriteEnableAttr->Set(false);
	}

	{
		m_pZTestEnableAttr = i3ZTestEnableAttr::new_object();
		m_pZTestEnableAttr->Set(false);
	}

	{
		m_pDispList = i3RenderAttrListAttr::new_object();
	}
}

i3ProjectiveShadowShader::~i3ProjectiveShadowShader(void)
{
	I3_SAFE_RELEASE( m_pViewMatrixObj);
	I3_SAFE_RELEASE( m_pInvViewMatrixObj);
	I3_SAFE_RELEASE( m_pProjectMatrixObj);

	I3_SAFE_RELEASE( m_pCaster);
	I3_SAFE_RELEASE( m_pShader);
	I3_SAFE_RELEASE( m_pRenderTargetAttr);
	I3_SAFE_RELEASE( m_pClearRenderTargetAttr);
	I3_SAFE_RELEASE( m_pBlendModeAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pTextureEnableAttr);
	I3_SAFE_RELEASE( m_pLightingEnableAttr);
	I3_SAFE_RELEASE( m_pProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pViewMatrixAttr);
	I3_SAFE_RELEASE( m_pZWriteEnableAttr);
	I3_SAFE_RELEASE( m_pZTestEnableAttr);
	I3_SAFE_RELEASE( m_pColorAttr);
	I3_SAFE_RELEASE( m_pDispList);
}

void i3ProjectiveShadowShader::SetCaster( i3Node * pScene)
{
	I3_REF_CHANGE( m_pCaster, pScene);
}

void i3ProjectiveShadowShader::SetLightTransform( MATRIX * pMatrix, MATRIX * pInvMatrix)
{
	m_pInvViewMatrixObj->SetMatrix( pMatrix);

	if( pInvMatrix == nullptr)
	{
		i3Matrix::Inverse( m_pViewMatrixObj->GetMatrix(), nullptr, pMatrix);
	}
	else
	{
		m_pViewMatrixObj->SetMatrix( pInvMatrix);
	}

	m_pViewMatrixAttr->SetMatrix( m_pViewMatrixObj->GetMatrix());
}

void i3ProjectiveShadowShader::UpdateProjectMatrix(void)
{
	i3Matrix::PerspectiveFOVRH( m_pProjectMatrixAttr->GetMatrix(), m_CutoffAngle, 
					1.0f, 1.0f, m_Range);

	m_pShader->SetProjectionMatrix( m_pProjectMatrixAttr->GetMatrix());
}

void i3ProjectiveShadowShader::SetLightRange( REAL32 val)
{
	m_Range = val;

	UpdateProjectMatrix(); 
}

void i3ProjectiveShadowShader::SetCutoffAngle( REAL32 val)
{
	m_CutoffAngle = val;

	UpdateProjectMatrix();
}

//
// 1. 새로운 Render Target을 설정하고, clear 한다. 이때, Depth Buffer는 원래의 것을 그대로 사용한다.
// 3. 아래와 같이 Render State를 설정한다.
//		- Lighting : false
//		- Texture : false
//		- Blend : true
//		- Source Blend : ONE
//		- Dest Blend : false
//		- COLOR : Gray( 0.5, 0.5, 0.5, 0.5)
// 4. Caster를 Rendering 한다.
// 2. Light-View에서의 View Matrix와 Project Matrix를 설정한다.
// 4. Soft edge를 위해서....반복 (Source : SrcALPHA, Dest : INVSrcALPHA)
// 5. 원래의 Render Target으로 되돌린다.
// 6. Rendering이 끝난 Texture(Render Target)을 Projective Texture Shader에 Projected Texture로 설정한다.
// 7. Project Matrix와 View Matrix를 Projective Texture Shader에 그대로 설정한다.
// 8. Projective Texture Shader에 아래와 같이 설정한다.
//		- Source Blend : SRCALPHA
//		- Dest Blend : INVSRCALPHA
// 8. Scene을 Traversal한다.
// 9. 
void i3ProjectiveShadowShader::Apply( i3SceneTracer * pTracer)
{
	MATRIX OldViewMatrix, OldInvViewMatrix, OldProjectMatrix;
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();

	i3Matrix::Copy( &OldViewMatrix, pTracer->GetViewMatrix());
	i3Matrix::Copy( &OldInvViewMatrix, pTracer->GetInverseViewMatrix());
	i3Matrix::Copy( &OldProjectMatrix, pTracer->GetProjectionMatrix());

	// Scene Graph를 Trace하기 위한 준비
	{
		pTracer->SetViewMatrix( m_pViewMatrixObj->GetMatrix(), m_pInvViewMatrixObj->GetMatrix());
		pTracer->SetProjectionMatrix( m_pProjectMatrixObj->GetMatrix());

		pTracer->GetModelViewMatrixStack()->PushIdentity();

		pTracer->PushOutputList( m_pDispList);
	}

	{
		pManager->PushAttr( m_pRenderTargetAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pClearRenderTargetAttr,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pColorAttr,				I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pBlendModeAttr,			I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pBlendEnableAttr,			I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pTextureEnableAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pLightingEnableAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pProjectMatrixAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pViewMatrixAttr,			I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pZWriteEnableAttr,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PushAttr( m_pZTestEnableAttr,			I3_DEF_ATTR_PRIORITY - 1);
	}
 
	// Traversal
	pTracer->Trace( m_pCaster);


	// 원래의 Scene Tracer 상태로 복구
	{
		pManager->PopAttr( I3_ATTRID_RENDERTARGET,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_CLEARRENDERTARGET,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_COLOR,				I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_BLEND_MODE,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_BLEND_ENABLE,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_TEXTURE_ENABLE,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_LIGHTING_ENABLE,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_PROJECT_MATRIX,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_VIEW_MATRIX,		I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_Z_WRITE_ENABLE,	I3_DEF_ATTR_PRIORITY - 1);
		pManager->PopAttr( I3_ATTRID_Z_TEST_ENABLE,		I3_DEF_ATTR_PRIORITY - 1);

		pTracer->PopOutputList();

		//m_pDispList->Dump();

		pTracer->GetModelViewMatrixStack()->Pop();

		pTracer->SetViewMatrix( &OldViewMatrix, &OldInvViewMatrix);
		pTracer->SetProjectionMatrix( &OldProjectMatrix);
	}

	pTracer->AddAttributeToOutput( m_pDispList);

	m_pShader->Apply( pTracer);
}

void i3ProjectiveShadowShader::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pCaster != nullptr)
	{
		m_pCaster->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED PROJECTIVE_SHADOW_SHADER
	{
		char				m_ID[4] = { 'P', 'S', 'S', '1' };
		OBJREF				m_Caster = 0;
		UINT32				m_TexRes = 0;
		REAL32				m_Range = 0.0f;
		REAL32				m_CutoffAngle = 0.0f;
		UINT32				m_Reserved[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ProjectiveShadowShader::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PROJECTIVE_SHADOW_SHADER Info;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	Info.m_Caster = (OBJREF) pResFile->GetObjectPersistID( m_pCaster);
	Info.m_TexRes = GetShadowTextureRes();
	Info.m_Range = GetLightRange();
	Info.m_CutoffAngle = GetCutoffAngle();

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Projective Texture Shader
	Rc = m_pShader->OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnSave()", "Could not write texture shader info.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Light Transform
	Rc = m_pViewMatrixAttr->OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnSave()", "Could not write light transform.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Color
	Rc = m_pColorAttr->OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnSave()", "Could not write color.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3ProjectiveShadowShader::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ProjectiveShadowShader::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::PROJECTIVE_SHADOW_SHADER Info;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnSave()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( Info.m_Caster != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( Info.m_Caster);
		I3ASSERT( pNode != nullptr);

		SetCaster( pNode);
	}
	
	SetShadowTextureWidth( Info.m_TexRes);

	SetLightRange( Info.m_Range);
	SetCutoffAngle( Info.m_CutoffAngle);

	// Projective Texture Shader
	Rc = m_pShader->OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnLoad()", "Could not read texture shader info.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Light Transform
	Rc = m_pViewMatrixAttr->OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnLoad()", "Could not read light transform.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Color
	Rc = m_pColorAttr->OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ProjectiveShadowShader::OnLoad()", "Could not read color.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}
