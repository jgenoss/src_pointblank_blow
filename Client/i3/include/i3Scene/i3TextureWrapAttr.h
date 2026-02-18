#if !defined( __I3_TEXTURE_WRAP_ATTR_H)
#define __I3_TEXTURE_WRAP_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureWrapAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureWrapAttr, i3RenderAttr);

protected:
	I3G_TEXTURE_BIND		m_Bind = I3G_TEXTURE_BIND_DIFFUSE;

	I3G_TEXTURE_WRAP		m_HorzWrap = I3G_TEXTURE_WRAP_WRAP;
	I3G_TEXTURE_WRAP		m_VertWrap = I3G_TEXTURE_WRAP_WRAP;


public:
	i3TextureWrapAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)			{ m_Bind = bind; }
	I3G_TEXTURE_BIND	GetBindType(void)							{ return m_Bind; }

	void				SetHorzWrap( I3G_TEXTURE_WRAP mode)			{ m_HorzWrap = mode; }
	I3G_TEXTURE_WRAP	GetHorzWrap(void)							{ return m_HorzWrap; }

	void				SetVertWrap( I3G_TEXTURE_WRAP mode)			{ m_VertWrap = mode; }
	I3G_TEXTURE_WRAP	GetVertWrap(void)							{ return m_VertWrap; }

	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void		Dump(void) override;
	#endif
};

#endif

