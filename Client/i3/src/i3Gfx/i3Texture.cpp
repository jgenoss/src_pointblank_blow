#include "i3GfxType.h"
#include "i3Texture.h"
#include "i3ImageFileBMP.h"
#include "i3ImageFile.h"
#include "i3ImageUtil.h"
#include "i3GfxUtil.h"
#include "i3GfxGlobalVariable.h"

I3_CLASS_INSTANCE( i3Texture);

#if defined( I3_DEBUG)
INT32		g_GlobalTextureCount = 0;
#endif

i3Texture::i3Texture(void)
{
	m_ResType = I3G_RESOURCE_TYPE_TEXTURE;
		
#if defined( I3_DEBUG)
	g_GlobalTextureCount++;
#endif

	m_SurfaceList.reserve(4);
}

i3Texture::~i3Texture(void)
{
	RemoveAllSurface();

	I3_SAFE_RELEASE( m_pFullTexture);

#if defined( I3_DEBUG)
	g_GlobalTextureCount--;
#endif
}

void i3Texture::RemoveAllSurface(void)
{
	INT32 i;
	i3Surface * pSurface;

	for( i = 0 ; i < getSurfaceCount(); i++)
	{
		pSurface = getSurface( i);

		I3_MUST_RELEASE(pSurface);
	}

	m_SurfaceList.clear();
}

void i3Texture::SetClut( INT32 level, i3Clut * pClut)
{
	I3ASSERT( getSurfaceCount() > level);

	i3Surface * pSurface = getSurface( level);

	pSurface->SetClut( pClut);
}

void i3Texture::_setFormat( I3G_IMAGE_FORMAT fmt)
{
	m_Format = fmt;

	switch( fmt)
	{
		case I3G_IMAGE_FORMAT_I4 :	
			m_pGetPixelProc = i3PixelGet_4;
			m_pSetPixelProc = i3PixelSet_4;
			break;

		case I3G_IMAGE_FORMAT_I8 :
			m_pGetPixelProc = i3PixelGet_8;
			m_pSetPixelProc = i3PixelSet_8;
			break;

		case I3G_IMAGE_FORMAT_RGB_565 :
			m_pGetPixelProc = i3PixelGet_RGB_565;
			m_pSetPixelProc = i3PixelSet_RGB_565;
			break;

		case I3G_IMAGE_FORMAT_XRGB_1555 :
			m_pGetPixelProc = i3PixelGet_XRGB_1555;
			m_pSetPixelProc = i3PixelSet_XRGB_1555;
			break;

		case I3G_IMAGE_FORMAT_ARGB_1555 :
			m_pGetPixelProc = i3PixelGet_ARGB_1555;
			m_pSetPixelProc = i3PixelSet_ARGB_1555;
			break;

		case I3G_IMAGE_FORMAT_XBGR_1555 :
			m_pGetPixelProc = i3PixelGet_XRGB_1555;
			m_pSetPixelProc = i3PixelSet_XRGB_1555;
			break;

		case I3G_IMAGE_FORMAT_ABGR_1555 :
			m_pGetPixelProc = i3PixelGet_ARGB_1555;
			m_pSetPixelProc = i3PixelSet_ARGB_1555;
			break;

		case I3G_IMAGE_FORMAT_XRGB_4444 :
			m_pGetPixelProc = i3PixelGet_ABGR_4444;
			m_pSetPixelProc = i3PixelSet_ABGR_4444;
			break;

		case I3G_IMAGE_FORMAT_ARGB_4444 :
			m_pGetPixelProc = i3PixelGet_ABGR_4444;
			m_pSetPixelProc = i3PixelSet_ABGR_4444;
			break;

		case I3G_IMAGE_FORMAT_XBGR_4444 :
			m_pGetPixelProc = i3PixelGet_ABGR_4444;
			m_pSetPixelProc = i3PixelSet_ABGR_4444;
			break;

		case I3G_IMAGE_FORMAT_ABGR_4444 :
			m_pGetPixelProc = i3PixelGet_ABGR_4444;
			m_pSetPixelProc = i3PixelSet_ABGR_4444;
			break;

		case I3G_IMAGE_FORMAT_RGB_888 :
			m_pGetPixelProc = i3PixelGet_RGB_888;
			m_pSetPixelProc = i3PixelSet_RGB_888;
			break;

		case I3G_IMAGE_FORMAT_BGR_888 :
			m_pGetPixelProc = i3PixelGet_RGB_888;
			m_pSetPixelProc = i3PixelSet_RGB_888;
			break;

		case I3G_IMAGE_FORMAT_RGBX_8888 :
			m_pGetPixelProc = i3PixelGet_XBGR_8888;
			m_pSetPixelProc = i3PixelSet_XBGR_8888;
			break;

		case I3G_IMAGE_FORMAT_BGRX_8888 :
			m_pGetPixelProc = i3PixelGet_XRGB_8888;
			m_pSetPixelProc = i3PixelSet_XRGB_8888;
			break;

		case I3G_IMAGE_FORMAT_RGBA_8888 :
			m_pGetPixelProc = i3PixelGet_ABGR_8888;
			m_pSetPixelProc = i3PixelSet_ABGR_8888;
			break;

		case I3G_IMAGE_FORMAT_BGRA_8888 :
			m_pGetPixelProc = i3PixelGet_ARGB_8888;
			m_pSetPixelProc = i3PixelSet_ARGB_8888;
			break;
		case I3G_IMAGE_FORMAT_DXT1_X:
		case I3G_IMAGE_FORMAT_DXT1:
			m_pGetPixelProc = i3PixelGet_DXT1;
			m_pSetPixelProc = i3PixelSet_DXT1;
			break;
		case I3G_IMAGE_FORMAT_DXT2:
			m_pGetPixelProc = i3PixelGet_DXT2;
			m_pSetPixelProc = i3PixelSet_DXT2;
			break;
		case I3G_IMAGE_FORMAT_DXT3:
			m_pGetPixelProc = i3PixelGet_DXT3;
			m_pSetPixelProc = i3PixelSet_DXT3;
			break;
		case I3G_IMAGE_FORMAT_DXT4:
			m_pGetPixelProc = i3PixelGet_DXT4;
			m_pSetPixelProc = i3PixelSet_DXT4;
			break;
		case I3G_IMAGE_FORMAT_DXT5:
			m_pGetPixelProc = i3PixelGet_DXT5;
			m_pSetPixelProc = i3PixelSet_DXT5;
			break;


		case I3G_IMAGE_FORMAT_R_32F:
			m_pGetPixelProc = i3PixelGet_R_32F;
			m_pSetPixelProc = i3PixelSet_R_32F;
			break;

		case I3G_IMAGE_FORMAT_GR_32F:
			m_pGetPixelProc = i3PixelGet_GR_32F;
			m_pSetPixelProc = i3PixelSet_GR_32F;
			break;
	}
}

bool i3Texture::Create( UINT32 width, UINT32 height, UINT32 levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	I3ASSERT( (width > 0) && (width <= MAX_TEXTURE_WIDTH));
	I3ASSERT( (height > 0) && (height <= MAX_TEXTURE_HEIGHT));
	I3ASSERT( (levels > 0) && (levels <= MAX_MIPMAP_LEVEL));

	SetUsageFlag( Usage);

	if( levels <= 0)
		levels = 1;

	m_Width = static_cast<UINT16>(width);
	m_Height = static_cast<UINT16>(height);
	m_LevelCount = static_cast<UINT8>(levels);
	_setFormat( format);
	m_PersistFormat = format;

	return true;
}

bool i3Texture::Create( const char * pszPath, UINT32 levels)
{
	I3ASSERT( levels > 0);

	SetName( pszPath);

	m_LevelCount = static_cast<UINT8>(levels);

	return true;
}

void i3Texture::SetPersistData( INT32 level, INT8 * pData)
{
	I3ASSERT( getSurfaceCount() > level);

	i3Surface * pSurface = getSurface( level);

	pSurface->SetPersistData( pData);
}

void i3Texture::MakeRuntimeForm(void)
{
	UINT32 i;

	if( m_PersistFormat != m_Format)
	{
		// Runtime Format과 Persist Format은 다를 수 있다.
		// i3Texture는 기본적으로 항상 Persist Data를 보관한다.
		// Runtime Data는 Persist Data를 바탕으로 재생성한다.

		// 플랫폼이 다른 경우
		UINT32 PersistPlatformMask = m_PersistFormat & I3G_IMAGE_FORMAT_MASK_PLATFORM;
		UINT32 RuntimePlatformMask = m_Format & I3G_IMAGE_FORMAT_MASK_PLATFORM;
		bool bMakeLinear = false;
		I3_PLATFORM PersistPlatform = I3_PLATFORM_UNKNOWN;

		if( PersistPlatformMask != RuntimePlatformMask )
		{
			if( PersistPlatformMask == I3G_IMAGE_FORMAT_MASK_PSP )
			{
				PersistPlatform = I3_PLATFORM_PSP;
				if( RuntimePlatformMask == 0 || RuntimePlatformMask == I3G_IMAGE_FORMAT_MASK_PC )
				{
					bMakeLinear = true;
				}
			}
		}

		for( i = 0; i < (UINT32) getSurfaceCount(); i++)
		{
			i3Surface * pSurface = getSurface( i);

			pSurface->MakeRuntimeForm( bMakeLinear);
		}
	}

	if( g_pRenderContext != nullptr)
	{
		// Persist Data를 제거한다.
		if( g_pRenderContext->getGfxOption()->getKeepPersistDataEnable() == false)
		{
			for( i = 0; i < (UINT32) getSurfaceCount(); i++)
			{
				i3Surface * pSurface = getSurface( i);

				pSurface->DeletePersistData();
			}
		}
	}
}

void i3Texture::MakePersistForm(void)
{
	INT32 i = 0;

	for( i=0; i<getSurfaceCount(); i++)
	{
		i3Surface* pSurface = getSurface(i);

		pSurface->MakePersistForm();
	}

	SetPersistFormat( GetFormat());
}

UINT32 i3Texture::OnSave( i3ResourceFile * pResFile)
{
	i3ImageFile file;

	if( file.SaveI3I( pResFile->GetStream(), this) == false)
	{
		if( file.isSkipResource())
			return 0;
		else 
			return STREAM_ERR;
	}

	return file.GetReadSize();
}

UINT32 i3Texture::OnLoad( i3ResourceFile * pResFile)
{
	if( IsLoaded() )
	{
		I3PRINTLOG(I3LOG_WARN, "Multiple Load found!!!!!!!!!!!!!!!!!!!!");
		//I3TRACE( "WARNING : [%s] Multiple Load found!!!!!!!!!!!!!!!!!!!!\n",  __FUNCTION__  );
		return 0;
	}

	setThreadMode( pResFile->InThread());

	i3ImageFile file;

	if( file.LoadI3I( pResFile->GetStream(), this) == false)
	{
		return STREAM_ERR;
	}

	setThreadMode( false);

	return file.GetReadSize();
}

#if defined( I3_DEBUG)
void i3Texture::_getInfoString( char * pszStr, const INT32 nStrMax )
{
	INT32 i;
	UINT32 sz = 0;

	for( i = 0; i < getSurfaceCount(); i++)
	{
		i3Surface * pSurface = getSurface( i);

		sz += pSurface->GetDataSize();
	}

	i3::snprintf( pszStr, nStrMax, "%s [%dx%dx%d] %d bytes", GetNameString(), GetWidth(), GetHeight(), getSurfaceCount(), sz);
}

void i3Texture::Dump( const char * pszPath)
{
}
#endif

const char * i3Texture::GetFormatText(void)
{
	switch(m_Format)
	{
	case I3G_IMAGE_FORMAT_NONE:				return "I3G_IMAGE_FORMAT_NONE";			break;
	case I3G_IMAGE_FORMAT_DEFAULT_COLOR:	return "I3G_IMAGE_FORMAT_DEFAULT_COLOR";break;
	
	// 4~8 bit format
	case I3G_IMAGE_FORMAT_I4: 				return "I3G_IMAGE_FORMAT_I4";			break;
	case I3G_IMAGE_FORMAT_I8: 				return "I3G_IMAGE_FORMAT_I8";			break;
	case i3G_IMAGE_FORMAT_L8: 				return "i3G_IMAGE_FORMAT_L8";			break;
	case i3G_IMAGE_FORMAT_A8: 				return "i3G_IMAGE_FORMAT_A8";			break;
	
	// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565: 			return "I3G_IMAGE_FORMAT_RGB_565";		break;
	case I3G_IMAGE_FORMAT_BGR_565: 			return "I3G_IMAGE_FORMAT_BGR_565";		break;
	case I3G_IMAGE_FORMAT_XRGB_1555: 		return "I3G_IMAGE_FORMAT_XRGB_1555";	break;
	case I3G_IMAGE_FORMAT_ARGB_1555: 		return "I3G_IMAGE_FORMAT_ARGB_1555";	break;
	case I3G_IMAGE_FORMAT_XBGR_1555: 		return "I3G_IMAGE_FORMAT_XBGR_1555";	break;
	case I3G_IMAGE_FORMAT_ABGR_1555: 		return "I3G_IMAGE_FORMAT_ABGR_1555";	break;
	case I3G_IMAGE_FORMAT_XRGB_4444: 		return "I3G_IMAGE_FORMAT_XRGB_4444";	break;
	case I3G_IMAGE_FORMAT_ARGB_4444: 		return "I3G_IMAGE_FORMAT_ARGB_4444";	break;
	case I3G_IMAGE_FORMAT_XBGR_4444: 		return "I3G_IMAGE_FORMAT_XBGR_4444";	break;
	case I3G_IMAGE_FORMAT_ABGR_4444: 		return "I3G_IMAGE_FORMAT_ABGR_4444";	break;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888: 			return "I3G_IMAGE_FORMAT_RGB_888";		break;
	case I3G_IMAGE_FORMAT_BGR_888: 			return "I3G_IMAGE_FORMAT_BGR_888";		break;

	// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888: 		return "I3G_IMAGE_FORMAT_RGBX_8888";	break;
	case I3G_IMAGE_FORMAT_BGRX_8888: 		return "I3G_IMAGE_FORMAT_BGRX_8888";	break;
	case I3G_IMAGE_FORMAT_RGBA_8888: 		return "I3G_IMAGE_FORMAT_RGBA_8888";	break;
	case I3G_IMAGE_FORMAT_BGRA_8888: 		return "I3G_IMAGE_FORMAT_BGRA_8888";	break;

	// Bump-map
	case I3G_IMAGE_FORMAT_VU_88: 			return "I3G_IMAGE_FORMAT_VU_88";		break;
	case I3G_IMAGE_FORMAT_QWVU_8888: 		return "I3G_IMAGE_FORMAT_QWVU_8888";	break;

	// Compressed formats
	case I3G_IMAGE_FORMAT_DXT1_X: 			return "I3G_IMAGE_FORMAT_DXT1_X";		break;
	case I3G_IMAGE_FORMAT_DXT1: 			return "I3G_IMAGE_FORMAT_DXT1";			break;
	case I3G_IMAGE_FORMAT_DXT2: 			return "I3G_IMAGE_FORMAT_DXT2";			break;
	case I3G_IMAGE_FORMAT_DXT3: 			return "I3G_IMAGE_FORMAT_DXT3";			break;
	case I3G_IMAGE_FORMAT_DXT4: 			return "I3G_IMAGE_FORMAT_DXT4";			break;
	case I3G_IMAGE_FORMAT_DXT5: 			return "I3G_IMAGE_FORMAT_DXT5";			break;

	// HDR
	case I3G_IMAGE_FORMAT_ABGR_16F: 		return "I3G_IMAGE_FORMAT_ABGR_16F";		break;
	case I3G_IMAGE_FORMAT_ABGR_32F: 		return "I3G_IMAGE_FORMAT_ABGR_32F";		break;
	case I3G_IMAGE_FORMAT_ABGR_16: 			return "I3G_IMAGE_FORMAT_ABGR_16";		break;
	case I3G_IMAGE_FORMAT_GR_16F: 			return "I3G_IMAGE_FORMAT_GR_16F";		break;
	case I3G_IMAGE_FORMAT_R_32F: 			return "I3G_IMAGE_FORMAT_R_32F";		break;
	case I3G_IMAGE_FORMAT_GR_32F: 			return "I3G_IMAGE_FORMAT_GR_32F";		break;

	// depth, stencil, vertex buffers
	case I3G_IMAGE_FORMAT_D16: 				return "I3G_IMAGE_FORMAT_D16";			break;
	case I3G_IMAGE_FORMAT_D24S8: 			return "I3G_IMAGE_FORMAT_D24S8";		break;
	case I3G_IMAGE_FORMAT_D24X8: 			return "I3G_IMAGE_FORMAT_D24X8";		break;
	case I3G_IMAGE_FORMAT_D32: 				return "I3G_IMAGE_FORMAT_D32";			break;
	case I3G_IMAGE_FORMAT_D32F: 			return "I3G_IMAGE_FORMAT_D32F";			break;
	case I3G_IMAGE_FORMAT_D24FS8: 			return "I3G_IMAGE_FORMAT_D24FS8";		break;					

	default:								return "Format: UnKnown";				break;					
	}
}

