#include "i3GfxType.h"
#include "i3ImageFile.h"
#include "i3Texture.h"
#include "i3TextureMem.h"
#include "i3TextureCube.h"

#if defined( I3G_DX)
#include "i3TextureDX.h"
#elif defined( I3G_XBOX)
#include "i3TextureXbox.h"
#elif defined( I3G_PSP)
#include "i3TexturePSP.h"
#elif defined( I3G_OGLES)
#include "i3TextureOGLES.h"
#endif

#include "i3Base/string/ext/extract_fileext.h"

static bool s_bBGLoadEnable = false;

I3_CLASS_INSTANCE( i3ImageFile);

i3Texture * i3ImageFile::Load( const char * pszFileName, bool bBackgroundLoad /*= false*/ )
{
	i3Texture * pTex;
	char szExt[16];
	bool b2D = true;
	
	i3::extract_fileext(pszFileName, szExt);

	if ( szExt[0] == 0 )
		return false;

	if( i3::generic_is_iequal( szExt, "DDS") )
	{
		b2D = ! isCubeDDS( pszFileName);
	}
	else if( i3::generic_is_iequal( szExt, "I3I") )
	{
		b2D = !isCubeI3I( pszFileName);
	}

	if( b2D)
	{
		pTex = i3Texture::new_object();
		if( bBackgroundLoad)
			pTex->setThreadMode(true);
	}
	else
	{
		pTex = i3TextureCube::new_object();
	}

	if( Load( pszFileName, pTex, 0, bBackgroundLoad && b2D) == false)
	{
		I3_MUST_RELEASE( pTex);
		return nullptr;
	}

	pTex->setThreadMode(false);

    return pTex;	
}

bool i3ImageFile::Load( const char * pszFileName, i3Texture * pTex, INT64 offset, bool bBackgroundLoad)
{
	char szExt[16];
	i3FileStream Stream;
	bool bRv;

	i3::extract_fileext( pszFileName, szExt);
	if (szExt[0] == 0)
		return false;

	if( Stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		i3Log( "i3ImageFile::Load()", "Could not open %s file.", pszFileName);
		return false;
	}

	Stream.SetPosition( (INT32) offset, STREAM_BEGIN);

	if( i3::generic_is_iequal( szExt, "BMP" )  )
	{
		bRv = LoadBMP( &Stream, pTex);
	}
	else if( i3::generic_is_iequal( szExt, "TGA" )  )
	{
		bRv = LoadTGA( &Stream, pTex);
	}
	else if( i3::generic_is_iequal( szExt, "DDS") )
	{
		bRv = LoadDDS( &Stream, pTex);
	}
#ifndef I3_WINCE
	else if( i3::generic_is_iequal( szExt, "PNG" ) )
	{
		bRv = LoadPNG( &Stream, pTex);
	}
#endif
	else
	{
		if( bBackgroundLoad  && IsBGLoadEnable() )
		{
			bRv = BGLoadI3I( &Stream, pTex);
		}
		else
		{
			bRv = LoadI3I( &Stream, pTex);
		}
	}

	if( pTex->hasName() == false)
	{
		pTex->SetName( pszFileName);
	}

	return bRv;
}

i3Texture * i3ImageFile::LoadBMP( const char * pszFileName )
{
	i3FileStream stream;
	i3Texture * pTex;

	if( !stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) )
	{
		i3Log( "i3ImageFile::LoadBMP()", "Could not open %s file.", pszFileName);
		return false;
	}

	pTex = i3Texture::new_object();

	if( LoadBMP( &stream, pTex) == false)
	{
		I3_MUST_RELEASE( pTex);
		i3Log( "i3ImageFile::LoadBMP()", "Could not load %s file.", pszFileName);
		return nullptr;
	}

	return pTex;
}

i3Texture * i3ImageFile::LoadTGA( const char * pszFileName )
{
	i3FileStream stream;
	i3Texture * pTex;

	if( !stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) )
	{
		i3Log( "i3ImageFile::LoadTGA()", "Could not open %s file.", pszFileName);
		return false;
	}

	pTex = i3Texture::new_object();

	if( LoadTGA( &stream, pTex) == false)
	{
		I3_MUST_RELEASE( pTex);
		i3Log( "i3ImageFile::LoadTGA()", "Could not open %s file.", pszFileName);
		return false;
	}

	return pTex;
}

i3Texture * i3ImageFile::LoadI3I( const char * pszFileName, i3Texture * pTexture  )
{
	i3FileStream stream;
	i3Texture * pTex;

	if( !stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) )
	{
		i3Log( "i3ImageFile::LoadI3I()", "Could not open %s file.", pszFileName);
		return false;
	}

	if( pTexture == nullptr )
	{
		pTex = i3Texture::new_object();
	}
	else
	{
		pTex = pTexture;
		I3_SAFE_ADDREF( pTex);
	}

	stream.SetPosition( 0, STREAM_BEGIN);

	if( LoadI3I( &stream, pTex) == false)
	{
		I3_MUST_RELEASE( pTex);
		i3Log( "i3ImageFile::LoadI3I()", "Could not open %s file.", pszFileName);
		stream.Close();
		return nullptr;
	}

	stream.Close();

	return pTex;
}

#ifndef I3_WINCE
i3Texture * i3ImageFile::LoadPNG( const char * pszFileName )
{
	i3FileStream stream;
	i3Texture * pTex;

	if( !stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) )
	{
		i3Log( "i3ImageFile::LoadPNG()", "Could not open %s file.", pszFileName);
		return false;
	}

	pTex = i3Texture::new_object();

	if( LoadPNG( &stream, pTex) == false)
	{
		I3_MUST_RELEASE( pTex);
		i3Log( "i3ImageFile::LoadPNG()", "Could not open %s file.", pszFileName);
		return false;
	}

	return pTex;
}
#endif

bool i3ImageFile::Save( const char * pszFileName, i3Texture * pImage )
{
	char szExt[16];
	
	i3::extract_fileext(pszFileName, szExt);
	if (szExt[0] == 0)
		return false;
	
	if( i3::generic_is_iequal( szExt, "BMP" )  )
	{
		return SaveBMP( pszFileName, pImage );
	}
	else if( i3::generic_is_iequal( szExt, "TGA" ) )
	{
		return SaveTGA( pszFileName, pImage );
	}
	else if( i3::generic_is_iequal( szExt, "I3I" ) )
	{
		return SaveI3I( pszFileName, pImage );
	}
#ifndef I3_WINCE
	else if( i3::generic_is_iequal( szExt, "PNG" ) )
	{
		return SavePNG( pszFileName, pImage );
	}
#endif

	return false;
}

bool i3ImageFile::SaveBMP( const char * pszFileName, i3Texture * pImage )
{
	i3FileStream stream;

	if( !stream.Create( pszFileName, STREAM_WRITE ) )
	{
		i3Log( "i3ImageFile::SaveBMP()", "Could not open %s file.", pszFileName);
		return false;
	}

	return SaveBMP( &stream, pImage );
}

bool i3ImageFile::SaveTGA( const char * pszFileName, i3Texture * pImage )
{
	i3FileStream stream;

	if( !stream.Create( pszFileName, STREAM_WRITE ) )
	{
		i3Log( "i3ImageFile::SaveTGA()", "Could not open %s file.", pszFileName);
		return false;
	}

	return SaveTGA( &stream, pImage );
}

bool i3ImageFile::SaveI3I( const char * pszFileName, i3Texture * pImage )
{
	i3FileStream stream;

	if( !stream.Create( pszFileName, STREAM_WRITE ) )
	{
		i3Log( "i3ImageFile::SaveI3I()", "Could not open %s file.", pszFileName);
		return false;
	}

	return SaveI3I( &stream, pImage );
}

bool i3ImageFile::SaveDDS( const char * pszFileName, i3Texture * pImage )
{
	i3FileStream stream;

	if( !stream.Create( pszFileName, STREAM_WRITE ) )
	{
		i3Log( "i3ImageFile::SaveDDS()", "Could not open %s file.", pszFileName);
		return false;
	}

	return SaveDDS( &stream, pImage );
}


#ifndef I3_WINCE
bool i3ImageFile::SavePNG( const char * pszFileName, i3Texture * pImage )
{
	i3FileStream stream;

	if( !stream.Create( pszFileName, STREAM_WRITE ) )
	{
		i3Log( "i3ImageFile::SavePNG()", "Could not open %s file.", pszFileName);
		return false;
	}

	return SavePNG( &stream, pImage );
}
#endif

void i3ImageFile::SetBGLoadEnable( bool bEnable)
{
	s_bBGLoadEnable = bEnable;
}

bool i3ImageFile::IsBGLoadEnable( void)
{
	return s_bBGLoadEnable;
}
