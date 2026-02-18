#include "stdafx.h"
#include "i3SoundContext.h" 
#include "i3SoundGlobal.h"

#include "i3Base/itl/vector_util.h"

#if defined( I3SND_FMOD)
#include "fmod.hpp"
#include "fmod.h"
#include "i3SoundContextFMOD.h"
#include "i3SoundFMOD.h"
#include "i3SoundPlayStateFMOD.h"
#include "i3Scene.h"

I3_CLASS_INSTANCE( i3SoundContextFMOD);

static void * F_CALLBACK  i3FMOD_Alloc( UINT32 size, FMOD_MEMORY_TYPE type)
{
	return i3MemAlign( size, 16);
}

static void * F_CALLBACK  i3FMOD_ReAlloc(  void *  ptr, UINT32 size, FMOD_MEMORY_TYPE type)
{
	return i3MemReAlign( ptr, size, 16);
}

static void F_CALLBACK  i3FMOD_Free( void *  ptr, FMOD_MEMORY_TYPE  type)
{
	i3MemFree( ptr);
}

static FMOD_RESULT F_CALLBACK i3FMOD_Open( const char * pszName, int unicode, unsigned int * filesize, void ** pHandle, void ** pUserData)
{
	i3FileStream * pFile;

	pFile = i3FileStream::new_object();

	if( pFile->Open( pszName, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		//I3PRINTLOG(I3LOG_WARN,  "Could not open %s sound file.", pszName);
		return FMOD_ERR_FILE_NOTFOUND;
	}

	*pHandle = pFile;
	*filesize = pFile->GetSize();
	*pUserData = nullptr;

	return FMOD_OK;
}

static FMOD_RESULT F_CALLBACK i3FMOD_Close( void * pHandle, void * pUserData)
{
	i3FileStream * pFile = (i3FileStream *) pHandle;

	pFile->Close();
	I3_MUST_RELEASE( pFile);

	return FMOD_OK;
}

static FMOD_RESULT F_CALLBACK i3FMOD_Read( void * pHandle, void * pBuffer, unsigned int size, unsigned int * pReadBytes, void * pUserData)
{
	i3FileStream * pFile = (i3FileStream *) pHandle;
	UINT32 rc;

	rc = pFile->Read( pBuffer, size);
	if( rc == STREAM_ERR)
	{
		return FMOD_ERR_FILE_EOF;
	}

	*pReadBytes = rc;

	return FMOD_OK;
}

static FMOD_RESULT F_CALLBACK i3FMOD_Seek( void * pHandle, unsigned int pos, void * pUserData)
{
	i3FileStream * pFile = (i3FileStream *) pHandle;

	pFile->SetPosition( pos, STREAM_BEGIN);

	return FMOD_OK;
}

bool i3SoundContextFMOD::Create( I3SND_CONTEXTCREATE_PARAM * pInitParam)
{
	m_hMainThreadID = ::GetCurrentThreadId();

	FMOD_RESULT      result;
    unsigned int     version;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;

	i3SoundContext::Create( pInitParam);

    /*  Create a System object and initialize. */
    result = FMOD::System_Create( &m_pFMODSystem);
    ERRCHECK(result);

	g_pFMODSystem = m_pFMODSystem;
    
    result = m_pFMODSystem->getVersion( &version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        I3PRINTLOG(I3LOG_FATAL,  "Error!  You are using an old version of FMOD %08x.  This program requires %08x", version, FMOD_VERSION);
        return false;
    }

	result = FMOD::Memory_Initialize(nullptr, 0,   i3FMOD_Alloc, i3FMOD_ReAlloc, i3FMOD_Free);
	ERRCHECK( result);
    
    result = m_pFMODSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
	//Driver Caps를 얻지 못한다면....제대로 동작할리가 없다라고 보아효~~
    if( result != FMOD_OK)
	{
		result = m_pFMODSystem->close();
		ERRCHECK( result);

		result = m_pFMODSystem->release();
		ERRCHECK( result);
		
		g_pFMODSystem = nullptr;
		return false;
	}

	#if defined( I3_DEBUG)

	I3TRACE( "[SOUND] * Speaker Mode : ");

	switch( speakermode)
	{
		case FMOD_SPEAKERMODE_RAW		:	I3TRACE( "RAW\n");	break;
		case FMOD_SPEAKERMODE_MONO		:	I3TRACE( "MONO\n");	break;
		case FMOD_SPEAKERMODE_STEREO	:	I3TRACE( "STEREO\n");	break;
		case FMOD_SPEAKERMODE_QUAD		:	I3TRACE( "QUAD\n");	break;
		case FMOD_SPEAKERMODE_SURROUND	:	I3TRACE( "SURROUND\n");	break;
		case FMOD_SPEAKERMODE_5POINT1	:	I3TRACE( "5.1 CHANNELS\n");	break;
		case FMOD_SPEAKERMODE_7POINT1	:	I3TRACE( "7.1 CHANNELS\n");	
			break;
		case FMOD_SPEAKERMODE_PROLOGIC	:	I3TRACE( "PROLOGIC\n");	break;
		default :							I3TRACE( "UNKNOWN\n");	break;
	}
	#endif

    result = m_pFMODSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
    ERRCHECK(result);

	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT,    -1.0f,  1.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT,    1.0f,  1.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER,   0.0f,  1.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_LOW_FREQUENCY,  0.0f,  0.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_BACK_LEFT,     -1.0f, -1.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT,     1.0f, -1.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT,     -1.0f,  0.0f, true);
	m_pFMODSystem->set3DSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT,     1.0f,  0.0f, true);

    if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
    {                                                   /* You might want to warn the user about this. */
        result = m_pFMODSystem->setDSPBufferSize(4096, 10);    /* At 48khz, the latency between issuing an fmod command and hearing it will now be about 213ms. */
        ERRCHECK(result);
    }

	result = m_pFMODSystem->setSoftwareChannels( 48);

    //result = m_pFMODSystem->init( 255, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_SOFTWARE_OCCLUSION | FMOD_INIT_VOL0_BECOMES_VIRTUAL, 0);    /* Replace with whatever channel count and flags you use! */
	result = m_pFMODSystem->init( 255, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_VOL0_BECOMES_VIRTUAL, 0);    /* Replace with whatever channel count and flags you use! */

    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = m_pFMODSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
                
        result = m_pFMODSystem->init( 255, FMOD_INIT_NORMAL, 0); /* Replace with whatever channel count and flags you use! */
        ERRCHECK(result);
    }
	else
	{
		 if( result != FMOD_OK)
		{
			result = m_pFMODSystem->close();
			ERRCHECK( result);

			result = m_pFMODSystem->release();
			ERRCHECK( result);
			
			g_pFMODSystem = nullptr;
			return false;
		}
	}

	//advanced Settings
	{
		FMOD_ADVANCEDSETTINGS adSet;
		i3mem::FillZero( &adSet, sizeof( FMOD_ADVANCEDSETTINGS));
		adSet.cbsize = sizeof( FMOD_ADVANCEDSETTINGS);
		adSet.HRTFMinAngle	= 270.0f;	// default : 180
		adSet.HRTFMaxAngle	= 360.0f;	// default : 360
		adSet.vol0virtualvol= 0.0125f;		// 이 볼륨 이하가되면 virtual로 설정

		result = m_pFMODSystem->setAdvancedSettings( &adSet);
		ERRCHECK( result);
	}
	
	result = m_pFMODSystem->setFileSystem( i3FMOD_Open, i3FMOD_Close, i3FMOD_Read, i3FMOD_Seek, 2048);
	ERRCHECK( result);
	
	return true;
}

void i3SoundContextFMOD::_UpdateListenerParam(void)
{
	if( m_InitParam.m_bHasListener == false)
		return ;
	
	VEC3D * pvecPos = i3Matrix::GetPos( &m_MtxListener);
	VEC3D * pvecAt	= i3Matrix::GetAt( &m_MtxListener);
	VEC3D * pvecUp	= i3Matrix::GetUp( &m_MtxListener);

	FMOD_VECTOR pos, vel, at, up;

	pos.x = pvecPos->x;		
	pos.y = pvecPos->y;
	pos.z = pvecPos->z;

	vel.x = m_vListenerVelocity.x;
	vel.y = m_vListenerVelocity.y;
	vel.z = m_vListenerVelocity.z;

	at.x = -pvecAt->x;
	at.y = -pvecAt->y;
	at.z = -pvecAt->z;

	up.x = pvecUp->x;
	up.y = pvecUp->y;
	up.z = pvecUp->z;

	FMOD_RESULT rv = m_pFMODSystem->set3DListenerAttributes( 0, &pos, &vel, &at, &up);
	ERRCHECK( rv);

	_UpdateMuteState();

	i3SoundContext::_UpdateListenerParam();
}

//현재 플레이중인 3d positional sound의 mute state를 업데이트 한다.
void i3SoundContextFMOD::_UpdateMuteState(void)
{
	for( size_t i = 0; i < m_UsedPlayStateList.size(); ++i)
	{
		i3SoundPlayState * pState = m_UsedPlayStateList[i];
		
		if( pState != nullptr)
		{
			pState->UpdateMuteState();
		}
	}
}

DWORD i3SoundContextFMOD::m_hMainThreadID = 0xFFFFFFFF;

FMOD_RESULT F_CALLBACK  i3SoundContextFMOD::_ChannelEndProc( FMOD_CHANNEL *  channel, FMOD_CHANNEL_CALLBACKTYPE  type, int  command, unsigned int  commanddata1, unsigned int  commanddata2)
{
	FMOD::Channel * pChannel = (FMOD::Channel *) channel;

	i3SoundPlayStateFMOD * pState;

	pChannel->getUserData( (void **) &pState);
	I3ASSERT( pState != nullptr);

	size_t idx;
	i3SoundContextFMOD * pCtx = (i3SoundContextFMOD *) g_pSoundCtx;
	if( pCtx == nullptr)
	{
		return FMOD_OK;
	}

	idx = i3::vu::index_of(pCtx->m_UsedPlayStateList, pState);
	if( idx != pCtx->m_UsedPlayStateList.size() )
	{
		pCtx->m_UsedPlayStateList.erase( pCtx->m_UsedPlayStateList.begin() + idx);
	}

	idx = i3::vu::index_of(pCtx->m_FreePlayStateList, pState);
	if( idx == pCtx->m_FreePlayStateList.size() )
	{
		pCtx->m_FreePlayStateList.push_back( pState);
	}

	pState->Reset();

	return FMOD_OK;
}

void	i3SoundContextFMOD::OnDrive(REAL32 rDeltaSeconds)
{
	i3SoundContext::OnDrive( rDeltaSeconds);

	m_pFMODSystem->update();
	
	//I3TRACE( "%d %d\n", m_UsedPlayStateList.GetCount(), m_FreePlayStateList.GetCount());
}

void i3SoundContextFMOD::setMainVolume( UINT8 nVolume)
{
	i3SoundContext::setMainVolume( nVolume);

	i3SoundPlayState * pState;

	for(size_t i = 0; i < m_UsedPlayStateList.size(); i++)
	{
		pState = m_UsedPlayStateList[i];

		pState->SetVolume( pState->GetVolume());
	}
}

void i3SoundContextFMOD::SetTypeVolume( UINT32 nType, UINT8 nVolume)
{
	i3SoundContext::SetTypeVolume( nType, nVolume);

	i3SoundPlayState * pState;

	for(size_t i = 0; i < m_UsedPlayStateList.size(); i++)
	{
		pState = m_UsedPlayStateList[i];

		if( pState->isSoundOfType( nType) && pState->isPlaying())
			pState->SetVolume( pState->GetVolume());
	}
}

INT32 i3SoundContextFMOD::ChannelGroup_Add( const char * pszChannelName)
{
	FMOD_RESULT rv = FMOD_OK;
	FMOD::ChannelGroup * pNewGroup = nullptr;

	rv = g_pFMODSystem->createChannelGroup( pszChannelName, &pNewGroup);
	ERRCHECK( rv);

	FMOD::ChannelGroup * pMaster;
	rv = g_pFMODSystem->getMasterChannelGroup( &pMaster);
	ERRCHECK( rv);

	rv = pMaster->addGroup( pNewGroup);
	ERRCHECK( rv);

	INT32 nGrpIdx = 0;

	rv = pMaster->getNumGroups( &nGrpIdx);
	ERRCHECK( rv);

	return nGrpIdx - 1;
}

bool i3SoundContextFMOD::ChannelGroup_SetVolume( INT32 idx, INT32 volume)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)GetChannelGroup( idx);
	if( pGroup == nullptr)
		return false;


	REAL32 finalvolume =  MINMAX( 0.0f, (REAL32)volume/100, 1.0f);

	FMOD_RESULT rv = pGroup->setVolume( finalvolume);
	ERRCHECK( rv);

	return true;
}

bool i3SoundContextFMOD::ChannelGroup_Stop( INT32 idx)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)GetChannelGroup( idx);
	if( pGroup == nullptr)
		return false;

	FMOD_RESULT rv = pGroup->stop();
	ERRCHECK( rv);

	return true;
}

bool i3SoundContextFMOD::ChannelGroup_SetPitch(INT32 idx, REAL32 rPitch)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)GetChannelGroup( idx);
	if( pGroup == nullptr)
		return false;

	FMOD_RESULT rv = pGroup->setPitch( rPitch);
	ERRCHECK( rv);

	return true;
}

bool i3SoundContextFMOD::ChannelGroup_Pause( INT32 idx, bool bPause)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)GetChannelGroup( idx);
	if( pGroup == nullptr)
		return false;

	bool isPaused = false;
	FMOD_RESULT rv = pGroup->getPaused( &isPaused);
	ERRCHECK( rv);

	if( isPaused != bPause)
	{
		rv = pGroup->setPaused( bPause);
		ERRCHECK( rv);
	}

	return true;
}

INT32 i3SoundContextFMOD::ChannelGroup_GetNumChannels( INT32 idx)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)GetChannelGroup( idx);
	if( pGroup == nullptr)
		return 0;

	INT32 numChannels = 0;
	pGroup->getNumChannels( &numChannels);

	return numChannels;
}

void* i3SoundContextFMOD::GetChannelGroup( INT32 nGrpIdx)
{
	FMOD::ChannelGroup * pMaster;
	FMOD_RESULT rv = g_pFMODSystem->getMasterChannelGroup( &pMaster);
	ERRCHECK( rv);
	if( rv != FMOD_OK)
	{
		return nullptr;
	}

	//GrpIdx 가 음수이면 마스터그룹을 넘긴다.
	if( nGrpIdx <= -1)
	{
		return pMaster;
	}

	INT32 nGroupCount = 0;
	rv = pMaster->getNumGroups( &nGroupCount);
	ERRCHECK( rv);

	//마스터그룹 하위에 아무런 그룹도 없다면 마스터그룹을 넘긴다.
	//포인트 블랭크에선 채널그룹을 따로 세팅하겠지만, 기본적인경우( 툴)에는
	//따로 세팅하지 않을것이므로, 이 방식으로 합니다.
	if( nGroupCount == 0)
	{
		return pMaster;
	}

	FMOD::ChannelGroup * pGroup;

	rv = pMaster->getGroup( nGrpIdx, &pGroup);
	ERRCHECK( rv);
	if( rv != FMOD_OK)
	{
		return nullptr;
	}

	return (void*)pGroup;
}

void i3SoundContextFMOD::CreateReverb(void)
{
	FMOD_RESULT rv = FMOD_OK;
/*
	FMOD_PLUGINTYPE type = FMOD_PLUGINTYPE_DSP;
	INT32 idx;
	rv = g_pFMODSystem->loadPlugin( "Ambience.dll", &type, &idx);
	ERRCHECK( rv);

	rv = g_pFMODSystem->createDSPByIndex(idx, &m_pDSPReverb); 
	ERRCHECK( rv);

	rv = m_pDSPReverb->setActive( true);
	ERRCHECK( rv);
*/

	/*
	rv = g_pFMODSystem->createDSPByType(FMOD_DSP_TYPE_REVERB, &m_pDSPReverb); 
	ERRCHECK( rv);
	
	rv = m_pChannelGroup[I3SND_CHANNEL_BATTLE]->addDSP( m_pDSPReverb);
	ERRCHECK( rv);

	rv = m_pDSPReverb->setActive( true);
	ERRCHECK( rv);
	*/
	rv = g_pFMODSystem->createReverb( &m_pReverb);
	ERRCHECK( rv);

	FMOD_VECTOR vPos;
	REAL32	min, max;

	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;

	min = 5.0f;
	max = 10.0f;

	m_pReverb->set3DAttributes( &vPos, min, max);

	FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_BATHROOM;
	rv = m_pReverb->setProperties( &prop);
	ERRCHECK( rv);

	m_pReverb->setActive( true);
	/*
	rv = m_pDSPReverb->setParameter( 0, 1.0f);
	ERRCHECK( rv);

	rv = m_pDSPReverb->setParameter( 1, 0.0f);
	ERRCHECK( rv);

	rv = m_pDSPReverb->setParameter( 4, 0.5f);
	ERRCHECK( rv);

	rv = m_pDSPReverb->setParameter( 5, 0.1f);
	ERRCHECK( rv);
	*/
	/*
	{
		index : 0, name:Roomsize, label: , description: Roomsize.  0.0 to 1.0.  Default = 0.5, min: 0.000000, max: 1.000000
		index : 1, name:Damp, label: , description: Damp.  0.0 to 1.0.  Default = 0.5, min: 0.000000, max: 1.000000
		index : 2, name:Wet, label: , description: Wet mix.  0.0 to 1.0.  Default = 0.33, min: 0.000000, max: 1.000000
		index : 3, name:Dry, label: , description: Dry mix.  0.0 to 1.0.  Default = 0.66, min: 0.000000, max: 1.000000
		index : 4, name:Width, label: , description: Width.  0.0 to 1.0.  Default = 1.0, min: 0.000000, max: 1.000000
		index : 5, name:Mode, label: , description: Mode.  0, 1.  Default = 0, min: 0.000000, max: 1.000000
	}*/
	/*
	{
		INT32 nCnt;
		char name[MAX_PATH];
		char label[MAX_PATH]; 
		char description[MAX_PATH];
		char valuestring[MAX_PATH];
		REAL32	min;
		REAL32	max;
		REAL32  value;

		rv = m_pDSPReverb->getNumParameters( &nCnt);
		ERRCHECK( rv);

		for( INT32 i = 0;i < nCnt; ++i)
		{
			m_pDSPReverb->getParameterInfo( i, name, label, description, MAX_PATH -1, &min, &max);
			m_pDSPReverb->getParameter( i, &value, valuestring, MAX_PATH -1);
			I3TRACE("index : %d, name:%s, label: %s, description: %s, min: %f, max: %f, value: %f, valuestring: %s\n", i, name, label, description, min, max, value, valuestring);
		}
	}*/
}

i3::string i3SoundContextFMOD::ReportPerformance(void)
{
	char szPerf[256];
	
	REAL32 dsp, stream, update, total;
	INT32 hwch, swch, playch, curalloced, maxalloced, mutecnt;

	m_pFMODSystem->getCPUUsage( &dsp, &stream, &update, &total);
	m_pFMODSystem->getHardwareChannels( nullptr, nullptr, &hwch);
	m_pFMODSystem->getSoftwareChannels( &swch);
	m_pFMODSystem->getChannelsPlaying( &playch);
	FMOD::Memory_GetStats( &curalloced, &maxalloced);

	mutecnt = 0;
	for( size_t i = 0; i < m_UsedPlayStateList.size(); ++i)
	{
		i3SoundPlayState * pState = m_UsedPlayStateList[i];
		if( !pState->isFree())
		{
			if( pState->isMuted() || pState->isPaused())
			{
				mutecnt++;
			}
		}
	}

	sprintf( szPerf, "----------- Sound -------------\n CPU:%.1f  STREAM:%.1f  UPDATE:%.1f  TOTAL:%.1f\n Channels - PLAY:%d  HW:%d  SW:%d MUTE: %d\n MEM_USAGE: curalloc: %d, maxalloc: %d\n",
		dsp, stream, update, total,
		playch, hwch, swch, mutecnt, curalloced, maxalloced);

	return szPerf;
}


#endif