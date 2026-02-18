#include "pch.h"

#include "WeaponFunction.h"

//#define ENABLE_WEAPON_ANIM_OUTPUT

namespace WeaponFunction
{

	INT8	FindAIStateIndex(WeaponBase* pWeapon, const char* aiName)
	{
		i3AIContext* pAICtx = pWeapon->GetAIContext();
		if (pAICtx == nullptr) return -1;
		return INT8(pAICtx->FindAIStateByName(aiName)); 
	}

	bool	PlayWeaponAnim( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale)
	{
		i3AIContext* pAICtx = pWeapon->GetAIContext();

		if( pAICtx == nullptr)		return false;
		if( aiStateIdx < 0)		return false;
		I3ASSERT( pAICtx->getAIStateCount() < 128);
		if( aiStateIdx >= (INT8) pAICtx->getAIStateCount()) return false;

		pAICtx->SetEnable();
		pAICtx->setCurrentAIState( (INT32) aiStateIdx, 0.f, rTimeScale);
//
		pWeapon->setAIIndex( WEAPON::AI_CURRENT, aiStateIdx);
//

#ifdef ENABLE_WEAPON_ANIM_OUTPUT	//	임시
		i3AIState * pAIState = pAICtx->getNextAIState();
		char * pszAIName = "(null)";

		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();

		if( pAIState != nullptr)
		{
			I3TRACE("- Weapon (%s) Anim : (%s) %f\n", pAIState->GetName(),	pszAIName,rTimeScale);
		}
		else
			I3TRACE("- Weapon (nullptr)\n");
#endif		
		return true;
	}

	bool	PlayWeaponAnimDirect( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale )
	{
		i3AIContext* pAICtx = pWeapon->GetAIContext();

		if( pAICtx == nullptr)	return false;
		if( aiStateIdx < 0)			return false;
		if( aiStateIdx >= (INT8) pAICtx->getAIStateCount()) return false;

		pAICtx->setCurrentAIStateDirect( (INT32) aiStateIdx, 0.f, rTimeScale);

		pWeapon->setAIIndex( WEAPON::AI_CURRENT, aiStateIdx);

	#ifdef ENABLE_WEAPON_ANIM_OUTPUT	//	임시
		i3AIState * pAIState = pAICtx->getCurrentAIState();
		char * pszAIName = "(null)";

		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();

		if( pAIState != nullptr)
		{
			I3TRACE("- Weapon (%s) Anim : (%s) %f\n", pAIState->GetName(),	pszAIName,rTimeScale);
		}
		else
			I3TRACE("- Weapon (nullptr)\n");
	#endif

		return true;
	}
	
	void	PlayWeaponAnim(WeaponBase* pWeapon, const char * pszName)
	{
		i3AIContext* pAICtx = pWeapon->GetAIContext();

		if( pAICtx == nullptr)
			return;

		pAICtx->setCurrentAIState( pszName);		

	#ifdef ENABLE_WEAPON_ANIM_OUTPUT	//	임시
		i3AIState * pAIState = pAICtx->getNextAIState();
		char * pszAIName = "(null)";

		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();

		if( pAIState != nullptr)
		{
			I3TRACE("- Weapon (%s) Anim : (%s) %f\n", pAIState->GetName(),	pszAIName);
		}
		else
			I3TRACE("- Weapon (nullptr)\n");
	#endif
	}
	
	void	SetCollideeGroup_NoUseCollidee(WeaponBase* pWeapon)
	{
		i3Body * pBody = pWeapon->getSceneObject()->GetBody();
		I3ASSERT( pBody != nullptr);
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		I3ASSERT( pSkel != nullptr);
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i) )
				pSkel->setShapeSet( i, nullptr);
		}
	}

	void	SetCollideeGroup_NormalWeapon(WeaponBase* pWeapon)
	{
		i3Scene::InitColliderSet(pWeapon->GetNode(), CGRP_ITEM, CGRP_MASK_ITEM, 0, pWeapon);
	}

	void	SetCollideeGroup_HitWeapon(WeaponBase* pWeapon)
	{
		i3Scene::InitColliderSet( pWeapon->GetNode(), CGRP_ITEM_HIT, CGRP_MASK_ITEM, 0, pWeapon);
		i3Scene::ModifyDynamicState(pWeapon->getSceneObject(), I3_PHYSIX_STATE_DISABLE, I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_NO_RAYCAST);
	}
	
	void	SetCollideeGroup_DinoDummy(WeaponBase* pWeapon)
	{
		i3Scene::InitColliderSet( pWeapon->GetNode(), CGRP_ITEM_WHOLE, CGRP_MASK_ITEM, 0, pWeapon);
		i3Scene::ModifyDynamicState( pWeapon->getSceneObject(), 0, I3_PHYSIX_STATE_NO_COLLISION);
	}
	
	void	SetCollideeGroup_SentryGun(WeaponBase* pWeapon)
	{
		WF::SetCollideeGroup_DinoDummy(pWeapon);
	}

	void	SetCollideeGroup_Supply(WeaponBase* pWeapon)
	{
		i3Scene::InitColliderSet( pWeapon->GetNode(), CGRP_OBJECT, CGRP_MASK_TEAM_BLUE | CGRP_MASK_TEAM_RED, 0, pWeapon);
	}


}