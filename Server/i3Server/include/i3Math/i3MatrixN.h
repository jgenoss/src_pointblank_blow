#ifndef __I3_MULTIPLEMATRIX_H
#define __I3_MULTIPLEMATRIX_H

#include "i3Base.h"
#include "i3MathDefine.h"
#include "i3MathType.h"
#include "i3MathUtil.h"
#include "i3MathGlobalVariable.h"
#include "i3Vector.h"
#include "i3VectorN.h"

struct I3_EXPORT_MATH ALIGN32 MATRIX_N
{
	INT32	m_iRows;				// Number of rows 
	INT32	m_iCols;				// Number of columns 
	REAL32*	m_pM;					// Array of vector entries - stored in column order

public:
	MATRIX_N( void)
	{
		m_iRows		= 0;
		m_iCols		= 0;
		m_pM		= NULL;
	}

	MATRIX_N( INT32 iRows, INT32 iCols)
	{
		m_iRows		= 0;
		m_iCols		= 0;
		m_pM		= NULL;

		setSize( iRows, iCols);
	}

	~MATRIX_N( void)
	{
		I3MEM_SAFE_FREE( m_pM);
	}

	INT32	getRowCount( void)							{ return m_iRows; }
	INT32	getColCount( void)							{ return m_iCols; }

	REAL32	getValue( INT32 iR, INT32 iC)
	{
		I3ASSERT( iR >= 0 && iR < m_iRows);
		I3ASSERT( iC >= 0 && iC < m_iCols);
		return m_pM[ (iR * m_iCols) + iC];
	}
	void	setValue( INT32 iR, INT32 iC, REAL32 rVal)
	{
		I3ASSERT( iR >= 0 && iR < m_iRows);
		I3ASSERT( iC >= 0 && iC < m_iCols);
		m_pM[ (iR * m_iCols) + iC] = rVal;
	}

	REAL32 * getValues( void)							{ return m_pM; }

	REAL32 * getColumnValues( INT32 iColumn)
	{
		I3ASSERT( 0 <= iColumn && iColumn < m_iCols );	
		return (m_pM + (iColumn * m_iRows));
	}

	void	setTriple( INT32 iRows, INT32 iCols, VEC3D * pVec )
	{
		INT32 ii = 3 * iRows;
		I3ASSERT( 0 <= iRows && ii+2< m_iRows && 0<=iCols && iCols<m_iCols );
		m_pM[ (iCols * m_iRows) + ii] = i3Vector::GetX( pVec);
		m_pM[ (iCols * m_iRows) + ii + 1] = i3Vector::GetY( pVec);
		m_pM[ (iCols * m_iRows) + ii + 2] = i3Vector::GetZ( pVec);
	}

	void	setSize( INT32 iRows, INT32 iCols)
	{
		if( (iRows * iCols) > (m_iRows * m_iCols) )
		{
			I3MEM_SAFE_FREE( m_pM);
			m_pM = (REAL32*) i3MemAlloc( sizeof(REAL32) * (iRows * iCols) );
		}

		m_iRows = iRows;
		m_iCols = iCols;
	}
	

};

/** \brief 임의의 크기(row x col)의 행렬을 다루는 함수들의 네임스페이스		*/
namespace i3MatrixN
{
	// Row Echelon Form and Reduced Row Echelon Form routines
	// Row echelon form here allows non-negative entries (instead of 1's) in the positions of lead variables.
	I3_EXPORT_MATH	void	ConvertToRefNoFree( MATRIX_N * pOut);				// Converts the matrix in place to row echelon form -- assumption is no free variables will be found
	
	/** \brief 주어진 행렬을 단위행렬로 만듭니다. 
		\details 주어진 행렬이 정사각행렬이 아닌경우 i==j이면 1, 아니면 0을 성분으로 갖는 행렬을 만듭니다. 
		\param [out] 단위행렬로 만들 행렬의 포인터 
		\return none		*/
	I3_EXPORT_MATH	void	Identity( MATRIX_N * pOut);

	/** \brief 주어진 행렬을 영행렬로 만듭니다.	
		\param [out] 영행렬로 만들 행렬의 포인터
		\return none		*/
	I3_EXPORT_MATH	void	FillZero( MATRIX_N * pOut);

	/** \brief pOut = pOut + pIn
		\param [in,out] pOut 첫번째 행렬(결과값이 이곳에 저장됩니다.)
		\param [in] pIn 두번째 행렬
		\return none
		\note 두 행렬의 크기는 반드시 같아야 합니다. */
	I3_EXPORT_MATH	void	Add( MATRIX_N * pOut, MATRIX_N * pIn);

	/** \brief pOut = pOut - pIn
		\param [in,out] pOut 첫번째 행렬(결과값이 이곳에 저장됩니다.)
		\param [in] pIn 두번째 행렬
		\return none
		\note 두 행렬의 크기는 반드시 같아야 합니다. */
	I3_EXPORT_MATH	void	Sub( MATRIX_N * pOut, MATRIX_N * pIn);

	/** \brief 행렬을 복사합니다. 
		\param [out] pDest 복사한 행렬을 저장할 행렬 구조체의 포인터
		\param [in] pSrc 복사할 행렬의 포인터
		\return none		
		\note pSrc행렬의 크기는 반드시 pDest행렬의 크기보다 작거나 같아야 합니다. */
	I3_EXPORT_MATH	void	Copy( MATRIX_N * pDest, MATRIX_N * pSrc);

	/** \brief 전치행렬을 구합니다. 
		\param [out] pOut 전치행렬을 저장할 행렬 구조체의 포인터
		\param [in] pIn 전치행렬을 구할 행렬의 포인터
		\return none
		\note pOut의 크기는 전치행렬의 크기보다 크거나 같아야 합니다.	*/
	I3_EXPORT_MATH	void	Transpose( MATRIX_N * pOut, MATRIX_N * pIn);

	I3_EXPORT_MATH	void	SetDiagonalEntries( MATRIX_N * pOut, REAL32 d );
	I3_EXPORT_MATH	void	SetDiagonalEntries( MATRIX_N * pOut, VECND * pIn );
	I3_EXPORT_MATH	void	SetSuperDiagonalEntries( MATRIX_N * pOut, REAL32 d );
	I3_EXPORT_MATH	void	SetSuperDiagonalEntries( MATRIX_N * pOut, VECND * pIn );
	I3_EXPORT_MATH	void	SetSubDiagonalEntries( MATRIX_N * pOut, REAL32 d );
	I3_EXPORT_MATH	void	SetSubDiagonalEntries( MATRIX_N * pOut, VECND * pIn );
	I3_EXPORT_MATH	void	SetColumn( MATRIX_N * pOut, INT32 idx, VECND * pIn );
	I3_EXPORT_MATH	void	SetRow( MATRIX_N * pOut, INT32 idx, VECND * pIn );
	I3_EXPORT_MATH	void	SetSequence( MATRIX_N * pOut, VECND * d, INT32 startRow, INT32 startCol, INT32 deltaRow, INT32 deltaCol );

	I3_EXPORT_MATH	REAL32	FrobeniusNormSq( MATRIX_N * pOut );
	I3_EXPORT_MATH	REAL32	FrobeniusNorm( MATRIX_N * pOut );

	// Operations on VECND's
	I3_EXPORT_MATH	void	Multiply( VECND * pOut, MATRIX_N * pInMat, VECND * pInVec);						// pOut = (pInMat)*(pInVec)
	I3_EXPORT_MATH	void	MultiplyTranspose( VECND * pOut, MATRIX_N * pInMat, VECND * pInVec );			// Equivalent to mult by row vector on left
	I3_EXPORT_MATH	REAL32	DotProductColumn( MATRIX_N * pInMat, VECND * pInVec, INT32 colNum );			// Returns dot product of v with i-th column

	// Operations on MATRIX_N's
	I3_EXPORT_MATH	void	Multiply( MATRIX_N * pOut, MATRIX_N * pM1, MATRIX_N * pM2 );						// Sets dst = A*B.
	I3_EXPORT_MATH	void	MultiplyTranspose( MATRIX_N * pOut, MATRIX_N * pA, MATRIX_N * pB );		// Sets dst = A*(B-tranpose).
	I3_EXPORT_MATH	void	TransposeMultiply( MATRIX_N * pOut, MATRIX_N * pA, MATRIX_N * pB );		// Sets dst = (A-transpose)*B.
	

	// Miscellaneous operation
	I3_EXPORT_MATH	void AddToDiagonal( MATRIX_N * pOut, REAL32 d );					// Adds d to each diagonal

	// Solving systems of linear equations
	I3_EXPORT_MATH	void Solve( MATRIX_N * pOut, MATRIX_N * pAugmentedMat, VECND * pB, VECND * x );	  // Solves the equation   (*this)*x = b;    Uses row operations.  Assumes *this is invertible.   

	// Givens transformation
	I3_EXPORT_MATH	void CalcGivensValues( REAL32 a, REAL32 b, REAL32 *c, REAL32 *s );
	I3_EXPORT_MATH	void PostApplyGivens( MATRIX_N * pOut, REAL32 c, REAL32 s, INT32 idx );							// Applies Givens transform to columns idx and idx+1.
	I3_EXPORT_MATH	void PostApplyGivens( MATRIX_N * pOut, REAL32 c, REAL32 s, INT32 idx1, INT32 idx2 );				// Applies Givens transform to columns idx1 and idx2.

	// Singular value decomposition
	I3_EXPORT_MATH	void ComputeSVD( MATRIX_N * pIn, MATRIX_N * U, VECND * w, MATRIX_N * V, VECND * pSuperDiag );

	// Some useful routines for experts who understand the inner workings of these classes.
	I3_EXPORT_MATH	REAL32 DotArray( INT32 length, const REAL32* ptrA, INT32 strideA, const REAL32* ptrB, INT32 strideB );
	I3_EXPORT_MATH	void CopyArrayScale( INT32 length, const REAL32* from, INT32 fromStride, REAL32 *to, INT32 toStride, REAL32 scale );
	I3_EXPORT_MATH	void AddArrayScale( INT32 length, const REAL32* from, INT32 fromStride, REAL32 *to, INT32 toStride, REAL32 scale );

	I3_EXPORT_MATH	void CalcBidiagonal( MATRIX_N * U, MATRIX_N * V, VECND * w, VECND * superDiag );
	I3_EXPORT_MATH	void ConvertBidiagToDiagonal( MATRIX_N * pIn, MATRIX_N * U, MATRIX_N * V, VECND * w, VECND * superDiag );
	I3_EXPORT_MATH	void SvdHouseholder( REAL32 * basePt,
							    INT32 colLength, INT32 numCols, INT32 colStride, INT32 rowStride, 
								REAL32 * retFirstEntry );
	I3_EXPORT_MATH	void ExpandHouseholders( MATRIX_N * pOut, INT32 numXforms, INT32 numZerosSkipped, const REAL32 * basePt, INT32 colStride, INT32 rowStride );
	I3_EXPORT_MATH	bool UpdateBidiagIndices( INT32 * firstBidiagIdx, INT32 * lastBidiagIdx, VECND * w, VECND * superDiag, REAL32 eps = I3_EPS);
	I3_EXPORT_MATH	void ApplyGivensCBTD( REAL32 cosine, REAL32 sine, REAL32 *a, REAL32 *b, REAL32 *c, REAL32 *d );
	I3_EXPORT_MATH	void ApplyGivensCBTD( REAL32 cosine, REAL32 sine, REAL32 *a, REAL32 *b, REAL32 *c, 
															 REAL32  d, REAL32 *e, REAL32 *f );
	I3_EXPORT_MATH	void ClearRowWithDiagonalZero( INT32 firstBidiagIdx, INT32 lastBidiagIdx, 
											MATRIX_N * U, REAL32 *wPtr, REAL32 *sdPtr, REAL32 eps = I3_EPS);
	I3_EXPORT_MATH	void ClearColumnWithDiagonalZero( INT32 endIdx, MATRIX_N * V, REAL32 *wPtr, REAL32 *sdPtr, REAL32 eps = I3_EPS);
#if defined( I3_DEBUG)
	// Good for debugging SVD computations (I recommend this be used for any new application to check for bugs/instability).
	I3_EXPORT_MATH	bool DebugCheckSVD( MATRIX_N * pIn, MATRIX_N * U, VECND * w, MATRIX_N * V );
	I3_EXPORT_MATH	bool DebugCalcBidiagCheck( MATRIX_N * pIn, MATRIX_N * U, VECND * w, VECND * superDiag, MATRIX_N * V );
#endif
};

#endif
