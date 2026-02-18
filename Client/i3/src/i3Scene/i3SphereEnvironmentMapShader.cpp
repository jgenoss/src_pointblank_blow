#include "i3SceneDef.h"
#include "i3SphereEnvironmentMapShader.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3SphereEnvironmentMapShader, i3Node);

I3_TRACE_RESULT I3SG_NODE_PROC( i3SphereEnvironmentMapShader)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3SphereEnvironmentMapShader * pShader = (i3SphereEnvironmentMapShader *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

#if NODE_PROFILE
	START_PROFILE( 28);
#endif

	pTracer->PushOutputList( pShader->GetRenderAttrList());

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	pTracer->PopOutputList();

	pShader->Apply( pTracer);

#if NODE_PROFILE
	END_PROFILE( 28);
#endif

	return Result;
}

i3SphereEnvironmentMapShader::i3SphereEnvironmentMapShader(void)
:	m_pSphereTexBind( NULL),
	m_pGlossTexBind( NULL),
	m_pNewOutput( NULL),
	m_pTexCoordSource( NULL),
	m_pTexFunc( NULL),
	m_pTexMatrix( NULL),
	m_pTexFilter( NULL),
	m_pSphereTex( NULL),
	m_pGlossTex( NULL),
	m_ReflectionAmount( 0.5f)
{
	_setTraceProc( I3SG_NODE_PROC( i3SphereEnvironmentMapShader));

	m_pNewOutput = i3RenderAttrListAttr::NewObject();
	I3ASSERT( m_pNewOutput != NULL);

	// Texture Coordinates Source
	{
		m_pTexCoordSource = i3TextureCoordSourceAttr::NewObject();
		I3ASSERT( m_pTexCoordSource != NULL);
		m_pTexCoordSource->SetMode( I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL);
	}

	// Texture Function
	{
		m_pTexFunc = i3TextureFunctionAttr::NewObject();
		I3ASSERT( m_pTexFunc != NULL);

		m_pTexFunc->SetFunction( I3G_TEXTURE_FUNC_ADD);
	}

	// Texture Filter
	{
		m_pTexFilter = i3TextureFilterAttr::NewObject();

		m_pTexFilter->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pTexFilter->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
	}

	// Texture Matrix
	{
		RT_MATRIX tmx;

		m_pTexMatrix = i3TextureMatrixAttr::NewObject();
		I3ASSERT( m_pTexMatrix != NULL);

		{
			i3Matrix::SetRight(		&tmx, __RT(0.5f), __RT(0.0f), __RT(0.0f), __RT(0.0f));
			i3Matrix::SetUp(		&tmx, __RT(0.0f), -__RT(0.5f), __RT(0.0f), __RT(0.0f));
			i3Matrix::SetAt(		&tmx, __RT(0.0f), __RT(0.0f), __RT(0.0f), __RT(0.0f));
			i3Matrix::SetPos(		&tmx, __RT(0.5f), __RT(0.5f), __RT(0.0f), __RT(0.0f));
		}

		m_pTexMatrix->SetOutputElementCount( 2);
		m_pTexMatrix->SetMatrix( &tmx);
	}
}

i3SphereEnvironmentMapShader::~i3SphereEnvironmentMapShader(void)
{
	if( m_pTexCoordSource != NULL)
	{
		m_pTexCoordSource->Release();
		m_pTexCoordSource = NULL;
	}

	if( m_pTexFunc != NULL)
	{
		m_pTexFunc->Release();
		m_pTexFunc = NULL;
	}

	if( m_pTexFilter != NULL)
	{
		m_pTexFilter->Release();
		m_pTexFilter = NULL;
	}

	if( m_pTexMatrix != NULL)
	{
		m_pTexMatrix->Release();
		m_pTexMatrix = NULL;
	}

	if( m_pNewOutput != NULL)
	{
		m_pNewOutput->Release();
		m_pNewOutput = NULL;
	}

	if( m_pSphereTexBind != NULL)
	{
		m_pSphereTexBind->Release();
		m_pSphereTexBind = NULL;
	}

	if( m_pGlossTexBind != NULL)
	{
		m_pGlossTexBind->Release();
		m_pGlossTexBind = NULL;
	}

	if( m_pSphereTex != NULL)
	{
		m_pSphereTex->Release();
		m_pSphereTex = NULL;
	}

	if( m_pGlossTex != NULL)
	{
		m_pGlossTex->Release();
		m_pGlossTex = NULL;
	}
}

void i3SphereEnvironmentMapShader::SetSphereTexture( i3Texture * pTex)
{
	if( m_pSphereTexBind == NULL)
	{
		m_pSphereTexBind = i3TextureBindAttr::NewObject();
		m_pSphereTexBind->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	}

	if( m_pSphereTex != NULL)
	{
		m_pSphereTex->Release();
		m_pSphereTex = NULL;
	}

	m_pSphereTex = pTex;

	m_pSphereTexBind->SetTexture( m_pSphereTex);

	if( m_pSphereTex != NULL)
	{
		m_pSphereTex->AddRef();
	}
}

void i3SphereEnvironmentMapShader::SetGlossTexture( i3Texture * pTex)
{
	if( m_pGlossTexBind == NULL)
	{
		m_pGlossTexBind = i3TextureBindAttr::NewObject();
		m_pGlossTexBind->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	}

	if( m_pGlossTex != NULL)
	{
		m_pGlossTex->Release();
		m_pGlossTex = NULL;
	}

	m_pGlossTex = pTex;

	m_pGlossTexBind->SetTexture( m_pGlossTex);

	if( m_pGlossTex != NULL)
	{
		m_pGlossTex->AddRef();
	}
}

void i3SphereEnvironmentMapShader::Apply( i3SceneTracer * pTracer)
{
	// 현재 Texture Stage를 1로 고정하여 사용하고 있으나,
	// 현실적으로는 Multi-Texture Shader 등과의 기능 혼합을 위해, Texture Stage 예약을 통해
	// 자신이 사용할 Stage Number를 부여받아야 한다.
	m_pTexCoordSource->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	m_pTexFunc->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	m_pTexFilter->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	m_pTexMatrix->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);
	m_pSphereTexBind->SetBindType( I3G_TEXTURE_BIND_ENVIRONMENT);

	pTracer->AddAttributeToOutput( m_pTexCoordSource);
	pTracer->AddAttributeToOutput( m_pTexFunc);
	pTracer->AddAttributeToOutput( m_pTexMatrix);
	pTracer->AddAttributeToOutput( m_pTexFilter);
	pTracer->AddAttributeToOutput( m_pSphereTexBind);

	// 미리 Traversal을 돌아, 구성해둔 Child Scene의 Output을 기존 Output에 포함시킨다.
	pTracer->AddAttributeToOutput( m_pNewOutput);
}

void i3SphereEnvironmentMapShader::CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method)
{
	i3SphereEnvironmentMapShader * pShader = (i3SphereEnvironmentMapShader *) pNode;

	i3Node::CopyTo( pNode, method);

	pShader->SetSphereTexture( GetSphereTexture());
	pShader->SetGlossTexture( GetGlossTexture());
	pShader->SetReflectionAmount( GetReflectionAmount());
}

void i3SphereEnvironmentMapShader::OnBuildObjectList( i3List * pList)
{
	if( m_pSphereTex != NULL)
	{
		m_pSphereTex->OnBuildObjectList( pList);
	}

	if( m_pGlossTex != NULL)
	{
		m_pGlossTex->OnBuildObjectList( pList);
	}

	i3Node::OnBuildObjectList( pList);
}

UINT32 i3SphereEnvironmentMapShader::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF objref;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Sphere Texture
	{
		objref = (OBJREF) pResFile->GetObjectPersistID( m_pSphereTex);

		Rc = pStream->Write( &objref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write sphere map source reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Gloss Texture
	{
		objref = (OBJREF) pResFile->GetObjectPersistID( m_pGlossTex);

		Rc = pStream->Write( &objref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write gloss map source reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Reflection Rate
	{
		Rc = pStream->Write( &m_ReflectionAmount, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write reflection amount data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3SphereEnvironmentMapShader::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3SphereEnvironmentMapShader::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF objref;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Sphere Texture
	{
		i3Texture * pTex;

		Rc = pStream->Read( &objref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write sphere map source reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( objref != 0)
		{
			pTex = (i3Texture *) pResFile->FindObjectByID( (UINT32) objref);
			I3ASSERT( pTex != NULL);

			SetSphereTexture( pTex);
		}
	}

	// Gloss Texture
	{
		i3Texture * pTex;

		Rc = pStream->Read( &objref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write gloss map source reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( objref != 0)
		{
			pTex = (i3Texture *) pResFile->FindObjectByID( (UINT32) objref);
			I3ASSERT( pTex != NULL);

			SetGlossTexture( pTex);
		}
	}

	// Reflection Rate
	{
		Rc = pStream->Read( &m_ReflectionAmount, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SphereEnvironmentMapShader::OnSave()", "Could not write reflection amount data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}
