#if !defined( __I3_RENDERTARGET_DX_H)
#define __I3_RENDERTARGET_DX_H

#if defined( I3G_DX)
#include "../i3Base/i3CommonType.h"
#include "i3GfxDefine.h"
#include "i3GfxResource.h"
#include "i3RenderTarget.h"

class i3RenderTargetDX : public i3RenderTarget
{
protected:
	IDirect3DSurface8 *		m_pSurface;

public:
	i3RenderTargetDX(void);
	virtual ~i3RenderTargetDX(void);

	virtual BOOL Create( UINT32 width, UINT32 height, 
						I3_GFX_RASTERFORMAT Format, BOOL bLockable = FALSE);

	virtual UINT32						GetBufferSize(void);
	virtual I3_GFX_RESOURCE_TYPE		GetType(void);

	virtual	void *	Lock( I3_GFX_LOCKMETHOD lockMethod);
	virtual void	Unlock(void);
};

#endif
#endif