#include "stdafx.h"
#include "i3SoundContext.h" 
#include "i3SoundResManager.h"
#include "i3SoundGlobal.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3SoundContext);

/*-----------------------------------------------------------------------------
// Name: i3SoundContext::i3SoundContext(void)
// Desc: Create
//----------------------------------------------------------------------------*/
i3SoundContext::i3SoundContext(void)
{
	g_pSoundCtx				= this;
	
	m_SoundPlayStateList.reserve( 256);
	m_FreePlayStateList.reserve( 256);
	m_UsedPlayStateList.reserve( 256);
}


/*-----------------------------------------------------------------------------
// Name: i3SoundContext::~i3SoundContext(void)
// Desc: Destroy
//----------------------------------------------------------------------------*/
i3SoundContext::~i3SoundContext(void)
{
	Destroy();

	g_pSoundCtx = nullptr;
}

bool	i3SoundContext::Create( I3SND_CONTEXTCREATE_PARAM * pInitParam )
{
	I3ASSERT( pInitParam != nullptr);

	m_InitParam.m_nFlags			=	pInitParam->m_nFlags;
	m_InitParam.m_nVoiceCount		=	pInitParam->m_nVoiceCount;
	m_InitParam.m_bInitialState		=	pInitParam->m_bInitialState;
	m_InitParam.m_nPlatformData		=	pInitParam->m_nPlatformData;
	m_InitParam.m_bHasListener		=	pInitParam->m_bHasListener;

	PrepareContext();

	return true;
}

void i3SoundContext::SetPendingEnable( bool bPending)
{
	m_bPendingUpdate = bPending;
}


bool i3SoundContext::PrepareContext()
{
	i3SoundPlayState * pState = nullptr;
	INT32	i;

	for( i = 0; i < m_InitParam.m_nVoiceCount ; ++i)
	{
		pState = i3SoundPlayState::new_object();
		pState->SetIndex( i );

		m_SoundPlayStateList.push_back( pState );
		m_FreePlayStateList.push_back(pState);
	}

	return true;
}

i3SoundPlayState * i3SoundContext::AddNewPlayState()
{
	i3SoundPlayState * pState = i3SoundPlayState::new_object();

	pState->SetIndex( m_InitParam.m_nVoiceCount);

	m_InitParam.m_nVoiceCount++;

	m_SoundPlayStateList.push_back( pState );

	return pState;
}

bool i3SoundContext::Destroy()
{
	StopAllSound(true);
	ReleaseAllPlayState();
	ReleaseAllSound();

	return true;
}

void i3SoundContext::ReleaseAllSound()
{
//	m_SoundList.SafeReleaseAll();			// 쓰이지 않는듯 하여 주석..
}

void i3SoundContext::ReleaseAllPlayState()
{
	i3::cu::for_each_container(m_SoundPlayStateList, i3::fu::mem_fun_safe(&i3SoundPlayState::Release));
	m_SoundPlayStateList.clear();
	//m_SoundPlayStateList.SafeReleaseAll();
}

void i3SoundContext::FlushSound( bool bForce)
{
	StopAllSound(bForce);

	i3SoundResManager * pSndMng = (i3SoundResManager*)i3ResourceFile::GetResourceManager( i3Sound::static_meta());
	if( pSndMng != nullptr)
		pSndMng->FlushBuffers();
}

void				i3SoundContext::StopAllSound(bool bForce )
{
	
	i3SoundPlayState * pState;

	for(size_t i = 0; i < m_UsedPlayStateList.size(); i++)
	{
		bool	bStop = false;
		pState = m_UsedPlayStateList[i];

		if( bForce)
		{
			bStop = true;
		}
		else
		{
			if( !pState->isSoundOfType( I3SND_PROP_TYPE_BGM) && ( pState->isLoop() || (!pState->isListnerSound())))
			{
				bStop = true;
			}

			//땜빵입니다...이부분에 관한것은 따로좀..연구를...raja
			//스테이지 종료시 stopallsound가 호출되는 상황에서의 대응에관한 부분입니다.
			// 
			if( pState->getPlayInfo() != nullptr)
			{
				if( pState->getPlayInfo()->isForceStopAtStageEnd())
				{
					bStop = true;
				}
			}
		}

		if( bStop)
		{
			size_t size_before = m_UsedPlayStateList.size();

			pState->Stop();
			
			if ( size_before == m_UsedPlayStateList.size() )
				m_UsedPlayStateList.erase(m_UsedPlayStateList.begin() + i);

			--i;

		//	INT32 idx = m_FreePlayStateList.IndexOf( pState);
			size_t idx = i3::vu::index_of(m_FreePlayStateList, pState);
			if( idx == m_FreePlayStateList.size() )
			{
				m_FreePlayStateList.push_back( pState);
			}
		}
	}
}

i3SoundPlayState *	i3SoundContext::Play( i3SoundPlayInfo * pInfo, VEC3D * pPos, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	MATRIX tmx;
	i3SoundPlayState * pState;

	if( pPos != nullptr)
	{
		i3Matrix::SetTranslate( &tmx, pPos);

		pState = Play( pInfo, &tmx, GrpIdx, callback);
	}
	else
	{
		pState = Play( pInfo, (MATRIX *) nullptr, GrpIdx, callback);
	}

	return pState;
}

i3SoundPlayState *	i3SoundContext::GetFreePlayState(void)
{
	i3SoundPlayState * pFree;

	if( m_FreePlayStateList.size() <= 0)
		return nullptr;

	size_t idx = m_FreePlayStateList.size() - 1;
	pFree = m_FreePlayStateList[ idx];

	// Free List에서 제거
	m_FreePlayStateList.erase(m_FreePlayStateList.begin() + idx);

	// Used List에 추가
	m_UsedPlayStateList.push_back( pFree);

	return pFree;
}

i3SoundPlayState *	i3SoundContext::Play( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	i3SoundPlayState * pState = GetFreePlayState();

	if( pState == nullptr)
	{
		I3TRACE( "[SOUND] Not enough sound channel.\n");
		return nullptr;
	}

	//pState->SetSoundPlayInfo( pInfo);
	if( !pState->Play( pInfo, pMat, GrpIdx, callback))
	{
		
		size_t idx = i3::vu::index_of(m_UsedPlayStateList, pState);
		if( idx != m_UsedPlayStateList.size() )
		{
			m_UsedPlayStateList.erase(m_UsedPlayStateList.begin() + idx);
		}

		idx = i3::vu::index_of(m_FreePlayStateList, pState);
		if( idx == m_FreePlayStateList.size() )
		{
			m_FreePlayStateList.push_back( pState);
		}

		pState->Reset();

		return nullptr;
	}
	else
	{
		return pState;
	}
}

i3SoundPlayState *	i3SoundContext::RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, VEC3D * pPos, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	MATRIX mtx;

	i3Matrix::SetTranslate( &mtx, pPos);

	return RePlay( pState, pInfo, &mtx, GrpIdx, callback);
}

i3SoundPlayState *	i3SoundContext::RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx, FMOD_CHANNEL_CALLBACK callback)
{
	if( pState == nullptr)
	{
		return Play( pInfo, pMat, GrpIdx);
	}

	if( pState->Replay( pInfo, pMat, GrpIdx, callback) == false)
	{
		i3::vu::remove(m_UsedPlayStateList, pState);
		return Play( pInfo, pMat, GrpIdx, callback);
	}

	return pState;
}

bool				i3SoundContext::Stop( i3SoundPlayState * pState, bool bForce )
{
	if( pState->isFree() == false)
	{
		pState->Stop( bForce);

		//플레이 중이던 state의 위치는 channel end proc에서 처리하도록한다.
		/*
		m_UsedPlayStateList.Remove( pState);
		m_FreePlayStateList.Add( pState);
		*/
	}

	return true;
}

void				i3SoundContext::UpdatePos( i3SoundPlayState * pPlayState, VEC3D * pPos, REAL32 rDeltaSeconds)
{
	MATRIX mtx;

	i3Matrix::SetTranslate( &mtx, pPos);

	return UpdatePos( pPlayState, &mtx, rDeltaSeconds);
}

void				i3SoundContext::UpdatePos( i3SoundPlayState * pPlayState, MATRIX * pMat, REAL32 rDeltaSeconds)
{
	pPlayState->UpdatePos( pMat);
}

void i3SoundContext::_UpdateListenerParam(void)
{
	m_bChangedListenerMatrix = false;
}

void i3SoundContext::UpdateListener(void)
{
	if( !m_bChangedListenerMatrix || !m_InitParam.m_bHasListener)
		return;

	_UpdateListenerParam();
}

void i3SoundContext::SetListnerMatrix(MATRIX* pMat, REAL32 rDeltaSeconds)
{
	if( i3Matrix::IsSame( &m_MtxListener, pMat))
	{
		return;
	}

	//i3Vector::Sub( &m_vListenerVelocity, i3Matrix::GetPos( pMat), i3Matrix::GetPos( &m_MtxListener));
	//i3Vector::Scale( &m_vListenerVelocity, &m_vListenerVelocity, 1.0f / rDeltaSeconds);

	//도플러 효과 적용시에 카메라 위치가 갑자기 많이 변하는경우와같은 상황에서 소리가 희한하게 들립니다.
	//다른 방법을 찾지 않는다면 그냥 0으로 클리어 합니다.
	i3Vector::Zero( &m_vListenerVelocity);

	i3Matrix::Copy( &m_MtxListener, pMat);

	m_bChangedListenerMatrix = true;
}

//청취가 가능한 소리인지 여부 체크( 3d사운드인경우)
bool i3SoundContext::isAudioable( MATRIX * pMat, REAL32 rMaxDistance)
{
	if( rMaxDistance <= 0.0f || pMat == nullptr)
		return true;

	VEC3D vTmp;
	i3Vector::Sub( &vTmp, i3Matrix::GetPos( &m_MtxListener), i3Matrix::GetPos( pMat));
	return (i3Vector::LengthSq( &vTmp) < rMaxDistance * rMaxDistance);
}

void i3SoundContext::setMainVolume( UINT8 nVolume)
{
	UINT8 volume = (nVolume <= 0) ? 0 :
	( nVolume >= 100) ? 100 : nVolume;

	if( volume != m_nMainVolume)
	{
		m_nMainVolume = volume;
	}
}

UINT8 i3SoundContext::GetTypeVolume(UINT32 nType)
{
	UINT32 nT = nType >> 16;
	UINT32 nIdx = 0;

	while( nT > 0x01)
	{
		nT = nT >> 1;
		nIdx++;
	}

	return m_nTypeVolume[nIdx];
}

void i3SoundContext::SetTypeVolume( UINT32 nType, UINT8 nVolume)
{
	UINT32 nT = nType >> 16;
	UINT32 nIdx = 0;

	while( nT > 0x01)
	{
		nT = nT >> 1;
		nIdx++;
	}

	UINT8 volume = (nVolume <= 0) ? 0 :
	( nVolume >= 100) ? 100 : nVolume;
    
	if( m_nTypeVolume[nIdx] != volume)
	{
		m_nTypeVolume[nIdx] = volume;
	}
}

void i3SoundContext::OnDrive( REAL32 tm)
{
	UpdateListener();
}

i3::string i3SoundContext::ReportPerformance(void)
{
	return "";
}
