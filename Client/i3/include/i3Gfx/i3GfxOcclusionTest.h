#if !defined( __I3_GFX_OCCLUSION_TEST_H)
#define __I3_GFX_OCCLUSION_TEST_H

#include "i3GfxResource.h"

class I3_EXPORT_GFX i3GfxOcclusionTest : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3GfxOcclusionTest, i3GfxResource);

protected:
	INT32					m_DrawnPixelCount = -1;
	IDirect3DQuery9 *		m_pQuery = nullptr;

public:
	virtual ~i3GfxOcclusionTest(void);

	INT32				getDrawnPixelCount(void)				{ return m_DrawnPixelCount; }

	void				Create(void);

	void				Begin(void);
	void				End(void);
	bool				Update( bool bWait = false);

	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		isDestroyed( void) override;
	virtual bool		Revive( i3RenderContext * pCtx) override;
};

#endif
