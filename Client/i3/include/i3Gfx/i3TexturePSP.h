#ifndef __I3_GFX_TEXTURE_PSP_H
#define __I3_GFX_TEXTURE_PSP_H

#include "i3Texture.h"

class i3TexturePSP : public i3Texture
{
	I3_CLASS_DEFINE( i3TexturePSP);
protected:
	INT32						m_BufferIdx; 
	char	*					m_pBuffer[ 8];
	INT32						m_LockedCount;
	INT32						m_NativeFormat;
	INT32						m_BufferWidth[8];
	

public:
	i3TexturePSP(void);
	virtual ~i3TexturePSP(void);

	char *			GetImageData( INT32 level)				{ return m_pBuffer[ level]; }
	INT32			GetNativeFormat(void)					{ return m_NativeFormat; }
	INT32			GetBufferWidth( INT32 level)			{ return m_BufferWidth[level]; }

	virtual BOOL	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);
	virtual BOOL	Create( const char * pszPath, UINT32 Levels = 0);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock( INT32 level = 0);

	virtual void	MakeRuntimeForm(void);
};

#endif //__I3_GFX_TEXTURE_DX_H
