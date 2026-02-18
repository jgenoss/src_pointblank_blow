#if !defined( __I3_TEXTURE_CUBE_MEM_H)
#define __I3_TEXTURE_CUBE_MEM_H

#include "i3TextureCube.h"

class I3_EXPORT_GFX i3TextureCubeMem : public i3TextureCube
{
	I3_CLASS_DEFINE( i3TextureCubeMem);

protected:
	UINT32	m_LevelOffset[ MAX_MIPMAP_LEVEL][6];
	char *	m_pBuffer;
	//INT32	m_BufferIdx; 

	I3_PLATFORM m_RuntimePlatform;

public:
	i3TextureCubeMem(void);
	virtual ~i3TextureCubeMem(void);

	void SetRuntimePlatform( I3_PLATFORM Platform ){ m_RuntimePlatform = Platform; }

	virtual BOOL Create( UINT32 width, UINT32 height, UINT32 levels,I3G_IMAGE_FORMAT format, I3G_USAGE Usage);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);

	virtual	char *	Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual	void	Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level = 0);
	
	virtual void SetPersistData( INT32 level, INT8 * pData);
};

#if defined( I3G_SOFTWARE)
typedef i3TextureMem		i3TexturePlatform;
#endif

#endif
