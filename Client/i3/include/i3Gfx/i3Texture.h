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
#define		I3I_TEXFLAG_EMISSIVE		0x04000000
#define		I3I_TEXFLAG_MASK_USAGE		0xFF000000

class I3_EXPORT_GFX i3Texture : public i3GfxResource
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3Texture, i3GfxResource);

protected:
	// Persist members
	UINT16				m_Width = 0;
	UINT16				m_Height = 0;
	I3G_IMAGE_FORMAT	m_PersistFormat = I3G_IMAGE_FORMAT_NONE;
	I3G_MIPMAP			m_MipmapGen = I3G_MIPMAP_LOAD;
	I3G_TEXTURE_FILTER	m_MipmapGenFilter = I3G_TEXTURE_FILTER_LINEAR;
	UINT8				m_LevelCount = 1;

	// Volatile members
	I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT_NONE;
	I3G_LOCKMETHOD		m_LockMethod = 0;
	char *				m_pLockedBuf[MAX_MIPMAP_LEVEL] = { nullptr, };
	INT8				m_LockedLevel = -1;

	i3::vector<i3Surface*>	m_SurfaceList;

	I3G_PIXEL_PROC		m_pGetPixelProc = nullptr;
	I3G_PIXEL_PROC		m_pSetPixelProc = nullptr;

	UINT32				m_TexFlag = 0;				// 사용되는 Texture의 종류(예:Normal map, Specular map, Enviroment map etc..)

	i3Mutex				m_Mutex;
	

protected:
	void				RemoveAllSurface(void);
	void				_setFormat( I3G_IMAGE_FORMAT fmt);

public:
	i3Texture(void);
	virtual ~i3Texture(void);

	virtual bool		Create( UINT32 widht, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage);
	virtual bool		Create( const char * pszPath, UINT32 Levels = 1 );

	// UsageFlag
	void				setTextureFlag( UINT32 flag)				{ m_TexFlag = flag;	}
	UINT32				getTextureFlag(void)						{ return m_TexFlag;	}
	void				addTextureFlag( UINT32 mask)				{ m_TexFlag |= mask; }
	void				removeTextureFlag( UINT32 mask)				{ m_TexFlag &= ~mask; }

	i3Surface *			getSurface( INT32 idx)					{ return m_SurfaceList[idx]; }
	INT32				getSurfaceCount(void)					{ return (INT32)m_SurfaceList.size(); }

	UINT32				GetWidth(void)							{ return m_Width; }
	UINT32				GetHeight(void)							{ return m_Height; }
	UINT32				GetLevelCount(void)						{ return m_LevelCount; }

	I3G_MIPMAP			getMipmapGenType(void)					{ return m_MipmapGen; }
	void				setMipmapGenType( I3G_MIPMAP type)		{ m_MipmapGen = type; }

	I3G_TEXTURE_FILTER	getMipmapGenFilter(void)				{ return m_MipmapGenFilter; }
	void				setMipmapGenFilter( I3G_TEXTURE_FILTER filter)
	{
		m_MipmapGenFilter = filter;
	}

	virtual UINT32		GetBitsPerPixel( INT32 lv = 0)			{ return getSurface( lv)->getBitsPerPixel(); }
	virtual UINT32		GetStride( INT32 lv = 0)				{ return getSurface( lv)->getStride(); }
	virtual UINT32		GetPitch( INT32 lv = 0)					{ return getSurface( lv)->getPitch(); }
	virtual UINT32		GetDataSize( INT32 lv = 0)				{ return getSurface( lv)->GetDataSize(); }

	char *				GetLockedBuffer( INT32 level) const		
	{ 
		if (level < 0 || level >= MAX_MIPMAP_LEVEL)		// 메인 엔진 인터페이스이지만, 뭔가 좀 이상하다.
		{												// 원래대로라면 MAX_MIPMAP_LEVEL이 문제가 아니고 선언배열을 12개나 잡은게 문제이다..
			I3_BOUNDCHK(level, MAX_MIPMAP_LEVEL);		// 일단 커버리티의 드러눕기 진상 처리를 위해 수정함. (2016.07.11.수빈)
			return nullptr;
		}
		return m_pLockedBuf[ level]; 
	}

	// 마지막으로 lock한 level
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

	inline I3G_PIXEL_PROC GetProcGetPixel() { return m_pGetPixelProc; }
	inline I3G_PIXEL_PROC GetProcSetPixel() { return m_pSetPixelProc; }

	i3Clut *			GetClut( INT32 level)					{ return getSurface( level)->getClut(); }
	void				SetClut( INT32 level, i3Clut * pClut);

	INT8 *				GetPersistData( INT32 level)			{ return getSurface( level)->getPersistData(); }
	virtual		void	SetPersistData( INT32 level, INT8 * pData);

	bool				IsIndexed(void)							{ return GetClut( 0) != nullptr; }

	virtual		void		MakeRuntimeForm(void);
	void					MakePersistForm(void);

	virtual		char *		Lock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) = 0;
	virtual		void		Unlock( INT32 level) = 0;

	virtual		char *		SafeLock( INT32 level, I3G_LOCKMETHOD lock = 0, RECT * pRect = nullptr) = 0;
	virtual		bool		SafeUnlock( INT32 level) = 0;

	virtual		UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual		UINT32		OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void				_getInfoString( char * pszStr, const INT32 nStrMax ) override;
	virtual void				Dump( const char * pszPath);
	#endif

	const char * GetFormatText(void);

private:
	bool		m_bInThread = false;

	bool		m_bIsPreload = false;	// 프리로딩되는 텍스쳐. 로딩 완료되면 m_pFullTexture를 사용한다.
	i3Texture *	m_pFullTexture = nullptr;	// 원본 텍스쳐.

public:
	void		setThreadMode( bool bEnable)				{ m_bInThread = bEnable; }
	bool		getThreadMode( void)						{ return m_bInThread; }

	void		SetPreload( bool bVal)						{ m_bIsPreload = bVal; }
	bool		IsPreload( void)							{ return m_bIsPreload; }

	void		BindFullLoadedTexture( i3Texture * pTex)	{ I3_REF_CHANGE( m_pFullTexture, pTex); }
	i3Texture *	GetFullLoadedTexture( void)					{ return m_pFullTexture; }
};

#if defined( I3_DEBUG)
extern INT32		g_GlobalTextureCount;
#endif

#define		I3G_TEXTURE_MODIFY_ENABLE			0x00000001
#define		I3G_TEXTURE_MODIFY_TEXTURE			0x00000002
#define		I3G_TEXTURE_MODIFY_WRAP				0x00000004
#define		I3G_TEXTURE_MODIFY_FILTER			0x00000008
#define		I3G_TEXTURE_MODIFY_FUNCTION			0x00000010
#define		I3G_TEXTURE_MODIFY_MATRIX			0x00000020
#define		I3G_TEXTURE_MODIFY_BORDER_COLOR		0x00000040

#define		I3G_TEXTURE_MODIFY_ALL				(I3G_TEXTURE_MODIFY_ENABLE | I3G_TEXTURE_MODIFY_TEXTURE | I3G_TEXTURE_MODIFY_WRAP | I3G_TEXTURE_MODIFY_FILTER | \
												 I3G_TEXTURE_MODIFY_BORDER_COLOR )

struct I3G_TEXTURE_BIND_CONTEXT
{
	bool					m_bEnable = false;
	UINT32					m_ModifyFlag = 0xFFFFFFFF;
	i3Texture *				m_pTexture = nullptr;
	I3G_TEXTURE_WRAP		m_HWrap = I3G_TEXTURE_WRAP_CLAMP,	m_VWrap = I3G_TEXTURE_WRAP_CLAMP;
	I3G_TEXTURE_FILTER		m_MinFilter = I3G_TEXTURE_FILTER_LINEAR,	m_MagFilter = I3G_TEXTURE_FILTER_LINEAR;
	MATRIX					m_Matrix;
	VEC3D					m_Scale = { 1.0f, 1.0f, 1.0f };
	VEC3D					m_Offset;
	INT32					m_CoordCount = 0;
	bool					m_bProjective = false;
	I3G_TEXTURE_BIND		m_BindType = I3G_TEXTURE_BIND_DIFFUSE;
	UINT32					m_BorderColor = 0;
	bool					m_bEnableChangedByChangeTex = false;
	
	bool	isEnable(void) const		{ return m_bEnable;}
	void	setEnable( bool bState)
	{
		m_bEnable = bState;
		m_ModifyFlag |= I3G_TEXTURE_MODIFY_ENABLE;
	}

	const i3Texture *		getTexture(void) const		{ return m_pTexture; }
	void	setTexture( i3Texture * pTex)
	{
		m_pTexture = pTex;
		m_ModifyFlag |= I3G_TEXTURE_MODIFY_TEXTURE;
	}

	I3G_TEXTURE_WRAP	getHorzWrap(void) const			{ return m_HWrap; }
	I3G_TEXTURE_WRAP	getVertWrap(void) const			{ return m_VWrap; }
	void				setWrap( I3G_TEXTURE_WRAP wrap)
	{
		m_HWrap = wrap;
		m_VWrap = wrap;

		m_ModifyFlag |= I3G_TEXTURE_MODIFY_WRAP;
	}

	I3G_TEXTURE_FILTER	getMinFilter(void) const			{ return m_MinFilter; }
	I3G_TEXTURE_FILTER	getMagFilter(void) const			{ return m_MagFilter; }
	void				setFilter( I3G_TEXTURE_FILTER minFilter, I3G_TEXTURE_FILTER magFilter)
	{
		m_MinFilter = minFilter;
		m_MagFilter = magFilter;

		m_ModifyFlag |= I3G_TEXTURE_MODIFY_FILTER;
	}

	UINT32				getBorderColor(void) const			{ return m_BorderColor; }
	void				setBorderColor(UINT32 borderColor)
	{
		m_BorderColor = borderColor;
	}

};

#endif
