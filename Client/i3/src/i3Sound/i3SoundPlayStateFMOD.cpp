#include "stdafx.h"

#ifdef I3SND_FMOD

#include "i3SoundFMOD.h"
#include "i3SoundPlayStateFMOD.h"
#include "i3SoundContext.h"
#include "i3SoundContextFMOD.h"
#include "i3SoundGlobal.h"

I3_CLASS_INSTANCE( i3SoundPlayStateFMOD);

FMOD::Sound *		i3SoundPlayStateFMOD::getFMODSound(void)
{
	I3ASSERT_RETURN( m_pInfo != nullptr, nullptr);

	i3SoundFMOD * pSoundFMOD = (i3SoundFMOD *) m_pInfo->getSound();
	I3ASSERT( pSoundFMOD != nullptr);
	
	if(pSoundFMOD == nullptr)
	{
#if defined(I3_DEBUG)
		I3PRINTLOG(I3LOG_NOTICE, "Sound가 없슴다. name : %s", m_pInfo->_getSourceFileName());
#endif
		return nullptr;
	}

	switch( m_pInfo->Get3DMode())
	{
		case I3SND_RT_3DMODE_DISABLE :		return pSoundFMOD->Get2DSound( true);	break;
		case I3SND_RT_3DMODE_CAMERA_SPACE :	return pSoundFMOD->Get3DSound( true, true);	break;
		case I3SND_RT_3DMODE_WORLD_SPACE :	return pSoundFMOD->Get3DSound( true, false);	break;
	}

	return nullptr;
}

bool i3SoundPlayStateFMOD::Play( i3SoundPlayInfo * pInfo, MATRIX * pMtx, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	I3ASSERT( pInfo != nullptr);

	if( !isFree())
	{
		I3TRACE("종료되지 않은 사운드에 대해 다시 플레이가 요청되었음.\n");

		if( m_pInfo == pInfo)
		{
			if( pInfo->GetRefCount() <= 1)
			{
				I3PRINTLOG(I3LOG_FATAL, "이상한 사운드 플레이 요청, 이부분이 나온다면 필히 이부분을 수정해야합니다.");
			}
		}

		Stop(true);
	}

	SetSoundPlayInfo( pInfo);

	if( !isListnerSound() && !isLoop() && pMtx != nullptr && m_pInfo->isMuteAtMaxDistance()) 
	{
		bool bMute = !g_pSoundCtx->isAudioable( pMtx, m_pInfo->getMaxDistance());
		if( bMute)
		{
			Reset();
			return false;
		}
	}

	FMOD::Sound * pFMOD = getFMODSound();

	//reset arg
	{
		m_dwPlayPointer		= 0;
		m_nPlayFlag			= 0;
		m_dwPriority		= 0;
	}

	FMOD_RESULT rv = g_pFMODSystem->playSound( FMOD_CHANNEL_FREE, pFMOD, true, &m_pChannel);
	ERRCHECK( rv);

	m_pChannel->setUserData( this);

	if(callback == nullptr)
	{
		m_pChannel->setCallback( FMOD_CHANNEL_CALLBACKTYPE_END, i3SoundContextFMOD::_ChannelEndProc, 0);
	}
	else
	{
		m_pChannel->setCallback( FMOD_CHANNEL_CALLBACKTYPE_END, callback, 0);
	}
	

	// loopcount : 1 -> 1번 루프후 종료, 0 : 1번만 , -1 : 무한반복
	// 차후 loopcount를 지정해줘야 하는경우에대한 처리를 하게되면 변경되어야 하는 부분입니다.-raja-
	if( isLoop())
		m_pChannel->setLoopCount( -1);
	else
		m_pChannel->setLoopCount( 0);

	if( m_pInfo->Get3DMode() != I3SND_RT_3DMODE_DISABLE)
	{
		_UpdateLocationParam( m_pInfo, pMtx);

		Set3DParams();
		//UpdatePos( pMtx);
	}

	SetVolume( m_nVolume);

	SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

	SetChannelGroup( GrpIdx);

	rv = m_pChannel->setPaused( false);
	ERRCHECK( rv);

	UpdateMuteState();

	return true;
}

bool i3SoundPlayStateFMOD::SetChannelGroup( INT32 GrpIdx)
{
	FMOD::ChannelGroup * pGroup = (FMOD::ChannelGroup*)((i3SoundContextFMOD*)g_pSoundCtx)->GetChannelGroup( GrpIdx);

	if( pGroup != nullptr && m_pChannel != nullptr)
	{
		FMOD_RESULT rv = m_pChannel->setChannelGroup( pGroup);
		ERRCHECK( rv);

		return true;
	}

	return false;
}

bool i3SoundPlayStateFMOD::Replay( i3SoundPlayInfo * pInfo, MATRIX * pMtx, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	FMOD_RESULT rv;

	I3ASSERT( m_pChannel != nullptr);

	// REUSE 옵션으로 playSound를 호출하는 순간, 이전에 play 중이던 사운드는
	// 강제로 Off되며, Callback이 호출된다.
	// 이 때, m_pInfo 등이 Callback 함수 내에서 nullptr로 초기화되기 때문에
	// 임시로 Callback을 없앤다.
	m_pChannel->setCallback( FMOD_CHANNEL_CALLBACKTYPE_END, nullptr, 0);

	SetSoundPlayInfo( pInfo);

	//////
	rv = m_pChannel->setPosition( 0, FMOD_TIMEUNIT_PCM);
	ERRCHECK( rv);
	
	FMOD::Sound * pFMOD = getFMODSound();

	rv = g_pFMODSystem->playSound( FMOD_CHANNEL_REUSE, pFMOD, true, &m_pChannel);
	ERRCHECK( rv);

	m_pChannel->setUserData( this);
	m_pChannel->setCallback( FMOD_CHANNEL_CALLBACKTYPE_END, i3SoundContextFMOD::_ChannelEndProc, 0);

	// loopcount : 1 -> 1번 루프후 종료, 0 : 1번만 , -1 : 무한반복
	// 차후 loopcount를 지정해줘야 하는경우에대한 처리를 하게되면 변경되어야 하는 부분입니다.-raja-
	if( isLoop())
		m_pChannel->setLoopCount( -1);
	else
		m_pChannel->setLoopCount( 0);

	if( m_pInfo->Get3DMode() != I3SND_RT_3DMODE_DISABLE)
	{
		_UpdateLocationParam( m_pInfo, pMtx);
		Set3DParams();
	}

	SetVolume( m_nVolume);

	SetPlayState( I3SND_RT_PLAYSTATE_PLAY);

	SetChannelGroup( GrpIdx);

	rv = m_pChannel->setPaused( false);
	ERRCHECK( rv);

	UpdateMuteState();

	return true;
}

bool i3SoundPlayStateFMOD::Stop( bool bForce)
{
	//I3ASSERT( m_pChannel != nullptr);

	// i3SoundContext를 경유하지 않고 직접 Stop이 호출되는 경우가
	// 발생할 수 있다.
	if( isFree() == false)
	{
		//Release하지 않습니다.
		m_nVolume		= -1;
		--m_nCount;

		FMOD_RESULT rc = m_pChannel->stop();
		ERRCHECK( rc);

		//Reset();
	}
	
	return true;
}

bool i3SoundPlayStateFMOD:: Pause(void)
{
	I3ASSERT( m_pChannel != nullptr);

	if( !isPaused())
	{
		FMOD_RESULT rc = m_pChannel->setPaused( true);
		ERRCHECK( rc);

		rc = m_pChannel->setVolume( 0.0f);
		ERRCHECK( rc);
	}

	AddState( I3SND_RT_PLAYSTATE_PAUSE);

	return true;
}

bool i3SoundPlayStateFMOD::Mute(bool bMute)
{
	I3ASSERT( m_pChannel != nullptr);

	if( isMuted() != bMute)
	{
		FMOD_RESULT rc = m_pChannel->setMute( bMute);
		ERRCHECK( rc);
	}

	if( bMute )	AddState( I3SND_RT_PLAYSTATE_MUTE);
	else		RemoveState( I3SND_RT_PLAYSTATE_MUTE);

	return true;
}

bool i3SoundPlayStateFMOD:: Resume(void)
{
	I3ASSERT( m_pChannel != nullptr);

	if( isPaused())
	{
		SetVolume( m_nVolume);

		FMOD_RESULT rc = m_pChannel->setPaused( false);
		ERRCHECK( rc);
	}

	RemoveState( I3SND_RT_PLAYSTATE_PAUSE);

	return true;
}

bool i3SoundPlayStateFMOD:: SetVolume( INT32 vol)
{
	I3ASSERT( m_pChannel != nullptr);

	m_nVolume = vol;

	INT32 nVolume = _calcCtxVolume();

	FMOD_RESULT rc = m_pChannel->setVolume( (REAL32) nVolume * 0.01f);
	ERRCHECK( rc);
		
	return true;
}

bool i3SoundPlayStateFMOD:: SetPitch( INT32 pitch)
{
	I3ASSERT( m_pChannel != nullptr);

	m_nPitch = pitch;

	FMOD_RESULT rc = m_pChannel->setFrequency( (REAL32) m_nPitch);
	ERRCHECK( rc);

	return true;
}

void i3SoundPlayStateFMOD::Set3DParams()
{
 	i3SoundPlayState::Set3DParams();
	FMOD_VECTOR vec;
	//FMOD_MODE mode;
	//FMOD_RESULT rv;

	if( isListnerSound())
	{
		/*
		rv = m_pChannel->setMode( FMOD_3D_HEADRELATIVE); 
		ERRCHECK( rv);

		vec.x = 0.0f;	vec.y = 0.0f;	vec.z = 0.0f;
		rv = m_pChannel->set3DAttributes( &vec, &vec);
		ERRCHECK( rv);

		rv = m_pChannel->set3DConeOrientation( (FMOD_VECTOR *) &I3_ZAXIS);
		ERRCHECK( rv);

		rv = m_pChannel->set3DConeSettings(	360.0f, 360.0f, (REAL32)m_pInfo->getOutsideVolume() * 0.01f);
		ERRCHECK( rv);
		*/
	}
	else
	{
		// Min, Max Distance
		m_pChannel->set3DMinMaxDistance( m_pInfo->getMinDistance(), 10000.0f); //m_pInfo->getMaxDistance());
		//I3TRACE( "%f, %f\n", m_pInfo->getMinDistance(), m_pInfo->getMaxDistance());

		// Position & Velocity
		FMOD_VECTOR pos, vel;

		pos.x = m_vecPos.x + I3_EPS;	pos.y = m_vecPos.y + I3_EPS;	pos.z = m_vecPos.z + I3_EPS;
		vel.x = m_vVelocity.x;			vel.y = m_vVelocity.y;			vel.z = m_vVelocity.z;
		//vel.x = 0.0f;					vel.y = 0.0f;					vel.z = 0.0f;
		m_pChannel->set3DAttributes( &pos, &vel);

		// Cone Orientation
		vec.x = m_vDirection.x;		vec.y = m_vDirection.y;		vec.z = m_vDirection.z;
		m_pChannel->set3DConeOrientation( &vec);

		m_pChannel->set3DConeSettings(	(REAL32) m_pInfo->getInsideConeAngle(),				// Degree
										(REAL32) m_pInfo->getOutsideConeAngle(),			// Degree
										(REAL32)m_pInfo->getOutsideVolume() * 0.01f			// Volume은 0~100의 값을 가진다.
										);
	
		switch( m_pInfo->Get3DMode())
		{
			//표준 LISTENER연관 3DPOSITIONING 
			case I3SND_RT_3DMODE_WORLD_SPACE :		
				m_pChannel->setMode( FMOD_3D_WORLDRELATIVE);	
				break;

			case I3SND_RT_3DMODE_CAMERA_SPACE :		
				m_pChannel->setMode( FMOD_3D_HEADRELATIVE);	
				break;
		}
	}
}

bool i3SoundPlayStateFMOD::Drive()
{
	/*
	bool bPlaying;

	//버퍼의 상태 검사
	FMOD_RESULT rc = m_pChannel->isPlaying( &bPlaying);
	ERRCHECK( rc);

	if( bPlaying == false)
	{
		if( isPaused() == false)
		{
			SetPlayState( I3SND_RT_PLAYSTATE_FREE);
			return false;
		}
		else
		{
			SetPlayState( I3SND_RT_PLAYSTATE_PAUSE);
		}
	}
	*/

	return true;
}

bool i3SoundPlayStateFMOD::UpdatePos( MATRIX * pMat)
{
	FMOD_RESULT rc;

	if( isFree())								return false;
	if( isListnerSound())						return false;

	I3ASSERT( m_pChannel != nullptr);

	if( pMat == nullptr)
		return true;

	if( !_UpdateLocationParam( m_pInfo, pMat))
	{
		return false;
	}

	if( i3Vector::isAlike( &m_vecPos, &m_vecOldPos) && i3Vector::isAlike( &m_vDirection, &m_vOldDirection))
	{
		return true;
	}

	FMOD_VECTOR pos, vec;

	vec.x = m_vDirection.x;
	vec.y = m_vDirection.y;
	vec.z = m_vDirection.z;
	rc = m_pChannel->set3DConeOrientation( &vec);

	pos.x = m_vecPos.x;
	pos.y = m_vecPos.y;
	pos.z = m_vecPos.z;

	vec.x = m_vVelocity.x;
	vec.y = m_vVelocity.y;
	vec.z = m_vVelocity.z;
	rc = m_pChannel->set3DAttributes( &pos, &vec);

	UpdateMuteState();
	
	return true;
}

bool i3SoundPlayStateFMOD::UpdateMuteState(void)
{
	if( isFree())			return false;
	if( isListnerSound())	return false;
	if( !m_pInfo->isMuteAtMaxDistance()) return false;

	MATRIX mtx;
	i3Matrix::SetTranslate( &mtx, &m_vecPos);

	//max distance밖,, 즉 청취범위 밖의 소리라면 묵음처리 한다.
	bool bMute = !g_pSoundCtx->isAudioable( &mtx, m_pInfo->getMaxDistance());

	if( isLoop())
	{
		if( bMute)
		{
			if( !isPaused()) Pause();
		}
		else
		{
			if( isPaused())	Resume();
		}
	}
	else
	{
		if( bMute != isMuted())
		{
			Mute( bMute);
		}
	}

	return bMute;
}

#endif
