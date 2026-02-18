#include "pch.h"
#include "WeaponWithGunStock.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"


I3_CLASS_INSTANCE( WeaponWithGunStock);//, WeaponBase);

WeaponWithGunStock::WeaponWithGunStock()
{
	for( INT32 a=0; a< MAX_WEAPON_LOD; a++) 
	{
		for( INT32 b=0; b< MAX_GUN_STOCK_MODEL; b++) 
		{
			m_pGunStockModel[a][b] = nullptr;
		}
	}
	
	for( INT32 i=0; i< IDLE_MAX; i++) 
	{
		m_aiiUI_Idle_SWAT_Male[i] = -1;
		m_aiiUI_Idle_SWAT_Female[i] = -1;
		m_aiiUI_Idle_REBEL_Male[i] = -1;
		m_aiiUI_Idle_REBEL_Female[i] = -1;
	}
}

WeaponWithGunStock::~WeaponWithGunStock()
{
}

/*virtual*/ void WeaponWithGunStock::PostCreate( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			// Glock 18 의 경우 경우에 따라 안보여야하는 개머리판 모델			
			i3Body * pBody = getSceneObject()->GetBody();
			I3ASSERT( pBody != nullptr);

			for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
			{
				i3LOD * pLOD = pBody->getLOD( i );		

				m_pGunStockModel[i][0] = (i3Geometry*) i3Scene::FindNodeByName( pLOD->getShapeNode(), "Model_Stock_Front");
				m_pGunStockModel[i][1] = (i3Geometry*) i3Scene::FindNodeByName( pLOD->getShapeNode(), "Model_Stock_Back");
			}

			m_aiiUI_Idle_SWAT_Male[ IDLE_A]		= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_SWAT_Male");
			m_aiiUI_Idle_SWAT_Female[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_SWAT_Female");
			m_aiiUI_Idle_REBEL_Male[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_REBEL_Male");
			m_aiiUI_Idle_REBEL_Female[ IDLE_A]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_REBEL_Female");

			m_aiiUI_Idle_SWAT_Male[ IDLE_B]		= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_SWAT_Male");
			m_aiiUI_Idle_SWAT_Female[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_SWAT_Female");
			m_aiiUI_Idle_REBEL_Male[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_REBEL_Male");
			m_aiiUI_Idle_REBEL_Female[ IDLE_B]	= (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_REBEL_Female");
		}
		break;
	}
}


/*virtual*/ void WeaponWithGunStock::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponBase::OnUpdate( rDeltaSeconds);	

	if(g_pFramework->IsBattleStage())	
	{
		// 현재 들고 있거나 월드에 떨어뜨릴때는 개머리판이 보이도록
		if( m_pOwner && m_pOwner->GetCurrentCharaWeapon() == this || 
			getDropInfo()->_bDropping || getDropInfo()->_bDroped)
			SetVisibleGunStock( true);
		else
			SetVisibleGunStock( false);	// Glock 18 의 경우 권총집에 넣을때 너무 커서 개머리판을 안보이게 한다.
	}
	else
	{
		SetVisibleGunStock( true);
	}
}

/*virtual*/ void WeaponWithGunStock::OnSetVisibleExtension( bool bVal )
{
	//SetVisibleGunStock( bVal);
}

/*virtual*/ void WeaponWithGunStock::OnDrop( void)
{
	WeaponBase::OnDrop();

	// 안보이던 개머리판은 월드에 떨어지면 보이게 한다.
	SetVisibleGunStock( true);
}

void WeaponWithGunStock::SetVisibleGunStock( bool bVal)
{
	for( UINT32 a=0; a< MAX_WEAPON_LOD; a++ )
	{
		for( INT32 b=0; b< MAX_GUN_STOCK_MODEL; b++) 
		{	// 총 일부 모델을 보였다 안보였다 스위칭
			if( m_pGunStockModel[a][b])
			{
				if( bVal)
					m_pGunStockModel[a][b]->RemoveFlag( I3_NODEFLAG_INVISIBLE);
				else
					m_pGunStockModel[a][b]->AddFlag( I3_NODEFLAG_INVISIBLE);
			}
		}
	}
}

void WeaponWithGunStock::PlayAnim_UI_IdleA( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			// Glock 18 의 경우 로비에서 개머리판을 접은 상태로 
			if( m_aiiUI_Idle_SWAT_Male[ IDLE_A] > -1)
			{
				WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_A]);
			}
		}
		break;
	}
}

void WeaponWithGunStock::PlayAnim_UI_IdleB( void)
{
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

	I3ASSERT( m_pWeaponInfo != nullptr);

	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_HANDGUN:	
		{
			//if( m_pOwner != nullptr)
			//{
			//	// Glock 18 의 경우 경우에 캐릭터에 따라 개머리판을 접었다 폈다하는 경우
			//	switch( m_pOwner->getCharaInfo()->GetResType())
			//	{
			//	case CHARA_RES_ID_ACID_POLE:		WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_B]);		break;
			//	case CHARA_RES_ID_SWAT_FEMALE:	WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Female[ IDLE_B]);	break;
			//	}
			//}

			if( m_aiiUI_Idle_SWAT_Male[ IDLE_A] > -1)
			{
				WF::PlayWeaponAnim(this, m_aiiUI_Idle_SWAT_Male[ IDLE_A]);
			}
		}
		break;
	}
}

