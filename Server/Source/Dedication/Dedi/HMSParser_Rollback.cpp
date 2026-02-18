#include "pch.h"
#include "HMSParser.h"
#include "DediRoom.h"
#include "DroppedWeaponMgr.h"
#include "WeaponTable.h"


//hms parser에 의해 변경되는 변수 보관
//_RollBack()과 변수 일치해야 함
void HMSParser::_SaveData( DS_PACKET_ROLLBACK* pRollback )
{
	I3ASSERT( pRollback );

	CDediRoom* pRoom = pRollback->m_pRoom;
	I3ASSERT( pRoom != NULL );
	
	UINT32 ui32InfoFlag = pRollback->m_ui32InfoFlag;
	//I3ASSERT( ui32InfoFlag != 0 );

	UINT32 ui32SlotIdx = pRollback->m_ui32SlotIdx;
	CCharacter*	pChara = pRoom->GetGameCharacter(ui32SlotIdx);
	I3ASSERT( pChara != NULL );
	if( !pChara ) return;

	if( ui32InfoFlag & P2P_INFO_FLAG_CHARASTATE )
	{
		pChara->GetState()->CopyTo( &pRollback->m_state );
	}

	if( ui32InfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		pRollback->m_bIsOnLoad				= pChara->GetOnLoad();
	}

	if( ui32InfoFlag & P2P_INFO_FLAG_CHARAPOS)
	{
		pChara->GetPosRot()->CopyTo( &pRollback->m_pos );
		pRollback->m_r32PosRecvTime			= pChara->GetPosRecvTime();
		pRollback->m_ui32FlagChkRespawnPos  = pRoom->m_ui32FlagChkRespawnPos; 
	}
	
	if( ui32InfoFlag & P2P_INFO_FLAG_ACTIONKEY ) 
	{
		pRollback->m_bUseM197				 = pChara->IsUseM197();
		pRollback->m_ui32PrimaryWeaponID	 = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_PRIMARY );
	}
	
	if( ui32InfoFlag & P2P_INFO_FLAG_SYNCWEAPON )
	{
		pRollback->m_ui32PrimaryWeaponID     = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_PRIMARY );
		pRollback->m_ui32SecondaryWeaponID   = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_SECONDARY );
	}

	if( ui32InfoFlag & P2P_INFO_FLAG_MISSION)
	{
		pRollback->m_r32TriggerTime			 = pRoom->m_r32TriggerTime[ui32SlotIdx];
	}

	if( ui32InfoFlag & P2P_INFO_FLAG_GETWEAPON)
	{
		pRollback->m_ui32PrimaryWeaponID     = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_PRIMARY );
		pRollback->m_ui32SecondaryWeaponID   = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_SECONDARY );
	}

	if( ui32InfoFlag & P2P_INFO_FLAG_DROPWEAPON)
	{
		pRollback->m_ui32PrimaryWeaponID	 = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_PRIMARY );
		pRollback->m_ui32SecondaryWeaponID   = pRoom->GetServerWeaponID( ui32SlotIdx, WEAPON_SLOT_SECONDARY );
	}
	
	if( ui32InfoFlag & P2P_INFO_FLAG_FIRE)
	{
		pChara->GetState()->CopyTo ( &pRollback->m_state );
		pRollback->m_r32LastFireTime		= pChara->GetLastFireTime();
		pRollback->m_TLastFireItemID		= pChara->GetLastFireItemID();
		pRollback->m_eLastFireAttackType	= pChara->GetLastFireAttackType();
		pRollback->m_ui32FireCount			= pChara->GetFireCount();
		pRollback->m_r32FireCheckTime		= pChara->GetFireCheckTime();
	}
}


void HMSParser::_RollBack ( DS_PACKET_ROLLBACK* pRollback )
{
	I3ASSERT( pRollback != NULL );
	CDediRoom*	 pRoom = pRollback->m_pRoom;
	I3ASSERT( pRoom != NULL );
	CCharacter*	pChara = pRoom->GetGameCharacter( pRollback->m_ui32SlotIdx );
	I3ASSERT( pChara != NULL );
	
	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		pChara->SetState( &pRollback->m_state );
	}

	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		pChara->SetOnLoad( pRollback->m_bIsOnLoad );
	}

	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_CHARAPOS)
	{
		pChara->SetPosRot( &pRollback->m_pos );
		pChara->SetPosRecvTime( pRollback->m_r32PosRecvTime );
		pRoom->m_ui32FlagChkRespawnPos = pRollback->m_ui32FlagChkRespawnPos; 
	}
	
	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_ACTIONKEY ) 
	{
		pChara->SetUseM197( pRollback->m_bUseM197 );
		UINT32 ui32PrimaryWeaponID = pRollback->m_ui32PrimaryWeaponID;
		pChara->SetEquipWeapon( ui32PrimaryWeaponID, WEAPON_SLOT_PRIMARY );
	}
	
	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_SYNCWEAPON )
	{
		UINT32 ui32PrimaryWeaponID = pRollback->m_ui32PrimaryWeaponID;
		pChara->SetEquipWeapon( ui32PrimaryWeaponID, WEAPON_SLOT_PRIMARY );
	}

	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_MISSION)
	{
		pRoom->m_r32TriggerTime[ pRollback->m_ui32SlotIdx] = pRollback->m_r32TriggerTime;	
	}

	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_GETWEAPON)
	{		
		UINT32 ui32WeaponID = pRollback->m_ui32PrimaryWeaponID;
		pChara->SetEquipWeapon( ui32WeaponID, WEAPON_SLOT_PRIMARY );

		ui32WeaponID = pRollback->m_ui32SecondaryWeaponID;
		pChara->SetEquipWeapon( ui32WeaponID, WEAPON_SLOT_SECONDARY );
	}

	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_DROPWEAPON)
	{
		UINT32 ui32WeaponID = pRollback->m_ui32PrimaryWeaponID;
		pChara->SetEquipWeapon( ui32WeaponID, WEAPON_SLOT_PRIMARY );

		ui32WeaponID = pRollback->m_ui32SecondaryWeaponID;
		pChara->SetEquipWeapon( ui32WeaponID, WEAPON_SLOT_SECONDARY );
	}
	
	if( pRollback->m_ui32InfoFlag & P2P_INFO_FLAG_FIRE)
	{
		pChara->SetState ( &pRollback->m_state );
		pChara->SetLastFireInfo( pRollback->m_r32LastFireTime, pRollback->m_TLastFireItemID, pRollback->m_eLastFireAttackType );
		pChara->SetFireCount( pRollback->m_ui32FireCount );
		pChara->SetFireCheckTime ( pRollback->m_r32FireCheckTime );
	}
}