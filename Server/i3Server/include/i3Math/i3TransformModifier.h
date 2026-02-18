#if !defined( __I3_TRANSFORM_MODIFIER_H)
#define __I3_TRANSFORM_MODIFIER_H

#include "i3Matrix.h"

class I3_EXPORT_MATH i3TransformModifier : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3TransformModifier);

public:
	enum APPLY_TYPE
	{
		APPLY_NONE = 0,
		APPLY_COPY,
		APPLY_PREMULTIPLY,
		APPLY_POSTMULTIPLY,
	};

protected:
	MATRIX		m_Matrix;
	APPLY_TYPE	m_ApplyType;

public:
	i3TransformModifier(void);
	virtual ~i3TransformModifier(void);

	MATRIX *	getMatrix(void)					{ return &m_Matrix; }
	void		setMatrix( MATRIX * pMtx)		{ i3Matrix::Copy( &m_Matrix, pMtx); }

	APPLY_TYPE	getApplyType(void)				{ return m_ApplyType; }
	void		setApplyType( APPLY_TYPE type)	{ m_ApplyType = type; }

	void		Apply( MATRIX * pMtx);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
