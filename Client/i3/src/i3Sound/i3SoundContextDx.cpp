#include "stdafx.h"
#include "i3SoundContext.h" 

#if defined( I3SND_DSOUND)
#include "i3SoundDx.h"
#include "i3SoundPlayStateDx.h"
#include "i3SoundContextDX.h"

#include "i3Input/dxerr.h"

#include "i3SoundGlobal.h"

I3_CLASS_INSTANCE( i3SoundContextDX);

#define	READYLIST_ENTER				m_pReadyListMutex->Enter()
#define	READYLIST_LEAVE				m_pReadyListMutex->Leave()
#define	PLAYLIST_ENTER				m_pPlayListMutex->Enter()
#define	PLAYLIST_LEAVE				m_pPlayListMutex->Leave()
#define	UPDATELIST_ENTER			m_pUpdateListMutex->Enter()
#define	UPDATELIST_LEAVE			m_pUpdateListMutex->Leave()

i3SoundContextDX::i3SoundContextDX(void)
{
	m_hdsound3d = LoadLibrary( "dsound3d.dll");


	m_pPlayThread			= nullptr;
	m_pUpdateThread			= nullptr;
	m_bNeedRaisePlaythread	= false;
	
	m_pReadyListMutex	= i3Mutex::new_object();
	m_pPlayListMutex	= i3Mutex::new_object();
	m_pUpdateListMutex	= i3Mutex::new_object();

	//Control Play/Update state list
	m_pHead_ReqPlay			= i3SoundPlayStateDx::new_object();	//Play Request가 들어온 State
	m_pHead_Update			= i3SoundPlayStateDx::new_object();	//Update( Drive) 할 State
	m_pHead_Ready			= i3SoundPlayStateDx::new_object();	//놀고있는 스테이트

	m_rUpdateTime			= 0.0f;
}

i3SoundContextDX::~i3SoundContextDX(void)
{
	{
		if( m_pPlayThread != nullptr)
		{
			m_pPlayThread->Terminate();
			m_pPlayThread->Release();
			m_pPlayThread = nullptr;
		}

		if( m_pUpdateThread != nullptr)
		{
			m_pUpdateThread->Terminate();
			m_pUpdateThread->Release();
			m_pUpdateThread = nullptr;
		}
	}

	if( m_hdsound3d != nullptr)
	{
		FreeLibrary(m_hdsound3d);
	}

	I3_SAFE_RELEASE( m_pReadyListMutex);
	I3_SAFE_RELEASE( m_pPlayListMutex);
	I3_SAFE_RELEASE( m_pUpdateListMutex);

	I3_SAFE_RELEASE( m_pHead_ReqPlay);
	I3_SAFE_RELEASE( m_pHead_Update);
	I3_SAFE_RELEASE( m_pHead_Ready);
}

bool i3SoundContextDX::Create( I3SND_CONTEXTCREATE_PARAM * pInitParam)
{
	i3SoundContext::Create( pInitParam);

	_LinkPlayStateList();

	if( CreatePrimaryBuffer( (HWND)m_InitParam.m_nPlatformData ) == false)
	{
		I3TRACE("Failed CreatePrimaryBuffer\n");
		return false;
	}

	if( m_InitParam.m_bHasListener)
	{
		GetListenerObject();
	}

	// Play Thread
	{
		m_pPlayThread = i3SoundPlayThread::new_object();

		m_pPlayThread->setPlayStateListHead( m_pHead_ReqPlay);
		m_pPlayThread->setUpdateStateListHead( m_pHead_Update);

		m_pPlayThread->setPlayMutex( m_pPlayListMutex);
		m_pPlayThread->setUpdateMutex( m_pUpdateListMutex);
		if( m_pPlayThread->Create( "SndPlayThread", 0, 4096, 0, PRIORITY_TIME_CRITICAL) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not create sound play thread");
		}

		m_pUpdateThread = i3SoundUpdateThread::new_object();
		m_pUpdateThread->setUpdateStateListHead( m_pHead_Update);
		m_pUpdateThread->setReadyStateListHead( m_pHead_Ready);
		
		m_pUpdateThread->setUpdateMutex( m_pUpdateListMutex);
		m_pUpdateThread->setReadyMutex( m_pReadyListMutex);

		if( m_pUpdateThread->Create( "SndUpdateThread", 0, 4096, 0, PRIORITY_HIGH) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not create sound Update thread");
		}

		//SetPendingEnable( !m_InitParam.m_bInitialState);
		//SetPendingEnable( true);
	}

	return true;
}

void i3SoundContextDX::_LinkPlayStateList(void)
{
	INT32 i;
	i3SoundPlayStateDx * pState;

	for( i = 0; i < m_SoundPlayStateList.GetCount(); i++)
	{
		pState = (i3SoundPlayStateDx *) m_SoundPlayStateList.Items[i];

		pState->SetNext( m_pHead_Ready->getNext());
		m_pHead_Ready->SetNext( pState);
	}
}

i3SoundPlayState	*	i3SoundContextDX::GetFreePlayState( void)
{
	i3SoundPlayStateDx * pState = nullptr;

	READYLIST_ENTER;
		pState = getReadyHead()->getNext();
	READYLIST_LEAVE;

	if( pState == nullptr)
	{
		pState = (i3SoundPlayStateDx *) AddNewPlayState();
	}
	else
	{
		//Ready List에서 떼어내고
		READYLIST_ENTER;
		{
			getReadyHead()->SetNext( pState->getNext());
		}
		READYLIST_LEAVE;

		pState->SetNext( nullptr);

		//준비상태인 스테이트가 아니면 Update List로 다시 보내고
		if( pState->isFree() != true)
		{
			UPDATELIST_ENTER;
			{
				i3SoundPlayStateDx * pNext = getUpdateHead();
				while(( pNext != nullptr) && (pNext->getNext()))
				{
					pNext = pNext->getNext();
				}

				pNext->SetNext( pState);
			}
			UPDATELIST_LEAVE;

			return GetFreePlayState();
		}
	}

	return pState;
}

void i3SoundContextDX::StopAllSound(bool bForce)
{
	SetPendingEnable( true);

	INT32	i = 0;
	INT32	nCount = m_SoundPlayStateList.GetCount();

	if( bForce)
	{
		for( i = 0; i < nCount ; ++i)
		{
			i3SoundPlayState * pState = (i3SoundPlayState*)m_SoundPlayStateList.GetItem( i );

			Stop( pState, bForce);
		}
	}
	else
	{
		i3SoundPlayStateDx::addGlobalRequest( I3SND_RT_REQ_STOP);
	}

	SetPendingEnable( false);
}

i3SoundPlayState	*	i3SoundContextDX::RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, VEC3D * pPos)
{
	MATRIX mtx;

	i3Matrix::SetTranslate( &mtx, pPos);

	return RePlay( pState, pInfo, &mtx);
}

i3SoundPlayState	*	i3SoundContextDX::RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, MATRIX * pMat)
{
	i3SoundPlayStateDx * pStateDX = (i3SoundPlayStateDx *) pState;

	if( pState == nullptr)
	{
		return Play( pInfo, pMat);
	}
	else
	{
		bool bValid = false;

		//현재 업데이트중인지 체크하고 그렇지 못하면 다시 플레이 신청
		UPDATELIST_ENTER;
		i3SoundPlayStateDx * pUpdate = getUpdateHead();
		while( pUpdate != nullptr)
		{
			if( pUpdate == pState)
			{
				bValid = true;
				break;
			}

			pUpdate = pUpdate->getNext();
		}
		UPDATELIST_LEAVE;

		
		if( bValid == false)
		{
			return Play( pInfo, pMat);
		}
	}

	//Update List에서 떼어내고
	UPDATELIST_ENTER;
	{
		pStateDX->getPrev()->SetNext( pStateDX->getNext());
		pStateDX->SetNext( nullptr);
	}
	UPDATELIST_LEAVE;

	if( pState != nullptr && pStateDX->Replay( pInfo, pMat))
	{
		PLAYLIST_ENTER;
		{
			i3SoundPlayStateDx * pNext = getPlayHead();
			while(( pNext != nullptr) && (pNext->getNext()))
			{
				pNext = pNext->getNext();
			}

			pNext->SetNext( (i3SoundPlayStateDx *) pState);
		}
		PLAYLIST_LEAVE;

		return pState;
	}
	else
	{
		return Play( pInfo, pMat);
	}
}

i3SoundPlayState*	i3SoundContextDX::Play( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat)
{
	i3SoundPlayStateDx * pState = (i3SoundPlayStateDx *) GetFreePlayState();

	#if defined ( I3G_PSP ) && defined (I3G_PS2)
		VEC3D vecPosOnCameraWorld;
		VEC3D *pVecPos = nullptr;

		if( pMat != nullptr)
		{
			_CalculatePosByListener( &vecPosOnCameraWorld, i3Matrix::GetPos( pMat));
			pVecPos = &vecPosOnCameraWorld;
		}
	#endif

		I3TRACE( "PLAY : %s\n", pSoundInfo->getSound()->GetName());

	if( pState->Play( pSoundInfo, pMat))
	{
		#if !defined( PLAY_MULTHREAD)
		if( pState->CMD_Play())
		{
		#endif

			i3SoundPlayStateDx * pNext = nullptr;
			{
				#if defined( PLAY_MULTHREAD)
					PLAYLIST_ENTER;
					pNext = getPlayHead();
				#else
					UPDATELIST_ENTER;
					pNext = getUpdateHead();
				#endif

				while(( pNext != nullptr) && (pNext->getNext()))
				{
					pNext = pNext->getNext();
				}

				I3ASSERT( pNext != nullptr);
				pNext->SetNext( pState);

				#if defined( PLAY_MULTHREAD)
					PLAYLIST_LEAVE;
				#else
					UPDATELIST_LEAVE;
				#endif
			}
			
			return pState;		/// <<<<<<<<<<< return

		#if !defined( PLAY_MULTHREAD)
		}
		#endif
	}
	
	// 할당에 실패한 경우..
	{
		i3SoundPlayStateDx * pNext = nullptr;
		READYLIST_ENTER;
		{
			pNext = getReadyHead();
			while(( pNext != nullptr) && (pNext->getNext()))
			{
				pNext = pNext->getNext();
			}
		}

		I3ASSERT( pNext != nullptr);
		pNext->SetNext( pState);

		READYLIST_LEAVE;
	}

	return nullptr;
}

bool	i3SoundContextDX::Stop( i3SoundPlayState * pState, bool bForce )
{
	if( pState == nullptr)
		return true;

	pState->Stop( bForce);

	return true;
}


void i3SoundContextDX::SetListnerMatrix(MATRIX* pMat, REAL32 rDeltaSeconds)
{
	i3SoundContext::SetListnerMatrix( pMat, rDeltaSeconds);

	setNeedUpdateListener();
}

void	i3SoundContextDX::_CalculatePosByListener( VEC3D * pPos, VEC3D * pSrcPos)
{
	if( pSrcPos == nullptr)
	{
		i3Vector::Zero( pPos);
		return ;
	}
}

void i3SoundContextDX::_UpdateListenerParam()
{
	if( m_InitParam.m_bHasListener == false)
		return ;

	if( m_pDSListener == nullptr)
		return;

	if( m_bChangedListenerMatrix == false)
		return;

	m_dsListenerParams.dwSize = sizeof( DS3DLISTENER );

	m_pDSListener->GetAllParameters( &m_dsListenerParams );
	
	VEC3D * pvecPos = i3Matrix::GetPos( &m_MtxListener);
	VEC3D * pvecAt	= i3Matrix::GetAt( &m_MtxListener);
	VEC3D * pvecUp	= i3Matrix::GetUp( &m_MtxListener);

	m_dsListenerParams.vVelocity.x  = getX( &m_vListenerVelocity);
	m_dsListenerParams.vVelocity.y  = getZ( &m_vListenerVelocity);
	m_dsListenerParams.vVelocity.z  = getY( &m_vListenerVelocity);

	m_dsListenerParams.vPosition.x = getX( pvecPos);
	m_dsListenerParams.vPosition.y = getY( pvecPos);
	m_dsListenerParams.vPosition.z = getZ( pvecPos);

	m_dsListenerParams.vOrientFront.x = getX( pvecAt);
	m_dsListenerParams.vOrientFront.y = getY( pvecAt);
	m_dsListenerParams.vOrientFront.z = getZ( pvecAt);

	m_dsListenerParams.vOrientTop.x = getX( pvecUp);
	m_dsListenerParams.vOrientTop.y = getY( pvecUp);
	m_dsListenerParams.vOrientTop.z = getZ( pvecUp);

	m_dsListenerParams.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;
	m_dsListenerParams.flDopplerFactor	= DS3D_DEFAULTDOPPLERFACTOR;
	m_dsListenerParams.flRolloffFactor	= DS3D_DEFAULTROLLOFFFACTOR;
	
	{
		//m_pDSListener->SetAllParameters( &m_dsListenerParams, DS3D_DEFERRED );
		m_pDSListener->SetAllParameters( &m_dsListenerParams, DS3D_IMMEDIATE );
	}

	i3SoundContext::_UpdateListenerParam();
}

bool i3SoundContextDX::GetListenerObject()
{
	HRESULT hr;
	LPDIRECTSOUNDBUFFER	pDSBPrimary = NULL;
	DSBUFFERDESC	dsbd;

	//SetListener////////////////////////////////////////////////////////////
	i3mem::FillZero( &dsbd, sizeof( DSBUFFERDESC ));
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL )))
	{
		DXTRACE_ERR("g_pDS->CreateSoundBuffer(DSBPRIMARY) Fail In GetListenerObject(): %X\n", hr );
		return false;
	}

	if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, (VOID**)&m_pDSListener)))
	{
		DXTRACE_ERR("pDSBPrimary->QueryInterface() Fail In GetListenerObject(): %X\n", hr );
		I3_SAFE_RELEASE( pDSBPrimary );
		return false;
	}

	I3_SAFE_RELEASE( pDSBPrimary );
	return true;
}

//Set PrimaryBuffer Format
bool i3SoundContextDX::CreatePrimaryBuffer( HWND PlatformData )
{
	HRESULT hr;
	LPDIRECTSOUNDBUFFER	pDSBPrimary = NULL;
/*
	///////////////////////////////////////////////////////////
	if( FAILED( hr = DirectSoundCreate8( NULL, &g_pDS, NULL )))
	{
		DXTRACE_ERR( "DirectSoundCreate8", hr);
		return false;
	}

	if( g_pDS == nullptr )	return false;
*/
	{
		HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);
		if( FAILED( hr))
		{
			//release g_pDs
			DXTRACE_ERR( "CoInitializeEx()", hr);
		}

		hr = CoCreateInstance( CLSID_DirectSound8,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IDirectSound8,
			(LPVOID*) &g_pDS);

		if( FAILED( hr))
		{
			//release g_pDs
			DXTRACE_ERR( "CoCreateInstance()", hr);
			return false;
		}

		hr = g_pDS->Initialize( nullptr);
		if( FAILED( hr))
		{
			//release g_pDs
			DXTRACE_ERR( "CoCreateInstance()", hr);
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////
	hr = g_pDS->SetCooperativeLevel( (HWND)m_InitParam.m_nPlatformData, DSSCL_PRIORITY );

	{
		DSBUFFERDESC	dsbd;
		i3mem::FillZero( &dsbd, sizeof( DSBUFFERDESC ));

		dsbd.dwSize			= sizeof( DSBUFFERDESC );
		dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes	= 0;
		dsbd.lpwfxFormat	= nullptr;

		//Create Primary Buffer/////////////////////////////////////////////////
		if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, nullptr )))
		{
			DXTRACE_ERR( "CreateSoundBuffer", hr);
			return false;
		}
	}

	{
		DSCAPS cap;
		i3mem::FillZero( &cap, sizeof( DSCAPS));
		cap.dwSize  = sizeof( DSCAPS);

		if( FAILED( hr = g_pDS->GetCaps( &cap)))
		{
			I3TRACE("failed getting caps\n");
		}

		if( cap.dwMaxHwMixingAllBuffers > 1)
		{
			m_nState |= I3SND_CMN_USEACCELERATION;
		}
	}

	//Setting Wav File Format
	{
		WAVEFORMATEX	wfx;
		i3mem::FillZero( &wfx, sizeof( WAVEFORMATEX));

		wfx.wFormatTag			= WAVE_FORMAT_PCM;
		wfx.nChannels			= (WORD)I3SND_WAV_CHANNELS;
		wfx.nSamplesPerSec		= I3SND_WAV_SAMPLERATE;
		wfx.nBlockAlign			= I3SND_WAV_BLOCKALIGN;
		wfx.wBitsPerSample		= (WORD)I3SND_WAV_BITSPERSAMPLE;
		wfx.nAvgBytesPerSec		= wfx.nSamplesPerSec * wfx.nBlockAlign;

		////////////////////////////////////////////////////////////////////////
		if( FAILED( hr = pDSBPrimary->SetFormat( &wfx )))
		{
			DXTRACE_ERR( "pDSBPrimary->SetFormat( &wfx )", hr);
			return false;
		}
	}

	I3_SAFE_RELEASE( pDSBPrimary );

	return true;
}

void i3SoundContextDX::UpdatePos( i3SoundPlayState * pPlayState, VEC3D * pPos, REAL32 rDeltaSeconds)
{
	MATRIX tmp;

	i3Matrix::SetTranslate( &tmp, pPos);

	pPlayState->UpdatePos( &tmp);
}

void i3SoundContextDX::UpdatePos( i3SoundPlayState * pPlayState, MATRIX * pMat, REAL32 rDeltaSeconds)
{
	if( pPlayState == nullptr)	return ;
	if( pMat == nullptr)	return ;

	pPlayState->UpdatePos( pMat);
}

//Listener 의 위치 및 방향을 업데이트 해준다.
void	i3SoundContextDX::UpdateListener()
{
	if( m_pDSListener == nullptr)
		return;

	i3SoundContext::UpdateListener();

	m_bNeedUpdateListener = false;
}

void i3SoundContextDX::setMainVolume( UINT8 nVolume)
{
	i3SoundContext::setMainVolume( nVolume);

	i3SoundPlayStateDx::addGlobalRequest( I3SND_RT_REQ_VOLUME);
}

void i3SoundContextDX::SetTypeVolume( UINT32 nType, UINT8 nVolume)
{
	i3SoundContext::SetTypeVolume( nType, nVolume);

	i3SoundPlayStateDx::addGlobalRequest( I3SND_RT_REQ_VOLUME);
}


void	i3SoundContextDX::OnDrive(REAL32 rDeltaSeconds)
{
	m_rUpdateTime += rDeltaSeconds;

	if(( m_pPlayThread != nullptr) && ( getPlayHead()->getNext() != nullptr))
	{
		m_pPlayThread->WakeUp();
	}

	if( m_rUpdateTime > 0.128f)
	{
		if( m_pUpdateThread != nullptr)
		{
			m_pUpdateThread->WakeUp();
		}

		m_rUpdateTime = i3Math::fmod( m_rUpdateTime, 0.128f);
	}
}

/*
DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT 
DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC 
DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL 
DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM 
DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL 
DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE 
DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA 
DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR 
DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY 
DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY 
DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR 
DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY 
DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST 
DSFX_I3DL2_ENVIRONMENT_PRESET_CITY 
DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS 
DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY 
DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN 
DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT 
DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE 
DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER 
DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM 
DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL 
DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL 
DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE 
*/

/*
void i3SoundContextDX::SetFXType( INT32 nFXType)
{
	m_nEffectType = nFXType;
}

void i3SoundContextDX::SetReverbParam( UINT32 nPresetIndex)
{
	m_nReverbPresetIndex = nPresetIndex;

	switch( nPresetIndex)
	{
		case DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_PADDEDCELL\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_BATHROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_LIVINGROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_STONEROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_AUDITORIUM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_CONCERTHALL\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_CAVE\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_ARENA\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_HANGAR\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_ALLEY\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_FOREST\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_CITY  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_CITY\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_QUARRY\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_PLAIN\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_PARKINGLOT\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_UNDERWATER\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL\n");
		break;
		case DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE  :
			I3TRACE("DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE\n");
		break;
	}
}
*/
#endif
