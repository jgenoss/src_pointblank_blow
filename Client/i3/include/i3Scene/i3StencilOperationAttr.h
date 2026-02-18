#if !defined( __I3_STENCIL_OPERATION_ATTR_H)
#define __I3_STENCIL_OPERATION_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3StencilOperationAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3StencilOperationAttr, i3RenderAttr);

protected:
	I3G_STENCIL_OP		m_SFail = I3G_STENCIL_OP_KEEP;
	I3G_STENCIL_OP		m_SPass = I3G_STENCIL_OP_KEEP;
	I3G_STENCIL_OP		m_ZFail = I3G_STENCIL_OP_KEEP;

public:
	i3StencilOperationAttr(void);

	void				SetSFail( I3G_STENCIL_OP op)		{ m_SFail = op; }
	I3G_STENCIL_OP		GetSFail(void)						{ return m_SFail; }

	void				SetSPass( I3G_STENCIL_OP op)		{ m_SPass = op; }
	I3G_STENCIL_OP		GetSPass(void)						{ return m_SPass;}

	void				SetZFail( I3G_STENCIL_OP op)		{ m_ZFail = op; }
	I3G_STENCIL_OP		GetZFail(void)						{ return m_ZFail; }

	virtual void		Apply( i3RenderContext * pContext) override;
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool		IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
