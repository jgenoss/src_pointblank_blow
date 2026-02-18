#include "i3MatrixStack.h"
#include "i3FixedPointMatrix.h"

I3_CLASS_INSTANCE( i3MatrixStack, i3ElementBase);

i3MatrixStack::i3MatrixStack(void)
{
	Reset();
}

i3MatrixStack::~i3MatrixStack(void)
{
}

void i3MatrixStack::PushPost( RT_MATRIX * pMatrix)
{
	RT_MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);

	if( m_Index > 0)
	{
		i3Matrix::Mul( pDest, & m_Stack[ m_Index], pMatrix);
	}
	else
	{
		i3Matrix::Copy( pDest, pMatrix);
	}

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::PushPre( RT_MATRIX * pMatrix)
{
	RT_MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);

	if( m_Index > 0)
	{
		i3Matrix::MulAffine( pDest, pMatrix, & m_Stack[ m_Index]);
	}
	else
	{
		i3Matrix::Copy( pDest, pMatrix);
	}

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::Push( RT_MATRIX  *pMatrix)
{
	RT_MATRIX * pDest;

	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	pDest = &(m_Stack[m_Index + 1]);


	i3Matrix::Copy( pDest, pMatrix);

	m_Index++;

	m_bModified = TRUE;
}

void i3MatrixStack::Pop( void)
{
	if( m_Index > 0)
	{
		m_Index--;

		m_bModified = TRUE;
	}
}

void i3MatrixStack::SetTop( RT_MATRIX * pMatrix)
{
	i3Matrix::Copy( & m_Stack[ m_Index], pMatrix);

	m_bModified = TRUE;
}

void i3MatrixStack::Reset(void)
{
	i3Matrix::Identity( m_Stack);

	m_Index = 0;

	m_bModified = TRUE;
}
