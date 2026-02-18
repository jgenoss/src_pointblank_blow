#include "pch.h"
#include "ObjectHatchling.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CGameObjectHatchling);//, CGameControledObject);


const char * s_AnimName_Hatchling[] =
{
	"Object/moving/Hatchling/Hatchling_wait.i3a",
	"Object/moving/Hatchling/Hatchling_Damaged.i3a",
};


CGameObjectHatchling::CGameObjectHatchling()
{
	m_AnimKey			= HATA_NONE;
	m_rAnimLocalTime	= 0.0f;

	i3mem::FillZero( m_pAnim,	sizeof( i3Animation*) * HATA_COUNT);
}

CGameObjectHatchling::~CGameObjectHatchling()
{
	for( INT32 i = 0; i < HATA_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pAnim[i]);
	}
}

void CGameObjectHatchling::_InitAnimation( void)
{
	for( INT32 i = 0; i < HATA_COUNT; i++)
	{
		if( s_AnimName_Hatchling[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_Hatchling[i]);
			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_Hatchling[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	SetAnimationKey( HATA_IDLE);
}

void CGameObjectHatchling::InitObject()
{
	_InitAnimation();
}

void CGameObjectHatchling::ResetObject()
{
}

void CGameObjectHatchling::EnableObject()
{
}



void CGameObjectHatchling::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);

	m_rAnimLocalTime += rDeltaSeconds;

	switch( m_AnimKey)
	{
	case HATA_IDLE :
		break;

	case HATA_DAMAGE :
		{
			i3Body * pBody = getSceneObject()->GetBody();
			if( m_rAnimLocalTime >= pBody->GetDuration() )
			{
				SetAnimationKey( HATA_IDLE);
			}
		}
		break;
	}
}


void CGameObjectHatchling::SetControledObjectState( INT32 state)
{
	SetAnimationKey( (HATCHLING_ANIM) state);
}

void CGameObjectHatchling::SetAnimationKey( HATCHLING_ANIM animKey)
{
	if( animKey != HATA_NONE && m_pAnim[animKey] != nullptr )
	{
		if( m_AnimKey == HATA_DAMAGE && animKey == HATA_DAMAGE )
		{
			return;
		}

		if( animKey == HATA_IDLE)
		{
		}
		else if( animKey == HATA_DAMAGE)
		{
		}

		m_pSceneObj->PlayAnim( m_pAnim[animKey]);

		m_rAnimLocalTime	= 0.f;
	}

	m_AnimKey = animKey;
}

