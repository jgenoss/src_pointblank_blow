#if !defined( __I3_TEXTURE_FILTER_ATTR_H)
#define __I3_TEXTURE_FILTER_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureFilterAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureFilterAttr, i3RenderAttr);

protected:
	I3G_TEXTURE_BIND		m_Bind = I3G_TEXTURE_BIND_DIFFUSE;

	I3G_TEXTURE_FILTER		m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
	I3G_TEXTURE_FILTER		m_MinFilter = I3G_TEXTURE_FILTER_LINEAR;

public:
	i3TextureFilterAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND Idx)			{ m_Bind = Idx; }
	I3G_TEXTURE_BIND	GetBindType(void)							{ return m_Bind; }

	void				SetMagFilter( I3G_TEXTURE_FILTER filter)	{ m_MagFilter = filter; }
	I3G_TEXTURE_FILTER	GetMagFilter(void)							{ return m_MagFilter; }

	void				SetMinFilter( I3G_TEXTURE_FILTER filter)	{ m_MinFilter = filter; }
	I3G_TEXTURE_FILTER	GetMinFilter(void)							{ return m_MinFilter; }

	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void		Dump( void) override;
	#endif
};

#endif
