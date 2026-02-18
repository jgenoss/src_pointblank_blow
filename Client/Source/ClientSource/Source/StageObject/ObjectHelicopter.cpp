#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectHelicopter.h"

const char * s_AnimName_Helicopter[] =
{
	"Object/cgame/Uh60_Idle01.i3a",
	"Object/cgame/Uh60_Idle02.i3a",
	"Object/cgame/Uh60_Crash.i3a",
};

I3_CLASS_INSTANCE( CGameObjectHelicopter);//, CGameControledObject);

CGameObjectHelicopter::CGameObjectHelicopter(void)
{
	//Animation
	m_AnimKey			= HA_NONE;
	m_rAnimLocalTime	= 0.0f;

	i3mem::FillZero( &m_pAnim, sizeof( i3Animation*) * HA_COUNT);
	
	m_pUH60_DustEffect			= nullptr;

	i3Vector::Zero( &m_vFieldPos);
}

CGameObjectHelicopter::~CGameObjectHelicopter(void)
{
	for( INT32 i = 0; i < HA_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pAnim[i]);
	}
}

void CGameObjectHelicopter::InitObject()
{
	InitAnimation();

	m_bManualUpdate = true;
}

void CGameObjectHelicopter::ResetObject()
{
	EnableDustEffect( false);

	m_rAnimLocalTime	= 0.f;

	SetAnimationKey( HA_IDLE);
}

void CGameObjectHelicopter::EnableObject()
{
	EnableDustEffect( true);

	SetAnimationKey( HA_IDLE);
}

void CGameObjectHelicopter::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
	Process( rDeltaSeconds);
}

void CGameObjectHelicopter::InitAnimation( void)
{	
	for( INT32 i = 0; i < HA_COUNT; i++)
	{
		if( s_AnimName_Helicopter[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_Helicopter[i]);
			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_Helicopter[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	m_pSceneObj->BuildBoneList();

	SetAnimationKey( HA_IDLE);
}

void CGameObjectHelicopter::SetAnimationKey( HELICOPTER_ANIM animKey)
{
	if( animKey != HA_NONE && m_pAnim[animKey] != nullptr )
	{
		if( animKey == HA_CRASH )
		{
			m_pSceneObj->PlayAnim( m_pAnim[animKey], I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, 1.0f, I3_ANIM_PLAY_MODE_CLAMP, I3_ANIM_PLAY_MODE_STOP);
		}
		else
		{
			m_pSceneObj->PlayAnim( m_pAnim[animKey]);
		}

		m_rAnimLocalTime	= 0.f;
	}

	m_AnimKey = animKey;
}

void CGameObjectHelicopter::EnableDustEffect( bool bEnable)
{
	if( bEnable)
	{
		if( m_pUH60_DustEffect == nullptr )
		{
			VEC3D vec;
			i3Vector::Set( &vec, 0.f, -1.f, 0.f);

			i3CollideeLine line;
			line.SetStart( GetPos());
			line.SetDir( &vec);

			I3_PHYSIX_HIT_RESULT * pResult;
			pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
			if( pResult != nullptr)
			{
				I3ASSERT( pResult->m_pDestShapeSet != nullptr);
				i3Vector::Copy( &m_vFieldPos, GetPos());
				i3Vector::AddY( &m_vFieldPos, - pResult->m_T);

				m_pUH60_DustEffect = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_UH60_DUST_COMMON, &m_vFieldPos, &I3_YAXIS);
			}
		}
		else
		{
			if( m_pUH60_DustEffect->IsFlag( I3_NODEFLAG_DISABLE))
			{
				m_pUH60_DustEffect = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_UH60_DUST_COMMON, &m_vFieldPos, &I3_YAXIS);
			}
		}
	}
	else
	{
		if( m_pUH60_DustEffect != nullptr)
		{	
			m_pUH60_DustEffect->DeactivateNode();
			m_pUH60_DustEffect = nullptr;
		}
	}
}

void CGameObjectHelicopter::Process( REAL32 rDeltaSeconds)
{
	m_rAnimLocalTime += rDeltaSeconds;

	switch( getAnimationKey() )
	{
	case HA_IDLE :
		if( getCurState() == 1 )
		{
			SetAnimationKey( HA_HIT);
		}
		break;
	case HA_HIT :
		if( getCurState() == 2 )
		{
			SetAnimationKey( HA_CRASH);
		}
		break;
	case HA_CRASH :
		{
			if( m_rAnimLocalTime > 1.f)
			{
				// 일정 시간후 먼지 이팩트를 끕니다.
				EnableDustEffect( false);

				SetAnimationKey( HA_NONE);
			}
		}
		break;
	}
}

void CGameObjectHelicopter::SetControledObjectState( INT32 state)
{

}