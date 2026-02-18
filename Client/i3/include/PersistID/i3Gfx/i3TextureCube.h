#if !defined( I3G_CUBE_TEXTURE_H)
#define I3G_CUBE_TEXTURE_H

#include "i3Texture.h"
#include "i3GfxDefine.h"

class I3_EXPORT_GFX i3TextureCube : public i3Texture
{
	I3_ABSTRACT_CLASS_DEFINE( i3TextureCube);

public:
	i3TextureCube(void);
	virtual ~i3TextureCube(void);

	virtual UINT32		GetBitsPerPixel( INT32 lv = 0)			{ return getSurface( lv * 6)->getBitsPerPixel(); }
	virtual UINT32		GetStride( INT32 lv = 0)				{ return getSurface( lv * 6)->getStride(); }
	virtual UINT32		GetPitch( INT32 lv = 0)					{ return getSurface( lv * 6)->getPitch(); }
	virtual UINT32		GetDataSize( INT32 lv = 0)				{ return getSurface( lv * 6)->GetDataSize(); }

	virtual		char *		Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual		void		Unlock(void);

	virtual		char *		Lock( I3G_CUBE_TEXTURE_FACE face, INT32 level, I3G_LOCKMETHOD lock = 0) = 0;
	virtual		void		Unlock( I3G_CUBE_TEXTURE_FACE face, INT32 level = 0) = 0;
};

#endif
