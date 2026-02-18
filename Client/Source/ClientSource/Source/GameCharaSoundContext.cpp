#include "pch.h"
#include "GameCharaSoundContext.h"
#include "GlobalVariables.h"

#include "BattleHUD_Scope.h"

#include "Character/GameCharaSoundControl.h"
#include "Character/GameCharaSoundControlDino.h"
#include "BattleSlotContext.h"

static bool _SetSoundNodeProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( i3::same_of<i3AIContext*>(pNode) == false)
		return true;

	bool * isOwner = (bool*)pUserData;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == nullptr)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if((pInfo->GetNode() != nullptr) && i3::kind_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr) 
					continue;

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
					continue;

				UINT32 n3DMode = I3SND_RT_3DMODE_WORLD_SPACE;
				//Listener Sound의 속성을 가지고 있으면서 Owner속성인경우만 positional sound를 disable시킨다.
				if( *isOwner == true)
				{
					n3DMode = I3SND_RT_3DMODE_DISABLE;
				}

				pPlayInfo->set3DMode( n3DMode);
			}
		}
	}

	return true;
}

I3_CLASS_INSTANCE( CGameCharaSoundContext);

CGameCharaSoundContext::CGameCharaSoundContext()
{
	for(INT32 i = 0; i < CHARA_SOUND_COUNT; i++)
	{
		m_pCharaSoundRes[ i] = nullptr;
		m_pCharaSoundState[ i] = nullptr;
	}
}

CGameCharaSoundContext::~CGameCharaSoundContext()
{
	OnDestroy();
}

void CGameCharaSoundContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{	
	m_pSoundControl = CGameCharaSoundControl::new_object();
	m_pSoundControl->BindOwner( pOwner);

	m_pDinoSoundControl = CGameCharaSoundControlDino::new_object();
	m_pDinoSoundControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaSoundContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	m_pCurrentSoundControl = nullptr;
	I3_SAFE_RELEASE( m_pSoundControl );
	I3_SAFE_RELEASE( m_pDinoSoundControl);

	for( INT32 i = 0; i < CHARA_SOUND_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pCharaSoundRes[ i]);
		I3_SAFE_RELEASE( m_pCharaSoundState[ i]);
	}
}

void CGameCharaSoundContext::OnChange1PV()
{
	if( m_pOwner->IsDino())
	{
		m_pCurrentSoundControl = m_pDinoSoundControl;
		return;
	}

	m_pCurrentSoundControl = m_pSoundControl;
}

void CGameCharaSoundContext::OnChange3PV()
{
	if( m_pOwner->IsDino())
	{
		m_pCurrentSoundControl = m_pDinoSoundControl;
		return;
	}

	m_pCurrentSoundControl = m_pSoundControl;
}

bool CGameCharaSoundContext::PlayFootStep( I3_TERRAIN_TYPE nType,  bool bLeftfoot, bool bListener)
{
	I3ASSERT( g_pViewer != nullptr);
	I3ASSERT( g_pSndMng != nullptr);

	if( (m_pOwner == nullptr) || m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return false;

	//CHARA_STATE_FLOOR 상태일때만 발소리가 나도록 함
	//다른 플레이어의 캐릭터에 CHARA_STATE_FLOOR가 적용되지 않아서 이렇게 작성됨
	//(GameUDPHandler.cpp의 _CheckGameDataToSend()함수 참고)
	const GAMEINFO_CHARACTER*	pCharaInfo = BattleSlotContext::i()->getGameInfo_Chara(m_pOwner->getCharaNetIndex());
	GAMEINFO_CHARA_STATE		charaState = pCharaInfo->_tNetData._tCharaState;

	if (charaState.getJumpState() != CHARA_SPACE_NONE)
		return false;

	INT32 nRand = i3Math::Rand()%2;

	INT32 nFootStepIdx = GTBT_MOVE_LEFT_1;
	if( !bLeftfoot)
	{
		nFootStepIdx += 2;
	}

	nFootStepIdx += nRand;

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( m_pOwner->IsRex() || m_pOwner->IsDomiMutantRex())
	{
		if( (pMyChara != nullptr) && (pMyChara->IsRex() == false) && (pMyChara != m_pOwner))
		{
			VEC3D	vDiff;
			i3Vector::Sub( &vDiff, m_pOwner->GetPos(), pMyChara->GetPos());

			REAL32 rLength = i3Vector::Length( &vDiff);

			REAL32 rate = 1.0f - ( (MINMAX( 0.0f, rLength, 30.0f) / 30.0f));
			if( rate > I3_EPS)
			{
				g_pCamera->SetTremble( MINMAX( 0.05f, 0.3f * rate, 0.3f), MINMAX( 5.0f, 10.0f * rate, 10.0f), 0.0f, 0.3f + rate * 0.3f);
			}
		}

		nFootStepIdx = GTTBDTR_MOVE_TREX_LEFT_1 + nRand;
		if( !bLeftfoot)
		{
			nFootStepIdx += 2;
		}

		g_pSndMng->StartDinoTerrainTypeSnd( nType, nFootStepIdx, m_pOwner->GetPos(), bListener);
	}
	else if( m_pOwner->IsRaptorBase() || m_pOwner->IsDomiRaptorBoneBase())
	{

		if (m_pOwner->IsTank() || m_pOwner->IsDomiTank())
		{
			nFootStepIdx = GTTBDTR_MOVE_TANK_LEFT_1 + nRand;
			if( !bLeftfoot)
			{
				nFootStepIdx += 2;
			}

			g_pSndMng->StartDinoTerrainTypeSnd( nType, nFootStepIdx, m_pOwner->GetPos(), bListener);
		}
		else if (m_pOwner->IsAcid() || m_pOwner->IsCCAcid() || m_pOwner->IsAcidVulcan() || 
			m_pOwner->IsDomiAcid() || m_pOwner->IsDomiAcidNotBomb() || m_pOwner->IsDomiAcidVulcan() )
		{
			nFootStepIdx = GTTBDTR_MOVE_ACID_LEFT_1 + nRand;
			if( !bLeftfoot)
			{
				nFootStepIdx += 2;
			}

			g_pSndMng->StartDinoTerrainTypeSnd( nType, nFootStepIdx, m_pOwner->GetPos(), bListener);
		}
		else
		{
			nFootStepIdx = GTTBDTR_MOVE_RAPTOR_LEFT_1 + nRand; 
			if( !bLeftfoot)
			{
				nFootStepIdx += 2;
			}

			g_pSndMng->StartDinoTerrainTypeSnd( nType, nFootStepIdx, m_pOwner->GetPos(), bListener);
		}
	}
	else
	{
		g_pSndMng->StartTerrainTypeSnd( nType, nFootStepIdx, m_pOwner->GetPos(), bListener);
	}

	return true;
}


void CGameCharaSoundContext::SetSoundOwner( void)
{
	bool bOwner = m_pOwner->is1PV();
	//Sound에 Owner에따라서 3dmode를 실행 할런지 말런지 결정한다.
	{
		i3GameNodeTraversal Traverse;

		Traverse.setUserData( &bOwner);
		Traverse.setUserProc( _SetSoundNodeProc);

		Traverse.Traverse( m_pOwner);
	}
}

void CGameCharaSoundContext::PlayScopeSound( WeaponBase * pWeapon, bool bZoomIn /*= true*/)
{
	I3ASSERT( pWeapon != nullptr);
	I3ASSERT( pWeapon->getWeaponInfo() != nullptr);

	WEAPON_CLASS_TYPE weaponClass = pWeapon->getWeaponInfo()->GetTypeClass();

	bool bOnScopeSound = false;
	const SUB_SCOPE_INFO* subscopeinfo = g_pWeaponInfoDataBase->GetSubScopeInfo(pWeapon->getWeaponInfo()->GetScopeSubType());
	if (subscopeinfo != nullptr)
	{
		bOnScopeSound = subscopeinfo->m_bOnScopeSound;
	}

	if( (weaponClass == WEAPON_CLASS_SNIPER) ||
		(weaponClass == WEAPON_SUBCLASS_SNIPER) ||
		bOnScopeSound)
	{
		if( bZoomIn)
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 0, m_pOwner->GetPos(), m_pOwner->is1PV());
		else
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 2, m_pOwner->GetPos(), m_pOwner->is1PV());
	}
	else if( weaponClass == WEAPON_CLASS_ROCKET_LAUNCHER)
	{
		if( bZoomIn)
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_GRAZE_RPG7, 1, m_pOwner->GetPos(), m_pOwner->is1PV());
		else
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_GRAZE_RPG7, 2, m_pOwner->GetPos(), m_pOwner->is1PV());
	}
	else
	{
		if( bZoomIn)
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ASSAULTSCOPE, 0, m_pOwner->GetPos(), m_pOwner->is1PV());
		else
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ASSAULTSCOPE, 2, m_pOwner->GetPos(), m_pOwner->is1PV());
	}
}

void CGameCharaSoundContext::_PlayCharaHitSound( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener)
{
	m_pCurrentSoundControl->OnHit( pDamageInfo, pPos, bListener);
}

void CGameCharaSoundContext::_PlayJumpStartSound( void)
{
	m_pCurrentSoundControl->OnJumpStart();
}

void CGameCharaSoundContext::_PlayJumpEndSound( void)
{
	m_pCurrentSoundControl->OnJumpEnd();
}
