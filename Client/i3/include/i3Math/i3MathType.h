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


/////////////////////////////////////////////////////////////
// DirectX
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// PSP
#if defined (I3_PSP) 
#define VECTORALIGN			ALIGN16
#define VECTORALIGN0
#elif defined( I3G_DX) || defined( I3G_XBOX)
#define VETORALIGN_SIZE		16
#define	VECTORALIGN			ALIGN16
#define	VECTORALIGN0		__declspec( align(VETORALIGN_SIZE) )
#elif defined( I3_WINCE )
#define VECTORALIGN			ALIGN16
#define VECTORALIGN0		
#elif defined( I3G_OGLES)
#define VECTORALIGN			ALIGN4
#endif

/////////////////////////////////////////////////////////////

#if defined( I3_COMPILER_VC)
#pragma warning( disable : 4201 )
#endif

////////////////////////////////////////////////////////////
// Persistant Format
#if defined( I3_COMPILER_VC)
#pragma pack(push, 1)
#endif

namespace i3
{
	namespace pack
	{
		struct PACKED VEC2D
		{
			REAL32 x = 0.0f;
			REAL32 y = 0.0f;
			VEC2D() {}
			VEC2D(REAL32 _x, REAL32 _y) { x = _x; y = _y; }
		};

		struct PACKED VEC3D
		{
			REAL32 x = 0.0f;
			REAL32 y = 0.0f;
			REAL32 z = 0.0f;
			VEC3D() {}
			VEC3D(REAL32 _x, REAL32 _y, REAL32 _z) { x = _x; y = _y; z = _z; }
		};

		struct PACKED VEC4D
		{
			REAL32 x = 0.0f;
			REAL32 y = 0.0f;
			REAL32 z = 0.0f;
			REAL32 w = 0.0f;
			VEC4D() {}
			VEC4D(REAL32 _x, REAL32 _y, REAL32 _z, REAL32 _w) { x = _x; y = _y; z = _z; w = _w; }
		};

		struct PACKED COLORREAL
		{
			REAL32 r = 0.0f;
			REAL32 g = 0.0f;
			REAL32 b = 0.0f;
			REAL32 a = 0.0f;
			COLORREAL() {}
			COLORREAL(REAL32 _r, REAL32 _g, REAL32 _b, REAL32 _a) { r = _r; g = _g; b = _b; a = _a; }
		};

		struct PACKED MATRIX
		{
			VEC4D	mat[4];

			MATRIX()
			{
				mat[0].x = 1.0f; mat[1].y = 1.0f; mat[2].z = 1.0f; mat[3].w = 1.0f;
			}
		};

		struct PACKED RECT
		{
			REAL32 left = 0.0f;
			REAL32 top = 0.0f;
			REAL32 right = 0.0f;
			REAL32 bottom = 0.0f;

			RECT() {}
			RECT(REAL32 l, REAL32 t, REAL32 r, REAL32 b) : left(l), top(t), right(r), bottom(b) {}

			REAL32		Width(void)
			{
				return right - left + 1.0f;
			}

			REAL32		Height(void)
			{
				return bottom - top + 1.0f;
			}

			void		Normalize(void)
			{
				REAL32 temp;

				if (left > right)
				{
					temp = right;
					right = left;
					left = temp;
				}

				if (top > bottom)
				{
					temp = bottom;
					bottom = top;
					top = temp;
				}
			}
		};

		struct PACKED POINT2D
		{
			INT32 x = 0;
			INT32 y = 0;
			POINT2D() {}
			POINT2D(INT32 _x, INT32 _y) { x = _x; y = _y; }
		};

		struct PACKED POINT3D
		{
			INT32 x = 0;
			INT32 y = 0;
			INT32 z = 0;
			POINT3D() {}
			POINT3D(INT32 _x, INT32 _y, INT32 _z) { x = _x; y = _y; z = _z; }
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
