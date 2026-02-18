#if !defined( __I3_TEXTURE_WRAP_ATTR_H)
#define __I3_TEXTURE_WRAP_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureWrapAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureWrapAttr);

protected:
	I3G_TEXTURE_BIND		m_Bind;

	I3G_TEXTURE_WRAP		m_HorzWrap;
	I3G_TEXTURE_WRAP		m_VertWrap;


public:
	i3TextureWrapAttr(void);
	virtual ~i3TextureWrapAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)			{ m_Bind = bind; }
	I3G_TEXTURE_BIND	GetBindType(void)							{ return m_Bind; }

	void				SetHorzWrap( I3G_TEXTURE_WRAP mode)			{ m_HorzWrap = mode; }
	I3G_TEXTURE_WRAP	GetHorzWrap(void)							{ return m_HorzWrap; }

	void				SetVertWrap( I3G_TEXTURE_WRAP mode)			{ m_VertWrap = mode; }
	I3G_TEXTURE_WRAP	GetVertWrap(void)							{ return m_VertWrap; }

	virtual void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void		Dump(void);
	#endif
};

#endif

