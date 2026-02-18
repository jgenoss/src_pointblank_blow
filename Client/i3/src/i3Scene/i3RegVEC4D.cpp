#include "i3SceneDef.h"
#include "i3RegVEC4D.h"

I3_CLASS_INSTANCE( i3RegVEC4D);

i3RegVEC4D::i3RegVEC4D(void)
{
	m_nDataType = I3REG_TYPE_VEC4D;
}

i3RegVEC4D::i3RegVEC4D( REAL32 x, REAL32 y, REAL32 z, REAL32 w)
{
	m_nDataType = I3REG_TYPE_VEC4D;
	i3Vector::Set( &m_Value, x, y, z, w);
}

i3RegVEC4D::i3RegVEC4D( VEC4D * pVec)
{
	m_nDataType = I3REG_TYPE_VEC4D;
	i3Vector::Copy( &m_Value, pVec);
}

i3RegVEC4D::i3RegVEC4D( i3RegVEC4D * pVec)
{
	m_nDataType = I3REG_TYPE_VEC4D;
	i3Vector::Copy( &m_Value, pVec->getValue());
}


void i3RegVEC4D::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegVEC4D*>(pDest)); 

	i3RegVEC4D * pVec4 = ( i3RegVEC4D*)pDest;

	pVec4->set( *this);
}

void	i3RegVEC4D::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegVEC4D::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;
	i3::pack::VEC4D	data;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	{
		data.x = m_Value.x;
		data.y = m_Value.y;
		data.z = m_Value.z;
		data.w = m_Value.w;
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegVEC4D::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::VEC4D data;
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Vector::Set( &m_Value, data.x, data.y, data.z, data.w);

	return Result;
}

void i3RegVEC4D::GetValueByString( i3::wstring& out)
{
	i3::sprintf(out, L"%f, %f, %f, %f", m_Value.x, m_Value.y, m_Value.z, m_Value.w);
}