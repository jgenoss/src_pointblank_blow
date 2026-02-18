#include "i3GfxType.h"
#include <stdio.h>
#include "i3ImageFile.h"
#include "i3ImageFileI3I.h"
#include "i3Texture.h"
#include "i3TextureMem.h"
#include "i3GfxUtil.h"
#include "i3ImageUtil.h"
#include "i3ZLib.h"
#include "i3GfxGlobalVariable.h"
#include "i3TextureResManager.h"

#if defined( I3G_DX)
#include "i3TextureDx.h"
#elif defined( I3G_XBOX)
#include "i3TextureXbox.h"
#elif defined( I3G_PSP)
#include "i3TexturePSP.h"
#elif defined( I3G_OGLES)
#include "i3TextureOGLES.h" 
#endif

static I3G_CUBE_TEXTURE_FACE s_Face[6] =
{
	I3G_CUBE_TEXTURE_FACE_PX,
	I3G_CUBE_TEXTURE_FACE_NX,
	I3G_CUBE_TEXTURE_FACE_PY,
	I3G_CUBE_TEXTURE_FACE_NY,
	I3G_CUBE_TEXTURE_FACE_PZ,
	I3G_CUBE_TEXTURE_FACE_NZ
};

UINT32	i3ImageFile::LoadI3I_Header( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, char * pszName)
{
	UINT32 Rc, Result = 0;

		// Header
	Rc = pStream->Read( ((char*) pHeader), sizeof(i3::pack::IMAGE_FILE_HEADER) );
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::LoadI3I()", "Could not read I3I header information.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( memcmp( pHeader->m_OldVersionHeader.m_Signature, I3I_SIGNATURE_BINARY, I3I_SIGNATURE_SIZE) != 0)
		return STREAM_ERR;

	// Version 3 Header
	if( pHeader->m_OldVersionHeader.m_Version[0] >= 5)
	{
		Rc = pStream->Read( ((char *)pHeader) + sizeof(i3::pack::IMAGE_FILE_HEADER), sizeof(i3::pack::IMAGE_FILE_HEADER3) - sizeof(i3::pack::IMAGE_FILE_HEADER));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::LoadI3I()", "Could not read I3I header3 information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}
	else
	{
		// Version 2 Header
		if( pHeader->m_OldVersionHeader.m_Version[0] > 2)
		{
			Rc = pStream->Read( ((char *)pHeader) + sizeof(i3::pack::IMAGE_FILE_HEADER), sizeof(i3::pack::IMAGE_FILE_HEADER2) - sizeof(i3::pack::IMAGE_FILE_HEADER));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::LoadI3I()", "Could not read I3I header2 information.");
				return STREAM_ERR;
			}
			Result += Rc;

			
			pHeader->m_DataSize[0] = 0;
			pHeader->m_Usage = 0;
			pHeader->m_MipmapGen = I3G_MIPMAP_LOAD;
			pHeader->m_MipmapGenFilter = I3G_TEXTURE_FILTER_NONE;
		}
		else
		{
			// 이전 Version의 I3I에서는 단지 하나의 Mipmap만을 지원했기 때문에...
			pHeader->m_DataSize[0] = 0;
			pHeader->m_MipMapCount = 1;
			pHeader->m_TexFlag = 0;
			pHeader->m_NameLength = 0;
		}
	}

	// Name
	if( pHeader->m_OldVersionHeader.m_Version[0] >= 4)
	{
		if(pHeader->m_NameLength > 0)
		{
			if( pszName != nullptr)
			{
				Rc = pStream->Read( pszName, pHeader->m_NameLength);
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3ImageFileI3I::LoadI3I()", "Could not read texture name.");
					return STREAM_ERR;
				}
				Result += Rc;
			}
		}
	}
	else
	{
		pHeader->m_NameLength = 0;
	}

	if( pszName != nullptr)
		pszName[ pHeader->m_NameLength] = 0;		// nullptr-terminated string

	if( pHeader->m_OldVersionHeader.m_Format == I3G_IMAGE_FORMAT_DXT1_X)
		pHeader->m_OldVersionHeader.m_Format = I3G_IMAGE_FORMAT_DXT1;

	if( pHeader->m_MipmapGen == I3G_MIPMAP_AUTO_GEN)
	{
		pHeader->m_Usage |= I3G_USAGE_AUTOGENMIPMAP;

		if( pHeader->m_MipmapGenFilter == I3G_TEXTURE_FILTER_NONE)
		{
			pHeader->m_MipmapGenFilter = I3G_TEXTURE_FILTER_LINEAR;
		}
	}

	return Result;
}

/** \brief Image File Header
	\return true : Success, false : Skip */
bool Parsing_Header( const i3::pack::IMAGE_FILE_HEADER3 & head, INT32 * nX, INT32 * nY, INT32 * mipmapLevel, UINT32 * usage, INT32 * passCnt)
{
	*nX = head.m_OldVersionHeader.m_Width;
	*nY = head.m_OldVersionHeader.m_Height;
	*mipmapLevel = head.m_MipMapCount;
	*usage = head.m_Usage;
	*passCnt = 0;

	if( g_pRenderContext != nullptr)
	{
		// Gfx Option에따른 처리를 합니다.
		if( g_pRenderContext->getGfxOption()->getNormalMapEnable() == false)		
		{
			if( head.m_TexFlag & I3I_TEXFLAG_NORMAL)
				return false;
		}

		if( g_pRenderContext->getGfxOption()->getSpecularMapEnable() == false)
		{
			if( head.m_TexFlag & I3I_TEXFLAG_SPECULAR)
				return false;
		}

		if( g_pRenderContext->getGfxOption()->getSpecularMapEnable() == false)
		{
			if( head.m_TexFlag & I3I_TEXFLAG_SPECULAR)	
				return false;
		}

		if( g_pRenderContext->getGfxOption()->getEmissiveMapEnable() == false)
		{
			if( head.m_TexFlag & I3I_TEXFLAG_EMISSIVE)	
				return false;
		}

		// Texture Quality
		INT32 texQuality;

		if(( head.m_OldVersionHeader.m_Width > 32) && (head.m_OldVersionHeader.m_Height > 32))
		{
			texQuality = g_pRenderContext->getGfxOption()->getTextureQuality();
		}
		else
		{
			texQuality = 0;
		}

		if( head.m_MipMapCount > 1 && texQuality > 0)
		{
			switch( texQuality)
			{
			case 1 :
				{
					*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.5f);
					*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.5f);

					*mipmapLevel = head.m_MipMapCount-1;
					*passCnt = 1;
				}
				break;

			case 2 :
				{
					*mipmapLevel = head.m_MipMapCount-2;
					if( *mipmapLevel < 1)
					{
						*mipmapLevel = 1;
						*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.5f);
						*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.5f);
						*passCnt = 1;
					}
					else
					{
						*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.25f);
						*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.25f);
						*passCnt = 2;
					}
				}
				break;

			case 3 :
				{
					*mipmapLevel = head.m_MipMapCount-3;
					if( *mipmapLevel < 0)
					{	// mipmap이 2개뿐이다.
						*mipmapLevel = 1;
						*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.5f);
						*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.5f);
						*passCnt = 1;
					}
					else if( *mipmapLevel < 1)
					{	// mipmap이 3개다.
						*mipmapLevel = 1;
						*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.25f);
						*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.25f);
						*passCnt = 2;
					}
					else
					{
						*nX = (INT32)(head.m_OldVersionHeader.m_Width*0.125f);
						*nY = (INT32)(head.m_OldVersionHeader.m_Height*0.125f);
						*passCnt = 3;
					}
				}
				break;
			default :
				*nX = head.m_OldVersionHeader.m_Width;
				*nY = head.m_OldVersionHeader.m_Height;
				*mipmapLevel = head.m_MipMapCount;
				*usage = head.m_Usage;
				break;
			}
		}
	}

	return true;
}

bool i3ImageFile::LoadI3I( i3Stream * pStream, i3Texture * pTex)
{
	i3::pack::IMAGE_FILE_HEADER3 header;
	UINT32 Rc;
	bool Rv = false, Result = false;
	char szName[MAX_PATH];

	m_ReadBytes = 0;
	m_nPassCount = 0;

	pTex->setRestorePoint( pStream);

	i3mem::FillZero( &header, sizeof( header));

	Rc = LoadI3I_Header( pStream, &header, szName);
	if( Rc == STREAM_ERR)
	{
		goto ExitPart;
	}

	INT32 nX = header.m_OldVersionHeader.m_Width;
	INT32 nY = header.m_OldVersionHeader.m_Height;
	INT32 mipmapLevel = header.m_MipMapCount;
	UINT32 usage = header.m_Usage;

	if( Parsing_Header( header, &nX, &nY, &mipmapLevel, &usage, &m_nPassCount) == false)
	{
		setSkipResource(true);
		return true;
	}

	if( szName[0] != 0)
		pTex->SetName( szName);
	else if( i3::same_of<i3FileStream*>(pStream))
		pTex->SetName( ((i3FileStream*)pStream)->GetFileName());


	Rv = pTex->Create( nX, nY, mipmapLevel, header.m_OldVersionHeader.m_Format, usage);

	if( Rv == false)
	{
		i3Log( "i3ImageFile::LoadI3I()", "Could not create i3Image object.");
		goto ExitPart;
	}
	
	m_ReadBytes += Rc;

	if( header.m_TexFlag & I3I_TEXFLAG_CUBE)
	{
		I3ASSERT( i3::kind_of<i3TextureCube*>(pTex));

		Result = LoadI3I_Cube( pStream, &header, (i3TextureCube *) pTex);
	}
	else
	{
		Result = LoadI3I_2D( pStream, &header, pTex);
	}

	if( Result)
		pTex->MakeRuntimeForm();

ExitPart:
	
	return Result;
}

bool i3ImageFile::BGLoadI3I( i3Stream * pStream, i3Texture * pTexture)
{
	i3TextureResManager * pMng = static_cast<i3TextureResManager*>(i3ResourceFile::GetResourceManager( i3Texture::static_meta()));
	
	if( pMng != nullptr && pMng->getCPUCount() > 1)
	{
		i3::pack::IMAGE_FILE_HEADER3 header;
		UINT32 Rc;
		bool Rv = false;
		char szName[MAX_PATH];
		bool Result = false;

		m_ReadBytes = 0;
		m_nPassCount = 0;

		pTexture->setRestorePoint( pStream);

		i3mem::FillZero( &header, sizeof( header));

		Rc = LoadI3I_Header( pStream, &header, szName);
		if( Rc == STREAM_ERR)
		{
			goto ExitPart;
		}

		INT32 nX;
		INT32 nY;
		INT32 mipmapLevel;
		UINT32 usage;

		if( Parsing_Header( header, &nX, &nY, &mipmapLevel, &usage, &m_nPassCount) == false)
		{
			setSkipResource(true);
			return true;
		}

		INT32 w = nX;
		INT32 h = nY;
		INT32 lv = mipmapLevel;
		while( lv > 1)
		{
			w = (w>>1);
			h = (h>>1);
			lv--;
		}

		Rv = pTexture->Create( w, h, 1, header.m_OldVersionHeader.m_Format, usage);

		if( szName[0] != 0)
			pTexture->SetName( szName);

		if( Rv == false)
		{
			i3Log( "i3ImageFile::LoadI3I()", "Could not create i3Image object.");
			goto ExitPart;
		}
		
		m_ReadBytes += Rc;

		if( header.m_TexFlag & I3I_TEXFLAG_CUBE)
		{
			I3ASSERT( i3::kind_of<i3TextureCube*>(pTexture));

			Result = LoadI3I_Cube( pStream, &header, (i3TextureCube *) pTexture);
		}
		else
		{
			Result = LoadI3I_2D_Level( pStream, nX, nY, &header, pTexture, mipmapLevel-1);
			
			I3_BOUNDCHK( nX, 65536);
			I3_BOUNDCHK( nY, 65536);
			I3_BOUNDCHK( mipmapLevel, 16);
			pMng->RequestSplitLoad( pTexture, (UINT16)nX, (UINT16)nY, (UINT16)mipmapLevel, header.m_OldVersionHeader.m_Format, usage);
		}

		if( Result)
			pTexture->MakeRuntimeForm();

ExitPart:
		return Result;
	}
	else
	{
		return LoadI3I( pStream, pTexture);
	}
}

bool i3ImageFile::LoadI3I_2D( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTex)
{
	UINT32 Width, Height;
	UINT32 Rc, ImageSz, level;
	bool Rv = false;
	UINT8 * pZBuff = nullptr;
	bool bLocked = false;
	UINT32 ZBuffSz = 0;
	INT32 nTempLevel = 0;
	UINT8 cnt = 0;

	// Pixel Data
	// 
	// I3I 파일의 Pixel Data는 아무런 Alignment도 적용하지 않는다. 즉, 모든 Pixel, Low들은 연속적인 공간에
	// 저장된다.
	Width = pHeader->m_OldVersionHeader.m_Width;
	Height = pHeader->m_OldVersionHeader.m_Height;

	ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat());

	if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_ZLIB)
	{
		
		ZBuffSz = pHeader->m_DataSize[0];

		pZBuff = (UINT8 *) i3MemAlloc( ZBuffSz);
	}

	for( level = 0; level < pHeader->m_MipMapCount; level++)
	{
		char * pDest = nullptr;

		// level이 외부에서 설정된 texture의 level보다 크면 무시(사용하지 않는 것으로 간주)
		/*if( level >= pTex->GetLevelCount())
			break;*/

		// Clut
		{
			if( pHeader->m_OldVersionHeader.m_ClutCount > 0)
			{
				i3Clut * pClut;

				pClut = i3Clut::new_object();

				Rc = pClut->Load( pStream);
				if( Rc == STREAM_ERR)
				{
					I3_MUST_RELEASE( pClut);
					I3_MUST_RELEASE( pTex);
					goto ExitPart;
				}
				m_ReadBytes += Rc;

				if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
				{
					pTex->SetClut( level, pClut);
				}

				I3_MUST_RELEASE( pClut);
			}
		}

		ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat());
		
		if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_ZLIB)
		{
			// 압축 형식
			Rc = pStream->Read( pZBuff, pHeader->m_DataSize[ level]);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
				goto ExitPart;
			}
			m_ReadBytes += Rc;
		}

		if( pTex->GetPersistFormat() == pTex->GetFormat())
		{
			// Runtime Format과 Persist Format이 같은 경우에는.
			// 아무런 Data 변환이 필요없는 경우다.
			// 그러므로, 직접 Texture Data에 읽어들인다.
			
			if( m_nPassCount == 0) // Texture퀄러티에 따른, 조절변수입니다.(송명일)
			{
				cnt = 0;
				do {
					pDest = pTex->Lock( nTempLevel);
					if( pDest == nullptr)
					{
						i3Timer::Sleep( 20);
						cnt++;
						if( cnt > 100)
						{
							I3PRINTLOG(I3LOG_FATAL,  "Tex Lock failed!!!");
							break;
						}
					}
				} while( pDest == nullptr);

				bLocked = true;
			}
			
			if( pZBuff != nullptr)
			{
				// 압축 데이터에서 압축을 푼다.

				Rv = i3ZLib::Decompress( pZBuff, pHeader->m_DataSize[level], (UINT8 *) pDest, &ImageSz);
				if( Rv == false)
				{
					I3TRACE1( "i3ImageFile::LoadI3I() - Could not decompress %dth image data.", level);
					goto ExitPart;
				}
			}
			else
			{
				if( m_nPassCount == 0)	// Texture퀄러티에 따른, 조절변수입니다.(송명일)
				{
					Rc = pStream->Read( pDest, ImageSz);
					if( Rc == STREAM_ERR)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
						goto ExitPart;
					}

#if defined( I3_DEBUG) && 0
					if( m_bDump)
					{
						i3Texture * pDumpTex = i3Texture::new_object();
						pDumpTex->Create( Width, Height, 1, pTex->GetPersistFormat(), 0);
						char * pDumpDest = pDumpTex->Lock(0);
						i3mem::Copy( pDumpDest, pDest, ImageSz);
						pDumpTex->Unlock(0);
						i3ImageFile ff;
						static int ffCnt = 0;
						i3::string str;
						i3::sprintf( str, "D:/loadDump%d.i3i", ffCnt);
						ffCnt++;
						ff.Save( str.c_str(), pDumpTex);
						I3_SAFE_RELEASE( pDumpTex);
					}
#endif
				}
				else
				{
					Rc = pStream->SetPosition( ImageSz, STREAM_CURRENT);
					m_nPassCount--;
				}

				m_ReadBytes += Rc;
			}
		
			if( bLocked)
			{
				pTex->Unlock( nTempLevel);

				nTempLevel++;

				bLocked = false;
			}
		}
		else
		{
			// Runtime Format과 Persist Format이 다른 경우에는
			// Texture의 Persist Data로 별도 메모리를 할당하여 Loading한다.
			// 이 자체를 특별한 Error로 취급하지는 않고, Warning으로 처리한다.

			pDest = (char *) i3MemAlloc( ImageSz);

			if( level < pTex->GetLevelCount())
				pTex->SetPersistData( level, (INT8 *) pDest);

			if( pZBuff != nullptr)
			{
				// 압축 해제
				Rv = i3ZLib::Decompress( pZBuff, pHeader->m_DataSize[level], (UINT8 *) pDest, &ImageSz);
				if( Rv == false)
				{
					I3TRACE1( "i3ImageFile::LoadI3I() - Could not decompress %dth image data.", level);
					goto ExitPart;
				}
			}
			else
			{
				if( m_nPassCount == 0)	// Texture퀄러티에 따른, 조절변수입니다.
				{
					Rc = pStream->Read( pDest, ImageSz);
					if( Rc == STREAM_ERR)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
						goto ExitPart;
					}
				}
				else
				{
					Rc = pStream->SetPosition( ImageSz, STREAM_CURRENT);
					m_nPassCount--;
				}

				m_ReadBytes += Rc;
			}

			if( level >= pTex->GetLevelCount())
			{	// 사용할 수 없는 데이터 삭제하자
				I3MEM_SAFE_FREE( pDest);
			}
		}

		Width = Width >> 1;
		Height = Height >> 1;		
	}

	if( (pHeader->m_OldVersionHeader.m_Format & I3G_IMAGE_FORMAT_MASK_INDEXED) == 0)
	{
		INT32 i;

		// Bug fix
		for( i = 0; i < pTex->getSurfaceCount(); i++)
		{
			i3Surface * pSurface = pTex->getSurface( i);

			pSurface->SetClut( nullptr);
		}
	}

ExitPart:
	if( pZBuff != nullptr)
	{
		i3MemFree( pZBuff);
	}

	return true;
}

bool i3ImageFile::LoadI3I_2D_Level( i3Stream * pStream, UINT32 Width, UINT32 Height, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTex, INT32 level)
{
	UINT32 Rc = 0, ImageSz = 0;
	bool Rv = false;
	UINT8 * pZBuff = nullptr;
	UINT32 ZBuffSz = 0;
	UINT32 nTempLevel = 0;
	UINT8 cnt = 0;

	if( level >= pHeader->m_MipMapCount)
		return false;

	// Pixel Data
	// I3I 파일의 Pixel Data는 아무런 Alignment도 적용하지 않는다. 즉, 모든 Pixel, Low들은 연속적인 공간에
	// 저장된다.
	if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_ZLIB)
	{	
		ZBuffSz = pHeader->m_DataSize[0];
		pZBuff = (UINT8 *) i3MemAlloc( ZBuffSz);
	}

	// Clut
	if( pHeader->m_OldVersionHeader.m_ClutCount > 0)
	{
		i3Clut * pClut;

		pClut = i3Clut::new_object();

		Rc = pClut->Load( pStream);
		if( Rc == STREAM_ERR)
		{
			I3_MUST_RELEASE( pClut);
			I3_MUST_RELEASE( pTex);
			goto ExitPart;
		}
		m_ReadBytes += Rc;

		if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			pTex->SetClut( level, pClut);
		}

		I3_MUST_RELEASE( pClut);
	}

	char * pDest = nullptr;
	UINT32 totalPos = 0;

	while( cnt < level)
	{
		ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat());
		totalPos += ImageSz;

		Width = Width >> 1;
		Height = Height >> 1;
		cnt++;
	}

	//totalPos -= ImageSz;
	ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat());
	
	if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_ZLIB)
	{
		// 압축 형식
		Rc = pStream->Read( pZBuff, pHeader->m_DataSize[ level]);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
			goto ExitPart;
		}
		m_ReadBytes += Rc;
	}

	if( pTex->GetPersistFormat() == pTex->GetFormat())
	{
		// Runtime Format과 Persist Format이 같은 경우에는.
		// 아무런 Data 변환이 필요없는 경우다.
		// 그러므로, 직접 Texture Data에 읽어들인다.
		
		pDest = pTex->Lock( nTempLevel);
		if( pDest == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Tex Lock failed!!!");
		}
		
		if( pZBuff != nullptr)
		{
			// 압축 데이터에서 압축을 푼다.

			Rv = i3ZLib::Decompress( pZBuff, pHeader->m_DataSize[level], (UINT8 *) pDest, &ImageSz);
			if( Rv == false)
			{
				I3TRACE1( "i3ImageFile::LoadI3I() - Could not decompress %dth image data.", level);
				goto ExitPart;
			}
		}
		else
		{
			Rc = pStream->SetPosition( totalPos + m_ReadBytes, STREAM_BEGIN);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
				goto ExitPart;
			}
			m_ReadBytes += totalPos;

			Rc = pStream->Read( pDest, ImageSz);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
				goto ExitPart;
			}

			m_ReadBytes += Rc;
		}
	
		pTex->Unlock( nTempLevel);
	}
	else
	{
		// Runtime Format과 Persist Format이 다른 경우에는
		// Texture의 Persist Data로 별도 메모리를 할당하여 Loading한다.
		// 이 자체를 특별한 Error로 취급하지는 않고, Warning으로 처리한다.

		pDest = (char *) i3MemAlloc( ImageSz);

		if( nTempLevel < pTex->GetLevelCount())
			pTex->SetPersistData( nTempLevel, (INT8 *) pDest);

		if( pZBuff != nullptr)
		{
			// 압축 해제
			Rv = i3ZLib::Decompress( pZBuff, pHeader->m_DataSize[nTempLevel], (UINT8 *) pDest, &ImageSz);
			if( Rv == false)
			{
				I3TRACE1( "i3ImageFile::LoadI3I() - Could not decompress %dth image data.", nTempLevel);
				I3MEM_SAFE_FREE( pDest);
				goto ExitPart;
			}
		}
		else
		{
			Rc = pStream->SetPosition( totalPos + m_ReadBytes, STREAM_BEGIN);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "SetPos %dth mipmap image data : %s", nTempLevel, pTex->GetName());
				I3MEM_SAFE_FREE( pDest);
				goto ExitPart;
			}

			m_ReadBytes += totalPos;

			Rc = pStream->Read( pDest, ImageSz);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", nTempLevel, pTex->GetName());
				I3MEM_SAFE_FREE( pDest);
				goto ExitPart;
			}

			m_ReadBytes += Rc;
		}

		if( nTempLevel >= pTex->GetLevelCount())
		{	// 사용할 수 없는 데이터 삭제하자
			I3MEM_SAFE_FREE( pDest);
		}
	}

	if( (pHeader->m_OldVersionHeader.m_Format & I3G_IMAGE_FORMAT_MASK_INDEXED) == 0)
	{
		i3Surface * pSurface = pTex->getSurface( nTempLevel);
		pSurface->SetClut( nullptr);
	}

ExitPart:
	if( pZBuff != nullptr)
	{
		i3MemFree( pZBuff);
	}

	return true;
}

bool i3ImageFile::LoadI3I_Cube( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3TextureCube * pTex)
{
	UINT32 Width, Height;
	UINT32 Rc, ImageSz, level, face;
	bool bLocked = false;
	INT32 nTempLevel = 0;
	UINT8 cnt = 0;

	// Pixel Data
	// 
	// I3I 파일의 Pixel Data는 아무런 Alignment도 적용하지 않는다. 즉, 모든 Pixel, Low들은 연속적인 공간에
	// 저장된다.
	Width = pHeader->m_OldVersionHeader.m_Width;
	Height = pHeader->m_OldVersionHeader.m_Height;

	ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat()) * 6;

	for( level = 0; level < pHeader->m_MipMapCount; level++)
	{
		char * pDest = nullptr;

		// 개별 Surface에 대한 크기만...
		ImageSz = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTex->GetPersistFormat());

		// Clut
		if( pHeader->m_OldVersionHeader.m_ClutCount > 0)
		{
			i3Clut * pClut;

			pClut = i3Clut::new_object();

			Rc = pClut->Load( pStream);
			if( Rc == STREAM_ERR)
			{
				I3_MUST_RELEASE( pClut);
				I3_MUST_RELEASE( pTex);
				goto ExitPart;
			}
			m_ReadBytes += Rc;

			if( pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
			{
				pTex->SetClut( level, pClut);
			}

			I3_MUST_RELEASE( pClut);
		}

		for( face = 0; face < 6; face++)
		{
			if( pTex->GetPersistFormat() == pTex->GetFormat())
			{
				// Runtime Format과 Persist Format이 같은 경우에는.
				// 아무런 Data 변환이 필요없는 경우다.
				// 그러므로, 직접 Texture Data에 읽어들인다.
				
				if( m_nPassCount == 0) // Texture퀄러티에 따른, 조절변수입니다.(송명일)
				{
					cnt = 0;
					do {
						pDest = pTex->Lock( s_Face[ face], nTempLevel);
						if( pDest == nullptr)
						{
							i3Timer::Sleep( 20);
							cnt++;
							if( cnt > 100)
							{
								I3PRINTLOG(I3LOG_FATAL,  "Tex lock failed!!!");
								break;
							}
						}
					} while( pDest == nullptr);
					bLocked = true;
				}
				
				if( m_nPassCount == 0)	// Texture퀄러티에 따른, 조절변수입니다.(송명일)
				{
					Rc = pStream->Read( pDest, ImageSz);
					if( Rc == STREAM_ERR)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
						goto ExitPart;
					}
				}
				else
				{
					Rc = pStream->SetPosition( ImageSz, STREAM_CURRENT);
					m_nPassCount--;
				}

				m_ReadBytes += Rc;
			
				if( bLocked)
				{
					cnt = 0;
					while( pTex->Unlock( s_Face[face], nTempLevel) == false)
					{
						i3Timer::Sleep( 20);
						cnt++;
						if( cnt > 100)
						{
							I3PRINTLOG(I3LOG_FATAL,  "Tex Unlock failed!!!!!");
							break;
						}
					}

					bLocked = false;
				}
			}
			else
			{
				// Runtime Format과 Persist Format이 다른 경우에는
				// Texture의 Persist Data로 별도 메모리를 할당하여 Loading한다.
				// 이 자체를 특별한 Error로 취급하지는 않고, Warning으로 처리한다.
				//INT32 hMem;
				//hMem = i3MemAlloc( ImageSz);
				pDest = (char *) i3MemAlloc( ImageSz);

				pTex->SetPersistData( level, (INT8 *) pDest);

				if( m_nPassCount == 0)	// Texture퀄러티에 따른, 조절변수입니다.
				{
					Rc = pStream->Read( pDest, ImageSz);
					if( Rc == STREAM_ERR)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Could not load %dth mipmap image data : %s", level, pTex->GetName());
						goto ExitPart;
					}
				}
				else
				{
					Rc = pStream->SetPosition( ImageSz, STREAM_CURRENT);
					m_nPassCount--;
				}

				m_ReadBytes += Rc;
			}
		}

		nTempLevel++;
		Width = Width >> 1;
		Height = Height >> 1;		
	}

	if( (pHeader->m_OldVersionHeader.m_Format & I3G_IMAGE_FORMAT_MASK_INDEXED) == 0)
	{
		INT32 i;

		// Bug fix
		for( i = 0; i < pTex->getSurfaceCount(); i++)
		{
			i3Surface * pSurface = pTex->getSurface( i);

			pSurface->SetClut( nullptr);
		}
	}

ExitPart:

	return true;
}


bool i3ImageFile::SaveI3I( i3Stream * pStream, i3Texture * pTexture )
{
	i3::pack::IMAGE_FILE_HEADER3 header;
	UINT32 Rc;
	bool Result = false;

	I3ASSERT( pStream != nullptr);
	I3ASSERT( pTexture != nullptr);

	m_ReadBytes = 0;

	
	// Header
	{
		memcpy( header.m_OldVersionHeader.m_Signature, I3I_SIGNATURE_BINARY, I3I_SIGNATURE_SIZE );

		header.m_OldVersionHeader.m_Version[0] = 5;
		header.m_OldVersionHeader.m_Version[1] = 0;

		header.m_OldVersionHeader.m_Width = (UINT16) pTexture->GetWidth();
		header.m_OldVersionHeader.m_Height = (UINT16) pTexture->GetHeight();
		header.m_OldVersionHeader.m_Format = pTexture->GetPersistFormat();

		if( pTexture->hasName())
			header.m_NameLength = (UINT16) pTexture->GetNameString().size();
		else
			header.m_NameLength = 0;

		header.m_MipMapCount = (UINT16) pTexture->GetLevelCount();

		if( pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			// 모든 Mipmap Image들의 Pixel Format은 동일하다!
			i3Surface * pSurface = pTexture->getSurface( 0);
			i3Clut * pClut = pSurface->getClut();

			header.m_OldVersionHeader.m_ClutCount = (UINT16) pClut->GetColorCount();
		}
		else
		{
			header.m_OldVersionHeader.m_ClutCount = 0;
		}

		// Header에 Texture UsageFlag를 설정합니다.
		header.m_TexFlag = pTexture->getTextureFlag();

		m_HeaderPos = pStream->GetPosition();

		Rc = pStream->Write( &header, sizeof(i3::pack::IMAGE_FILE_HEADER3));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveI3I()", "Could not write I3I header information.");
			goto ExitPart;
		}
		m_ReadBytes += Rc;

		if( header.m_NameLength > 0)
		{
			Rc = pStream->Write( (void *) pTexture->GetName(), header.m_NameLength);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3ImageFile::SaveI3I()", "Could not write name of I3I image.");
				goto ExitPart;
			}
			m_ReadBytes += Rc;
		}
	}

	if( pTexture->getTextureFlag() & I3I_TEXFLAG_CUBE)
	{
		Result = SaveI3I_Cube( pStream, &header, (i3TextureCube *) pTexture);
	}
	else if( pTexture->getTextureFlag() & I3I_TEXFLAG_VOLUME)
	{
	}
	else
	{
		Result = SaveI3I_2D( pStream, &header, pTexture);
	}

ExitPart:

	return Result;
}

bool i3ImageFile::SaveI3I_2D( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTexture)
{
	UINT8 * pZLibBuf = nullptr;
	UINT32 ZBuffSz = 0;
	UINT32 Width, Height;
	bool bLocked;
	UINT32 level;
	UINT32 Rc;
	bool bResult = false;
	UINT8 cnt = 0;

	// Pixel Data
	Width = pTexture->GetWidth();
	Height = pTexture->GetHeight();

	if( pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_ZLIB)
	{
		UINT32 ImageSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTexture->GetPersistFormat());		

		ZBuffSz = i3ZLib::CalcCompressedSize( ImageSize);

		pZLibBuf = (UINT8 *) i3MemAlloc( ZBuffSz);
	}


	for( level = 0; level < pTexture->GetLevelCount(); level++)
	{
		UINT32 ImageSize;
		char * pBuf;

		// Clut
		if( pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			i3Clut * pClut = pTexture->GetClut( level);

			Rc = pClut->Save( pStream);
			if( Rc == STREAM_ERR)
			{
				goto ExitPart;
			}

			m_ReadBytes += Rc;
		}

		bLocked = false;

		if((pTexture->GetPersistFormat() != pTexture->GetFormat()) && (pTexture->GetPersistData( level) != nullptr))
		{
			// Runtime format과 Persist Format이 다르다.
			// 이 때, 원래의 Persist Data를 저장한다.
			pBuf = (char *) pTexture->GetPersistData( level);

			ImageSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTexture->GetPersistFormat());
		}
		else
		{
			cnt = 0;
			do {
				pBuf = pTexture->Lock(level);
				if( pBuf == nullptr)
				{
					i3Timer::Sleep( 20);
					cnt++;
					if( cnt > 100)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Tex lock failed!!!");
						break;
					}
				}
			} while( pBuf == nullptr);
			bLocked = true;

			ImageSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTexture->GetFormat());
		}

		if( pZLibBuf != nullptr)
		{
			pHeader->m_DataSize[level] = ZBuffSz;
			i3ZLib::Compress( (UINT8 *) pBuf, ImageSize, pZLibBuf, &pHeader->m_DataSize[level]);

			Rc = pStream->Write( pZLibBuf, pHeader->m_DataSize[level]);
		}
		else
		{
			Rc = pStream->Write( pBuf, ImageSize);
		}

		if( bLocked == true)
		{
			pTexture->Unlock( level);
		}

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveI3I()", "Could not write pixel data.");
			goto ExitPart;
		}

		m_ReadBytes += Rc;

		Width = Width >> 1;
		Height = Height >> 1;
	}


	if( pZLibBuf != nullptr)
	{
		// Data Size Field가 갱신되었기 때문에 Header를 새로 Update한다.
		UINT32 oldPos = pStream->GetPosition();

		pStream->SetPosition( m_HeaderPos, STREAM_BEGIN);

		Rc = pStream->Write( pHeader, sizeof(i3::pack::IMAGE_FILE_HEADER3));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveI3I()", "Could not write I3I header information.");
			goto ExitPart;
		}

		pStream->SetPosition( oldPos, STREAM_BEGIN);
	}

	bResult = true;

ExitPart:
	if( pZLibBuf != nullptr)
	{
		i3MemFree( pZLibBuf);
	}

	return bResult;
}

bool i3ImageFile::SaveI3I_Cube( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3TextureCube * pTexture)
{
	UINT32 Width, Height;
	bool bLocked;
	UINT32 level, face;
	UINT32 Rc;
	UINT8 cnt = 0;

	// Pixel Data
	Width = pTexture->GetWidth();
	Height = pTexture->GetHeight();

	for( level = 0; level < pTexture->GetLevelCount(); level++)
	{
		UINT32 ImageSize;
		char * pBuf;

		// Clut
		if( pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			i3Clut * pClut = pTexture->GetClut( level);

			Rc = pClut->Save( pStream);
			if( Rc == STREAM_ERR)
			{
				return false;
			}

			m_ReadBytes += Rc;
		}

		for( face = 0; face < 6; face++)
		{
			bLocked = false;

			if((pTexture->GetPersistFormat() != pTexture->GetFormat()) && (pTexture->GetPersistData( level) != nullptr))
			{
				// Runtime format과 Persist Format이 다르다.
				// 이 때, 원래의 Persist Data를 저장한다.
				pBuf = (char *) pTexture->GetPersistData( (level * 6) + face);

				ImageSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTexture->GetPersistFormat());
			}
			else
			{
				cnt = 0;
				do {
					pBuf = pTexture->Lock( s_Face[face], level);
					if( pBuf == nullptr)
					{
						i3Timer::Sleep( 20);
						cnt++;
						if( cnt > 100)
						{
							I3PRINTLOG(I3LOG_FATAL,  "Tex Lock Failed!!!");
							break;
						}
					}
				} while( pBuf == nullptr);

				bLocked = true;

				ImageSize = i3Gfx::CalcRasterBufferSize( Width, Height, 1, pTexture->GetFormat());
			}

			Rc = pStream->Write( pBuf, ImageSize);

			if( bLocked == true)
			{
				cnt = 0;
				while( pTexture->Unlock( s_Face[face]) == false)
				{
					i3Timer::Sleep( 20);
					cnt++;
					if( cnt > 100)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Tex Unlock failed!!!!!");
						break;
					}
				}
			}

			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN,  "Could not write pixel data.");
				return false;
			}

			m_ReadBytes += Rc;
		}

		Width = Width >> 1;
		Height = Height >> 1;
	}

	return true;
}

bool i3ImageFile::isCubeI3I( const char * pszPath)
{
	i3FileStream strm;

	if( strm.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not open %s file.", pszPath);
		return false;
	}

	i3::pack::IMAGE_FILE_HEADER3 header;

	header.m_TexFlag = 0;

	if( LoadI3I_Header( &strm, &header, nullptr) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not load a header of %s file.", pszPath);
		strm.Close();
		return false;
	}

	strm.Close();

	if( header.m_TexFlag & I3I_TEXFLAG_CUBE)
		return true;

	return false;
}
