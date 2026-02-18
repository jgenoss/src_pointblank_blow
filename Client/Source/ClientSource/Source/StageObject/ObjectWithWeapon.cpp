#include "pch.h"
#include "ObjectWithWeapon.h"
#include "GlobalVariables.h"
#include "GlobalFunc.h"
#include "WeaponOnObject.h"
#include "StageBattle.h"
#include "Camera/GameCamera_OnObject.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "UtilBattle.h"
#include "../Weapon/WeaponItemID.h"

#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

CGameObjectWithWeapon::CGameObjectWithWeapon()
{
	m_pWeaponOnObject = nullptr;
	m_rEnableTime = 0.f;
	m_bHalfDestroyed = false;

	InitEnterCharaIdx();
}

CGameObjectWithWeapon::~CGameObjectWithWeapon()
{
}

bool CGameObjectWithWeapon::EnableOnOffObject(UINT64 keyStroke)
{
	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( pMyChara != nullptr)
	{
		if((keyStroke & GAME_KEY_ACTION_SPECIAL && pMyChara->getActionContext()->isEnableOnObject()))
		{
			if(m_rEnableTime > g_pFramework->GetAccTime()) {
				return false;
			}
			// 1초마다 키가 작동하게 한다.
			m_rEnableTime = g_pFramework->GetAccTime() + 1.f;
			return true;
		}
	}
	return false;
}


void CGameObjectWithWeapon::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);

	bool bIsOnObjectCamera = i3::same_of<CGameCamera_OnObject*>(g_pCamera);
	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe() == false)
	{
#endif
	if( pMyChara != nullptr &&
		pMyChara->isCharaStateMask( CHARA_STATE_DEATH) == false )
	{
		// revision 64711 리스폰 후 오브젝트 무기를 사용해도 무적상태 시간은 진행되어야 함 - 2013. 9. 26 상훈
		if( pMyChara->isLocal() && !pMyChara->isEnable() )
			pMyChara->ProcessTimer(rDeltaSeconds);

		return;
	}
#if defined( DEF_OBSERVER_MODE)
	}
#endif

	if( m_iEnterCharaIdx == -1)
	{
		// revision 64853 옵저버라면 그냥 넘긴다. 탑승 오브젝트가 2개 이상 존재할 가능성이 있기 때문에.
		if(!UserInfoContext::i()->IsGM_Observer())
		{
			if( bIsOnObjectCamera)
			{	
				ub::SetToOldCameraIfOnObjectCamera();
				getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
				g_pFramework->SetNightVisionEnable(false);
				InitEnterCharaIdx();
			}
		}
		return;
	}
	
	CGameCharaBase* pEnterChara = g_pCharaManager->getCharaByNetIdx(m_iEnterCharaIdx);
	CGameCharaBase* pCameraTargetChara = CGameCameraManager::i()->getTargetCharacter();

	if(pEnterChara == nullptr)
	{
		if( bIsOnObjectCamera)
		{	
			ub::SetToObserveCamera();
			getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
			g_pFramework->SetNightVisionEnable(false);
			InitEnterCharaIdx();
		}
		return;
	}

	if(pEnterChara != pCameraTargetChara && bIsOnObjectCamera)
	{
		//옵저버라면 그냥 넘긴다. 탑승 오브젝트가 2개 이상 존재할 가능성이 있기 때문에.
		if(!UserInfoContext::i()->IsGM_Observer())
		{
			ub::SetToObserveCamera();
			getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
			g_pFramework->SetNightVisionEnable(false);
			pEnterChara->removeCharaStateMask(CHARA_STATE_OBSERVER);

			return;
		}
		else
		{
			pEnterChara = pCameraTargetChara;
		}
	}
	
	

	if( pEnterChara->isCharaStateMask( CHARA_STATE_DEATH))
	{
		if( bIsOnObjectCamera)
		{
			// pStage->ToggleOnObjectCameraMode( m_pWeaponOnObject->getSceneObject()->GetBody(), m_pWeaponOnObject->GetCameraLookAtIdx(), this, m_pWeaponOnObject);
			ub::SetToObserveCamera();

			getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
			InitEnterCharaIdx();
			g_pFramework->SetNightVisionEnable(false);
			pEnterChara->removeCharaStateMask(CHARA_STATE_OBSERVER);
		}
		return;
	}

	if( !pEnterChara->isLocal())
	{
		if( pEnterChara->isCharaStateMask(CHARA_STATE_OBSERVER) && !bIsOnObjectCamera)
		{
			ub::SetToOnObjectCamera( m_pWeaponOnObject->getSceneObject()->GetBody(), m_pWeaponOnObject->GetCameraLookAtIdx(), this, m_pWeaponOnObject);
			getSceneObject()->AddFlag(I3_NODEFLAG_INVISIBLE);
		}
		else if( !pEnterChara->isCharaStateMask(CHARA_STATE_OBSERVER) && bIsOnObjectCamera )
		{	
			ub::SetToOldCameraIfOnObjectCamera();
			getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
			g_pFramework->SetNightVisionEnable(false);
			// InitEnterCharaIdx();
		}
	}
}

void CGameObjectWithWeapon::ResetObject()
{
	I3ASSERT(m_pWeaponOnObject);
	
	getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
	m_pWeaponOnObject->DeactiveWeaponOnObject();

	if(m_iEnterCharaIdx == -1)
	{
		return;
	}

	CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(m_iEnterCharaIdx);
	
	if(pChara != nullptr) 
	{
		pChara->OffObject();
	}
	InitEnterCharaIdx();
	g_pFramework->SetNightVisionEnable(false);
	m_bHalfDestroyed = false;	// revision 62269
}

void CGameObjectWithWeapon::ProcessIntrude( const GAMEINFO_OBJECT_NETDATA * pNetData)
{
	I3ASSERT( pNetData != nullptr);

	// revision 62269 오브젝트 폭파 컬리전 체크 수정 및 파괴미션 헬기 버그 수정
	if(!GetHalfDestroyed() && pNetData->getState() == 1)
	{
		LeaveObject();
		SetHalfDestroyed(true);
	}

	//I3TRACE( "StageInfo Recv:: %d, %.4f, %d, %d\n", pRecvObjectInfo->_nState, pRecvObjectInfo->_fKey, pRecvObjectInfo->_nPathKey, pRecvObjectInfo->_EnterCharaIdx);
	if( pNetData->getObjectType() & UDP_OBJECT_TYPE_OWN)
	{
		if( pNetData->getEnterCharaIndex() < SLOT_MAX_COUNT)
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pNetData->getEnterCharaIndex());
			if( pChara != nullptr)
				EnterObject( pChara);
		}
	}
}

void CGameObjectWithWeapon::EnterObject(CGameCharaBase* pChara)
{
	I3ASSERT(pChara);
	I3ASSERT(m_pWeaponOnObject);

	if(m_iEnterCharaIdx != -1)
	{
		return;
	}

	if(pChara == nullptr)
	{
		return;
	}

	if(pChara->isCharaStateMask( CHARA_STATE_DEATH))
	{
		return;
	}

	if(pChara->IsOnObject())
	{
		return;
	}
	
	
	

	if( pChara->isLocal() || pChara->isCharaStateMask( CHARA_STATE_OBSERVER) )
	{
		ub::SetToOnObjectCamera( m_pWeaponOnObject->getSceneObject()->GetBody(), m_pWeaponOnObject->GetCameraLookAtIdx(), this, m_pWeaponOnObject);
		getSceneObject()->AddFlag(I3_NODEFLAG_INVISIBLE);
		pChara->Cmd_Reset();
		g_pCamera->SetCameraFOV(g_pCamera->GetCameraDefaultFOV());
	}

	m_pWeaponOnObject->setOwner(pChara);
	m_pWeaponOnObject->ActiveWeaponOnObject(this);
	pChara->OnObject(this);
	g_pCharaManager->InvisibleCharaAndWarp(pChara->getCharaNetIndex());
	m_iEnterCharaIdx = pChara->getCharaNetIndex();

	// GlobalFunc::PB_TRACE("EnterObject GetEnterCharaIdx %d \n", GetEnterCharaIdx());
}

void CGameObjectWithWeapon::LeaveObject(INT32 iIdx)
{
	if( iIdx == m_iEnterCharaIdx )
	{
		LeaveObject();
	}
}

void CGameObjectWithWeapon::LeaveObject()
{
	I3ASSERT(m_pWeaponOnObject);
	
	if(m_iEnterCharaIdx == -1)
	{
		return;
	}

	CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(m_iEnterCharaIdx);
	if(pChara == nullptr) return;

	if(pChara->IsOnObject() == false)
	{
		return;
	}

	pChara->OffObject();
	g_pCharaManager->VisibleCharaAndWarpSavedPos(m_iEnterCharaIdx);

	if( pChara->isLocal() || pChara->isCharaStateMask( CHARA_STATE_OBSERVER) )
	{
		ub::SetToOldCameraIfOnObjectCamera();
		getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
		g_pFramework->SetNightVisionEnable(false);
	}
	m_pWeaponOnObject->DeactiveWeaponOnObject();
	m_pWeaponOnObject->setOwner(nullptr);
	
	// revision 64853 옵저버라면 이곳에서 다시 카메라를 원래대로 돌린다.
	if(UserInfoContext::i()->IsGM_Observer() && i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		ub::SetToOldCameraIfOnObjectCamera();
		getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
		g_pFramework->SetNightVisionEnable(false);
	}
	
	m_iEnterCharaIdx = -1;
	// pChara->addStateMask( CHARA_STATE_ONOBJECT);
}

VEC3D* CGameObjectWithWeapon::GetMuzzlePos()
{ 
	I3ASSERT(m_pWeaponOnObject); 
	return m_pWeaponOnObject->GetMuzzlePos(); 
}

void CGameObjectWithWeapon::ExplosionObject( i3GameObj * pSender)
{
	if( BattleServerContext::i()->IsP2PHost() == false) 
		return;

	CGameCharaBase * pShooter = nullptr;
	WeaponBase * pWeapon = nullptr;

	if( pSender != nullptr )
	{
		while( i3::kind_of<i3Object*>(pSender))
		{
			pSender = pSender->getEventSender();
			if( pSender == nullptr )
				break;
		}

		if( i3::kind_of<WeaponBase*>(pSender))
		{	
			pWeapon = (WeaponBase*) pSender;
			pShooter = pWeapon->getOwner();
		}
	}

	if( m_iEnterCharaIdx != -1)
	{
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(m_iEnterCharaIdx);
		if( pChara == nullptr) return;

		VEC3D vDir;
		i3Vector::Set( &vDir, 0.f, 1.f, 0.f);

		g_pCharaManager->DeathChara( pChara, m_iEnterCharaIdx, &vDir, CHARA_DEATH_F );

		GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;

		deathInfo.Reset();
		if( pShooter != nullptr)
		{
			deathInfo._iKillidx			= ((CGameCharaBase*)pShooter)->getCharaNetIndex();

			if( pWeapon != nullptr)
			{
				deathInfo._ui32WeaponID		= pWeapon->getWeaponInfo()->GetItemID();
			}
			else
			{
				deathInfo._ui32WeaponID		= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, WEAPON::getItemIndex( WEAPON::KNIFE_M7) );
			}
		}
		else
		{
			deathInfo._iKillidx			= m_iEnterCharaIdx;
			deathInfo._ui32WeaponID		= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, WEAPON::getItemIndex( WEAPON::KNIFE_M7) );
		}

		deathInfo._iDeathidx		= m_iEnterCharaIdx;
		deathInfo._nDeathType		= CHARA_DEATH_O;
		deathInfo._nHitPart			= CHARA_HIT_BODY1;
		deathInfo._pHitDir			= &vDir;

		GAMEEVENT::Write_Chara_Death( &deathInfo);
	}
}
void CGameObjectWithWeapon::InitEnterCharaIdx()
 { 
	 m_iEnterCharaIdx = -1; 
 }

 bool CGameObjectWithWeapon::IsEnableEnter(CGameCharaBase* pChara)
 {
	if(pChara->isCharaStateMask( CHARA_STATE_ONOBJECT))
	{
		return IsEnableEnter(pChara, true);
	}
	else
	{
		return IsEnableEnter(pChara, false);
	}
 }

 bool CGameObjectWithWeapon::IsEnableTime()
 {
	 if(m_rEnableTime < g_pFramework->GetAccTime()) 
	 {
		 return true;
	 }
	 else
	 {
		return false;
	 }
 }

 bool CGameObjectWithWeapon::IsEnableEnter(CGameCharaBase* pChara, bool bTakeObject)
 { 
	 return (GetEnterCharaIdx() == -1 &&  bTakeObject); 
 }


