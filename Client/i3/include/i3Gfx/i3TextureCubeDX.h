#if !defined( I3G_TEXTURE_CUBE_DX_H)
#define I3G_TEXTURE_CUBE_DX_H

#if defined( I3G_DX)
#include "i3TextureCube.h"

class I3_EXPORT_GFX i3TextureCubeDX : public i3TextureCube
{
//	I3_CLASS_DEFINE( i3TextureCubeDX);
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3TextureCubeDX, i3TextureCube, i3::cms_concrete_d3d9::value);
protected:
	IDirect3DCubeTexture9 *		m_pCubeTex = nullptr;
	D3DLOCKED_RECT				m_LockRect;
	INT32						m_LockedCount = 0;

	void		SetDXTexture( IDirect3DCubeTexture9 * pTexture);

public:
	i3TextureCubeDX(void);
	virtual ~i3TextureCubeDX(void);

	IDirect3DCubeTexture9 *			GetDXImage(void)			{ return m_pCubeTex; }

	virtual bool	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage) override;

	virtual char *	Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lock = 0) override;
	virtual bool	Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level = 0) override;

	virtual void	MakeRuntimeForm(void) override;

	virtual bool	Destroy( bool bLostDevice) override;
	virtual bool	isDestroyed( void) override;
	virtual bool	Revive( i3RenderContext * pCtx) override;

	#if defined( I3_DEBUG)
	virtual	void		OnQueryStat( I3G_RES_STAT * pInfo) override;
	#endif
};
#endif

#endif