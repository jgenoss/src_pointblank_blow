#include "i3SceneDef.h"
#include "i3CollideeCylinder.h"

I3_CLASS_INSTANCE( i3CollideeCylinder);

void i3CollideeCylinder::Transform( MATRIX * pMatrix, i3Collidee * pCol)
{
	i3CollideeCylinder * pDest = (i3CollideeCylinder *) pCol;

	I3ASSERT( i3::same_of<i3CollideeCylinder*>(pDest));

	i3Matrix::TransformPoints( pDest->GetStart(), &m_Start, 1, pMatrix);
	i3Matrix::TransformVectors( pDest->GetDirection(), &m_Dir, 1, pMatrix);

	pDest->m_Length = m_Length;
	pDest->m_Radius = m_Radius;
	pDest->m_RadiusSq = m_RadiusSq;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 COLLIDEE_CYLINDER
	{
		i3::pack::VEC3D	m_Start;
		i3::pack::VEC3D	m_End;
		REAL32			m_Radius = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3CollideeCylinder::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::COLLIDEE_CYLINDER Data;

	Result = i3Collidee::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	Data.m_Start.x = i3Vector::GetX( &m_Start);
	Data.m_Start.y = i3Vector::GetY( &m_Start);
	Data.m_Start.z = i3Vector::GetZ( &m_Start);

	{
		VEC3D end;

		i3Vector::Scale( &end, &m_Dir, m_Length);
		i3Vector::Add( &end, &end, &m_Start);

		Data.m_End.x = i3Vector::GetX( &end);
		Data.m_End.y = i3Vector::GetY( &end);
		Data.m_End.z = i3Vector::GetZ( &end);
	}

	Data.m_Radius = m_Radius;

	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CollideeCylinder::OnSave()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3CollideeCylinder::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::COLLIDEE_CYLINDER Data;

	Result = i3Collidee::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;
	
	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3CollideeCylinder::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	i3Vector::Set( &m_Start, Data.m_Start.x, Data.m_Start.y, Data.m_Start.z);

	{
		VEC3D diff;

		i3Vector::Set( &diff, Data.m_End.x, Data.m_End.y, Data.m_End.z);
		i3Vector::Sub( &diff, &diff, &m_Start);

		m_Length = i3Vector::Length( &diff);

		i3Vector::Normalize( &m_Dir, &diff);
	}

	return Result;
}
