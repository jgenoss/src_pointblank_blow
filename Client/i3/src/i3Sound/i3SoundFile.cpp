#include "stdafx.h"
#include "i3SoundFile.h"
#include "i3Base/string/ext/extract_fileext.h"

I3_CLASS_INSTANCE( i3SoundFile);

i3Sound * i3SoundFile::Load( const char * pszFileName)
{
	i3Sound * pSound = i3Sound::new_object();

#if defined( I3SND_DSOUND)
	if( !Load( pszFileName, pSound ))
	{
		I3_SAFE_RELEASE( pSound );
		return nullptr;
	}
#elif defined( I3SND_FMOD)
	pSound->Create( (char *) pszFileName);
#endif

	return pSound;
}

UINT32 i3SoundFile::Load( i3Stream * pStream, i3Sound * pSound)
{
#if defined( I3SND_DSOUND)
	i3FileStream * pFileStream = ( i3FileStream *)pStream;

	char * pszFileName = pFileStream->GetFileName();

	char szExt[16];
	if( i3String::SplitFileExt( pszFileName, szExt, 16 ) == 0 )
	{
		return STREAM_ERR;
	}

	if( i3String::Compare( szExt, "WAV" ) == 0 )
	{
		return LoadWav( pStream, pSound);
	}
	else if( i3String::Compare( szExt, "VAG" ) == 0 )
	{
		return LoadVag( pStream, pSound);
	}
	else if( i3String::Compare( szExt, "I3SND") == 0)
	{
		return Loadi3Snd( pStream, pSound);
	}
	
#else
	I3ASSERT_0;
#endif

	return STREAM_ERR;
}

UINT32	  i3SoundFile::Load( const char * pszFileName, i3Sound * pSound)
{
#if defined( I3SND_DSOUND)
	char szExt[16];
	char szName[MAX_PATH];
	i3FileStream Stream;

	if( i3String::SplitFileExt( pszFileName, szExt, 16 ) == 0 )
	{
		return STREAM_ERR;
	}

	if( Stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		i3Log( "i3SoundFile::Load()", "Could not open %s file.", pszFileName);
		return STREAM_ERR;
	}

	i3String::SplitFileName( pszFileName, szName, true);
	pSound->SetName( szName);

	if( i3String::Compare( szExt, "WAV" ) == 0 )
	{
		return LoadWav( &Stream, pSound);
	}
	else if( i3String::Compare( szExt, "VAG" ) == 0 )
	{
		return LoadVag( &Stream, pSound);
	}
	else if( i3String::Compare( szExt, "I3SND") == 0)
	{
		return Loadi3Snd( &Stream, pSound);
	}
#else
	I3ASSERT_0;
#endif

	return STREAM_ERR;
}


i3Sound * i3SoundFile::LoadWav( const char * pszFileName )
{
	i3FileStream File;
	
	if( !File.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD ))
	{
		i3Log( "i3SoundFile::Load()", "Could not open %s file.", pszFileName);
		return nullptr;
	}

	i3Sound * pSound = i3Sound::new_object();

	if( !LoadWav( &File, pSound))
	{
		i3Log( "i3SoundFile::LoadWav()", "Could Read %s file.", pszFileName);
		I3_SAFE_RELEASE( pSound );
		return nullptr;
	}

	return pSound;
}

i3Sound * i3SoundFile::LoadVag( const char * pszFileName )
{
	i3FileStream File;
	
	if( !File.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD ))
	{
		i3Log( "i3SoundFile::Load()", "Could not open %s file.", pszFileName);
		return nullptr;
	}

	i3Sound * pSound = i3Sound::new_object();

	if( !LoadVag( &File, pSound))
	{
		i3Log( "i3SoundFile::LoadVag()", "Could Read %s file.", pszFileName);
		I3_SAFE_RELEASE( pSound );
		return nullptr;
	}

	return pSound;
}

i3Sound * i3SoundFile::Loadi3Snd( const char * pszFileName, i3Sound * pSound)
{
	i3FileStream File;
	
	if( !File.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD ))
	{
		i3Log( "i3SoundFile::Load()", "Could not open %s file.", pszFileName);
		return nullptr;
	}

	if( pSound == nullptr) pSound = i3Sound::new_object();

	if( !Loadi3Snd( &File, pSound))
	{
		i3Log( "i3SoundFile::Loadi3Snd()", "Could Read %s file.", pszFileName);
		I3_SAFE_RELEASE( pSound );
		return nullptr;
	}

	return pSound;
}

///////////////////////////////////////////////////////////////////////////////////
UINT32	  i3SoundFile::Save( i3Stream * pStream, i3Sound * pSound)
{
	I3ASSERT( pSound != nullptr);

	UINT32 Result = 0;
	UINT32 nFormat = pSound->GetFormat();

	switch( nFormat)
	{
	case I3SND_CMN_FORMAT_WAV:
		Result = SaveWav( pStream, pSound );
		break;

	case I3SND_CMN_FORMAT_VAG:
		Result = SaveVag( pStream, pSound );
		break;

	case I3SND_CMN_FORMAT_I3SND:
		Result = Savei3Snd( pStream, pSound);
		break;

	case I3SND_CMN_FORMAT_MP3:
		break;

	default:
		break;
	}

	return Result;
}

UINT32	  i3SoundFile::Save( const char * pszFileName , i3Sound * pSound )
{
	char szExt[16];
	i3::extract_fileext(pszFileName, szExt);
	if (szExt[0] == 0)
		return STREAM_ERR;

//	if( i3String::SplitFileExt( pszFileName, szExt, 16 ) == 0 )
//	{
//		return STREAM_ERR;
//	}

	if( i3::generic_is_iequal( szExt, "WAV" )  )
	{
		return SaveWav( pszFileName, pSound );
	}
	else if( i3::generic_is_iequal( szExt, "VAG" )  )
	{
		return SaveVag( pszFileName, pSound );
	}
	else if( i3::generic_is_iequal( szExt, "I3SND" ) )
	{
		return Savei3Snd( pszFileName, pSound );
	}

	return STREAM_ERR;
}

UINT32	  i3SoundFile::SaveWav( const char * pszFileName , i3Sound * pSound )
{
	UINT32	Result;
	i3FileStream File;

	if(File.Create( pszFileName, STREAM_WRITE ) == false)
	{
		i3Log( "i3SoundFile::SaveWav()", "Could Open %s file.", pszFileName);
		return STREAM_ERR;
	}

	Result = SaveWav( &File, pSound );
	I3_CHKIO( Result);
    
	File.Close();

	return Result;
}

UINT32	  i3SoundFile::SaveVag( const char * pszFileName , i3Sound * pSound )
{
	UINT32			Result;
	i3FileStream	File;

	if(File.Create( pszFileName, STREAM_WRITE ) == false)
	{
		i3Log( "i3SoundFile::SaveVag()", "Could Open %s file.", pszFileName);
		return 0;
	}

	Result = SaveVag( &File, pSound );
	I3_CHKIO( Result);
    
	File.Close();

	return Result;
}
UINT32	  i3SoundFile::Savei3Snd( const char * pszFileName , i3Sound * pSound )
{
	UINT32			Result;
	i3FileStream	File;

	if( File.Create( pszFileName, STREAM_WRITE ) == false)
	{
		i3Log( "i3SoundFile::Savei3Snd()", "Could Open %s file.", pszFileName);
		return 0;
	}

	Result = Savei3Snd( &File, pSound );
	I3_CHKIO( Result);

	File.Close();

	return Result;
}

