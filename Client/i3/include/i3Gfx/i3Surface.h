#if !defined( __I3G_SURFACE_H)
#define __I3G_SURFACE_H

#include "i3Clut.h"

class I3_EXPORT_GFX i3Surface : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Surface, i3ElementBase);

	friend class i3Texture;

protected:
	I3G_USAGE			m_Usage = 0;
	I3G_IMAGE_FORMAT	m_PersistFormat = I3G_IMAGE_FORMAT_NONE;
	UINT16				m_Width = 0;
	UINT16				m_Height = 0;

	I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT_NONE;
	UINT8				m_BitsPerPixel = 0;
	UINT32				m_Stride = 0;
	UINT32				m_Pitch = 0;

	i3Clut *			m_pClut = nullptr;
	INT8 *				m_pPersistData = nullptr;

	char *				m_pLockedBuf = nullptr;

	INT32				m_LockCount = 0;

#if defined( I3G_DX)
	IDirect3DSurface9 *		m_pSurface = nullptr;
#endif

	i3Mutex				m_Mutex;

public:
	i3Surface(void);
	virtual ~i3Surface(void);

	I3G_USAGE			getUsage(void)							{ return m_Usage; }
	void				setUsage( I3G_USAGE usage)				{ m_Usage = usage; }

	UINT16				getWidth(void)							{ return m_Width; }
	void				setWidth( UINT16 w)						{ m_Width = w; }

	UINT16				getHeight(void)							{ return m_Height; }
	void				setHeight( UINT16 h)					{ m_Height = h; }

	UINT8				getBitsPerPixel(void)					{ return m_BitsPerPixel; }
	void				setBitsPerPixel(UINT8 Bits)				{ m_BitsPerPixel = Bits;}
	
	UINT32				getStride(void)							{ return m_Stride; }
	void				setStride( UINT32 stride)				{ m_Stride = stride; }

	UINT32				getPitch(void)							{ return m_Pitch; }
	void				setPitch( UINT32 pitch)					{ m_Pitch = pitch; }

	void				SetClut( i3Clut * pClut);
	i3Clut *			getClut(void)							{ return m_pClut; }

	INT8 *				getPersistData(void)					{ return m_pPersistData; }
	void				SetPersistData( INT8 * pData);

	I3G_IMAGE_FORMAT	getFormat(void)									{ return m_Format; }
	void				setFormat( I3G_IMAGE_FORMAT format);

	INT32				GetDataSize(void);

	I3G_IMAGE_FORMAT	getPersistFormat(void)							{ return m_PersistFormat; }
	void				setPersistFormat( I3G_IMAGE_FORMAT format)		{ m_PersistFormat = format; }

	virtual bool		Lock( I3G_LOCKMETHOD lock = 0);
	virtual void		Unlock(void);

	bool				SafeLock( I3G_LOCKMETHOD lock = I3G_LOCK_NONE);
	void				SafeUnlock( void);

	char *				getLockedBuffer(void)							{ return m_pLockedBuf; }

	void				Destroy(void);
	void				Revive(void);
	virtual void		MakeRuntimeForm( bool bMakeLinear);
	
	virtual	void		MakePersistForm(void);

	void				DeletePersistData(void);

#if defined( I3G_DX)
	IDirect3DSurface9 *		getSurface(void)		{ return m_pSurface; }
	void					SetSurface( IDirect3DSurface9 * pObj, I3G_IMAGE_FORMAT persistFormat, UINT32 usage);
#if defined( I3_DEBUG)
	void					SetSurface( IDirect3DSurface9 * pObj, I3G_IMAGE_FORMAT persistFormat, UINT32 usage, const char * pszFile, INT32 line);
#endif

	void					_forceSetPersistData( INT8 * pData)			{ m_pPersistData = pData; }

	void				CopySurfaceTo( INT32 sx, INT32 sy, INT32 cx, INT32 cy, i3Surface * pDest, INT32 dx, INT32 dy);
#endif

	//			thread safe function
private:
	bool		m_bInThread = false;

public:
	void		setThreadMode( bool bEnable)				{ m_bInThread = bEnable; }

	UINT32		GetDescForThread( D3DSURFACE_DESC * pDesc);
	UINT32		UpdateSurfaceForThread( RECT * pSrcRect, i3Surface * pDest, POINT * pDestP);
	UINT32		LockSurfaceForThread( D3DLOCKED_RECT * pInfo, I3G_LOCKMETHOD lock);
	UINT32		UnlockSurfaceForThread( void);

	UINT32		GetReqResult( UINT32 req);

	static void	UpdateForThread( void);
	static void	LostDeviceForThread( void);
	
};

#endif

