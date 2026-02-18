#include "i3OptPCH.h"
#include "i3OptQVector5.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// i3NVector
////////////////////////////////////////////////////////////////////////////////////////////////
i3NVector::i3NVector(void)
{
	dim = 0;
}

void i3NVector::Alloc( INT32 nDim)
{
	dim = nDim;
	vec = (double*)i3MemAlloc( sizeof( double) * dim);

	i3mem::FillZero( vec, sizeof( double)*dim);
}

void i3NVector::Free(void)
{
	I3MEM_SAFE_FREE( vec);
}

i3NVector::~i3NVector(void)
{
	Free();
}

i3NVector::i3NVector(INT32 dimension)
{
	dim = 0;

	Alloc( dimension);
}

i3NVector& i3NVector::operator=(double s)
{
	for( INT32 i = 0; i < dim; ++i)	vec[i] = s;

	return *this;
}

i3NVector& i3NVector::operator=(const i3NVector& v)
{
	for(INT32 i = 0; i < dim; ++i)	vec[i] = v[i];
	return *this;
}

i3NVector& i3NVector::operator+=(const i3NVector& v)
{
	for( INT32 i = 0; i < dim; ++i)	vec[i] += v[i];
	return *this;
}

i3NVector& i3NVector::operator-=(const i3NVector& v)
{
	for( INT32 i = 0; i < dim; ++i)	vec[i] -= v[i];
	return *this;
}


i3NVector& i3NVector::operator*=(double s)
{
	for( INT32 i = 0; i < dim; ++i)	vec[i] *= s;
	return *this;
}

double i3NVector::operator*(const i3NVector& v)
{
	double dot = 0.0;
	for( INT32 i = 0;i < dim; ++i)
	{
		dot += vec[i]*v[i];
	}

	return dot;
}

i3NVector& i3NVector::operator^(const i3NVector& v)
{
	i3NVector nv(3);

	nv[0] = vec[1]*v[2] - vec[2]*v[1];
	nv[1] = vec[2]*v[0] - vec[0]*v[2];
	nv[2] = vec[0]*v[1] - vec[1]*v[0];

	*this = nv;

	return *this;
}

void i3NVector::Transcoord( i3NVector& nv, const i3NMatrix &m)
{
	for( INT32 i = 0;i < dim; ++i)
	{
		nv[i] = 0.0;
		for( INT32 j = 0; j < dim; ++j)
		{
			nv[i] += m( j, i) * vec[j];
		}
	}
}

double i3NVector::lengthsq(void)
{
	double lengthsq = 0.0;

	for( INT32 i = 0; i < dim; ++i)	lengthsq += vec[i]*vec[i];

	return lengthsq;
}

double i3NVector::length(void)
{
	return (sqrt(lengthsq()));
}

void i3NVector::normalize(void)
{
	double length = i3NVector::lengthsq();
	if( length !=1.0 && length !=0.0 )
	{
		length = sqrt( length);
		length = 1.0 / length;

		for( INT32 i = 0; i < dim; ++i)	vec[i] *= length;
	}
}

double i3NVector::dot3( i3NVector& v)
{
	double dot = 0.0;

	for( INT32 i = 0; i < 3; ++i)	dot += vec[i]*v[i];

	return dot;
}

double i3NVector::lengthsq3()
{
	double lengthsq = 0.0;

	for( INT32 i = 0; i < 3; ++i)	lengthsq += vec[i]*vec[i];

	return lengthsq;
}

double i3NVector::length3()
{
	return (sqrt(lengthsq3()));
}

void i3NVector::normalize3(void)
{
	double length = i3NVector::lengthsq3();
	if( length !=1.0 && length !=0.0 )
	{
		length = sqrt( length);
		length = 1.0 / length;

		for( INT32 i = 0; i < 3; ++i)	vec[i] *= length;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// i3NMatrix
////////////////////////////////////////////////////////////////////////////////////////////////
i3NMatrix::i3NMatrix(void)
{
	dim = 0;
}

i3NMatrix::i3NMatrix( INT32 dimension)
{
	dim = 0;

	Alloc( dimension);
}

void i3NMatrix::Alloc( INT32 dimension)
{
	dim = dimension;
	if( dim <= 0)	return;

	m = (double*)i3MemAlloc( sizeof( double) * dim * dim);

	i3mem::FillZero( m, sizeof( double) * dim * dim);
}

void i3NMatrix::Free(void)
{
	I3MEM_SAFE_FREE( m);
}

i3NMatrix::~i3NMatrix(void)
{
	Free();
}

i3NMatrix& i3NMatrix::operator=(double s)
{
	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] = s;
	}

	return *this;
}

i3NMatrix& i3NMatrix::operator=(const i3NMatrix& nM)
{
	I3ASSERT( nM.dim == dim);

	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] = nM[i];
	}

	return *this;
}

i3NMatrix& i3NMatrix::operator*=(double s)
{
	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] *= s;
	}

	return *this;
}

i3NMatrix& i3NMatrix::operator+=(const i3NMatrix& nM)
{
	I3ASSERT( nM.dim == dim);

	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] += nM[i];
	}

	return *this;
}

i3NMatrix&  i3NMatrix::operator-=(double s)
{
	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] -= s;
	}

	return *this;
}

i3NMatrix& i3NMatrix::operator-=(const i3NMatrix& nM)
{
	I3ASSERT( nM.dim == dim);

	for( INT32 i = 0; i< dim*dim; ++i)
	{
		m[i] -= nM[i];
	}

	return *this;
}

bool i3NMatrix::operator==(const i3NMatrix& nM)
{
	I3ASSERT( nM.dim == dim);

	for( INT32 i = 0; i< dim*dim; ++i)
	{
		if( m[i] != nM[i])	return false;
	}

	return true;
}

bool i3NMatrix::operator!=(const i3NMatrix& nM)
{
	I3ASSERT( nM.dim == dim);

	for( INT32 i = 0; i< dim*dim; ++i)
	{
		if( m[i] != nM[i])	return true;
	}

	return false;
}

i3NMatrix&	i3NMatrix::operator*=(const i3NMatrix& nM)
{
	i3NMatrix n( dim);

	for( INT32 i = 0; i< dim; ++i)
	{
		for( INT32 j = 0; j< dim; j++)
		{
			for( INT32 k = 0; k< dim; k++)
			{
				n(i,j) += (*this)(i,k) * nM(k,j);
			}
		}
	}

	*this=n;

	return *this;
}



void i3NMatrix::Mul( i3NVector * vn, const i3NVector& v)
{
	for( INT32 i = 0; i< dim; ++i)
	{
		(*vn)[i] = 0.0;
		for( INT32 j = 0; j< dim; j++)
		{
			(*vn)[i] += (*this)(i,j) * v[j];
		}
	}
}

void i3NMatrix::identity(void)
{
	for( INT32 i = 0;i < dim; ++i)
	for( INT32 j = 0; j< dim; ++j)
	{
		(*this)(i,j) = (double)(i==j);
	}
}


void i3NMatrix::symmetric_sub( i3NVector &pv1, i3NVector &pv2)
{
	for( INT32 i = 0;i < dim; ++i)
	for( INT32 j = 0;j < dim; ++j)
	{
		(*this)(i,j) -= pv1[i] * pv2[j];
	}
}

#define SWAP(a, b, t)   {t = a; a = b; b = t;}

double i3NMatrix::__Inverse( i3NMatrix &A, i3NMatrix &B)
{
	INT32 N = dim;
	INT32 i=0, j=0, k=0 ;
    double max, t, det, pivot;

    /*---------- forward elimination ----------*/
	B.identity();

    det = 1.0;
    for (i=0; i<N; i++)
	{               /* eliminate in column i, below diag */
        max = -1.0;
        for (k=i; k<N; k++)             /* find pivot for column i */
		{
            if (fabs(A(k, i)) > max) 
			{
                max = fabs(A(k, i));
                j = k;
            }
		}

        if (max<=0.0) return 0.;         /* if no nonzero pivot, PUNT */
        if (j != i)                      /* swap rows i and j */
		{
            for (k=i; k<N; k++)
			{
                SWAP(A(i, k), A(j, k), t);
			}

            for (k=0; k<N; k++)
			{
                SWAP(B(i, k), B(j, k), t);
			}

            det = -det;
        }

        pivot = A(i, i);
        det *= pivot;
        for (k=i+1; k<N; k++)           /* only do elems to right of pivot */
		{
            A(i, k) /= pivot;
		}

        for (k=0; k<N; k++)
		{
            B(i, k) /= pivot;
		}
        /* we know that A(i, i) will be set to 1, so don't bother to do it */

        for (j=i+1; j<N; j++)          /* eliminate in rows below i */
		{
            t = A(j, i);                /* we're gonna zero this guy */

            for (k=i+1; k<N; k++)       /* subtract scaled row i from row j */
			{
                A(j, k) -= A(i, k)*t;   /* (ignore k<=i, we know they're 0) */
			}

            for (k=0; k<N; k++)
			{
                B(j, k) -= B(i, k)*t;
			}
        }
    }

    /*---------- backward elimination ----------*/

    for (i=N-1; i>0; i--)              /* eliminate in column i, above diag */
	{
        for (j=0; j<i; j++)            /* eliminate in rows above i */
		{
            t = A(j, i);                /* we're gonna zero this guy */

            for (k=0; k<N; k++)         /* subtract scaled row i from row j */
			{
                B(j, k) -= B(i, k)*t;
			}
        }
    }

    return det;
}

double i3NMatrix::invert( i3NMatrix *mtx)
{
	i3NMatrix src, dest;
	
	src.Alloc( dim);
	src = *this;

	dest.Alloc( dim);

	double det = __Inverse(src, dest);
	*mtx = dest;

	return det;
}