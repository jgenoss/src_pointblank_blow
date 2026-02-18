#if !defined( __I3_RENDER_TARGET_H)
#define __I3_RENDER_TARGET_H

#include "../i3Base/i3CommonType.h"
#include "i3GfxResource.h"
#include "i3GfxDefine.h"

class I3_EXPORT_GFX i3RenderTarget : public i3GfxResource
{
	I3_CLASS_DEFINE( i3RenderTarget);

protected:
	UINT32						m_Width;
	UINT32						m_Height;
	I3G_IMAGE_FORMAT			m_Format;
	I3G_IMAGE_FORMAT			m_DepthFormat;
	BOOL						m_bLockable;
	BOOL						m_bLocked;
	UINT32						m_Pitch;

#if defined( I3G_DX) || defined( I3G_XBOX)
	IDirect3DSurface9 *			m_pColorSurface;
	IDirect3DSurface9 *			m_pDepthSurface;

	I3G_RESOURCE_TYPE GetSurfaceType(void);
#elif defined( I3G_PSP)
	void *						m_DrawBP;
	void *						m_DispBP;
	void *						m_DepthBP;
	INT32						m_OffsetX;
	INT32						m_OffsetY;
	UINT32						m_DepthMin;
	UINT32						m_DepthMax;

public:
	void						SetOffset( INT32 x, INT32 y);
	void						SetDepthRange( UINT32 Min, UINT32 Max);

	UINT32						GetDepthMin(void)						{ return m_DepthMin; }
	UINT32						GetDepthMax(void)						{ return m_DepthMax; }
#elif defined( I3G_OGLES)
	REAL32						m_DepthMin;
	REAL32						m_DepthMax;

	void						SetDepthRange( REAL32 Min, REAL32 Max);

	REAL32						GetDepthMin(void)						{ return m_DepthMin; }
	REAL32						GetDepthMax(void)						{ return m_DepthMax; }
#endif

public:
	i3RenderTarget(void);
	virtual ~i3RenderTarget(void);

	BOOL Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, BOOL bLockable = FALSE);

#if defined( I3G_DX)
	BOOL Create( IDirect3DSurface9 * pColorSurface, IDirect3DSurface9 * pDepthSurface);

	IDirect3DSurface9 *GetColorSurface(void)		{ return m_pColorSurface; }
	IDirect3DSurface9 *GetDepthSurface(void)		{ return m_pDepthSurface; }
#endif

	I3G_IMAGE_FORMAT		GetFormat(void)				{	return m_Format; }
	I3G_IMAGE_FORMAT		GetDepthFormat(void)		{ return m_DepthFormat; }

	UINT32	GetWidth(void)				{	return m_Width; }
	UINT32	GetHeight(void)				{	return m_Height; }
	BOOL	IsLockable(void)			{	return m_bLockable; }
	BOOL	IsLocked(void)				{	return m_bLocked; }

	virtual void *	Lock( I3G_LOCKMETHOD lockMethod);
	virtual void	Unlock(void);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	BOOL Dump( const char *szFileName );
	BOOL Dump( char *pBuf, INT32 Pitch );

#ifdef I3_PSP
	void *	GetDrawBP(){ return m_DrawBP; }
	void *	GetDispBP(){ return	m_DispBP; }
	void *	GetDepthBP(){ return m_DepthBP; }
#endif
};

#endif
