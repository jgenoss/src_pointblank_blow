#if !defined( __I3_TEXTURE_MEM_H)
#define __I3_TEXTURE_MEM_H

#include "i3Texture.h"

class I3_EXPORT_GFX i3TextureMem : public i3Texture
{
	I3_EXPORT_GHOST_CLASS_DEFINE(i3TextureMem, i3Texture);

protected:
	char *	m_pBuffer = nullptr;

	I3_PLATFORM m_RuntimePlatform = I3_PLATFORM_UNKNOWN;

public:
	virtual ~i3TextureMem(void);

	void SetRuntimePlatform( I3_PLATFORM Platform ){ m_RuntimePlatform = Platform; }

	virtual bool Create( UINT32 width, UINT32 height, UINT32 levels,I3G_IMAGE_FORMAT format, I3G_USAGE Usage) override;

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) override;
	virtual void	Unlock( INT32 level) override;

	virtual	char *	SafeLock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) override { return Lock( level, lock, pRect); }
	virtual	bool	SafeUnlock( INT32 level) override { Unlock( level); return true;}

	virtual bool	Destroy(bool bLostDevice) override { return true; }
	virtual bool	isDestroyed(void) override { return true; }
	virtual bool	Revive(i3RenderContext * pCtx) override { return true; }
	
	virtual void SetPersistData( INT32 level, INT8 * pData) override;
};

#if defined( I3G_SOFTWARE)
typedef i3TextureMem		i3TexturePlatform;
#endif

#endif
