#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponControl.h"
#include "GameCharaMultiWeaponContext.h"

#include "WeaponBase.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "UtilBattle.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "GameMissionManager.h"
#include "GameEvent.H"

#include "MainWeapon_DualCIC.h"
#include "MainWeapon_DualKnife.h"

#include "BattleObjContext.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "WeaponWatermelon.h"
#include "../Weapon/WeaponItemID.h"

#include "../UI/UIHudMultiWeapon.h"
#include "../UI/UIHudManager.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl);//, CGameCharaControlBase );

bool CGameCharaWeaponControl::_ProcessDropedWeaponSearching( void)
{
	INT32	i;
	VEC3D	vDir;
	REAL32	rLength;
	WeaponBase * pWeapon;

	if( m_pOwner->IsDino()) {
		return false;
	}

	I3ASSERT( g_pWeaponManager != nullptr );
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH)) return false;
	// 떨어진 무기가 없으면 리턴
	if( g_pWeaponManager->GetDropListCount() == 0 ) return false;

	if ( m_pOwner->getMultiWeaponContext()->IsPickupProhibit() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		return false;

	REAL32 rCheckTime = getWeaponContext()->getDropedWeaponCheckTime();

	REAL32 rCheckTimeSync = 1.0f;
	if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		rCheckTimeSync = 2.0f;

	rCheckTime += g_pViewer->getDeltaTime();
	if( rCheckTime > (WEAPON_CHECKTAKING_TIME * rCheckTimeSync))
	{
		getWeaponContext()->setDropedWeaponCheckTime( 0.f);

		// 일반적으로 주무기 슬롯이 비어 있는지 확인
		// 다중무기 시스템을 사용할 경우 소지 갯수 최대치를 넘으면 안됨(AI는 다중무기를 사용할 수 없음)
		if ( ( getWeapon(WEAPON_SLOT_PRIMARY) == nullptr && !MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->CheckEmptySlot() && !m_pOwner->isAI()))
		{
			for ( i = 0; i < g_pWeaponManager->GetDropListCount(); i++)
			{
				pWeapon = g_pWeaponManager->GetDropWeapon(i);
				if ( !pWeapon->IsDroped())
					continue;

				if ( pWeapon->getWeaponInfo()->GetTypeUsage() != WEAPON_SLOT_PRIMARY)
					continue;

				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( static_cast<INT32>( pWeapon->getNetworkIdx()));
				if ( pInfo->getState() != NET_WEAPONDROPED_SLEEP)
					continue;

				if ( pWeapon->getTotalBulletCount() > 0 ||
					pWeapon->getLoadedBulletCount() > 0 ||
					pWeapon->getLoadedBulletDualCount() > 0)
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), m_pOwner->GetPos());
					rLength = i3Vector::LengthSq( &vDir);
					if ( rLength < 1.f)
					{
						// Pickup weapon
						GAMEEVENT::Write_Chara_PickupWeapon(m_pOwner, pWeapon);
						return true;
					}
				}
			}
		}

		// 보조 무기 슬롯이 비어있다. 체크
		if( getWeapon( WEAPON_SLOT_SECONDARY) == nullptr )
		{
			for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
			{
				pWeapon = g_pWeaponManager->GetDropWeapon( i );
				if( !pWeapon->IsDroped())
					continue;

				if( pWeapon->getWeaponInfo()->GetTypeUsage() != WEAPON_SLOT_SECONDARY)
					continue;

				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(static_cast<INT32>(pWeapon->getNetworkIdx()));
				if (pInfo->getState() != NET_WEAPONDROPED_SLEEP)
					continue;

				if( pWeapon->getTotalBulletCount() > 0 ||
					pWeapon->getLoadedBulletCount() > 0 )
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), m_pOwner->GetPos() );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f )
					{	
						// Pickup weapon
						GAMEEVENT::Write_Chara_PickupWeapon( m_pOwner, pWeapon);
						return true;
					}
				}
			}
		}
	}
	else
	{
		getWeaponContext()->setDropedWeaponCheckTime( rCheckTime);
	}

	return false;
}


bool CGameCharaWeaponControl::_ProcessFireForNetwork( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder)
{
	if( getCurrentWeapon() == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	CGameMissionManager * pMissionManager = CGameMissionManager::i();

	//	주 공격
	switch( pWeaponInfo->GetTypeClass())
	{			
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_KNUCKLE:
		{	//	나이프일 경우 공격 처리
			if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
			{
				WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
				//pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);

			}
			else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
			{
			//	WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
				MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(getCurrentWeapon()->GetMainWeapon());
				mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
			}
		}
		break;
	case WEAPON_CLASS_MISSION :
		{
			ISNETWORK
			{
				if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) ||		// 폭파모드가 아닌경우
					(pMissionManager->getMissionState() != GAME_MISSION_STATE_INBATTLE) )			// 설치전이 아닌경우
					return false;
			}
		}
		break;
	}

	getCurrentWeapon()->NET_Fire( pStartPos, pTargetPos, fireOrder);
	m_pOwner->Cmd_ApplyCrosshair();

	// 편차를 계산
	getBoneContext()->_ProcessFire();

	return true;
}

void CGameCharaWeaponControl::_AfterProcessDropWeapon( WeaponBase * pWeapon, bool bDirectPickup, bool bChangeNextWeapon /*= true*/)
{
	// revision 60658 네트워크 상태일 때도 자동 무기 변환 가능하도록 수정 ( hansoft. 5600 )
	/*if( m_pOwner->isNetwork() )
		return;*/

	if( !bChangeNextWeapon )
		return;

	if( pWeapon == nullptr)
		return;

	if( pWeapon->getWeaponInfo()->isC4() )
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//	주무기로 바로 교체
		if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
		{
			if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
			{
				//	바로 이전 무기로 교체
				m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getOldSlotNum());
			}
		}
	}
	else
	{
		//	다음 무기로 바로 교체
		INT32 nIdx = 0;

		//	다음 번호의 무기로 교체
		while( !getWeaponContext()->checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), true) )
		{
			if( nIdx < MAX_WEAPON_CHECK_SLOT_COUNT )
				nIdx++;
			else
			{
				nIdx = -1;
				break;
			}
		}

		m_pOwner->Cmd_ChangeWeapon( static_cast<WEAPON_SLOT_TYPE>(nIdx));
	}
}


void CGameCharaWeaponControl::_NoUseWeapon( WeaponBase * pWeapon)
{
	VEC3D vec;

	I3ASSERT( pWeapon != nullptr);

	pWeapon->ReturnToReady();									// Ready 상태로 변경
	pWeapon->getSceneObject()->GetBody()->StopAnim();			// 애니메이션을 정지
	pWeapon->getSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );	// 업데이트 할 필요가 없습니다.
	// 무기는 공중으로 보내줍니다.
	i3Vector::Set( &vec, (REAL32)( pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pWeapon, &vec );
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool CGameCharaWeaponControl::SetBlendAlpha( WEAPON_SLOT_TYPE nSlot, REAL32 fVal)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if( pWeapon == nullptr)
		return false;

	pWeapon->OnSetBlendAlpha( fVal);
	return true;
}

void CGameCharaWeaponControl::SetBlendAlphaAll( REAL32 fVal)
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		SetBlendAlpha( static_cast<WEAPON_SLOT_TYPE>(i), fVal);
	}
}

bool CGameCharaWeaponControl::SetMaterialColor( WEAPON_SLOT_TYPE nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase * pWeapon = getWeapon( nSlot);
	
	if( pWeapon == nullptr)
		return false;

	pWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
	return true;
}

void CGameCharaWeaponControl::SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		SetMaterialColor( static_cast<WEAPON_SLOT_TYPE>(i), pfDiffuse, pfSpecular);
	}
}

void CGameCharaWeaponControl::OnUpdate( REAL32 rDeltatime)
{
	//	무기 변경때 한프레임 랜더링 버그 막기위해...이전 프레임에 바뀐(바뀌기로 예약된) 무기를 지금 변경한다.
	if( getSwapSlotNumber() != getCurrentSlotNumber() || getWeaponContext()->getForcedChange() )
	{
		if( getCurrentWeapon() != nullptr )
		{
			// 이전 무기는 사용하지 않는 무기
			_NoUseWeapon( getCurrentWeapon());

			// 변경되는 무기가 올바른 손에 붙어있어야 한다.
			getCurrentWeapon()->SetCurrentAttachHand();
		}

		// 무기 교체
		OnSelectWeapon( getSwapSlotNumber());
	}
}

bool CGameCharaWeaponControl::OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync)
{
	bool bMustCreate = false;
	bool bRecreate = false;
	I3ASSERT( nSlot < WEAPON_SLOT_COUNT );

	CWeaponInfo * pNewWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid);
	if( pNewWeaponInfo == nullptr)	// 무기 정보가 없다.
		return false;

	WeaponBase * pNewWeapon = getWeapon( nSlot);
	T_ItemID subItem = Itemid;

	if( pNewWeaponInfo->GetUseBombTrigger())
	{
		Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1, WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT)); // 412001
	}

	//	무기 매니저를 통해 Instance를 받아온다.
	//	다중무기 시스템을 사용할 경우 해당 무기를 다른 유저가 사용하고 있을 수 있기 때문에 새로 생성
	if( pNewWeapon == nullptr || ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && g_pFramework->IsBattleStage()) )
	{
		bRecreate = true;
	}
	else
	{
		CWeaponInfo * pWeaponInfo = nullptr;

		// 실제 기본 무기는 BombTrigger 지만 생성시에는 C5의 ItemID를 가지고 비교해야 합니다.
		// 서버를 통해 받는 ItemID 도 C5로 되어 있으나 슬롯인덱스를 통해 getWeapon 한 pNewWeapon의 경우
		// BombTrigger 를 얻게 되어 있어 해당 경우에 대하여 연결된 C5의 정보를 가지고 비교하도록 수정합니다.

		if ( i3::same_of<WeaponBombTrigger*>(pNewWeapon) )
		{
			bMustCreate = true;
			if ( ((WeaponBombTrigger*)pNewWeapon)->getBomb() != nullptr )
				pWeaponInfo = ((WeaponBombTrigger*)pNewWeapon)->getBomb()->getWeaponInfo();
		}
		else
		{
			pWeaponInfo = pNewWeapon->getWeaponInfo();
		}

		I3ASSERT( pWeaponInfo != nullptr);
			
		//	전혀 다른 무기일 경우 다시 받아온다.
		if( pWeaponInfo->GetItemID() != pNewWeaponInfo->GetItemID())
		{
			OnDeleteMultiWeapon( nSlot);
			bRecreate = true;
		}
		else
		{
			// 이미 있는 무기와 똑같다.
		}
	}

	if( bRecreate)
	{
		pNewWeapon = g_pWeaponManager->QueryWeapon( m_pOwner, Itemid, subItem , bMustCreate );
		if ( pNewWeapon == nullptr)
			return false;

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot == WEAPON_SLOT_PRIMARY && g_pFramework->IsBattleStage())
		{
			// 다중 무기에서는 기존 무기와 다르게 관리
			if ( m_pOwner->isLocal())
			{
				m_pOwner->getMultiWeaponContext()->AddMultiWeapon( pNewWeapon, nMultiWeaponSync);
			}
			else
				m_pOwner->getMultiWeaponContext()->ChangeMultiWeapon( pNewWeapon);
		}
		else
		{
			setWeapon( nSlot, pNewWeapon);
		}
	}

	I3ASSERT( pNewWeapon != nullptr);

	// 생성 후 공중에 띄워줍니다.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)( pNewWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pNewWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pNewWeapon, &vec );

	pNewWeapon->SetOwner( m_pOwner);
	pNewWeapon->setCreater( m_pOwner);
	pNewWeapon->setUserRespawnCnt( m_pOwner->getCharaInfo()->GetRespawnCnt());
	pNewWeapon->Reset();

	return bRecreate;
}

void CGameCharaWeaponControl::OnDeleteWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if( pWeapon != nullptr )
	{
		//  모든 무기를 때어내도록 합니다..
		INT32 i;
		for( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE) i);
		}
		
		if( pWeapon == getPurchasePrimWeapon() )
		{	// 등에 매고 있다면 때어낸다.
			OnDetachPurchasePrimWeapon();
		}
		
		pWeapon->SetOwner( nullptr);
		setWeapon( nSlot, nullptr);
	}
}

void CGameCharaWeaponControl::OnDeleteMultiWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeapon( nSlot);

	if ( pWeapon != nullptr)
	{
		//  모든 무기를 때어내도록 합니다..
		INT32 i;
		for ( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE)i);
		}

		if ( pWeapon == getPurchasePrimWeapon())
		{	// 등에 매고 있다면 때어낸다.
			OnDetachPurchasePrimWeapon();
		}

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->IsUsingMultiWeapon(pWeapon))
		{
			m_pOwner->getMultiWeaponContext()->DeleteMultiWeapon( pWeapon);
		}

		pWeapon->SetOwner(nullptr);
		setWeapon( nSlot, nullptr);
	}
}

/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = getWeapon( nSlot );

	if( pWeapon == nullptr )	return nullptr;

	pWeapon->SetEnable( true);

	pWeapon->ChangeGunDummy( WEAPON_ATTACH_GRIP);	//	기본 Root로 변경
	pWeapon->AttachNode( pSceneNode, pGameNode );

	m_pOwner->Cmd_ApplyCrosshair();

	return pWeapon;
}
/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnAttachBackWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = getWeapon( nSlot );

	if( pWeapon == nullptr )	return nullptr;

	pWeapon->SetEnable( true);
		
	pWeapon->ChangeGunDummy( WEAPON_ATTACH_GRIP);	//	기본 Root로 변경

	pWeapon->MoveNodeForBow();
	pWeapon->AttachNode( pSceneNode, pGameNode );

	m_pOwner->Cmd_ApplyCrosshair();

	return pWeapon;
}
/*virtual*/ WeaponBase * CGameCharaWeaponControl::OnDetachWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != nullptr )
	{
		pWeapon->ReturnToReady();
		pWeapon->DetachNode();
	}

	return pWeapon;
}

void CGameCharaWeaponControl::OnResetWeapon( void)
{
	setCurrentSlotNumber( WEAPON_SLOT_PRIMARY);
	setOldSlotNumber( WEAPON_SLOT_UNKNOWN);
	setSwapSlotNumber( WEAPON_SLOT_PRIMARY);

	if( m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;

	bool bCheck = false;
	INT32 i = 0;

	while( !bCheck )
	{
		if( OnSelectWeapon( static_cast<WEAPON_SLOT_TYPE>(i), false) )
		{
			break;
		}

		i++;

		if( i >= WEAPON_SLOT_COUNT )
		{
			break;
			//I3PRINTLOG(I3LOG_FATAL,  "invalid server information for weapon list." );
		}
	}

	if( getCurrentWeapon() != nullptr )
	{
		getCurrentWeapon()->ReturnToReady();
		getWeaponContext()->setStaticCurrentWeapon( getCurrentWeapon());
	}

	setSwapSlotNumber( getCurrentSlotNumber());
	setOldSlotNumber( getCurrentSlotNumber());

	for( i = 0; i < WEAPON_SLOT_COUNT; ++i)
	{
		WeaponBase * pWeapon = getWeapon( static_cast<WEAPON_SLOT_TYPE>(i));

		if( pWeapon != nullptr)
			pWeapon->Reset();
	}

	//	무기는 기본 오른손에 부착
	if(getCurrentWeapon() != nullptr)
	{
		getCurrentWeapon()->ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}

	SetBlendAlpha( getCurrentSlotNumber(), 1.0f);
}

void CGameCharaWeaponControl::OnAttackIdleWeapon( void)
{
	WeaponBase*	pWeapon	= getCurrentWeapon();

	if( !i3::same_of<WeaponCrossFire*>(pWeapon) 
		|| !pWeapon->isExtensionActivate()  )
	{
		if (getActionContext())
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);
		else
			TRACE("CGameCharaWeaponControl::OnAttackIdleWeapon getActionContext = nullptr\n");
	}

	if(  i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{
		if(m_pOwner->is1PV())
		{
			( (CGameCamera_FPS*)g_pCamera )->SetNeckAnim(false);
		}
	}
}

bool CGameCharaWeaponControl::OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot)
{
	if( nSlot == WEAPON_SLOT_UNKNOWN)
		return false;

	WeaponBase * pPrvWeapon = getWeapon( nSlot);
	if( pPrvWeapon == nullptr )
		return false;

	WeaponBase * pCurWeapon = getCurrentWeapon();
	if( pCurWeapon == nullptr)
		return false;

	//	같은 무기를 들고 있으므로 등에서 떼어낸다.
	if( pCurWeapon == getPurchasePrimWeapon() )
	{
		OnDetachPurchasePrimWeapon();
	}
	
	if( pPrvWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
	{	//	무기가 주무기면 등짝에 붙인다 (등에 맨다)
		pPrvWeapon->OnSetBlendAlpha( 1.f );
		setPurchasePrimWeapon( pPrvWeapon);
		OnAttachBackWeapon( nSlot, getDummyPrimPurchase(), m_pOwner);
		
		pPrvWeapon->OnSetVisibleExtension( false);

		return true;
	}
	else
	{
		if( getPurchasePrimWeapon() != nullptr)
		{
			if( getPurchasePrimWeapon()->getWeaponInfo() != nullptr )
			{	
				getPurchasePrimWeapon()->MoveNodeForBow();
				//	안붙어 있으면 다시 붙인다.
				if( !getPurchasePrimWeapon()->IsAttachedScene())
					getPurchasePrimWeapon()->AttachNode( getDummyPrimPurchase(), m_pOwner);
			}
			else
			{// 해당 무기는 없는 경우이다.(월드에 버린 경우)
				setPurchasePrimWeapon( nullptr);
			}
		}
	}

	return false;
}

void CGameCharaWeaponControl::OnDetachPurchasePrimWeapon( void)
{
	if( getPurchasePrimWeapon() == nullptr )
		return;

	if( getPurchasePrimWeapon()->getParentScene() == getDummyPrimPurchase())
		getPurchasePrimWeapon()->DetachNode();

	setPurchasePrimWeapon( nullptr);
}

bool CGameCharaWeaponControl::OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot)
{
	return false;
}

void CGameCharaWeaponControl::OnDetachPurchaseSideWeapon( void)
{
}

bool CGameCharaWeaponControl::OnPreSelectWeapon(WEAPON_SLOT_TYPE nSlot)
{
	if ( nSlot <= WEAPON_SLOT_UNKNOWN || nSlot >= WEAPON_SLOT_COUNT)
		return false;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if ( nSlot == WEAPON_SLOT_MISSION)
	{
		if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) && (pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE))
			return false;	// 폭탄인 경우 설치전이 아니라면 바꿀 수 없습니다.
	}

	// 교체할 무기
	WeaponBase * pSwapWeapon;

	if ( !MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot != WEAPON_SLOT_PRIMARY)
		|| !g_pFramework->IsBattleStage() )
	{
		pSwapWeapon = getWeaponContext()->getWeapon( nSlot);
	}
	else
	{
		// 다중무기 교체 시
		if ( m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr)
		{
			UINT8 nPrevIndex = m_pOwner->getMultiWeaponContext()->GetCurrSlotInfo()->nSlotNumber;
			m_pOwner->getWeaponContext()->setPrevMultiWeaponIdx( nPrevIndex);
		}

		pSwapWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon( false);

		if ( pSwapWeapon != nullptr && getCurrentSlotNumber() == WEAPON_SLOT_PRIMARY && nSlot == WEAPON_SLOT_PRIMARY)
		{
			getWeaponContext()->setForcedChange(true);
		}
	}

	// 교체할 무기 정보가 없으면 Error
	if ( pSwapWeapon == nullptr)
		return false;

	if ( pSwapWeapon->isWorldWeapon() == false)
	{
		if ( MyRoomInfoContext::i()->IsLockedWeaponSlot(nSlot))
		{
			return false;
		}
	}

	CWeaponInfo * pWeaponInfo = pSwapWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if ( pWeaponInfo->isUnableSwapIfZero())
	{
		//	소모성 무기인 Throwing 또는 Item은 지닌 갯수가 0이면 교체될 수 없다.
		if ( pSwapWeapon->getLoadedBulletCount() <= 0)
			return false;
	}

	// KEYWORD : WEAPON_C5
	// 폭탄을 썼다면 바뀌지 않도록합니다. 기획 요청 0724 - dfly79
	if ( i3::same_of<WeaponBombTrigger*>(pSwapWeapon))
	{
		if ( (pSwapWeapon->getLoadedBulletCount() == 0) && (((WeaponBombTrigger*)pSwapWeapon)->getDropedBombCount() == 0))
			return false;
	}

	WeaponBase * pCurWeapon = getCurrentWeapon();

	if ( pCurWeapon != nullptr)
	{
		if (i3::same_of<WeaponDualThrowKnife*>(pCurWeapon))
		{
			if (static_cast<WeaponDualThrowKnife*>(pCurWeapon)->getDoThrowKnife())
			{
				return false;
			}
		}
		else if (i3::same_of<WeaponDualGrenade*>(pCurWeapon))
		{
			if (static_cast<WeaponDualGrenade*>(pCurWeapon)->getThrowFirstGrenade())
			{
				return false;
			}
		}
	}

	//	Weapon Change
	if( getCurrentWeapon() != nullptr)
	{	
		//	캐릭터 애니메이션을 변경하면 다음 프레임부터 적용되기 때문에 이번 프레임에 무기를 바꾸지 않고 다름 프레임에 
		//	무기를 바꾸도록 한다.
		if ( getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
		{
			//스왑일 경우에는 신규 점사 방식이 작동 중이더라도 중단 시키고 초기화한다.
			//원래 무기 발상중에 바뀌는게 버그이나, 기획의도로 지켜줘야 하므로 해당 구현이 들어간다.
			getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
			getCurrentWeapon()->setAutoFireCount(0);
		}

		if ( m_pOwner->isNetwork() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			getCurrentWeapon()->ReturnToReady();
			getCurrentWeapon()->DetachNode();
		}

		setOldSlotNumber( getCurrentSlotNumber());
		setSwapSlotNumber( nSlot);		//	다음 프레임에 바뀔 무기 번호 
	}
	else
	{	//	현재 무기가 nullptr이면 당장 바꾼다.		
		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
		if( !OnSelectWeapon( nSlot))			return false;

		//	무기는 기본 오른손에 부착
		//revision 55728 2013. 3. 25 박기성
		//챌린지 모드에서 미니액스 던진 후 모든 무기를 버리면 미니액스가 붙어나오는 문제 수정
		//미니액스 뿐 아니라 탄도단검도 발생.
		if(!i3::kind_of<WeaponMiniAxe*>(getCurrentWeapon()))
			getCurrentWeapon()->ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}

	if( g_pEdgeTrail != nullptr)
		g_pEdgeTrail->Reset();

	return true;
}

void CGameCharaWeaponControl::NoUseWeapon(WeaponBase* pWeapon)
{
#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
	I3TRACE("날려보낼 무기 - %s\n", wstrWeaponName);
#endif

	pWeapon->DetachNode();

	_NoUseWeapon(pWeapon);
}

bool CGameCharaWeaponControl::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( nSlot == WEAPON_SLOT_UNKNOWN)
		return false;

	//	교체할 무기
	WeaponBase * pNextWeapon = nullptr;
	
	if (!MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
		|| ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nSlot != WEAPON_SLOT_PRIMARY)
		|| !g_pFramework->IsBattleStage())
	{
		pNextWeapon = getWeapon(nSlot);

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			getWeaponContext()->setForcedChange(false);
			m_pOwner->getMultiWeaponContext()->ResetChange();
		}
	}
	else
	{
		// 다중무기에서 교체 무기 정보
		pNextWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon( true);

		getWeaponContext()->setForcedChange(false);
	}

	if( pNextWeapon != nullptr )
	{	
		// 모든 무기를 때어내도록 합니다..
		INT32 i;
		for( i = 0; i < WEAPON_SLOT_COUNT; i++)
		{
			OnDetachWeapon( (WEAPON_SLOT_TYPE)i);
		}

		if ( m_pOwner->getWeaponContext()->getPrevMultiWeaponIdx() != 0 &&
			MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			UINT8 nPrevWeaponIndex = m_pOwner->getWeaponContext()->getPrevMultiWeaponIdx();
			WeaponBase* pPrevWeapon = m_pOwner->getMultiWeaponContext()->GetMultiWeapon( nPrevWeaponIndex);

			if ( pPrevWeapon != nullptr)
			{
				pPrevWeapon->DetachNode();
				_NoUseWeapon(pPrevWeapon);
				m_pOwner->getWeaponContext()->setPrevMultiWeaponIdx(0);
			}
		}

		if( pNextWeapon->getOwner() == nullptr)
			pNextWeapon->SetOwner( m_pOwner);

		//	바뀐 무기로 붙인다.
		OnAttachWeapon( nSlot, getWeaponAttachRoot(), m_pOwner );

		pNextWeapon->ReturnToReady();

		if(pNextWeapon->IsExtensionWeapon())
		{
			pNextWeapon->OnSetVisibleExtension( pNextWeapon->isExtensionActivate());
		}

		#if defined( AI_MOVE_TO_WEAPON)
			setOldSlotNumber( getCurrentSlotNumber());
		#endif
		
		setCurrentSlotNumber( nSlot );
		setSwapSlotNumber( nSlot);

		// 바뀐 무기
		if( g_pFramework->IsBattleStage())
		{
			pNextWeapon->WeaponChange( bPlaySound);		//	무기 교체시 이후 무기는 Change 상태로
			getHUDContext()->ChangeWeapon();

			m_pOwner->Cmd_ApplyCrosshair();
		}
		else
		{
			pNextWeapon->WeaponChange( false);		//	무기 교체시 이후 무기는 Change 상태로
		}
		
		{
			VEC3D vec;
			pNextWeapon->SetEnable( true);
			pNextWeapon->SetPos( &vec );		// LocalPose를 초기화 해줘야합니다.
		}

		// 중요
		// 무기가 바뀌면 해당 무기에 맞는 애니메이션 셋을 세팅해야 합니다.
		getAnimContext()->ChangeAnimationContext();

		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		{
			getWeaponContext()->setStaticCurrentWeapon( pNextWeapon);
		}

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnWeaponDropToWorld( WEAPON_SLOT_TYPE nSlot, VEC3D * pPos, bool bChangeNextWeapon, bool bDirectPickup)
{
	//	SceneNode -> World Scene Node
	//	GameNode -> World Game Node
	WeaponBase*	pWeapon	= nullptr;

	pWeapon = getWeapon( nSlot);

	if( pWeapon == nullptr)
		return false;
	
	//pWeapon->setExtensionActivate( false );				// 무기의 Extension 상태를 초기화해준다.
	pWeapon->ResetSound();					// 사운드 초기화
	pWeapon->getSceneObject()->GetBody()->StopAnim();	// 애니메이션 업데이트 중지
	pWeapon->ReturnToReady();							// 리셋
	
	if( i3::same_of<WeaponC4*>(pWeapon))
	{

#if defined( USE_EVENT_SHUTTLEX)
		{
			// 이벤트용 C4
			// 기존 C4는 삭제하고 이벤트용 C4로 교체합니다.
			g_pWeaponManager->ReleaseCharaWeapon( pWeapon);
			pWeapon = g_pWeaponManager->QueryWeapon( m_pOwner, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_SHUTTLEX);
			pWeapon->setOwner( m_pOwner);
			pWeapon->Reset();
			pWeapon->SetEnable( true);
		}
#endif

		if(m_pOwner->getMoveContext()->getRideObject() == nullptr)
		{
			//	월드(캐릭터 부모)에 붙는다.
			((WeaponC4*)pWeapon)->InstallC4ToWorld(m_pOwner->getSceneObject()->GetParent()->GetParent(), m_pOwner->getParent(), pPos, true, nullptr);
		}
		else 
		{
			VEC3D v3Pos;
			MATRIX invMat;

			i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( m_pOwner->getMoveContext()->getRideObject()->getSceneObject());
				
			I3ASSERT(pTrans);

			MATRIX* pMat = pTrans->GetCacheMatrix();
				
			i3Matrix::Inverse( &invMat, nullptr, pMat);

			if( BattleServerContext::i()->IsUseDedicationMode() == false)
				i3Vector::TransformCoord( &v3Pos, pPos, &invMat);
			else
				i3Vector::Copy( &v3Pos, pPos);

			((WeaponC4*)pWeapon)->InstallC4ToMoveObject( pTrans, m_pOwner->getParent(), &v3Pos, pPos, &invMat);
		}
	}
	else
	{
		//Stage root에 붙인다
		pWeapon->Transmission(m_pOwner->getSceneObject()->GetParent()->GetParent(), m_pOwner->getParent(), pPos, true);
	}

	// 이전 캐릭터로써 소유자를 참조할 수 있다.
	pWeapon->SetOwner( m_pOwner);
	
	//	이제부터 무기 매니저의 소유이며 관리된다. 해당 캐릭터는 소유권이 없다.
	g_pWeaponManager->MoveWeaponCharaToDrop( pWeapon);	

	pWeapon->getSceneObject()->GetBody()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	if( i3::same_of<WeaponC4*>(pWeapon))
	{
		I3ASSERT( ((WeaponC4*)pWeapon)->m_bEnableActive == false);

		((WeaponC4*)pWeapon)->m_bEnableActive = true;
	}
	
	pWeapon->EnableRenderAABB(true);
	
	if ( !( nSlot == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
	{
		setWeapon( nSlot, nullptr);
	}
	
	if( bChangeNextWeapon )
		_AfterProcessDropWeapon( pWeapon, bDirectPickup);

	return true;
}

bool CGameCharaWeaponControl::OnWeaponTake( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList)
{	
	WEAPON_SLOT_TYPE	nWeaponSlot = WEAPON_SLOT_PRIMARY;
	I3ASSERT( pWeapon != nullptr );

	switch( pWeapon->getWeaponInfo()->GetTypeUsage() )
	{
	case WEAPON_SLOT_UNKNOWN	:	break;
	case WEAPON_SLOT_PRIMARY	:
	case WEAPON_SLOT_SECONDARY	:
	case WEAPON_SLOT_MELEE		:
	case WEAPON_SLOT_MISSION	:	nWeaponSlot = pWeapon->getWeaponInfo()->GetTypeUsage(); break;

	case WEAPON_SLOT_THROWING1	:
	case WEAPON_SLOT_THROWING2	:
	default :
		I3PRINTLOG(I3LOG_FATAL,  "Take weapon.  invalid type." );
		break;
	}

	bool bUsedMultiWeapon = false;

	// 이 부분 검토가 필요합니다...
	if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		if( getWeapon( nWeaponSlot ) != nullptr )
		{
			if( getWeapon(nWeaponSlot)->getWeaponInfo()->GetNumber() == pWeapon->getWeaponInfo()->GetNumber())
				OnDeleteMultiWeapon( nWeaponSlot);
		}

		if( pWeapon->getWeaponInfo()->GetTypeUsage() ==  WEAPON_SLOT_MISSION)
		{
			//미션 무기 획득
			I3_BOUNDCHK( m_pOwner->getCharaNetIndex(), SLOT_MAX_COUNT);
			UserInfoContext::i()->SetMyUsurpationIdx( static_cast<UINT8>(m_pOwner->getCharaNetIndex()) );
		}
	}
	else
	{
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY)
		{
			bUsedMultiWeapon = true;
		}

		//	새로운 무기를 습득하기 위해서는 해당 무기 슬롯이 반드시 비어 있어야 합니다. 여기서 걸리면 버그임...komet
		if( getWeapon( nWeaponSlot ) != nullptr && !bUsedMultiWeapon)
		{
			if( pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_MELEE)
			{
				if( getWeapon(nWeaponSlot)->getWeaponInfo()->GetNumber() == pWeapon->getWeaponInfo()->GetNumber())
					OnDeleteWeapon( nWeaponSlot);
				else
					return false;

			}
			else
				return false;
		}
	}

	I3TRACE( "[TakeWeapon] chara : %d, id : %d, idx : %d\n", m_pOwner->getCharaNetIndex(), pWeapon->getNetworkIdx(), pWeapon->getNetworkIdx() );
	// Ageia 무기를 Kinematic으로 설정하기전에 컬리젼이 안되도록 세팅(문제가 있어서입니다.)
	VEC3D vec;

	CWeaponInfo *pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	i3Vector::Set( &vec, (REAL32)( pWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( pWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( pWeapon, &vec );

	pWeapon->setDynamicActiveState( true, false );
	pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

	if( pWeapon->getContactHitBox() != nullptr)
	{
		pWeapon->getContactHitBox()->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION, 0);
	}

	pWeapon->DetachNode();
	pWeapon->SetOwner( m_pOwner);
	
	// 다중 무기에서는 리스폰 외에 주무기 셋팅하지 않음
	if ( !bUsedMultiWeapon)
	{
		setWeapon(nWeaponSlot, pWeapon);
	}

	// LOD를 세팅하고 바운딩 박스를 제거한다.
	pWeapon->OnChangeFPSView();

	if( BattleServerContext::i()->IsP2PActivate() )
	{
		MATRIX mat;
		
		pWeapon->getSceneObject()->SetMatrix( &mat );
		if( pWeapon->isExtensionActivate() )
			pWeapon->OnSetVisibleExtension( true );
	}

	//	월드에서 캐릭터로 옮긴다.
	pWeapon->Transmission( m_pOwner->getSceneObject(), m_pOwner, nullptr, false);			// SceneGraph 노드 구성, 캐릭터 노드로 붙입니다.
	pWeapon->ReturnToReady();

	if ( m_pOwner->getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_PRIMARY &&
		nWeaponSlot == WEAPON_SLOT_PRIMARY && m_pOwner->isLocal() &&
		MyRoomInfoContext::i()->getStage()->IsMultiWeapon() )
	{
		m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();
	}
	
	// bOnlyList
	// nSyncNumber - 다중무기 추가할 때 꼭 필요
	// bCheckDropList - 드랍리스트에서 찾이 않는 경우, 배틀오브젝트아이템에서 사용
	// bPushWeaponList - m_CharaWeaponList 드랍에서 필요
	// m_CharaWeaponList에 없으면 CreateAllSpecialDamageFilter 생성 안함
	g_pWeaponManager->MoveWeaponDropToChara( pWeapon, false, nSyncNumber, bCheckDropList, bPushWeaponList);	// 무기 관리자의 캐릭터용 리스트로 옮긴다.
	
	 if( nWeaponSlot == WEAPON_SLOT_SECONDARY && getCurrentSlotNumber() == WEAPON_SLOT_PRIMARY)
	{	// 주무기를 들고 보조무기만 습득한 경우
		// 초기화 순서 중요합니다.
		pWeapon->ReturnToReady();
		OnDetachWeapon( nWeaponSlot);
		
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			m_pOwner->getMultiWeaponContext()->ResetChange();
	}
	else
	{	// 나머지 무기에 대해서는 해당 무기로 들도록 합니다.
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY)
		{
			m_pOwner->getMultiWeaponContext()->HasPickupChange();
		}

		if( m_pOwner->Cmd_ChangeWeapon( nWeaponSlot))
		{
			// 다음 프레임에 무기 교체를 하기 전에 바꾸기 전 무기를 셋팅하는데
			if ( !( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && nWeaponSlot == WEAPON_SLOT_PRIMARY))
			{
				OnAttachWeapon(nWeaponSlot, getWeaponAttachRoot(), m_pOwner);		// 캐릭터에 GameNode를 붙입니다.
				getAnimContext()->ChangeAnimationContext();
			}
		}
		else
		{
			if ( nWeaponSlot == WEAPON_SLOT_PRIMARY)
			{
				pWeapon->ReturnToReady();
				OnAttachPurchasePrimWeapon(nWeaponSlot);
			}
		}
	}
	
	 //2014.07.16 ENG_김대영 버그 9893 
	 //탈취 모드에서 줍는 무기인 중식 칼에 스왑 사운드를 재생하도록 코드 추가함. 
	
	 if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  // 탈취 모드일 경우에만.
		 && pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::CHINESECLEAVER_SEIZED) ) 
	 {
		pWeapon->OnPlayChangeSound();
	 }

	 //2014.07.16 
	return true;
}

bool CGameCharaWeaponControl::OnDropedWeaponSearching( REAL32 rDeltatime)
{
	I3ASSERT( !m_pOwner->isNetwork() );

	return _ProcessDropedWeaponSearching();
}

bool CGameCharaWeaponControl::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( !m_pOwner->isNetwork())
	{
		if( getCurrentWeapon() == nullptr )	return false;

		CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
		I3ASSERT( pWeaponInfo != nullptr);

		CGameMissionManager * pMissionManager = CGameMissionManager::i();

		//	주 공격
		switch( pWeaponInfo->GetTypeClass())
		{			
		case WEAPON_CLASS_KNIFE:
		case WEAPON_CLASS_KNUCKLE:
			{	//	나이프일 경우 공격 처리
				if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
				{
					WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
					pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);
				//	pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				}
				else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
				{
				//	WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
					MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(getCurrentWeapon()->GetMainWeapon());
					mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				}

			}
			break;
		case WEAPON_CLASS_MISSION :
			{
				if ( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )
				{
					return false;
				}
				else 
				{
					ISNETWORK
					{
						if( (pMissionManager->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치전이 아닌경우
							(pMissionManager->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
							return false;
					}
				}

				if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) ||
					getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
					!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))
					return false;

				if( pMissionManager->IsEnableMy() == false)
				{	// C4 설치 불가능
					if( CHud::i())
						CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_NONAREA"));	// 지점에서만 설치할수 있심.
					return false;
				}
			}
			break;
		}

		if( !getCurrentWeapon()->Fire( WEAPON::PRIMARY_FIRE) )
			return false;

		//현재 무기 정보가 신규 점사 기능을 작동 시켜야 한다면 발사와 동시에 해당 상태로 무기를 바꿔줌.
		//다행이도 이 함수는 상속받은 부분이 없어 공통으로 들어갈 것으로 예상된다.
		INT32 autopointshot = 0;
		if (m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM) == false)
			autopointshot = pWeaponInfo->GetAutoPointShotEnable();
		else 
			autopointshot = pWeaponInfo->GetExtAutoPointShotEnable();

		if(autopointshot != 0)
			getCurrentWeapon()->addWeaponStateMask(WSM_POINTSHOTING);

		// 이건	버그이나 기획실의 요청으로 사용하지 않습니다. 발사중에 스왑안되는 기능입니다.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		// 수류탄 계열은 버그로 던지지 못하고 없어질 수 있습니다.

		switch(pWeaponInfo->GetTypeClass())
		{
		case WEAPON_CLASS_THROWING_GRENADE:
		case WEAPON_CLASS_THROWING_CONTAIN:
		case WEAPON_CLASS_THROWING_ATTACK:
		case WEAPON_CLASS_THROWING_HEAL:
		case WEAPON_CLASS_ROCKET_LAUNCHER:
			{
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
			}
			break;
		}

		// 다음 프레임에 발사가 안되도록
		if( pWeaponInfo->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT )
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

		m_pOwner->Cmd_ApplyCrosshair();

		// 편차를 계산
		getBoneContext()->_ProcessFire();

		return true;
		
	}
	
	return _ProcessFireForNetwork( pStartPos, pTargetPos);
//	return false;
}

bool CGameCharaWeaponControl::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( !m_pOwner->isNetwork() )
	{
		if( getCurrentWeapon() == nullptr )		return false;

		CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
		I3ASSERT( pWeaponInfo != nullptr);

		//	보조 공격
		switch( pWeaponInfo->GetTypeClass())
		{			
		case WEAPON_CLASS_KNIFE:
		case WEAPON_CLASS_KNUCKLE:
			{
				if( i3::same_of<WeaponKnife*>(getCurrentWeapon()))
				{
					WeaponKnife * pWeaponKnife = (WeaponKnife *) getCurrentWeapon();
					// pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
					pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_SECONDARY_ATTACK);

				}
				else if( i3::same_of<WeaponDualKnife*>(getCurrentWeapon()))
				{
					WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) getCurrentWeapon();
					
					MainWeapon_DualKnife* mainWeapon_DualKnife = static_cast<MainWeapon_DualKnife*>(pWeaponKnife->GetMainWeapon());
					
					mainWeapon_DualKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
				}
			}
			break;
		case WEAPON_CLASS_CIC:
			if( m_pOwner->is1PV())
			{
				MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(getCurrentWeapon()->GetMainWeapon());
		
				WeaponKnife * pWeaponKnife = (WeaponKnife*) mainWeapon_DualCIC->getLeftWeapon();
				
				if( i3::same_of<WeaponKnifeGender*>(mainWeapon_DualCIC->getLeftWeapon()) )
					break;

			//	pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				pWeaponKnife->GetMainWeapon()->SetKnifeAttackType(WEAPON_KNIFE_ATTACK);
				if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
				{
					((CGameCamera_FPS*)g_pCamera)->SetNeckAnim(true);
				}
			}
			break;
		}

		if( !getCurrentWeapon()->Fire( WEAPON::SECONDARY_FIRE) )
			return false;

		// 이건	버그이나 기획실의 요청으로 사용하지 않습니다. 발사중에 스왑안되는 기능입니다.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

		if( i3::same_of<WeaponBombTrigger*>(getCurrentWeapon())
			||	i3::kind_of<WeaponGrenadeLauncher*>(getCurrentWeapon()))
		{
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		}

		m_pOwner->Cmd_ApplyCrosshair();

		// 편차를 계산
		getBoneContext()->_ProcessFire();

		return true;
	}
	else
		return _ProcessFireForNetwork( pStartPos, pTargetPos, WEAPON::SECONDARY_FIRE);
}

bool CGameCharaWeaponControl::OnFireReady( void)
{
	if( getCurrentWeapon() == nullptr )	return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	
	// (임시) 발사 준비. 현재 투척 무기만 가능한 동작....변경시 애니메이션 추가 필요함.
	switch(pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_THROWING_GRENADE	:
	case WEAPON_CLASS_THROWING_CONTAIN	:
	case WEAPON_CLASS_THROWING_ATTACK	:
	case WEAPON_CLASS_THROWING_HEAL		:
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW				:	
		{
			getCurrentWeapon()->FireReady();
			return true;
		}
		break;
	}
	
	return false;

}

bool CGameCharaWeaponControl::OnExtActivate( void)
{
	WeaponBase * pWeapon = getCurrentWeapon();
	if( pWeapon == nullptr )				return false;
	if( !pWeapon->IsExtensionWeapon())	return false;
	if( m_pOwner->isLocal() )
	{
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
		{// 듀얼 탄창을 교체할 수 있는가?
			if( pWeapon->getTotalBulletCount() <= 0 &&
				pWeapon->getLoadedBulletDualCount() <= 0)
				return false;
		}

		if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE ))
			return false;
	}

	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
	bool bAnimPlay = true;
	// DualMagazine, Silencer은 캐릭터 애니메이션이 없다.
	if( m_pOwner->is3PV() )
	{
		if( i3::same_of<WeaponDualMagazine*, WeaponSilencer*>(pWeapon))
			bAnimPlay = false;
	}

	if( pWeapon->ExtensionActivate( true, bAnimPlay))
	{
		if( m_pOwner->isLocal())
			pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnExtDeactivate( void)
{
	WeaponBase * pWeapon = getCurrentWeapon();

	if( pWeapon == nullptr )				return false;
	if( !pWeapon->IsExtensionWeapon())	return false;

	if( m_pOwner->isLocal())
	{
		if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE) )
			return false;

		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
		{// 듀얼 탄창을 교체할 수 있는가?
			if( pWeapon->getTotalBulletCount() <= 0 &&
				pWeapon->getLoadedBulletDualCount() <= 0)
				return false;
		}
	}

	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
	bool bAnimPlay = true;
	// DualMagazine은 3인칭 캐릭터 애니메이션이 없다.
	if( m_pOwner->is3PV() )
	{
		if( i3::same_of<WeaponDualMagazine*, WeaponSilencer*>(pWeapon))
			bAnimPlay = false;
	}

	if( pWeapon->ExtensionActivate( false, bAnimPlay))
	{
		if( m_pOwner->isLocal())
			pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		return true;
	}

	return false;
}

bool CGameCharaWeaponControl::OnLoadMagazine( void)
{
	if( getCurrentWeapon() == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	//	총기 계열만 가능
	if( pWeaponInfo->isGun() == false)		return false;

	getCurrentWeapon()->SetEnable( true);

	if( !m_pOwner->isNetwork())
	{
		// 무기 스왑중에는 리로드 금지
		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))
			return false;
	}

	if (!m_pOwner->isNetwork() && getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
	{
		getCurrentWeapon()->setAutoFireCount(0);
		getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
	}
	
	if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER &&
		getCurrentWeapon()->isExtensionActivate() )
	{
		WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) getCurrentWeapon();

		if( !m_pOwner->isNetwork())
		{
			if( pGLauncher->getTotalShellCount() <= 0)
				return false;
		}
		
		pGLauncher->LoadShell();
	}
	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER	
	else if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER &&
		getCurrentWeapon()->isExtensionActivate() )
	{
		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) getCurrentWeapon();
		
		if( !m_pOwner->isNetwork())
		{
			if( pLauncher->getTotalShellCount() <= 0)
				return false;

			if( (pLauncher->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
				pLauncher->getLoadedShellCount() == pLauncher->getExtensionWeaponInfo()->GetLoadBullet() )
				return false;
		}
		
		//	LSS 장전
		pLauncher->LoadShell();
	}
	else
	{
		if( !m_pOwner->isNetwork())
		{
			//	소유 총알이 부족할 경우 장전을 할 수 없다.
			if( getCurrentWeapon()->getTotalBulletCount() <= 0)
				return false;

			if( (getCurrentWeapon()->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
				getCurrentWeapon()->getLoadedBulletCount() == getCurrentWeapon()->GetLoadBulletWithCashItem() )
				return false;
		}

		// Zoom을 키고 LoadMag를 하면 Zoom을 꺼버립니다.
		// Jinsik.kim(2015/04/10)
		// 김대영 : 소음기, 총검 무기 때문에 WEAPON_EXT_MASK_DIRECT_USE 이걸 사용하고 싶었으나, 지금 이걸 쓰면 안되서...
		// 이렇게 처리합니다.
		if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_SILENCER &&
			getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_BAYONET )
			m_pOwner->Cmd_Extension(false);	//사일런스는 재장전을 해도 유지 해야됨.
		getCurrentWeapon()->LoadMagazine();
	}
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}

bool CGameCharaWeaponControl::OnLoadMagazineDual( void)
{	
	if( getCurrentWeapon() == nullptr )								return false;
	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != nullptr);
	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)		return false;
	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_DUAL_MAG )
		return false;

	if (!m_pOwner->isNetwork() && getCurrentWeapon()->isWeaponStateMask(WSM_POINTSHOTING))
	{
		getCurrentWeapon()->setAutoFireCount(0);
		getCurrentWeapon()->removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);
	}

	if( !m_pOwner->isNetwork())
	{
		//	소유 총알이 부족할 경우 장전을 할 수 없다.
		if( getCurrentWeapon()->getTotalBulletCount() <= 0)	
			return false;
	}

	getCurrentWeapon()->SetEnable( true);

	//	무기 Extension 동작 
	getCurrentWeapon()->ExtensionActivate( true);

	if( !m_pOwner->isNetwork())
	{
		g_pFramework->getChatBox()->PutSystemChatting( L"Extension On");
		//	 애니메이션이 끝나면 발사 가능
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	}

	return true;
}

bool CGameCharaWeaponControl::OnLoadBullet( void)
{
	if( getCurrentWeapon() == nullptr )							return false;
	//	총기 계열만 가능
	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;

	if( !m_pOwner->isNetwork())
	{
		if( getCurrentWeapon()->isLoadBulletEnabled() == false)
			return false;
	}

	getCurrentWeapon()->SetEnable( true);
	getCurrentWeapon()->LoadBullet();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl::OnProcessRespawn( void)
{
	//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
	OnSelectWeapon( WEAPON_SLOT_MELEE, false);	// 무기제한에 포함되지 않는 근접무기로 초기 설정

	m_pOwner->Cmd_SyncWeapon();

	if( getCurrentWeapon() != nullptr )
		getCurrentWeapon()->ReturnToReady();	//	무기 교체시 이전 무기는 Ready 상태로 변경

	return true;
}

bool CGameCharaWeaponControl::OnLoadMagazine_Ready( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != nullptr);

	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)		return false;

	if( !m_pOwner->isNetwork())
	{
		//	소유 총알이 부족할 경우 장전을 할 수 없다.
		if( getCurrentWeapon()->getTotalBulletCount() <= 0)
			return false;

		if( (getCurrentWeapon()->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
			getCurrentWeapon()->getLoadedBulletCount() == getCurrentWeapon()->GetLoadBulletWithCashItem() )
			return false;

		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// 무기 스왑중에는 리로드 금지
			return false;

		if( !getCurrentWeapon()->isLoadMagazineEnabled())
			return false;
	}


	getCurrentWeapon()->SetEnable( true);
	getCurrentWeapon()->LoadMagazine_Ready();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}




