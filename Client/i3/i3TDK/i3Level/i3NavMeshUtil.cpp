#include "stdafx.h"
#include "i3NavMeshUtil.h"

#include <algorithm>
#include <array>

navMeshVertex& navMeshVertex::operator=(const navMeshVertex& v)
{
	::memcpy(&vec3D, &v.vec3D, sizeof(VEC3D));	bRemoved = v.bRemoved;	return *this;		// 위 복사생성자와는 달리, 원래 없던 대입연산자를 구현한 것으로 모두 복사되는 것으로 간주 bRemoved도 카피처리한다. (2015.08.31.수빈)
}


bool	navMeshVertex::operator == ( const navMeshVertex& rhs ) const
{
	return !( *this != rhs );
}

bool	navMeshVertex::operator != ( const navMeshVertex& rhs ) const
{	
	for( int i = 0; i < 3; ++i )
	{
		if( f[i] == rhs.f[i] ) return false;
	}

	return true;
}

bool	navMeshVertex::operator < ( const navMeshVertex& rhs ) const
{
	for( int i = 0; i < 3; ++i )
	{
		float temp = fabsf(f[i] - rhs.f[i]);

		if( temp < 0.0001f && -0.0001f < temp ) // 값이 동일
		{
			continue;
		}

		if( f[i] < rhs.f[i] ) return true;
		else if( f[i] > rhs.f[i] ) return false;
	}

	return false;
}


navMeshPoly::navMeshPoly() : na(0), nb(0), nc(0), bRemoved( false )
{
	i3Vector::Set( &normal, 0.0f, 0.0f, 0.0f );
}

navMeshPoly::navMeshPoly( int _a, int _b, int _c ) : na( _a ), nb( _b), nc( _c ), bRemoved( false )
{
	i3Vector::Set( &normal, 0.0f, 0.0f, 0.0f );
}

bool	navMeshPoly::operator == ( const navMeshPoly& rhs ) const
{
	return !( *this != rhs );
}

bool	navMeshPoly::operator != ( const navMeshPoly& rhs ) const
{
	for( int i = 0; i < 3; ++i )
	{
		int ncheck = 0;
		for( ; ncheck < 3; ++ncheck )
		{
			if( n[i] == rhs.n[ncheck] )
				break;
		}

		if( ncheck == 3 )
		{
			return true;
		}
	}

	return false;
}

bool	navMeshPoly::operator < ( const navMeshPoly& rhs ) const
{
	if( *this != rhs )
	{
		std::tr1::array< int, 3 > tmp_lhs;
		std::tr1::array< int, 3 > tmp_rhs;

		for( int i = 0; i < 3; ++i )
		{
			tmp_lhs[i] = this->n[i];
			tmp_rhs[i] = rhs.n[i];
		}

		i3::sort( tmp_lhs.begin(), tmp_lhs.end(), i3::less< int >() );
		i3::sort( tmp_rhs.begin(), tmp_rhs.end(), i3::less< int >() );

		for( int i = 0; i < 3; ++i )
		{			
			if( tmp_lhs[i] < tmp_rhs[i] ) return true;
			else if( tmp_lhs[i] > tmp_rhs[i] ) return false;
		}		
	}

	return false;
}

bool	I3NAVMESH::IntersectLineToLine( const VEC2D* pta1, const VEC2D* pta2, const VEC2D* ptb1, const VEC2D* ptb2 )
{
	float q = (pta1->y - ptb1->y) * (ptb2->x - ptb1->x) - (pta1->x - ptb1->x) * (ptb2->y - ptb1->y);
	float d = (pta2->x - pta1->x) * (ptb2->y - ptb1->y) - (pta2->y - pta1->y) * (ptb2->x - ptb1->x);

	if( d == 0 )
	{
		return false;
	}

	float r = q / d;

	q = (pta1->y - ptb1->y) * (pta2->x - pta1->x) - (pta1->x - ptb1->x) * (pta2->y - pta1->y);
	float s = q / d;

	if( r < 0 || r > 1 || s < 0 || s > 1 )
	{
		return false;
	}

	return true;
}

bool	I3NAVMESH::IntersectLineToLine( const VEC3D* p1, const VEC3D* p2, const VEC3D* p3, const VEC3D* p4, VEC3D* pr1, VEC3D* pr2 )
{
	float EPS = 0.0001f;
	float mua, mub;

	VEC3D p13,p43,p21;
	float d1343,d4321,d1321,d4343,d2121;
	float numer,denom;

	p13.x = p1->x - p3->x;
	p13.y = p1->y - p3->y;
	p13.z = p1->z - p3->z;
	p43.x = p4->x - p3->x;
	p43.y = p4->y - p3->y;
	p43.z = p4->z - p3->z;
	if (i3Math::abs(p43.x) < EPS && i3Math::abs(p43.y) < EPS && i3Math::abs(p43.z) < EPS)
		return	(false);
	p21.x = p2->x - p1->x;
	p21.y = p2->y - p1->y;
	p21.z = p2->z - p1->z;
	if (i3Math::abs(p21.x) < EPS && i3Math::abs(p21.y) < EPS && i3Math::abs(p21.z) < EPS)
		return	(false);

	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

	denom = d2121 * d4343 - d4321 * d4321;
	if (i3Math::abs(denom) < EPS)
		return	(false);
	numer = d1343 * d4321 - d1321 * d4343;

	mua = numer / denom;
	mub = (d1343 + d4321 * mua) / d4343;

	if( pr1 )
	{
		pr1->x = p1->x + mua * p21.x;
		pr1->y = p1->y + mua * p21.y;
		pr1->z = p1->z + mua * p21.z;
	}
	
	if( pr2 )
	{
		pr2->x = p3->x + mub * p43.x;
		pr2->y = p3->y + mub * p43.y;
		pr2->z = p3->z + mub * p43.z;
	}	

	return true;
}

bool	I3NAVMESH::IntersectLineToRect( const VEC2D* p1, const VEC2D* p2, const i3::pack::RECT* r)
{
	float x = r->left;
	float y = r->top;

	float w = r->right - r->left;
	float h = r->bottom - r->top;

	VEC2D v1, v2, v3, v4;
	
	v1.x = p1->x, v1.y = p1->y;
	v2.x = p2->x, v2.y = p2->y;
	
	i3Vector::Set( &v3, x,		y );
	i3Vector::Set( &v4, x + w,	y );

	if( true == I3NAVMESH::IntersectLineToLine( &v1, &v2, &v3, &v4 ) )
	{
		return true;
	}
	
	i3Vector::Set( &v3, x + w,	y );
	i3Vector::Set( &v4, x + w,	y + h );

	if( true == I3NAVMESH::IntersectLineToLine( &v1, &v2, &v3, &v4 ) )
	{
		return true;
	}

	i3Vector::Set( &v3, x + w,	y + h );
	i3Vector::Set( &v4, x,		y + h );

	if( true == I3NAVMESH::IntersectLineToLine( &v1, &v2, &v3, &v4 ) )
	{
		return true;
	}

	i3Vector::Set( &v3, x, y + h );
	i3Vector::Set( &v4, x, y );

	if( true == I3NAVMESH::IntersectLineToLine( &v1, &v2, &v3, &v4 ) )
	{
		return true;
	}

	if( i3Math::PtInRect( (i3::pack::RECT*) r, (VEC2D*) p1) && i3Math::PtInRect( (i3::pack::RECT*) r, (VEC2D*) p2 ) )
	{
		return true;
	}

	return false;
}

bool	I3NAVMESH::IntersectLineToTri( const VEC3D* p1, const VEC3D* p2, const VEC3D* n, const VEC3D* p0, VEC3D* pc,REAL32 *t)
{
	REAL32 num, den;
	VEC3D p21;
	
	i3Vector::Sub( &p21, p2, p1 );

	num = i3Vector::Dot( n, p0 ) - i3Vector::Dot( n, p1);
	den = i3Vector::Dot( n, &p21);

	if( den*den <= 1E-15f * num * num ) 
	{
		return false;
	}

	*t = num / den;

	i3Vector::Lerp( pc, p1, p2, *t);

	if ( 0.0f <= *t && *t <= 1.0f ) 
	{
		return true;
	}
	
	return false;
}
