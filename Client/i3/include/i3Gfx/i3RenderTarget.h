#if !defined( __I3_RENDER_TARGET_H)
#define __I3_RENDER_TARGET_H

#include "../i3Base/i3CommonType.h"
#include "i3GfxResource.h"
#include "i3GfxDefine.h"
#include "i3Texture.h"

class I3_EXPORT_GFX i3RenderTarget : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3RenderTarget, i3GfxResource);

protected:
	UINT32						m_Width = 0;
	UINT32						m_Height = 0;
	I3G_IMAGE_FORMAT			m_Format = I3G_IMAGE_FORMAT_NONE;
	I3G_IMAGE_FORMAT			m_DepthFormat = I3G_IMAGE_FORMAT_D24X8;
	bool						m_bLockable = false;
	bool						m_bIsAllocated = false;
	bool						m_bIsVolatile = false;
	UINT32						m_Pitch = 0;

	i3Texture *					m_pColorTex = nullptr;
	i3Texture *					m_pDepthTex = nullptr;

	IDirect3DSurface9 *			m_pColorSurface = nullptr;
	IDirect3DSurface9 *			m_pDepthSurface = nullptr;

	bool						m_bTextureCreated = false;

	REAL32						m_fScaleRate = 0.0f;

	I3G_RESOURCE_TYPE GetSurfaceType(void);


public:
	i3RenderTarget(void);
	virtual ~i3RenderTarget(void);

	bool Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, char * ppName = nullptr, bool isVolatile = true);
	bool Bind( IDirect3DSurface9 * pColorSurface, IDirect3DSurface9 * pDepthSurface);

	i3Texture *			GetColorTexture(void)			{ return m_pColorTex; }
	i3Texture *			GetDepthTexture(void)			{ return m_pDepthTex; }

	IDirect3DSurface9 * GetColorSurface(void)			{ return m_pColorSurface; }
	IDirect3DSurface9 * GetDepthSurface(void)			{ return m_pDepthSurface; }

	I3G_IMAGE_FORMAT	GetFormat(void)					{ return m_Format; }
	I3G_IMAGE_FORMAT	GetDepthFormat(void)			{ return m_DepthFormat; }

	UINT32				GetWidth(void)					{ return m_Width; }
	UINT32				GetHeight(void)					{ return m_Height; }

	void				SetAllocated(bool isAlloc)		{ m_bIsAllocated = isAlloc; }
	bool				IsAllocated()					{ return m_bIsAllocated; }

	void				SetVolatile(bool isVolatile)	{ m_bIsVolatile = isVolatile; }
	bool				IsVolatile()					{ return m_bIsVolatile; }
	
	bool				IsTextureCreated(void)			{ return m_bTextureCreated; }

	virtual void *	Lock( I3G_LOCKMETHOD lockMethod);
	virtual void	Unlock(void);

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	virtual bool		Destroy(bool bLostDevice) override;
	virtual bool		isDestroyed(void) override;
	virtual bool		Revive(i3RenderContext * pCtx) override;

#if defined( I3_DEBUG)
	virtual void		OnQueryStat( I3G_RES_STAT * pInfo) override;
	virtual	void		SaveDebugImage(char * pszPath);
#endif
};

#endif
