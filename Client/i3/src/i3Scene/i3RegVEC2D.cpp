#include "i3SceneDef.h"
#include "i3RegVEC2D.h"

I3_CLASS_INSTANCE( i3RegVEC2D);

i3RegVEC2D::i3RegVEC2D(void)
{
	m_nDataType = I3REG_TYPE_VEC2D;
}

i3RegVEC2D::i3RegVEC2D( REAL32 u, REAL32 v)
{
	m_nDataType = I3REG_TYPE_VEC2D;
	i3Vector::Set( &m_Value, v, u);
}

i3RegVEC2D::i3RegVEC2D( VEC2D * pVec)
{
	m_nDataType = I3REG_TYPE_VEC2D;
	i3Vector::Copy( &m_Value, pVec);
}

void i3RegVEC2D::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegVEC2D*>(pDest)); 

	i3RegVEC2D * pVec2 = ( i3RegVEC2D*)pDest;

	pVec2->set( *this);
}

void	i3RegVEC2D::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegVEC2D::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;
	i3::pack::VEC2D	data;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	{
		data.x = m_Value.x;
		data.y = m_Value.y;
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegVEC2D::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::VEC2D data;
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Vector::Set( &m_Value, data.x, data.y);

	return Result;
}

void i3RegVEC2D::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%f, %f", m_Value.x, m_Value.y);
}
