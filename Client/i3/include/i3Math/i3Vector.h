// i3interfaceVector.h

#ifndef __I3_VECTOR_H
#define __I3_VECTOR_H

#include "i3Base.h"
#include "i3MathDefine.h"
#include "i3MathType.h"

union MATRIX;
union VEC4D;

////////////////////////////////////////////
// VEC2D 

union I3_EXPORT_MATH VEC2D
{
	struct
	{
		REAL32 x, y;
	};

	REAL32 vec[2];

	VEC2D(void) : x(0.0f), y(0.0f) {}

	VEC2D(REAL32 _x, REAL32 _y) : x(_x), y(_y) {}

	void	Set( REAL32 _x, REAL32 _y)
	{
		x = _x;
		y = _y;
	}

	VEC2D & operator = ( const VEC2D & a)
	{
		x = a.x;
		y = a.y;

		return *this;
	}

	VEC2D & operator += ( const VEC2D & a)
	{
		x += a.x;
		y += a.y;

		return *this;
	}

	VEC2D & operator -= ( const VEC2D & a)
	{
		x -= a.x;
		y -= a.y;

		return *this;
	}

	VEC2D & operator *= ( const VEC2D & a)
	{
		x *= a.x;
		y *= a.y;

		return *this;
	}

	VEC2D & operator /= ( const VEC2D & a)
	{
		x /= a.x;
		y /= a.y;

		return *this;
	}

	VEC2D & operator += ( REAL32 val)
	{
		x += val;
		y += val;

		return *this;
	}

	VEC2D & operator -= ( REAL32 val)
	{
		x -= val;
		y -= val;

		return *this;
	}

	VEC2D & operator *= ( REAL32 val)
	{
		x *= val;
		y *= val;

		return *this;
	}

	VEC2D & operator /= ( REAL32 val)
	{
		x /= val;
		y /= val;

		return *this;
	}

	VEC2D operator + (const VEC2D & a) const
	{
		VEC2D r;

		r.x = x + a.x;
		r.y = y + a.y;

		return r;
	}

	VEC2D operator - (const VEC2D & a) const
	{
		VEC2D r;

		r.x = x - a.x;
		r.y = y - a.y;

		return r;
	}

	VEC2D operator * (const VEC2D & a) const
	{
		VEC2D r;

		r.x = x * a.x;
		r.y = y * a.y;

		return r;
	}

	VEC2D operator / (const VEC2D & a) const
	{
		VEC2D r;

		r.x = x / a.x;
		r.y = y / a.y;

		return r;
	}

	VEC2D operator + ( REAL32 val) const
	{
		VEC2D r;

		r.x = x + val;
		r.y = y + val;

		return r;
	}

	VEC2D operator - ( REAL32 val) const
	{
		VEC2D r;

		r.x = x - val;
		r.y = y - val;

		return r;
	}

	VEC2D operator * ( REAL32 val) const
	{
		VEC2D r;

		r.x = x * val;
		r.y = y * val;

		return r;
	}

	VEC2D operator / ( REAL32 val) const
	{
		VEC2D r;

		r.x = x / val;
		r.y = y / val;

		return r;
	}

	bool operator == (const VEC2D & p) const
	{
		return i3Math::isAlike( x, p.x) && i3Math::isAlike( y, p.y);
	}

	bool operator != (const VEC2D & p) const
	{
		return ! (*this == p);
	}

	bool operator < (const VEC2D & p) const
	{
		if( i3Math::isAlike( x, p.x))
		{
			if( i3Math::isAlike( y, p.y))
			{
				return false;
			}
			else if( y > p.y)
			{
				return false;
			}
		}
		else if( x > p.x)
		{
			return false;
		}
		
		return true;
	}

	bool operator > (const VEC2D & p) const
	{
		return !( (*this) < p || (*this)==p );
	}

	bool operator <= (const VEC2D & p) const
	{
		return ((*this) < p || (*this == p));
	}

	bool operator >= (const VEC2D & p) const
	{
		return !((*this) < p);
	}

	bool	isAlike( const VEC2D & a, REAL32 eps = I3_EPS)	const
	{
		return	i3Math::isAlike( x, a.x, eps) &&
				i3Math::isAlike( y, a.y, eps);
	}

	REAL32	Length(void)	const
	{
		return i3Math::Sqrt( (x * x) + (y * y));
	}

	REAL32	LengthSq(void)	const
	{
		return (x * x) + (y * y);
	}

	REAL32	Normalize(void)
	{
		REAL32 len;

		len = Length();

		if( len <= I3_EPS)
		{
			return 0.0f;
		}

		REAL32 rlen = 1.0f / len;

		x *= rlen;
		y *= rlen;

		return len;
	}

	bool	isValid(void)	const
	{
		return i3Math::isValid(x) && i3Math::isValid(y);
	}

	VEC4D Transform( const MATRIX & mtx ) const;
	VEC2D TransformCoord( const MATRIX & mtx) const;
	VEC2D TransformNormal( const MATRIX & mtx) const;

	static REAL32	Dot( const VEC2D & v1, const VEC2D & v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y);
	}

	static VEC2D	Minimize( const VEC2D & v1, const VEC2D & v2)
	{
		VEC2D r;

		r.x = MIN( v1.x, v2.x);
		r.y = MIN( v1.y, v2.y);

		return r;
	}

	static VEC2D	Maximize( const VEC2D & v1, const VEC2D & v2)
	{
		VEC2D r;

		r.x = MAX( v1.x, v2.x);
		r.y = MAX( v1.y, v2.y);

		return r;
	}

	static VEC2D	Lerp( const VEC2D & v1, const VEC2D & v2, REAL32 s)
	{
		VEC2D r;

		r.x = v1.x + ((v2.x - v1.x) * s);
		r.y = v1.y + ((v2.y - v1.y) * s);

		return r;
	}

}  VECTORALIGN;

////////////////////////////////////////////
// VEC3D 

union I3_EXPORT_MATH VEC3D
{
	struct
	{
		REAL32 x, y, z;
	};

	REAL32 vec[3];

	VEC3D(void) : x(0.0f), y(0.0f), z(0.0f) {}
	VEC3D(REAL32 _x, REAL32 _y, REAL32 _z) : x(_x), y(_y), z(_z) {}

	void	Set( REAL32 _x, REAL32 _y, REAL32 _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	VEC3D & operator = (const VEC3D & p)
	{
		x = p.x;
		y = p.y;
		z = p.z;

		return *this;
	}

	VEC3D & operator += ( const VEC3D & a)
	{
		x += a.x;
		y += a.y;
		z += a.z;

		return *this;
	}

	VEC3D & operator -= ( const VEC3D & a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;

		return *this;
	}

	VEC3D & operator *= ( const VEC3D & a)
	{
		x *= a.x;
		y *= a.y;
		z *= a.z;

		return *this;
	}

	VEC3D & operator /= ( const VEC3D & a)
	{
		x /= a.x;
		y /= a.y;
		z /= a.z;

		return *this;
	}

	VEC3D & operator += ( REAL32 val)
	{
		x += val;
		y += val;
		z += val;

		return *this;
	}

	VEC3D & operator -= ( REAL32 val)
	{
		x -= val;
		y -= val;
		z -= val;

		return *this;
	}

	VEC3D & operator *= ( REAL32 val)
	{
		x *= val;
		y *= val;
		z *= val;

		return *this;
	}

	VEC3D & operator /= ( REAL32 val)
	{
		x /= val;
		y /= val;
		z /= val;

		return *this;
	}

	VEC3D operator + (const VEC3D & a) const
	{
		VEC3D r;

		r.x = x + a.x;
		r.y = y + a.y;
		r.z = z + a.z;

		return r;
	}

	VEC3D operator - (const VEC3D & a) const
	{
		VEC3D r;

		r.x = x - a.x;
		r.y = y - a.y;
		r.z = z - a.z;

		return r;
	}

	VEC3D operator * (const VEC3D & a) const
	{
		VEC3D r;

		r.x = x * a.x;
		r.y = y * a.y;
		r.z = z * a.z;

		return r;
	}

	VEC3D operator / (const VEC3D & a) const
	{
		VEC3D r;

		r.x = x / a.x;
		r.y = y / a.y;
		r.z = z / a.z;

		return r;
	}

	VEC3D operator + ( REAL32 val) const
	{
		VEC3D r;

		r.x = x + val;
		r.y = y + val;
		r.z = z + val;

		return r;
	}

	VEC3D operator - ( REAL32 val) const
	{
		VEC3D r;

		r.x = x - val;
		r.y = y - val;
		r.z = z - val;

		return r;
	}

	VEC3D operator * ( REAL32 val) const
	{
		VEC3D r;

		r.x = x * val;
		r.y = y * val;
		r.z = z * val;

		return r;
	}

	VEC3D operator / ( REAL32 val) const
	{
		VEC3D r;

		r.x = x / val;
		r.y = y / val;
		r.z = z / val;

		return r;
	}

	bool operator < (const VEC3D & p) const
	{
		if( i3Math::isAlike( x, p.x))
		{
			if( i3Math::isAlike( y, p.y))
			{
				if( i3Math::isAlike( z, p.z))
					return false;
				else if( z > p.z)
					return false;
			}
			else if( y > p.y)
			{
				return false;
			}
		}
		else if( x > p.x)
		{
			return false;
		}
		
		return true;
	}

	bool operator == (const VEC3D & p) const
	{
		return i3Math::isAlike( x, p.x) && i3Math::isAlike( y, p.y) && i3Math::isAlike( z, p.z);
	}

	bool operator != (const VEC3D & p) const
	{
		return ! (*this == p);
	}

	bool operator > (const VEC3D & p) const
	{
		return !( (*this) < p || (*this)==p );
	}

	bool operator <= (const VEC3D & p) const
	{
		return ((*this) < p || (*this == p));
	}

	bool operator >= (const VEC3D & p) const
	{
		return !((*this) < p);
	}

	bool	isAlike( const VEC3D & a, REAL32 eps = I3_EPS)	const
	{
		return	i3Math::isAlike( x, a.x, eps) &&
				i3Math::isAlike( y, a.y, eps) &&
				i3Math::isAlike( z, a.z, eps);
	}

	REAL32	LengthSq(void) const
	{
		return (x * x) + (y * y) + (z * z);
	}

	REAL32	Length(void)	const
	{
		return i3Math::Sqrt( LengthSq());
	}

	REAL32	Normalize(void)
	{
		REAL32 len = Length();

		if( len <= I3_EPS)
			return 0.0f;

		REAL32 rlen = 1.0f / len;

		x *= rlen;
		y *= rlen;
		z *= rlen;

		return len;
	}

	bool	isValid(void)	const
	{
		return i3Math::isValid(x) && i3Math::isValid(y) && i3Math::isValid(z);
	}

	VEC4D	Transform(	const MATRIX & mtx) const;
	VEC3D	TransformCoord( const MATRIX & mtx) const;
	VEC3D	TransformNormal( const MATRIX & mtx) const;

	static REAL32	Dot( const VEC3D & v1, const VEC3D & v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	}

	static VEC3D Cross( const VEC3D & v1, const VEC3D & v2)
	{
		VEC3D r;

		r.x = (v1.y * v2.z) - (v1.z * v2.y);
		r.y = (v1.z * v2.x) - (v1.x * v2.z);
		r.z = (v1.x * v2.y) - (v1.y * v2.x);
		
		return r;
	}

	static VEC3D NormalVector( const VEC3D & v1, const VEC3D & v2, const VEC3D & v3)
	{
		VEC3D d1, d2, r;

		d1 = v1 - v2;
		d2 = v3 - v2;

		r = Cross( d2, d1);
		r.Normalize();

		return r;
	}

	static VEC3D Minimize( const VEC3D & v1, const VEC3D & v2)
	{
		VEC3D r;

		r.x = MIN( v1.x, v2.x);
		r.y = MIN( v1.y, v2.y);
		r.z = MIN( v1.z, v2.z);

		return r;
	}

	static VEC3D Maximize( const VEC3D & v1, const VEC3D & v2)
	{
		VEC3D r;

		r.x = MAX( v1.x, v2.x);
		r.y = MAX( v1.y, v2.y);
		r.z = MAX( v1.z, v2.z);

		return r;
	}

	static VEC3D Lerp( const VEC3D & v1, const VEC3D & v2, REAL32 s)
	{
		VEC3D r;

		r.x = v1.x + ((v2.x - v1.x) * s);
		r.y = v1.y + ((v2.y - v1.y) * s);
		r.z = v1.z + ((v2.z - v1.z) * s);

		return r;
	}

	static VEC3D	RandomVector(void)
	{
		VEC3D r;

		i3::pack::VEC3D* pIn = &_random_vector_table[ i3Math::Rand() & 0x00ff];

		r.Set( pIn->x, pIn->y, pIn->z);
		
		return r;
	}

} VECTORALIGN;

////////////////////////////////////////////
// VEC4D 

union I3_EXPORT_MATH VEC4D
{
	struct
	{
		REAL32 x, y, z, w;
	};

	REAL32 vec[4];

	VEC4D(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	VEC4D(REAL32 _x, REAL32 _y, REAL32 _z, REAL32 _w) : x(_x), y(_y), z(_z), w(_w) {}

	void	Set( REAL32 _x, REAL32 _y, REAL32 _z, REAL32 _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	VEC4D & operator = (const VEC4D & p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
		w = p.w;

		return *this;
	}

	VEC4D & operator += ( const VEC4D & a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;

		return *this;
	}

	VEC4D & operator -= ( const VEC4D & a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;

		return *this;
	}

	VEC4D & operator *= ( const VEC4D & a)
	{
		x *= a.x;
		y *= a.y;
		z *= a.z;
		w *= a.w;

		return *this;
	}

	VEC4D & operator /= ( const VEC4D & a)
	{
		x /= a.x;
		y /= a.y;
		z /= a.z;
		w /= a.w;

		return *this;
	}

	VEC4D & operator += ( REAL32 val)
	{
		x += val;
		y += val;
		z += val;
		w += val;

		return *this;
	}

	VEC4D & operator -= ( REAL32 val)
	{
		x -= val;
		y -= val;
		z -= val;
		w -= val;

		return *this;
	}

	VEC4D & operator *= ( REAL32 val)
	{
		x *= val;
		y *= val;
		z *= val;
		w *= val;

		return *this;
	}

	VEC4D & operator /= ( REAL32 val)
	{
		x /= val;
		y /= val;
		z /= val;
		w /= val;

		return *this;
	}

	VEC4D operator + (const VEC4D & a) const
	{
		VEC4D r;

		r.x = x + a.x;
		r.y = y + a.y;
		r.z = z + a.z;
		r.w = w + a.w;

		return r;
	}

	VEC4D operator - (const VEC4D & a) const
	{
		VEC4D r;

		r.x = x - a.x;
		r.y = y - a.y;
		r.z = z - a.z;
		r.w = w - a.w;

		return r;
	}

	VEC4D operator * (const VEC4D & a) const
	{
		VEC4D r;

		r.x = x * a.x;
		r.y = y * a.y;
		r.z = z * a.z;
		r.w = w * a.w;

		return r;
	}

	VEC4D operator / (const VEC4D & a) const
	{
		VEC4D r;

		r.x = x / a.x;
		r.y = y / a.y;
		r.z = z / a.z;
		r.w = w / a.w;

		return r;
	}

	VEC4D operator + ( REAL32 s) const
	{
		VEC4D r;

		r.x = x + s;
		r.y = y + s;
		r.z = z + s;
		r.w = w + s;

		return r;
	}

	VEC4D operator - ( REAL32 s) const
	{
		VEC4D r;

		r.x = x - s;
		r.y = y - s;
		r.z = z - s;
		r.w = w - s;

		return r;
	}

	VEC4D operator * ( REAL32 s) const
	{
		VEC4D r;

		r.x = x * s;
		r.y = y * s;
		r.z = z * s;
		r.w = w * s;

		return r;
	}

	VEC4D operator / ( REAL32 s) const
	{
		VEC4D r;

		r.x = x / s;
		r.y = y / s;
		r.z = z / s;
		r.w = w / s;

		return r;
	}

	bool operator < (const VEC4D & p) const
	{
		if( i3Math::isAlike( x, p.x))
		{
			if( i3Math::isAlike( y, p.y))
			{
				if( i3Math::isAlike( z, p.z))
				{
					if( i3Math::isAlike( w, p.w))
						return false;
					else if( w > p.w)
						return false;
				}
				else if( z > p.z)
					return false;
			}
			else if( y > p.y)
			{
				return false;
			}
		}
		else if( x > p.x)
		{
			return false;
		}
		
		return true;
	}

	bool operator == (const VEC4D & p) const
	{
		return i3Math::isAlike( x, p.x) && i3Math::isAlike( y, p.y) && i3Math::isAlike( z, p.z) && i3Math::isAlike( w, p.w);
	}

	bool operator != (const VEC4D & p) const
	{
		return ! (*this == p);
	}

	bool operator > (const VEC4D & p) const
	{
		return !( (*this) < p || (*this)==p );
	}

	bool operator <= (const VEC4D & p) const
	{
		return ((*this) < p || (*this == p));
	}

	bool operator >= (const VEC4D & p) const
	{
		return !((*this) < p);
	}

	REAL32	LengthSq(void)	const
	{
		return (x*x) + (y*y) + (z*z) + (w*w);
	}

	REAL32	Length(void)	const
	{
		return i3Math::Sqrt( LengthSq());
	}

	REAL32	Normalize(void)
	{
		REAL32 len = Length();

		if( len <= I3_EPS)
			return 0.0f;
		
		REAL32 rlen = 1.0f / len;

		x *= rlen;
		y *= rlen;
		z *= rlen;
		w *= rlen;
		
		return len;
	}

	bool	isValid(void)	const
	{
		return i3Math::isValid(x) && i3Math::isValid(y) && i3Math::isValid(z) && i3Math::isValid(w);
	}

	VEC4D	Transform( const MATRIX & mtx) const;

	static REAL32	Dot( const VEC4D & v1, const VEC4D & v2)
	{
		return	(v1.x * v2.x) + 
				(v1.y * v2.y) +
				(v1.z * v2.z) +
				(v1.w * v2.w);
	}

	static VEC4D	Minimize( const VEC4D & v1, const VEC4D & v2)
	{
		VEC4D r;

		r.x = MIN( v1.x, v2.x);
		r.y = MIN( v1.y, v2.y);
		r.z = MIN( v1.z, v2.z);
		r.w = MIN( v1.w, v2.w);

		return r;
	}

	static VEC4D	Maximize( const VEC4D & v1, const VEC4D & v2)
	{
		VEC4D r;

		r.x = MAX( v1.x, v2.x);
		r.y = MAX( v1.y, v2.y);
		r.z = MAX( v1.z, v2.z);
		r.w = MAX( v1.w, v2.w);

		return r;
	}

	static VEC4D	Lerp( const VEC4D & v1, const VEC4D & v2, REAL32 s)
	{
		VEC4D r;

		r.x = v1.x + ((v2.x - v1.x) * s);
		r.y = v1.y + ((v2.y - v1.y) * s);
		r.z = v1.z + ((v2.z - v1.z) * s);
		r.w = v1.w + ((v2.w - v1.w) * s);

		return r;
	}

} VECTORALIGN;

typedef VEC4D					QUATERNION ALIGN16;


namespace i3Vector
{
	//////////////////////////////////////////////////////////////////////////////////////
	// Vector 2
	inline void Copy( VEC2D * pDest, const VEC2D * pSrc) { *pDest = *pSrc; }
	inline void Copy(i3::pack::VEC2D * pDest, const VEC2D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; }
	inline void Copy(VEC2D * pDest, const i3::pack::VEC2D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; }

	inline
	bool IsSame( VEC2D * pV1, VEC2D * pV2)
	{
		return *pV1 == *pV2;
	}

	/** \brief pV1과 pV2가 비슷한지 확인한다.
		\param[in] pV1 VEC2D 구조체 포인터
		\param[in] pV2 VEC2D 구조체 포인터
		\param[in] eps Epsilon값 (default I3_EPS)
		\return true : 비슷한 vector. false : 다른 vector */
	inline
	bool	isAlike( const VEC2D * pV1, const VEC2D * pV2, REAL32 eps = I3_EPS)
	{
		return	pV1->isAlike( *pV2, eps);
	}

	inline
	void	Set( VEC2D * pVec, REAL32 u, REAL32 v)
	{
		pVec->vec[0] = u;
		pVec->vec[1] = v;
	}

	inline
	void	SetU( VEC2D * pVec, REAL32 val)
	{
		pVec->vec[0] = val;
	}

	inline
	void	SetX( VEC2D * pVec, REAL32 val)
	{
		SetU( pVec, val );
	}

	inline
	void	SetV( VEC2D * pVec, REAL32 val)
	{
		pVec->vec[1] = val;
	}

	inline
	void	SetY( VEC2D * pVec, REAL32 val)
	{
		SetV( pVec, val );
	}

	inline
	REAL32 GetU( VEC2D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetX( VEC2D * pVec)
	{
		return GetU( pVec );
	}

	inline
	REAL32 GetV( VEC2D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetY( VEC2D * pVec)
	{
		return GetV( pVec );
	}

	inline
	void	Zero( VEC2D * pVec)
	{
		pVec->vec[0] = 0.0f;
		pVec->vec[1] = 0.0f;
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 348.477 ms
	// Vfpu unUse: 1914.200 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	// Vfpu unUse(function으로 호출): 2043.208 ms - 안에서 Sqrt가 쓰였음(1862.623 ms)
	/////////////////////////////////////////////////////////////////
	inline
	REAL32 Length( const VEC2D * pV )
	{
		return pV->Length();
	}

	inline
	REAL32 LengthSq( const VEC2D *pV )
	{
		return pV->LengthSq();
	}

	inline
	REAL32 Dot( const VEC2D *pV1, const VEC2D *pV2 )
	{
		return VEC2D::Dot( *pV1, *pV2);
	}

	inline
	void Add( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		*pOut = *pV1 + *pV2;
	}

	inline void Sub( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		*pOut = *pV1 - *pV2;
	}
	inline void Sub(VEC2D *pOut, const VEC2D *pV1, const i3::pack::VEC2D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
	}
	inline void Sub(i3::pack::VEC2D * pOut, const VEC2D * pV1, const VEC2D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
	}

	inline
	void	Mul( VEC2D * pOut, const VEC2D * pV1, const VEC2D * pV2)
	{
		*pOut = *pV1 * *pV2;
	}

	inline
	void Minimize( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		*pOut = VEC2D::Minimize( *pV1, *pV2);
	}

	inline
	void Maximize( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2 )
	{
		*pOut = VEC2D::Maximize( *pV1, *pV2);
	}

	inline
	void Scale( VEC2D *pOut, const VEC2D *pV, REAL32 s )
	{
		*pOut = *pV * s;
	}

	inline
	void ScaleAdd( VEC2D *pOut, const VEC2D *pV, REAL32 s )
	{
		*pOut += (*pV * s);
	}

	inline
	void Lerp( VEC2D *pOut, const VEC2D *pV1, const VEC2D *pV2, REAL32 s )
	{
		*pOut = VEC2D::Lerp( *pV1, *pV2, s);
	}

	inline
	REAL32 Normalize( VEC2D *pOut, const VEC2D *pV )
	{
		*pOut = *pV;
		return pOut->Normalize();
	}
	inline REAL32 Normalize(i3::pack::VEC2D * pOut, const i3::pack::VEC2D * pV)
	{
		VEC2D vec(pV->x, pV->y);
		REAL32 rv = vec.Normalize();
		pOut->x = vec.x;
		pOut->y = vec.y;
		return rv;
	}

	inline
	void Transform( VEC4D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		*pOut = pV->Transform( *pM);
	}

	inline
	void TransformCoord( VEC2D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		*pOut = pV->TransformCoord( *pM);
	}

	inline
	void TransformNormal( VEC2D *pOut, const VEC2D *pV, const MATRIX *pM )
	{
		*pOut = pV->TransformNormal( *pM);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Vector 3
	inline
	void	Copy( VEC3D * pDest, const VEC3D * pSrc)
	{
		*pDest = *pSrc;
	}
	inline void Copy(i3::pack::VEC3D * pDest, const VEC3D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; pDest->z = pSrc->z; }
	inline void Copy(VEC3D * pDest, const i3::pack::VEC3D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; pDest->z = pSrc->z; }

	inline
	bool IsSame( VEC3D * pV1, VEC3D * pV2)
	{
		return *pV1 == *pV2;
	}

	inline
	bool	isAlike( VEC3D * pV1, VEC3D * pV2, REAL32 eps = I3_EPS)
	{
		return pV1->isAlike( *pV2, eps);
	}

	inline
	void	Set( VEC3D * pVec, REAL32 x, REAL32 y, REAL32 z)
	{
		pVec->vec[0] = x;
		pVec->vec[1] = y;
		pVec->vec[2] = z;
	}

	inline
	void	SetX( VEC3D * pVec, REAL32 x)
	{
		pVec->vec[0] = x;
	}

	inline
	void	SetY( VEC3D * pVec, REAL32 y)
	{
		pVec->vec[1] = y;
	}

	inline
	void	SetZ( VEC3D * pVec, REAL32 z)
	{
		pVec->vec[2] = z;
	}

	inline
	void	Zero( VEC3D * pVec)
	{
		pVec->vec[0] = pVec->vec[1] = pVec->vec[2] = 0.0f;
	}

	inline
	REAL32	GetX( VEC3D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetY( VEC3D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetZ( VEC3D * pVec)
	{
		return pVec->vec[2];
	}

	inline
	void AddX( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[0] += r;
	}

	inline
	void AddY( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[1] += r;
	}

	inline
	void AddZ( VEC3D * pVec, REAL32 r)
	{
		pVec->vec[2] += r;
	}

	inline
	void Add( VEC2D * pVec, REAL32 x, REAL32 y)
	{
		pVec->vec[0] += x;
		pVec->vec[1] += y;
	}

	inline
	void AddX( VEC2D * pVec, REAL32 r)
	{
		pVec->vec[0] += r;
	}

	inline
	void AddY( VEC2D * pVec, REAL32 r)
	{
		pVec->vec[1] += r;
	}

	inline
	void AddZ( VEC2D * pVec, REAL32 r)
	{
		pVec->vec[2] += r;
	}

	inline
	void Add( VEC3D * pVec, REAL32 x, REAL32 y, REAL32 z)
	{
		pVec->vec[0] += x;
		pVec->vec[1] += y;
		pVec->vec[2] += z;
	}

	inline
	void Mul( VEC3D * pOut, VEC3D * pVec1, VEC3D * pVec2)
	{
		*pOut = *pVec1 * *pVec2;
	}

	inline
	REAL32 LengthSq( const VEC3D *pV )
	{
		return pV->LengthSq();
	}

	inline
	REAL32 Length( const VEC3D *pV )
	{
		return pV->Length();
	}

	inline
	REAL32 Dot( const VEC3D *pV1, const VEC3D *pV2 )
	{
		return VEC3D::Dot( *pV1, *pV2);
	}

	inline
	void Cross( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		*pOut = VEC3D::Cross( *pV1, *pV2);
	}

	inline
	void Add( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		*pOut = *pV1 + *pV2;
	}

	inline
	void Sub( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		*pOut = *pV1 - *pV2;
	}
	inline void Sub(VEC3D * pOut, const VEC3D * pV1, const i3::pack::VEC3D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
	}
	inline void Sub(VEC3D * pOut, const i3::pack::VEC3D * pV1, const VEC3D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
	}
	inline void Sub(VEC3D * pOut, const i3::pack::VEC3D * pV1, const i3::pack::VEC3D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
	}
	inline void Sub(i3::pack::VEC3D * pOut, const VEC3D * pV1, const VEC3D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
	}

	inline
	void Minimize( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		*pOut = VEC3D::Minimize( *pV1, *pV2);
	}

	inline
	void Maximize( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 )
	{
		*pOut = VEC3D::Maximize( *pV1, *pV2);
	}

	inline
	void Scale( VEC3D *pOut, const VEC3D *pV, REAL32 s)
	{
		*pOut = *pV * s;
	}
	inline void Scale(VEC3D * pOut, const i3::pack::VEC3D * pV, REAL32 s)
	{
		pOut->x = pV->x * s;
		pOut->y = pV->y * s;
		pOut->z = pV->z * s;
	}

	inline
	void ScaleAdd( VEC3D *pOut, const VEC3D *pV, REAL32 s)
	{
		*pOut += (*pV * s);
	}

	inline
	void MulAdd( VEC3D *pOut, const VEC3D *pV1, VEC3D * pV2)
	{
		*pOut += (*pV1 * *pV2);
	}

	inline
	void Lerp( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2, REAL32 s )
	{
		*pOut = VEC3D::Lerp( *pV1, *pV2, s);
	}

	inline
	REAL32 Normalize( VEC3D *pOut, const VEC3D *pV )
	{
		*pOut = *pV;

		return pOut->Normalize();
	}
	inline REAL32 Normalize(i3::pack::VEC3D * pOut, const i3::pack::VEC3D * pV)
	{
		VEC3D vec(pV->x, pV->y, pV->z);
		REAL32 rv = vec.Normalize();
		pOut->x = vec.x;
		pOut->y = vec.y;
		pOut->z = vec.z;
		return rv;
	}

	inline
	void Normal( VEC3D *pOut, const VEC3D *pV1, const VEC3D *pV2 , const VEC3D *pV3 )
	{
		*pOut = VEC3D::NormalVector( *pV1, *pV2, *pV3);
	}

	inline
	void Transform( VEC4D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		*pOut = pV->Transform( *pM);
	}

	inline
	void TransformCoord( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		*pOut = pV->TransformCoord( *pM);
	}

	inline
	void TransformNormal( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM )
	{
		*pOut = pV->TransformNormal( *pM);
	}

	#if defined( I3_DEBUG)
	inline
	void	Dump( VEC3D * pVec)
	{
		I3TRACE( "%f, %f, %f\n", i3Vector::GetX( pVec), i3Vector::GetY(pVec), i3Vector::GetZ(pVec));
	}
	inline
	void	Dump( VEC2D * pVec)
	{
		I3TRACE( "%f, %f\n", i3Vector::GetX( pVec), i3Vector::GetY(pVec) );
	}
	#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Vector 4
	inline void	Copy( VEC4D * pDest, const VEC4D * pSrc) { *pDest = *pSrc; }
	inline void Copy(i3::pack::VEC4D * pDest, const VEC4D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; pDest->z = pSrc->z; pDest->w = pSrc->w; }
	inline void Copy(VEC4D * pDest, const i3::pack::VEC4D * pSrc) { pDest->x = pSrc->x; pDest->y = pSrc->y; pDest->z = pSrc->z; pDest->w = pSrc->w; }
	
	inline
	bool IsSame( VEC4D * pV1, VEC4D * pV2)
	{
		return	*pV1 == *pV2;
	}

	inline
	void	Set( VEC4D * pVec, REAL32 x, REAL32 y, REAL32 z, REAL32 w)
	{
		pVec->vec[0] = x;
		pVec->vec[1] = y;
		pVec->vec[2] = z;
		pVec->vec[3] = w;
	}

	inline
	void	SetX( VEC4D * pVec, REAL32 x)
	{
		pVec->vec[0] = x;
	}

	inline
	void	SetY( VEC4D * pVec, REAL32 y)
	{
		pVec->vec[1] = y;
	}

	inline
	void	SetZ( VEC4D * pVec, REAL32 z)
	{
		pVec->vec[2] = z;
	}

	inline
	void	SetW( VEC4D * pVec, REAL32 w)
	{
		pVec->vec[3] = w;
	}

	inline
	void	Zero( VEC4D * pVec)
	{
		pVec->vec[0] = 0.0f;
		pVec->vec[1] = 0.0f;
		pVec->vec[2] = 0.0f;
		pVec->vec[3] = 0.0f;
	}

	inline
	REAL32	GetX( VEC4D * pVec)
	{
		return pVec->vec[0];
	}

	inline
	REAL32 GetY( VEC4D * pVec)
	{
		return pVec->vec[1];
	}

	inline
	REAL32 GetZ( VEC4D * pVec)
	{
		return pVec->vec[2];
	}

	inline
	REAL32 GetW( VEC4D * pVec)
	{
		return pVec->vec[3];
	}

	inline
	REAL32 LengthSq( const VEC4D *pV )
	{
		return pV->LengthSq();
	}

	inline
	REAL32 Length( const VEC4D *pV )
	{
		return pV->Length();
	}

	inline
	REAL32 Dot( const VEC4D *pV1, const VEC4D *pV2 )
	{
		return VEC4D::Dot( *pV1, *pV2);
	}

	inline
	void Add( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		*pOut = *pV1 + *pV2;
	}

	inline
	void Sub( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		*pOut = *pV1 - *pV2;
	}
	inline void Sub(i3::pack::VEC4D * pOut, const VEC4D * pV1, const VEC4D *pV2)
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
		pOut->w = pV1->w - pV2->w;
	}

	inline
	void Minimize( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		*pOut = VEC4D::Minimize( *pV1, *pV2);
	}

	inline
	void Maximize( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2 )
	{
		*pOut = VEC4D::Maximize( *pV1, *pV2);
	}

	inline
	void Scale( VEC4D *pOut, const VEC4D *pV, REAL32 s)
	{
		*pOut = *pV * s;
	}

	inline
	void Lerp( VEC4D *pOut, const VEC4D *pV1, const VEC4D *pV2, REAL32 s )
	{
		*pOut = VEC4D::Lerp( *pV1, *pV2, s);
	}

	inline
	REAL32 Normalize( VEC4D *pOut, const VEC4D *pV )
	{
		*pOut = *pV;

		return pOut->Normalize();
	}
	inline REAL32 Normalize(i3::pack::VEC4D * pOut, const i3::pack::VEC4D *pV)
	{
		VEC4D vec(pV->x, pV->y, pV->z, pV->w);
		REAL32 rv = vec.Normalize();
		pOut->x = vec.x;
		pOut->y = vec.y;
		pOut->z = vec.z;
		pOut->w = vec.w;
		return rv;
	}

	inline
	void Transform( VEC4D *pOut, const VEC4D *pV, const MATRIX *pM )
	{
		*pOut = pV->Transform( *pM);
	}

	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	inline
	void GetRandVector(VEC3D* pOut)
	{
		*pOut = VEC3D::RandomVector();
	}

	// 2D 좌표계로 만들어 줌.
	// 정수화한 후 0.5f를 빼줌.
	inline void Make2DCoord( VEC2D *pVec )
	{
		pVec->vec[0] = INT32( pVec->vec[0] ) - 0.5f;
		pVec->vec[1] = INT32( pVec->vec[1] ) - 0.5f;
	}

	// 2D 좌표계로 만들어 줌.
	// 정수화한 후 0.5f를 빼줌.
	inline void Make2DCoord( VEC3D *pVec )
	{
		pVec->vec[0] = INT32( pVec->vec[0] ) - 0.5f;
		pVec->vec[1] = INT32( pVec->vec[1] ) - 0.5f;
		pVec->vec[2] = INT32( pVec->vec[2] ) - 0.5f;
	}

	enum CONVERT_MODE
	{
		CONVERT_MODE_XY,
		CONVERT_MODE_XYZ = CONVERT_MODE_XY,
		CONVERT_MODE_XZ,
		CONVERT_MODE_XZY = CONVERT_MODE_XZ,
		CONVERT_MODE_MINUS_X_Z,
		CONVERT_MODE_MINUS_X_Z_Y = CONVERT_MODE_MINUS_X_Z,
	};

	// 2D 좌표를 3D로 변환
	inline
	VEC3D *	Convert( VEC3D * pDest, VEC2D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			{
				Set( pDest, GetX( pSrc ), GetY( pSrc ), 0.0f );
			}
			break;
		case CONVERT_MODE_XZY:
			{
				Set( pDest, GetX( pSrc ), 0.0f, GetY( pSrc ) );
			}
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			{
				Set( pDest, -1.0f * GetX( pSrc ), 0.0f, GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0.0f, 0.0f, 0.0f );
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	// 3D 좌표를 3D로 좌표계 변환
	inline
	VEC3D *	Convert( VEC3D * pDest, VEC3D * pSrc, INT32 nMode )
	{
		switch( nMode )
		{
		case CONVERT_MODE_XYZ:
			{
				Set( pDest, GetX( pSrc ), GetY( pSrc ), GetZ( pSrc ) );
			}
			break;
		case CONVERT_MODE_XZY:
			{
				Set( pDest, GetX( pSrc ), GetZ( pSrc ), GetY( pSrc ) );
			}
			break;
		case CONVERT_MODE_MINUS_X_Z_Y:
			{
				Set( pDest, -1.0f * GetX( pSrc ), GetZ( pSrc ), GetY( pSrc ) );
			}
			break;
		default:
			{
				Set( pDest, 0.0f, 0.0f, 0.0f );
				I3ASSERT_0;
			}
			break;
		}

		return pDest;
	}

	inline
	bool	isValid( const VEC3D * pSrc)
	{
		return pSrc->isValid();
	}

	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec);
	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec);
	I3_EXPORT_MATH	bool	SetToXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec);

	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC2D * pVec);
	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC3D * pVec);
	I3_EXPORT_MATH	bool	GetFromXML( i3XMLElement * pXML, const char * pszName, VEC4D * pVec);
};

#define setU( pVec, u)					i3Vector::SetU( (pVec), (u))
#define setV( pVec, v)					i3Vector::SetV( (pVec), (v))
#define setUV( pVec, u, v)				i3Vector::Set( (pVec), (u), (v))
#define getU( pVec)						i3Vector::GetU( (pVec))
#define getV( pVec)						i3Vector::GetV( (pVec))

#define setX( pVec, x)					i3Vector::SetX( (pVec), x)
#define setY( pVec, y)					i3Vector::SetY( (pVec), y)
#define setZ( pVec, z)					i3Vector::SetZ( (pVec), z)
#define setW( pVec, w)					i3Vector::SetW( (pVec), w)

#define setXYZ( pVec, x, y, z)			i3Vector::Set( (pVec), (x), (y), (z))
#define getX( pVec)						i3Vector::GetX( (pVec))
#define getY( pVec)						i3Vector::GetY( (pVec))
#define getZ( pVec)						i3Vector::GetZ( (pVec))
#define getW( pVec)						i3Vector::GetW( (pVec))

#define addX( pVec, r)					i3Vector::AddX( (pVec), r)
#define addY( pVec, r)					i3Vector::AddY( (pVec), r)
#define addZ( pVec, r)					i3Vector::AddZ( (pVec), r)
#define addXYZ( pVec, x, y, z)			i3Vector::Add( (pVec), x, y, z)

extern I3_EXPORT_MATH VEC3D			I3_XAXIS;
extern I3_EXPORT_MATH VEC3D			I3_YAXIS;
extern I3_EXPORT_MATH VEC3D			I3_ZAXIS;
extern I3_EXPORT_MATH VEC3D			I3_ZEROVECTOR;

extern I3_EXPORT_MATH MATRIX		I3_IDENTITYMATRIX;

#endif
////////////
