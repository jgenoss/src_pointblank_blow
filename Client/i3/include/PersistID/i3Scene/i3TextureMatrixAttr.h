#if !defined( I3SG_TEXTURE_MATRIX_ATTR_H)
#define I3SG_TEXTURE_MATRIX_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureMatrixAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureMatrixAttr);
protected:
	RT_MATRIX			m_Matrix;
	BOOL				m_bProjective;
	INT32				m_OutputElementCount;
	I3G_TEXTURE_BIND	m_Bind;

public:
	i3TextureMatrixAttr(void);
	virtual ~i3TextureMatrixAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND idx)		{ m_Bind = idx; }
	I3G_TEXTURE_BIND	GetBindType(void)						{ return m_Bind; }

	RT_MATRIX *		GetMatrix(void)							{ return &m_Matrix; }
	void			SetMatrix( RT_MATRIX * pMatrix)			{ i3Matrix::Copy( &m_Matrix, pMatrix); }

	BOOL			IsProjective(void)						{ return m_bProjective; }
	void			SetProjectiveState( BOOL bFlag)			{ m_bProjective = bFlag; }

	void			SetOutputElementCount( INT32 Count)		{ m_OutputElementCount = Count; }
	INT32			GetOutputElementCount(void)				{ return m_OutputElementCount; }

	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual void	Apply( i3RenderContext * pContext);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump(void);
	#endif
};

#endif

