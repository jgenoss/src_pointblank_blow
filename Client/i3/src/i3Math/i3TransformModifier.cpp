#include "i3MathType.h"
#include "i3TransformModifier.h"
#include "i3Matrix.h"

I3_CLASS_INSTANCE( i3TransformModifier);

void i3TransformModifier::Apply( MATRIX * pMatrix)
{
	switch( m_ApplyType)
	{
		case APPLY_COPY :			i3Matrix::Copy( pMatrix, &m_Matrix);	break;
		case APPLY_PREMULTIPLY :	i3Matrix::Mul( pMatrix, &m_Matrix, pMatrix);	break;
		case APPLY_POSTMULTIPLY :	i3Matrix::Mul( pMatrix, pMatrix, &m_Matrix);	break;
	}
}

void i3TransformModifier::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	i3TransformModifier * pMod = (i3TransformModifier *) pDest;

	pMod->setMatrix( getMatrix());
	pMod->setApplyType( getApplyType());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 TRANSFORM_MODIFIER
	{
		UINT8			m_ID[4] = { 'T', 'M', 'F', '1' };
		i3::pack::MATRIX	m_Matrix;
		UINT32			m_ApplyType = 0;

		UINT32			pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TransformModifier::OnSave( i3ResourceFile * pResFile)
{
	pack::TRANSFORM_MODIFIER data;
	UINT32 Rc, Result;

	Result = 0;
	
	i3Matrix::Copy( &data.m_Matrix, getMatrix());
	data.m_ApplyType = (UINT32)	getApplyType();

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3TransformModifier::OnLoad( i3ResourceFile * pResFile)
{
	pack::TRANSFORM_MODIFIER data;
	UINT32 Rc, Result;

	Result = 0;

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Matrix::Copy( &m_Matrix, &data.m_Matrix);
	setApplyType( (APPLY_TYPE) data.m_ApplyType);

	return Result;
}
