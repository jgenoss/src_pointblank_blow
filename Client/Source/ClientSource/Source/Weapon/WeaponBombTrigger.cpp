#include "pch.h"
#include "WeaponBombTrigger.h"
#include "GlobalVariables.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "StageBattle.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "GameMissionManager.h"
#include "WeaponItemID.h"
#include "WeaponFunction.h"
#include "MainWeapon_C5.h"


#define WEAPON_BOMB_THROW_TIME_C5		0.4f
#define WEAPON_BOMB_TRIGGER_TIME_C5		0.05f

I3_CLASS_INSTANCE( WeaponBombTrigger);//, WeaponBase);

WeaponBombTrigger::WeaponBombTrigger()
{
	m_pBomb				= nullptr;
	m_nSideWeaponType	= WEAPON_CLASS_UNKNOWN;

	m_uCount			= 0x0000FFFF & i3Math::Rand();
	m_uCount |= (UINT32)(g_pRand->GetRand( m_uCount) << 20);
}

WeaponBombTrigger::~WeaponBombTrigger()
{	
}

void WeaponBombTrigger::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}


// classType과 number는 연결된 무기가 됩니다..
void WeaponBombTrigger::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	_OnInitVairable();

	if( g_pWeaponInfoDataBase != nullptr)
	{
		CWeaponInfo* thisWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(Itemid, true);

		if( thisWeaponInfo->GetBombtriggerItemID() )
		{
			m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( thisWeaponInfo->GetBombtriggerItemID(), true);
		}
		else
		{
			m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT));
		}

		m_pAbsoluteWeaponInfo = m_pWeaponInfo;		//	처음 초기화때만 셋팅되는 메인 정보이다.

		m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	첫번째 익스텐션으로 초기화 설정. 중간에 바뀔수 있는 값

		//	무기 매니저에서 리소스를 받아온다.
		//i3GameResChara * pRes = g_pWeaponManager->QueryResource( WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT));
		i3GameResChara * pRes = nullptr;
		if( thisWeaponInfo->GetBombtriggerItemID() )
		{
			pRes = g_pWeaponManager->QueryResource( thisWeaponInfo->GetBombtriggerItemID());
		}
		else
		{
			pRes = g_pWeaponManager->QueryResource( WEAPON_CLASS_BOMBTRIGGER, WEAPON::getItemIndex(WEAPON::BOMBTRIGGER_DEFAULT));
		}
		I3ASSERT( pRes != nullptr);
		if(pRes == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN, "Bome Trigger 리소스가 없습니다.");
		}
	
		////
		if (!m_mainWeapon)
			m_mainWeapon.reset( this->CreateMainWeapon() );
		////
		//
		// 아래 함수가 WeaponBase::Create가 아니기때문에 좋지 않은 구조이다..
	    //

		i3Chara::Create( g_pFramework, pRes);

		Instantiate();

		// 트리거에 연결할 무기가 없으면 안됩니다..(보통 던지는 무기류)
		{
			m_pBomb = g_pWeaponManager->QueryWeapon( pOwner, Itemid, 0, false, true);

			VEC3D vPos;
			i3Vector::Set( &vPos, 0.05f, 0.f, 0.f);
			m_pBomb->SetPos( &vPos);
			if( g_pFramework->IsBattleStage() )
				m_pBomb->SetVisible( false);
			m_pBomb->setPairWeapon( this);
		}

		//	노드 상위에 알파처리를 위한 AttrSet을 붙인다.
		{
			m_pAttrSet = i3AttrSet::new_object_ref();
			m_pAttrSet->SetPriority( 0);	

			while( getAttachNode()->GetChildCount() > 0)
			{
				i3Node * pChild = getAttachNode()->GetChild( 0);
				pChild->ChangeParent( m_pAttrSet);
			}	

			getAttachNode()->AddChild( m_pAttrSet);		

			m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
			m_pBlendEnable->Set( false);
			m_pAttrSet->AddAttr( m_pBlendEnable);

			i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
			pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
			pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
			m_pAttrSet->AddAttr( pBlendMode);

			m_pMaterialAttr = i3MaterialAttr::new_object_ref();
			m_pAttrSet->AddAttr( m_pMaterialAttr);

			COLORREAL col;

			i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
			m_pMaterialAttr->SetDiffuse( &col);
			m_pMaterialAttr->SetSpecular( &col);
			m_pMaterialAttr->SetShininess( 128.0f);
			
		}
    
		//	Set Update Mode
		m_pBody = getSceneObject()->GetBody();
		I3ASSERT( m_pBody != nullptr);

		//	임의로 변경하기 쉽게 하기 위해 강제로 LOD Slope값을 지정합니다. (기존에 CharaTool에서 지정한 Slope값은 사용하지 않습니다. komet)
		m_pBody->setSlope( WEAPON_LOD_SLOPE);
		m_pBody->removeStyle( I3BS_LOD_AUTO);
//		m_pBody->AddFlag( I3_NODEFLAG_MUSTBEPROCESS , true);

		_LoadAllSound();
		
	}

	{
		m_pLight = i3Light::new_object();

		m_pLight->SetLightType( I3G_LIGHT_POINT);

		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
	}

#if defined( TEST_COLLISION_LINE )
	COLORREAL Col;
	i3Color::Set( &Col, 0.8f, 0.8f, 0.0f, 0.8f);
	i3Node * pNode = (i3Node*) i3SceneUtil::BuildLine( m_pVA, 1, &Col);
	g_pFramework->Get3DRoot()->AddChild( pNode);
#endif

	// Init
	_InitParameters();

	_InitCollideeGroup();
	
	// 기본적으로 오브젝트를 어딘가에 붙여줍니다.
	setDynamicActiveState( true, false);
	SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);

	PostCreate();
}

void WeaponBombTrigger::Reset( void)
{
	WeaponBase::Reset();

	// 폭탄의 갯수로 맞춰줍니다.
	if( getBomb() != nullptr && getBomb()->getWeaponInfo() != nullptr)
	{
		m_pBomb->Reset();
		setLoadedBulletCount( getBomb()->GetLoadBulletWithCashItem() );
		//setTotalBulletCount( getBomb()->GetMaxBulletWithCashItem());

		// hansoft 11744.
		INT32 totalBullet = getBomb()->GetMaxBulletWithCashItem() - getBomb()->GetLoadBulletWithCashItem();
		if (totalBullet < 0)
			totalBullet = 0;
		setTotalBulletCount(totalBullet);
	}

	m_uCount			= 0x0000FFFF & i3Math::Rand();
	m_uCount |= (UINT32)(g_pRand->GetRand( m_uCount) << 20);

	// revision 46747 C-5 자살시 3인칭 시점에서 Explosion()함수 호출 안되는 버그 수정
	WeaponBase * pWeapon = nullptr;
	for(size_t idx = 0 ; idx < m_BombList.size() ; idx ++)
	{
		pWeapon = m_BombList[idx];
		I3ASSERT( pWeapon != nullptr );

		if( i3::same_of<WeaponC5*>(pWeapon) )
		{
			MainWeapon_C5* mainWeapon_C5 = static_cast<MainWeapon_C5*>(pWeapon->GetMainWeapon());
			mainWeapon_C5->RemoveInvalidBomb();
		}
	}

	m_BombList.clear();
}

void WeaponBombTrigger::SetOwner( CGameCharaBase * pOwner)
{
	WeaponBase::SetOwner( pOwner);

	if( m_pBomb != nullptr)
		m_pBomb->SetOwner( pOwner);
}

void WeaponBombTrigger::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	WeaponBase::NET_Fire( pStartPos, pTargetPos, type);

	if( m_pBomb == nullptr)	return;

	WF::PlayWeaponAnim( m_pBomb, getAIIndex( WEAPON::AI_FIRE) );
}

bool WeaponBombTrigger::Fire( WEAPON::FIRE_ORDER type)
{
	if( getBomb() == nullptr)
		return false;

	if( (type == WEAPON::SECONDARY_FIRE) && (getLoadedBulletCount() <= 0) )
		return false;

	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

	NET_Fire(nullptr, m_TargetPos, m_FireOrder );

	addWeaponStateMask( WSM_NEED_SEND_FIREACTION);

	return true;
}

namespace
{
	void HackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x000F0000)>>16) + 1) <<16) | (ra & 0x0000FFFF) | ((g_pRand->GetRand(ra&0x0000FFFF) << 20));
		if( ((curCnt & 0x000F0000) >> 16) > (UINT32) LocaleValue::GetInt32("FireWall/GrenadeCount") ||
			((curCnt & 0xFFF00000)>>20) != (UINT32)(g_pRand->GetRand( curCnt&0x0000FFFF) & 0x00000FFF) )
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f ,C_MANY_GRENADE_COUNT);
		}
	}
}

void WeaponBombTrigger::ThrowBomb( INT32 nNetSlotIdx)
{
	VEC3D vPos;
	VEC3D offset;

	// 폭탄을 던지는 액션이다.
	{
		REAL32	Theta;
		VEC3D	vAt;
		MATRIX	mtx;

		i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pOwner->GetMatrix()));

		Theta = atan2f( getX( &vAt), getZ( &vAt));
		i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

		m_pOwner->getDBInfo()->GetFPSCamPos( &offset);
		if( m_pOwner->isLeftHand())	offset.x = -offset.x;

		i3Vector::Copy( &vAt, (VEC3D*)&offset);
		i3Vector::AddY( &vAt, -0.1f);

		//Rotate
		i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( m_pOwner->GetMatrix()), - m_pOwner->getBoneContext()->getRho());
		i3Vector::TransformCoord( &vAt, &vAt, &mtx);
		
		if( m_pOwner->is1PV())
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_NECK)));
		}
		else
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_IK)));
			i3Vector::SetY( &vPos, getY( m_pOwner->GetPos()) + m_pOwner->getCameraContext()->getIKHeightForCam());
		}

		i3Vector::Add( m_StartPos, &vAt,&vPos);
		
		GetShotTargetPos( m_TargetPos);
	}

	T_ItemID bombID = getBomb()->getWeaponInfo()->GetItemID();
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( bombID);
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( bombID);

	WeaponBase * pThrowBomb = g_pWeaponManager->GetFreeThrowWeapon( classType, number);
	// 던진 폭탄 리스트에 등록
	if( pThrowBomb != nullptr)
	{
		m_BombList.push_back( pThrowBomb);

		pThrowBomb->SetOwner( getOwner());	// 먼저 세팅해야됩니다.	
		pThrowBomb->Reset();
			
		pThrowBomb->SetPos( &vPos);

		VEC3D dir;
		i3Vector::Sub( &dir, m_TargetPos, m_StartPos);
		i3Vector::Normalize( &dir, &dir);
		pThrowBomb->NET_FirstThrow( m_StartPos, &dir, pThrowBomb->getNetworkIdx());
		// Network ID
		// 2014-08-22 c5폭탄의 바인드 되는 네트워트 아이디값은 해당 버퍼에서 처리된 값이어야 정상처리 되어 수정함. // ENG_김대영
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex());
		pThrowBomb->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());


		pThrowBomb->SetPos( m_StartPos);
		GAMEEVENT::BindNetC5Info( pThrowBomb->getNetworkIdx(), pThrowBomb, m_StartPos);

		MainWeapon_C5* mainWeapon_Bomb = static_cast<MainWeapon_C5*>(pThrowBomb->GetMainWeapon());
		mainWeapon_Bomb->addBombState( WEAPON_BOMBSTATE_THROWING);
		
		// 손에 들고 있는 C5는 꺼줍니다
		// 대신 한 발 더 들고 있으면 안끕니다.
		if (getLoadedBulletCount() < 2)
		{
			m_pBomb->SetVisible(false);
		}

		// 해킹 방지 count
		if( getOwner() != nullptr && getOwner()->isLocal())
		{
			// KEYWORD : Hack Protect
			HackDetection(m_uCount);
		}

	#ifdef ENABLE_TEST_COMMAND
		if( !g_pWeaponManager->COMMAND_IsUnlimitGrenade())		//	Use Cheat
	#endif
		{
			PB_VM_S

				INT32 iBullet = getLoadedBulletCount();
				iBullet--;
				setLoadedBulletCount( iBullet);

			PB_VM_E
		}
	}
}

void WeaponBombTrigger::FireEnd()
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
}


void WeaponBombTrigger::OnSetBlendAlpha( REAL32 val)
{
	WeaponBase::OnSetBlendAlpha( val);

	if( m_pBomb != nullptr)
		m_pBomb->OnSetBlendAlpha( val);
}

void WeaponBombTrigger::OnChangeFPSView( void)
{
	WeaponBase::OnChangeFPSView();

	if( m_pBomb != nullptr)
		m_pBomb->OnChangeFPSView();
}

void WeaponBombTrigger::OnChangeObserverView( void)
{
	WeaponBase::OnChangeObserverView();

	if( m_pBomb != nullptr)
		m_pBomb->OnChangeObserverView();
}

REAL32 WeaponBombTrigger::GetThrowSpeed()
{
	if( m_pBomb != nullptr)
		return m_pBomb->GetThrowSpeed();

	return 0.f;
}

void WeaponBombTrigger::PlayAnim_UI_IdleB( void)
{
	if( m_pBomb != nullptr)
		m_pBomb->PlayAnim_UI_IdleB();
}

///////////////////////////////////////////////////////////////////////////////////////////
//		Animation Callback
void WeaponBombTrigger::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	bool bLoaded = true;
	
	if( getOwner() == nullptr)		return;
	
	I3ASSERT( m_pBomb != nullptr);

	if( m_pOwner->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) && (m_pBomb != nullptr) && m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx() )
	{
		m_pBomb->SetVisible( false);
	}

	if( (getLoadedBulletCount() == 0) && (getTotalBulletCount() == 0) && (m_BombList.size() == 0) )
	{
		setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);
		bLoaded = false;
	}
	
	if( bLoaded)
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE);
		setWeaponState( WEAPON::STATE_READY);
	}
}

void WeaponBombTrigger::OnFireDomi(i3AIContext* pCtx, REAL32 tm) {}

void WeaponBombTrigger::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	if( pCtx->getLocalTime() == 0.0f)
	{
		I3ASSERT( m_pOwner != nullptr);
		I3ASSERT( getWeaponInfo() != nullptr);

		// 연이어 발사하지 못하도록...
		setWeaponState( WEAPON::STATE_FIRE);
		if( m_pOwner != nullptr && m_pOwner->isLocal())
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}
		
		removeWeaponStateMask( WSM_EMPTY_CARTRIDGE);
		setAutoFireCount( 0);

		// 발사 사운드
		_OnPlayFireSound();
	}
	else
	{
		REAL32 rDelay = pCtx->getLocalTime() + tm;

		INT32 autoFireCount = getAutoFireCount();
		
		if( autoFireCount == 0)
		{
			switch( m_FireOrder)
			{
			case WEAPON::PRIMARY_FIRE :
				// 일반 Fire는 Bomb을 던지지 않았을 경우 NetFire Sync를 보내지 않는다.
				if (m_BombList.empty() == true) break;

				// 격발기를 누르는 액션이다.
				if( (rDelay > WEAPON_BOMB_TRIGGER_TIME_C5))
				{
					autoFireCount++;

					NET_FireSync( m_TargetPos, m_FireOrder );

					if( IsDedicatedHost() == false)
						break;

					for( size_t i = 0; i < m_BombList.size(); i++)
					{
						WeaponBase * pWeapon = m_BombList[i];
						I3ASSERT( pWeapon->GetRefCount() > 0);
						I3ASSERT( i3::same_of<WeaponC5*>(pWeapon));

						MainWeapon_C5* mainWeapon_C5 = static_cast<MainWeapon_C5*>(pWeapon->GetMainWeapon());

						if( !mainWeapon_C5->isBombState( WEAPON_BOMBSTATE_SETTLED) )
							continue;

						// 네트워크 정보
						if( pWeapon->getNetworkIdx() != -1)
						{
							GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(pWeapon->getWeaponInfo()->GetItemID(), pWeapon->GetPos(), NET_GRENADE_STATE_CLICKFIRE);

							GAMEEVENT::Write_ThrowingWeapon( pWeapon->getNetworkIdx(), &throwInfo);
						}

						mainWeapon_C5->addBombState( WEAPON_BOMBSTATE_TIMESTART);
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 0, pWeapon->GetPos(), false);

						// 던져진 리스트에서 삭제
						i3::vu::erase_index(m_BombList, i);
						i--;
					}
				}
				break;
			case WEAPON::SECONDARY_FIRE :
				// 폭탄을 던지는 액션이다.
				{
					REAL32 rOffset = 0.f;
					if( !BattleServerContext::i()->IsP2PHost() && m_pOwner->isLocal())
					{
						rOffset = -0.1f;
					}
					if( (rDelay + tm > WEAPON_BOMB_THROW_TIME_C5 + rOffset))
					{	// 폭탄을 던진다.
						NET_Throw( m_StartPos, m_TargetPos, -1);
						
						autoFireCount++;
					}
				}
				break;
			}
		}

		setAutoFireCount( autoFireCount);

		if( rDelay + tm > m_pOwner->getAnimContext()->getUpperDuration() )
		{
			_OnPlayWeaponIdleAnim();
			removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}	
	}
}

/*virtual*/ void WeaponBombTrigger::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	if( getFireOrder() == WEAPON::SECONDARY_FIRE )
	{
		INT32 total_count = getTotalBulletCount();

		if( total_count > 0 )
		{
			INT32 reload_count = 1;

			setLoadedBulletCount(reload_count);
			setTotalBulletCount( MIN(0, total_count - reload_count) );
		}
	}

	WeaponBase::OnEndFire(pCtx, tm);
}

/*virtual*/void WeaponBombTrigger::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponBase::AttachNode( pSceneNode, pOwner);

	if( pSceneNode == m_pOwner->getWeaponContext()->getAttachRoot())
	{
		m_pBomb->AttachNode( m_pOwner->getWeaponContext()->getDummyLeftHand(), pOwner);
		m_pBomb->SetEnable();
		m_pBomb->ReturnToReady();

		if (getLoadedBulletCount() <= 0)
		{
			m_pBomb->SetVisible(false);
		}

	}
}

/*virtual*/void WeaponBombTrigger::DetachNode( void )
{
	WeaponBase::DetachNode();
	m_pBomb->DetachNode();
	m_pBomb->SetVisible( false);
}

/*virtual*/void WeaponBombTrigger::ReturnInstance( void)
{
	WeaponBase::ReturnInstance();
	
	m_pBomb->setPairWeapon( nullptr);
	g_pWeaponManager->ReleaseCharaWeapon( m_pBomb);
	m_pBomb = nullptr;
}

void WeaponBombTrigger::NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx)
{
	ThrowBomb( nNetSlotIdx);
}

void WeaponBombTrigger::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx)
{
	WeaponBase::NET_Throw( pStartPos, pTargetPos, nThrowNetIdx );

	VEC3D start, dir;
	
	if( pStartPos == nullptr )
	{	
		m_pOwner->getCameraContext()->getVirtualCamPosition( &start);					// 현재는 Camera의 지점
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	// 던져집니다.
	NET_FireSync( pTargetPos );

	//다른캐릭터나 NoneServer일경우만 처리했으나
	//내 캐릭터도 바로 처리한다.
	if( getOwner() != nullptr )
	{
		i3Vector::Sub( &dir, pTargetPos, &start);
		i3Vector::Normalize( &dir, &dir);

		NET_FirstThrow( &start, &dir, nThrowNetIdx );
	}
	
}

/*virtual*/ void WeaponBombTrigger::ApplyBulletWithDropItem(REAL32 rRate)
{
	setLoadedBulletCount( 1);	//C5는 무조건 하나 준다.
	setTotalBulletCount(1);
}

