#include "pch.h"

#include "WeaponDual.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"

#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponDual);//, WeaponBase);

WeaponDual::WeaponDual()
{
	m_pLeftHandWeapon		= nullptr;
	m_bIsDummy		= false;
}


WeaponDual::~WeaponDual()
{
}

//virtual
void WeaponDual::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponBase::Create( pOwner, Itemid, bCreateInWeapon);

	I3ASSERT( m_pWeaponInfo != nullptr);
	
	// 왼손 무기를 생성
	if( bCreateInWeapon == false)
	{
		m_pLeftHandWeapon = g_pWeaponManager->QueryWeapon( getOwner(), m_pWeaponInfo->GetLeftHandWeaponItemID(), 0, false, true );
		I3ASSERT( m_pLeftHandWeapon != nullptr);

		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setPairWeapon( this);

		// 생성 후 공중에 띄워줍니다.
		VEC3D vec;
		i3Vector::Set( &vec, (REAL32)( m_pLeftHandWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( m_pLeftHandWeapon->getIdx() / 10) );
		g_pWeaponManager->MoveShapePosition( m_pLeftHandWeapon, &vec );
	}
}

//virtual
void WeaponDual::OnSetBlendAlpha( REAL32 val)
{
	WeaponBase::OnSetBlendAlpha( val);
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnSetBlendAlpha( val);
}

//virtual
void WeaponDual::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase ::OnSetMaterialColor( pfDiffuse, pfSpecular);

	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

//virtual
void WeaponDual::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponBase::AttachNode( pSceneNode, pOwner);
}

//virtual
void WeaponDual::DetachNode( void )
{
	WeaponBase::DetachNode();
}

//virtual
void WeaponDual::ReturnInstance()
{
	WeaponBase::ReturnInstance();
	
	m_pLeftHandWeapon->setPairWeapon( nullptr);
	g_pWeaponManager->ReleaseCharaWeapon( m_pLeftHandWeapon);
	m_pLeftHandWeapon = nullptr;
}

//virtual
void WeaponDual::OnChangeFPSView( void)
{
	WeaponBase::OnChangeFPSView();
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnChangeFPSView();
}

//virtual
void WeaponDual::OnChangeObserverView( void)
{
	WeaponBase::OnChangeObserverView();
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnChangeObserverView();
}

//virtual
void WeaponDual::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->ReturnToReady();
}

//virtual
void WeaponDual::SetOwner( CGameCharaBase * pOwner)
{
	WeaponBase::SetOwner( pOwner);
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->SetOwner( pOwner);
}

//virtual
void WeaponDual::OnDrop( void)
{
	WeaponBase::OnDrop();
}

void WeaponDual::SetEnable( bool bTrue, REAL32 tm)
{
	WeaponBase::SetEnable( bTrue, tm);

	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->SetEnable( bTrue, tm);
}


void WeaponDual::NET_LoadMagazineDualGun( CHARA_VIEW_TYPE eCAT)
{
	// KEYWORD : WEAPON_DUALHANDGUN
	{
		if( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL));
		}
		else
		{
			if( m_pOwner->is1PV())
			{
				//I3ASSERT( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT) != -1);
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT));
			}
			else
			{
				//I3ASSERT( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT) != -1);
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT));
			}
		}
	}

	setWeaponState( WEAPON::STATE_LOADMAG);

	// 왼손 무기 장전 모션
	if( m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->setWeaponState( WEAPON::STATE_LOADMAG);
		m_pLeftHandWeapon->ReloadAnimation(eCAT);
	}
}


void WeaponDual::NET_ChangeDualGun( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale )
{
	if( eCAT == CHARA_VIEW_1PV)
	{	// revision 28684
		I3ASSERT( getAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT) != -1);
		WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT), rTimeScale );
	}
	else
	{
		I3ASSERT( getAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT) != -1);
		WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT), rTimeScale );
	}

	// 왼손 무기 장전 모션
	if( m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->ChangeDualAnimation(eCAT, rTimeScale);
	}
}


