// i3GraphicType.h

#ifndef __I3_MATH_TYPE_H
#define __I3_MATH_TYPE_H

#include "i3Base.h"

#if defined( I3G_DX)
	#include <d3d9.h>
	#include <d3dx9math.h>
#elif defined( I3G_XBOX)
	#include <XGMath.h>
#elif defined( I3G_PSP)
	#include <psptypes.h>
	#include <libvfpu.h>
#endif

#if ( defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL )
	#if defined( I3_MATH_BUILD)
		#define	I3_EXPORT_MATH		__declspec( dllexport)
		#define I3_EXTERN_MATH		extern
	#else
		#define I3_EXPORT_MATH		__declspec( dllimport)
		#define I3_EXTERN_MATH		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_MATH)
		#define I3_EXPORT_MATH
		#define I3_EXTERN_MATH		extern
	#endif
#endif

/////////////////////////////////////////////////////////////
#define I3_PI4				0.78539816339744830962f		/* PI / 4		*/
#define	I3_PI2				1.57079632679489661923f		/* PI / 2		*/
#define	I3_PI				3.14159265358979323846f		/* PI			*/
#define	I3_2PI				6.28318530717958647692f		/* 2 * PI		*/
#define I3_EPS				0.00002f
#define I3_EPS_REAL16		4.88e-04f
#define I3_EPS_REAL32		5.96e-08f
#define	I3_EPS_REAL64		1.11e-16
#define I3_E				2.7182818284590452354f		/* e            */
#define I3_LOG2E			1.4426950408889634074f		/* log2(e)      */
#define I3_LOG10E			0.43429448190325182765f		/* log10(e)     */
#define I3_LN2				0.69314718055994530942f		/* ln(2)        */
#define I3_LN10				2.30258509299404568402f		/* ln(10)       */

#define I3_DEG2RAD( deg)			((deg) * I3_PI * 0.0055555555555555555555555555555556f)
#define I3_RAD2DEG( rad)			((rad) * 180.0f * 0.31830988618379067153803535746773f)


typedef struct I3COLOR
{
	UINT8	r, g, b, a;

	UINT32	GetARGB(void) const
	{
		return  ((a << 24) & 0xFF000000) |
				((r << 16) & 0x00FF0000) |
				((g << 8)  & 0x0000FF00) |
				((b)       & 0x000000FF);
	}

	UINT32	GetRGBA(void) const
	{
		return  ((a << 24) & 0xFF000000) |
				((b << 16) & 0x00FF0000) |
				((g <<  8) & 0x0000FF00) |
				((r      ) & 0x000000FF);
	}

	void	SetARGB( UINT32 val)
	{
		a = (UINT8)((val >> 24) & 0xFF);
		r = (UINT8)((val >> 16) & 0xFF);
		g = (UINT8)((val >> 8)  & 0xFF);
		b = (UINT8)((val)		& 0xFF);
	}

	void	SetRGBA( UINT32 val)
	{
		a = (UINT8)((val >> 24) & 0xFF);
		b = (UINT8)((val >> 16) & 0xFF);
		g = (UINT8)((val >> 8)  & 0xFF);
		r = (UINT8)((val)		& 0xFF);
	}

} I3COLOR;

typedef union _i3UnionColor
{
	struct
	{
		UINT8 r, g, b, a;
	} rgba;

	UINT32 value;

} I3UNIONCOLOR;

/////////////////////////////////////////////////////////////
// DirectX
#if defined (I3G_DX)	

/*
typedef D3DXMATRIX	MATRIX;
typedef D3DXVECTOR2				VEC2D;
typedef D3DXVECTOR3	VEC3D;
typedef D3DXVECTOR4				VEC4D;
typedef I3COLOR					COLOR;
typedef D3DXCOLOR				COLORREAL;
typedef D3DXQUATERNION			QUATERNION;
*/
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// PSP
#if defined (I3_PSP) 
#define VECTORALIGN			ALIGN16
#define VECTORALIGN0
#elif defined( I3G_DX) || defined( I3G_XBOX)
#define	VECTORALIGN			ALIGN16
#define	VECTORALIGN0		__declspec( align(16))
#elif defined( I3_WINCE )
#define VECTORALIGN			ALIGN16
#define VECTORALIGN0		
#elif defined( I3G_OGLES)
#define VECTORALIGN			ALIGN4
#endif

#include "i3MathDefine.h"

union VEC2D
{
	struct
	{
		REAL32 x, y;
	};

	REAL32 vec[2];

	VEC2D & operator = (const VEC2D & p)
	{
		x = p.x;
		y = p.y;

		return *this;
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

	bool operator == (const VEC2D & p) const
	{
		return i3Math::isAlike( x, p.x) && i3Math::isAlike( y, p.y);
	}

	bool operator != (const VEC2D & p) const
	{
		return ! (*this == p);
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

}  VECTORALIGN;

union VEC3D
{
	struct
	{
		#if defined( I3_PSP)
			REAL32 x, y, z, pad;
		#else
			REAL32 x, y, z;
		#endif
	};

	#if defined( I3_PSP)
		REAL32 vec[4];
	#else
		REAL32 vec[3];
	#endif

	VEC3D & operator = (const VEC3D & p)
	{
		x = p.x;
		y = p.y;
		z = p.z;

		return *this;
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

} VECTORALIGN;

union VEC4D
{
	struct
	{
		REAL32 x, y, z, w;
	};

	REAL32 vec[4];

	VEC4D & operator = (const VEC4D & p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
		w = p.w;

		return *this;
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
} VECTORALIGN;

typedef I3COLOR					COLOR;

struct COLORREAL
{
	REAL32 r, g, b, a;

	COLORREAL & operator = (const COLORREAL & p)
	{
		r = p.r;
		g = p.g;
		b = p.b;
		a = p.a;

		return *this;
	}

	bool operator < (const COLORREAL & p) const
	{
		if( i3Math::isAlike( r, p.r))
		{
			if( i3Math::isAlike( g, p.g))
			{
				if( i3Math::isAlike( b, p.b))
				{
					if( i3Math::isAlike( a, p.a))
						return false;
					else if( a > p.a)
						return false;
				}
				else if( b > p.b)
					return false;
			}
			else if( g > p.g)
			{
				return false;
			}
		}
		else if( r > p.r)
		{
			return false;
		}
		
		return true;
	}

	bool operator == (const COLORREAL & p) const
	{
		return i3Math::isAlike( r, p.r) && i3Math::isAlike( g, p.g) && i3Math::isAlike( b, p.b) && i3Math::isAlike( a, p.a);
	}

	bool operator != (const COLORREAL & p) const
	{
		return ! (*this == p);
	}

	bool operator > (const COLORREAL & p) const
	{
		return !( (*this) < p || (*this)==p );
	}

	bool operator <= (const COLORREAL & p) const
	{
		return ((*this) < p || (*this == p));
	}

	bool operator >= (const COLORREAL & p) const
	{
		return !((*this) < p);
	}
} VECTORALIGN;

typedef VEC4D					QUATERNION ALIGN16;

typedef VECTORALIGN0 union _tagi3Matrix
{
	struct
	{
		REAL32	m00, m01, m02, m03;
		REAL32	m10, m11, m12, m13;
		REAL32	m20, m21, m22, m23;
		REAL32	m30, m31, m32, m33;
	};

	struct
	{
		VEC4D vec[4];
	};
	
	REAL32 m[4][4];
	REAL32 v[16];
	
} MATRIX VECTORALIGN;

/////////////////////////////////////////////////////////////

typedef struct _i3VEC2DFixed32
{
	FIXED32 x;
	FIXED32 y;
} FIXED32_VEC2D;

typedef struct _i3VEC3DFixed32
{
	FIXED32 x;
	FIXED32 y;
	FIXED32 z;
} FIXED32_VEC3D;

typedef struct _i3VEC4DFixed32
{
	FIXED32 x;
	FIXED32 y;
	FIXED32 z;
	FIXED32 w;
} FIXED32_VEC4D;

typedef struct _i3QUATFixed32
{
	FIXED32 x;
	FIXED32 y;
	FIXED32 z;
	FIXED32 w;
} FIXED32_QUAT;

typedef struct PACKED _i3COLORREALFixed32
{
	FIXED32 r;
	FIXED32 g;
	FIXED32 b;
	FIXED32 a;
} FIXED32_COLORREAL;

#if defined( I3_COMPILER_VC)
#pragma warning( disable : 4201 )
#endif

typedef union _tagi3MatrixFixed32
{
	struct
	{
		FIXED32	m00, m01, m02, m03;
		FIXED32	m10, m11, m12, m13;
		FIXED32	m20, m21, m22, m23;
		FIXED32	m30, m31, m32, m33;
	};

	struct
	{
		FIXED32_VEC4D vec[4];
	};
	
	FIXED32 m[4][4];
	
} FIXED32_MATRIX;

////////////////////////////////////////////////////////////
// Persistant Format
#if defined( I3_COMPILER_VC)
#pragma pack(push, 1)
#endif

typedef struct PACKED _i3VEC2DPersist
{
	REAL32 x;
	REAL32 y;
} PERSIST_VEC2D;

typedef struct PACKED _i3VEC3DPersist
{
	REAL32 x;
	REAL32 y;
	REAL32 z;
} PERSIST_VEC3D;

typedef struct PACKED  _i3VEC4DPersist
{
	REAL32 x;
	REAL32 y;
	REAL32 z;
	REAL32 w;
} PERSIST_VEC4D;

typedef struct PACKED _i3COLORREALPersist
{
	REAL32 r;
	REAL32 g;
	REAL32 b;
	REAL32 a;
} PERSIST_COLORREAL;

struct PACKED PERSIST_MATRIX
{
	PERSIST_VEC4D	mat[4];
} ;

struct PACKED I3RECT
{
	REAL32 left;
	REAL32 top;
	REAL32 right;
	REAL32 bottom;

	REAL32		Width(void)
	{
		return right - left + 1.0f;
	}

	REAL32		Height(void)
	{
		return bottom - top + 1.0f;
	}
};

struct PACKED POINT2D
{
	INT32 x;
	INT32 y;
};

typedef struct PACKED _Point3D
{
	INT32 x;
	INT32 y;
	INT32 z;
} POINT3D;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#if defined( I3_FIXED_POINT)
	typedef FIXED32_VEC2D			RT_VEC2D;
	typedef FIXED32_VEC3D			RT_VEC3D;
	typedef FIXED32_VEC4D			RT_VEC4D;
	typedef FIXED32_QUAT			RT_QUAT;
	typedef FIXED32_MATRIX			RT_MATRIX;
#else
	typedef VEC2D					RT_VEC2D;
	typedef VEC3D	RT_VEC3D;
	typedef VEC4D					RT_VEC4D;
	typedef QUATERNION				RT_QUAT;
	typedef MATRIX					RT_MATRIX;
#endif
#endif
