#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectSiegeGolem.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "../Weapon/WeaponItemID.h"

I3_CLASS_INSTANCE( CGameObjectSiegeGolem);//, CGameControledObject);

const char * s_AnimName_SiegeGolem[] =
{
	"Object/cgame/Siege_Golem/Siege_Golem_Wait.i3a",
	"Object/cgame/Siege_Golem/Siege_Golem_Atk01.i3a",
	"Object/cgame/Siege_Golem/Siege_Golem_Death.i3a",
	"Object/cgame/Siege_Golem/Siege_Golem_SpAtk01.i3a",
};

CGameObjectSiegeGolem::CGameObjectSiegeGolem(void)
{
	m_AnimKey			= SGA_NONE;
	m_rAnimLocalTime	= 0.0f;

	m_bAttacking = false;
	
	i3mem::FillZero( m_pFxDummy, sizeof( i3Transform*) * 2);
	i3mem::FillZero( m_pAnim,	sizeof( i3Animation*) * SGA_COUNT);
}

CGameObjectSiegeGolem::~CGameObjectSiegeGolem(void)
{
	for( INT32 i = 0; i < SGA_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pAnim[i]);
	}
}

void CGameObjectSiegeGolem::InitObject()
{
	_InitAnimation();
	_InitFx();
}

void CGameObjectSiegeGolem::ResetObject()
{
	m_rAnimLocalTime	= 0.f;
	m_bAttacking		= false;

	SetAnimationKey( SGA_IDLE);
}

void CGameObjectSiegeGolem::EnableObject()
{
	SetAnimationKey( SGA_IDLE);
}

void CGameObjectSiegeGolem::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
	Process( rDeltaSeconds);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CGameObjectSiegeGolem::_InitAnimation( void)
{
	for( INT32 i = 0; i < SGA_COUNT; i++)
	{
		if( s_AnimName_SiegeGolem[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_SiegeGolem[i]);
			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_SiegeGolem[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	SetAnimationKey( SGA_IDLE);
}

void CGameObjectSiegeGolem::_InitFx(void)
{
	const char * szFxDummyName[] = 
	{
		"FXDummy_LeftHand",
		"FXDummy_RightHand",
	};

	for( INT32 idx = 0; idx < 2; ++idx)
	{
		m_pFxDummy[idx] = (i3Transform*)i3Scene::FindNodeByName( getSceneObject(), szFxDummyName[idx]);
		I3ASSERT(  m_pFxDummy[idx] != nullptr);

		if( i3::same_of<i3BoneRef*>(m_pFxDummy[idx]))
		{
			for( INT32 i = 0; i < m_pFxDummy[idx]->GetChildCount(); i++)
			{
				i3Node * pChild = m_pFxDummy[idx]->GetChild( i);
				if( i3::same_of<i3Transform*>(pChild))
				{
					m_pFxDummy[idx] = (i3Transform*)pChild;
					break;
				}
			}
		}
	}
}

void CGameObjectSiegeGolem::SetAnimationKey( SIEGEGOLEM_ANIM animKey)
{
	if( animKey != SGA_NONE && m_pAnim[animKey] != nullptr )
	{
		if( animKey == SGA_CRASH )
		{
			m_pSceneObj->PlayAnim( m_pAnim[animKey], I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, 1.0f, I3_ANIM_PLAY_MODE_CLAMP, I3_ANIM_PLAY_MODE_STOP);
		}
		else
		{
			m_pSceneObj->PlayAnim( m_pAnim[animKey]);
		}

		m_rAnimLocalTime	= 0.f;

		m_AnimKey = animKey;
	}
}

#define SIEGE_GOLEM_ATTACK_TIME		4.667f//4.8f
#define SIEGE_GOLEM_SECATTACK_TIME	4.4f

void CGameObjectSiegeGolem::Process( REAL32 rDeltaSeconds)
{
	m_rAnimLocalTime += rDeltaSeconds;

	switch( m_AnimKey)
	{
	case SGA_IDLE :
		if( getCurrentHP() == 0.f)
		{
			SetAnimationKey( SGA_CRASH);
		}
		else
		{
			ISTEST_S
			{
				if( (m_rAnimLocalTime > 3.f) )
				{
					m_rAnimLocalTime = 0.f;

					if( (i3Math::Rand() % 100 < 20))
						SetAnimationKey( SGA_ATTACK);
					else if(i3Math::Rand() % 100 < 40)
						SetAnimationKey( SGA_ATTACK2);
				}
			}
			ISTEST_E
		}
		break;
	case SGA_ATTACK :
		if( getCurrentHP() == 0.f)
		{
			SetAnimationKey( SGA_CRASH);
		}
		else
		{
			if( !m_bAttacking && (m_rAnimLocalTime - rDeltaSeconds < SIEGE_GOLEM_ATTACK_TIME - 3.5f) && ( m_rAnimLocalTime >= SIEGE_GOLEM_ATTACK_TIME - 3.5f) )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SIEGE_ATTACK, 0, GetPos(), false);
			}

			if( !m_bAttacking && (m_rAnimLocalTime + rDeltaSeconds >= SIEGE_GOLEM_ATTACK_TIME) )
			{	// Attack
				VEC3D vPos;
				i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pFxDummy[0]->GetCacheMatrix()));
				//i3Vector::SetY( &vPos, getY( &vPos) - 2.2f);
				_ProcessCharaHit( &vPos, 8.f, 100.f, 0.8f);
				g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_SIEGE_GOLEM_FIRE, &vPos, &I3_YAXIS);

				i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pFxDummy[1]->GetCacheMatrix()));
				//i3Vector::SetY( &vPos, getY( &vPos) - 2.2f);
				_ProcessCharaHit( &vPos, 8.f, 100.f, 0.8f);
				g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_SIEGE_GOLEM_FIRE, &vPos, &I3_YAXIS);
				
				
				m_bAttacking = true;
			}
			
			i3Body * pBody = getSceneObject()->GetBody();
			if( m_rAnimLocalTime >= pBody->GetDuration() )
			{
				SetAnimationKey( SGA_IDLE);
				m_bAttacking = false;
			}
		}
		break;

	case SGA_CRASH :
		{	
		}
		break;

	case SGA_ATTACK2:
		if( getCurrentHP() == 0.f)
		{
			SetAnimationKey( SGA_CRASH);
		}
		else
		{
			if( !m_bAttacking && (m_rAnimLocalTime - rDeltaSeconds < SIEGE_GOLEM_SECATTACK_TIME - 3.5f) &&
				( m_rAnimLocalTime >= SIEGE_GOLEM_SECATTACK_TIME - 3.5f) )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SIEGE_ATTACK, 0, GetPos(), false);
			}

			if( !m_bAttacking && (m_rAnimLocalTime + rDeltaSeconds >= SIEGE_GOLEM_SECATTACK_TIME) )
			{	// Attack
				VEC3D vPos;
				i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pFxDummy[1]->GetCacheMatrix()));
				i3Vector::SetY( &vPos, getY( &vPos) - 2.2f);
				_ProcessCharaHit( &vPos, 10.f, 110.f, 0.8f);
				g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_SIEGE_GOLEM_FIRE, &vPos, &I3_YAXIS);

				m_bAttacking = true;
			}

			i3Body * pBody = getSceneObject()->GetBody();
			if( m_rAnimLocalTime >= pBody->GetDuration() )
			{
				SetAnimationKey( SGA_IDLE);
				m_bAttacking = false;
			}
		}
		break;
	}
}

void CGameObjectSiegeGolem::_ProcessCharaHit( VEC3D * pPos, REAL32 rRange, REAL32 rDamage, REAL32 rSlop)
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
	DamageInfo._pWallHitResult		= nullptr;
	DamageInfo._IsOwner				= false;
	DamageInfo._WallShotReduceRate	= 0;
	DamageInfo._HitType				= GHT_CHARA;
	DamageInfo.SetHitPart( CHARA_HIT_ROOT );

	for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr )	continue;

		INT32 idx = pChara->getCharaNetIndex();
		if( idx == -1)	continue;

		if( !BattleServerContext::i()->IsP2PHost() &&	(idx != BattleSlotContext::i()->getMySlotIdx()) )
			continue;

		if( pChara->getCharaTeam() & CHARACTER_TEAM_BLUE)
			continue;
		
		VEC3D vLength, vDir;
		i3Vector::Sub( &vLength, pChara->GetPos(), pPos );
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

	if( g_pCamera == nullptr)
		return;

	MATRIX * pmtxCam = g_pCamera->getCamMatrix();
	
	{
		VEC3D vLength;
		i3Vector::Sub( &vLength, pPos, i3Matrix::GetPos( pmtxCam));
		rLength = i3Vector::Length( &vLength);

		REAL32 rMaxLen = rRange * 3;
		if( rLength < rMaxLen)
		{
			REAL32 TrembAmpRate = 1.0f - (rLength/rMaxLen);
			g_pCamera->SetTremble( 5.0f * TrembAmpRate * TrembAmpRate, 7.0f * TrembAmpRate, 0.1f, 2.f * TrembAmpRate);
		}
	}
}

void CGameObjectSiegeGolem::SetControledObjectState( INT32 state)
{
	if( state == (UINT32)SGA_ATTACK)
	{
		SetAnimationKey( SGA_ATTACK);
	}
	else if( state == (UINT32)SGA_ATTACK2)
	{
		SetAnimationKey( SGA_ATTACK2);
	}
}
