#include "i3SceneDef.h"
#include "i3RegVEC3D.h"
#include "i3RegData.h"

I3_CLASS_INSTANCE( i3RegVEC3D);

i3RegVEC3D::i3RegVEC3D(void)
{
	m_nDataType = I3REG_TYPE_VEC3D;
}

i3RegVEC3D::i3RegVEC3D( REAL32 x, REAL32 y, REAL32 z)
{
	m_nDataType = I3REG_TYPE_VEC3D;
	i3Vector::Set( &m_Value, x, y, z);
}

i3RegVEC3D::i3RegVEC3D( VEC3D * pVec)
{
	m_nDataType = I3REG_TYPE_VEC3D;
	i3Vector::Copy( &m_Value, pVec);
}

void i3RegVEC3D::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegVEC3D*>(pDest)); 

	i3RegVEC3D * pVec3 = ( i3RegVEC3D*)pDest;

	pVec3->set( *this);
}

void	i3RegVEC3D::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegVEC3D::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;
	i3::pack::VEC3D	data;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	{
		data.x = m_Value.x;
		data.y = m_Value.y;
		data.z = m_Value.z;
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegVEC3D::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::VEC3D data;
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Vector::Set( &m_Value, data.x, data.y, data.z);

	return Result;
}

void i3RegVEC3D::GetValueByString( i3::wstring& out)
{
	i3::sprintf(out, L"%f, %f, %f", m_Value.x, m_Value.y, m_Value.z);
}