#include "pch.h"
#include "MainWeapon_Dual.h"

#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"

MainWeapon_Dual::MainWeapon_Dual(WeaponBase* p) : MainWeapon(p), m_pLeftHandWeapon(nullptr) 
{
	

}
	
void	MainWeapon_Dual::OnCreate( bool bCreateInWeapon)
{
	// 왼손 무기를 생성
	if( bCreateInWeapon == false)
	{
		CWeaponInfo* weapon_info = m_pWeaponBase->getWeaponInfo();
		I3ASSERT( weapon_info != nullptr);

		T_ItemID TLeftWeaponID = weapon_info->GetLeftHandWeaponItemID();
		m_pLeftHandWeapon = g_pWeaponManager->QueryWeapon( m_pWeaponBase->getOwner(), TLeftWeaponID, 0, false, true );
		I3ASSERT( m_pLeftHandWeapon != nullptr);
		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setPairWeapon( m_pWeaponBase );
		
		// 생성 후 공중에 띄워줍니다.
		VEC3D vec;
		i3Vector::Set( &vec, (REAL32)( m_pLeftHandWeapon->getIdx() % 10), g_pConfigEx->GetPhysix().HeightWeapon, (REAL32)( m_pLeftHandWeapon->getIdx() / 10) );
		g_pWeaponManager->MoveShapePosition( m_pLeftHandWeapon, &vec );
	}
}

void	MainWeapon_Dual::ReturnInstance() 
{
	if (m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->setPairWeapon( nullptr);
		g_pWeaponManager->ReleaseCharaWeapon( m_pLeftHandWeapon);
		m_pLeftHandWeapon = nullptr;
	}
}

void	MainWeapon_Dual::OnSetBlendAlpha( REAL32 val)
{
	if (m_pLeftHandWeapon)
		m_pLeftHandWeapon->OnSetBlendAlpha( val);
}

void	MainWeapon_Dual::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if (m_pLeftHandWeapon)
		m_pLeftHandWeapon->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

void	MainWeapon_Dual::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	if( m_pLeftHandWeapon != nullptr)
	{
		if( pSceneNode ==  m_pWeaponBase->getOwner()->getWeaponContext()->getDummyRightHand() )
		{
			m_pLeftHandWeapon->AttachNode(  m_pWeaponBase->getOwner()->getWeaponContext()->getDummyLeftHand(), pOwner);
			m_pLeftHandWeapon->ChangeGunDummy( WEAPON_ATTACH_GRIP);
			m_pLeftHandWeapon->SetEnable();
		}
		else //if( pSceneNode == m_pOwner->getWeaponContext()->getDummySidePurchase())
		{
			m_pLeftHandWeapon->DetachNode();
			m_pLeftHandWeapon->SetEnable( false);
		}

		m_pLeftHandWeapon->SetOwner(  m_pWeaponBase->getOwner() );
	}
}

void	MainWeapon_Dual::DetachNode( void )
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->DetachNode();

	//m_pLeftHandWeapon->SetEnable( false);
}

void	MainWeapon_Dual::OnChangeFPSView( void)
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnChangeFPSView();
}

void	MainWeapon_Dual::OnChangeObserverView( void)
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->OnChangeObserverView();
}

void	MainWeapon_Dual::ReturnToReady()
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->ReturnToReady();
}

void	MainWeapon_Dual::SetOwner( CGameCharaBase * pOwner)
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->SetOwner( pOwner);
}

void	MainWeapon_Dual::SetEnable( bool bTrue, REAL32 tm)
{
	if( m_pLeftHandWeapon != nullptr)
		m_pLeftHandWeapon->SetEnable( bTrue, tm);
}

//void	MainWeapon_Dual::PlayAnim_UI_IdleA( void)
//{
//	m_pWeaponBase->Reset();	
//}
//
//void	MainWeapon_Dual::PlayAnim_UI_IdleB( void)
//{
//	m_pWeaponBase->Reset();	
//}

void MainWeapon_Dual::NET_LoadMagazineDualGun( CHARA_VIEW_TYPE eCAT)
{
	// KEYWORD : WEAPON_DUALHANDGUN
	{
		if( m_pWeaponBase->getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		{
			 m_pWeaponBase->_PlayWeaponAnimWithApplyTimeScale(  m_pWeaponBase->getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL));
		}
		else
		{
			if( m_pWeaponBase->getOwner()->is1PV())
			{
				//I3ASSERT( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT) != -1);
				 m_pWeaponBase->_PlayWeaponAnimWithApplyTimeScale(  m_pWeaponBase->getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT));
			}
			else
			{
				//I3ASSERT( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT) != -1);
				 m_pWeaponBase->_PlayWeaponAnimWithApplyTimeScale(  m_pWeaponBase->getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT));
			}
		}
	}

	 m_pWeaponBase->setWeaponState( WEAPON::STATE_LOADMAG);

	// 왼손 무기 장전 모션
	if( m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->setWeaponState( WEAPON::STATE_LOADMAG);
		m_pLeftHandWeapon->ReloadAnimation(eCAT);
	}
}


void MainWeapon_Dual::NET_ChangeDualGun( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale )
{
	if( eCAT == CHARA_VIEW_1PV)
	{	// revision 28684
		I3ASSERT( m_pWeaponBase->getAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT) != -1);
		WF::PlayWeaponAnim( m_pWeaponBase,  m_pWeaponBase->getAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT), rTimeScale );
	}
	else
	{
		I3ASSERT( m_pWeaponBase->getAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT) != -1);
		WF::PlayWeaponAnim( m_pWeaponBase,  m_pWeaponBase->getAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT), rTimeScale );
	}

	// 왼손 무기 장전 모션
	if( m_pLeftHandWeapon)
	{
		m_pLeftHandWeapon->ChangeDualAnimation(eCAT, rTimeScale);
	}
}

