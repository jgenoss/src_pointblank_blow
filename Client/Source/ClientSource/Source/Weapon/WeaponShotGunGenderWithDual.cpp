#include "pch.h"
#include "WeaponShotGunGenderWithDual.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"
#include "AIDriver_Weapon.h"

namespace
{
	i3::string s_strAIName[AI_COUNT] =
	{
		"LoadMag_Ready",
		"LoadMag",
		"LoadBullet",
		"Fire",
		"Fire_Ext",
		"Idle",
		"Idle_Ext",
		"ExtensionActivate",
		"ExtensionDeactivate",
		"Change",
		"Change_Ext",
		"Change_Ext_Dual_Right",
		"Change_Ext_Dual_Left"
	};

	i3::string s_strGenderType[GENDER_COUNT] =
	{
		"Male",
		"Female"
	};
};

I3_CLASS_INSTANCE( WeaponShotGunGenderWithDual);//, WeaponShotGun);

WeaponShotGunGenderWithDual::WeaponShotGunGenderWithDual()
{
	m_bFirstLoadMag			= false;

	m_iLoadedBulletCount	= -1;
	m_iLoadableBulletCount	= -1;

	i3mem::FillZero(m_pBullet, sizeof(m_pBullet) );
	i3mem::Fill(m_i8AIIndex, -1, sizeof(m_i8AIIndex) );

	m_pDualWeapon			= nullptr;
	m_bUseDualWeapon		= false;
}

WeaponShotGunGenderWithDual::~WeaponShotGunGenderWithDual()
{
}

void WeaponShotGunGenderWithDual::_CreateDualWeapon(CGameCharaBase * pOwner, T_ItemID dual_itemID)
{
	// 듀얼용 무기를 생성
	m_pDualWeapon = g_pWeaponManager->QueryWeapon( pOwner, dual_itemID);
	I3ASSERT( m_pDualWeapon != nullptr);

	m_pDualWeapon->Reset();
	m_pDualWeapon->SetOwner( pOwner);
	m_pDualWeapon->setPairWeapon(this);

	// 생성 후 공중에 띄워줍니다.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)( m_pDualWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( m_pDualWeapon->getIdx() / 10) );
	g_pWeaponManager->MoveShapePosition( m_pDualWeapon, &vec );
}

void WeaponShotGunGenderWithDual::_OnChangeAnimProcess(REAL32 rTimeScale /* = 1.f*/)
{
	if( isExtensionActivate() )
	{
		if( m_pDualWeapon != nullptr )
		{	
			m_pDualWeapon->OnPlayChangeAniImpl(rTimeScale);
			return;
		}
	}

	WeaponBase::OnPlayChangeAniImpl( rTimeScale);
}

/*virtual*/ void WeaponShotGunGenderWithDual::_OnInitVairable()
{
	WeaponShotGun::_OnInitVairable();

	m_bFirstLoadMag			= false;

	m_iLoadedBulletCount	= -1;
	m_iLoadableBulletCount	= 0;

	i3mem::FillZero(m_pBullet, sizeof(m_pBullet) );
	i3mem::Fill(m_i8AIIndex, -1, sizeof(m_i8AIIndex) );

	m_bUseDualWeapon		= false;
}

/*virtual*/ void WeaponShotGunGenderWithDual::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);

	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		if( rDelay > 0.6f )		
			addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) )
		{
			MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);

			_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
			I3ASSERT( m_pOwner->getMoveContext() != nullptr);
			_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
		}
	}
}

void WeaponShotGunGenderWithDual::_VisibleBullet( bool bEnable)
{
	for( INT32 type = 0 ; type < LOD_COUNT ; ++type )
	{
		for ( INT32 idx = 0 ; idx < BULLET_COUNT ; ++idx)
		{
			if( m_pBullet[type][idx] != nullptr)
			{
				if( bEnable )
					m_pBullet[type][idx]->RemoveFlag(I3_NODEFLAG_INVISIBLE);
				else
					m_pBullet[type][idx]->AddFlag(I3_NODEFLAG_INVISIBLE);
			}
		}
	}
}

void WeaponShotGunGenderWithDual::_BindAnimationIndex(CGameCharaBase * pOwner)
{
	if( pOwner == nullptr )
		return;

	INT32 type = pOwner->isFemale() ? 1 : 0 ;	// 남 : 0 , 여 : 1;

	setAIIndex( WEAPON::AI_LOADMAGAZINE,			m_i8AIIndex[type][1]	);
	setAIIndex( WEAPON::AI_LOADBULLET,				m_i8AIIndex[type][2]	);
	setAIIndex( WEAPON::AI_FIRE,					m_i8AIIndex[type][3]	);
	setAIIndex( WEAPON::AI_FIRE_FOLDED,				m_i8AIIndex[type][4]	);
	setAIIndex( WEAPON::AI_IDLE,					m_i8AIIndex[type][5]	);
	setAIIndex( WEAPON::AI_IDLE_FOLDED,				m_i8AIIndex[type][6]	);
	setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE,		m_i8AIIndex[type][7]	);
	setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE,	m_i8AIIndex[type][8]	);
	setAIIndex( WEAPON::AI_CHANGE,					m_i8AIIndex[type][9]	);
	setAIIndex( WEAPON::AI_IDLE_FOLDED_CHANGE,		m_i8AIIndex[type][10]	);
	setAIIndex( WEAPON::AI_CHANGEDUAL_RIGHT,		m_i8AIIndex[type][11]	);
	setAIIndex( WEAPON::AI_CHANGEDUAL_LEFT,			m_i8AIIndex[type][12]	);
}

bool WeaponShotGunGenderWithDual::_SwitchingWeapon( WeaponBase * pPrevWeapon, WeaponBase * pNextWeapon, bool bPlaySound /* = false */)
{
	DetachNode();
	pNextWeapon->AttachNode( m_pOwner->getWeaponContext()->getAttachRoot(), m_pOwner);
	pNextWeapon->ReturnToReady();
	m_pOwner->Cmd_ApplyCrosshair();

	{
		VEC3D vec;
		pNextWeapon->SetEnable( true);
		pNextWeapon->SetPos( &vec );		// LocalPose를 초기화 해줘야합니다.
	}

	// *중요* 무기가 바뀌면 해당 무기에 맞는 애니메이션 셋을 세팅해야 합니다.
	m_pOwner->Cmd_Only_ChangeWeaponAction();

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
		m_pOwner->getWeaponContext()->setStaticCurrentWeapon( pNextWeapon );

	pPrevWeapon->SetVisible( false);
	pNextWeapon->SetVisible( true);

	// 교체 사운드 플레이
	if( bPlaySound)	pNextWeapon->PlaySwapSound();

	return true;
}

/*virtual*/ void WeaponShotGunGenderWithDual::PostCreate( void)
{
	I3ASSERT(m_pWeaponInfo != nullptr);

	_BindAnimationIndex(getOwner());

	T_ItemID dual_ItemID = m_pWeaponInfo->GetSubExtensionItemID(WEAPON::EXT_DUAL);

	if( dual_ItemID != 0)
		_CreateDualWeapon(getOwner(), dual_ItemID);
}

/*virtual*/ void WeaponShotGunGenderWithDual::Reset(void)
{
	WeaponBase::Reset();

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->Reset();

	SetEnableDualWeapon( false);
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnBindResource(void)
{
	WeaponBase::OnBindResource();

	// LOD별 Bullet Dummy 설정
	for( INT32 type = 0 ; type < LOD_COUNT ; ++type)
	{
		i3LOD * pLOD = getSceneObject()->GetBody()->getLOD(type);
		I3ASSERT( pLOD != nullptr );

		for( INT32 idx = 0; idx < BULLET_COUNT ; ++idx )
		{
			i3::string str_tmp = i3::format_string( "VolanimationDummy%02d", idx + 1 );
			m_pBullet[type][idx] = i3Scene::FindNodeByName( pLOD->getShapeNode(), str_tmp.c_str() );
		}
	}

	// Animation 설정
	for( INT32 type = 0 ; type < GENDER_COUNT ; ++type )
	{
		for( INT32 idx = 0 ; idx < AI_COUNT ; ++idx )
		{
			i3::string str_tmp = i3::format_string( "%s_%s", s_strAIName[idx], s_strGenderType[type] );
			m_i8AIIndex[type][idx] = static_cast<INT8>( m_pAICtx->FindAIStateByName( str_tmp.c_str() ) );
		}	
	}
}

/*virtual*/ void WeaponShotGunGenderWithDual::ReturnInstance()
{
	WeaponBase::ReturnInstance();

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->setPairWeapon( nullptr);
	g_pWeaponManager->ReleaseCharaWeapon( m_pDualWeapon);
	m_pDualWeapon = nullptr;
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate( rDeltaSeconds);
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)
		m_pDualWeapon->OnUpdate( rDeltaSeconds);
}

/*virtual*/ void WeaponShotGunGenderWithDual::SetOwner( CGameCharaBase * pOwner)
{
	_BindAnimationIndex(pOwner);

	WeaponBase::SetOwner( pOwner);

	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->SetOwner( pOwner);
}

/*virtual*/ void WeaponShotGunGenderWithDual::SetEnable( bool bTrue, REAL32 tm)
{
	WeaponBase::SetEnable( bTrue, tm);

	if( m_pDualWeapon != nullptr)
		m_pDualWeapon->SetEnable( bTrue, tm);
}

bool WeaponShotGunGenderWithDual::SetEnableDualWeapon( bool bUse, bool bPlaySound /*= false*/)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	I3ASSERT( m_pOwner != nullptr);

	if( m_bUseDualWeapon && !bUse)
	{	
		// 듀얼에서 싱글로
		m_bUseDualWeapon = bUse;
		return _SwitchingWeapon( m_pDualWeapon, this, bPlaySound );
	}
	else if( !m_bUseDualWeapon && bUse)
	{	
		// 싱글에서 듀얼로
		if( m_pDualWeapon->getLoadedBulletCount() <= 0)
			return false;

		m_bUseDualWeapon = bUse;
		return _SwitchingWeapon(this, m_pDualWeapon, bPlaySound);
	}

	return false;
}

/*virtual*/ void WeaponShotGunGenderWithDual::SetBulletCountForNetwork(INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount)
{
	WeaponBase::SetBulletCountForNetwork( iLoadedCount, iDualLoadedCount, iMaxCount );

	if( getDualWeapon() != nullptr )
	{
		INT32	iCashBullet	= getDualWeapon()->GetLoadBulletWithCashItem();
		if( iCashBullet < iDualLoadedCount)
			iDualLoadedCount	= iCashBullet;

		setLoadedBulletDualCount( iDualLoadedCount);
	}
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnPlayChangeSound( void)
{
	// 무기 교체 사운드 
	// SetEnableDualWeapon 함수 내부에서 ReturnToReady를 호출하여 StopSwapSound가 되기 때문에 이후에 해야 함. komet
	if( m_pDualWeapon != nullptr && isUsedDualWeapon())
		m_pDualWeapon->PlaySwapSound();
	else
		_PlaySwapSound();
}

/************************************************************************/
/*							Material Functions							*/
/************************************************************************/
/*virtual*/ void WeaponShotGunGenderWithDual::OnSetBlendAlpha( REAL32 val)
{
	WeaponBase::OnSetBlendAlpha( val);
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnSetBlendAlpha( val);
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase ::OnSetMaterialColor( pfDiffuse, pfSpecular);
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

/************************************************************************/
/*							View Process Functions						*/
/************************************************************************/

/*virtual*/ void WeaponShotGunGenderWithDual::OnChangeFPSView( void)
{
	WeaponBase::OnChangeFPSView();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnChangeFPSView();
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnChangeObserverView( void)
{
	WeaponBase::OnChangeObserverView();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->OnChangeObserverView();
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnSetVisibleExtension( bool bVal)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	m_pDualWeapon->OnSetVisibleExtension( bVal);
	else					_VisibleBullet( bVal);
}

/****************************************************************************/
/*							Weapon Order Functions							*/
/****************************************************************************/

/*virtual*/ bool WeaponShotGunGenderWithDual::Fire( WEAPON::FIRE_ORDER type)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	return m_pDualWeapon->Fire( type);
	else					return WeaponBase::Fire( type);
}

/*virtual*/ void WeaponShotGunGenderWithDual::FireEnd(void)
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	m_pDualWeapon->FireEnd();
	else					WeaponBase::FireEnd();
}

/*virtual*/ void WeaponShotGunGenderWithDual::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->ReturnToReady();
}

/*virtual*/ void WeaponShotGunGenderWithDual::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( m_bUseDualWeapon)	m_pDualWeapon->FireCollision( pStartPos, pTargetPos);
	else					WeaponBase::FireCollision( pStartPos, pTargetPos);
}

/*virtual*/ void WeaponShotGunGenderWithDual::LoadMagazine_Ready( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);

		addWeaponStateMask( WSM_LOADMAG_DISABLE );
	}

	OnPlayLoadMagReadyAnimation();
}

/*virtual*/ void WeaponShotGunGenderWithDual::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	I3ASSERT( m_pDualWeapon != nullptr);

	if( !m_pDualWeapon->isAttached() )
		WeaponBase::AttachNode( pSceneNode, pOwner);

	if( i3::same_of<CGameCharaBase*>(pOwner))
		m_pDualWeapon->SetOwner( m_pOwner);
}

/*virtual*/ void WeaponShotGunGenderWithDual::DetachNode( void )
{
	WeaponBase::DetachNode();
	I3ASSERT( m_pDualWeapon != nullptr);
	m_pDualWeapon->DetachNode();
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnDrop( void)
{
	WeaponBase::OnDrop();

	I3ASSERT( m_pDualWeapon != nullptr);

	m_bUseDualWeapon = false;
}

/****************************************************************************/
/*							Animation Play Functions						*/
/****************************************************************************/

/*virtual*/ void WeaponShotGunGenderWithDual::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/ )
{
	if( m_bUseDualWeapon)
	{
		SetEnableDualWeapon( false);
		SetEnableDualWeapon( true);
		setExtensionActivate( m_bUseDualWeapon);
	}
	else
	{
		SetEnableDualWeapon( true);
		SetEnableDualWeapon( false);	
		setExtensionActivate( false);
	}

	_OnChangeAnimProcess(rTimeScale);
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnPlayLoadMagReadyAnimation( void)
{
	I3ASSERT( m_pOwner != nullptr );
	INT32 type = m_pOwner->isFemale() ? 1 : 0 ;	// 남 : 0 , 여 : 1;

	_PlayWeaponAnimWithApplyTimeScale( m_i8AIIndex[type][0] );
}

/****************************************************************************/
/*						Weapon Extension Functions							*/
/****************************************************************************/

/*virtual*/ bool WeaponShotGunGenderWithDual::OnCheckExtensionPossible( bool bVal)
{
	if( WeaponBase::OnCheckExtensionPossible( bVal))
		return SetEnableDualWeapon( bVal, true);

	return false;
}

void WeaponShotGunGenderWithDual::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{	
	if( bAnimPlay )
	{
		REAL32 rTimeScale = 1.0f;
		if( m_pOwner )		
			rTimeScale = m_pOwner->GetWeaponSwapTimeScale();

		_OnChangeAnimProcess(rTimeScale);
	}
	else
		_OnPlayWeaponIdleAnim();	
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnProcessExtension( bool bVal)
{
	SetEnableDualWeapon( bVal, true);

	if( m_pOwner != nullptr && m_pOwner->isLocal())
	{
		i3::stack_wstring	wstrString;

		if( bVal)
		{
			//	"듀얼모드로 변경되었습니다."
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVATE_DUAL_WEAPON"));
			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
		}
		else
		{
			//	"듀얼모드가 해제되었습니다."
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STR_TBL_INGAME_HUD_DEACTIVATE_DUAL_WEAPON"));
			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
		}
	}
}

/****************************************************************************/
/*								Network Functions							*/
/****************************************************************************/

/*virtual*/ void WeaponShotGunGenderWithDual::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
	I3ASSERT( m_pDualWeapon != nullptr);
	if( m_bUseDualWeapon)	m_pDualWeapon->NET_Fire( pStartPos, pTargetPos, type);
	else					WeaponBase::NET_Fire( pStartPos, pTargetPos, type);
}

/****************************************************************************/
/*						Animation Callback function							*/
/****************************************************************************/

/*virtual*/ void WeaponShotGunGenderWithDual::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun::OnIdle( pCtx, tm);

	m_bFirstLoadMag = true;
	removeWeaponStateMask( WSM_READY_TO_LOADMAG);
	m_FireOrder = WEAPON::PRIMARY_FIRE;
}

/*virtual*/ bool WeaponShotGunGenderWithDual::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bLoaded = false;

	bLoaded = WeaponShotGun::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() == 0.f && m_bFirstLoadMag)
	{
		m_iLoadedBulletCount = -1;

		m_iLoadableBulletCount = GetLoadBulletWithCashItem() - getLoadedBulletCount();
		if( m_iLoadableBulletCount > BULLET_COUNT)
			m_iLoadableBulletCount = BULLET_COUNT;

		_VisibleBullet( true);
		m_bFirstLoadMag = false;
	}
	else if( pCtx->getLocalTime() > 0.15f)
	{
		if( m_iLoadedBulletCount == -1)
		{
			m_iLoadedBulletCount = 0;
		}
	}

	if( bLoaded && m_iLoadedBulletCount != -1)
	{
		m_iLoadedBulletCount++;
	}

	return bLoaded;
}

/*virtual*/ void WeaponShotGunGenderWithDual::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnLoadMag_Ready( pCtx, tm);

	I3ASSERT( pCtx != nullptr);
	REAL32 localTime = pCtx->getLocalTime();

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( localTime + tm > pAIState->getDuration() )
	{
		setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
		addWeaponStateMask( WSM_READY_TO_LOADMAG);
	}
}
