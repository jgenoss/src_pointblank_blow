#if !defined( __I3_VIEW_MATRIX_ATTR_H)
#define __I3_VIEW_MATRIX_ATTR_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3Math.h"

class I3_EXPORT_SCENE i3ViewMatrixAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ViewMatrixAttr);

protected:
	RT_MATRIX	m_Matrix;

public:
	i3ViewMatrixAttr(void);
	virtual ~i3ViewMatrixAttr(void);

	void	SetMatrix( RT_MATRIX * pMatrix)
	{
		i3Matrix::Copy( &m_Matrix, pMatrix);
	}

	RT_MATRIX * GetMatrix(void)
	{
		return &m_Matrix;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

#if defined( I3_DEBUG)
	virtual void	Dump(void);
#endif
};

#endif
