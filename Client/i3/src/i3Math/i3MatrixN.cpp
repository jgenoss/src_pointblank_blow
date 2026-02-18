#include "i3MathType.h"
#include "i3VectorN.h"
#include "i3MatrixN.h"

void i3MatrixN::Identity( MATRIX_N * pOut)
{
	//I3ASSERT( pOut->m_iRows == pOut->m_iCols);
	FillZero( pOut);
	SetDiagonalEntries( pOut, 1.f);
}

void i3MatrixN::FillZero( MATRIX_N * pOut)
{
	INT32 i, j;

	REAL32 * pValue = pOut->getValues();

	for( i = 0; i < pOut->getRowCount(); i++)
	{
		for( j = 0; j < pOut->getColCount(); j++)
		{
			pValue[ (i * pOut->getColCount()) + j] = 0.f;
		}
	}
}

void i3MatrixN::Add( MATRIX_N * pOut, MATRIX_N * pIn)
{
	INT32 i;

	REAL32 * aPtr = pOut->getValues();
	REAL32 * bPtr = pIn->getValues();

	I3ASSERT( aPtr != nullptr);
	I3ASSERT( bPtr != nullptr);

	I3ASSERT( pOut->getRowCount() == pIn->getRowCount() && pOut->getColCount() == pIn->getColCount());

	for ( i = pOut->getRowCount() * pOut->getColCount(); i > 0; i-- )
	{
		(*(aPtr++)) += *(bPtr++);
	}
}

void i3MatrixN::Sub( MATRIX_N * pOut, MATRIX_N * pIn)
{
	INT32 i;

	REAL32 * aPtr = pOut->getValues();
	REAL32 * bPtr = pIn->getValues();

	I3ASSERT( pOut->getRowCount() == pIn->getRowCount() && pOut->getColCount() == pIn->getColCount());

	for ( i = pOut->getRowCount() * pOut->getColCount(); i > 0; i-- )
	{
		(*(aPtr++)) -= *(bPtr++);
	}
}

void i3MatrixN::Copy( MATRIX_N * pDest, MATRIX_N * pSrc)
{
	I3ASSERT( pSrc->getRowCount() <= pDest->getRowCount() && pSrc->getColCount() <= pDest->getColCount() );

	INT32 i, j;
	INT32 extraColStep = pDest->getRowCount() - pSrc->getRowCount();
	REAL32 *to = pDest->getValues();
	REAL32 *from = pSrc->getValues();

	for( i = pSrc->getColCount(); i > 0; i-- ) {			// Copy columns of A, one per time thru loop
		for( j = pSrc->getRowCount(); j > 0; j-- ) {		// Copy all elements of this column of A
			*(to++) = *(from++);
		}
		to += extraColStep;
	}	
}

void i3MatrixN::Transpose( MATRIX_N * pOut, MATRIX_N * pIn)
{
	I3ASSERT( pIn->getRowCount() <= pOut->getColCount() && pIn->getColCount() <= pOut->getRowCount() );

	INT32 i, j;
	REAL32 * rowPtr = pOut->getValues();
	REAL32 * from = pIn->getValues();
	for( i = pIn->getColCount(); i > 0; i-- ) {				// Copy columns of A, once per loop
		REAL32* to = rowPtr;
		for( j = pIn->getRowCount(); j > 0; j-- ) {			// Loop copying values from the column of A
			*to = *(from++);
			to += pOut->getRowCount();
		}
		rowPtr ++;
	}	
}

void i3MatrixN::SetDiagonalEntries( MATRIX_N * pOut, REAL32 d )
{
	INT32	diagLen = MIN( pOut->getRowCount(), pOut->getColCount() );
	REAL32 * pValue = pOut->getValues();

	for( ; diagLen > 0; diagLen-- )
	{
		*pValue = d;
		pValue += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetDiagonalEntries( MATRIX_N * pOut, VECND * pIn )
{
	INT32 diagLen = MIN( pOut->getRowCount(), pOut->getColCount());
	REAL32 * pDest = pOut->getValues();
	REAL32 * pSrc = pIn->getValues();

	I3ASSERT( pIn->getSize() == diagLen );

	for ( ; diagLen>0; diagLen-- )
	{
		*pDest = *(pSrc++);
		pDest += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetSuperDiagonalEntries( MATRIX_N * pOut, REAL32 d )
{
	INT32 sDiagLen = MIN( pOut->getRowCount(), (INT32)(pOut->getColCount()-1) );
	REAL32 * toValue = pOut->getValues() + pOut->getRowCount();

	for ( ; sDiagLen>0; sDiagLen-- ) {
		*toValue = d;
		toValue += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetSuperDiagonalEntries( MATRIX_N * pOut, VECND * pIn )
{
	INT32 sDiagLen = MIN( (INT32)(pOut->getRowCount()-1), pOut->getColCount());
	I3ASSERT( sDiagLen == pIn->getSize());
	REAL32 * toValue = pOut->getValues() + pOut->getRowCount();
	REAL32 * pSrc = pIn->getValues();

	for ( ; sDiagLen>0; sDiagLen-- )
	{
		*toValue = *(pSrc++);
		toValue += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetSubDiagonalEntries( MATRIX_N * pOut, REAL32 d )
{
	INT32 sDiagLen = MIN( pOut->getRowCount(), pOut->getColCount()) - 1;
	REAL32 * toValue = pOut->getValues() + 1;

	for ( ; sDiagLen>0; sDiagLen-- )
	{
		*toValue = d;
		toValue += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetSubDiagonalEntries( MATRIX_N * pOut, VECND * pIn )
{
	INT32 sDiagLen = MIN( pOut->getRowCount(), pOut->getColCount()) - 1;
	I3ASSERT( sDiagLen == pIn->getSize() );
	REAL32 * toValue = pOut->getValues() + 1;
	REAL32 * pSrc = pIn->getValues();

	for ( ; sDiagLen>0; sDiagLen-- )
	{
		*toValue = *(pSrc++);
		toValue += pOut->getRowCount()+1;
	}
}

void i3MatrixN::SetColumn( MATRIX_N * pOut, INT32 idx, VECND * pIn )
{
	INT32 i;

	I3ASSERT( pOut->getRowCount() == pIn->getSize());

	REAL32 * to = pOut->getValues() + (idx * pOut->getRowCount());
	const REAL32 * from = pIn->getValues();

	for( i = pOut->getRowCount(); i > 0; i-- )
	{
		*(to++) = *(from++);
	}
}

void i3MatrixN::SetRow( MATRIX_N * pOut, INT32 idx, VECND * pIn )
{
	INT32 i;

	I3ASSERT( pOut->getColCount() == pIn->getSize() );

	REAL32 * to = pOut->getValues() + idx;
	const REAL32 * from = pIn->getValues();

	for( i = pOut->getRowCount(); i > 0; i-- )
	{
		*to = *(from++);
		to += pOut->getRowCount();
	}
}

void i3MatrixN::SetSequence( MATRIX_N * pOut, VECND * d, INT32 startRow, INT32 startCol, INT32 deltaRow, INT32 deltaCol )
{
	INT32 length = d->getSize();

	I3ASSERT( startRow >= 0 && startRow < pOut->getRowCount() && startCol >= 0 && startCol < pOut->getColCount() );
	I3ASSERT( startRow + ((length-1) * deltaRow) >=0 && startRow + ((length-1) * deltaRow) < pOut->getRowCount() );
 	I3ASSERT( startCol + ((length-1) * deltaCol) >=0 && startCol + ((length-1) * deltaCol) < pOut->getColCount() );

	REAL32 *to = pOut->getValues() + startRow + (pOut->getRowCount() * startCol);
	REAL32 *from = d->getValues();
	INT32 stride = deltaRow + (pOut->getRowCount() * deltaCol);

	for ( ; length > 0; length-- )
	{
		*to = *(from++);
		to += stride;
	}
}

REAL32 i3MatrixN::FrobeniusNormSq( MATRIX_N * pOut)
{
	INT32 i;
	REAL32 * pM = pOut->getValues();
	REAL32 rRv = 0.f;
	for ( i = pOut->getRowCount() * pOut->getColCount(); i > 0; i--)
	{
		rRv += (*(pM)) * (*(pM));
		pM++;
	}
	return rRv;
}

REAL32 i3MatrixN::FrobeniusNorm( MATRIX_N * pOut)
{
	return i3Math::Sqrt( FrobeniusNormSq( pOut) );
}

// Operations on VECND's
void i3MatrixN::Multiply( VECND * pOut, MATRIX_N * pInMat, VECND * pInVec )
{
	INT32 i, j;

	I3ASSERT ( pInVec->getSize() == pInMat->getColCount() && pOut->getSize() == pInMat->getRowCount());
	
	REAL32 * toValue = pOut->getValues();			// Points to entry in result vector
	const REAL32* pRow = pInMat->getValues();					// Points to beginning of next row in matrix

	for( i = pInMat->getRowCount(); i > 0; i-- )
	{
		const REAL32 * pIn = pInVec->getValues();
		const REAL32 * m = pRow++;
		*toValue = 0.f;

		for ( j = pInMat->getColCount(); j > 0; j-- )
		{
			*toValue += (*(pIn++)) * (*m);
			m += pInMat->getRowCount();
		}

		toValue++;
	}
}

void i3MatrixN::MultiplyTranspose( VECND * pOut, MATRIX_N * pInMat, VECND * pInVec )
{
	INT32 i, j;

	I3ASSERT( pInVec->getSize() == pInMat->getRowCount() && pOut->getSize() == pInMat->getColCount() );

	REAL32 * toValue = pOut->getValues();				// Points to entry in result vector
	const REAL32 * colPtr = pInMat->getValues();					// Points to beginning of next column in matrix
	for( i = pInMat->getColCount(); i > 0; i-- )
	{
		const REAL32 * pIn = pInVec->getValues();

		*toValue = 0.f;
		for( j = pInMat->getRowCount(); j > 0; j-- )
		{
			*toValue += (*(pIn++)) * (*(colPtr++));
		}
		toValue++;
	}
}

REAL32 i3MatrixN::DotProductColumn( MATRIX_N * pOut, VECND * v, INT32 colNum )
{
	INT32 i;

	I3ASSERT( v->getSize() == pOut->getRowCount() );

	REAL32 * ptrC = pOut->getValues() + (colNum * pOut->getRowCount());
	REAL32 * ptrV = v->getValues();	
	REAL32 rRv = 0.f;

	for( i = pOut->getRowCount(); i > 0; i-- )
	{
		rRv += (*(ptrC++))*(*(ptrV++));
	}
	return rRv;
}


// Operations on MATRIX_N's
// Sets dst = A*B
void i3MatrixN::Multiply( MATRIX_N * pOut, MATRIX_N * pM1, MATRIX_N * pM2)
{
	INT32 i, j;

	I3ASSERT( pM1->getColCount() == pM2->getRowCount() && pM1->getRowCount() == pOut->getRowCount() && pM2->getColCount() == pOut->getColCount() );

	INT32 length = pM1->getColCount();
	REAL32 *bPtr = pM2->getValues();						// Points to beginning of column in B
	REAL32 *dPtr = pOut->getValues();

	for ( i = pOut->getColCount(); i > 0; i-- )
	{
		REAL32 *aPtr = pM1->getValues();					// Points to beginning of row in A
		for ( j = pOut->getRowCount(); j > 0; j-- )
		{
			*dPtr = DotArray( length, aPtr, pM1->getRowCount(), bPtr, 1 );
			dPtr++;
			aPtr++;
		}
		bPtr += pM2->getRowCount();
	}
}

// Sets dst = A*(B-tranpose).
void i3MatrixN::MultiplyTranspose( MATRIX_N * pOut, MATRIX_N * pA, MATRIX_N * pB )
{
	INT32 i, j;

	I3ASSERT( pA->getColCount() == pB->getColCount() && pA->getRowCount() == pOut->getRowCount() && pB->getRowCount() == pOut->getColCount() );

	INT32 length = pA->getColCount();
	REAL32 *bPtr = pB->getValues();						// Points to beginning of row in B
	REAL32 *dPtr = pOut->getValues();
	for ( i = pOut->getColCount(); i>0; i-- ) {
		REAL32 *aPtr = pA->getValues();					// Points to beginning of row in A
		for ( j = pOut->getRowCount(); j>0; j-- ) {
			*dPtr = DotArray( length, aPtr, pA->getRowCount(), bPtr, pB->getRowCount() );
			dPtr++;
			aPtr++;
		}
		bPtr++;
	}
}

// Sets dst = (A-transpose)*B
void i3MatrixN::TransposeMultiply( MATRIX_N * pOut, MATRIX_N * pA, MATRIX_N * pB )
{
	INT32 i, j;

	I3ASSERT( pA->getRowCount() == pB->getRowCount() && pA->getColCount() == pOut->getRowCount() && pB->getColCount() == pOut->getColCount() );

	INT32 length = pA->getRowCount();
	REAL32 *bPtr = pB->getValues();										// bPtr Points to beginning of column in B
	REAL32 *dPtr = pOut->getValues();

	for ( i = pOut->getColCount(); i > 0; i-- )
	{				// Loop over all columns of dst
		REAL32 *aPtr = pA->getValues();									// aPtr Points to beginning of column in A
		for ( j = pOut->getRowCount(); j>0; j-- )
		{			// Loop over all rows of dst
			*dPtr = DotArray( length, aPtr, 1, bPtr, 1 );
			dPtr ++;
			aPtr += pA->getRowCount();
		}
		bPtr += pB->getRowCount();
	}
}


// Miscellaneous operation
// Adds d to each diagonal
void i3MatrixN::AddToDiagonal( MATRIX_N * pOut, REAL32 d )
{
	INT32 diagLen = MIN( pOut->getRowCount(), pOut->getColCount() );
	REAL32* dPtr = pOut->getValues();

	for ( ; diagLen>0; diagLen-- ) {
		*dPtr += d;
		dPtr += pOut->getRowCount()+1;
	}
}

// Solving systems of linear equations
// Solves the equation   (*this)*x = B;    Uses row operations.  Assumes *this is invertible.   
void i3MatrixN::Solve( MATRIX_N * pOut, MATRIX_N * pAugmentedMat, VECND * pB, VECND * x )
{
	I3ASSERT( pOut->getRowCount() == pOut->getColCount() && pOut->getColCount() == x->getSize() && pOut->getRowCount() == pB->getSize() );

	INT32 i, j;

	// Copy this matrix and b into an Augmented Matrix
	I3ASSERT( pAugmentedMat != nullptr);
	pAugmentedMat->setSize( pOut->getRowCount(), pOut->getColCount() + 1);
	Copy( pAugmentedMat, pOut);
	SetColumn( pAugmentedMat, pOut->getRowCount(), pB );

	// Put into row echelon form with row operations
	ConvertToRefNoFree( pAugmentedMat);

	// Solve for x vector values using back substitution
	REAL32* xLast = x->getValues() + pOut->getRowCount() - 1;				// Last entry in xVec
	REAL32* endRow = pAugmentedMat->getValues() + (pOut->getRowCount() * pOut->getColCount()) - 1;	// Last entry in the current row of the coefficient part of Augmented Matrix
	REAL32* bPtr = endRow + pOut->getRowCount();				// Last entry in augmented matrix (end of last column, in augmented part)

	for( i = pOut->getRowCount(); i > 0; i-- )
	{
		REAL32 accum = *(bPtr--);
		// Next loop computes back substitution terms
		REAL32* rowPtr = endRow;					// Points to entries of the current row for back substitution.
		REAL32* xPtr = xLast;						// Points to entries in the x vector (also for back substitution)
		for( j = pOut->getRowCount() - i; j > 0; j-- ) {			
			accum -= (*rowPtr)*(*(xPtr--));
			rowPtr -= pOut->getColCount();						// Previous entry in the row
		}
		if( *rowPtr != 0.f)
		{
			I3ASSERT( *rowPtr != 0.f );					// Are not supposed to be any free variables in this matrix
			*xPtr = accum/(*rowPtr);
		}
		endRow--;
	}
}

// ConvertToRefNoFree
// Converts the matrix (in place) to row echelon form
// For us, row echelon form allows any non-zero values, not just 1's, in the 
//		position for a lead variable.
// The "NoFree" version operates on the assumption that no free variable will be found.
// Algorithm uses row operations and row pivoting (only).  
// Augmented matrix is correctly accomodated.  Only the first square part participates 
//		in the main work of row operations.
void i3MatrixN::ConvertToRefNoFree( MATRIX_N * pOut)
{
	// Loop over all columns (variables)
	// Find row with most non-zero entry.
	// Swap to the highest active row
	// Subtract appropriately from all the lower rows (row op of type 3)
	INT32 i, j;
	INT32 numIters = MIN( pOut->getRowCount(), pOut->getColCount());
	REAL32* rowPtr1 = pOut->getValues();
	const INT32 diagStep = pOut->getRowCount() + 1;
	INT32 lenRowLeft = pOut->getColCount();

	for ( ; numIters > 1; numIters-- )
	{
		// Find row with most non-zero entry.
		REAL32* rowPtr2 = rowPtr1;
		REAL32 maxAbs = i3Math::abs( (*rowPtr1) );
		REAL32 *rowPivot = rowPtr1;
		
		for( i = numIters - 1; i > 0; i-- )
		{
			const REAL32& newMax = *(++rowPivot);
			if ( newMax > maxAbs ) {
				maxAbs = *rowPivot;
				rowPtr2 = rowPivot;
			}
			else if ( -newMax > maxAbs ) {
				maxAbs = -newMax;
				rowPtr2 = rowPivot;
			}
		}
		// Pivot step: Swap the row with highest entry to the current row
		if ( rowPtr1 != rowPtr2 ) {
			REAL32 *to = rowPtr1;
			for ( i = lenRowLeft; i > 0; i-- ) {
				REAL32 temp = *to;
				*to = *rowPtr2;
				*rowPtr2 = temp;
				to += pOut->getRowCount();
				rowPtr2 += pOut->getRowCount();
			}
		}
		// Subtract this row appropriately from all the lower rows (row operation of type 3)
		rowPtr2 = rowPtr1;
		for ( i = numIters - 1; i > 0; i-- ) {
			rowPtr2++;
			REAL32* to = rowPtr2;
			REAL32* from = rowPtr1;
			if( *from != 0.f)
			{
				I3ASSERT( *from != 0.f );
				REAL32 alpha = (*to)/(*from);
				*to = 0.f;
				for ( j=lenRowLeft-1; j>0; j-- ) {
					to += pOut->getRowCount();
					from += pOut->getRowCount();
					*to -= (*from)*alpha;
				}
			}
		}
		// Update for next iteration of loop
		rowPtr1 += diagStep;
		lenRowLeft--;
	}
}

// Calculate the c=cosine and s=sine values for a Givens transformation.
// The matrix M = ( (c, -s), (s, c) ) in row order transforms the
//   column vector (a, b)^T to have y-coordinate zero.
void i3MatrixN::CalcGivensValues( REAL32 a, REAL32 b, REAL32 *c, REAL32 *s )
{
	REAL32 denomInv = i3Math::Sqrt( a*a + b*b);
	if ( denomInv==0.f ) {
		*c = 1.f;
		*s = 0.f;
	}
	else {
		denomInv = 1.f/denomInv;
		*c = a*denomInv;
		*s = -b*denomInv;
	}
}

// Applies Givens transform to columns i and i+1.
// Equivalent to postmultiplying by the matrix
//      ( c  -s )
//		( s   c )
// with non-zero entries in rows i and i+1 and columns i and i+1
void i3MatrixN::PostApplyGivens( MATRIX_N * pIn, REAL32 c, REAL32 s, INT32 idx )
{
	I3ASSERT( 0 <= idx && idx < pIn->getColCount() );
	INT32 i;
	REAL32 *colA = pIn->getValues() + (idx * pIn->getRowCount());
	REAL32 *colB = colA + pIn->getRowCount();

	for( i = pIn->getRowCount(); i > 0; i-- )
	{
		REAL32 temp = *colA;
		*colA = (*colA)*c + (*colB)*s;
		*colB = (*colB)*c - temp*s;
		colA++;
		colB++;
	}
}

// Applies Givens transform to columns idx1 and idx2.
// Equivalent to postmultiplying by the matrix
//      ( c  -s )
//		( s   c )
// with non-zero entries in rows idx1 and idx2 and columns idx1 and idx2
void i3MatrixN::PostApplyGivens( MATRIX_N * pIn, REAL32 c, REAL32 s, INT32 idx1, INT32 idx2 )
{
	I3ASSERT( idx1 != idx2 && 0 <= idx1 && idx1 < pIn->getColCount() && 0 <= idx2 && idx2 < pIn->getColCount() );
	INT32 i;
	REAL32 *colA = pIn->getValues() + (idx1 * pIn->getRowCount());
	REAL32 *colB = pIn->getValues() + (idx2 * pIn->getRowCount());
	for( i = pIn->getRowCount(); i > 0; i-- )
	{
		REAL32 temp = *colA;
		*colA = (*colA)*c + (*colB)*s;
		*colB = (*colB)*c - temp*s;
		colA++;
		colB++;
	}
}


// ********************************************************************************************
// Singular value decomposition.
// Return othogonal matrices U and V and diagonal matrix with diagonal w such that
//     (this) = U * Diag(w) * V^T     (V^T is V-transpose.)
// Diagonal entries have all non-zero entries before all zero entries, but are not
//		necessarily sorted.  (Someday, I will write ComputedSortedSVD that handles
//		sorting the eigenvalues by magnitude.)
// ********************************************************************************************
void i3MatrixN::ComputeSVD( MATRIX_N * pIn, MATRIX_N * U, VECND * w, MATRIX_N * V, VECND * pSuperDiag )
{
	I3ASSERT( U->getRowCount() == pIn->getRowCount() && V->getColCount() == pIn->getColCount() &&
		U->getRowCount() == U->getColCount() && V->getRowCount() == V->getColCount() &&
		w->getSize() == MIN( pIn->getRowCount(), pIn->getColCount()) );

	I3ASSERT( pSuperDiag != nullptr);
	pSuperDiag->setSize( w->getSize() - 1 );		// Some extra work space.  Will get passed around.

	// Choose larger of U, V to hold intermediate results
	// If U is larger than V, use U to store intermediate results
	// Otherwise use V.  In the latter case, we form the SVD of A transpose,
	//		(which is essentially identical to the SVD of A).
	MATRIX_N * leftMatrix;
	MATRIX_N * rightMatrix;
	if ( pIn->getRowCount() >= pIn->getColCount() )
	{
		Copy( U, pIn);				// Copy A into U
		leftMatrix = U;
		rightMatrix = V;
	}
	else
	{
		Transpose( V, pIn);		// Copy A-transpose into V
		leftMatrix = V;
		rightMatrix = U;
	}

	// Do the actual work to calculate the SVD 
	// Now matrix has at least as many rows as columns
	CalcBidiagonal( leftMatrix, rightMatrix, w, pSuperDiag );
	ConvertBidiagToDiagonal( pIn, leftMatrix, rightMatrix, w, pSuperDiag );
}

// Some useful routines for experts who understand the inner workings of these classes.
REAL32 i3MatrixN::DotArray( INT32 length, const REAL32* ptrA, INT32 strideA, const REAL32* ptrB, INT32 strideB )
{
	REAL32 rRv = 0.f;

    for ( ; length > 0 ; length-- )
	{
		rRv += (*ptrA)*(*ptrB);
		ptrA += strideA;
		ptrB += strideB;
	}

	return rRv;
}

void i3MatrixN::CopyArrayScale( INT32 length, const REAL32* from, INT32 fromStride, REAL32 *to, INT32 toStride, REAL32 scale )
{
	for ( ; length > 0; length-- )
	{
		*to = (*from) * scale;
		from += fromStride;
		to += toStride;
	}
}

void i3MatrixN::AddArrayScale( INT32 length, const REAL32* from, INT32 fromStride, REAL32 *to, INT32 toStride, REAL32 scale )
{
	for ( ; length>0; length-- )
	{
		*to += (*from) * scale;
		from += fromStride;
		to += toStride;
	}
}

// ************************************************ CalcBidiagonal **************************
// Helper routine for SVD computation
// U is a matrix to be bidiagonalized.
// On return, U and V are orthonormal and w holds the new diagonal
//	  elements and superDiag holds the super diagonal elements.
void i3MatrixN::CalcBidiagonal( MATRIX_N * U, MATRIX_N * V, VECND * w, VECND * superDiag )
{
	I3ASSERT( U->getRowCount() >= V->getRowCount() );

	// The diagonal and superdiagonal entries of the bidiagonalized
	//	  version of the U matrix
	//	  are stored in the vectors w and superDiag (temporarily).

	// Apply Householder transformations to U.
	// Householder transformations come in pairs.
	//   First, on the left, we map a portion of a column to zeros
	//   Second, on the right, we map a portion of a row to zeros
	const INT32 rowStep = U->getColCount();
	const INT32 diagStep = U->getColCount() + 1;
	REAL32 *diagPtr = U->getValues();
	REAL32 *wPtr = w->getValues();
	REAL32 *superDiagPtr = superDiag->getValues();
	INT32 colLengthLeft = U->getRowCount();
	INT32 rowLengthLeft = V->getColCount();
	bool bLoof = true;

	while ( bLoof) {
		// Apply a Householder xform on left to zero part of a column
		SvdHouseholder( diagPtr, colLengthLeft, rowLengthLeft, 1, rowStep, wPtr ); 

		if ( rowLengthLeft==2 ) {
			*superDiagPtr = *(diagPtr+rowStep);
			break;
		}
		// Apply a Householder xform on the right to zero part of a row
		SvdHouseholder( diagPtr+rowStep, rowLengthLeft-1, colLengthLeft, rowStep, 1, superDiagPtr ); 

		rowLengthLeft--;
		colLengthLeft--;
		diagPtr += diagStep;
		wPtr++;
		superDiagPtr++;
	}

	INT32 extra = 0;
	diagPtr += diagStep;
	wPtr++;
	if ( colLengthLeft > 2 ) {
		extra = 1;
		// Do one last Householder transformation when the matrix is not square
		colLengthLeft--;
		SvdHouseholder( diagPtr, colLengthLeft, 1, 1, 0, wPtr );
	}
	else {
		*wPtr = *diagPtr;
	}

	// Form U and V from the Householder transformations
	ExpandHouseholders( V, V->getColCount()-2, 1, U->getValues() + U->getRowCount(), U->getRowCount(), 1 );
	ExpandHouseholders( U, (INT32)(V->getColCount()-1+extra), 0, U->getValues(), 1, U->getRowCount() );

	// Done with bidiagonalization
	return;
}

// **************** ConvertBidiagToDiagonal ***********************************************
// Do the iterative transformation from bidiagonal form to diagonal form using
//		Givens transformation.  (Golub-Reinsch)
// U and V are square.  Size of U less than or equal to that of U.
void i3MatrixN::ConvertBidiagToDiagonal( MATRIX_N * pIn, MATRIX_N * U, MATRIX_N * V, VECND * w, VECND * superDiag )
{
	INT32 i;

	// These two index into the last bidiagonal block  (last in the matrix, it will be
	//	first one handled.
	INT32 lastBidiagIdx = V->getRowCount() - 1;
	INT32 firstBidiagIdx = 0;
	//REAL32 eps = 1.0e-15 * MAX(w.MaxAbs(), superDiag.MaxAbs());
	REAL32 eps = 1.0e-15f * MAX( i3VectorN::MaxAbs( w), i3VectorN::MaxAbs( superDiag));

	bool bLoof = true;

	while ( bLoof )
	{
		bool workLeft = UpdateBidiagIndices( &firstBidiagIdx, &lastBidiagIdx, w, superDiag, eps );
		if ( !workLeft )
		{
			break;
		}

		// Get ready for first Givens rotation
		// Push non-zero to M[2,1] with Givens transformation
		REAL32 * wPtr = w->getValues() + firstBidiagIdx;
		REAL32 * sdPtr = superDiag->getValues() + firstBidiagIdx;
		REAL32 extraOffDiag = 0.f;

		if ( (*wPtr) == 0.f )
		{
			ClearRowWithDiagonalZero( firstBidiagIdx, lastBidiagIdx, U, wPtr, sdPtr, eps );
			if ( firstBidiagIdx > 0 )
			{
				if( *(--sdPtr) <= eps  )
				{
					*sdPtr = 0.f;
				}
				else
				{
					ClearColumnWithDiagonalZero( firstBidiagIdx, V, wPtr, sdPtr, eps );
				}
			}
			continue;
		}

		// Estimate an eigenvalue from bottom four entries of M
		// This gives a lambda value which will shift the Givens rotations
		// Last four entries of M^T * M are  ( ( A, B ), ( B, C ) ).
		REAL32 A;
		A = (firstBidiagIdx<lastBidiagIdx-1) ? i3Math::NormalSquare( superDiag->getValue( lastBidiagIdx-2)) : 0.f;
		REAL32 BSq = i3Math::NormalSquare( w->getValue( lastBidiagIdx-1));
		A += BSq;										// The "A" entry of M^T * M
		REAL32 C = i3Math::NormalSquare( superDiag->getValue( lastBidiagIdx-1));	
		BSq *= C;										// The squared "B" entry
		C += i3Math::NormalSquare( w->getValue( lastBidiagIdx));		// The "C" entry
		REAL32 lambda;									// lambda will hold the estimated eigenvalue
		lambda = i3Math::Sqrt( i3Math::NormalSquare( (A-C) * 0.5f) + BSq );		// Use the lambda value that is closest to C.
		if ( A > C )
		{
			lambda = -lambda;
		}
		lambda += (A+C) * 0.5f;						// Now lambda equals the estimate for the last eigenvalue
		REAL32 t11 = i3Math::NormalSquare( w->getValue( firstBidiagIdx));
		REAL32 t12 = w->getValue( firstBidiagIdx) * superDiag->getValue( firstBidiagIdx);

		REAL32 c, s;
		CalcGivensValues( t11-lambda, t12, &c, &s );
		ApplyGivensCBTD( c, s, wPtr, sdPtr, &extraOffDiag, wPtr+1 );
		PostApplyGivens( V, c, -s, firstBidiagIdx );
		
		for ( i = firstBidiagIdx; i < (INT32)(lastBidiagIdx-1); i++ ) {
			// Push non-zero from M[i+1,i] to M[i,i+2] 
			CalcGivensValues( *wPtr, extraOffDiag, &c, &s );
			ApplyGivensCBTD( c, s, wPtr, sdPtr, &extraOffDiag, extraOffDiag, wPtr+1, sdPtr+1 );
			PostApplyGivens( U, c, -s, i );
			// Push non-zero from M[i,i+2] to M[1+2,i+1]
			CalcGivensValues( *sdPtr, extraOffDiag, &c, &s );
			ApplyGivensCBTD( c, s, sdPtr, wPtr+1, &extraOffDiag, extraOffDiag, sdPtr+1, wPtr+2 );
			PostApplyGivens( V, c, -s, i+1 );
			wPtr++;
			sdPtr++;
		}
		// Push non-zero value from M[i+1,i] to M[i,i+1] for i==lastBidiagIdx-1
		CalcGivensValues( *wPtr, extraOffDiag, &c, &s );
		ApplyGivensCBTD( c, s, wPtr, &extraOffDiag, sdPtr, wPtr+1 );
		PostApplyGivens( U, c, -s, i );

#if defined( I3_DEBUG)
		//DebugCalcBidiagCheck( pIn, V, w, superDiag, U );
#endif

	}
}

// Helper routine for CalcBidiagonal
// Performs a series of Householder transformations on a matrix
// Stores results compactly into the matrix:   The Householder vector u (normalized)
//   is stored into the first row/column being transformed.
// The leading term of that row (= plus/minus its magnitude is returned
//	 separately into "retFirstEntry"
void i3MatrixN::SvdHouseholder( REAL32 * basePt,
						    INT32 colLength, INT32 numCols, INT32 colStride, INT32 rowStride, 
							REAL32 * retFirstEntry )
{
	INT32 i;

	// Calc norm of vector u
	REAL32 * cPtr = basePt;
	REAL32 norm = 0.f;
	
	for( i = colLength; i > 0; i-- ) {
		norm += i3Math::NormalSquare( *cPtr );
		cPtr += colStride;
	}
	norm = i3Math::Sqrt( norm);					// Norm of vector to reflect to axis  e_1

	// Handle sign issues		
	REAL32 imageVal;					// Choose sign to maximize distance
	if ( (*basePt) < 0.f )
	{
		imageVal = norm;
		norm = 2.f * norm * (norm - (*basePt));	
	}
	else
	{
		imageVal = -norm;
		norm = 2.f * norm * (norm + (*basePt));
	}
	norm = i3Math::Sqrt( norm);					// Norm is norm of reflection vector

	if ( norm == 0.f )
	{	// If the vector being transformed is equal to zero
		// Force to zero in case of roundoff errors
		cPtr = basePt;
		for ( i = colLength; i > 0; i-- )
		{
			*cPtr = 0.f;
			cPtr += colStride;
		}
		*retFirstEntry = 0.f;
		return;
	}

	*retFirstEntry = imageVal;

	// Set up the normalized Householder vector
	*basePt -= imageVal;					// First component changes. Rest stay the same.
	// Normalize the vector
	norm = 1.f / norm;					// Now it is the inverse norm
	cPtr = basePt;
	for ( i = colLength; i > 0 ; i-- )
	{
		*cPtr *= norm;
		cPtr += colStride;
	}

	// Transform the rest of the U matrix with the Householder transformation
	REAL32 *rPtr = basePt;
	for( i = (INT32) (numCols-1); i > 0; i-- )
	{
		rPtr += rowStride;

		// Calc dot product with Householder transformation vector
		REAL32 dotP = DotArray( colLength, basePt, colStride, rPtr, colStride );

		// Transform with I - 2*dotP*(Householder vector)
		AddArrayScale( colLength, basePt, colStride, rPtr, colStride, -2.f * dotP );
	}
}

// ********************************* ExpandHouseholders ********************************************
// The matrix will be square.
//   numXforms = number of Householder transformations to concatenate
//		Each Householder transformation is represented by a unit vector
//		Each successive Householder transformation starts one position later
//			and has one more implied leading zero
//	 basePt = beginning of the first Householder transform
//	 colStride, rowStride: Householder xforms are stored in "columns"
//   numZerosSkipped is the number of implicit zeros on the front each
//			Householder transformation vector (only values supported are 0 and 1).
void i3MatrixN::ExpandHouseholders( MATRIX_N * pOut, INT32 numXforms, INT32 numZerosSkipped, const REAL32 * basePt, INT32 colStride, INT32 rowStride )
{
	INT32 i, j;

	// Number of applications of the last Householder transform
	//     (That are not trivial!)
	INT32 numToTransform = pOut->getColCount() - numXforms + 1 - numZerosSkipped;			
	I3ASSERT( numToTransform > 0 );

	if ( numXforms==0 )
	{
		Identity( pOut);
		return;
	}

	// Handle the first one separately as a special case,
	// "this" matrix will be treated to simulate being preloaded with the identity
	INT32 hDiagStride = rowStride + colStride;
	const REAL32 * hBase = basePt + hDiagStride*(numXforms-1);	// Pointer to the last Householder vector
	const REAL32 * hDiagPtr = hBase + colStride*(numToTransform-1);		// Pointer to last entry in that vector
	
	REAL32 * diagPtr = pOut->getValues() + (pOut->getColCount() * pOut->getRowCount()) - 1;			// Last entry in matrix (points to diagonal entry)
	REAL32 * colPtr = diagPtr-(numToTransform-1);			// Pointer to column in matrix
	for ( i = 0; i < numToTransform; i++ )
	{
		CopyArrayScale( numToTransform, hBase, colStride, colPtr, 1, -2.f * (*hDiagPtr) );
		*diagPtr += 1.f;						// Add back in 1 to the diagonal entry (since xforming the identity)
		diagPtr -= (pOut->getRowCount() + 1);				// Next diagonal entry in this matrix
		colPtr -= pOut->getRowCount();						// Next column in this matrix
		hDiagPtr -= colStride;
	}

	// Now handle the general case
	// A row of zeros must be in effect added to the top of each old column (in each loop)
	REAL32 * colLastPtr = pOut->getValues() + (pOut->getRowCount() * pOut->getColCount()) - numToTransform - 1; 
	for ( i = numXforms-1; i>0; i-- )
	{
		numToTransform++;							// Number of non-trivial applications of this Householder transformation
		hBase -= hDiagStride;						// Pointer to the beginning of the Householder transformation
		colPtr = colLastPtr;
		for ( j = numToTransform-1; j>0; j-- )
		{
			// Get dot product
			REAL32 dotProd2N = -2.f * DotArray( numToTransform-1, hBase+colStride, colStride, colPtr+1, 1 );
			*colPtr = dotProd2N * (*hBase);			// Adding onto zero at initial point
			AddArrayScale( numToTransform-1, hBase+colStride, colStride, colPtr+1, 1, dotProd2N ); 
			colPtr -= pOut->getRowCount();
		}
		// Do last one as a special case (may overwrite the Householder vector)
		CopyArrayScale( numToTransform, hBase, colStride, colPtr, 1, -2.f * (*hBase) );
		*colPtr += 1.f;				// Add back one one as identity
		// Done with this Householder transformation
		colLastPtr --;
	}    

	if ( numZerosSkipped!=0 ) {
		I3ASSERT( numZerosSkipped == 1 );
		// Fill first row and column with identity (More generally: first numZerosSkipped many rows and columns)
		REAL32 * d = pOut->getValues();
		*d = 1;
		REAL32 * d2 = d;
		for ( i = pOut->getRowCount() - 1; i > 0; i-- )
		{
			*(++d) = 0;
			*(d2+=pOut->getRowCount()) = 0;
		}
	}
}

// Helper routine for SVD conversion from bidiagonal to diagonal
bool i3MatrixN::UpdateBidiagIndices( INT32 * firstBidiagIdx, INT32 * lastBidiagIdx, VECND * w, VECND * superDiag, REAL32 eps )
{
	INT32 lastIdx = *lastBidiagIdx;
	REAL32 * sdPtr = superDiag->getValues() + lastIdx-1;		// Entry above the last diagonal entry
	while ( *sdPtr <= eps )
	{
		*(sdPtr--) = 0.f;
		lastIdx--;
		if ( lastIdx == 0 ) {
			return false;
		}
	}
	*lastBidiagIdx = lastIdx;
	INT32 firstIdx = lastIdx-1;
	REAL32 * wPtr = w->getValues() + firstIdx;
	while ( firstIdx > 0 )
	{
		if ( *wPtr <= eps ) {			// If this diagonal entry (near) zero
			*wPtr = 0.f;
			break;
		}
		if ( *(--sdPtr) <= eps ) {		// If the entry above the diagonal entry is (near) zero
			*sdPtr = 0.f;
			break;
		}
		wPtr--;
		firstIdx--;
	}
	*firstBidiagIdx = firstIdx;
	return true;
}

// Matrix A is  ( ( a c ) ( b d ) ), i.e., given in column order.
// Mult's G[c,s]  times  A, replaces pA->
void i3MatrixN::ApplyGivensCBTD( REAL32 cosine, REAL32 sine, REAL32 *a, REAL32 *b, REAL32 *c, REAL32 *d )
{
	REAL32 rTemp = *a;
	*a = cosine * (*a) - sine * (*b);
	*b = (sine * rTemp) + cosine * (*b);
	rTemp = *c;
	*c = cosine * (*c) - sine * (*d);
	*d = (sine * rTemp) + cosine * (*d);
}

// Now matrix A given in row order, A = ( ( a b c ) ( d e f ) ).
// Return G[c,s] * A, replace pA->  d becomes zero, no need to return.
//  Also, it is certain the old *c value is taken to be zero!
void i3MatrixN::ApplyGivensCBTD( REAL32 cosine, REAL32 sine, REAL32 *a, REAL32 *b, REAL32 *c,
														 REAL32  d, REAL32 *e, REAL32 *f )
{
	REAL32 rTemp = *b;

	*a = (cosine * (*a)) - (sine * d);
	*b = (cosine * (*b)) - (sine * (*e));
	*e = (sine * rTemp) + (cosine * (*e));
	*c = -sine * (*f);
	*f = cosine * (*f);
}

// This is called when there is a zero diagonal entry, with a non-zero superdiagonal entry on the same row.
// We use Givens rotations to "chase" the non-zero entry across the row; when it reaches the last
//	column, it is finally zeroed away.
// wPtr points to the zero entry on the diagonal.  sdPtr points to the non-zero superdiagonal entry on the same row.
void i3MatrixN::ClearRowWithDiagonalZero( INT32 firstBidiagIdx, INT32 lastBidiagIdx, 
										MATRIX_N * U, REAL32 *wPtr, REAL32 *sdPtr, REAL32 eps )
{
	REAL32 c, s;
	REAL32 curSd = *sdPtr;		// Value being chased across the row
	*sdPtr = 0.f;
	INT32 i = firstBidiagIdx + 1;

	bool bLoof = true;

	while ( bLoof)
	{
		// Rotate row i and row firstBidiagIdx (Givens rotation)
		
		CalcGivensValues( *(++wPtr), curSd, &c, &s );
		PostApplyGivens( U, c, -s, i, firstBidiagIdx );
		*wPtr = c*(*wPtr) - s*curSd;
		if( i == lastBidiagIdx )
		{
			break;
		}
		curSd = s*(*(++sdPtr));		// New value pops up one column over to the right
		*sdPtr = c*(*sdPtr);
		i++;
	}
}

// This is called when there is a zero diagonal entry, with a non-zero superdiagonal entry in the same column.
// We use Givens rotations to "chase" the non-zero entry up the column; when it reaches the last
//	column, it is finally zeroed away.
// wPtr points to the zero entry on the diagonal.  sdPtr points to the non-zero superdiagonal entry in the same column.
void i3MatrixN::ClearColumnWithDiagonalZero( INT32 endIdx, MATRIX_N * V, REAL32 *wPtr, REAL32 *sdPtr, REAL32 eps )
{
	REAL32 curSd = *sdPtr;		// Value being chased up the column
	*sdPtr = 0.f;
	INT32 i = endIdx-1;

	bool bLoof = true;

	while ( bLoof)
	{
		REAL32 c, s;
		CalcGivensValues( *(--wPtr), curSd, &c, &s );
		PostApplyGivens( V, c, -s, i, endIdx );
		*wPtr = c*(*wPtr) - s*curSd;

		if ( i==0 )		break;

		curSd = s*(*(--sdPtr));		// New value pops up one row above
		if ( curSd < eps )
			break;

		*sdPtr = c*(*sdPtr);
		i--;
	}
}

#if defined( I3_DEBUG)
// Good for debugging SVD computations (I recommend this be used for any new application to check for bugs/instability).
bool i3MatrixN::DebugCheckSVD( MATRIX_N * pIn, MATRIX_N * U, VECND * w, MATRIX_N * V )
{
	// Special SVD test code

	MATRIX_N IV( V->getRowCount(), V->getColCount() );
	Identity( &IV);
	MATRIX_N VTV( V->getRowCount(), V->getColCount() );
	TransposeMultiply( &VTV, V, V );		// VTV = (V-Transpose) * V
	Sub( &IV, &VTV);
	REAL32 error = FrobeniusNorm( &IV);

	MATRIX_N IU( U->getRowCount(), U->getColCount() );
	Identity( &IU);
	MATRIX_N UTU( U->getRowCount(), U->getColCount() );
	TransposeMultiply( &UTU, U, U );		// UTU = (U-Transpose) * U;
	Sub( &IU, &UTU);
	error += FrobeniusNorm( &IU);

	MATRIX_N Diag( U->getRowCount(), V->getRowCount() );
	FillZero( &Diag);
	SetDiagonalEntries( &Diag, w );
	MATRIX_N B( U->getRowCount(), V->getRowCount() );
	MATRIX_N C( U->getRowCount(), V->getRowCount() );
	Multiply( &B, U, &Diag );					// B = U * Diag
	MultiplyTranspose( &C, &B, V );				// C = B * (V-Transpose)
	Sub( &C, pIn);
	error += FrobeniusNorm( &C);

	bool ret = ( i3Math::abs( error) <= 1.0e-15f * i3VectorN::MaxAbs( w) );
	I3ASSERT ( ret );

	return ret;
}

bool i3MatrixN::DebugCalcBidiagCheck( MATRIX_N * pIn, MATRIX_N * U, VECND * w, VECND * superDiag, MATRIX_N * V )
{
	// Special SVD test code
	MATRIX_N IV( V->getRowCount(), V->getColCount() );
	Identity( &IV);

	MATRIX_N VTV( V->getRowCount(), V->getColCount() );
	TransposeMultiply( &VTV, V, V );		// VTV = (V-Transpose) * V
	Sub( &IV, &VTV);
	REAL32 error = FrobeniusNorm( &IV);

	MATRIX_N IU( U->getRowCount(), U->getColCount() );
	Identity( &IU);
	MATRIX_N UTU( U->getRowCount(), U->getColCount() );
	TransposeMultiply( &UTU, U, U );		// UTU = (U-Transpose) * U
	Sub( &IU, &UTU);
	error += FrobeniusNorm( &IU);

	MATRIX_N DiagAndSuper( U->getRowCount(), V->getRowCount() );
	FillZero( &DiagAndSuper);
	SetDiagonalEntries( &DiagAndSuper, w );
	if ( pIn->getRowCount() >= pIn->getColCount() )
	{
		SetSequence( &DiagAndSuper, superDiag, 0, 1, 1, 1 );
	}
	else
	{
		SetSequence( &DiagAndSuper, superDiag, 1, 0, 1, 1 );
	}
	MATRIX_N B( U->getRowCount(), V->getRowCount() );
	MATRIX_N C( U->getRowCount(), V->getRowCount() );
	Multiply( &B, U, &DiagAndSuper );		// B = U * DiagAndSuper
	MultiplyTranspose( &C, &B, V );			// C = (B-Transpose) * V
	Sub( &C, pIn);
	error += FrobeniusNorm( &C);

	bool ret = ( i3Math::abs( error) < 1.0e-15f * MAX( i3VectorN::MaxAbs( w), i3VectorN::MaxAbs( superDiag)) );
	I3ASSERT( ret );

	return ret;
}
#endif


