#if !defined( __I3_RENDERTARGET_DX_H)
#define __I3_RENDERTARGET_DX_H

#if defined( I3G_OGLES)
#include "../i3Base/i3CommonType.h"
#include "i3GfxDefine.h"
#include "i3GfxResource.h"
#include "i3RenderTarget.h"

class i3RenderTargetOGLES : public i3RenderTarget
{
public:
	i3RenderTargetOGLES(void);
	virtual ~i3RenderTargetOGLES(void);

	virtual BOOL Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, BOOL bLockable = FALSE);

	virtual I3_GFX_RESOURCE_TYPE		GetType(void);

	virtual	void *	Lock( I3_GFX_LOCKMETHOD lockMethod);
	virtual void	Unlock(void);
};

#endif
#endif