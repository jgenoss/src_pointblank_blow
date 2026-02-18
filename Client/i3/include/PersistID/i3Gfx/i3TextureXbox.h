#ifndef __I3_GFX_TEXTURE_XBOX_H
#define __I3_GFX_TEXTURE_XBOX_H

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for DirectX and Windows."
#endif

#include "i3Texture.h"

class i3TextureXbox : public i3Texture
{
	I3_CLASS_DEFINE( i3TextureXbox);
protected:
	// IDirect3DTexture8 Object를 사용하여, DirectX가 직접적으로 접근할 수 있는 Object로
	// 관리한다. 그러나 Texture Object를 Video Memory를 사용하지 않고 System Memory에 상주 시킨다.
	// 일면 메모리를 낭비하게 되는 면이 없지 않지만, Device Lost에 적절히 대응할 수 있으며,
	// 언제나 Lock 할 수 있다는 장점이 있다.
	// 또한 능동적으로 Texture Swapping을 구현하기 위해서도 필요하다.
	IDirect3DTexture8 *			m_pTexture;
	D3DLOCKED_RECT				m_LockRect;
	INT32						m_LockedCount;

	void		SetPlatformTexture( IDirect3DTexture8 * pTexture);

public:
	i3TextureXbox(void);
	virtual ~i3TextureXbox(void);

	IDirect3DTexture8 *			GetPlatformTexture(void)			{ return m_pTexture; }

	virtual BOOL	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_USAGE Usage, I3G_IMAGE_FORMAT format);
	virtual BOOL	Create( const char * pszPath, UINT32 Levels = 0);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock( INT32 level = 0);

	virtual void	MakeRuntimeForm(void);
	//virtual void	MakePersistForm(void);
};

#endif //__I3_GFX_TEXTURE_XBOX_H