#if !defined( __I3G_SURFACE_H)
#define __I3G_SURFACE_H

#include "i3Clut.h"

class i3Surface : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Surface);
protected:
	UINT32				m_Usage;
	UINT32				m_Width;
	UINT32				m_Height;
	I3G_IMAGE_FORMAT	m_PersistFormat;


	I3G_IMAGE_FORMAT	m_Format;
	UINT32				m_BitsPerPixel;
	UINT32				m_Stride;
	UINT32				m_Pitch;

	i3Clut *			m_pClut;
	INT8 *				m_pPersistData;

	char *				m_pLockedBuf;

#if defined( I3G_DX)
	IDirect3DSurface9 *		m_pSurface;
#endif

	friend class i3Texture;

public:
	i3Surface(void);
	virtual ~i3Surface(void);

	UINT32				getUsage(void)							{ return m_Usage; }
	void				setUsage( UINT32 usage)					{ m_Usage = usage; }

	UINT32				getWidth(void)							{ return m_Width; }
	void				setWidth( UINT32 w)						{ m_Width = w; }

	UINT32				getHeight(void)							{ return m_Height; }
	void				setHeight( UINT32 h)					{ m_Height = h; }

	UINT32				getBitsPerPixel(void)					{ return m_BitsPerPixel; }
	void				setBitsPerPixel(UINT32 Bits)			{ m_BitsPerPixel = Bits;}
	
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

	virtual void		Lock( I3G_LOCKMETHOD lock = 0);
	virtual void		Unlock(void);
	char *				getLockedBuffer(void)							{ return m_pLockedBuf; }

	void				Destroy(void);
	void				Revive(void);
	virtual void		MakeRuntimeForm( bool bMakeLinear);
	
	virtual	void		MakePersistForm(void);

	void				DeletePersistData(void);

#if defined( I3G_DX)
	IDirect3DSurface9 *		getSurface(void)		{ return m_pSurface; }
	void					SetSurface( IDirect3DSurface9 * pObj, I3G_IMAGE_FORMAT persistFormat, UINT32 usage);

	void					_forceSetPersistData( INT8 * pData)			{ m_pPersistData = pData; }
#endif
};

#endif

