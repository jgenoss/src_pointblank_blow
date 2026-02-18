#if !defined( I3G_TEXTURE_CUBE_DX_H)
#define I3G_TEXTURE_CUBE_DX_H

#if defined( I3G_DX)
#include "i3TextureCube.h"

class I3_EXPORT_GFX i3TextureCubeDX : public i3TextureCube
{
	I3_CLASS_DEFINE( i3TextureCubeDX);
protected:
	IDirect3DCubeTexture9 *		m_pCubeTex;
	D3DLOCKED_RECT				m_LockRect;
	INT32						m_LockedCount;

	void		SetDXTexture( IDirect3DCubeTexture9 * pTexture);

public:
	i3TextureCubeDX(void);
	virtual ~i3TextureCubeDX(void);

	IDirect3DCubeTexture9 *			GetDXImage(void)			{ return m_pCubeTex; }

	virtual BOOL	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);

	virtual char *	Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level = 0);

	virtual void	MakeRuntimeForm(void);

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	isDestroyed( void);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};
#endif

#endif