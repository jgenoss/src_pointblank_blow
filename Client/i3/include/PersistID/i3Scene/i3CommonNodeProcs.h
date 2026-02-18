#if !defined( __I3_COMMON_NODE_PROCS_H)
#define __I3_COMMON_NODE_PROCS_H

#include "i3Node.h"
#include "i3SceneTracer.h"

////////////////////////////////////////////////////////////////////////////////
//
// _copyTop
//
BEGIN_INLINE
void _copyTop( RT_MATRIX * pDest, i3MatrixStack * pStack)
{
	#if defined( I3_PSP)
		__asm__ (
			".set			push\n"					//
			".set			noreorder\n"			//

			"sv.q			c500,  0 + %0\n"
			"sv.q			c510, 16 + %0\n"
			"sv.q			c520, 32 + %0\n"
			"sv.q			c530, 48 + %0\n"		// Copy Top Matrix -> pDest

			".set			pop\n"					// restore assember option
			: "=m"( *pDest)
		);
	#else
		i3Matrix::Copy( pDest, pStack->GetTop());
	#endif
}
END_INLINE

////////////////////////////////////////////////////////////////////////////////
//
// _copyTopWorld
//
BEGIN_INLINE
void _copyTopWorld( i3SceneTracer * pTracer, RT_MATRIX * pDest, i3MatrixStack * pStack)
{
	if( pTracer->IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
	{
		#if defined( I3_PSP)
			MATRIX * pInvView = pTracer->GetInverseViewMatrix();

			__asm__ (
				".set			push\n"					//
				".set			noreorder\n"			//

				"lv.q			c100,  0 + %1\n"
				"lv.q			c110, 16 + %1\n"
				"lv.q			c120, 32 + %1\n"
				"lv.q			c130, 48 + %1\n"

				"vrmmul.q		e000, e500, e100\n"

				"sv.q			c000,  0 + %0\n"
				"sv.q			c010, 16 + %0\n"
				"sv.q			c020, 32 + %0\n"
				"sv.q			c030, 48 + %0\n"		// Copy Top Matrix -> pDest

				".set			pop\n"					// restore assember option
				: "=m"( *pDest)
				: "m"( *pInvView)
			);
		#else
			i3Matrix::Mul( pDest, pStack->GetTop(), pTracer->GetInverseViewMatrix());
		#endif
	}
	else
	{
		#if defined( I3_PSP)
			__asm__ (
				".set			push\n"					//
				".set			noreorder\n"			//

				"sv.q			c500,  0 + %0\n"
				"sv.q			c510, 16 + %0\n"
				"sv.q			c520, 32 + %0\n"
				"sv.q			c530, 48 + %0\n"		// Copy Top Matrix -> pDest

				".set			pop\n"					// restore assember option
				: "=m"( *pDest)
			);
		#else
			i3Matrix::Copy( pDest, pStack->GetTop());
		#endif
	}
}
END_INLINE

////////////////////////////////////////////////////////////////////////////////
//
// _setInverseModelView3x3
//
BEGIN_INLINE
void _setInverseModelView3x3(  RT_MATRIX * pDest, i3MatrixStack * pStack, i3SceneTracer * pTracer)
{
	if( pTracer->IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
	{
		#if defined( I3_PSP)
			__asm__ (
				".set			push\n"					//
				".set			noreorder\n"			//

				"vmmov.t		e100, m500\n"			// 
				"vmov.q			r103, r503\n"
				"vmov.t			c130, c530\n"			// Transpose 3x3

				"sv.q			c100,  0 + %0\n"
				"sv.q			c110, 16 + %0\n"
				"sv.q			c120, 32 + %0\n"
				"sv.q			c130, 48 + %0\n"		// Copy Top Matrix -> pDest

				".set			pop\n"					// restore assember option
				: "=m"( *pDest)
			);
		#else
			i3Matrix::Transpose3x3( pDest, pStack->GetTop());		// 역행렬을 구하는데 Rotation 부분만 필요하기 때문에 Transpose로 구현.
		#endif
	}
	else
	{
		#if defined( I3_PSP)
			__asm__ (
				".set			push\n"					//
				".set			noreorder\n"			//

				"vrmmul.q		e000, e500, e600\n"		// pDest = World * View

				"vmmov.t		e100, m000\n"			// 
				"vmov.q			r103, r003\n"
				"vmov.t			c130, c030\n"			// Transpose 3x3

				"sv.q			c100,  0 + %0\n"
				"sv.q			c110, 16 + %0\n"
				"sv.q			c120, 32 + %0\n"
				"sv.q			c130, 48 + %0\n"		// Copy Top Matrix -> pDest

				".set			pop\n"					// restore assember option
				: "=m"( *pDest)
			);
		#else
   			i3Matrix::Mul( pDest, pStack->GetTop(), pTracer->GetViewMatrix());
			i3Matrix::Transpose3x3( pDest, pDest);		// 역행렬을 구하는데 Rotation 부분만 필요하기 때문에 Transpose로 구현.
		#endif
	}
}
END_INLINE

#endif
