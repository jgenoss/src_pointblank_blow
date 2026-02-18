#include "i3MathType.h"
#include "i3Vector.h"
#include "i3VectorN.h"

void i3VectorN::Fill( VECND * pV, REAL32 rVal)
{
	I3ASSERT( pV);

	INT32 i;
	for( i = pV->getSize() - 1; i >= 0; i--)
		pV->setValue( i, rVal);
}

void i3VectorN::FillZero( VECND * pV)
{
	Fill( pV, 0.f);
}

void i3VectorN::Add( VECND * pDest, VECND * pSrc)
{
	INT32 i;

	I3ASSERT( pDest);
	I3ASSERT( pSrc);
	I3ASSERT( pDest->getSize() == pSrc->getSize());

	REAL32 * pSrcValue = pSrc->getValues();
	REAL32 * pDestValue = pDest->getValues();

	for( i = pDest->getSize(); i > 0; i--)
	{
		*pDestValue += *pSrcValue;
		pDestValue ++;
		pSrcValue++;
	}
}

void i3VectorN::AddScaled( VECND * pDest, VECND * pSrc, REAL32 rScaleFactor )
{
	INT32 i;

	I3ASSERT( pDest);
	I3ASSERT( pSrc);
	I3ASSERT( pDest->getSize() == pSrc->getSize());

	REAL32 * pSrcValue = pSrc->getValues();
	REAL32 * pDestValue = pDest->getValues();

	for( i = pDest->getSize(); i > 0; i--)
	{
		*pDestValue += *pSrcValue * rScaleFactor;
		pDestValue++;
		pSrcValue++;
	}
}

void i3VectorN::Scale( VECND * pDest, REAL32 rScaleFactor)
{
	INT32 i;

	I3ASSERT( pDest);
	REAL32 * pValue = pDest->getValues();

	for( i = pDest->getSize(); i > 0; i--)
	{
		*pValue *= rScaleFactor;
		pValue++;
	}
}

void i3VectorN::Sub( VECND * pOut, VECND * pV1, VECND * pV2)
{
	INT32 i;

	I3ASSERT( pOut);
	I3ASSERT( pV1);
	I3ASSERT( pV2);

	I3ASSERT( pOut->getSize() == pV1->getSize());
	I3ASSERT( pOut->getSize() == pV2->getSize());

	REAL32 * pOutValue = pOut->getValues();
	REAL32 * pValue1 = pV1->getValues();
	REAL32 * pValue2 = pV2->getValues();

	for( i = pOut->getSize(); i > 0; i--)
	{
		*pOutValue = *pValue1 - *pValue2;
		pOutValue++;
		pValue1++;
		pValue2++;
	}
}

void i3VectorN::Sub( VECND * pOut, VEC3D * pV1, VEC3D * pV2)
{
	I3ASSERT( pOut);
	I3ASSERT( pV1);
	I3ASSERT( pV2);
	
	REAL32 * pOutValue = pOut->getValues();
	
	{
		*pOutValue = i3Vector::GetX( pV1) - i3Vector::GetX( pV2);
		pOutValue++;

		*pOutValue = i3Vector::GetY( pV1) - i3Vector::GetY( pV2);
		pOutValue++;

		*pOutValue = i3Vector::GetZ( pV1) - i3Vector::GetZ( pV2);
		pOutValue++;
	}
}

REAL32 i3VectorN::Dot( VECND * pV1, VECND * pV2 ) 
{
	I3ASSERT( pV1!= nullptr && pV2!= nullptr);
	
	I3ASSERT( pV1->getSize() == pV2->getSize());

	INT32 i;
	REAL32 rRv = 0.f;

	REAL32 * pValue1 = pV1->getValues();
	REAL32 * pValue2 = pV2->getValues();

	for( i = pV1->getSize(); i > 0; i--)
	{
		rRv += (*pValue1) * (*pValue2);
		pValue1++;
		pValue2++;
	}

	return rRv;
}

REAL32 i3VectorN::LengthSq( VECND * pV)
{
	I3ASSERT( pV);

	INT32 i;
	REAL32 rRv = 0.f;

	REAL32 * pValue = pV->getValues();

	for( i = pV->getSize(); i > 0; i-- )
	{
		rRv += (*pValue) * (*pValue);
		pValue++;
	}

	return rRv;
}

REAL32 i3VectorN::Length( VECND * pV)
{
	return i3Math::Sqrt( LengthSq( pV));
}

REAL32 i3VectorN::MaxAbs( VECND * pV)
{
	I3ASSERT( pV);

	INT32 i;
	REAL32 rRv = 0.f;

	REAL32 * pValue = pV->getValues();
	
	for( i = pV->getSize(); i > 0; i-- )
	{
		if ( *pValue > rRv )
		{
			rRv = *pValue;
		}
		else if ( -(*pValue) > rRv )
		{
			rRv = -(*pValue);
		}

		pValue++;
	}

	return rRv;
}

void i3VectorN::LoadScaled( VECND * pOut, REAL32 * pIn, REAL32 scale)
{
	INT32 i;

	I3ASSERT( pOut != nullptr);
	REAL32 *to = pOut->getValues();

	for( i = pOut->getSize(); i > 0; i-- )
	{
		*(to++) = (*(pIn++)) * scale;
	}
}



