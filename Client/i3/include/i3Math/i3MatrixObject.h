#if !defined( __I3_MATRIX_OBJECT_H)
#define __I3_MATRIX_OBJECT_H

#include "i3Base.h"
#include "i3Matrix.h"

class I3_EXPORT_MATH i3MatrixObject : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3MatrixObject, i3PersistantElement);

protected:
	MATRIX			m_Matrix;

public:
	virtual void	SetMatrix( MATRIX * pMatrix)
	{
		i3Matrix::Copy( &m_Matrix, pMatrix);
	}

	virtual MATRIX *GetMatrix(void)
	{
		return &m_Matrix;
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;
};

class I3_EXPORT_MATH i3MatrixObjectProxy : public i3MatrixObject
{
	I3_EXPORT_CLASS_DEFINE( i3MatrixObjectProxy, i3MatrixObject );

protected:
	MATRIX	*m_pSourceMatrix = nullptr;
	MATRIX	m_LocalOffsetTx;
	VEC3D	*m_pSourcePos = nullptr;

	bool	m_bLocalOffset = false;

public:
	void	SetSourceMatrix( MATRIX *pMat )
	{
		m_pSourceMatrix = pMat;
	}

	void	SetLocalOffsetTransform( MATRIX *pMat );

	void	SetPositionSource( VEC3D *pPos )
	{
		m_pSourcePos = pPos;
	}

	virtual MATRIX *GetMatrix(void) override;
};

#endif
