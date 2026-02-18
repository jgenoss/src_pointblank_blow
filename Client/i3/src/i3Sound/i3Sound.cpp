//-----------------------------------------------------------------------------
// File: i3Sound.cpp
//
// Desc: 
//		
// Date: 2003/12/26~		by: Sung Baek, kim
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "i3Sound.h"
#include "i3SoundUtil.h"
#include "i3SoundFile.h"
 
I3_CLASS_INSTANCE( i3Sound);

///////////////////////////////////////////////////////////////////////////////////////////////////////
i3Sound::i3Sound(void)
{
	m_nAvgBytesPerSec	= m_nBlockAlign * m_nSampleRate;
}

i3Sound::~i3Sound(void)
{
	ClearBuffers();

	I3MEM_SAFE_FREE_POINTER( m_pData);
}

bool	i3Sound::Create( UINT32	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize)
{
	I3ASSERT( nFormat < I3SND_CMN_FORMAT_UNKNOWN );

	m_nPersistFormat	= nFormat;

	m_nChannels			= nChannels;
	m_nSampleRate		= nSampleRate;
	m_nBitsPerSample	= nBitsPerSample;
	m_nDataSize			= nSampleSize;

	m_nBlockAlign		= (m_nBitsPerSample>>3) * m_nChannels;
	m_nAvgBytesPerSec	= m_nBlockAlign * m_nSampleRate;

	return true;
}

bool	i3Sound::Create( char * pszPath )
{
	SetName( pszPath );

	return true;
}

void	i3Sound::SetData( char * pData)
{
	if( m_pData != nullptr )
	{
		I3TRACE("사운드의 데이터가 이미 설정되어있습니다. 크기가 변경되었는지 확인해주십시오\n");
		I3MEM_SAFE_FREE_POINTER( m_pData );
	}

	m_pData = pData;
}

REAL32 i3Sound::GetDuration()
{
	return (REAL32) m_nDataSize / m_nAvgBytesPerSec;
}

//2005.12.13 - 헤더파일의 내용만 틀릴뿐 i3snd와 vag파일의 data는 동일합니다. - raja
bool	i3Sound::MakeRuntimeForm(char * pDstBuf, char * pSrcBuf, INT32	nSrcLength )
{
	bool bChanged = false;
	switch( m_nPersistFormat )
	{
		//
		case I3SND_CMN_FORMAT_WAV:
		{
			switch( m_nFormat )
			{
				case I3SND_CMN_FORMAT_I3SND:
				case I3SND_CMN_FORMAT_VAG:
				{
					bChanged = I3SNDUtil_Convert_WAV2VAG( pSrcBuf, nSrcLength, pDstBuf );
				}
				break;

				default:
					I3TRACE("INVALID DESTINATION SOUND FORMAT\n");
					break;
			}
		}
		break;

		//
		case I3SND_CMN_FORMAT_I3SND:
		case I3SND_CMN_FORMAT_VAG:
		{
			switch( m_nFormat )
			{
				case I3SND_CMN_FORMAT_WAV:
				{
					bChanged = I3SNDUtil_Convert_VAG2WAV( pSrcBuf, nSrcLength, pDstBuf );
				}
				break;

				default:
					I3TRACE("INVALID DESTINATION SOUND FORMAT\n");
					break;
			}
		}
		break;

		//
		default:
		{
			I3TRACE("INVALID SOURCE SOUND FORMAT\n");
		}
		break;
	}

	if( bChanged )	return true;
	else			return false;
}

UINT32	i3Sound::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Result = 0, Rc = 0;
	i3SoundFile	File;

	// Named Element
	Result = i3ResourceObject::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = File.Savei3Snd( pResFile->GetStream(), this);
	I3_CHKIO( Rc);
	Result += Rc;
	
	return Result;
}

UINT32	i3Sound::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Result = 0, Rc = 0;

	// Named Element
	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	if( IsLoaded() )
	{
		//I3TRACE( "WARNING : [%s] Multiple Load found!!!!!!!!!!!!!!!!!!!!\n",  __FUNCTION__  );
		return 0;
	}

	i3SoundFile File;
	Rc = File.Loadi3Snd( pResFile->GetStream(), this);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

void i3Sound::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3ResourceObject::OnBuildObjectList( List);

	return ;
}
