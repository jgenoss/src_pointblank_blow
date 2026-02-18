#ifndef __I3_GFX_TEXTURE_OGLES_H
#define __I3_GFX_TEXTURE_OGLES_H

#include "i3Texture.h"

#define		INVALID_TEX_HANDLE			0

class i3TextureOGLES : public i3Texture
{
	I3_CLASS_DEFINE( i3TextureOGLES);
protected:	
	char	*					m_pBuffer[ 8];
	INT32						m_LockedCount;
	INT32						m_BufferWidth[8];

	INT32						m_PixelFormat;
	INT32						m_PixelType;
	
	UINT32						m_Handle;
	UINT32						m_DataSize;

public:
	i3TextureOGLES(void);
	virtual ~i3TextureOGLES(void);

	char *			GetImageData( INT32 level)				{ return m_pBuffer[ level]; }
	INT32			GetBufferWidth( INT32 level)			{ return m_BufferWidth[level]; }

	UINT32			GetHandle(void)							{ return m_Handle; }
	void			SetHandle( UINT32 hndl)					{ m_Handle = hndl; }

	INT32			GetPixelFormat(void)					{ return m_PixelFormat; }
	INT32			GetPixelType(void)						{ return m_PixelType; }

	virtual BOOL	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);
	virtual BOOL	Create( const char * pszPath, UINT32 Levels = 0);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock( INT32 level = 0 );
	virtual void	Unlock(void);

	virtual void	MakeRuntimeForm(void);
};

#endif //__I3_GFX_TEXTURE_DX_H
