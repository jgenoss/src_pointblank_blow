#if !defined( I3_SG_TEXTURE_COORD_SOURCE_ATTR_H)
#define I3_SG_TEXTURE_COORD_SOURCE_ATTR_H

#include "i3RenderAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3TextureCoordSourceAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureCoordSourceAttr, i3RenderAttr);

protected:
	I3G_TEXTURE_BIND			m_Bind = I3G_TEXTURE_BIND_DIFFUSE;
	I3G_TEXTURE_COORD_SOURCE	m_TexSource = I3G_TEXTURE_COORD_SOURCE_VERTEX;

public:
	i3TextureCoordSourceAttr(void);

	void						SetBindType( I3G_TEXTURE_BIND bind)			{ m_Bind = bind; }
	I3G_TEXTURE_BIND			GetBindType(void)							{ return m_Bind; }

	void						SetMode( I3G_TEXTURE_COORD_SOURCE source)		{ m_TexSource = source; }
	I3G_TEXTURE_COORD_SOURCE	GetMode(void)									{ return m_TexSource; }

	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual void Apply( i3RenderContext * pContext) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;
	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif
