#if !defined( __I3_MATRIX_OBJECT_H)
#define __I3_MATRIX_OBJECT_H

#include "i3Base.h"
#include "i3Matrix.h"
#include "i3FixedPointMatrix.h"

class I3_EXPORT_MATH i3MatrixObject : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3MatrixObject);

protected:
	RT_MATRIX			m_Matrix;

public:
	i3MatrixObject(void);
	virtual ~i3MatrixObject(void);

	virtual void	SetMatrix( RT_MATRIX * pMatrix)
	{
		i3Matrix::Copy( &m_Matrix, pMatrix);
	}

	virtual RT_MATRIX *GetMatrix(void)
	{
		return &m_Matrix;
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

class I3_EXPORT_MATH i3MatrixObjectProxy : public i3MatrixObject
{
	I3_CLASS_DEFINE( i3MatrixObjectProxy );

protected:
	RT_MATRIX	*m_pSourceMatrix;
	RT_MATRIX	m_LocalOffsetTx;
	RT_VEC3D	*m_pSourcePos;

	BOOL	m_bLocalOffset;

public:
	i3MatrixObjectProxy(void);
	virtual ~i3MatrixObjectProxy(void);

	void	SetSourceMatrix( RT_MATRIX *pMat )
	{
		m_pSourceMatrix = pMat;
	}

	void	SetLocalOffsetTransform( RT_MATRIX *pMat );

	void	SetPositionSource( RT_VEC3D *pPos )
	{
		m_pSourcePos = pPos;
	}

	virtual RT_MATRIX *GetMatrix(void);
};

#endif
