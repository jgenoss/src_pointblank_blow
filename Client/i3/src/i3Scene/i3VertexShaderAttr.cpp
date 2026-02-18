#include "i3SceneDef.h"
#include "i3VertexShaderAttr.h"

I3_CLASS_INSTANCE( i3VertexShaderAttr, i3RenderAttr);

i3VertexShaderAttr::i3VertexShaderAttr(void)
{
	SetID( I3_ATTRID_VERTEX_SHADER);

	m_pShader = NULL;
}

i3VertexShaderAttr::~i3VertexShaderAttr(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

void i3VertexShaderAttr::SetShader( i3VertexShader * pShader)
{
	if( pShader != NULL)
		pShader->AddRef();

	I3_SAFE_RELEASE( m_pShader);

	m_pShader = pShader;
}

void i3VertexShaderAttr::Apply( i3RenderContext * pContext)
{
	// NULL인 경우에는 Fixed-Pipeline을 사용하는 것으로 간주한다.
	pContext->SetVertexShader( m_pShader);
}

void i3VertexShaderAttr::OnBuildObjectList( i3List * pList)
{
	if( m_pShader != NULL)
		m_pShader->OnBuildObjectList( pList);

	i3RenderAttr::OnBuildObjectList( pList);
}

UINT32 i3VertexShaderAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	ref = (OBJREF) m_pShader;

	Rc = pStream->Write( &ref, sizeof(ref));
	if( Rc == STREAM_ERR)
	{
		I3WARN( "Could not write the reference to vertex shader.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Rc;
}

UINT32 i3VertexShaderAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	OBJREF ref;

	Rc = pStream->Read( &ref, sizeof(ref));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not read the reference to vertex shader.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( ref != 0)
	{
		i3VertexShader * pShader = (i3VertexShader *) pResFile->FindObjectByID( ref);
		I3ASSERT( pShader != NULL);

		SetShader( pShader);
	}

	return Result;
}
