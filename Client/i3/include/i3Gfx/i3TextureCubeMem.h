#if !defined( __I3_TEXTURE_CUBE_MEM_H)
#define __I3_TEXTURE_CUBE_MEM_H

#include "i3TextureCube.h"

class I3_EXPORT_GFX i3TextureCubeMem : public i3TextureCube
{
	I3_EXPORT_GHOST_CLASS_DEFINE( i3TextureCubeMem, i3TextureCube);
protected:
	UINT32	m_LevelOffset[MAX_MIPMAP_LEVEL][6] = { 0 };
	char *	m_pBuffer = nullptr;

	I3_PLATFORM m_RuntimePlatform = I3_PLATFORM_UNKNOWN;

public:
	virtual ~i3TextureCubeMem(void);

	void SetRuntimePlatform( I3_PLATFORM Platform ){ m_RuntimePlatform = Platform; }

	virtual bool Create( UINT32 width, UINT32 height, UINT32 levels,I3G_IMAGE_FORMAT format, I3G_USAGE Usage) override;

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) override;
	virtual void	Unlock( INT32 level) override;

	virtual	char *	Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lock = 0) override;
	virtual	bool	Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level = 0) override;
	
	virtual void SetPersistData( INT32 level, INT8 * pData) override;
};

#if defined( I3G_SOFTWARE)
typedef i3TextureMem		i3TexturePlatform;
#endif

#endif
