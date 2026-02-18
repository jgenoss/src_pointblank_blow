#include "i3MathType.h"
#include "i3Math.h"
#include "i3BoundBox.h"

I3_CLASS_INSTANCE( i3BoundBox);

void i3BoundBox::ExtendByVec( VEC2D * pVec)
{
	VEC3D vec3D;
	i3Vector::Set( &vec3D, i3Vector::GetX( pVec ), i3Vector::GetY( pVec ), 0.0f );

	if( m_bEmpty)
	{
		i3Vector::Copy( &m_Min, &vec3D );
		i3Vector::Copy( &m_Max, &vec3D );
		i3Vector::Copy( &m_Center, &vec3D);

		m_bEmpty = false;
	}
	else
	{
		i3Vector::Minimize( &m_Min, &m_Min, &vec3D );
		i3Vector::Maximize( &m_Max, &m_Max, &vec3D );

		i3Vector::Add( &m_Center, &m_Min, &m_Max);
		i3Vector::Scale( &m_Center, &m_Center, 0.5f);
	}
}

void i3BoundBox::ExtendByVec( VEC3D * pVec)
{
	if( m_bEmpty)
	{
		i3Vector::Copy( &m_Min, pVec);
		i3Vector::Copy( &m_Max, pVec);
		i3Vector::Copy( &m_Center, pVec);

		m_bEmpty = false;
	}
	else
	{
		i3Vector::Minimize( &m_Min, &m_Min, pVec);
		i3Vector::Maximize( &m_Max, &m_Max, pVec);

		i3Vector::Add( &m_Center, &m_Min, &m_Max);
		i3Vector::Scale( &m_Center, &m_Center, 0.5f);
	}
}

void i3BoundBox::ExtendByBoundBox( i3BoundBox * pBox)
{
	if( m_bEmpty)
	{
		i3Vector::Copy( &m_Min, pBox->GetMin());
		i3Vector::Copy( &m_Max, pBox->GetMax());
		i3Vector::Copy( &m_Center, pBox->getCenter());
		m_bEmpty = false;
	}
	else
	{
		i3Vector::Minimize( &m_Min, &m_Min, pBox->GetMin());
		i3Vector::Maximize( &m_Max, &m_Max, pBox->GetMax());

		i3Vector::Add( &m_Center, &m_Min, &m_Max);
		i3Vector::Scale( &m_Center, &m_Center, 0.5f);
	}
}

void i3BoundBox::GetCornerVertices( VEC3D * pVtx)
{
	REAL32 nx, px;
	REAL32 ny, py;
	REAL32 nz, pz;

	nx = i3Vector::GetX( &m_Min);
	ny = i3Vector::GetY( &m_Min);
	nz = i3Vector::GetZ( &m_Min);

	px = i3Vector::GetX( &m_Max);
	py = i3Vector::GetY( &m_Max);
	pz = i3Vector::GetZ( &m_Max);
	
	i3Vector::Set( &pVtx[0],	nx,	ny, nz);
	i3Vector::Set( &pVtx[1],	px,	ny, nz);
	i3Vector::Set( &pVtx[2],	px,	py, nz);
	i3Vector::Set( &pVtx[3],	nx,	py, nz);

	i3Vector::Set( &pVtx[4],	nx,	ny, pz);
	i3Vector::Set( &pVtx[5],	px,	ny, pz);
	i3Vector::Set( &pVtx[6],	px,	py, pz);
	i3Vector::Set( &pVtx[7],	nx,	py, pz);
}

void	i3BoundBox::GetPlanes( VEC4D * pPlanes)
{
	//bottom plane
	i3Vector::Set( &pPlanes[0], 0.0f, -1.0f, 0.0f,	i3Math::abs( GetMin()->y));

	//top plane
	i3Vector::Set( &pPlanes[1], 0.0f, 1.0f, 0.0f,	i3Math::abs( GetMax()->y));

	//left plane
	i3Vector::Set( &pPlanes[2], -1.0f, 0.0f, 0.0f,	i3Math::abs( GetMin()->x));

	//right plane
	i3Vector::Set( &pPlanes[3], 1.0f, 0.0f, 0.0f,	i3Math::abs( GetMax()->x));

	//back plane
	i3Vector::Set( &pPlanes[4], 0.0f, 0.0f, -1.0f,	i3Math::abs( GetMin()->z));

	//front plane
	i3Vector::Set( &pPlanes[5], 0.0f, 0.0f, 1.0f,	i3Math::abs( GetMax()->z));
}

void	i3BoundBox::Transform( i3BoundBox * pOut, MATRIX * pMtx)
{
	VEC3D pos[8];
	INT32 i;

	GetCornerVertices( pos);
	pOut->Reset();

	i3Matrix::TransformPoints( pos, pos, 8, pMtx);

	for( i = 0; i < 8; i++)
	{
		pOut->ExtendByVec( &pos[i]);
	}
}

void i3BoundBox::Extend( REAL32 sz)
{
	VEC3D dir;

	getSize( &dir);
	//i3Vector::Normalize( &dir, &dir);

	i3Vector::Scale( &dir, &dir, (sz - 1.0f) * 0.5f);
	i3Vector::Add( &m_Max, &m_Max, &dir);
	i3Vector::Sub( &m_Min, &m_Min, &dir);
}

TOUCH_TYPE i3BoundBox::IntersectWith( i3BoundBox * pInBox)
{
	TOUCH_TYPE touch = TOUCH_WHOLE;

	if( IsTouched( pInBox) == false)
		return TOUCH_NONE;

	if( m_Min.x < pInBox->GetMin()->x)
	{
		touch = TOUCH_PARTIAL;
		m_Min.x = pInBox->GetMin()->x;
	}

	if( m_Min.y < pInBox->GetMin()->y)
	{
		touch = TOUCH_PARTIAL;
		m_Min.y = pInBox->GetMin()->y;
	}

	if( m_Min.z < pInBox->GetMin()->z)
	{
		touch = TOUCH_PARTIAL;
		m_Min.z = pInBox->GetMin()->z;
	}

	if( m_Max.x > pInBox->GetMax()->x)
	{
		touch = TOUCH_PARTIAL;
		m_Max.x = pInBox->GetMax()->x;
	}

	if( m_Max.y > pInBox->GetMax()->y)
	{
		touch = TOUCH_PARTIAL;
		m_Max.y = pInBox->GetMax()->y;
	}

	if( m_Max.z > pInBox->GetMax()->z)
	{
		touch = TOUCH_PARTIAL;
		m_Max.z = pInBox->GetMax()->z;
	}

	i3Vector::Add( &m_Center, &m_Min, &m_Max);
	i3Vector::Scale( &m_Center, &m_Center, 0.5f);

	return touch;
}

bool i3BoundBox::IsWhollyContained( i3BoundBox * pSmallBox)
{
	if( (m_Min.x > pSmallBox->GetMin()->x) || (m_Max.x < pSmallBox->GetMax()->x))
		return false;

	if( (m_Min.y > pSmallBox->GetMin()->y) || (m_Max.y < pSmallBox->GetMax()->y))
		return false;

	if( (m_Min.z > pSmallBox->GetMin()->z) || (m_Max.z < pSmallBox->GetMax()->z))
		return false;

	return true;
}

bool clipTest( REAL32 p, REAL32 q, REAL32  &u1, REAL32 &u2) 
{
	if(p < 0.0f) 
	{
		const REAL32 r = q/p;

		if(r > u2) 
			return false;
		else 
		{
			if(r > u1) 
				u1 = r;

			return true;
		}
	}
	else 
	{
		if(p > 0.0f)	
		{
			const REAL32 r = q/p;

			if(r < u1) 
				return false;
			else
			{
				if(r < u2) 
					u2 = r;

				return true;
			}
		}
		else 
		{
			return q >= 0.0f;
		}
	}
}

bool	i3BoundBox::IntersectWithLine( VEC3D * pStart, VEC3D * pDir, VEC3D * pIntersect)
{
	VEC3D * p = pStart;
	VEC3D * dir = pDir;
	VEC3D * vmin = GetMin();
	VEC3D * vmax = GetMax();
	REAL32 t1 = 0.0;
	REAL32 t2 = FLT_MAX;

	bool intersect =
		clipTest( -dir->z, p->z - vmin->z, t1, t2) && clipTest( dir->z, vmax->z - p->z, t1, t2) &&
		clipTest( -dir->y, p->y - vmin->y, t1, t2) && clipTest( dir->y, vmax->y - p->y, t1, t2) &&
		clipTest( -dir->z, p->x - vmin->x, t1, t2) && clipTest( dir->x, vmax->x - p->x, t1, t2);

	if( !intersect) 
		return false;

	intersect = false;

	if( 0.0f <= t1) 
	{
		i3Vector::Scale( pIntersect, dir, t1);
		i3Vector::Add( pIntersect, pIntersect, p);
		intersect = true;
	}

	if( 0.0f <= t2) 
	{
		i3Vector::Scale( pIntersect, dir, t2);
		i3Vector::Add( pIntersect, pIntersect, p);
		intersect = true;
	}
	return intersect;
}

bool i3BoundBox::IsTouched( i3BoundBox * pBox)
{
#if defined( I3_FIXED_POINT)
	VEC3D min1, min2, max1, max2;

	i3Vector::Copy( &min1, &m_Min);
	i3Vector::Copy( &max1, &m_Max);

	i3Vector::Copy( &min2, pBox->GetMin());
	i3Vector::Copy( &max2, pBox->GetMax());

	return i3Math::BoundBoxContainBoundBox( &min1, &max1, &min2, &max2);
#else
	return i3Math::BoundBoxContainBoundBox( &m_Min, &m_Max, pBox->GetMin(), pBox->GetMax());
#endif
}

bool i3BoundBox::isInside( VEC3D * pPos)
{
	if((pPos->x < m_Min.x) || (pPos->x > m_Max.x))
		return false;

	if((pPos->y < m_Min.y) || (pPos->y > m_Max.y))
		return false;

	if((pPos->z < m_Min.z) || (pPos->z > m_Max.z))
		return false;

	return true;
}

UINT32	i3BoundBox::getSideFlag( VEC3D * pPos)
{
	UINT32 side = 0;

	if( pPos->x < m_Min.x)		side |= BB_SIDE_NX;
	else if( pPos->x > m_Max.x)	side |= BB_SIDE_PX;

	if( pPos->y < m_Min.y)		side |= BB_SIDE_NY;
	else if( pPos->y > m_Max.y)	side |= BB_SIDE_PY;

	if( pPos->z < m_Min.z)		side |= BB_SIDE_NZ;
	else if( pPos->z > m_Max.z)	side |= BB_SIDE_PZ;

	return side;
}

void i3BoundBox::GetCenter( VEC2D * pVec)
{
	VEC3D sp;

	GetCenter( &sp );

	i3Vector::Set( pVec, i3Vector::GetX( &sp),i3Vector::GetY( &sp ) );
}

void i3BoundBox::GetCenter( VEC3D * pVec)
{
	VEC3D sp;

	i3Vector::Add( &sp, &m_Min, &m_Max);
	i3Vector::SetX( pVec, i3Math::Mul( i3Vector::GetX(&sp), 0.5f));
	i3Vector::SetY( pVec, i3Math::Mul( i3Vector::GetY(&sp), 0.5f));
	i3Vector::SetZ( pVec, i3Math::Mul( i3Vector::GetZ(&sp), 0.5f));
}

UINT32 i3BoundBox::OnSave( i3ResourceFile * pResFile)
{
	i3::pack::VEC3D vec;
	UINT32 Rc, Result = 0;

	vec.x = i3Vector::GetX( &m_Min);
	vec.y = i3Vector::GetY( &m_Min);
	vec.z = i3Vector::GetZ( &m_Min);

	Rc = pResFile->GetStream()->Write( &vec, sizeof(i3::pack::VEC3D));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BoundBox::OnSave()", "Could not write i3BoundBox object information.");
		return STREAM_ERR;
	}
	Result += Rc;

	vec.x = i3Vector::GetX( &m_Max);
	vec.y = i3Vector::GetY( &m_Max);
	vec.z = i3Vector::GetZ( &m_Max);

	Rc = pResFile->GetStream()->Write( &vec, sizeof(i3::pack::VEC3D));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BoundBox::OnSave()", "Could not write i3BoundBox object information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3BoundBox::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::VEC3D vec;
	UINT32 Rc, Result = 0;

	Rc = pResFile->GetStream()->Read( &vec, sizeof(i3::pack::VEC3D));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BoundBox::OnLoad()", "Could not read i3BoundBox object information.");
		return STREAM_ERR;
	}
	Result += Rc;

	i3Vector::Copy(&m_Min, &vec);

	Rc = pResFile->GetStream()->Read( &vec, sizeof(i3::pack::VEC3D));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BoundBox::OnLoad()", "Could not read i3BoundBox object information.");
		return STREAM_ERR;
	}
	Result += Rc;

	i3Vector::Copy(&m_Max, &vec);

	i3Vector::Add( &m_Center, &m_Min, &m_Max);
	i3Vector::Scale( &m_Center, &m_Center, 0.5f);

	return Result;
}

bool i3BoundBox::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3::pack::VEC3D vec;

	vec.x = i3Vector::GetX(&m_Min);
	vec.y = i3Vector::GetY(&m_Min);
	vec.z = i3Vector::GetZ(&m_Min);

	pXML->addAttr("MinVecX", vec.x);
	pXML->addAttr("MinVecY", vec.y);
	pXML->addAttr("MinVecZ", vec.z);

	vec.x = i3Vector::GetX(&m_Max);
	vec.y = i3Vector::GetY(&m_Max);
	vec.z = i3Vector::GetZ(&m_Max);

	pXML->addAttr("MaxVecX", vec.x);
	pXML->addAttr("MaxVecY", vec.y);
	pXML->addAttr("MaxVecZ", vec.z);

	return true;
}
bool i3BoundBox::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3::pack::VEC3D vec;

	pXML->getAttr("MinVecX", &vec.x);
	pXML->getAttr("MinVecY", &vec.y);
	pXML->getAttr("MinVecZ", &vec.z);
	i3Vector::Copy(&m_Min, &vec);

	pXML->getAttr("MaxVecX", &vec.x);
	pXML->getAttr("MaxVecY", &vec.y);
	pXML->getAttr("MaxVecZ", &vec.z);
	i3Vector::Copy(&m_Max, &vec);

	i3Vector::Add(&m_Center, &m_Min, &m_Max);
	i3Vector::Scale(&m_Center, &m_Center, 0.5f);

	return true;
}

#if defined( I3_DEBUG)
void i3BoundBox::vlog( UINT8 r, UINT8 g, UINT8 b)
{

}

#endif
