#include "pch.h"
#include "GameDinoAnimContext.h"
#include "../GameCharaActionContext.h"

#include "GameCharaAnimControl.h"

I3_CLASS_INSTANCE( CGameDinoAnimContext);

INT32 CGameDinoAnimContext::getDinoAIUpperID( ID_UPPER_DINO_AI ai)
{
	I3ASSERT( ai < ID_UPPER_DINO_AI_MAX);
	return static_cast<INT32>(m_nDinoAIUpperId[ m_pOwner->getViewType()][ ai]);
}

INT32 CGameDinoAnimContext::getDinoAILowerID( ID_LOWER_DINO_AI ai)
{
	I3ASSERT( ai < ID_LOWER_DINO_AI_MAX);
	return static_cast<INT32>(m_nDinoAILowerId[ getActionContext()->getBodyLower()][ ai]);
}

ID_LOWER_DINO_AI CGameDinoAnimContext::getMoveIdByDirForDino( VEC3D * vDir)
{
	// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)	// 0.f)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_MOVE_FRONT_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_MOVE_FRONT_RIGHT;	//pszAIName = "FrontRight";
		else											return ID_LOWER_DINO_MOVE_FRONT;	//pszAIName = "Front";
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_MOVE_BACK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_MOVE_BACK_RIGHT;	//pszAIName = "BackRight";
		else											return ID_LOWER_DINO_MOVE_BACK;	//pszAIName = "Back";
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_MOVE_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_MOVE_RIGHT;	//pszAIName = "Right";
	}

	//return pszAIName;
	return ID_LOWER_DINO_IDLE;
}

void CGameDinoAnimContext::BindBoneList( void)
{
	UINT32 i;
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	if( m_pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;
	
	if( i3::kind_of<CGameCharaDinoRaptor*>((CGameCharaBase*)(m_pOwner)) ) //렙터 본세팅
	{
		for( i = 0; i < CHARA_LOWER_DINO_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerDinoBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_RAPTOR_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperDinoBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( i3::kind_of<CGameCharaDinoSting*>((CGameCharaBase*)(m_pOwner))) //스팅 본세팅
	{
		for( i = 0; i < CHARA_LOWER_DINO_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerStingBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_DINO_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperStingBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( i3::kind_of<CGameCharaDinoAcid*>((CGameCharaBase*)(m_pOwner))) //에시드 본세팅
	{
		for( i = 0; i < CHARA_LOWER_DINO_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerAcidBoneName[i]));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_DINO_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperAcidBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( m_pOwner->IsElite()) //엘리트 본세팅
	{
		for( i = 0; i < CHARA_LOWER_DINO_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerEliteBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_ELITE_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperEliteBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( m_pOwner->IsTank()) //탱크 본세팅
	{
		for( i = 0; i < CHARA_LOWER_DINO_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerTankBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_DINO_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperTankBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( m_pOwner->IsRex()) //티라노 본세팅
	{
		for( i = 0; i < CHARA_LOWER_TREX_BONE_COUNT; i++ ) 	
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerTRexBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_TREX_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperTRexBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
	else if( m_pOwner->IsDomiMutantRex()) //뮤턴트렉스 본세팅	
	{
		for( i = 0; i < CHARA_LOWER_TREX_BONE_COUNT; i++ )
		{
			m_LowerBoneList[i] = static_cast<INT8>(pBody->FindBoneByName( g_szLowerTRexBoneName[i] ));
			I3ASSERT( m_LowerBoneList[ i ] != -1 );
		}

		for( i = 0; i < CHARA_UPPER_MUTANT_TREX_BONE_COUNT; i++)
		{
			m_UpperBoneList[ i] = static_cast<INT8>(pBody->FindBoneByName( g_szUpperMutantTRexBoneName[ i]));
			I3ASSERT( m_UpperBoneList[ i] != -1);
		}
	}
}

ID_LOWER_DINO_AI CGameDinoAnimContext::getWalkIdByDirForDino( VEC3D * vDir)
{
	I3ASSERT( m_pOwner->IsDino());

	// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_WALK_FRONT_LEFT;
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_WALK_FRONT_RIGHT;
		else											return ID_LOWER_DINO_WALK_FRONT;
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_WALK_BACK_LEFT;
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_WALK_BACK_RIGHT;
		else											return ID_LOWER_DINO_WALK_BACK;
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_DINO_WALK_LEFT;
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_DINO_WALK_RIGHT;
	}

	return ID_LOWER_DINO_IDLE;
}

bool CGameDinoAnimContext::_Play_Anim_DinoUpper( ID_UPPER_DINO_AI aiID,
												 REAL32 startTime /*= 0.f*/,
												 REAL32 scale /*= 1.f*/)
{
	I3ASSERT( m_pOwner->IsDino() );
	return m_pCurrentAnimControl->PlayDinoUpper( aiID, startTime, scale) == true;
}

bool CGameDinoAnimContext::_Play_AnimDino_Lower( ID_LOWER_DINO_AI aiID,
												 REAL32 startTime /*= 0.f*/,
												 REAL32 scale /*= 1.f*/)
{
	I3ASSERT( m_pOwner->IsDino() );
	return m_pCurrentAnimControl->PlayDinoLower( aiID, startTime, scale) == true;
}
