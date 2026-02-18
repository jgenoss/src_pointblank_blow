#if !defined( __I3_STENCIL_FUNC_ATTR_H)
#define __I3_STENCIL_FUNC_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3StencilFuncAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3StencilFuncAttr);

protected:
	I3G_COMP_FUNC		m_Func;
	UINT8					m_StencilRef;
	UINT8					m_ReadMask;
	UINT8					m_WriteMask;

public:
	i3StencilFuncAttr(void);
	virtual ~i3StencilFuncAttr(void);

	void				SetFunction( I3G_COMP_FUNC func)		{ m_Func = func; }
	I3G_COMP_FUNC		GetFunction(void)						{ return m_Func; }

	void				SetRefValue( UINT8 val)					{ m_StencilRef = val; }
	UINT8				GetRefValue(void)						{ return m_StencilRef; }

	void				SetReadMask( UINT8 val)					{ m_ReadMask = val; }
	UINT8				GetReadMask(void)						{ return m_ReadMask; }

	void				SetWriteMask( UINT8 val)				{ m_WriteMask = val; }
	UINT8				GetWriteMask(void)						{ return m_WriteMask; }

	virtual void		Apply( i3RenderContext * pContext);
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL		IsSame( i3RenderAttr * pContext);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
