#ifndef __I3_GFX_TEXTURE_DX_H
#define __I3_GFX_TEXTURE_DX_H

#include "i3Texture.h"

class i3RenderContext;

class I3_EXPORT_GFX i3TextureDX : public i3Texture
{
	I3_CLASS_DEFINE( i3TextureDX);
protected:
	// IDirect3DTexture9 Object를 사용하여, DirectX가 직접적으로 접근할 수 있는 Object로
	// 관리한다. 그러나 Texture Object를 Video Memory를 사용하지 않고 System Memory에 상주 시킨다.
	// 일면 메모리를 낭비하게 되는 면이 없지 않지만, Device Lost에 적절히 대응할 수 있으며,
	// 언제나 Lock 할 수 있다는 장점이 있다.
	// 또한 능동적으로 Texture Swapping을 구현하기 위해서도 필요하다.
	IDirect3DTexture9 *			m_pTexture;
	D3DLOCKED_RECT				m_LockRect;
	INT32						m_LockedCount;

public:
	i3TextureDX(void);
	virtual ~i3TextureDX(void);

	void			SetDXTexture( IDirect3DTexture9 * pTexture);
	IDirect3DTexture9 *			GetDXImage(void)			{ return m_pTexture; }

	virtual BOOL	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);
	virtual BOOL	Create( const char * pszPath, UINT32 Levels = 0);

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	isDestroyed( void);
	virtual BOOL	Revive( i3RenderContext * pCtx);

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0);
	virtual void	Unlock(void);
};

#endif //__I3_GFX_TEXTURE_DX_H