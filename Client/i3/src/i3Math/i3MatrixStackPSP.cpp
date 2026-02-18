#include "i3MatrixStack.h"

I3_CLASS_INSTANCE( i3MatrixStack, i3ElementBase);

i3MatrixStack::i3MatrixStack(void)
{
	Reset();
}

i3MatrixStack::~i3MatrixStack(void)
{
}

void i3MatrixStack::PushPost( MATRIX * pMatrix)
{
	MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);

	if( m_Index > 0)
	{
		#if 0
			i3Matrix::Mul( pDest, & m_Stack[ m_Index], pMatrix);

			__asm__ (
				"lv.q		c500,  0 + %0\n"
				"lv.q		c510, 16 + %0\n"
				"lv.q		c520, 32 + %0\n"
				"lv.q		c530, 48 + %0\n"		// Load World Matrix
				:
				: "m"( *pDest)
			);
		#else
			__asm__ (
				".set		push\n"
				".set		noreorder\n"

				"lv.q		c000,  0 + %1\n"
				"lv.q		c010, 16 + %1\n"
				"lv.q		c020, 32 + %1\n"
				"lv.q		c030, 48 + %1\n"		// Load World Matrix

				"vrmmul.q	e100, e500, e000\n"		// 이전 World Matrix에 새 World Matrix를 Pre-Multiply 한다.
				"vmmov.q	e500, e100\n"

				"sv.q		c100,  0 + %0\n"
				"sv.q		c110, 16 + %0\n"
				"sv.q		c120, 32 + %0\n"
				"sv.q		c130, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

				".set		pop\n"
				: "=m" ( *pDest)
				: "m"( *pMatrix)
			);
		#endif
	}
	else
	{
		#if 0
			i3Matrix::Copy( pDest, pMatrix);
		#else
			__asm__ (
				".set		push\n"
				".set		noreorder\n"

				"lv.q		c500,  0 + %1\n"
				"lv.q		c510, 16 + %1\n"
				"lv.q		c520, 32 + %1\n"
				"lv.q		c530, 48 + %1\n"		// Load World Matrix

				"sv.q		c500,  0 + %0\n"
				"sv.q		c510, 16 + %0\n"
				"sv.q		c520, 32 + %0\n"
				"sv.q		c530, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

				".set		pop\n"
				: "=m" ( *pDest)
				: "m"( *pMatrix)
			);
		#endif
	}

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::PushPre( MATRIX * pMatrix)
{
	MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);

	if( m_Index > 0)
	{
		#if 0
			i3Matrix::Mul( pDest, pMatrix, & m_Stack[ m_Index]);

			__asm__ (
				"lv.q		c500,  0 + %0\n"
				"lv.q		c510, 16 + %0\n"
				"lv.q		c520, 32 + %0\n"
				"lv.q		c530, 48 + %0\n"		// Load World Matrix
				:
				: "m"( *pDest)
			);
		#else
			__asm__ (
				".set		push\n"
				".set		noreorder\n"

				"lv.q		c000,  0 + %1\n"
				"lv.q		c010, 16 + %1\n"
				"lv.q		c020, 32 + %1\n"
				"lv.q		c030, 48 + %1\n"		// Load World Matrix

				"vrmmul.q	e100, e000, e500\n"		// 이전 World Matrix에 새 World Matrix를 Pre-Multiply 한다.
				"vmmov.q	e500, e100\n"

				"sv.q		c100,  0 + %0\n"
				"sv.q		c110, 16 + %0\n"
				"sv.q		c120, 32 + %0\n"
				"sv.q		c130, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

				".set		pop\n"
				: "=m" ( *pDest)
				: "m"( *pMatrix)
			);
		#endif
	}
	else
	{
		#if 0
			i3Matrix::Copy( pDest, pMatrix);
		#else
			__asm__ (
				".set		push\n"
				".set		noreorder\n"

				"lv.q		c500,  0 + %1\n"
				"lv.q		c510, 16 + %1\n"
				"lv.q		c520, 32 + %1\n"
				"lv.q		c530, 48 + %1\n"		// Load World Matrix

				"sv.q		c500,  0 + %0\n"
				"sv.q		c510, 16 + %0\n"
				"sv.q		c520, 32 + %0\n"
				"sv.q		c530, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

				".set		pop\n"
				: "=m" ( *pDest)
				: "m"( *pMatrix)
			);
		#endif
	}

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::Push( MATRIX  *pMatrix)
{
	MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);

	#if 0
		i3Matrix::Copy( pDest, pMatrix);
	#else
		__asm__ (
				".set		push\n"
				".set		noreorder\n"

				"lv.q		c500,  0 + %1\n"
				"lv.q		c510, 16 + %1\n"
				"lv.q		c520, 32 + %1\n"
				"lv.q		c530, 48 + %1\n"		// Load World Matrix

				"sv.q		c500,  0 + %0\n"
				"sv.q		c510, 16 + %0\n"
				"sv.q		c520, 32 + %0\n"
				"sv.q		c530, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

				".set		pop\n"
				: "=m" ( *pDest)
				: "m"( *pMatrix)
			);
	#endif

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::Pop( void)
{
	if( m_Index > 0)
	{
		m_Index--;

		#if 1
		__asm__ (
			".set		push\n"
			".set		noreorder\n"

			"lv.q		c500,  0 + %0\n"
			"lv.q		c510, 16 + %0\n"
			"lv.q		c520, 32 + %0\n"
			"lv.q		c530, 48 + %0\n"		// Load World Matrix

			".set		pop\n"
			: 
			: "m"( m_Stack[ m_Index])
		);
		#endif

		m_bModified = TRUE;
	}
}


void i3MatrixStack::SetTop( MATRIX * pMatrix)
{
#if 0
	i3Matrix::Copy( & m_Stack[ m_Index], pMatrix);
#else
	__asm__ (
			".set		push\n"
			".set		noreorder\n"

			"lv.q		c500,  0 + %1\n"
			"lv.q		c510, 16 + %1\n"
			"lv.q		c520, 32 + %1\n"
			"lv.q		c530, 48 + %1\n"		// Load World Matrix

			"sv.q		c500,  0 + %0\n"
			"sv.q		c510, 16 + %0\n"
			"sv.q		c520, 32 + %0\n"
			"sv.q		c530, 48 + %0\n"		// 새 World Matrix를 pDest에 저장.

			".set		pop\n"
			: "=m" ( m_Stack[ m_Index])
			: "m"( *pMatrix)
		);
#endif

	m_bModified = TRUE;
}

void i3MatrixStack::Reset(void)
{
#if 0
	i3Matrix::Identity( m_Stack);
#else
	__asm__ (
		".set			push\n"					//
		".set			noreorder\n"			//

		"vmidt.q		e500\n"					// View Matrix -> Identity

		"sv.q			c500,  0 + %0\n"
		"sv.q			c510, 16 + %0\n"
		"sv.q			c520, 32 + %0\n"
		"sv.q			c530, 48 + %0\n"		// Init m_ViewMatrix

		".set			pop\n"					// restore assember option
		: "=m"( * m_Stack)
		);
#endif

	m_Index = 0;

	m_bModified = TRUE;
}
