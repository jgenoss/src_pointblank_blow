#if !defined( __I3_OPT_QUADRIC_H_)
#define __I3_OPT_QUADRIC_H_

#include "i3OptQVector5.h"

struct I_FACE
{
	INT32	originid[3] = { 0 };
	INT32	id[3] = { 0 };
	double	plane[4] = { 0.0 };
	double	region = 0.0;
	INT32	mark = 0;
	bool	_isValid = false;
};

class i3Quadric3
{
public:
	double aa, ab, ac, ad;
	double	   bb, bc, bd;
	double	       cc, cd;
	double			   dd;

	double reg;

	void	Init( I_FACE * plane);

public:
	i3Quadric3(void){ clear();}
	i3Quadric3( double a, double b, double c, double d, double region) { Init( a,b,c,d,region);}
	i3Quadric3( I_FACE * plane) { Init( plane);}

	void	Init( double a, double b, double c, double d, double region);
public:
	void clear( double clear = 0.0f)	{ aa=ab=ac=ad=bb=bc=bd=cc=cd=dd=clear;}

	i3Quadric3& operator=(const i3Quadric3& Q);
    i3Quadric3& operator+=(const i3Quadric3& Q);
    i3Quadric3& operator*=(double s);

	void	SetRegion( double region)	{	reg = region;}
	double	getReg(void)			{	return reg;}

	double	GetError( double x, double y, double z);
	double	GetError( VEC3D * pvErr);

	void	GetMatrix( MATRIX * pMtx);
	void	GetVector( VEC3D * vec);

	double	GetC(void)	const	{ return dd;}

	bool	findOptimalVertPos( VEC3D * pos);
	bool	findOptimalVertPos( VEC3D * v1, VEC3D * v2, VEC3D * pOut);
	bool	findOptimalVertPos( VEC3D*v1, VEC3D*v2,VEC3D*v3, VEC3D * pOut);
};

class i3Quadric
{
private:
	INT32		dim;
	i3NMatrix	A;
	i3NVector	b;
	double		c;

	double		reg;

	void	Init( i3NVector * pv1, i3NVector * pv2, i3NVector* pv3, double region);

public:
	i3Quadric( void);
	i3Quadric( INT32 dim);
	i3Quadric( i3NVector *pv1, i3NVector* pv2, i3NVector * pv3, double region){ Init( pv1, pv2, pv3, region);}

	~i3Quadric(void);

	void	Alloc( INT32 dim);
	void	Free(void);

	i3Quadric& operator=(const i3Quadric& Q);
    i3Quadric& operator+=(const i3Quadric& Q);
    i3Quadric& operator*=(double s);
	i3Quadric& operator-=(const i3Quadric& Q);

	i3Quadric& operator+=(i3Quadric3& Q);

	INT32	getDim()		const {	return dim;}
	double	getReg()		const {	return reg;}

	void	SetRegion( double reg)	{	reg = reg;}

	double	GetError( i3NVector * v);
	bool	findOptimalVertPos( i3NVector *pvPos);
};

#endif