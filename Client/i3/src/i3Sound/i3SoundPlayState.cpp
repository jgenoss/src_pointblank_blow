
#include "stdafx.h"
#include "i3SoundPlayState.h"
#include "i3SoundStreamThread.h"
#include "mmsystem.h"
#include "i3SoundGlobal.h"

//I3_ABSTRACT_CLASS_INSTANCE( i3SoundPlayState, i3ElementBase );
I3_CLASS_INSTANCE( i3SoundPlayState);


INT32	i3SoundPlayState::m_nCount			= 0;
bool	i3SoundPlayState::m_bChangedVolume	= false;

i3SoundPlayState::~i3SoundPlayState(void)
{
	I3_SAFE_RELEASE( m_pInfo);
}

void i3SoundPlayState::SetPlayState( UINT32 nState)
{
	m_nState &= ~I3SND_RT_MASK_PLAYSTATE;

	AddState( nState);
}

bool i3SoundPlayState::SetSoundPlayInfo( i3SoundPlayInfo * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	I3_REF_CHANGE( m_pInfo, pInfo);

	if( m_pInfo == nullptr)
		return false;

	SetState( I3SND_RT_PLAYSTATE_READY);
	AddState( m_pInfo->GetSoundType());
	AddState( m_pInfo->GetPriorityType());
	AddState( m_pInfo->GetStreamType());
	AddState( m_pInfo->Get3DMode());
	AddState( m_pInfo->GetLoopMode());

	m_nVolume = m_pInfo->GetDefaultVolume();

	return true;
}

INT32 i3SoundPlayState::_calcCtxVolume(void)
{
	I3ASSERT( m_pInfo != nullptr);

	INT32 nVolume = m_pInfo->GetDefaultVolume();

	if( isListnerSound() )
	{
		nVolume = m_pInfo->getListenerVolume();
	}

	INT32 nCtxVolume   = g_pSoundCtx->getMainVolume();
	INT32 nTypeVolume  = g_pSoundCtx->GetTypeVolume( m_nState & I3SND_PROP_MASK_TYPE);

	return (INT32)(nVolume * nCtxVolume * nTypeVolume * 0.0001f);
}

bool i3SoundPlayState::_UpdateLocationParam( i3SoundPlayInfo * pInfo, MATRIX * pMat)
{
	if( pInfo == nullptr)
		return false;

	if( pMat != nullptr)
	{
		i3Vector::Copy( &m_vecOldPos, &m_vecPos);
		i3Vector::Copy( &m_vOldDirection, &m_vDirection);
		i3Vector::Copy( &m_vOldVelocity, &m_vVelocity);

		i3Vector::TransformCoord( &m_vecPos, pInfo->getPosition(), pMat);

		VEC3D vZero;

		if( i3Vector::isAlike( pInfo->getDirection(), &vZero))
		{
			i3Vector::Sub(&m_vDirection, i3Matrix::GetPos( g_pSoundCtx->GetListnerMatrix()), &m_vecPos);
			i3Vector::Normalize(&m_vDirection, &m_vDirection);
		}
		else
		{
			i3Matrix::TransformPoints( &m_vDirection, pInfo->getDirection(), 1, pMat);
			i3Vector::Normalize(&m_vDirection, &m_vDirection);
		}

		return true;
	}

	return false;
}

void i3SoundPlayState::Set3DParams()
{
}

void i3SoundPlayState::Reset(void)
{
	SetPlayState( I3SND_RT_PLAYSTATE_FREE);

	I3_SAFE_RELEASE( m_pInfo);
}

