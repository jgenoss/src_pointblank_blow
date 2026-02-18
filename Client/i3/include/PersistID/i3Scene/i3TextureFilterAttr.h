#if !defined( __I3_TEXTURE_FILTER_ATTR_H)
#define __I3_TEXTURE_FILTER_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextureFilterAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureFilterAttr);

protected:
	I3G_TEXTURE_BIND		m_Bind;

	I3G_TEXTURE_FILTER		m_MagFilter;
	I3G_TEXTURE_FILTER		m_MinFilter;

public:
	i3TextureFilterAttr(void);
	virtual ~i3TextureFilterAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND Idx)			{ m_Bind = Idx; }
	I3G_TEXTURE_BIND	GetBindType(void)							{ return m_Bind; }

	void				SetMagFilter( I3G_TEXTURE_FILTER filter)	{ m_MagFilter = filter; }
	I3G_TEXTURE_FILTER	GetMagFilter(void)							{ return m_MagFilter; }

	void				SetMinFilter( I3G_TEXTURE_FILTER filter)	{ m_MinFilter = filter; }
	I3G_TEXTURE_FILTER	GetMinFilter(void)							{ return m_MinFilter; }

	virtual void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void		Dump( void);
	#endif
};

#endif
