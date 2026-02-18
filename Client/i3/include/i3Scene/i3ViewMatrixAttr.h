#if !defined( __I3_VIEW_MATRIX_ATTR_H)
#define __I3_VIEW_MATRIX_ATTR_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3Math.h"

class I3_EXPORT_SCENE i3ViewMatrixAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ViewMatrixAttr, i3RenderAttr);

protected:
	MATRIX	m_Matrix;

public:
	i3ViewMatrixAttr(void);

	void	SetMatrix( MATRIX * pMatrix)
	{
		i3Matrix::Copy( &m_Matrix, pMatrix);
	}

	MATRIX * GetMatrix(void)
	{
		return &m_Matrix;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
#endif
};

#endif
