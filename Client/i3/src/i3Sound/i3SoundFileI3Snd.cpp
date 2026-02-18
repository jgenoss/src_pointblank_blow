#include "stdafx.h"
#include "i3SoundUtil.h"
#include "i3SoundFile.h"
#include "i3SoundFileI3Snd.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

UINT32	  i3SoundFile::Loadi3Snd( i3Stream * pStream, i3Sound * pSound )
{
	I3ASSERT( pStream != nullptr );
	I3ASSERT( pSound != nullptr );

	UINT32	Result = 0, Rc;
	i3::pack::SOUND_I3SND_HEADER	i3SndHeader;

	Result = pStream->Read( &i3SndHeader, sizeof( i3::pack::SOUND_I3SND_HEADER ));
	I3_CHKIO( Result);
    
//	if( i3String::NCompare( (char*)i3SndHeader.m_ID, "SNF1", 4) != 0)
	if (i3::generic_is_niequal((char*)i3SndHeader.m_ID, "SNF1", 4) == false)
	{
		I3TRACE("i3SoundFile::Loadi3Snd() : Not a I3SND File\n");
		return STREAM_ERR;
	}

	i3SoundFile File;

	switch( i3SndHeader.m_nNativeFormat)
	{
	case	I3SND_CMN_FORMAT_WAV:
		{
			Rc =  File.LoadWav( pStream, pSound);
			I3_CHKIO( Rc);
			Result += Rc;
		}
		break;

	case	I3SND_CMN_FORMAT_VAG:
		{
			Rc = File.LoadVag( pStream, pSound);
			I3_CHKIO( Rc);
			Result += Rc;
		}
		break;
	default:
		break;
	}

	return Result;
}

UINT32	  i3SoundFile::Savei3Snd( i3Stream * pStream, i3Sound * pSound )
{
	UINT32 Result, Rc;
	//기본적으로 I3SND파일의 데이터는 VAG파일과 같습니다.헤더만 다름
	i3::pack::SOUND_I3SND_HEADER i3SndHeader;
	
	i3SndHeader.m_nNativeFormat = pSound->GetPersistFormat();

	Result = pStream->Write( &i3SndHeader, sizeof( i3SndHeader ));
	I3_CHKIO( Result);

	i3SoundFile File;

	switch( i3SndHeader.m_nNativeFormat)
	{
	case I3SND_CMN_FORMAT_WAV:
		{
			Rc = File.SaveWav( pStream, pSound);
			I3_CHKIO( Rc);
			Result += Rc;
		}
		break;
	case I3SND_CMN_FORMAT_VAG:
		{
			Rc = File.SaveVag( pStream, pSound);
			I3_CHKIO( Rc);
			Result += Rc;
		}
		break;
	default:
		break;
	}

	return Result;
}
