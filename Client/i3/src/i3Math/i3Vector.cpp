#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Matrix.h"

/////////////////////////////////////////////////////////////////
// VEC2D

VEC4D VEC2D::Transform( const MATRIX & mtx ) const
{
	VEC4D r;

	r.x = (x * mtx.m[0][0]) + (y * mtx.m[0][1]) + mtx.m[0][2] + mtx.m[0][3];
	r.y = (x * mtx.m[1][0]) + (y * mtx.m[1][1]) + mtx.m[1][2] + mtx.m[1][3];
	r.z = (x * mtx.m[2][0]) + (y * mtx.m[2][1]) + mtx.m[2][2] + mtx.m[2][3];
	r.w = (x * mtx.m[3][0]) + (y * mtx.m[3][1]) + mtx.m[3][2] + mtx.m[3][3];

	return r;
}

VEC2D VEC2D::TransformCoord( const MATRIX & mtx) const
{
	VEC2D r;

	r.x = (x * mtx.m[0][0]) + (y * mtx.m[0][1]);
	r.y = (x * mtx.m[1][0]) + (y * mtx.m[1][1]);

	return r;
}

VEC2D VEC2D::TransformNormal( const MATRIX & mtx) const
{
	VEC2D r;

	r.x = (x * mtx.m[0][0]) + (y * mtx.m[0][1]);
	r.y = (x * mtx.m[1][0]) + (y * mtx.m[1][1]);

	return r;
}

/////////////////////////////////////////////////////////////////
// VEC3D
VEC4D VEC3D::Transform( const MATRIX & mtx ) const
{
	VEC4D r;

	r.x = (mtx.m[0][0] * x) + (mtx.m[1][0] * y) + (mtx.m[2][0] * z) + mtx.m[3][0];
	r.y = (mtx.m[0][1] * x) + (mtx.m[1][1] * y) + (mtx.m[2][1] * z) + mtx.m[3][1];
	r.z = (mtx.m[0][2] * x) + (mtx.m[1][2] * y) + (mtx.m[2][2] * z) + mtx.m[3][2];
	r.w = (mtx.m[0][3] * x) + (mtx.m[1][3] * y) + (mtx.m[2][3] * z) + mtx.m[3][3];

	return r;
}

VEC3D VEC3D::TransformCoord( const MATRIX & mtx) const
{
	VEC3D r;

	#if defined( I3G_DX)
		D3DXVec3TransformCoord( (D3DXVECTOR3 *) &r, (const D3DXVECTOR3 *) this, (const D3DXMATRIX *) &mtx);
	#else
		REAL32 x,y,z,w,rw;

		x = (mtx.m[0][0] * pV->x) + (mtx.m[1][0] * pV->y) + (mtx.m[2][0] * pV->z) + mtx.m[3][0];
		y = (mtx.m[0][1] * pV->x) + (mtx.m[1][1] * pV->y) + (mtx.m[2][1] * pV->z) + mtx.m[3][1];
		z = (mtx.m[0][2] * pV->x) + (mtx.m[1][2] * pV->y) + (mtx.m[2][2] * pV->z) + mtx.m[3][2];
		w = (mtx.m[0][3] * pV->x) + (mtx.m[1][3] * pV->y) + (mtx.m[2][3] * pV->z) + mtx.m[3][3];
		rw = 1.0f / w;
		r.x = x * rw;
		r.y = y * rw;
		r.z = z * rw;
	#endif

	return r;
}

VEC3D VEC3D::TransformNormal( const MATRIX & mtx) const
{
	VEC3D r;
	
	r.x = (mtx.m[0][0] * x) + (mtx.m[1][0] * y) + (mtx.m[2][0] * z);
	r.y = (mtx.m[0][1] * x) + (mtx.m[1][1] * y) + (mtx.m[2][1] * z);
	r.z = (mtx.m[0][2] * x) + (mtx.m[1][2] * y) + (mtx.m[2][2] * z);

	return r;
}

/////////////////////////////////////////////////////////////////
// VEC4D

VEC4D VEC4D::Transform( const MATRIX & mtx ) const
{
	VEC4D r;

	r.x = (mtx.m[0][0] * x) + (mtx.m[1][0] * y) + (mtx.m[2][0] * z) + (mtx.m[3][0] * w);
	r.y = (mtx.m[0][1] * x) + (mtx.m[1][1] * y) + (mtx.m[2][1] * z) + (mtx.m[3][1] * w);
	r.z = (mtx.m[0][2] * x) + (mtx.m[1][2] * y) + (mtx.m[2][2] * z) + (mtx.m[3][2] * w);
	r.w = (mtx.m[0][3] * x) + (mtx.m[1][3] * y) + (mtx.m[2][3] * z) + (mtx.m[3][3] * w);

	return r;
}

bool	i3Vector::SetToXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "x", pVec->x);
	pChild->addAttr( "y", pVec->y);

	return true;
}

bool	i3Vector::SetToXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "x", pVec->x);
	pChild->addAttr( "y", pVec->y);
	pChild->addAttr( "z", pVec->z);

	return true;
}

bool	i3Vector::SetToXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "x", pVec->x);
	pChild->addAttr( "y", pVec->y);
	pChild->addAttr( "z", pVec->z);
	pChild->addAttr( "w", pVec->w);

	return true;
}

bool	i3Vector::GetFromXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	if( pChild == nullptr)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr( "x", & pVec->x);
	pChild->getAttr( "y", & pVec->y);

	return true;
}

bool	i3Vector::GetFromXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	if( pChild == nullptr)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr( "x", & pVec->x);
	pChild->getAttr( "y", & pVec->y);
	pChild->getAttr( "z", & pVec->z);

	return true;
}

bool	i3Vector::GetFromXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	if( pChild == nullptr)
	{
		I3ERRLOG( i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr( "x", & pVec->x);
	pChild->getAttr( "y", & pVec->y);
	pChild->getAttr( "z", & pVec->z);
	pChild->getAttr( "w", & pVec->w);

	return true;
}
