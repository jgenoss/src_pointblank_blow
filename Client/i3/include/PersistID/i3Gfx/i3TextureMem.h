#if !defined( __I3_TEXTURE_MEM_H)
#define __I3_TEXTURE_MEM_H

#include "i3Texture.h"

class I3_EXPORT_GFX i3TextureMem : public i3Texture
{
	I3_CLASS_DEFINE( i3TextureMem);

protected:
	UINT32	m_LevelOffset[ MAX_MIPMAP_LEVEL];
	char *	m_pBuffer;
	//INT32	m_BufferIdx; 

	I3_PLATFORM m_RuntimePlatform;

public:
	i3TextureMem(void);
	virtual ~i3TextureMem(void);

	void SetRuntimePlatform( I3_PLATFORM Platform ){ m_RuntimePlatform = Platform; }

	virtual BOOL Create( UINT32 width, UINT32 height, UINT32 levels,I3G_IMAGE_FORMAT format, I3G_USAGE Usage);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);
	
	virtual void SetPersistData( INT32 level, INT8 * pData);
};

#if defined( I3G_SOFTWARE)
typedef i3TextureMem		i3TexturePlatform;
#endif

#endif
