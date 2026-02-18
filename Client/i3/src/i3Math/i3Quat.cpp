#include "i3MathType.h"
#include "i3MathDefine.h"
#include "i3Quat.h"

void i3QuaternionFromMatrix( QUATERNION * pQuat, MATRIX * pMtx)
{
	REAL32 x, y, z, w;
	REAL32 r, t;
	const VEC4D *pv = ((const MATRIX *)pMtx)->vec;

	t = pv[0].x + pv[1].y + pv[2].z + 1.0f;

	if ( t > 0.01f ) 
	{
		w = i3Math::Sqrt( t ) * 0.5f;
		r = 0.25f / w;
		x = ( pv[1].z - pv[2].y ) * r;
		y = ( pv[2].x - pv[0].z ) * r;
		z = ( pv[0].y - pv[1].x ) * r;
	} 
	else if ( pv[0].x > pv[1].y ) 
	{
		if ( pv[0].x > pv[2].z ) 
		{
			x = i3Math::Sqrt( 1.0f + pv[0].x - pv[1].y - pv[2].z ) * 0.5f;
			r = 0.25f / x;
			y = ( pv[1].x + pv[0].y ) * r;
			z = ( pv[2].x + pv[0].z ) * r;
			w = ( pv[1].z - pv[2].y ) * r;
		} 
		else 
		{
			z = i3Math::Sqrt( 1.0f + pv[2].z - pv[0].x - pv[1].y ) * 0.5f;
			r = 0.25f / z;
			x = ( pv[2].x + pv[0].z ) * r;
			y = ( pv[2].y + pv[1].z ) * r;
			w = ( pv[0].y - pv[1].x ) * r;
		}
	} 
	else 
	{
		if ( pv[1].y > pv[2].z ) 
		{
			y = i3Math::Sqrt( 1.0f + pv[1].y - pv[2].z - pv[0].x ) * 0.5f;
			r = 0.25f / y;
			x = ( pv[1].x + pv[0].y ) * r;
			z = ( pv[2].y + pv[1].z ) * r;
			w = ( pv[2].x - pv[0].z ) * r;
		} 
		else 
		{
			z = i3Math::Sqrt( 1.0f + pv[2].z - pv[0].x - pv[1].y ) * 0.5f;
			r = 0.25f / z;
			x = ( pv[2].x + pv[0].z ) * r;
			y = ( pv[2].y + pv[1].z ) * r;
			w = ( pv[0].y - pv[1].x ) * r;
		}
	}
	pQuat->vec[0] = x;
	pQuat->vec[1] = y;
	pQuat->vec[2] = z;
	pQuat->vec[3] = w;
}

#if defined( I3G_PSP)
void i3QuaternionSLERP( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t)
{
#if 1
	__asm__ (
		".set			push\n"					// save assembler option
		".set			noreorder\n"			// suppress reordering
		"mfc1			$8,   %3\n"				// t0   = t
		"mtv			$8,   s031\n"			// s031 = t0 = t
		"lv.q			c010, %1\n"				// c010 = *pq1
		"lv.q			c020, %2\n"				// c020 = *pq2
		"vcst.s			s001, VFPU_SQRT1_2\n"	// s001 = VFPU_SQRT1_2 = 1 / sqrt(2)
		"vdot.q			s000[-1:1], c010, c020\n"	// s000 = c010 dot c020 = *pq0 dot *pq1
		"vfim.s			s002, 0.998047\n"		// s002 = 0.998047
		"vcmp.s			GE, s000, s002\n"		// VFPU_CC[0] = (s000 >= 0.998047)
		"bvt			0,    1f\n"				// if (CC[0]!=0) goto 1f
		"vocp.s			s030, s031\n"			// s030 = 1 - s031    = (1 - t)					(delay slot)
		"vcmp.s			LT, s000[|x|], s001\n"	// CC[0] = |s000| < s001
		"vasin.s		s032, s000[|x|]\n"		// s032 = asin(abs(s000))
		"bvtl			0,    0f\n"				// if (CC[0]!=0) goto 0f
		"vocp.s			s032, s032\n"			// s032 = 1 - s032    = acos(abs(s000)) = angle	(delay slot)

		"vmul.s			s001, s000, s000\n"		// s001 = s000 * s000
		"vocp.s			s001, s001\n"			// s001 = 1 - s001
		"vsqrt.s		s001, s001\n"			// s001 = sqrt(s001)
		"vasin.s		s032, s001\n"			// s032 = asin(s001)  = asin(sqrt(1-s000*s000))
												//                    = acos(abs(s000)) = angle
	"0:\n"
		"vzero.s		s001\n"					// s001 = 0
		"lvi.s			s002, 0.00005\n"		// s002 = EPSILON
		"vscl.p			c030, c030, s032\n"		// s030 = s030 * s032 = (1 - t) * angle
												// s031 = s031 * s032 = t       * angle
		"vcmp.s			LT,   s000, s001\n"		// VFPU_CC[0] = (s000 < 0)
		"vsin.t			c030, c030\n"			// s030 = sin(s030)   = sin((1 - t) * angle)
												// s031 = sin(s031)   = sin(t)      * angle)
												// s032 = sin(s032)   = sin(angle)
		"vcmovt.q		c020, c020[-x,-y,-z,-w], 0\n"	// if (VFPU_CC[0]) c020 = -c020
		"vcmp.s			LT,   s032, s002\n"		// VFPU_CC[0] = (s032 < EPSILON)
		"vrcp.s			s032, s032\n"			// s032 = 1.0f / s032 = 1 / sin(angle)
		"vscl.p			c030, c030, s032\n"		// s030 = s030 * s032 = sin((1 - t) * angle) / sin(angle)
												// s031 = s031 * s032 = sin(t       * angle) / sin(angle)
		"bvtl			0,    2f\n"				// if (CC[0]!=0) goto 0f
		"vmov.q			c000, c010\n"			// c000 = c010
	"1:\n"
		"vscl.q			c000, c010, s030\n"		// c000 = c010 * s030
		"vscl.q			c030, c020, s031\n"		// c030 = c020 * s031
		"vadd.q			c000, c000, c030\n"		// c000 = c000 + c030
	"2:\n"
		"sv.q			c000, %0\n"				// *pq  = c000
		".set			pop\n"					// restore assembler option
		: "=m"(*pOut)
		: "m"(*pIn1), "m"(*pIn2), "f"(t)
		: "$8"
	);
#else
	__asm__ (
		".set			push\n"					// save assember option
		".set			noreorder\n"			// suppress reordering
		"mfc1			$8,   %3\n"				// t0   = t
		"mtv			$8,   s031\n"			// s031 = t0 = t
		"lv.q			c010, %1\n"				// c010 = *pq1
		"lv.q			c020, %2\n"				// c020 = *pq2
		"vcst.s			s001, VFPU_SQRT1_2\n"	// s001 = VFPU_SQRT1_2 = 1 / sqrt(2)
		"vdot.q			s000[-1:1], c010, c020\n"	// s000 = c010 dot c020 = *pq0 dot *pq1
		"vocp.s			s030, s031\n"			// s030 = 1 - s031    = (1 - t)
		"vcmp.s			LT, s000[|x|], s001\n"	// CC[0] = |s000| < s001

		"vasin.s		s032, s000[|x|]\n"		// s032 = asin(abs(s000))
		"bvtl			0, 0f\n"				// if (CC[0]!=0) goto 0f
		"vocp.s			s032, s032\n"			// s032 = 1 - s032    = acos(abs(s000)) = angle

		"vmul.s			s001, s000, s000\n"		// s001 = s000 * s000
		"vocp.s			s001, s001\n"			// s001 = 1 - s001
		"vsqrt.s		s001, s001\n"			// s001 = sqrt(s001)
		"vasin.s		s032, s001\n"			// s032 = asin(s001)  = asin(sqrt(1-s000*s000))
												//                    = acos(abs(s000)) = angle
	"0:\n"
		"vzero.s		s001\n"					// s001 = 0
		"lvi.s			s002, 0.00005\n"		// s002 = EPSILON
		"vscl.p			c030, c030, s032\n"		// s030 = s030 * s032 = (1 - t) * angle
												// s031 = s031 * s032 = t       * angle
		"vcmp.s			LT,   s000, s001\n"		// VFPU_CC[0] = (s000 < 0)
		"vsin.t			c030, c030\n"			// s030 = sin(s030)   = sin((1 - t) * angle)
												// s031 = sin(s031)   = sin(t)      * angle)
												// s032 = sin(s032)   = sin(angle)
		"vcmovt.q		c020, c020[-x,-y,-z,-w], 0\n"	// if (VFPU_CC[0]) c020 = -c020
		"vcmp.s			LT,   s032, s002\n"		// VFPU_CC[0] = (s032 < EPSILON)
		"vrcp.s			s032, s032\n"			// s032 = 1.0f / s032 = 1 / sin(angle)
		"vscl.p			c030, c030, s032\n"		// s030 = s030 * s032 = sin((1 - t) * angle) / sin(angle)
												// s031 = s031 * s032 = sin(t       * angle) / sin(angle)
		"vscl.q			c000, c010, s030\n"		// c000 = c010 * s030
		"vscl.q			c030, c020, s031\n"		// c030 = c020 * s031
		"vadd.q			c000, c000, c030\n"		// c000 = c000 + c030
		"vcmovt.q		c000, c010, 0\n"		// if (VFPU_CC[0]) c000 = c010
		"sv.q			c000, %0\n"				// *pq  = c000
		".set			pop\n"					// restore assember option
		: "=m"(*pOut)
		: "m"(*pIn1), "m"(*pIn2), "f"(t)
		: "$8"
	);
#endif

/*
	// Calculate the cosine of the angle between the two
	REAL32 scale0;
	REAL32 scale1, reverse;
	REAL32 cosinus =	(pIn1->vec[0] * pIn2->vec[0]) + 
						(pIn1->vec[1] * pIn2->vec[1]) +
						(pIn1->vec[2] * pIn2->vec[2]) + 
						(pIn1->vec[3] * pIn2->vec[3]);

	if( cosinus < 0.0f)
	{
		cosinus = -cosinus;
		reverse = -1.0f;
	}
	else
	{
		reverse = 1.0f;
	}

	// If the angle is significant, use the spherical interpolation
	if( (1.0f - i3Math::abs(cosinus)) > 0.00005f )
	{
		REAL32 temp  = i3Math::acos( cosinus );
		REAL32 sinus = i3Math::sin( temp );
		scale0 = i3Math::sin( ( 1.0f - t) * temp) / sinus;
		scale1 = i3Math::sin(     t  * temp) / sinus;
	}
	else
	{  //  Else use the cheaper linear interpolation
		scale0 = 1.0f - t;
		scale1 = t;
	}

	scale1 *= reverse;

	// Return the interpolated result
	pOut->vec[0] = (pIn1->vec[0] * scale0) + (pIn2->vec[0] * scale1);
	pOut->vec[1] = (pIn1->vec[1] * scale0) + (pIn2->vec[1] * scale1);
	pOut->vec[2] = (pIn1->vec[2] * scale0) + (pIn2->vec[2] * scale1);
	pOut->vec[3] = (pIn1->vec[3] * scale0) + (pIn2->vec[3] * scale1);
	*/
}
#else
	void i3QuaternionSLERP( QUATERNION * pOut, QUATERNION * pIn1, QUATERNION * pIn2, REAL32 t)
	{
		// Calculate the cosine of the angle between the two
		REAL32 scale0;
		REAL32 scale1, reverse;
		REAL32 cosinus =	(pIn1->vec[0] * pIn2->vec[0]) + 
							(pIn1->vec[1] * pIn2->vec[1]) +
							(pIn1->vec[2] * pIn2->vec[2]) + 
							(pIn1->vec[3] * pIn2->vec[3]);

		if( cosinus < 0.0f)
		{
			cosinus = -cosinus;
			reverse = -1.0f;
		}
		else
		{
			reverse = 1.0f;
		}

		// If the angle is significant, use the spherical interpolation
		if( (1.0f - i3Math::abs(cosinus)) > 0.00005f )
		{
			REAL32 temp  = acosf( cosinus );
			REAL32 sinus = 1.0f / sinf( temp );
			scale0 = sinf( ( 1.0f - t) * temp) * sinus;
			scale1 = sinf(     t  * temp) * sinus;
		}
		else
		{  //  Else use the cheaper linear interpolation
			scale0 = 1.0f - t;
			scale1 = t;
		}

		scale1 *= reverse;

		// Return the interpolated result
		pOut->vec[0] = (pIn1->vec[0] * scale0) + (pIn2->vec[0] * scale1);
		pOut->vec[1] = (pIn1->vec[1] * scale0) + (pIn2->vec[1] * scale1);
		pOut->vec[2] = (pIn1->vec[2] * scale0) + (pIn2->vec[2] * scale1);
		pOut->vec[3] = (pIn1->vec[3] * scale0) + (pIn2->vec[3] * scale1);
	}
#endif
