#if !defined( _I3_OPT_QVEC5_H__)
#define _I3_OPT_QVEC5_H__

class i3NMatrix;
class i3NVector
{
private:
	INT32	dim;
	double *vec = nullptr;

public:
	i3NVector(void);
	i3NVector(INT32 dim);
	~i3NVector(void);				// 가상소멸자 함부로 넣지 말고, 함부로 빼지도 말것..

	INT32	getDim(void)	const { return dim;}
	double *get(INT32 i)	{ return &vec[i];}

	void	Alloc( INT32 dim);
	void	Free(void);

	double& operator[](INT32 i)       { return vec[i]; }
    double  operator[](INT32 i) const { return vec[i]; }

	i3NVector& operator=(double s);
	i3NVector& operator=(const i3NVector& v);
	double	   operator*(const i3NVector& v);
    i3NVector& operator+=(const i3NVector& v);
	i3NVector& operator-=(const i3NVector& v);
    i3NVector& operator*=(double s);
	i3NVector& operator^(const i3NVector& v);

	void	Transcoord( i3NVector& nv, const i3NMatrix &m);

	double	lengthsq();
	double	length();
	void	normalize(void);

	double	dot3( i3NVector& v);
	double	lengthsq3();
	double	length3();
	void	normalize3(void);
};

class i3NMatrix
{
private:
	INT32 dim;
	double *m = nullptr;
public:
	i3NMatrix(void);
	i3NMatrix(INT32 dim);
	~i3NMatrix(void);					// 가상소멸자 함부로 넣지 말고, 함부로 빼지도 말것..

	INT32	getDim(void)	const { return dim;}

	void	Alloc( INT32 dim);
	void	Free(void);

	double&	operator()(INT32 i, INT32 j)	   { return m[(dim*i) + j];}
	double	operator()(INT32 i, INT32 j) const { return m[(dim*i) + j];}
	double& operator[](INT32 i)				   { return m[i];}
	double	operator[](INT32 i)			 const { return m[i];}

	i3NMatrix& operator=(double s);
	i3NMatrix& operator*=(double s);
	i3NMatrix& operator-=(double s);

	i3NMatrix& operator=(const i3NMatrix& m);
    i3NMatrix& operator+=(const i3NMatrix& m);
	i3NMatrix& operator-=(const i3NMatrix& m);

	bool	   operator==(const i3NMatrix& m);
	bool	   operator!=(const i3NMatrix& m);
	i3NMatrix& operator*=(const i3NMatrix& m);

	void	Mul( i3NVector * vn, const i3NVector& v);
	double	invert( i3NMatrix *mtx);
	void	identity(void);
	void	symmetric_sub( i3NVector& pv1, i3NVector& pv2);

protected:
	double  __Inverse( i3NMatrix &A, i3NMatrix &B);
};

#endif