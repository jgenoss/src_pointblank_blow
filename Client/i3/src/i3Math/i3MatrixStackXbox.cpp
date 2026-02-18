#include "i3MathType.h"
#include "../i3Base/i3Debug.h"
#include "i3MatrixStack.h"

#ifdef I3G_XBOX

I3_CLASS_INSTANCE( i3MatrixStack, i3ElementBase);

i3MatrixStack::i3MatrixStack(void)
{
	D3DXCreateMatrixStack( 0, &m_pStack);
}

i3MatrixStack::~i3MatrixStack(void)
{
	if( m_pStack != NULL)
	{
		m_pStack->Release();
		m_pStack = NULL;
	}
}

void i3MatrixStack::PushPost( MATRIX * pMatrix)
{
	I3ASSERT( m_pStack != NULL);

	// m_pStack->Push();

	// m_pStack->MultMatrix( pMatrix);
}

void i3MatrixStack::PushPre( MATRIX * pMatrix)
{
	I3ASSERT( m_pStack != NULL);

	// m_pStack->Push();

	// m_pStack->MultMatrixLocal( pMatrix);
}

void i3MatrixStack::Pop(void)
{
	I3ASSERT( m_pStack != NULL);

	// m_pStack->Pop();
}

MATRIX * i3MatrixStack::GetTop(void)
{
	I3ASSERT( m_pStack != NULL);

	// return m_pStack->GetTop();
	return NULL;
}

void i3MatrixStack::SetTop( MATRIX * pMatrix)
{
	I3ASSERT( m_pStack != NULL);

	// m_pStack->LoadMatrix( pMatrix);
}

#endif I3G_XBOX