#include "pch.h"
#include "MainWeapon_DualGrenade.h"

#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaWeaponContext.h"

//손에서 떠나가는 시간 (애니메이션팀에서 전달받은 수치입니다.)
#define GRENADE_DETACHHAND_TIME_1	0.4f		// 12 frame
#define GRENADE_DETACHHAND_TIME_2	1.233f		// 37 frame


MainWeapon_DualGrenade::MainWeapon_DualGrenade(WeaponBase* p) : MainWeapon_Grenade(p)
{
	m_bCheckThrowFirst	= false;

	m_pLeftGrenade = nullptr;

	m_TypeGrenade = GRENADE_RIGHT;		// default 오른쪽

	for(INT32 i=0; i< GRENADE_MAX; i++)
	{
		m_aiiGrenadeIdle[i] = -1;
		m_aiiGrenadeFire[i] = -1;
	}
}

void	MainWeapon_DualGrenade::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Grenade::OnCreate( bCreateInWeapon);
	
	// 오른쪽(메인) 수류탄일 경우 더미용 왼쪽 수류탄을 생성해서 들고 있는다.
	//if( m_TypeGrenade == GRENADE_RIGHT)
	if( bCreateInWeapon == false)
	{
		m_pLeftGrenade = WeaponDualGrenade::new_object();
		I3ASSERT( m_pLeftGrenade != nullptr);

		// 재귀호출 되면서 영구 생성되는 것을 막기위해 Create하기 전에 설정한다.
		
		m_pLeftGrenade->PreCreateMainWeaponBeforeCreateCall();

		MainWeapon_DualGrenade* mainWeapon_left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		mainWeapon_left->setGrenadeType( GRENADE_LEFT);	

		m_pLeftGrenade->Create( m_pWeaponBase->getOwner(), 
			m_pWeaponBase->getWeaponInfo()->GetItemID(), true	);
	}

}

void MainWeapon_DualGrenade::OnBindResource_DualGrenade()
{
	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Idle_Left");
	m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Idle_Right");

	m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Fire_Left");
	m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Fire_Right");

	I3ASSERT( m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_LEFT] != -1);
	I3ASSERT( m_aiiGrenadeIdle[ MainWeapon_DualGrenade::GRENADE_RIGHT] != -1);	

	I3ASSERT( m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_LEFT] != -1);	
	I3ASSERT( m_aiiGrenadeFire[ MainWeapon_DualGrenade::GRENADE_RIGHT] != -1);	
	
}

void MainWeapon_DualGrenade::SetOwner_LeftGrenade(CGameCharaBase* pOwner)
{
	if (m_pLeftGrenade)
	{
		m_pLeftGrenade->SetOwner(pOwner);
	}
}

void MainWeapon_DualGrenade::ReturnInstance_LeftGrenade()
{
	if( m_pLeftGrenade)	
	{
		m_pLeftGrenade->getAttachNode()->RemoveAllChild();

		m_pLeftGrenade->RemoveAllChild();	

		m_pLeftGrenade->DetachNode();		//	GameNode , SceneNode 제거

		I3_SAFE_RELEASE( m_pLeftGrenade);
	}

}

void	MainWeapon_DualGrenade::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	MainWeapon_Grenade::OnUpdate_Grenade(rDeltaSeconds);
	
	if( m_pWeaponBase->getOwner())
	{
		// 왼쪽 수류탄
		if( !m_bCheckThrowFirst && m_TypeGrenade == GRENADE_LEFT && 
			m_pWeaponBase->getOwner()->getActionContext()->isUpper( CHARA_UPPER_FIRE_READY))
		{		
			REAL32 fUpperAnimLocalTime = m_pWeaponBase->getOwner()->getAnimContext()->getAIUpper()->getLocalTime();

			// 캐릭터의 던지는 준비 동작에 맞춰 오른쪽에 2개 수류탄중 LeftGrenade 모델을 왼손으로 옮긴다.
			if( fUpperAnimLocalTime > 0.25f)
			{
				this->AttachToLeftHand();
			}
		}
	}
}

void MainWeapon_DualGrenade::AttachToLeftHand(void)
{
	if( m_pWeaponBase->getOwner())	// 왼손에 붙인다.
	{
		//	WeaponBase 함수를 사용하여 this만 적용되게 하며 WeaponDualGrenade::AttachNode가 호출되지 않도록 한다.
		// AttachNode는 가상함수인데..WeaponBase버전을 쓰도록 한정시킨게 원본 구현이다.
		// 하지만, 이런 구현은 좋지 않다.
		m_pWeaponBase->WeaponBase::AttachNode( m_pWeaponBase->getOwner()->getWeaponContext()->getDummyLeftHand(), m_pWeaponBase->getOwner());
	}
}

void	MainWeapon_DualGrenade::Reset()
{
	MainWeapon_Grenade::Reset();
	
	if(m_TypeGrenade == GRENADE_RIGHT)	//	메인이 되는 오른쪽 수류탄에 호출될 경우 m_pLeftGrenade도 함께 관리
	{
		m_pLeftGrenade->SetVisible( true);

		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		
		mainWeapon_Left->PlayIdleAnim();
	}
}

void	MainWeapon_DualGrenade::ReturnToReady()
{
	MainWeapon_Grenade::ReturnToReady();

	if(m_TypeGrenade == GRENADE_RIGHT)	//	메인이 되는 오른쪽 수류탄에 호출될 경우 m_pLeftGrenade도 함께 관리
	{
		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
	
		mainWeapon_Left->PlayIdleAnim();
	}

	this->PlayIdleAnim();	
}

void MainWeapon_DualGrenade::PlayIdleAnim( void)
{
	m_pWeaponBase->setAIIndex( WEAPON::AI_IDLE, m_aiiGrenadeIdle[ m_TypeGrenade]);

	static_cast<WeaponGrenade*>(m_pWeaponBase)->WeaponGrenade::_OnPlayWeaponIdleAnim();
}

void MainWeapon_DualGrenade::PlayFireAnim( void)
{
	m_pWeaponBase->setAIIndex( WEAPON::AI_FIRE, m_aiiGrenadeFire[ m_TypeGrenade]);

	static_cast<WeaponGrenade*>(m_pWeaponBase)->WeaponGrenade::_OnPlayWeaponFireAnim();
}

void	MainWeapon_DualGrenade::FireReady()
{
	if( m_TypeGrenade == GRENADE_RIGHT)	//	메인이 되는 오른쪽 수류탄에 호출될 경우 m_pLeftGrenade도 함께 관리
	{
		MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
		mainWeapon_Left->PlayFireAnim();	// 왼쪽 수류탄만 준비 동작에 플레이
	}
}

void	MainWeapon_DualGrenade::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	MainWeapon_Grenade::OnEndFire(pCtx, tm);

	m_bCheckThrowFirst = false;

	// 던진후 아직 수류탄이 남아있다면 모델링은 제자리로 이동
	if( m_pWeaponBase->getLoadedBulletCount() > 0)
	{
		if( m_TypeGrenade == GRENADE_RIGHT)	//	메인이 되는 오른쪽 수류탄에 호출될 경우 m_pLeftGrenade도 함께 관리
		{
			m_pLeftGrenade->SetVisible( true);

			MainWeapon_DualGrenade* mainWeapon_Left = static_cast<MainWeapon_DualGrenade*>(m_pLeftGrenade->GetMainWeapon());
			mainWeapon_Left->PlayIdleAnim();
			mainWeapon_Left->AttachToRightHand();
		}

		m_pWeaponBase->SetVisible( true);
		this->PlayIdleAnim();
		this->AttachToRightHand();
	}

}

void	MainWeapon_DualGrenade::AttachToRightHand(void)
{
	if( m_pWeaponBase->getOwner())	//	오른손에 붙인다.
	{
		//	WeaponBase 함수를 사용하여 this만 적용되게 하며 WeaponDualGrenade::AttachNode가 호출되지 않도록 한다.
		m_pWeaponBase->WeaponBase::AttachNode( m_pWeaponBase->getOwner()->getWeaponContext()->getDummyRightHand(), m_pWeaponBase->getOwner());	// 클래스한정을 떼면 안된다.
	}

}

void	MainWeapon_DualGrenade::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner )
{
	// OnAttachWeapon 에서는 메인이 되는 오른쪽 수류탄만 AttachNode를 호출하기 때문에 왼쪽 수류탄도 같이 관리 	
	if( m_TypeGrenade == GRENADE_RIGHT)
	{
		if( m_pLeftGrenade)
			m_pLeftGrenade->AttachNode( pSceneNode, pOwner);
	}
}

void	MainWeapon_DualGrenade::DetachNode()
{
	// OnDetachWeapon 에서는 메인이 되는 오른쪽 수류탄만 AttachNode를 호출하기 때문에 왼쪽 수류탄도 같이 관리 
	if( m_TypeGrenade == GRENADE_RIGHT)
	{
		if( m_pLeftGrenade)
			m_pLeftGrenade->DetachNode();
	}
}

void	MainWeapon_DualGrenade::OnSetBlendAlpha( REAL32 val)
{
	if(m_pLeftGrenade)
		m_pLeftGrenade->OnSetBlendAlpha( val);
}

void	MainWeapon_DualGrenade::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if(m_pLeftGrenade)
		m_pLeftGrenade->OnSetMaterialColor( pfDiffuse, pfSpecular);
}

bool MainWeapon_DualGrenade::_CheckThrow(REAL32 LocalTime)
{
	// 실제 체크는 메인(오른쪽) 수류탄에서만 한다. (왼쪽은 m_pLeftGrenade 는 더미일 뿐)
	if( m_TypeGrenade == GRENADE_LEFT)
		return false;

	// 오른손부터 던진다.
	if(!m_bCheckThrowFirst && LocalTime > GRENADE_DETACHHAND_TIME_1)
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// 네트워크는 패킷으로 처리합니다.
		{
			//손에서 떠나가는 시간
			// 시점이 향하고 있는 목표 지점을 구한다.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);

			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );

			m_pWeaponBase->SetVisible( false);
		}

		m_bCheckThrowFirst = true;
	}
	//	이후 왼손 던진다.
	else if (LocalTime > GRENADE_DETACHHAND_TIME_2)
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// 네트워크는 패킷으로 처리합니다.
		{
			//손에서 떠나가는 시간
			// 시점이 향하고 있는 목표 지점을 구한다.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);

			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );

			m_pLeftGrenade->SetVisible( false);
		}

		return true;
	}

	return false;
}

