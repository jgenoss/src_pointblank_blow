#if !defined( __I3_STENCIL_OPERATION_ATTR_H)
#define __I3_STENCIL_OPERATION_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3StencilOperationAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3StencilOperationAttr);

protected:
	I3G_STENCIL_OP		m_SFail;
	I3G_STENCIL_OP		m_SPass;
	I3G_STENCIL_OP		m_ZFail;

public:
	i3StencilOperationAttr(void);
	virtual ~i3StencilOperationAttr(void);

	void				SetSFail( I3G_STENCIL_OP op)		{ m_SFail = op; }
	I3G_STENCIL_OP		GetSFail(void)						{ return m_SFail; }

	void				SetSPass( I3G_STENCIL_OP op)		{ m_SPass = op; }
	I3G_STENCIL_OP		GetSPass(void)						{ return m_SPass;}

	void				SetZFail( I3G_STENCIL_OP op)		{ m_ZFail = op; }
	I3G_STENCIL_OP		GetZFail(void)						{ return m_ZFail; }

	virtual void		Apply( i3RenderContext * pContext);
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL		IsSame( i3RenderAttr * pAttr);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
