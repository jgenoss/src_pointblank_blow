#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectWildHogCannon.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "../Weapon/WeaponItemID.h"

I3_CLASS_INSTANCE( CGameObjectWildHogCannon);//, CGameControledObject);

const char * s_AnimName_WildHogCannon[] =
{
	"Object/cgame/Range_Golem/Range_Golem_Wait.i3a",
	"Object/cgame/Range_Golem/Range_Golem_SpAtk.i3a",
	"Object/cgame/Range_Golem/Range_Golem_Death.i3a",
};

CGameObjectWildHogCannon::CGameObjectWildHogCannon(void)
{
	m_AnimKey			= WHCA_NONE;
	m_rAnimLocalTime	= 0.0f;

	m_bAttacking = false;
	
	m_idxFxDummy		= -1;

	i3mem::FillZero( m_pAnim,	sizeof( i3Animation*) * WHCA_COUNT);

	m_bFireBall = false;
	m_rFireBallImpactTime = 0.f;
	m_nImpactCount = 0;

	i3Vector::Set( &m_vImpactPos[ 0], 18.f, 11.f, 11.f);
	i3Vector::Set( &m_vImpactPos[ 1], 14.f, 11.f, 12.f);
	i3Vector::Set( &m_vImpactPos[ 2], 13.f, 11.f, 12.f);
}

CGameObjectWildHogCannon::~CGameObjectWildHogCannon(void)
{
	for( INT32 i = 0; i < WHCA_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pAnim[i]);
	}
}

void CGameObjectWildHogCannon::InitObject()
{
	_InitAnimation();
	_InitFx();
}

void CGameObjectWildHogCannon::ResetObject()
{
	m_rAnimLocalTime	= 0.f;
	m_bAttacking		= false;
	m_bFireBall			= false;

	SetAnimationKey( WHCA_IDLE);
}

void CGameObjectWildHogCannon::EnableObject()
{
	SetAnimationKey( WHCA_IDLE);
}

void CGameObjectWildHogCannon::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
	Process( rDeltaSeconds);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CGameObjectWildHogCannon::_InitAnimation( void)
{
	for( INT32 i = 0; i < WHCA_COUNT; i++)
	{
		if( s_AnimName_WildHogCannon[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_WildHogCannon[i]);
			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_WildHogCannon[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	SetAnimationKey( WHCA_IDLE);
}

void CGameObjectWildHogCannon::_InitFx(void)
{
	m_idxFxDummy = getSceneObject()->GetBody()->FindBoneByName( "FXDummy");

	I3ASSERT( m_idxFxDummy != -1);
}

void CGameObjectWildHogCannon::SetAnimationKey( WILDHOG_CANNON_ANIM animKey)
{
	if( animKey != WHCA_NONE && m_pAnim[animKey] != nullptr )
	{
		if( animKey == WHCA_CRASH )
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

#define RANGE_GOLEM_ATTACK_TIME		4.85f

void CGameObjectWildHogCannon::Process( REAL32 rDeltaSeconds)
{
	m_rAnimLocalTime += rDeltaSeconds;

	switch( m_AnimKey)
	{
	case WHCA_IDLE :
		if( getCurrentHP() == 0.f)
		{
			SetAnimationKey( WHCA_CRASH);
		}
		else
		{
			ISTEST_S
			{
				if( (m_rAnimLocalTime > 3.f) )
				{
					m_rAnimLocalTime = 0.f;

					if( (i3Math::Rand() % 100 < 40))
						SetAnimationKey( WHCA_ATTACK);
				}
			}
			ISTEST_E
		}
		break;
	case WHCA_ATTACK :
		if( getCurrentHP() == 0.f)
		{
			SetAnimationKey( WHCA_CRASH);
		}
		else
		{
			if( !m_bAttacking && (m_rAnimLocalTime - rDeltaSeconds < RANGE_GOLEM_ATTACK_TIME - 0.5f) && ( m_rAnimLocalTime >= RANGE_GOLEM_ATTACK_TIME - 0.5f) )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_WILDHOG_ATTACK, 0, GetPos(), false);
				I3TRACE("와일드 호크 골렘 삽질\n");
			}

			i3Body * pBody = getSceneObject()->GetBody();
			if( !m_bAttacking && (m_rAnimLocalTime + rDeltaSeconds >= RANGE_GOLEM_ATTACK_TIME) )
			{
				MATRIX mtx;
				
				i3Matrix::Copy( &mtx, pBody->getCacheMatrix( m_idxFxDummy));
				i3Matrix::PreRotateX(&mtx, I3_DEG2RAD( 35.0f));

				// Attack
				INT32 idx = g_pEffectManager->GetEnvironmentEffectIdx( ENV_EFFECT_RANGE_GOLEM_FIRE);

				g_pEffectManager->EmitEffect( idx, &mtx, nullptr, nullptr,nullptr);

				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_WILDHOG_ATTACK, 0, GetPos(), false);

				_ProcessCamera( GetPos(), 25.f, 100.f, 3.f);

				m_bAttacking = true;
				m_bFireBall = true;
			}
			
			if( m_rAnimLocalTime + rDeltaSeconds >= pBody->GetDuration() )
			{
				SetAnimationKey( WHCA_IDLE);
				m_bAttacking = false;
			}
		}
		break;

	case WHCA_CRASH :
		{	
		}
		break;
	}

	ProcessImpactBall( rDeltaSeconds);
}

void CGameObjectWildHogCannon::_ProcessCamera( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop)
{
	if( g_pCamera == nullptr)
		return;

	MATRIX * pmtxCam = g_pCamera->getCamMatrix();
	
	VEC3D vLength;
	i3Vector::Sub( &vLength, pPos, i3Matrix::GetPos( pmtxCam));
	REAL32 rLength = i3Vector::Length( &vLength);

	REAL32 rMaxLen = rRange * 2;
	if( rLength < rMaxLen)
	{
		REAL32 TrembAmpRate = 1.0f - (rLength/rMaxLen);
		g_pCamera->SetTremble( 5.0f * TrembAmpRate * TrembAmpRate, 7.0f * TrembAmpRate, 0.1f, 2.f * TrembAmpRate);
	}
}

void CGameObjectWildHogCannon::_ProcessCharaHit( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop)
{
	INT32 i;
	CHARA_DAMAGEINFO DamageInfo;
	REAL32 rLength;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_THROWING_GRENADE, WEAPON::getItemIndex( WEAPON::THROWING_K400), true );
	DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);

	DamageInfo._DeathType			= CHARA_DEATH_O;
	DamageInfo._DamageType			= WEAPON::DAMAGE_HP_LOSS;
	DamageInfo._rDamage				= rDamage;
	DamageInfo._rExplosionRange		= rRange;
	DamageInfo._rExplosionSlope		= rSlop;
	DamageInfo._HitType				= GHT_CHARA;
	DamageInfo.SetHitPart( CHARA_HIT_ROOT );

	VEC3D vLength, vDir;

	for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;

		INT32 idx = pChara->getCharaNetIndex();
		if( idx == -1)	continue;

		if( !BattleServerContext::i()->IsP2PHost() && (idx != BattleSlotContext::i()->getMySlotIdx()) )
			continue;
		
		if( pChara->getCharaTeam() & CHARACTER_TEAM_RED)
			continue;

		i3Vector::Sub( &vLength, pChara->GetPos(), pPos);
		rLength = i3Vector::Normalize( &vDir, &vLength);
	
		if( rLength < rRange)
		{
			DamageInfo._nKillerIdx	= pChara->getCharaNetIndex();
			DamageInfo._pVecDir		= &vDir;
			DamageInfo._pVecPos		= pPos;
			DamageInfo._rDistance	= rLength;
			DamageInfo._nVictimIdx	= pChara->getCharaNetIndex();

			i3Vector::Copy( &DamageInfo._vStartPos, GetPos() );
			i3Vector::Copy( &DamageInfo._vHitPos, pChara->GetPos());

			if(BattleServerContext::i()->IsUseDedicationMode() == false)
			{
				pChara->OnHit( &DamageInfo );
			}
			else
			{
				GAMEEVENT::Write_Chara_SuicideDamage( pChara, CHARA_DEATH_FAST_OBJECT, CHARA_HIT_BODY1,
					(UINT16) DamageInfo._rDamage, DamageInfo._pVecDir, WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::KNIFE_M7), 0xFFFF, pChara->getCharaNetIndex());
			}
		}
	}

	_ProcessCamera( pPos, rRange, rDamage, rSlop);
}

void CGameObjectWildHogCannon::SetControledObjectState( INT32 state)
{
	if( state == (UINT32)WHCA_ATTACK)
	{
		SetAnimationKey( WHCA_ATTACK);
	}
}

void CGameObjectWildHogCannon::ProcessImpactBall( REAL32 rDeltatime)
{	
	if( m_bFireBall )
	{
		m_rFireBallImpactTime += rDeltatime;

		if( m_rFireBallImpactTime > 1.5f)
		{
			REAL32 rRate = getCurrentHP() / getMaxHP();
			if( rRate > 0.75f)
				m_nImpactCount = 0;
			else if( rRate > 0.55f)
				m_nImpactCount = 1;
			else
				m_nImpactCount = 2;

			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_WILDHOG_ATTACK, 1, &m_vImpactPos[ m_nImpactCount], false);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_RANGE_GOLEM_IMPACT, &m_vImpactPos[ m_nImpactCount], &I3_YAXIS);
			_ProcessCharaHit( &m_vImpactPos[ m_nImpactCount], 8.f, 100.f, 0.8f);

			m_bFireBall = false;
			m_rFireBallImpactTime = 0.f;
		}
	}
}



