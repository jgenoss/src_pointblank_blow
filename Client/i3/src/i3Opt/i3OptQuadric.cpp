#include "i3OptPCH.h"
#include "i3OptQuadric.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3Quadric3
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3Quadric3::Init( double a, double b, double c, double d, double region)
{
	reg = region;

	aa = a*a; ab = a*b; ac = a*c; ad = a*d; 
			  bb = b*b; bc = b*c; bd = b*d; 
						cc = c*c; cd = c*d; 
								  dd = d*d; 
}
	
void i3Quadric3::Init( I_FACE * plane)
{
	Init( plane->plane[0], plane->plane[1], plane->plane[2], plane->plane[3], plane->region);
}

i3Quadric3& i3Quadric3::operator=(const i3Quadric3& Q)
{
	aa = Q.aa; ab = Q.ab; ac = Q.ac; ad = Q.ad;
			   bb = Q.bb; bc = Q.bc; bd = Q.bd;
						  cc = Q.cc; cd = Q.cd;
									 dd = Q.dd;

	reg = Q.reg;

	return *this;
}

i3Quadric3& i3Quadric3::operator+=(const i3Quadric3& Q)
{
	aa += Q.aa; ab += Q.ab; ac += Q.ac; ad += Q.ad;
				bb += Q.bb; bc += Q.bc; bd += Q.bd;
							cc += Q.cc; cd += Q.cd;
										dd += Q.dd;

	reg += Q.reg;

	return *this;
}

i3Quadric3& i3Quadric3::operator*=(double s)
{
	aa*=s; ab*=s; ac*=s; ad*=s;
		   bb*=s; bc*=s; bd*=s; 
				  cc*=s; cd*=s; 
						 dd*=s; 

	return *this;
}

double i3Quadric3::GetError( double x, double y, double z)
{
	return x*x*aa+  2*x*y*ab +  2*x*z*ac +  2*x*ad
					+ y*y*bb +  2*y*z*bc +  2*y*bd
								+ z*z*cc +  2*z*cd
										 + dd;
}

double i3Quadric3::GetError( VEC3D * pos)
{
	return GetError( pos->x, pos->y, pos->z);
}

void i3Quadric3::GetMatrix( MATRIX * pMtx)
{
	i3Matrix::SetRight	( pMtx, (REAL32)aa, (REAL32)ab, (REAL32)ac, 0.0f);
	i3Matrix::SetUp		( pMtx, (REAL32)ab, (REAL32)bb, (REAL32)bc, 0.0f);
	i3Matrix::SetAt		( pMtx, (REAL32)ac, (REAL32)bc, (REAL32)cc, 0.0f);
	i3Matrix::SetPos	( pMtx, 0.0f, 0.0f, 0.0f, 0.0f);
}

void i3Quadric3::GetVector( VEC3D *vPos)
{
	i3Vector::Set( vPos, (REAL32)ad, (REAL32)bd, (REAL32)cd);
}

void _transnormvec( VEC3D *vOut, VEC3D *vPos, MATRIX * mtx)
{
	double x = (mtx->m00*vPos->x) + (mtx->m10*vPos->y) + (mtx->m20*vPos->z);
	double y = (mtx->m01*vPos->x) + (mtx->m11*vPos->y) + (mtx->m21*vPos->z);
	double z = (mtx->m02*vPos->x) + (mtx->m12*vPos->y) + (mtx->m22*vPos->z);

	i3Vector::Set( vOut, (REAL32)x, (REAL32)y, (REAL32)z);
}

void _crossproduct( VEC3D * pvOut, VEC3D *pV1, VEC3D *pV2)
{
	i3Vector::Set( pvOut,  pV1->y*pV2->z - pV2->y*pV1->z,
						  -pV1->x*pV2->z + pV2->x*pV1->z, 
						   pV1->x*pV2->y - pV2->x*pV1->y );
}

bool i3Quadric3::findOptimalVertPos( VEC3D * pos)
{
	MATRIX mtxInv, mtx;
	double det = 0.0f;
	VEC3D vx, vy, vz, vPos;
	MATRIX mtxQuad;

	GetMatrix( &mtxQuad);
	GetVector( &vPos);

	_crossproduct( &vx, i3Matrix::GetUp(&mtxQuad), i3Matrix::GetAt(&mtxQuad));
	_crossproduct( &vy, i3Matrix::GetAt(&mtxQuad), i3Matrix::GetRight(&mtxQuad));
	_crossproduct( &vz, i3Matrix::GetRight(&mtxQuad), i3Matrix::GetUp(&mtxQuad));

	det = i3Vector::Dot( &vx, i3Matrix::GetRight( &mtxQuad));
	if(fabs(det) < 1e-12)
		return false;

	double idet = 1.0f / det;

	i3Vector::Scale( &vx, &vx, (REAL32)idet);
	i3Vector::Scale( &vy, &vy, (REAL32)idet);
	i3Vector::Scale( &vz, &vz, (REAL32)idet);

	i3Matrix::SetRight( &mtx, &vx);
	i3Matrix::SetUp( &mtx, &vy);
	i3Matrix::SetAt( &mtx, &vz);

	i3Matrix::Transpose3x3( &mtxInv, &mtx);

	//
	i3Vector::Set( pos, i3Vector::Dot( i3Matrix::GetRight( &mtxInv), &vPos),
						 i3Vector::Dot( i3Matrix::GetUp( &mtxInv), &vPos),
						 i3Vector::Dot( i3Matrix::GetAt( &mtxInv), &vPos));
	i3Vector::Scale( pos, pos, -1.0f);

	return true;
}


bool i3Quadric3::findOptimalVertPos( VEC3D * v1, VEC3D * v2, VEC3D * pOut)
{
	VEC3D vDiff, vPos, vAV2, vAD;
	MATRIX mtxQuad;

	GetMatrix( &mtxQuad);
	GetVector( &vPos);
		
	i3Vector::Sub( &vDiff, v1, v2);
	
	i3Vector::TransformNormal( &vAV2, v2, &mtxQuad);
	i3Vector::TransformNormal( &vAD, &vDiff, &mtxQuad);

	double denom = 2 * i3Vector::Dot(&vDiff, &vAD);
	if( fabs( denom) > 1e-12)
	{
		double a = (-2.0f * i3Vector::Dot( &vPos, &vDiff) - i3Vector::Dot(&vDiff, &vAV2) - i3Vector::Dot( v2, &vAD)) / (2.0f* i3Vector::Dot( &vDiff, &vAD));
		a = MINMAX( 0.0f, a, 1.0f);

		i3Vector::Scale( &vDiff, &vDiff, (REAL32)a);
		i3Vector::Add( pOut, &vDiff, v2);

		return true;
	}

	return false;
}

bool i3Quadric3::findOptimalVertPos( VEC3D*v1, VEC3D*v2,VEC3D*v3, VEC3D * pOut)
{
	VEC3D vPos;
	MATRIX mtxQuad;

	GetMatrix( &mtxQuad);
	GetVector( &vPos);

	VEC3D vDif13, vDif23;
	i3Vector::Sub(&vDif13, v1, v3);
	i3Vector::Sub(&vDif23, v2, v3);

	VEC3D ad13;	i3Vector::TransformNormal(&ad13, &vDif13, &mtxQuad);
	VEC3D ad23;	i3Vector::TransformNormal(&ad23, &vDif23, &mtxQuad);
	VEC3D av3;	i3Vector::TransformNormal(&av3, v3, &mtxQuad);

	double d13_d23 = i3Vector::Dot( &vDif13, &ad23) + i3Vector::Dot( &vDif23, &ad13);
	double v3_d13  = i3Vector::Dot( &vDif13, &av3) + i3Vector::Dot( v3, &ad13);
	double v3_d23  = i3Vector::Dot( &vDif23, &av3) + i3Vector::Dot( v3, &ad23);

	double d23ad23 = i3Vector::Dot( &vDif23, &ad23);
	double d13ad13 = i3Vector::Dot( &vDif23, &ad13);

    double denom = d13ad13*d23ad23 - 2*d13_d23;
	if( fabs( denom) > 1e-12)
	{
		double a = (d23ad23 * (2 * i3Vector::Dot(&vPos, &vDif13) + v3_d13) - 
			d13_d23*(2*i3Vector::Dot(&vPos, &vDif23)+v3_d23)) / -denom;

		double b = (d13ad13*(2*i3Vector::Dot(&vPos, &vDif23)+v3_d23) - 
			d13_d23*(2*i3Vector::Dot(&vPos,&vDif13)+v3_d13)) / -denom;

		a = MINMAX( 0.0f, a, 1.0f);
		b = MINMAX( 0.0f, b, 1.0f);

		i3Vector::Scale(&vDif13, &vDif13, (REAL32)a);
		i3Vector::Scale(&vDif23, &vDif23, (REAL32)b);
		i3Vector::Add(pOut, &vDif13, &vDif23);
		i3Vector::Add(pOut, pOut, v3);

		return true;
	}

	return false;
}

i3Quadric::i3Quadric(void)
{
	dim	= 0;
	c	= 0.0;

	reg = 0.0;
}

void i3Quadric::Alloc( INT32 dimension)
{
	dim = dimension;

	A.Alloc( dim);
	b.Alloc( dim);
	c = 0.0;
}

void i3Quadric::Free(void)
{
	A.Free();
	b.Free();
}

void i3Quadric::Init( i3NVector * pv1, i3NVector * pv2, i3NVector * pv3, double r)
{
	I3ASSERT( pv1->getDim() == pv1->getDim() && pv1->getDim() == pv3->getDim());

	Alloc( pv1->getDim());

	reg = r;
	i3NVector e1(dim), e2(dim), t(dim);

	// e1 = p2-p1; unitize
	e1  = *pv2;
	e1 -= *pv1;
	e1.normalize();	

	//I3ASSERT( (1.0 - e1.length()) < I3_EPS);

	// e2 = p3-p1
	e2  = *pv3;
	e2 -= *pv1;					

	// e2 = p3-p1-e1*(e1*(p3-p1)); unitize
	t  = e1;
	t *= e1*e2;

	e2 -= t;
	e2.normalize();

	//I3ASSERT( (1.0 - e2.length()) < I3_EPS);

    double	p1e1 = *pv1*e1;
    double	p1e2 = *pv1*e2;

	A.identity();
	A.symmetric_sub( e1, e1);
	A.symmetric_sub( e2, e2);

    // b = e1*p1e1 + e2*p1e2 - p1
    b=e1; b*=p1e1;
	t=e2; t*=p1e2;

	b += t; 
	b -= *pv1;

    c = (*pv1)*(*pv1) - p1e1*p1e1 - p1e2*p1e2;
}

i3Quadric::i3Quadric( INT32 dimension)
{
	Alloc( dimension);

	reg = 0.0;
}

i3Quadric::~i3Quadric(void)
{
	Free();		
}

i3Quadric& i3Quadric::operator=(const i3Quadric& Q)
{
	A = Q.A; b = Q.b; c = Q.c; reg = Q.reg;
	return *this;
}

i3Quadric& i3Quadric::operator+=(const i3Quadric& Q)
{
	A += Q.A; b += Q.b; c += Q.c; reg += Q.reg;
	return *this;
}

i3Quadric& i3Quadric::operator*=(double s)
{
	A *= s; b *= s; c *= s;
	return *this;
}

i3Quadric& i3Quadric::operator-=(const i3Quadric& Q)
{
	A -= Q.A; b -= Q.b; c -= Q.c; reg -= Q.reg;
	return *this;
}

i3Quadric& i3Quadric::operator+=(i3Quadric3& Q)
{
	A(0,0) += Q.aa;	A(0,1) += Q.ab;	A(0,2) += Q.ac;
	A(1,0) += Q.ab;	A(1,1) += Q.bb;	A(1,2) += Q.bc;
	A(2,0) += Q.ac;	A(2,1) += Q.bc;	A(2,2) += Q.cc;

	b[0] += Q.ad;	b[1] += Q.bd;	b[2] += Q.cd;

	c += Q.dd;

	reg += Q.getReg();

	return *this;
}

double i3Quadric::GetError(i3NVector * v)
{
	i3NVector nv(v->getDim());

	A.Mul( &nv, *v);

	return (*v)*nv + 2*(b*(*v)) + c;
}

bool i3Quadric::findOptimalVertPos( i3NVector * pvPos)
{
	i3NMatrix Ainv( A.getDim());
	Ainv.identity();

	double det = A.invert( &Ainv);
	if( fabs( det) < 1e-13)
	{
		return false;
	}

	Ainv.Mul( pvPos, b);

	*pvPos *= -1.0;

	return true;
}

