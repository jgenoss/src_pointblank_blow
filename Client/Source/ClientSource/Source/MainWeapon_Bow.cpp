#include "pch.h"

#include "MainWeapon_Bow.h"
#include "WeaponFunction.h"


MainWeapon_Bow::MainWeapon_Bow(WeaponBase* p) : MainWeapon_Throw(p) 
{
	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	m_rBoundObjectLen			= 0.f;

	m_aii_UI_Idle_A		= -1;
	m_aii_UI_Idle_B		= -1;
	m_aii_Change_1PV		= -1;
	m_aii_Change_3PV		= -1;

	m_aii_ExtensionActivate_3PV = -1;
	m_aii_ExtensionDeactivate_3PV = -1;

	OnChangingFlag = false;
	OnActivateFlag = false;
	OnDeActivateFlag = false;

}

void MainWeapon_Bow::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	if (m_pWeaponBase->isExtensionActivate())
	{
		MainWeapon_Throw::OnEndFire(pCtx, tm);
	}
}

void	MainWeapon_Bow::Reset()
{
	MainWeapon_Throw::Reset();

	i3Vector::Zero( &m_vRightDir);

	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	m_rBoundObjectLen			= 0.f;

//	setLoadedBulletCount( 15);
	i3Matrix::Identity( m_pWeaponBase->GetMatrix());
	//활은 왼손에 Attacht하도록 합니다.
	m_pWeaponBase->SetCurrentAttachLeftHand();

	for( size_t i = 0; i < m_ArrowMaterialList.size(); i++)
	{
		i3Node * pNode = m_ArrowMaterialList[ i];

		pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	}

}

void	MainWeapon_Bow::OnCreate( bool bCreateInWeapon)
{
	i3Body* body = m_pWeaponBase->getSceneObject()->GetBody();

	for(UINT32 i = 0; i < body->getLODCount(); i++)
	{
		i3Node * pFind = i3Scene::FindNodeByName( body->getLOD(i)->getShapeNode(), "Model_Arrow");
		i3Node * pGrenadeFind = i3Scene::FindNodeByName( body->getLOD(i)->getShapeNode(), "Model_GrenadeArrow");
		if( pFind)
			m_ArrowMaterialList.push_back(pFind);

		if (pGrenadeFind)
		{
			m_ArrowMaterialList.push_back(pGrenadeFind);
		}
	}

}

void	MainWeapon_Bow::Transmission_Bow(bool bToWorld)
{
	for(size_t i = 0; i < m_ArrowMaterialList.size(); i++)
	{
		i3Node * pNode = m_ArrowMaterialList[ i];

		if( bToWorld)
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		else
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
}

void	MainWeapon_Bow::OnBindResource()
{
	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	m_aii_UI_Idle_A		= (INT8) pAICtx->FindAIStateByName( "UI_Idle_A");
	m_aii_UI_Idle_B		= (INT8) pAICtx->FindAIStateByName( "UI_Idle_B");
	m_aii_Change_1PV		= (INT8) pAICtx->FindAIStateByName( "Change1PV");
	m_aii_Change_3PV		= (INT8) pAICtx->FindAIStateByName( "Change3PV");
	m_aii_ExtensionActivate_3PV		=	(INT8) pAICtx->FindAIStateByName( "ExtensionActivate3PV");
	m_aii_ExtensionDeactivate_3PV		=	(INT8) pAICtx->FindAIStateByName( "ExtensionDeactivate3PV");
	
}

void	MainWeapon_Bow::PlayAnim_UI_IdleA_Bow()
{

	//활은 왼손에 Attacht하도록 합니다.
	m_pWeaponBase->SetCurrentAttachLeftHand();


	WF::PlayWeaponAnim(m_pWeaponBase, m_aii_UI_Idle_A);

}

void	MainWeapon_Bow::PlayAnim_UI_IdleB_Bow()
{
		//활은 왼손에 Attacht하도록 합니다.
	m_pWeaponBase->SetCurrentAttachLeftHand();
	
	WF::PlayWeaponAnim(m_pWeaponBase, m_aii_UI_Idle_B);

}

void MainWeapon_Bow :: PlayAnim_ChangeRun(bool Flag_3PV)
{
	if(!OnChangingFlag)
	{
		OnChangingFlag = true;
		m_pWeaponBase->SetCurrentAttachLeftHand();

		if(Flag_3PV)
		{
			WF::PlayWeaponAnim(m_pWeaponBase, m_aii_Change_3PV);
		}
		else
		{
			WF::PlayWeaponAnim(m_pWeaponBase, m_aii_Change_1PV);
		}
	}
}

void MainWeapon_Bow :: PlayAnim_ChangeEnd()
{
	OnChangingFlag = false;
}

void MainWeapon_Bow :: PlayAnim_ExtActivate3PV_Run()
{
	if(!OnActivateFlag)
	{
		OnActivateFlag = true;
		m_pWeaponBase->SetCurrentAttachLeftHand();

		WF::PlayWeaponAnim(m_pWeaponBase, m_aii_ExtensionActivate_3PV);
	}
}
void MainWeapon_Bow :: PlayAnim_ExtActivate3PV_End()
{
	OnActivateFlag = false;
}

void MainWeapon_Bow :: PlayAnim_ExtDeActivate3PV_Run()
{
	if(!OnDeActivateFlag)
	{
		OnDeActivateFlag = true;
		m_pWeaponBase->SetCurrentAttachLeftHand();

		WF::PlayWeaponAnim(m_pWeaponBase, m_aii_ExtensionDeactivate_3PV);
	}
}
void MainWeapon_Bow :: PlayAnim_ExtDeActivate3PV_End()
{
	OnDeActivateFlag = false;
}
