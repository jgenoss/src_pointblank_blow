#ifndef __I3_GFX_TEXTURE_DX_H
#define __I3_GFX_TEXTURE_DX_H

#include "i3Texture.h"

class i3RenderContext;

class I3_EXPORT_GFX i3TextureDX : public i3Texture
{
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3TextureDX, i3Texture, i3::cms_concrete_d3d9::value);

protected:
	// IDirect3DTexture9 Object를 사용하여, DirectX가 직접적으로 접근할 수 있는 Object로
	// 관리한다. 그러나 Texture Object를 Video Memory를 사용하지 않고 System Memory에 상주 시킨다.
	// 일면 메모리를 낭비하게 되는 면이 없지 않지만, Device Lost에 적절히 대응할 수 있으며,
	// 언제나 Lock 할 수 있다는 장점이 있다.
	// 또한 능동적으로 Texture Swapping을 구현하기 위해서도 필요하다.
	IDirect3DTexture9 *			m_pTexture = nullptr;
	D3DLOCKED_RECT				m_LockRect;
	
	UINT32						m_LockCount[MAX_MIPMAP_LEVEL] = { 0 };

public:
	i3TextureDX(void);
	virtual ~i3TextureDX(void);

	void			SetDXTexture( IDirect3DTexture9 * pTexture);
 	IDirect3DTexture9 *			GetDXImage(void)			{ return m_pTexture; }

	virtual bool	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage) override;
	virtual bool	Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage, D3DPOOL ppool);

	virtual bool	Create( const char * pszPath, UINT32 Levels = 0) override;

	virtual bool	Destroy( bool bLostDevice) override;
	virtual bool	isDestroyed( void) override;
	virtual bool	Revive( i3RenderContext * pCtx) override;

	virtual char *	Lock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) override;
	virtual void	Unlock( INT32 level) override;

	virtual	char *	SafeLock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) override;
	virtual	bool	SafeUnlock( INT32 level) override;

#if defined( I3_DEBUG)
	virtual	void		OnQueryStat( I3G_RES_STAT * pInfo) override;
	virtual void		Dump( const char * pszPath) override;
#endif

	///			thread safe function

public:
	UINT32		CreateDXBuffer( UINT32 width, UINT32 height, UINT32 Levels, DWORD rUsage, D3DFORMAT rFormat, D3DPOOL pool);
	UINT32		LockDXBuffer( UINT32 level, D3DLOCKED_RECT * pLockRect, RECT * pRect, I3G_LOCKMETHOD lock);
	UINT32		UnlockDXBuffer( UINT32 level);

	UINT32		CreateDXSurface( UINT32 width, UINT32 height, D3DFORMAT rFormat);
	UINT32		GetDXSurface( UINT32 level);

	UINT32		GetReqResult( UINT32 req);

	static void UpdateForThread( void);
	static void	LostDeviceForThread( void);
};

#endif //__I3_GFX_TEXTURE_DX_H