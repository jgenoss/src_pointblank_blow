#if !defined( __I3_GFX_TEXTURE_H)
#define __I3_GFX_TEXTURE_H

#include "i3GfxResource.h"
#include "i3Surface.h"
#include "i3Clut.h"

#define MAX_TEXTURE_WIDTH		4096
#define MAX_TEXTURE_HEIGHT		4096
#define MAX_MIPMAP_LEVEL		12

class i3Texture;
typedef void	(*I3G_PIXEL_PROC)( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol);

#define		I3I_TEXFLAG_CUBE			0x00000001
#define		I3I_TEXFLAG_VOLUME			0x00000002

#define		I3I_TEXFLAG_NORMAL			0x10000000
#define		I3I_TEXFLAG_SPECULAR		0x20000000
#define		I3I_TEXFLAG_ENVIRONMENT		0x40000000
#define		I3I_TEXFLAG_REFLECT			0x80000000
#define		I3I_TEXFLAG_REFLECT_MASK	0x01000000
#define		I3I_TEXFLAG_LUX				0x02000000

class I3_EXPORT_GFX i3Texture : public i3GfxResource
{
	I3_ABSTRACT_CLASS_DEFINE( i3Texture);

protected:
	// Persist members
	UINT32				m_Width;
	UINT32				m_Height;
	I3G_IMAGE_FORMAT	m_PersistFormat;
	UINT32				m_LevelCount;

	// Volatile members
	I3G_IMAGE_FORMAT	m_Format;
	char *				m_pLockedBuf;
	INT32				m_LockedLevel;
	I3G_LOCKMETHOD		m_LockMethod;

	i3List				m_SurfaceList;

	I3G_PIXEL_PROC		m_pGetPixelProc;
	I3G_PIXEL_PROC		m_pSetPixelProc;

	UINT32				m_TexFlag;				// 사용되는 Texture의 종류(예:Normal map, Specular map, Enviroment map etc..)
	

protected:
	void				RemoveAllSurface(void);
	void				_setFormat( I3G_IMAGE_FORMAT fmt);

public:
	i3Texture(void);
	virtual ~i3Texture(void);

	virtual BOOL		Create( UINT32 widht, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);
	virtual BOOL		Create( const char * pszPath, UINT32 Levels = 1 );

	// UsageFlag
	void				setTextureFlag( UINT32 flag)				{ m_TexFlag = flag;	}
	UINT32				getTextureFlag(void)						{ return m_TexFlag;	}
	void				addTextureFlag( UINT32 mask)				{ m_TexFlag |= mask; }
	void				removeTextureFlag( UINT32 mask)				{ m_TexFlag &= ~mask; }

	i3Surface *			getSurface( INT32 idx)					{ return (i3Surface *) m_SurfaceList.Items[idx]; }
	INT32				getSurfaceCount(void)					{ return m_SurfaceList.GetCount(); }

	UINT32				GetWidth(void)							{ return m_Width; }
	UINT32				GetHeight(void)							{ return m_Height; }
	UINT32				GetLevelCount(void)						{ return m_LevelCount; }

	virtual UINT32		GetBitsPerPixel( INT32 lv = 0)			{ return getSurface( lv)->getBitsPerPixel(); }
	virtual UINT32		GetStride( INT32 lv = 0)				{ return getSurface( lv)->getStride(); }
	virtual UINT32		GetPitch( INT32 lv = 0)					{ return getSurface( lv)->getPitch(); }
	virtual UINT32		GetDataSize( INT32 lv = 0)				{ return getSurface( lv)->GetDataSize(); }

	char *				GetLockedBuffer(void)					{ return m_pLockedBuf; }
	INT32				getLockedLevel(void)					{ return m_LockedLevel; }

	bool				isLocked(void)							{ return (m_LockedLevel != -1); }

	I3G_IMAGE_FORMAT	GetFormat(void)							{ return m_Format; }
	void				SetFormat( I3G_IMAGE_FORMAT format)		{ m_Format = format; }

	I3G_IMAGE_FORMAT	GetPersistFormat(void)					{ return m_PersistFormat; }
	void				SetPersistFormat( I3G_IMAGE_FORMAT format)
	{
		m_PersistFormat = format;
	}

	void				GetPixel( INT32 x, INT32 y, COLORREAL * pCol)
	{
		m_pGetPixelProc( this, x, y, pCol);
	}

	void				SetPixel( INT32 x, INT32 y, COLORREAL * pCol)
	{
		m_pSetPixelProc( this, x, y, pCol);
	}

	i3Clut *			GetClut( INT32 level)					{ return getSurface( level)->getClut(); }
	void				SetClut( INT32 level, i3Clut * pClut);

	INT8 *				GetPersistData( INT32 level)			{ return getSurface( level)->getPersistData(); }
	virtual		void	SetPersistData( INT32 level, INT8 * pData);

	BOOL				IsIndexed(void)							{ return GetClut( 0) != NULL; }

	virtual		void		MakeRuntimeForm(void);
	void					MakePersistForm(void);

	virtual		char *		Lock( INT32 level, I3G_LOCKMETHOD lock = 0) = 0;
	virtual		void		Unlock(void) = 0;

	virtual		UINT32		OnSave( i3ResourceFile * pResFile);
	virtual		UINT32		OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual char *				_getInfoString(void);
	#endif
};

extern INT32		g_GlobalTextureCount;

#define		I3G_TEXTURE_MODIFY_ENABLE			0x00000001
#define		I3G_TEXTURE_MODIFY_TEXTURE			0x00000002
#define		I3G_TEXTURE_MODIFY_WRAP				0x00000004
#define		I3G_TEXTURE_MODIFY_FILTER			0x00000008
#define		I3G_TEXTURE_MODIFY_FUNCTION			0x00000010
#define		I3G_TEXTURE_MODIFY_MATRIX			0x00000020
#define		I3G_TEXTURE_MODIFY_BORDER_COLOR		0x00000040

#define		I3G_TEXTURE_MODIFY_ALL				(I3G_TEXTURE_MODIFY_ENABLE | I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER | \
												 I3G_TEXTURE_MODIFY_BORDER_COLOR )

typedef struct _tagi3GfxTextureBindContext
{
	BOOL					m_bEnable;
	UINT32					m_ModifyFlag;
	i3Texture *				m_pTexture;
	I3G_TEXTURE_WRAP		m_HWrap,	m_VWrap;
	I3G_TEXTURE_FILTER		m_MinFilter,	m_MagFilter;
	I3G_TEXTURE_FUNCTION	m_Function;
	MATRIX					m_Matrix;
	VEC3D					m_Scale;
	VEC3D					m_Offset;
	INT32					m_CoordCount;
	BOOL					m_bProjective;
	I3G_TEXTURE_BIND		m_BindType;
	UINT32					m_BorderColor;
} I3G_TEXTURE_BIND_CONTEXT;

#endif
