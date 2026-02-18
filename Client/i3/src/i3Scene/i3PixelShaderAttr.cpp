#include "i3SceneDef.h"
#include "i3PixelShaderAttr.h"

I3_CLASS_INSTANCE( i3PixelShaderAttr, i3RenderAttr);

i3PixelShaderAttr::i3PixelShaderAttr(void)
{
	SetID( I3_ATTRID_PIXEL_SHADER);

	m_pShader = NULL;
}

i3PixelShaderAttr::~i3PixelShaderAttr(void)
{
	I3_SAFE_RELEASE( m_pShader);
}

void i3PixelShaderAttr::SetShader( i3PixelShader * pShader)
{
	if( pShader != NULL)
		pShader->AddRef();

	I3_SAFE_RELEASE( m_pShader);

	m_pShader = pShader;
}

void i3PixelShaderAttr::Apply( i3RenderContext * pContext)
{
	// NULL인 경우에는 Fixed-Pipeline을 사용하는 것으로 간주한다.
	pContext->SetPixelShader( m_pShader);
}

void i3PixelShaderAttr::OnBuildObjectList( i3List * pList)
{
	if( m_pShader != NULL)
		m_pShader->OnBuildObjectList( pList);

	i3RenderAttr::OnBuildObjectList( pList);
}

UINT32 i3PixelShaderAttr::OnSave( i3ResourceFile * pResFile)
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

UINT32 i3PixelShaderAttr::OnLoad( i3ResourceFile * pResFile)
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
		i3PixelShader * pShader = (i3PixelShader *) pResFile->FindObjectByID( ref);
		I3ASSERT( pShader != NULL);

		SetShader( pShader);
	}

	return Result;
}
