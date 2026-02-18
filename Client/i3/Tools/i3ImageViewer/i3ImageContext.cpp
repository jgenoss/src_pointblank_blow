#include "StdAfx.h"
#include ".\i3imagecontext.h"

#include "i3Base/string/ext/extract_filetitle_fileext.h"

Ci3ImageContext::Ci3ImageContext(void)
{
	ZeroMemory(m_cFileName, 256);						//파일명
	ZeroMemory(m_cExt, 8);								//파일종류
	
	m_Format		= I3G_IMAGE_FORMAT_NONE;		//이미지 포맷

	m_nImageWidth		= 0;							//이미지 크기 WIDTH
	m_nImageHeight		= 0;							//이미지 크기 HEIGHT

	m_nLevelCount		= 0;							//퍼픽셀 비트
	m_nBitPerPixel		= 0;							//퍼픽셀 비트
	m_nStride			= 0;							//Stride
	m_nPitch			= 0;							//Pitch
	for(INT32	i = 0 ; i < MAX_MIPMAP_LEVEL + 1 ; ++i)			m_nDataSize[i]			= 0;
}

Ci3ImageContext::~Ci3ImageContext(void)
{

}

/////////////////////////////////////////////////////////////////////////////
void	Ci3ImageContext::SetImageName( const char * szFileName)
{
	if(szFileName != NULL)
	{
//		i3String::SplitFileName(szFileName ,m_cFileName  , FALSE);
//		i3String::SplitFileExt(szFileName, m_cExt, sizeof(m_cExt) - 1);
		i3::extract_filetitle_fileext(szFileName, m_cFileName, m_cExt);
	}
	else
	{
		strcpy(m_cFileName , "");
		strcpy(m_cExt , "");
	}
}

void Ci3ImageContext::SetContext(i3Texture * pTexture)
{
	m_pTexture = pTexture;

	if(pTexture == NULL)
	{
		m_Format		= I3G_IMAGE_FORMAT_NONE;		//이미지 포맷

		m_nImageWidth		= 0;							//이미지 크기 WIDTH
		m_nImageHeight		= 0;							//이미지 크기 HEIGHT

		m_nLevelCount		= 0;							//퍼픽셀 비트
		m_nBitPerPixel		= 0;							//퍼픽셀 비트
		m_nStride			= 0;							//Stride
		m_nPitch			= 0;							//Pitch
		for(INT32	i = 0 ; i < MAX_MIPMAP_LEVEL + 1 ; ++i)			m_nDataSize[i]			= 0;
	}
	else
	{
		m_PersistFormat		= pTexture->GetPersistFormat();	//저장 포맷
		m_Format			= pTexture->GetFormat();		//이미지 포맷

		m_nImageWidth		= pTexture->GetWidth();			//이미지 크기 WIDTH
		m_nImageHeight		= pTexture->GetHeight();		//이미지 크기 HEIGHT

		m_nLevelCount		= pTexture->GetLevelCount();	//퍼픽셀 비트
		m_nBitPerPixel		= i3Gfx::GetImageFormatBitCount(pTexture->GetPersistFormat());//퍼픽셀 비트
		m_nStride			= pTexture->GetStride();		//Stride
		m_nPitch			= pTexture->GetPitch();			//Pitch

		//Calculate DataSize;//레벨
		INT32	width  = (INT32)m_nImageWidth;
		INT32	height = (INT32)m_nImageHeight;
		UINT32 BufferSz = 0, TexBufSz = 0;

		for(UINT32 i = 0; i < m_nLevelCount; i++)
		{
				m_nDataSize[i] = i3Gfx::CalcRasterBufferSize( width,  height, 1, m_Format);
				BufferSz += m_nDataSize[i];

				TexBufSz += pTexture->GetDataSize( i);

				width = width >> 1;
				height = height >> 1;

				if((width < 1) || (height < 1))
					break;
		}

		I3ASSERT( BufferSz == TexBufSz);

		m_nDataSize[MAX_MIPMAP_LEVEL] = TexBufSz;
	}
}


char * 	Ci3ImageContext::GetFormatStr(char * pszValueStr)
{
	UINT32 RuntimeImageFormat	= m_Format & ~( I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB );

	switch( RuntimeImageFormat )
	{
		case I3G_IMAGE_FORMAT_NONE					: strcpy( pszValueStr, "None");					break;//
		case I3G_IMAGE_FORMAT_I4					: strcpy( pszValueStr, "I4");					break;//4
		case I3G_IMAGE_FORMAT_I8					: strcpy( pszValueStr, "I8");					break;//8
		case i3G_IMAGE_FORMAT_L8					: strcpy( pszValueStr, "L8");					break;
		case i3G_IMAGE_FORMAT_A8					: strcpy( pszValueStr, "A8");					break;
		case I3G_IMAGE_FORMAT_RGB_565				: strcpy( pszValueStr, "RGB565");				break;//16
		case I3G_IMAGE_FORMAT_XRGB_1555				: strcpy( pszValueStr, "XRGB1555");				break;
		case I3G_IMAGE_FORMAT_ARGB_1555				: strcpy( pszValueStr, "ARGB1555");				break;
		case I3G_IMAGE_FORMAT_XRGB_4444				: strcpy( pszValueStr, "XRGB4444");				break;
		case I3G_IMAGE_FORMAT_ARGB_4444				: strcpy( pszValueStr, "ARGB4444");				break;
		case I3G_IMAGE_FORMAT_RGB_888				: strcpy( pszValueStr, "RGB888");				break;//24
		case I3G_IMAGE_FORMAT_BGR_888				: strcpy( pszValueStr, "BGR888");				break;

		case I3G_IMAGE_FORMAT_RGBX_8888					: strcpy( pszValueStr, "RGBX8888");			break;//32
		case I3G_IMAGE_FORMAT_BGRX_8888					: strcpy( pszValueStr, "BGRX8888");			break;
		case I3G_IMAGE_FORMAT_RGBA_8888					: strcpy( pszValueStr, "RGBA8888");			break;
		case I3G_IMAGE_FORMAT_BGRA_8888					: strcpy( pszValueStr, "BGRA8888");			break;
		case I3G_IMAGE_FORMAT_VU_88						: strcpy( pszValueStr, "VU88");				break;
		case I3G_IMAGE_FORMAT_QWVU_8888					: strcpy( pszValueStr, "QWVU8888");			break;
		case I3G_IMAGE_FORMAT_DXT1_X					: strcpy( pszValueStr, "DXT1X");			break;//dxt
		case I3G_IMAGE_FORMAT_DXT1						: strcpy( pszValueStr, "DXT1");				break;
		case I3G_IMAGE_FORMAT_DXT2						: strcpy( pszValueStr, "DXT2");				break;
		case I3G_IMAGE_FORMAT_DXT3						: strcpy( pszValueStr, "DXT3");				break;
		case I3G_IMAGE_FORMAT_DXT4						: strcpy( pszValueStr, "DXT4");				break;
		case I3G_IMAGE_FORMAT_DXT5						: strcpy( pszValueStr, "DXT5");				break;
		case I3G_IMAGE_FORMAT_FORCE_DWORD				: strcpy( pszValueStr, "FORCEDWORD");		break;
		
		// HDR Format
		case I3G_IMAGE_FORMAT_ABGR_16F					: strcpy( pszValueStr, "ABGR_16F");			break;
		case I3G_IMAGE_FORMAT_ABGR_32F					: strcpy( pszValueStr, "ABGR_32F");			break;
		case I3G_IMAGE_FORMAT_ABGR_16					: strcpy( pszValueStr, "ABGR_16");			break;
		case I3G_IMAGE_FORMAT_R_32F						: strcpy( pszValueStr, "R_32F");			break;
		case I3G_IMAGE_FORMAT_GR_32F					: strcpy( pszValueStr, "GR_32F");			break;

		default											: strcpy( pszValueStr, "UnKnown Type");		break;//unknown
	}

	return pszValueStr;
}

char *Ci3ImageContext::GetPersistFormatStr( char * szOut )
{
	// 플랫폼이 다른 경우
	UINT32 PersistPlatform		= m_PersistFormat & I3G_IMAGE_FORMAT_MASK_PLATFORM;
	UINT32 PersistImageFormat	= m_PersistFormat & ~( I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB );
	BOOL bZlib = m_PersistFormat & I3G_IMAGE_FORMAT_MASK_ZLIB;

	const char *szFlatform = "Unknown";
	switch( PersistPlatform )
	{
	case I3G_IMAGE_FORMAT_MASK_PC:
		{
			szFlatform = "PC";
		}
		break;

	case I3G_IMAGE_FORMAT_MASK_PSP:
		{
			szFlatform = "PSP";
		}
		break;
	default:
		{
			szFlatform = "Unknown / PC";
		}
		break;
	}

	sprintf( szOut, "%X : ", m_PersistFormat );

	switch( PersistImageFormat )
	{
	case I3G_IMAGE_FORMAT_NONE					: strcat( szOut, "None");					break;//
	case I3G_IMAGE_FORMAT_I4					: strcat( szOut, "I4");					break;//4
	case I3G_IMAGE_FORMAT_I8					: strcat( szOut, "I8");					break;//8
	case i3G_IMAGE_FORMAT_L8					: strcat( szOut, "L8");					break;
	case i3G_IMAGE_FORMAT_A8					: strcat( szOut, "A8");					break;
	case I3G_IMAGE_FORMAT_RGB_565				: strcat( szOut, "RGB565");				break;//16
	case I3G_IMAGE_FORMAT_XRGB_1555				: strcat( szOut, "XRGB1555");				break;
	case I3G_IMAGE_FORMAT_ARGB_1555				: strcat( szOut, "ARGB1555");				break;
	case I3G_IMAGE_FORMAT_XRGB_4444				: strcat( szOut, "XRGB4444");				break;
	case I3G_IMAGE_FORMAT_ARGB_4444				: strcat( szOut, "ARGB4444");				break;
	case I3G_IMAGE_FORMAT_RGB_888				: strcat( szOut, "RGB888");				break;//24
	case I3G_IMAGE_FORMAT_BGR_888				: strcat( szOut, "BGR888");				break;

	case I3G_IMAGE_FORMAT_RGBX_8888				: strcat( szOut, "RGBX8888");			break;//32
	case I3G_IMAGE_FORMAT_BGRX_8888				: strcat( szOut, "RGBA8888");			break;
	case I3G_IMAGE_FORMAT_BGRA_8888				: strcat( szOut, "BGRA8888");			break;
	case I3G_IMAGE_FORMAT_VU_88					: strcat( szOut, "VU88");				break;
	case I3G_IMAGE_FORMAT_QWVU_8888				: strcat( szOut, "QWVU8888");			break;
	case I3G_IMAGE_FORMAT_DXT1_X				: strcat( szOut, "DXT1X");			break;//dxt
	case I3G_IMAGE_FORMAT_DXT1					: strcat( szOut, "DXT1");				break;
	case I3G_IMAGE_FORMAT_DXT2					: strcat( szOut, "DXT2");				break;
	case I3G_IMAGE_FORMAT_DXT3					: strcat( szOut, "DXT3");				break;
	case I3G_IMAGE_FORMAT_DXT4					: strcat( szOut, "DXT4");				break;
	case I3G_IMAGE_FORMAT_DXT5					: strcat( szOut, "DXT5");				break;

	// HDR Format
	case I3G_IMAGE_FORMAT_ABGR_16F				: strcpy( szOut, "ABGR_16F");			break;
	case I3G_IMAGE_FORMAT_ABGR_32F				: strcpy( szOut, "ABGR_32F");			break;
	case I3G_IMAGE_FORMAT_ABGR_16				: strcpy( szOut, "ABGR_16");			break;
	case I3G_IMAGE_FORMAT_R_32F					: strcpy( szOut, "R_32F");				break;
	case I3G_IMAGE_FORMAT_GR_32F				: strcpy( szOut, "GR_32F");				break;

	default										: strcat( szOut, "UnKnown Type");		break;//unknown
	}

	strcat( szOut, " ( " );
	strcat( szOut, szFlatform );
	if( bZlib )
	{
		strcat( szOut, ", Zlib" );
	}
	strcat( szOut, " )" );

	return szOut;
}

BOOL Ci3ImageContext::HasAlpha()
{
	if( m_Format & I3G_IMAGE_FORMAT_MASK_ALPHA )
	{
		return TRUE;
	}
	else
	{
		if( m_Format & I3G_IMAGE_FORMAT_MASK_INDEXED )
		{
			i3Clut *pClut = m_pTexture->GetClut( 0 );
			I3G_IMAGE_FORMAT ClutFormat;
			ClutFormat = pClut->GetFormat();
			if( ClutFormat & I3G_IMAGE_FORMAT_MASK_ALPHA )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}