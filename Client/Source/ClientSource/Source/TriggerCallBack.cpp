#include "pch.h"
#include "TriggerCallBack.h"
#include "GameCharaMoveContext.h"
#include "GameCharaBase.h"
#include "GlobalFunc.h"
#include "GlobalVariables.h"
#include "GameContext.h"
#include "MyRoomInfoContext.h"
//#include "i3GameObj.h"

void CTriggerCallBack::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
//	NxActor* pActor = &(triggerShape.getActor());
	NxActor* pActor2 = &(otherShape.getActor());

	// GlobalFunc::PB_TRACE("onTrigger triggerShape %s %d otherShape %s %d %d \n", pActor->getName(), pActor->getGroup(), pActor2->getName(), pActor2->getGroup(), pActor2->getDominanceGroup() );

	i3ElementBase * pEBase = (i3ElementBase *)triggerShape.getActor().userData;
	
	if(pEBase == nullptr)
	{
		return;
	}
	
	if( i3::kind_of<i3Trigger*>(pEBase))
	{
		i3GameObj* pGameObj = (i3GameObj*)pEBase; 

		pGameObj->OnTrigger(triggerShape, otherShape, status);
	}
	else if( i3::same_of<PlayerController*>(pEBase))
	{
		if(pActor2->getGroup() != CGRP_OBJECT && pActor2->getGroup() != CGRP_OBJECT_WHOLE && pActor2->getGroup() != CGRP_WORLD)
		{
			return;
		}
		/*if(status != NX_TRIGGER_ON_STAY)
		{
			return;
		}*/
		if(m_rOldTime == g_pFramework->GetAccTime())
		{
			return;
		}
		
		// GlobalFunc::PB_TRACE("onTrigger %d \n", pActor2->getGroup());

		PlayerController* pCtrl = (PlayerController*)pEBase; 
		if( (pCtrl->getOwner() != nullptr) && ( pCtrl->getOwner()->getMoveContext() != nullptr) &&
			pCtrl->getOwner()->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
		{
			NxCapsuleShape* pCapsule = triggerShape.isCapsule();

			if( pCapsule != nullptr)
			{
				NxCapsule nxCapsule;
				pCapsule->getWorldCapsule(nxCapsule);

				REAL32 rOffset = 0.f;
				NxVec3 vMovementDir(0.f,0.f,0.f);
				// 오브젝트 움직이는 속도가 느리면, rOffset값을 줘야 자유낙하를 할 수 있음				
				if((MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_PORT_AKABA) ||
				   (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_CARGOPORT))
				{
					//rOffset = 0.01f;
					
					if( pActor2->isDynamic() )
					{
						vMovementDir = pActor2->getLinearVelocity();
					}
				}
				
				if( !pCtrl->getOwner()->isInvincible() )
				{
					REAL32 rSkinWidth = triggerShape.getSkinWidth() + otherShape.getSkinWidth() + rOffset;
					pCtrl->getOwner()->getMoveContext()->CalcOverlapLength(&otherShape, &nxCapsule, rSkinWidth, vMovementDir);
					m_rOldTime = g_pFramework->GetAccTime();

					if(i3::generic_is_iequal( pActor2->getName(), "__DeathObject") )
					{
						pCtrl->getOwner()->Cmd_DamageSelf(255);
					}
					else if( i3::generic_is_iequal( pActor2->getName(), "__FastObject") )
					{
						i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pActor2->userData;
						pCtrl->getOwner()->getMoveContext()->FromObjDamageToChara( &otherShape,  pShapeSet);
					}
				}
			}
		}
	}
}
