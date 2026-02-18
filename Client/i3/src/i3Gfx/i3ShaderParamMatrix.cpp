#include "i3GfxType.h"
#include "i3ShaderParamMatrix.h"
#include "i3RenderContext.h"

I3_CLASS_INSTANCE( i3ShaderParamMatrix);

i3ShaderParamMatrix::i3ShaderParamMatrix(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);
}

void i3ShaderParamMatrix::SetType( TYPE type)
{
	UINT32 mask;

	m_Type = type;

	switch( type)
	{
		default				 :		mask = I3G_SHADER_PMASK_ALL;	break;
		case TYPE_WORLD :			mask = I3G_SHADER_PMASK_MATRIX_WORLD;	break;

		case TYPE_INVVIEW :	
		case TYPE_VIEW :			mask = I3G_SHADER_PMASK_MATRIX_VIEW;	break;

		case TYPE_PROJECT :			mask = I3G_SHADER_PMASK_MATRIX_PROJECT;	break;
	}

	setUpdateMask( mask);
}

void i3ShaderParamMatrix::Update( i3RenderContext * pCtx)
{
	MATRIX mtx, mtx2;

	switch( m_Type)
	{
		case TYPE_WORLD :
			i3Matrix::Mul( &mtx, pCtx->GetWorldMatrix(), &m_Matrix);
			i3ShaderParam::Update( &mtx);	
			break;

		case TYPE_VIEW :
			i3Matrix::Mul( &mtx, pCtx->GetViewMatrix(), &m_Matrix);
			i3ShaderParam::Update( &mtx);	
			break;

		case TYPE_INVVIEW :
			i3Matrix::Inverse( &mtx2, nullptr, pCtx->GetViewMatrix());
			i3Matrix::Mul( &mtx, &mtx2, &m_Matrix);
			i3ShaderParam::Update( &mtx);	
			break;

		case TYPE_PROJECT :
			i3Matrix::Mul( &mtx, pCtx->GetProjectMatrix(), &m_Matrix);
			i3ShaderParam::Update( &mtx);	
			break;

		default :
			i3ShaderParam::Update( &m_Matrix);	
			break;
	}
}

UINT32 i3ShaderParamMatrix::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamMatrix::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
