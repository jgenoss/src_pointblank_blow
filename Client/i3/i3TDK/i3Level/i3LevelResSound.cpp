#include "stdafx.h"
#include "i3LevelResSound.h"

I3_CLASS_INSTANCE( i3LevelResSound);

i3LevelResSound::i3LevelResSound( void)
{
	m_ResType			= I3LV_TYPE_SOUND;
}

i3LevelResSound::~i3LevelResSound( void)
{
	I3_SAFE_RELEASE( m_pSound);
}

void	i3LevelResSound::_UpdateInfo( i3Sound * pSnd)
{	
	m_Format			= pSnd->GetFormat();
	m_nChannels			= pSnd->GetChannels();
	m_nSampleRate		= pSnd->GetSampleRate();
	m_nBitsPerSample	= pSnd->GetBitsPerSample();
	m_nDataSize			= pSnd->GetDataLength();

	m_nBlockAlign		= pSnd->getBlockAlign();
	m_nAvgBytesPerSec	= pSnd->GetAvgBytesPerSec();
	m_Duration			= pSnd->GetDuration();
}

bool	i3LevelResSound::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}

	i3FileStream File;
	i3::pack::SOUND_INFO_HEADER info;
	char szFull[ MAX_PATH];

	getFullPath( szFull);
	
	if( File.Open( szFull, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		return false;
	}

	if( I3SNDUtil_LoadFileInfo( &File, &info) == STREAM_ERR)
	{
		return false;
	}

	{
		m_Format			= info.m_nFormat;
		m_nChannels			= info.m_nChannels;
		m_nSampleRate		= info.m_nSampleRate;
		m_nBitsPerSample	= info.m_nBitsPerSample;
		m_nDataSize			= info.m_nDataSize;

		m_nBlockAlign		= info.m_nBlockAlign;
		m_nAvgBytesPerSec	= info.m_nAvgBytesPerSec;
		m_Duration			= (REAL32)m_nDataSize / info.m_nAvgBytesPerSec;
	}

	File.Close();

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool	i3LevelResSound::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	BeginValidate();
	{
		i3SoundFile		file;
		char szFull[ MAX_PATH];

		getFullPath( szFull);
		i3Sound		*	pSound = file.Load( szFull);

		I3_REF_CHANGE( m_pSound, pSound);
		if( m_pSound == nullptr)
			return false;
	}
	EndValidate();

	m_pSound->SetExternRes( TRUE);

	_UpdateInfo( m_pSound);

	addResState( I3LV_RES_STATE_LOADED);
	
	return true;
}

i3GameRes * i3LevelResSound::CreateGameRes(void)
{
	i3GameResSound * pRes;
	i3SoundPlayInfo * pInfo;

	pInfo = i3SoundPlayInfo::new_object_ref();
	pInfo->setSound( m_pSound);

	pRes = i3GameResSound::new_object();
	pRes->setSound( pInfo);

	return pRes;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace pack
{
	struct RES_SOUND
	{
		UINT8	m_ID[4] = { 'R', 'S', 'S', '1' };
		UINT32	m_Format = 0;
		UINT32	m_nChannels = 0;
		UINT32	m_nSampleRate = 0;
		UINT32	m_nBitsPerSample = 0;
		UINT32	m_nDataSize = 0;

		UINT32	m_nBlockAlign = 0;
		UINT32	m_nAvgBytesPerSec = 0;
		REAL32	m_Duration = 0.0f;
		UINT32	pad[7] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResSound::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::RES_SOUND data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Format			= m_Format;

	data.m_nChannels		= m_nChannels;
	data.m_nSampleRate		= m_nSampleRate;
	data.m_nBitsPerSample	= m_nBitsPerSample;
	data.m_nDataSize		= m_nDataSize;

	data.m_nBlockAlign		= m_nBlockAlign;
	data.m_nAvgBytesPerSec	= m_nAvgBytesPerSec;
	data.m_Duration			= m_Duration;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3LevelResSound::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::RES_SOUND data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Result);
	Result += Rc;

	m_Format			= data.m_Format;

	m_nChannels			= data.m_nChannels;
	m_nSampleRate		= data.m_nSampleRate;		
	m_nBitsPerSample	= data.m_nBitsPerSample;	
	m_nDataSize			= data.m_nDataSize;		

	m_nBlockAlign		= data.m_nBlockAlign;		
	m_nAvgBytesPerSec	= data.m_nAvgBytesPerSec;	
	m_Duration			= data.m_Duration;			

	return Result;
}
