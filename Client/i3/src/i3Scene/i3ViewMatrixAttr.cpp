#include "i3SceneDef.h"
#include "i3ViewMatrixAttr.h"

I3_CLASS_INSTANCE( i3ViewMatrixAttr);

i3ViewMatrixAttr::i3ViewMatrixAttr(void)
{
	SetID( I3_ATTRID_VIEW_MATRIX);
}

void i3ViewMatrixAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetViewMatrix( &m_Matrix);
}

void i3ViewMatrixAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	i3Matrix::Copy( ((i3ViewMatrixAttr *)pObj)->GetMatrix(), GetMatrix());
}

bool i3ViewMatrixAttr::IsSame( i3RenderAttr * pAttr)
{
	return i3Matrix::IsSame( &m_Matrix, ((i3ViewMatrixAttr *) pAttr)->GetMatrix());
}

UINT32	i3ViewMatrixAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3::pack::MATRIX pMtx;

	pMtx.mat[0].x = m_Matrix.m[0][0];
	pMtx.mat[0].y = m_Matrix.m[0][1];
	pMtx.mat[0].z = m_Matrix.m[0][2];
	pMtx.mat[0].w = m_Matrix.m[0][3];

	pMtx.mat[1].x = m_Matrix.m[1][0];
	pMtx.mat[1].y = m_Matrix.m[1][1];
	pMtx.mat[1].z = m_Matrix.m[1][2];
	pMtx.mat[1].w = m_Matrix.m[1][3];

	pMtx.mat[2].x = m_Matrix.m[2][0];
	pMtx.mat[2].y = m_Matrix.m[2][1];
	pMtx.mat[2].z = m_Matrix.m[2][2];
	pMtx.mat[2].w = m_Matrix.m[2][3];

	pMtx.mat[3].x = m_Matrix.m[3][0];
	pMtx.mat[3].y = m_Matrix.m[3][1];
	pMtx.mat[3].z = m_Matrix.m[3][2];
	pMtx.mat[3].w = m_Matrix.m[3][3];

	Rc = pResFile->GetStream()->Write( &pMtx, sizeof(i3::pack::MATRIX));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ViewMatrixAttr::OnSave()", "Could not write matrix data.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32	i3ViewMatrixAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32	i3ViewMatrixAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	i3::pack::MATRIX pMtx;

	Rc = pResFile->GetStream()->Read( &pMtx, sizeof(i3::pack::MATRIX));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ViewMatrixAttr::OnLoad()", "Could not read matrix data.");
		return STREAM_ERR;
	}

	i3Matrix::SetRight( &m_Matrix,	pMtx.mat[0].x,
									pMtx.mat[0].y,
									pMtx.mat[0].z,
									pMtx.mat[0].w);

	i3Matrix::SetUp( &m_Matrix,		pMtx.mat[1].x, 
									pMtx.mat[1].y, 
									pMtx.mat[1].z, 
									pMtx.mat[1].w);

	i3Matrix::SetAt( &m_Matrix,		pMtx.mat[2].x,
									pMtx.mat[2].y, 
									pMtx.mat[2].z, 
									pMtx.mat[2].w);

	i3Matrix::SetPos( &m_Matrix,	pMtx.mat[3].x, 
									pMtx.mat[3].y, 
									pMtx.mat[3].z, 
									pMtx.mat[3].w);

	return Rc;
}

#if defined( I3_DEBUG)
void i3ViewMatrixAttr::Dump(void)
{
	I3TRACE( "%s : \n", meta()->class_name());

	i3Matrix::Dump( &m_Matrix);
}
#endif
