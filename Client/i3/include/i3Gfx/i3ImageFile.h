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
	I3_EXPORT_CLASS_DEFINE( i3ImageFile, i3ElementBase);

protected:

	UINT32	m_ReadBytes = 0;
	INT32	m_nPassCount = 0;				// Loading시 품질 조절용
	bool	m_bSkipResource = false;
	UINT32	m_HeaderPos = 0;

public:
	UINT32 GetReadSize(void){ return m_ReadBytes; }

    void	setSkipResource( bool bFlag)			{ m_bSkipResource = bFlag;		}
	bool	isSkipResource(void)					{ return m_bSkipResource;		}


	i3Texture * Load( const char * pszFileName, bool bBackgroundLoad = false );
	i3Texture * LoadBMP( const char * pszFileName );
	i3Texture * LoadTGA( const char * pszFileName );
	i3Texture * LoadI3I( const char * pszFileName, i3Texture * pTexture = nullptr );

#ifndef I3_WINCE
	i3Texture * LoadPNG( const char * pszFileName );
#endif

	bool		Load( const char * pszFileName, i3Texture * pTexture, INT64 offset = 0L, bool bBackgroundLoad = false);

	bool Save( const char * pszFileName, i3Texture * pImage );
	bool SaveBMP( const char * pszFileName, i3Texture * pImage );
	bool SaveTGA( const char * pszFileName, i3Texture * pImage );
	bool SaveI3I( const char * pszFileName, i3Texture * pImage );
	bool SaveDDS( const char * pszFileName, i3Texture * pImage);
	bool SavePNG( const char * pszFileName, i3Texture * pImage );

	bool LoadBMP( i3Stream * pStream, i3Texture * pTexture );
	bool SaveBMP( i3Stream * pStream, i3Texture * pTexture );

	bool LoadTGA( i3Stream * pStream, i3Texture * pTexture );
	bool SaveTGA( i3Stream * pStream, i3Texture * pTexture );

	bool LoadI3I( i3Stream * pStream, i3Texture * pTexture);
	bool LoadI3I_2D( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTexture);
	bool LoadI3I_2D_Level( i3Stream * pStream, UINT32 Width, UINT32 Height, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTex, INT32 level);
	bool LoadI3I_Cube( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3TextureCube * pTexture);
	bool SaveI3I( i3Stream * pStream, i3Texture * pTexture );
	bool SaveI3I_2D( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3Texture * pTexture);
	bool SaveI3I_Cube( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, i3TextureCube * pTexture);
	bool isCubeI3I( const char * pszPath);

	bool	BGLoadI3I( i3Stream * pStream, i3Texture * pTexture);

	bool LoadDDS( i3Stream * pStream, i3Texture * pTexture);
	bool SaveDDS( i3Stream * pStream, i3Texture * pTexture);
	bool isCubeDDS( const char * pszPath);

#ifndef I3_WINCE
	bool LoadPNG( i3Stream * pStream, i3Texture * pTexture);
	bool SavePNG( i3Stream * pStream, i3Texture * pTexture);
#endif

	static UINT32	LoadI3I_Header( i3Stream * pStream, i3::pack::IMAGE_FILE_HEADER3 * pHeader, char * pszName);

	static void		SetBGLoadEnable( bool bEnable);
	static bool		IsBGLoadEnable( void);

public:
	bool	m_bDump = false;

	void	EnableDump( bool bEnable)	{ m_bDump = bEnable; }
};

#endif
