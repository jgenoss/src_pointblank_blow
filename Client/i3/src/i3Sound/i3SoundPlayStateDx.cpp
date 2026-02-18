#include "stdafx.h"

#ifdef I3SND_DSOUND

#include "i3SoundDx.h"
#include "i3SoundPlayStateDx.h"
#include "i3SoundContext.h"
#include "i3SoundGlobal.h"

#include "i3Input/dxerr.h"

UINT32	i3SoundPlayStateDx::m_nGlobalRequest	= I3SND_RT_REQ_NONE;


I3_CLASS_INSTANCE( i3SoundPlayStateDx);

//
//LPDIRECTSOUNDFXCHORUS8      i3SoundPlayStateDx::m_lpChorus		= nullptr;
//LPDIRECTSOUNDFXCOMPRESSOR8  i3SoundPlayStateDx::m_lpCompressor	= nullptr;
//LPDIRECTSOUNDFXDISTORTION8  i3SoundPlayStateDx::m_lpDistortion	= nullptr;
//LPDIRECTSOUNDFXECHO8        i3SoundPlayStateDx::m_lpEcho		= nullptr;
//LPDIRECTSOUNDFXFLANGER8     i3SoundPlayStateDx::m_lpFlanger		= nullptr;
//LPDIRECTSOUNDFXGARGLE8      i3SoundPlayStateDx::m_lpGargle		= nullptr;
//LPDIRECTSOUNDFXPARAMEQ8     i3SoundPlayStateDx::m_lpParamEq		= nullptr;
//LPDIRECTSOUNDFXI3DL2REVERB8 i3SoundPlayStateDx::m_lpReverb		= nullptr;
//
//DSFXChorus                  i3SoundPlayStateDx::m_paramsChorus;
//DSFXCompressor              i3SoundPlayStateDx::m_paramsCompressor;
//DSFXDistortion              i3SoundPlayStateDx::m_paramsDistortion;
//DSFXEcho                    i3SoundPlayStateDx::m_paramsEcho;
//DSFXFlanger                 i3SoundPlayStateDx::m_paramsFlanger;
//DSFXGargle                  i3SoundPlayStateDx::m_paramsGargle;
//DSFXParamEq                 i3SoundPlayStateDx::m_paramsParamEq;
//DSFXI3DL2Reverb             i3SoundPlayStateDx::m_paramsReverb;

//DSEFFECTDESC                i3SoundPlayStateDx::m_rgFxDesc[I3SND_CMN_FX_COUNT];
//const GUID *                i3SoundPlayStateDx::m_rgRefGuids[I3SND_CMN_FX_COUNT];
//LPVOID *                    i3SoundPlayStateDx::m_rgPtrs[I3SND_CMN_FX_COUNT];
//bool                        i3SoundPlayStateDx::m_rgLoaded[I3SND_CMN_FX_COUNT];
//DWORD                       i3SoundPlayStateDx::m_dwNumFX		= 0;

//#define PROFILE_PLAY

static bool DO_AND_CHECK_ERR( HRESULT hr, const char * pszString)
{
	if( FAILED( hr))
	{
		DXTRACE_ERR( pszString, hr);

		return false;
	}

	return true;
}

static i3Mutex *			g_pSndPlayMutex = nullptr;
#define I3SND_ENTER_CRITICAL() g_pSndPlayMutex->Enter();
#define I3SND_LEAVE_CRITICAL() g_pSndPlayMutex->Leave();

i3SoundPlayStateDx::i3SoundPlayStateDx(void)
{
	m_lpDSBuffer	= nullptr;
	m_p3DBuffer		= nullptr;

	m_nRequest		= I3SND_RT_REQ_NONE;
	m_nVolume		= 100;
	m_nFinalVolume	= 100;

	m_lpReverb		= nullptr;
	i3mem::FillZero( &m_rgFxDesc, sizeof( DSEFFECTDESC ));
	
	m_rgRefGuids	= nullptr;

	m_CreateFlag	= 0;
	m_pStreamThread = nullptr;

	m_pNext				= nullptr;
	m_pPrev				= nullptr;

	m_pBufferInfo		= nullptr;

	if( g_pSndPlayMutex == nullptr)
		g_pSndPlayMutex = i3Mutex::new_object();
}	

i3SoundPlayStateDx::~i3SoundPlayStateDx(void)
{
	m_lpDSBuffer = nullptr;

	I3_SAFE_RELEASE( m_pNext);
}

void i3SoundPlayStateDx::SetNext( i3SoundPlayStateDx *pState)
{
	I3_REF_CHANGE( m_pNext, pState);

	if( m_pNext != nullptr)
	{
		m_pNext->SetPrev( this);
	}
}

void i3SoundPlayStateDx::SetPrev( i3SoundPlayStateDx * pState)
{
	m_pPrev = pState;
}


void i3SoundPlayStateDx::GetPlayPointer( WCHAR32 * pPointer)
{ 
	if( m_lpDSBuffer == nullptr)
	{
		*pPointer = 0;
		I3PRINTLOG(I3LOG_WARN,  "m_lpDSBuffer == nullptr");
	}
	else
	{
		I3SND_ENTER_CRITICAL();
		DO_AND_CHECK_ERR( m_lpDSBuffer->GetCurrentPosition( pPointer, nullptr), "GetCurrentPosition()");
		I3SND_LEAVE_CRITICAL();
	}
}

bool i3SoundPlayStateDx::Replay( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat)
{
	I3ASSERT( pSoundInfo != nullptr);

	if( m_pInfo == nullptr)
		return false;

	if( isPlaying() && (pSoundInfo == m_pInfo))
	{
		if( !_UpdateLocationParam( pSoundInfo, pMat))
		{
			I3TRACE("!_UpdateLocationParam( pSoundInfo, pMat)\n");
			return false;
		}

		if( m_pBufferInfo == nullptr || m_pBufferInfo->isStopping)
		{
			I3TRACE("m_pBufferInfo == nullptr || m_pBufferInfo->isStopping\n");
			return false;
		}

		if( _IsValidPlayReq())
		{
			setRequest( I3SND_RT_REQ_REPLAY);
			return true;
		}
		else
		{
			I3TRACE("!_IsValidPlayReq()\n");
			return false;
		}
	}

	I3TRACE("!isPlaying() || (pSoundInfo != m_pInfo)\n");
	return false;
}

bool i3SoundPlayStateDx::CMD_Replay(void)
{
	bool bRet = false;

	setRequest( I3SND_RT_REQ_NONE);

	//I3SND_ENTER_CRITICAL();
	HRESULT hr = m_lpDSBuffer->SetCurrentPosition(0);
	//I3SND_LEAVE_CRITICAL();

	if( SUCCEEDED( hr))
	{
		if((m_CreateFlag & DSBCAPS_CTRL3D) != 0)
		{
			CMD_UpdatePos();
		}

		DWORD dwStatus;
		m_lpDSBuffer->GetStatus( &dwStatus);

		if(( dwStatus & DSBSTATUS_PLAYING) == 0)
		{
			I3SND_ENTER_CRITICAL();
			DO_AND_CHECK_ERR( m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag), "m_lpDSBuffer->Play()");
			I3SND_LEAVE_CRITICAL();
		}

		SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

		bRet = true;
	}
	else
	{
		DXTRACE_ERR( "!!!!!!!!!!!Replay!!!!!!!!", hr);
	}

/*
	DWORD dwStatus;
	hr = m_lpDSBuffer->GetStatus( &dwStatus);
	if( SUCCEEDED( hr))
	{
		if( dwStatus & DSBSTATUS_PLAYING)
		{
			I3TRACE("1_PLAYING");
		}

		if( dwStatus & DSBSTATUS_LOCHARDWARE)
		{
			I3TRACE("2_HARD");
		}
		else if( dwStatus & DSBSTATUS_LOCSOFTWARE)
		{
			I3TRACE("2_SOFT");
		}

		if( dwStatus & DSBSTATUS_LOOPING)
		{
			I3TRACE("3_LOOP");
		}

		if( dwStatus & DSBSTATUS_TERMINATED)
		{
			I3TRACE("4_TERMINATED");
		}

		if(( dwStatus & 0xFF) == 0)
		{
			I3TRACE("5_NONESTATE");
		}

		I3TRACE("\n");
	}
	else
	{
		DXTRACE_ERR("getstat", hr);
	}
*/
	return bRet;
}

static UINT32	_GetSoundCreateFlag( i3SoundPlayInfo * pPlayInfo)
{
	UINT32 nCreateFlag = DSBCAPS_CTRLVOLUME;

	if( pPlayInfo->Get3DMode() != I3SND_RT_3DMODE_DISABLE)
	{
		nCreateFlag |= DSBCAPS_CTRL3D;

		if( pPlayInfo->isMuteAtMaxDistance())
		{
			nCreateFlag |= DSBCAPS_MUTE3DATMAXDISTANCE;
		}

	//	nCreateFlag	|=	DSBCAPS_LOCSOFTWARE;
	}/*
	else
	{
		if( g_pSoundCtx->getState() & I3SND_CMN_USEACCELERATION)
		{
			nCreateFlag	|=	DSBCAPS_LOCDEFER;
		}
	}*/

	nCreateFlag	|=	DSBCAPS_LOCSOFTWARE;

	/*
	if( g_pSoundCtx->GetFXEnable() && (pPlayInfo->GetSoundType() == I3SND_PROP_TYPE_EFFECT) && (pPlayInfo->getI3DL2ReverbPreset() != -1))
	{
		nCreateFlag	|= DSBCAPS_CTRLFX;
	}
	*/

	if( pPlayInfo->GetSoundType() == I3SND_PROP_TYPE_BGM)
	{
		nCreateFlag |= DSBCAPS_GLOBALFOCUS;
	}

	return nCreateFlag;
}

bool i3SoundPlayStateDx::Play( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat)
{
	if( !_UpdateLocationParam( pSoundInfo, pMat))
	{
		setRequest( I3SND_RT_REQ_NONE);
		return false;
	}

	SetSoundPlayInfo( pSoundInfo);

	if( _IsValidPlayReq())
	{
		addRequest( I3SND_RT_REQ_PLAY);
		return true;
	}

	I3_SAFE_RELEASE( m_pInfo);

	setRequest( I3SND_RT_REQ_NONE);

	return false;
}


bool i3SoundPlayStateDx::CMD_Play(void)
{
	HRESULT hr;

	I3ASSERT( g_pSoundCtx != nullptr);
	I3ASSERT( g_pDS != nullptr);
	I3ASSERT( m_pInfo != nullptr);

	#if defined( PROFILE_PLAY)
		INT64 localtime;
		QueryPerformanceCounter( (LARGE_INTEGER*)&localtime);

		//I3TRACE("%s :\n", m_pInfo->getSound()->GetName());
	#endif

	i3SoundDx * pSoundDx = (i3SoundDx *) m_pInfo->getSound();
	if( pSoundDx == nullptr)
	{
		removeRequest( I3SND_RT_REQ_PLAY);
		return false;
	}

	/////////////////////////////////////////////
	m_CreateFlag = _GetSoundCreateFlag( m_pInfo);
	/////////////////////////////////////////////

//	I3TRACE("CREATEFLAG: %X\n", m_CreateFlag);
	m_pBufferInfo = pSoundDx->GetSoundBuffer( m_CreateFlag);
	if( m_pBufferInfo == nullptr)
	{
		removeRequest( I3SND_RT_REQ_PLAY);
		return false;
	}

	LPDIRECTSOUNDBUFFER lpSound = m_pBufferInfo->lpBuffer;
	if( lpSound != nullptr)
	{
		hr = lpSound->QueryInterface( IID_IDirectSoundBuffer8, (void**)&m_lpDSBuffer);
		if( FAILED( hr))
		{
			DXTRACE_ERR( "ueryInterface( IID_IDirectSoundBuffer)", hr);

			removeRequest( I3SND_RT_REQ_PLAY);

			return false;
		}
	}
	else
	{
		I3TRACE("SOUNDBUFFER IS nullptr\n");

		removeRequest( I3SND_RT_REQ_PLAY);

		return false;
	}

	///////////////////////
	if( isStreamingSound())
	{
		_CreateStream();
	}

	//reset arg
	{
		m_dwPlayPointer		= 0;
		m_nPlayFlag			= 0;
		m_dwPriority		= 0;
	}

	if( m_CreateFlag & DSBCAPS_CTRLFX)
	{
		SetReverb();
	}

	if( m_CreateFlag & DSBCAPS_CTRL3D)
	{
		Set3DParams();

		if( m_CreateFlag & DSBCAPS_LOCDEFER)
		{
			DSCAPS cap;
			i3mem::FillZero( &cap, sizeof( DSCAPS));
			cap.dwSize  = sizeof( DSCAPS);

			if( SUCCEEDED( hr = g_pDS->GetCaps( &cap)))
			{
				if( cap.dwFreeHw3DStaticBuffers > 0)
				{
					m_nPlayFlag |= DSBPLAY_TERMINATEBY_PRIORITY | DSBPLAY_TERMINATEBY_TIME;
				}
			}
			else
			{
				I3TRACE("failed getting caps\n");
			}
		}
	}
		
	SetVolume( m_nVolume);

	//스트리밍인경우 looping enable, soundplayinfo의 정보가 oneshot인경우라도 streamming은 loopenable
    if( isLoop() || isStreamingSound())
	{
		m_nPlayFlag		|= DSBPLAY_LOOPING;
	}

	//현재 사운드 버퍼는 DSBCAPS_LOCDEFER를 가지고 생성되었습니다.( 이펙트의 경우에만)
	//hardware buffer가 사용가능하지 않은경우, voicemanagement flag가 들어간상태로 플레이하면 실패하게됩니다.
	//위의 이유로 실패한경우는 voiceManagement Flag를 제거하고 software mode로 다시 플레이를 시도합니다.
	hr = m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag);

	if( FAILED( hr))
	{
		DXTRACE_ERR("Play()", hr );

		Stop();

		return false;
	}

	//I3TRACE("%s : %p, %s pos_x: %f y: %f z : %f\n", m_pInfo->getSound()->GetName(), m_pInfo, ( m_CreateFlag & DSBCAPS_CTRL3D) ? "Positional" : "Listener", getX( &m_vecPos), getY( &m_vecPos), getZ( &m_vecPos));

	#if defined( PROFILE_PLAY)
		INT64	endplay, res;
		QueryPerformanceCounter( (LARGE_INTEGER*)&endplay);
		QueryPerformanceFrequency( (LARGE_INTEGER*)&res);

		REAL32 localelapse = (REAL32)((endplay - localtime) * 1000.0f/res);
		I3TRACE("PLAY: %04f", localelapse);
	

		DWORD dwStatus;
		hr = m_lpDSBuffer->GetStatus( &dwStatus);
		if( SUCCEEDED( hr))
		{
			if( dwStatus & DSBSTATUS_PLAYING)
			{
				I3TRACE("1_PLAYING");
			}

			if( dwStatus & DSBSTATUS_LOCHARDWARE)
			{
				I3TRACE("2_HARD");
			}
			else if( dwStatus & DSBSTATUS_LOCSOFTWARE)
			{
				I3TRACE("2_SOFT");
			}

			if( dwStatus & DSBSTATUS_LOOPING)
			{
				I3TRACE("3_LOOP");
			}

			if( dwStatus & DSBSTATUS_TERMINATED)
			{
				I3TRACE("4_TERMINATED");
			}

			if(( dwStatus & 0xFF) == 0)
			{
				I3TRACE("5_NONESTATE");
			}

			I3TRACE("\n");
		}
		else
		{
			DXTRACE_ERR("getstat", hr);
		}

	#endif
	if( isStreamingSound())
	{
		_StartStream();
	}

	SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

	removeRequest( I3SND_RT_REQ_PLAY);

	++m_nCount;
/*
	VEC3D vLength;
	i3Vector::Sub( &vLength, &m_vecPos, i3Matrix::GetPos( g_pSoundCtx->GetListnerMatrix()));

	I3TRACE("name: %s, %s, l: %f, x: %f, y : %f, z : %f\n", m_pInfo->getSound()->GetName(), (m_CreateFlag & DSBCAPS_CTRL3D)? "Positional":"Listener" , i3Vector::Length( &vLength), getX( &m_vecPos), getY( &m_vecPos), getZ( &m_vecPos));
*/
	return true;
}

bool i3SoundPlayStateDx::Stop( bool bForce)
{
	if( isFree())
		return true;

	if( bForce)
	{
		CMD_Stop();
	}
	else
	{
		addRequest( I3SND_RT_REQ_STOP);
	}

	return true;
}

bool i3SoundPlayStateDx::CMD_Stop(void)
{
	#if defined( PROFILE_PLAY)
		INT64 localtime;
		QueryPerformanceCounter( (LARGE_INTEGER*)&localtime);
	#endif

	if( (m_pBufferInfo != nullptr) && (m_lpDSBuffer != nullptr))
	{
		if( isStreamingSound())
		{
			_StopStream();
		}

		I3SND_ENTER_CRITICAL();
		{
			if( ! DO_AND_CHECK_ERR( m_lpDSBuffer->Stop(), "Stop()"))
			{
				I3SND_LEAVE_CRITICAL();

				return false;
			}

			DO_AND_CHECK_ERR( m_lpDSBuffer->SetCurrentPosition( 0), "SetCurrentPosition(0)");
		}
		I3SND_LEAVE_CRITICAL();

		if( m_CreateFlag & DSBCAPS_CTRL3D)
		{
			I3_SAFE_RELEASE( m_p3DBuffer);
		}	

		if( m_CreateFlag & DSBCAPS_CTRLFX)
		{
			DisableAllFX();
		}

		//Release하지 않습니다.
		m_lpDSBuffer	= nullptr;
		m_nVolume		= -1;

		m_pBufferInfo->isUsed		= false;
		m_pBufferInfo->isStopping	= false;

		m_pBufferInfo	= nullptr;

		--m_nCount;
	}

	Reset();

	setRequest( I3SND_RT_REQ_NONE);

	#if defined( PROFILE_PLAY)
		INT64	endplay, res;
		QueryPerformanceCounter( (LARGE_INTEGER*)&endplay);
		QueryPerformanceFrequency( (LARGE_INTEGER*)&res);

		REAL32 localelapse = (REAL32)((endplay - localtime) * 1000.0f/res);
		if( m_pInfo != nullptr)
		{
			I3TRACE("%s : ", m_pInfo->getSound()->GetName());
		}

		I3TRACE("Stop Elapse: %04f\n", localelapse);
	#endif
	
	return true;
}

/////////////////////////////////////////////
// Pause
bool i3SoundPlayStateDx::Pause(void)
{
	if( isPaused())
		return true;

	addRequest( I3SND_RT_REQ_PAUSE);

	return true;
}

bool i3SoundPlayStateDx::CMD_Pause( void)
{
	bool bRet = false;

	removeRequest( I3SND_RT_REQ_PAUSE);
	if( m_lpDSBuffer != nullptr)
	{
		I3SND_ENTER_CRITICAL();
		{
			if( DO_AND_CHECK_ERR( m_lpDSBuffer->Stop(), "Pause()"))
			{
				AddState( I3SND_RT_PLAYSTATE_PAUSE );

				bRet = true;
			}
		}
		I3SND_LEAVE_CRITICAL();
	}

	return bRet;
}

//////////////////////////////////////////////////////
// Resume
bool i3SoundPlayStateDx::Resume(void)
{
	if( !isPaused())
		return true;

	addRequest( I3SND_RT_REQ_RESUME);

	return true;
}

bool i3SoundPlayStateDx::CMD_Resume(void)
{
	bool bRet = false;

	removeRequest( I3SND_RT_REQ_RESUME);

	if( m_lpDSBuffer != nullptr)
	{
		I3SND_ENTER_CRITICAL();
		{
			if( DO_AND_CHECK_ERR( m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag), "Resume()"))
			{
				RemoveState( I3SND_RT_PLAYSTATE_PAUSE );

				bRet = true;
			}
		}
		I3SND_LEAVE_CRITICAL();
	}

	return bRet;
}

//////////////////////////////////////////////////////////
//
bool i3SoundPlayStateDx::SetVolume(INT32 nVolume)
{
	m_nVolume = nVolume;

	addRequest( I3SND_RT_REQ_VOLUME);

	return true;
}

bool i3SoundPlayStateDx::CMD_SetVolume(void)
{
	bool bRet = false;
	static INT32 slightvolumedifference = 1;

	removeRequest( I3SND_RT_REQ_VOLUME);

	if(( m_lpDSBuffer != nullptr)&& ((m_CreateFlag & DSBCAPS_CTRLVOLUME) != 0))
	{
		INT32 nFinalVolume = _calcCtxVolume();

		INT32 DSVolume = (INT32)(2000.0f * log10( nFinalVolume * 0.01f));

		DSVolume	= MINMAX( DSBVOLUME_MIN, DSVolume + slightvolumedifference, DSBVOLUME_MAX);

		if( nFinalVolume <= 0)
		{
			DSVolume =  DSBVOLUME_MIN + slightvolumedifference;
		}

		slightvolumedifference++;
		if( slightvolumedifference >= 30)
			slightvolumedifference = 1;

		DSVolume	= MINMAX( DSBVOLUME_MIN, DSVolume, DSBVOLUME_MAX);

		I3SND_ENTER_CRITICAL();
		{
			bRet = DO_AND_CHECK_ERR( m_lpDSBuffer->SetVolume( DSVolume), "SetVolume()");
		}
		I3SND_LEAVE_CRITICAL();
	}
		
	return bRet;
}

////////////////////////////////////////////////
//
bool i3SoundPlayStateDx::SetPitch( INT32 nPitch)
{
	if( (m_pInfo != nullptr) && (nPitch < 0))
	{
		nPitch = I3SND_STANDARD_PITCH; 
	}

	m_nPitch = nPitch;

	addRequest( I3SND_RT_REQ_PITCH);

	return true;
}

bool i3SoundPlayStateDx::CMD_SetPitch(void)
{
	bool bRet = false;

	removeRequest( I3SND_RT_REQ_PITCH);

	if(( m_lpDSBuffer != nullptr) && ((m_CreateFlag & DSBCAPS_CTRLFREQUENCY) == 0))
	{
		I3SND_ENTER_CRITICAL();
			bRet = DO_AND_CHECK_ERR( m_lpDSBuffer->SetFrequency( m_nPitch), "SetFrequency()");
		I3SND_LEAVE_CRITICAL();
	}

	return bRet;
}

void i3SoundPlayStateDx::Set3DParams()
{
	if( m_lpDSBuffer == nullptr)				return;
	if((m_CreateFlag & DSBCAPS_CTRL3D) == 0) return;

 	i3SoundPlayState::Set3DParams();

	HRESULT hr = E_FAIL;

	I3SND_ENTER_CRITICAL();
	{
		hr = m_lpDSBuffer->QueryInterface( IID_IDirectSound3DBuffer8, (void**)&m_p3DBuffer);
	}
	I3SND_LEAVE_CRITICAL();

	if( SUCCEEDED( hr))
	{
		I3ASSERT( m_p3DBuffer != nullptr);

		m_dsBufferParams.dwSize = sizeof( DS3DBUFFER);

		I3SND_ENTER_CRITICAL();
		{
			m_p3DBuffer->GetAllParameters(&m_dsBufferParams);
		}
		I3SND_LEAVE_CRITICAL();

		if( isListnerSound())
		{
			I3SND_ENTER_CRITICAL();
			{
				m_p3DBuffer->SetMode( DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
				//m_p3DBuffer->SetMode( DS3DMODE_HEADRELATIVE, DS3D_DEFERRED);
			}
			I3SND_LEAVE_CRITICAL();

			m_dsBufferParams.vPosition.x = 0.0f;
			m_dsBufferParams.vPosition.y = 0.0f;
			m_dsBufferParams.vPosition.z = 0.0f;

			m_dsBufferParams.vVelocity.x = 0.0f;
			m_dsBufferParams.vVelocity.y = 0.0f;
			m_dsBufferParams.vVelocity.z = 0.0f;

			I3_SAFE_RELEASE( m_p3DBuffer);
			return;
		}
/*
		I3TRACE("3dparam: listenerpos: ");
		i3Vector::Dump( i3Matrix::GetPos( g_pSoundCtx->GetListnerMatrix()));
		I3TRACE("\t pos: ");
		i3Vector::Dump( &m_vecPos);
*/
		m_dsBufferParams.flMinDistance = m_pInfo->getMinDistance();
		m_dsBufferParams.flMaxDistance = m_pInfo->getMaxDistance();

		m_dsBufferParams.vPosition.x = getX( &m_vecPos) + I3_EPS;
		m_dsBufferParams.vPosition.y = getY( &m_vecPos) + I3_EPS;
		m_dsBufferParams.vPosition.z = getZ( &m_vecPos) + I3_EPS;

		m_dsBufferParams.vVelocity.x = getX( &m_vVelocity);
		m_dsBufferParams.vVelocity.y = getY( &m_vVelocity);
		m_dsBufferParams.vVelocity.z = getZ( &m_vVelocity);

		m_dsBufferParams.vConeOrientation.x = getX(&m_vDirection);
		m_dsBufferParams.vConeOrientation.y = getY(&m_vDirection);
		m_dsBufferParams.vConeOrientation.z = getZ(&m_vDirection);

		m_dsBufferParams.dwInsideConeAngle  = m_pInfo->getInsideConeAngle();
		m_dsBufferParams.dwOutsideConeAngle = m_pInfo->getOutsideConeAngle();

		{
			INT32 nVolume		= m_pInfo->getOutsideVolume();
			INT32 nCtxVolume	= g_pSoundCtx->getMainVolume();
			INT32 nTypeVolume	= g_pSoundCtx->GetTypeVolume( m_nState & I3SND_PROP_MASK_TYPE);
			INT32 nFinalVolume	= (INT32)(nVolume * nCtxVolume * nTypeVolume * 0.0001f);

			INT32 DSVolume = (nFinalVolume <= 0) ? DSBVOLUME_MIN :
			(nFinalVolume >= 100) ? DSBVOLUME_MAX :
			(INT32)(2000.0f * log10( nFinalVolume * 0.01f));

			m_dsBufferParams.lConeOutsideVolume = DSVolume;
		}

		switch( m_pInfo->Get3DMode())
		{
			//표준 LISTENER연관 3DPOSITIONING 
			case I3SND_RT_3DMODE_WORLD_SPACE :
				{
					m_dsBufferParams.dwMode = DS3DMODE_NORMAL;
				}
				break;

			case I3SND_RT_3DMODE_CAMERA_SPACE :
				{
					m_dsBufferParams.dwMode = DS3DMODE_HEADRELATIVE;
				}
				break;
		}
		
		I3SND_ENTER_CRITICAL();
		{
			DO_AND_CHECK_ERR( m_p3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_IMMEDIATE), "m_p3DBuffer->SetAllParameters()");
			//DO_AND_CHECK_ERR( m_p3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_DEFERRED), "m_p3DBuffer->SetAllParameters()");
		}
		I3SND_LEAVE_CRITICAL();
	}
	else
	{
		I3_SAFE_RELEASE( m_p3DBuffer);

		SetVolume( m_nVolume);
	}
}

bool i3SoundPlayStateDx::Drive()
{
	bool	bRv = false;
	DWORD	dwStatus;

	I3SND_ENTER_CRITICAL();

	//버퍼의 상태 검사
	if((m_pBufferInfo != nullptr) && (m_lpDSBuffer != nullptr))
	{
		if( DO_AND_CHECK_ERR( m_lpDSBuffer->GetStatus( &dwStatus), "GetStatus()"))
		{
			if( dwStatus & DSBSTATUS_PLAYING)	//현재 플레이중
			{
				bRv = true;
			}
			else
			{
				if( isPaused())//PAUSE상태
				{
					I3TRACE("paused\n");
					bRv = true;
				}
				else
				{
					m_pBufferInfo->isStopping = true;
				}
			}
		}
	}

	I3SND_LEAVE_CRITICAL();

	return bRv;
}

//////////////////////////////////////////////////
//
bool i3SoundPlayStateDx::UpdatePos( MATRIX * pMat)
{
	if( pMat == nullptr)
		return true;

	if( !_UpdateLocationParam( m_pInfo, pMat))
	{
		setRequest( I3SND_RT_REQ_NONE);
		return false;
	}

	addRequest( I3SND_RT_REQ_POSITION);

	return true;
}

bool i3SoundPlayStateDx::CMD_UpdatePos(void)
{
	bool bRet = false;

	removeRequest( I3SND_RT_REQ_POSITION);

	if( isFree())								return false;
	if((m_CreateFlag & DSBCAPS_CTRL3D) == 0)	return false;
	if( isListnerSound())						return false;

	I3ASSERT( m_p3DBuffer != nullptr);

	if( i3Vector::isAlike( &m_vecPos, &m_vecOldPos) && i3Vector::isAlike( &m_vDirection, &m_vOldDirection))
	{
		return true;
	}

	{
		m_dsBufferParams.vConeOrientation.x = getX(&m_vDirection);
		m_dsBufferParams.vConeOrientation.y = getY(&m_vDirection);
		m_dsBufferParams.vConeOrientation.z = getZ(&m_vDirection);

		m_dsBufferParams.vPosition.x = getX(&m_vecPos);
		m_dsBufferParams.vPosition.y = getY(&m_vecPos);
		m_dsBufferParams.vPosition.z = getZ(&m_vecPos);

		m_dsBufferParams.vVelocity.x = getX( &m_vVelocity);
		m_dsBufferParams.vVelocity.y = getY( &m_vVelocity);
		m_dsBufferParams.vVelocity.z = getZ( &m_vVelocity);

		I3SND_ENTER_CRITICAL();
		{
			bRet = DO_AND_CHECK_ERR( m_p3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_DEFERRED), "");
		}
		I3SND_LEAVE_CRITICAL();
	}

	return bRet;
}

bool i3SoundPlayStateDx::_IsValidPlayReq()
{
	if( m_pInfo == nullptr)	return false;

	//play 가능한 조건을 만족하는지 확인
	i3Sound * pSound = m_pInfo->getSound();
	if( pSound == nullptr)
		return false;

	if( m_pInfo->GetStreamType() == I3SND_RT_BUFFMODE_STREAM)
	{
		if( pSound->IsExactTypeOf( i3SoundStream::GetClassMeta()) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "this is not a Streamming Sound but initialized streaming");
			return false;
		}
	}

	return true;
}


/*
bool i3SoundPlayStateDx::RePlay()
{
	if( !(IsReady()))
	{
		HRESULT hr;

		if( FAILED( hr = m_lpDSBuffer->SetCurrentPosition(0)))
		{
			DXTRACE_ERR("FAILED SetCurrentPosition", hr);
			return false;
		}

		DWORD dwStatus;
		if( FAILED( hr = m_lpDSBuffer->GetStatus(&dwStatus)))
		{
			DXTRACE_ERR("FAILED GetStatus", hr);
			return false;
		}

		if(( dwStatus & DSBSTATUS_PLAYING) == 0)
		{
			if( FAILED( hr = m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag)))
			{
				DXTRACE_ERR("Failed m_lpDSBuffer->RePlay(: hr : %X)", hr );

				return false;
			}
		}

		SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

		return true;
	}

	return false;
}

bool i3SoundPlayStateDx::play( i3SoundPlayInfo * pSoundInfo, VEC3D * pPos)
{
	HRESULT hr;

	I3ASSERT( g_pDS != nullptr);
	I3ASSERT( pSoundInfo != nullptr);

	SetSoundPlayInfo( pSoundInfo);

	i3SoundDx * pSoundDx = (i3SoundDx *) m_pInfo->getSound();
	if( pSoundDx == nullptr)
	{
		return false;
	}

	m_lpDSBuffer = (LPDIRECTSOUNDBUFFER8)pSoundDx->GetSoundBuffer();
	if( m_lpDSBuffer == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Can't get SoundBuffer");
		return false;
	}

	m_nCount++;

	///////////////////////
	if( isStreamingSound())
	{
		_CreateStream();
	}

	if( FAILED(hr = m_lpDSBuffer->SetCurrentPosition( 0)))
	{
		DXTRACE_ERR( "m_lpDSBuffer->SetCurrentPosition(0)", hr);
	}

	m_nPlayFlag		= 0;
	if( isEffectSound() && (pPos != nullptr))
	{ 
		Set3DParams( pPos);

		//Effect Sound doesn't get hardware acceleration
		if( !isListnerSound())
		{
			//m_nPlayFlag = DSBPLAY_TERMINATEBY_DISTANCE | DSBPLAY_TERMINATEBY_PRIORITY;
		}
	}

	////////////////////////////////////////////////
	if( g_pSoundCtx->m_bEnableFX && isEffectSound())
	{
		//SetFX();
		SetReverb();
	}
		
	setVolume( 100);

	m_dwPriority	= 0x0;

	//스트리밍인경우 looping enable, soundplayinfo의 정보가 oneshot인경우라도 streamming은 loopenable
    if( isStreamingSound())
	{
		m_dwPriority	= 0;
		m_nPlayFlag |= DSBPLAY_LOOPING;
	}

	//loopsound 이면 looping enalble
	if( isLoop())
	{
		m_nPlayFlag |= DSBPLAY_LOOPING;
	}

	//현재 사운드 버퍼는 DSBCAPS_LOCDEFER를 가지고 생성되었습니다.( 이펙트의 경우에만)
	//hardware buffer가 사용가능하지 않은경우, voicemanagement flag가 들어간상태로 플레이하면 실패하게됩니다.
	//위의 이유로 실패한경우는 voiceManagement Flag를 제거하고 software mode로 다시 플레이를 시도합니다.
	hr = m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag);

	if( FAILED( hr))
	{
		if( hr == DSERR_INVALIDCALL)
		{
			m_nPlayFlag &= ~DSBPLAY_TERMINATEBY_DISTANCE;
			m_nPlayFlag &= ~DSBPLAY_TERMINATEBY_PRIORITY;
			m_nPlayFlag |= DSBPLAY_LOCSOFTWARE;

			hr = m_lpDSBuffer->Play( 0, 0, m_nPlayFlag);
		}

		if( FAILED( hr))
		{
			DXTRACE_ERR("Second Try : m_lpDSBuffer->Play()", hr );

			I3TRACE("pSoundDx: %s\n", pSoundDx->GetName());

			stop();

			return false;
		}
	}

	if( isStreamingSound())
	{
		_StartStream();
	}

	SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

	return true;
}
*/
/*


void i3SoundPlayStateDx::stop(bool bForce)
{
	//사용중이면 버퍼를 해제하지않고 종료해줄때까지 대기한다.
	//사용자 측에서 UsedFlag를 이용하여 PlayState를 사용중인 상태로 잡고 있길 원하거나, 
	//스트리밍 사운드에서 스레드Update시간내에 종료되길 원하는경우에 사용합니다.
	//강제 종료 플래그( bForce)가 true인 경우에는 모두 무시하고 종료해버립니다.
	if( m_lpDSBuffer != nullptr)
	{
		if( IsUsed() || (isStreamingSound() && !IsWaiting()))
		{
			AddState( I3SND_RT_PLAYSTATE_WAIT);
			if( !bForce)
			{
				return;
			}
		}

		HRESULT hr;
		if( FAILED( hr = m_lpDSBuffer->Stop()))
		{
			DXTRACE_ERR("FAILED m_lpDSBuffer->Stop(): %X\n", hr);
			return ;
		}

		if( isEffectSound())
		{
			I3_SAFE_RELEASE( m_p3DBuffer);

			if( g_pSoundCtx->m_bEnableFX)
			{
				DisableAllFX();
			}
		}

		if( isStreamingSound())
		{
			_StopStream();
		}
		
		m_nCount--;
	}

	//Release하지 않습니다.
	m_lpDSBuffer = nullptr;

	SetPlayState( I3SND_RT_PLAYSTATE_READY);

}

void i3SoundPlayStateDx::pause()
{
	if( m_lpDSBuffer == nullptr)
		return ;

	m_lpDSBuffer->GetCurrentPosition( &m_dwPlayPointer, nullptr);
	m_lpDSBuffer->Stop();

	AddState( I3SND_RT_PLAYSTATE_PAUSE );
}

void i3SoundPlayStateDx::resume()
{
	if( m_lpDSBuffer == nullptr)
		return ;

	m_lpDSBuffer->SetCurrentPosition( m_dwPlayPointer);
	m_lpDSBuffer->Play( 0, m_dwPriority, m_nPlayFlag);

	RemoveState( I3SND_RT_PLAYSTATE_PAUSE );
}

void i3SoundPlayStateDx::Update(VEC3D * pPos, REAL32 rDeltaSeconds)
{
	if( isListnerSound())	return;
	if( pPos == nullptr)	return ;
	if( m_p3DBuffer == nullptr)	return ;

	i3Vector::Copy( &m_vecOldPos, &m_vecPos);
	i3Vector::Copy( &m_vecPos, pPos);

	if( !IsPlaying())		return;

	//SetPos
	m_p3DBuffer->SetPosition( getX(&m_vecPos), getZ(&m_vecPos), getY(&m_vecPos), DS3D_DEFERRED);

	if( ! isListnerSound())
	{
		//Set Con Orient
		VEC3D vecOrient;
		i3Vector::Sub(&vecOrient, &m_vecPos, i3Matrix::GetPos( g_pSoundCtx->GetListnerMatrix()));
		i3Vector::Normalize(&vecOrient, &vecOrient);
		m_p3DBuffer->SetConeOrientation( getX(&vecOrient), getZ(&vecOrient), getY(&vecOrient), DS3D_DEFERRED);
	}
}

bool i3SoundPlayStateDx::OnDrive()
{
	bool	bRv = false;
	DWORD	dwStatus;
	HRESULT hResult;

	//버퍼의 상태 검사
	if(m_lpDSBuffer != nullptr)
	{
		hResult = m_lpDSBuffer->GetStatus( &dwStatus );
		if( hResult == S_OK)
		{
			if( dwStatus & DSBSTATUS_PLAYING)	//현재 플레이중
			{
				bRv = true;
			}
			else
			{
				if( IsPaused())//PAUSE상태
				{
					bRv = true;
				}
			}
		}

		if(isChangedVolume())
		{
			setVolume( 100);
			setChangedVolume( false);
		}
	}

	return bRv;
}

void i3SoundPlayStateDx::setVolume(UINT8 nVolume)
{
	if( m_lpDSBuffer == nullptr)
		return ;

	SetFinalVolume( nVolume);
}

void i3SoundPlayStateDx::SetDistVolume( VEC3D * pPos)
{
	I3ASSERT( pPos != nullptr);

	REAL32 rDistance		= i3Vector::Length( pPos);
	REAL32 rInvMinMaxGap	= i3Math::div( 1.0f, m_pInfo->getMaxDistance()- m_pInfo->getMinDistance());

	REAL32 v = 
		( rDistance <= m_pInfo->getMinDistance() ) ? 1.0f:
	( rDistance >= m_pInfo->getMaxDistance() ) ? 0.0f:
	(1.0f - ( i3Math::Mul( rDistance - m_pInfo->getMinDistance(), rInvMinMaxGap)));

	//REAL32 rDistanceRatio = MINMAX( 0.0f, powf( 10.0f, i3Math::Mul( 5.0f, v - 1.0f )), 1.0f);

	if( v <= 0.1f)
	{
		SetFinalVolume(0);
		return ;
	}

	INT32 nVolume = (INT32)MINMAX( 0, i3Math::Mul( 100.0f, v), 100);

	SetFinalVolume( nVolume);
}

void i3SoundPlayStateDx::SetFinalVolume( INT32 nVolume)
{
	INT32 nCtxVolume   = g_pSoundCtx->getMainVolume();
	INT32 nTypeVolume  = g_pSoundCtx->GetTypeVolume( m_nState & I3SND_PROP_MASK_TYPE);
	INT32 nFinalVolume = (INT32)(nVolume * nCtxVolume * nTypeVolume * m_nVolume * 0.000001f);

	INT32 DSVolume = (nFinalVolume <= 0) ? DSBVOLUME_MIN :
	(nFinalVolume >= 100) ? DSBVOLUME_MAX :
	(INT32)(4000.0f * log10( nFinalVolume * 0.01f));

	HRESULT hr = m_lpDSBuffer->SetVolume( DSVolume);
	if( FAILED( hr))
	{
		DXTRACE_ERR( "", hr);
	}

	m_nFinalVolume = nFinalVolume;
}
*/

bool i3SoundPlayStateDx::_CreateStream( )
{
	m_pStreamThread = i3SoundStreamThread::new_object();

	m_pStreamThread->setState( (void*)this);

	if( m_pStreamThread->Create( "SoundStreamThread") == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not create sound Stream thread");
	}

	return m_pStreamThread->Suspend();
}

bool i3SoundPlayStateDx::_StartStream()
{
	if( !isStreamingSound())	return false;
	if( m_pStreamThread == nullptr)	return false;

	if( m_pStreamThread->Resume())
		return true;

	return false;
}

bool i3SoundPlayStateDx::_StopStream()
{
	if( !isStreamingSound())		return false;
	if( m_pStreamThread == nullptr)	return false;

//	i3SoundStream * pStream = (i3SoundStream*)m_pInfo->getSound();
//	pStream->ResetStream();

	m_pStreamThread->Terminate();
	
	m_pStreamThread->Release();

	m_pStreamThread = nullptr;

	return true;
}

bool i3SoundPlayStateDx::UpdateStream()
{
	i3SoundPlayInfo * pInfo = getPlayInfo();
	i3SoundStream * pStream = (i3SoundStream*)pInfo->getSound();

	if( isPlaying())
	{
		if( isRequestEnable() && (getRequest() & I3SND_RT_REQ_STOP) != 0)
		{
			return false;
		}

		if( isPaused())
		{
			return true;
		}

		pStream->m_nOldPlayPos = pStream->m_nPlayPos;

		//check event
		GetPlayPointer( &pStream->m_nPlayPos);
		pStream->m_nPlayPos /= pStream->getBlockAlign();

		if (pStream->m_nOldPlayPos > pStream->m_nPlayPos)
			pStream->m_nPlaySize += pStream->m_nBufferSize - pStream->m_nOldPlayPos + pStream->m_nPlayPos;
		else 
			pStream->m_nPlaySize += pStream->m_nPlayPos - pStream->m_nOldPlayPos;

		{
			INT32 nGap = pStream->m_nReadSize - pStream->m_nPlaySize - pStream->m_nTempPlaySize;
			if( (nGap < (INT32)pStream->m_nGapBetweenCursor))
			{
				WCHAR32 nReadBytes = 0;

				//update buffer
				pStream->ReadFile( pStream->m_nWritePos, min( pStream->m_nReadOnce, pStream->m_pos.size - pStream->m_nReadSize), 0, &nReadBytes);

				pStream->m_nReadSize += nReadBytes;
				pStream->m_nWritePos = (pStream->m_nWritePos + nReadBytes) % pStream->m_nBufferSize;
				//I3TRACE("pp:%d, Ps: %d, Rs:%d\n", pStream->m_nPlayPos, pStream->m_nPlaySize, pStream->m_nReadSize);
			}
			else
			{
				if( nGap < 0)
				{
					I3TRACE("abnormalstate: pp:%d, Ps: %d, Rs:%d\n", pStream->m_nPlayPos, pStream->m_nPlaySize, pStream->m_nReadSize);
				}
			}
		}

		if ( pStream->m_nReadSize >= pStream->m_pos.size)
		{
			//I3TRACE("ARRIVED END POS\n");
			if( isLoop())
			{
				//rewind
				pStream->m_nTempPlaySize = pStream->m_nPlaySize + pStream->m_nTempPlaySize - pStream->m_nReadSize;
				pStream->m_nReadSize = 0;
				pStream->m_nPlaySize = 0;

				// 새 포지션 정보를 갱신한다.
				if (pStream->m_next_pos != nullptr)
				{
					//I3TRACE("SETSTREAMPOSITION\n");
					pStream->SetCurrentStreamPosition( pStream->m_next_pos);
				} 
				else// 현재 포지션 정보를 수정해서 loop 
				{
					pStream->m_pos.start_pos = pStream->m_pos.loop_start_pos;
					pStream->m_pos.size		-= pStream->m_pos.loop_start_pos - pStream->m_pos.start_pos;
				}

				pStream->MoveFromFile ( pStream->m_pos.start_pos);

				I3TRACE("REPEAT \n");
			}
			else
			{
				I3TRACE("REQUEST STOP \n");
				//Stop
				Stop();
			}
		}

		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SOUND FX
void	i3SoundPlayStateDx::InitReverb()
{
//	I3_SAFE_RELEASE( m_lpFxBuffer);
	I3_SAFE_RELEASE( m_lpReverb );

	i3mem::FillZero( &m_rgFxDesc, sizeof( DSEFFECTDESC ));

    m_rgFxDesc.dwSize					= sizeof(DSEFFECTDESC);
    m_rgFxDesc.dwFlags					= 0;
	m_rgFxDesc.guidDSFXClass			= GUID_DSFX_STANDARD_I3DL2REVERB;
    m_rgRefGuids						= &IID_IDirectSoundFXI3DL2Reverb8;

	//m_paramsReverb.lRoom				= DSFX_I3DL2REVERB_ROOM_DEFAULT;
	//m_paramsReverb.lRoomHF				= DSFX_I3DL2REVERB_ROOMHF_DEFAULT;
	//m_paramsReverb.flRoomRolloffFactor	= DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_DEFAULT;
	//m_paramsReverb.flDecayTime			= DSFX_I3DL2REVERB_DECAYTIME_DEFAULT;
	//m_paramsReverb.flDecayHFRatio		= DSFX_I3DL2REVERB_DECAYHFRATIO_DEFAULT;
	//m_paramsReverb.lReflections			= DSFX_I3DL2REVERB_REFLECTIONS_DEFAULT;
	//m_paramsReverb.flReflectionsDelay	= DSFX_I3DL2REVERB_REFLECTIONSDELAY_DEFAULT;

	//m_paramsReverb.lReverb				= DSFX_I3DL2REVERB_REVERB_DEFAULT;
	//m_paramsReverb.flReverbDelay		= DSFX_I3DL2REVERB_REVERBDELAY_DEFAULT;
	//m_paramsReverb.flHFReference		= DSFX_I3DL2REVERB_HFREFERENCE_DEFAULT;
	//m_paramsReverb.flDensity			= DSFX_I3DL2REVERB_DENSITY_DEFAULT;

	//m_paramsReverb.flDiffusion			= DSFX_I3DL2REVERB_DIFFUSION_DEFAULT;
}

HRESULT i3SoundPlayStateDx::SetReverb()
{
	HRESULT hr;
	if( m_lpDSBuffer == nullptr)
	{
		I3TRACE(" m_lpDSBuffer == nullptr\n");
		return S_OK;
	}

	if( (m_pInfo == nullptr) || (m_pInfo->getI3DL2ReverbPreset() == -1))
	{
		return S_OK;
	}

	InitReverb();

	DWORD dwResults;
	if( FAILED( hr = m_lpDSBuffer->SetFX( 1, &m_rgFxDesc, &dwResults ) ) )
	{
		DXTRACE_ERR("m_lpDSBuffer->SetFX( 1, &m_rgFxDesc, &dwResults )", hr);
		DXTRACE_ERR("result", dwResults);
        return hr;
	}

	if( FAILED( hr = m_lpDSBuffer->GetObjectInPath( m_rgFxDesc.guidDSFXClass, 0, *m_rgRefGuids, (LPVOID*)&m_lpReverb ) ) )
	{
		DXTRACE_ERR("hr = m_lpFxBuffer->GetObjectInPath()", hr);
		return E_FAIL;
	}

	if( FAILED( hr = SetReverbPreset( m_pInfo->getI3DL2ReverbPreset())))
	{
		DXTRACE_ERR("hr = SetReverbPreset()", hr);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT i3SoundPlayStateDx::DisableAllFX()
{
	if( nullptr == m_lpDSBuffer )
        return E_FAIL;

	I3SND_ENTER_CRITICAL();
	{
		// Buffer must be stopped before calling SetFx
		m_lpDSBuffer->Stop();

		// this removes all fx from the buffer
		m_lpDSBuffer->SetFX( 0, nullptr, nullptr );
	}
	I3SND_LEAVE_CRITICAL();
    
	InitReverb();

    return S_OK;
}

HRESULT i3SoundPlayStateDx::SetReverbParam( DSFXI3DL2Reverb * pparam)
{
	HRESULT hr = S_OK;

	I3SND_ENTER_CRITICAL();
	if( FAILED( hr = m_lpReverb->SetAllParameters( pparam)))
	{
		DXTRACE_ERR( "m_lpReverb->SetAllParameters()", hr);
	}
	I3SND_LEAVE_CRITICAL();

	return hr;
}

HRESULT i3SoundPlayStateDx::SetReverbPreset( UINT32 nPreset)
{
	HRESULT hr = S_OK;

	if( m_lpReverb )
	{
		if( nPreset >= DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT && nPreset <= DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE)
		{
			I3SND_ENTER_CRITICAL();
			if( FAILED( hr = m_lpReverb->SetPreset( nPreset)))
			{
				DXTRACE_ERR( "m_lpReverb->SetPreset()", hr);
			}
			I3SND_LEAVE_CRITICAL();
		}
		else
		{
			I3TRACE("Reverb preset is out of range!!\n");
		}
	}

	return hr;
}

/*
void i3SoundPlayStateDx::InitFX()
{
	I3_SAFE_RELEASE( m_lpChorus );
    I3_SAFE_RELEASE( m_lpCompressor );
    I3_SAFE_RELEASE( m_lpDistortion );
    I3_SAFE_RELEASE( m_lpEcho );
    I3_SAFE_RELEASE( m_lpFlanger );
    I3_SAFE_RELEASE( m_lpGargle );
    I3_SAFE_RELEASE( m_lpParamEq );
    I3_SAFE_RELEASE( m_lpReverb );

	m_dwNumFX = 0;
    i3mem::FillZero( m_rgFxDesc, sizeof( DSEFFECTDESC ) * I3SND_CMN_FX_COUNT );
    i3mem::FillZero( m_rgRefGuids, sizeof( GUID * ) * I3SND_CMN_FX_COUNT );
    i3mem::FillZero( m_rgPtrs, sizeof(LPVOID*) * I3SND_CMN_FX_COUNT );
    i3mem::FillZero( m_rgLoaded, sizeof( bool ) * I3SND_CMN_FX_COUNT );
}

HRESULT i3SoundPlayStateDx::DisableAllFX()
{
	if( nullptr == m_lpDSBuffer )
        return E_FAIL;

    // Buffer must be stopped before calling SetFx
    m_lpDSBuffer->Stop();

    // this removes all fx from the buffer
    m_lpDSBuffer->SetFX( 0, nullptr, nullptr );

    return S_OK;
}

HRESULT i3SoundPlayStateDx::SetFX( UINT32 nEffectType)
{
	InitFX();

	INT32 i			= 0;
	for( i = 0; i < I3SND_CMN_FX_COUNT ; ++i)
	{
		INT32 nEfIdx  = I3SND_CMN_FX_CHORUS << i;
		if(( nEfIdx & nEffectType) != 0)
		{
			SetFXEnable( i);
		}
	}

	return 0;
}

HRESULT i3SoundPlayStateDx::SetFX()
{
	HRESULT hr;
	if( m_lpDSBuffer == nullptr)
	{
		I3TRACE(" m_lpDSBuffer == nullptr\n");
		return S_OK;
	}

	if( m_dwNumFX <= 0)
	{
		return S_OK;
	}

	DWORD dwResults[I3SND_CMN_FX_COUNT];
	if( FAILED( hr = m_lpDSBuffer->SetFX( m_dwNumFX, m_rgFxDesc, dwResults ) ) )
	{
		DXTRACE_ERR("m_lpDSBuffer->SetFX( m_dwNumFX, m_rgFxDesc, dwResults )", hr);
        return hr;
	}

	INT32 i;
	for( i = 0; i < (INT32)m_dwNumFX; i++ )
	{
		if( *m_rgPtrs[i] != nullptr)
		{
			LPDIRECTSOUNDFXI3DL2REVERB8 pRev = (LPDIRECTSOUNDFXI3DL2REVERB8)*m_rgPtrs[i];
			I3_SAFE_RELEASE( pRev);
		}

		{
			if( FAILED( hr = m_lpDSBuffer->GetObjectInPath( m_rgFxDesc[i].guidDSFXClass, 0, *m_rgRefGuids[i], m_rgPtrs[i] ) ) )
			{
				DXTRACE_ERR("hr = m_lpDSBuffer->GetObjectInPath()", hr);
				return E_FAIL;
			}
		}
	}

	LoadParamValues();

	return S_OK;
}

HRESULT i3SoundPlayStateDx::EnableFX( GUID sfxClass, REFGUID refGUIDInterface, LPVOID * ppObj)
{
	if( *ppObj )
        return S_FALSE;

    if( m_dwNumFX >= I3SND_CMN_FX_COUNT )
        return E_FAIL;

    // set the effect to be enabled
	i3mem::FillZero( &m_rgFxDesc[m_dwNumFX], sizeof(DSEFFECTDESC) );
    m_rgFxDesc[m_dwNumFX].dwSize         = sizeof(DSEFFECTDESC);
    m_rgFxDesc[m_dwNumFX].dwFlags        = 0;
	i3mem::Copy( &m_rgFxDesc[m_dwNumFX].guidDSFXClass, &sfxClass, sizeof(GUID));

    m_rgRefGuids[m_dwNumFX] = &refGUIDInterface;
    m_rgPtrs[m_dwNumFX] = ppObj;

    m_dwNumFX++;

    return S_OK;
}

HRESULT i3SoundPlayStateDx::SetFXEnable( UINT32 nEffectType)
{
	HRESULT hr;

    if( nEffectType >= I3SND_CMN_FX_COUNT )
        return E_FAIL;

    if( m_rgLoaded[nEffectType] )
        return S_FALSE;
    else
        m_rgLoaded[nEffectType] = true;

    switch ( nEffectType )
    {
	case 0://I3SND_FX_CHORUS:
        hr = EnableFX( GUID_DSFX_STANDARD_CHORUS,     IID_IDirectSoundFXChorus8,
                              (LPVOID*) &m_lpChorus );
        break;
    case 1://I3SND_FX_COMPRESSOR:
        hr = EnableFX( GUID_DSFX_STANDARD_COMPRESSOR, IID_IDirectSoundFXCompressor8,
                              (LPVOID*) &m_lpCompressor );
        break;
    case 2://I3SND_FX_DISTORTION:
        hr = EnableFX( GUID_DSFX_STANDARD_DISTORTION, IID_IDirectSoundFXDistortion8,
                              (LPVOID*) &m_lpDistortion );
        break;
    case 3://I3SND_FX_ECHO:
        hr = EnableFX( GUID_DSFX_STANDARD_ECHO,       IID_IDirectSoundFXEcho8,
                              (LPVOID*) &m_lpEcho );
        break;
    case 4://I3SND_FX_FLANGER:
        hr = EnableFX( GUID_DSFX_STANDARD_FLANGER,    IID_IDirectSoundFXFlanger8,
                              (LPVOID*) &m_lpFlanger );
        break;
    case 5://I3SND_FX_GARGLE:
        hr = EnableFX( GUID_DSFX_STANDARD_GARGLE,     IID_IDirectSoundFXGargle8,
                              (LPVOID*) &m_lpGargle );
        break;
    case 6://I3SND_FX_PARAMEQ:
        hr = EnableFX( GUID_DSFX_STANDARD_PARAMEQ,    IID_IDirectSoundFXParamEq8,
                              (LPVOID*) &m_lpParamEq );
        break;
    case 7://I3SND_FX_REVERB:
        hr = EnableFX( GUID_DSFX_STANDARD_I3DL2REVERB,  IID_IDirectSoundFXI3DL2Reverb8,
                              (LPVOID*) &m_lpReverb );
        break;
    default:
        hr = E_FAIL;
        break;
    }

    return hr;
}

HRESULT i3SoundPlayStateDx::LoadParamValues()
{
	HRESULT hr = S_OK;

	if( m_lpChorus )
	{
		hr = m_lpChorus->SetAllParameters( &m_paramsChorus );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpChorus->SetAllParameter()", hr);
		}
	}

	if( m_lpCompressor )
	{
		hr = m_lpCompressor->SetAllParameters( &m_paramsCompressor );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpCompressor->SetAllParameters()", hr);
		}
	}

	if( m_lpDistortion )
	{
		hr = m_lpDistortion->SetAllParameters( &m_paramsDistortion );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpDistortion->SetAllParameters()", hr);
		}
	}

	if( m_lpEcho )
	{
		hr = m_lpEcho->SetAllParameters( &m_paramsEcho );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpEcho->SetAllParameters()", hr);
		}
	}

	if( m_lpFlanger )
	{
		hr = m_lpFlanger->SetAllParameters( &m_paramsFlanger );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpFlanger->SetAllParameters()", hr);
		}
	}

	if( m_lpGargle )
	{
		hr = m_lpGargle->SetAllParameters( &m_paramsGargle );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpGargle->SetAllParameters()", hr);
		}
	}
	
	if( m_lpParamEq )
	{
		hr = m_lpParamEq->SetAllParameters( &m_paramsParamEq );
		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpParamEq->SetAllParameters()", hr);
		}
	}

	if( m_lpReverb )
	{
		if( g_pSoundCtx->m_nReverbPresetIndex != -1)
		{
			hr = m_lpReverb->SetPreset( g_pSoundCtx->m_nReverbPresetIndex);
		}
		else
		{
			hr = m_lpReverb->SetAllParameters( &m_paramsReverb );
		}

		if( FAILED( hr))
		{
			DXTRACE_ERR( "m_lpReverb->SetPreset()", hr);
		}
	}

	return S_OK;
}
*/
#endif
