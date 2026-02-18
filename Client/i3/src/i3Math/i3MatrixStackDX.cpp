#include "i3MathType.h"
#include "i3MatrixStack.h"

I3_CLASS_INSTANCE( i3MatrixStack);

i3MatrixStack::i3MatrixStack(void)
{
	m_Stack[0] = &I3_IDENTITYMATRIX;
}

void i3MatrixStack::PushPost( MATRIX * pOut, MATRIX * pMatrix)
{
	I3ASSERT( m_Index < (MAX_MATRIXSTACK_DEPTH - 1));

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

	m_bModified = true;
}

void i3MatrixStack::PushPre( MATRIX * pOut, MATRIX * pMatrix)
{
	I3ASSERT( m_Index < (MAX_MATRIXSTACK_DEPTH - 1));

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

	m_bModified = true;
}

void i3MatrixStack::Push( MATRIX * pOut, MATRIX  *pMatrix)
{
	I3ASSERT( m_Index < (MAX_MATRIXSTACK_DEPTH - 1));

	if( pMatrix != nullptr)
	{
		i3Matrix::Copy( pOut, pMatrix);
	}

	m_Index++;
	m_Stack[ m_Index] = pOut;

	m_bModified = true;
}

void i3MatrixStack::PushIdentity(void)
{
	I3ASSERT( m_Index < (MAX_MATRIXSTACK_DEPTH - 1));

	m_Index++;
	m_Stack[ m_Index] = &I3_IDENTITYMATRIX;

	m_bModified = true;
}

void i3MatrixStack::Pop( void)
{
	if( m_Index > 0)
	{
		m_Index--;

		m_bModified = true;
	}
}

void i3MatrixStack::Pop( INT32 iCount)
{
	if( (iCount > 0) && (m_Index >= iCount) )
	{
		m_Index -= iCount;

		m_bModified = true;
	}
}

void i3MatrixStack::SetTop( MATRIX * pMatrix)
{
	i3Matrix::Copy( m_Stack[ m_Index], pMatrix);

	m_bModified = true;
}

void i3MatrixStack::Reset(void)
{
	m_Stack[0] = &I3_IDENTITYMATRIX;
	m_Index = 0;
	m_SavedIndex = -1;

	m_bModified = true;
}
