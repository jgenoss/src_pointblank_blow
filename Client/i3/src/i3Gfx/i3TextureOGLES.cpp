#include "i3GfxType.h"
#include "i3GfxUtil.h"
#include "i3ImageFile.h"

#if defined( I3G_OGLES)
#include "i3ImageUtil.h"
#include "i3GfxOGLES.h"
#include "i3TextureOGLES.h"
#include "i3GfxUtilOGLES.h"
#endif

I3_CONCRETE_CLASS_INSTANCE( i3TextureOGLES, i3Texture);

i3TextureOGLES::i3TextureOGLES(void)
{
	memset( m_pBuffer, 0, sizeof(m_pBuffer));

	m_LockedCount = 0;
	memset( m_BufferWidth, 0, sizeof( m_BufferWidth));

	m_Handle = INVALID_TEX_HANDLE;
	m_DataSize = 0;
}

i3TextureOGLES::~i3TextureOGLES(void)
{
	if( m_pBuffer[0] != NULL)
	{
		// MIPMAP Level이 1 이상이라 하더라도 모두 선형 메모리로 관리하기 때문에 Level 0 Image에 대해서만
		// 할당 해제하면 된다.		
		I3MEM_SAFE_FREE( m_pBuffer[0] );		 
	}

	if( m_Handle != INVALID_TEX_HANDLE)
	{
		OGL_CHECK( I3OGL::glDeleteTextures( 1, (const GLuint *) &m_Handle));
		m_Handle = INVALID_TEX_HANDLE;
	}
}

static UINT32 s_MemSize = 0;

BOOL i3TextureOGLES::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	UINT32 Sz, i, lnSz, bitscount;
	UINT32 BuffSz[8];
	UINT32 width2;
	UINT32 height2;

	I3ASSERT( width > 0);
	I3ASSERT( height > 0);
	I3ASSERT( Levels > 0);

	i3Texture::Create( width, height, Levels, format, Usage);

	// Find appropriate native image format
	i3OGLESUT_GetNativeImageFormat( format, &m_PixelFormat, &m_PixelType);

	// convert to I3 image format
	format = (I3G_IMAGE_FORMAT)(i3OGLESUT_GetI3ImageFormat( m_PixelFormat, m_PixelType) | (format & (I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB)));

	m_Format = format;

	bitscount = i3Gfx::GetImageFormatBitCount( format);

	#if defined( I3_DEBUG)
		if(width > 256 || height > 256)
		{
			I3TRACE("-----------------------------------------------\n");
			I3TRACE("Texture Size Error(Width:%d, Height%d)\n", width, height);
			I3TRACE("-----------------------------------------------\n");
		}
	#endif

	Sz = 0;

	for( i = 0; i < Levels; i++)
	{
		I3ASSERT( width > 0);
		I3ASSERT( height > 0);

		m_pBuffer[i] = (char *) Sz;

		// PSP의 Texture는 Width와 Start Address가 16 bytes align되어야 한다.
		lnSz = i3Gfx::CalcRasterBufferSize( width, 1, 4, format);

		m_BufferWidth[i] = ((lnSz << 3) + bitscount - 1) / bitscount;

		BuffSz[i] = (lnSz * height);
		Sz += BuffSz[i];

		width2 = width;
		height2 = height;

		width = width >> 1;
		height = height >> 1;
	}

	I3ASSERT( Sz > 0);

	m_DataSize = Sz;

	s_MemSize += m_DataSize;

	//I3TRACE( "TEXTURE MEM : (USE:%d) (Total:%d) (FREE:%d)  Format:%08x\n", m_DataSize, s_MemSize, sceKernelTotalFreeMemSize(), GetFormat());

	m_pBuffer[0] = (char *)i3MemAlloc( Sz);
	I3ASSERT( m_pBuffer[0] != NULL);

	// Base Pointer를 기준으로 각 MIPMAP Level의 Base Pointer를 설정한다.
	for( i = 1; i < Levels; i++)
	{
		m_pBuffer[i] = m_pBuffer[i - 1] + BuffSz[i - 1];
	}

#if defined( I3G_G10 )
	OGL_CHECK( I3OGL::glEnable( GL_TEXTURE_2D ));
#endif

	OGL_CHECK( I3OGL::glGenTextures( 1, (GLuint *) &m_Handle));
	OGL_CHECK( I3OGL::glBindTexture( GL_TEXTURE_2D, GetHandle()));

	OGL_CHECK( I3OGL::glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	OGL_CHECK( I3OGL::glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	i3Surface* pSurface = i3Surface::NewObject();
	m_SurfaceList.Add( pSurface);
	return TRUE;
}

BOOL i3TextureOGLES::Create( const char * pszPath, UINT32 /*Levels*/)
{
	i3ImageFile file;

	if( file.Load( pszPath, this) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

char * i3TextureOGLES::Lock( INT32 level, I3G_LOCKMETHOD lockmode)
{
	I3ASSERT( (level >= 0) && (level < 8));
	I3ASSERT( m_pBuffer[0] != NULL);

	m_LockedCount++;

	return m_pBuffer[ level];
}

void i3TextureOGLES::Unlock( INT32 level)
{	
	I3ASSERT( m_LockedCount > 0);

	m_LockedCount --;
	if( m_LockedCount == 0)
	{
		INT32 w, h;
		i3GfxCaps *pCaps = g_pRenderContext->GetCaps();
		I3ASSERT( pCaps != NULL);

		w = m_Width >> level;
		h = m_Height >> level;

		OGL_CHECK( I3OGL::glBindTexture( GL_TEXTURE_2D, GetHandle()));		

		OGL_CHECK( I3OGL::glPixelStorei( GL_UNPACK_ALIGNMENT, 1));
		OGL_CHECK( I3OGL::glPixelStorei( GL_PACK_ALIGNMENT, 1));

		if( IsIndexed() && pCaps->CanCompressTexFormat( m_PixelType))
		{	
			INT32 CompressClutSize;
			
			char * tempPBuffer;
			char * pBuffer, * pDest;
			CompressClutSize = i3OGLSUT_CalcCompressClutSize( m_PixelType);
			pBuffer = (char *)i3MemAlloc( CompressClutSize + m_DataSize);			
			pDest = (char *)i3MemAlloc( m_DataSize);			
			tempPBuffer = pBuffer;
			// 버퍼에 Clut도 넣어준다.
			memcpy( pBuffer, GetClut(level)->GetData(), CompressClutSize);
			pBuffer += CompressClutSize;
			// 4bit모드에서 픽셀의 위치를 재계산해야된다. by dfly79
			i3Image_Convert( m_PersistFormat, m_pBuffer[level], w, h, 1, GetClut(level), 
				m_Format, pDest, 4, GetClut(level));
			memcpy( pBuffer, pDest, m_DataSize);

			OGL_CHECK( I3OGL::glCompressedTexImage2D( GL_TEXTURE_2D, level, m_PixelType, w, h, 0, CompressClutSize + m_DataSize, tempPBuffer));

			I3MEM_SAFE_FREE( pBuffer);
			I3MEM_SAFE_FREE( pDest);
		}
		else
		{
			OGL_CHECK( I3OGL::glTexImage2D( GL_TEXTURE_2D, level, m_PixelFormat, w, h, 0, m_PixelFormat, m_PixelType, m_pBuffer[level]));
		}

		//OGL_CHECK( I3OGL::glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		//OGL_CHECK( I3OGL::glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR));

		//OGL_CHECK( I3OGL::glBindTexture( GL_TEXTURE_2D, 0));

		//pCaps->Release();
	}

	if( m_pBuffer[0] != NULL)
	{
		I3MEM_SAFE_FREE( m_pBuffer[0] );		 
	}
}

void i3TextureOGLES::Unlock(void)
{
	Unlock( 0 );
}

void i3TextureOGLES::MakeRuntimeForm(void)
{
	INT32 i;
	
#if defined( I3G_G10 )
	char * pSrc, * pDest;

	i3Surface * pSurface = getSurface( 0);
	pSrc = (char*)	pSurface->getPersistData();
	pDest = Lock( 0 );

	i3Image_Convert( m_PersistFormat, pSrc, m_Width, m_Height, 1, GetClut(0), m_Format, pDest, 4, GetClut(0));

	Unlock( 0 );
#else

	i3Texture::MakeRuntimeForm();

#endif

	for( i = 0; i < GetLevelCount(); i++)
	{		
		INT8* pData = (INT8 *)GetPersistData( i);
		I3MEM_SAFE_FREE( pData);

		//if( m_pPersistData[i] != NULL)
		//{			
			//i3mem::Free( m_pPersistData[i]);
			//m_pPersistData[i] = NULL; 
		//}
	}
}

