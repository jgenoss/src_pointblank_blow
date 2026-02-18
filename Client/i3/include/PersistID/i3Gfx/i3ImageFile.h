#if !defined( __I3_IMAGE_FILE_H)
#define __I3_IMAGE_FILE_H

#include "i3Base.h"
#include "i3Texture.h"
#include "i3TextureCube.h"
#include "i3ImageFileI3I.h"

//
// i3Image의 내용을 특정 Format(BMP, TGA)으로 기록하거나,
// 반대로 특정 Format의 파일을 Native Image(i3Image)로 변환하여 읽어 들이는
// 기능을 제공한다.
//
// BMP, TGA 등의 여러 형식을 지원하기 위해 이들 i3ImageFile class 들은
// 전체적으로 하나의 List로 등록 관리된다.
class I3_EXPORT_GFX i3ImageFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3ImageFile);

protected:

	UINT32	m_ReadBytes;
	INT32	m_nPassCount;				// Loading시 품질 조절용
	BOOL	m_bSkipResource;
	UINT32	m_HeaderPos;

public:
	i3ImageFile(void);
	virtual ~i3ImageFile(void);

	UINT32 GetReadSize(void){ return m_ReadBytes; }

    void	setSkipResource( BOOL bFlag)			{ m_bSkipResource = bFlag;		}
	BOOL	isSkipResource(void)					{ return m_bSkipResource;		}


	i3Texture * Load( const char * pszFileName );
	i3Texture * LoadBMP( const char * pszFileName );
	i3Texture * LoadTGA( const char * pszFileName );
	i3Texture * LoadI3I( const char * pszFileName, i3Texture * pTexture = NULL );
#ifndef I3_WINCE
	i3Texture * LoadPNG( const char * pszFileName );
#endif

	BOOL		Load( const char * pszFileName, i3Texture * pTexture, INT64 offset = 0L);

	BOOL Save( const char * pszFileName, i3Texture * pImage );
	BOOL SaveBMP( const char * pszFileName, i3Texture * pImage );
	BOOL SaveTGA( const char * pszFileName, i3Texture * pImage );
	BOOL SaveI3I( const char * pszFileName, i3Texture * pImage );
	BOOL SavePNG( const char * pszFileName, i3Texture * pImage );

	BOOL LoadBMP( i3Stream * pStream, i3Texture * pTexture );
	BOOL SaveBMP( i3Stream * pStream, i3Texture * pTexture );

	BOOL LoadTGA( i3Stream * pStream, i3Texture * pTexture );
	BOOL SaveTGA( i3Stream * pStream, i3Texture * pTexture );

	BOOL LoadI3I( i3Stream * pStream, i3Texture * pTexture);
	BOOL LoadI3I_2D( i3Stream * pStream, I3I_FILE_HEADER3 * pHeader, i3Texture * pTexture);
	BOOL LoadI3I_Cube( i3Stream * pStream, I3I_FILE_HEADER3 * pHeader, i3TextureCube * pTexture);
	BOOL SaveI3I( i3Stream * pStream, i3Texture * pTexture );
	BOOL SaveI3I_2D( i3Stream * pStream, I3I_FILE_HEADER3 * pHeader, i3Texture * pTexture);
	BOOL SaveI3I_Cube( i3Stream * pStream, I3I_FILE_HEADER3 * pHeader, i3TextureCube * pTexture);
	bool isCubeI3I( const char * pszPath);

	BOOL LoadDDS( i3Stream * pStream, i3Texture * pTexture);
	BOOL SaveDDS( i3Stream * pStream, i3Texture * pTexture);
	bool isCubeDDS( const char * pszPath);

#ifndef I3_WINCE
	BOOL LoadPNG( i3Stream * pStream, i3Texture * pTexture);
	BOOL SavePNG( i3Stream * pStream, i3Texture * pTexture);
#endif

	static UINT32	LoadI3I_Header( i3Stream * pStream, I3I_FILE_HEADER3 * pHeader, char * pszName);
};

#endif
