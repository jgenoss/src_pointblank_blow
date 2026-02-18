#include "i3SceneDef.h"
#include "i3RegMATRIX.h"

I3_CLASS_INSTANCE( i3RegMATRIX);

i3RegMATRIX::i3RegMATRIX(void)
{
	m_nDataType = I3REG_TYPE_MATRIX;
}

i3RegMATRIX::i3RegMATRIX( MATRIX * pMtx)
{
	m_nDataType = I3REG_TYPE_MATRIX;

	i3Matrix::Copy( &m_Value, pMtx);
}

void i3RegMATRIX::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegMATRIX*>(pDest));

	i3RegMATRIX * pMatrix = ( i3RegMATRIX*)pDest;

	pMatrix->set( *this);
}

void	i3RegMATRIX::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegMATRIX::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;
	i3::pack::MATRIX	data;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	{
		data.mat[0].x = m_Value.m[0][0];
		data.mat[0].y = m_Value.m[0][1];
		data.mat[0].z = m_Value.m[0][2];
		data.mat[0].w = m_Value.m[0][3];

		data.mat[1].x = m_Value.m[1][0];
		data.mat[1].y = m_Value.m[1][1];
		data.mat[1].z = m_Value.m[1][2];
		data.mat[1].w = m_Value.m[1][3];

		data.mat[2].x = m_Value.m[2][0];
		data.mat[2].y = m_Value.m[2][1];
		data.mat[2].z = m_Value.m[2][2];
		data.mat[2].w = m_Value.m[2][3];

		data.mat[3].x = m_Value.m[3][0];
		data.mat[3].y = m_Value.m[3][1];
		data.mat[3].z = m_Value.m[3][2];
		data.mat[3].w = m_Value.m[3][3];
	}

	Rc = pStream->Write( &data, sizeof( i3::pack::MATRIX));
	I3_CHKIO( Rc);
	Result += Rc;
	
	return Result;
}

UINT32	i3RegMATRIX::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::MATRIX data;
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &data, sizeof( i3::pack::MATRIX));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Matrix::SetRight( &m_Value,	data.mat[0].x,
									data.mat[0].y,
									data.mat[0].z,
									data.mat[0].w);

	i3Matrix::SetUp( &m_Value,		data.mat[1].x, 
									data.mat[1].y, 
									data.mat[1].z, 
									data.mat[1].w);

	i3Matrix::SetAt( &m_Value,		data.mat[2].x,
									data.mat[2].y, 
									data.mat[2].z, 
									data.mat[2].w);

	i3Matrix::SetPos( &m_Value,		data.mat[3].x, 
									data.mat[3].y, 
									data.mat[3].z, 
									data.mat[3].w);

	return Result;
}

void i3RegMATRIX::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", m_Value.m00, m_Value.m01, m_Value.m02, m_Value.m03
																					, m_Value.m10, m_Value.m11, m_Value.m12, m_Value.m13
																					, m_Value.m20, m_Value.m21, m_Value.m22, m_Value.m23
																					, m_Value.m30, m_Value.m31, m_Value.m32, m_Value.m33);
}