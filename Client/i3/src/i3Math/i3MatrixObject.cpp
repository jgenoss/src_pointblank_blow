#include "i3MathType.h"
#include "i3MatrixObject.h"

I3_CLASS_INSTANCE( i3MatrixObject);

void i3MatrixObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	if( i3::kind_of<i3MatrixObject*>(pDest))
	{
		i3MatrixObject * pObj = (i3MatrixObject *) pDest;

		pObj->SetMatrix( &m_Matrix);
	}
}

UINT32 i3MatrixObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3::pack::MATRIX pmtx;

	i3Matrix::Copy( &pmtx, &m_Matrix);

	Rc = pResFile->GetStream()->Write( &pmtx, sizeof(i3::pack::MATRIX));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3MatrixObject::OnSave()", "Could not write matrix data.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3MatrixObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3::pack::MATRIX pmtx;

	Rc = pResFile->GetStream()->Read( &pmtx, sizeof(i3::pack::MATRIX));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3MatrixObject::OnLoad()", "Could not read matrix data.");
		return STREAM_ERR;
	}

	i3Matrix::Copy( &m_Matrix, &pmtx);

	return Rc;
}

// ================================================================================================
//
// i3 Matrix Object Proxy
//
// ================================================================================================
I3_CLASS_INSTANCE( i3MatrixObjectProxy);

void i3MatrixObjectProxy::SetLocalOffsetTransform( MATRIX *pMat )
{
	if( pMat == nullptr)
	{
		m_bLocalOffset = false;
		i3Matrix::Identity( &m_LocalOffsetTx );
	}
	else
	{
		m_bLocalOffset = true;
		i3Matrix::Copy( &m_LocalOffsetTx, pMat );
	}
}

MATRIX *i3MatrixObjectProxy::GetMatrix(void)
{
	if( m_pSourceMatrix )
	{
		i3Matrix::Copy( &m_Matrix, m_pSourceMatrix );
	}

	if( m_bLocalOffset)
	{
		i3Matrix::Mul( &m_Matrix, &m_LocalOffsetTx, &m_Matrix);
	}

	if( m_pSourcePos != nullptr )
	{
		i3Matrix::PostTranslate( &m_Matrix, m_pSourcePos );
	}

	return &m_Matrix;
}
