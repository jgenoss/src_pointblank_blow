#include "i3MathType.h"
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

void i3MatrixStack::PushPost( MATRIX * pOut, MATRIX * pMatrix)
{
	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	if( m_Index > 0)
	{
		i3Matrix::Mul( pOut, m_Stack[ m_Index], pMatrix);
	}
	else
	{
		i3Matrix::Copy( pOut, pMatrix);
	}

	m_Index++;

	m_Stack[ m_Index] = pOut;

	m_bModified = TRUE;
}

void i3MatrixStack::PushPre( MATRIX * pOut, MATRIX * pMatrix)
{
	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	if( m_Index > 0)
	{
		i3Matrix::Mul( pOut, pMatrix, m_Stack[ m_Index]);
	}
	else
	{
		i3Matrix::Copy( pOut, pMatrix);
	}

	m_Index++;

	m_Stack[ m_Index] = pOut;

	m_bModified = TRUE;
}

void i3MatrixStack::Push( MATRIX * pOut, MATRIX  *pMatrix)
{
	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	if( pMatrix != NULL)
	{
		i3Matrix::Copy( pOut, pMatrix);
	}

	m_Index++;
	m_Stack[ m_Index] = pOut;

	m_bModified = TRUE;
}

void i3MatrixStack::PushIdentity(void)
{
	I3ASSERT( m_Index < MAX_MATRIXSTACK_DEPTH);

	m_Index++;
	m_Stack[ m_Index] = &I3_IDENTITYMATRIX;

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

void i3MatrixStack::SetTop( MATRIX * pMatrix)
{
	i3Matrix::Copy( m_Stack[ m_Index], pMatrix);

	m_bModified = TRUE;
}

void i3MatrixStack::Reset(void)
{
	m_Stack[0] = &I3_IDENTITYMATRIX;
	m_Index = 0;
	m_SavedIndex = -1;

	m_bModified = TRUE;
}
