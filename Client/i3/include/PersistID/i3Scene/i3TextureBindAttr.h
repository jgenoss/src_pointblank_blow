#if !defined( __I3_TEXTURE_BIND_ATTR_H)
#define __I3_TEXTURE_BIND_ATTR_H

#include "i3RenderAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3TextureBindAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextureBindAttr);

protected:
	I3G_TEXTURE_BIND	m_BindType;
	i3Texture *			m_pTexture;

public:
	i3TextureBindAttr(void);
	virtual ~i3TextureBindAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)	{ m_BindType = bind; }
	I3G_TEXTURE_BIND	GetBindType(void)					{ return m_BindType; }

	void				SetTexture( i3Texture * pTexture);
	i3Texture *			GetTexture(void)					{ return m_pTexture; }

	virtual void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump( void);
	#endif
};

#endif
