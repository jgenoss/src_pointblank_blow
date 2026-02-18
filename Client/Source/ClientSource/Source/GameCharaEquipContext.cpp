#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaEquipContext.h"
#include "GameCharaBoneContext.h"

#include "BattleSlotContext.h"

#include "./Character/GameCharaEquipControl1PV.h"
#include "./Character/GameCharaEquipControl3PV.h"

I3_CLASS_INSTANCE( CGameCharaEquipContext);

CGameCharaEquipContext::CGameCharaEquipContext()
{
	for (INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_pDummy[i] = nullptr;
		m_pCharaEquip[i] = nullptr;
		m_pEquipInfo[i] = nullptr;
	}
}

CGameCharaEquipContext::~CGameCharaEquipContext()
{
	OnDestroy();
}

bool CGameCharaEquipContext::isAttachOnChara( EQUIP::ePART part)
{	
	if( m_pCharaEquip[ part] == nullptr)
	{
		return false;
	}

	return (m_pCharaEquip[ part] != nullptr &&
		(m_pCharaEquip[ part]->IsAttached()) &&
		(m_pCharaEquip[ part]->GetOwner() == m_pOwner));
}

REAL32 CGameCharaEquipContext::GetAbility( EQUIP::eABILITY ability)
{
	INT32 i;
	REAL32 bVal = 0.0f;

	for( i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if( m_pEquipInfo[ i] != nullptr)
			bVal += m_pEquipInfo[ i]->GetAbilityValue( ability);
	}

	return bVal;
}

REAL32	CGameCharaEquipContext::GetHeadDefenseRate()
{
	REAL32 rVal = 0.0f;

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if ( i3::kind_of<CHeadInfo*>(m_pEquipInfo[ i]))
			rVal += ((CHeadInfo *)m_pEquipInfo[ i])->getDefenseRate();
	}

	return rVal;
}

REAL32	CGameCharaEquipContext::GetHeadProtectionTime()
{
	REAL32 rVal = 0.0f;

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if ( i3::kind_of<CHeadInfo*>(m_pEquipInfo[ i]))
			rVal += ((CHeadInfo *)m_pEquipInfo[ i])->getProtectionTime();
	}

	return rVal;
}

void CGameCharaEquipContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{	
	m_pEquipControl1PV = CGameCharaEquipControl1PV::new_object();
	m_pEquipControl1PV->BindOwner( pOwner);

	m_pEquipControl3PV = CGameCharaEquipControl3PV::new_object();
	m_pEquipControl3PV->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);

	CreateEquipmentDummy();
}


void CGameCharaEquipContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_pEquipCurrentControl->OnDeleteEquip( (EQUIP::ePART) i);
		I3_SAFE_NODE_RELEASE( m_pDummy[ (EQUIP::ePART) i]);
	}

	I3_SAFE_RELEASE( m_pEquipControl1PV);
	I3_SAFE_RELEASE( m_pEquipControl3PV);
	m_pEquipCurrentControl = nullptr;
}

void CGameCharaEquipContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	// 다른 파츠에 따라 헬멧을 가린다.
	bool bVisibleHeadgear = true;
	if( ((Avatar*)(CGameCharaBase*)m_pOwner)->IsDummyResource() == false)
	{
		const CJacketInfo * pInfo = static_cast<const CJacketInfo*>(getEquipContext()->getEquipInfo( EQUIP::ePART_VEST));
		if( pInfo != nullptr && pInfo->getHeadgearShow( m_pOwner->getTeam() ) == false)
		{	// Headgear의 Visible을 꺼야한다.
			bVisibleHeadgear = false;
		}
	}

	bool bVisibleFacegear = true;
	if( getEquip( EQUIP::ePART_HEADGEAR) != nullptr)
	{
		const CHeadInfo * pHeadInfo = static_cast<const CHeadInfo*>(getEquip( EQUIP::ePART_HEADGEAR)->GetInfo());
		if( pHeadInfo != nullptr && pHeadInfo->getFacegearShow( m_pOwner->getTeam()) == false)
		{	// Facegear의 Visible을 끈다.
			bVisibleFacegear = false;
		}
	}

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_pEquipCurrentControl->OnAttach( (EQUIP::ePART) i, true);
		SetBlendAlpha( (EQUIP::ePART) i, 1.f, true);

		if( i == EQUIP::ePART_HEADGEAR && !bVisibleHeadgear)
		{
			if( getEquip( EQUIP::ePART_HEADGEAR) != nullptr)
				getEquip( EQUIP::ePART_HEADGEAR)->SetVisibleOnlyScene( false);
		}
		else if( i == EQUIP::ePART_FACEGEAR && !bVisibleFacegear)
		{
			if( getEquip( EQUIP::ePART_FACEGEAR) != nullptr)
				getEquip( EQUIP::ePART_FACEGEAR)->SetVisibleOnlyScene( false);
		}
	}

	m_bHelmetProtect = false;
}

void CGameCharaEquipContext::OnChange1PV()
{
	m_pEquipCurrentControl = m_pEquipControl1PV;

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		CGameCharaEquip * pEquip = getEquip( (EQUIP::ePART) i);
		if( pEquip != nullptr )
			pEquip->SetVisible( false);

		if( m_pDummy[ (EQUIP::ePART) i] != nullptr)
			m_pDummy[ (EQUIP::ePART) i]->DeactivateNode( true);
	}
	
}

void CGameCharaEquipContext::OnChange3PV()
{
	m_pEquipCurrentControl = m_pEquipControl3PV;

	// 다른 파츠에 따라 헬멧을 가린다.
	bool bVisibleHeadgear = true;
	bool bVisibleFacegear = true;

	if( ((Avatar*)(CGameCharaBase*)m_pOwner)->IsSkinResource())
	{
		bVisibleHeadgear = false;
		bVisibleFacegear = false;
	}
	else
	{
		if( ((Avatar*)(CGameCharaBase*)m_pOwner)->IsDummyResource() == false)
		{
			const CJacketInfo * pInfo = static_cast<const CJacketInfo*>(getEquipContext()->getEquipInfo( EQUIP::ePART_VEST));
			if( pInfo != nullptr && pInfo->getHeadgearShow( m_pOwner->getTeam() ) == false)
			{	// Headgear의 Visible을 꺼야한다.
				bVisibleHeadgear = false;
			}
		}
		
		if( getEquip( EQUIP::ePART_HEADGEAR) != nullptr)
		{
			const CHeadInfo * pHeadInfo = static_cast<const CHeadInfo*>(getEquip( EQUIP::ePART_HEADGEAR)->GetInfo());
			if( pHeadInfo != nullptr && pHeadInfo->getFacegearShow( m_pOwner->getTeam()) == false)
			{	// Facegear의 Visible을 끈다.
				bVisibleFacegear = false;
			}
		}
	}

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		CGameCharaEquip * pEquip = getEquip( (EQUIP::ePART) i);
		if( pEquip != nullptr )
		{
			if( (i == EQUIP::ePART_HEADGEAR && !bVisibleHeadgear) ||
				( i == EQUIP::ePART_FACEGEAR && !bVisibleFacegear) )
				pEquip->SetVisible( false);
			else
				pEquip->SetVisible( true);

			if( m_pDummy[ (EQUIP::ePART) i] != nullptr)
				m_pDummy[ (EQUIP::ePART) i]->ActivateNode( true);
		}
	}
}

void CGameCharaEquipContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();
}

void CGameCharaEquipContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();
}

void CGameCharaEquipContext::OnChangeNPC( void)
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaEquipContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);
}

void CGameCharaEquipContext::DropToWorld( EQUIP::ePART type, VEC3D * pVecDir)
{
	I3ASSERT( pVecDir != nullptr);

	VEC3D vDir;
	
	CGameCharaEquip * pEquip = getEquip( type );
	if( pEquip == nullptr )
		return;

	// 헬멧을 날리는 힘
	i3Vector::Scale( &vDir, pVecDir, 3.f );
	i3Vector::Add( &vDir, 0.f, 3.5f, 0.f );
	
	i3Scene::SetDynamicVelocity( pEquip->getSceneObject(), nullptr, &vDir, &vDir );

	if( m_pOwner->Cmd_DropEquip( m_pOwner->getProtectPartsType(), i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()) ) )
	{
		pEquip->SetVisible( true);
		pEquip->setDynamicActiveState( true, false);
		pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
	}

	m_rTargetChaseC5 = 0.f;

	return;
}

void CGameCharaEquipContext::SetMaterialColor( EQUIP::ePART part, COLORREAL * pDiff, COLORREAL * pSpec, bool bOnChara)
{
	if( bOnChara == true && !isAttachOnChara( part))
		return;

	CGameCharaEquip * pEquip = getEquip( part);

	if( pEquip == nullptr)
		return;

	pEquip->SetMaterialColor( pDiff, pSpec);
}

void CGameCharaEquipContext::SetBlendAlpha( EQUIP::ePART type, REAL32 fAlpha, bool bOnChara)
{
	if( fAlpha >= 0.f)
	{
		if( bOnChara == true && isAttachOnChara( type) == false)	
			return;

		CGameCharaEquip * pEquip = getEquip( type);

		if( pEquip == nullptr)
			return;

		// revision 29616
		//GM 옵저버 모드 일 경우 헬멧이 안보이도록 함.
		if( BattleSlotContext::i()->IsGMObserverForClient( m_pOwner->getCharaNetIndex()))
			fAlpha = 0.f;

		pEquip->SetBlendAlpha( fAlpha);
	}
}

void CGameCharaEquipContext::ReleaseEquip( EQUIP::ePART type)
{
	if( getEquip( type) != nullptr)
	{	
		m_pEquipCurrentControl->OnDetach( type);	//	캐릭터에서 떼어낸다

		//	메모리 제거
		//I3_SAFE_RELEASE( m_pCharaEquip[ type]);
		if( m_pCharaEquip[ type] != nullptr)
		{
			m_pCharaEquip[ type]->ReturnInstance();
			m_pCharaEquip[ type] = nullptr;
		}
	}
}

void CGameCharaEquipContext::CreateEquip( CHARA_RES_ID charaResType, T_ItemID ItemID, INT32 res_subtype)
{
	if( ItemID == 0)
		return;

	EQUIP::ePART part = EQUIP::ItemID2PartsType(ItemID);
	
	const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(ItemID);
	
	if(pInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL, "Create Equip 실행 중 Info가 없습니다. ItemID : %d", ItemID);
		I3ASSERT_RETURN(pInfo != nullptr);
	}

	// 베레모 파츠 예외처리.
	if( part == EQUIP::ePART_BERET )
		part = EQUIP::ePART_HEADGEAR;

	// 목표추적헬멧 체크
	if( part == EQUIP::ePART_HEADGEAR)
		m_rTargetChaseC5 = pInfo->GetAbilityValue( EQUIP::eABILITY_DETECTC5);

	if( ! m_pEquipCurrentControl->OnCreateEquip( charaResType, ItemID, m_pOwner->getTeam(), res_subtype))
		return;

	if (part == EQUIP::ePART_UNKNOWN)
		return;
	
	CGameCharaEquip * pEquip = getEquip( part);

	if( pEquip != nullptr)
	{
		if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED )
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
		}
		else
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );
		}
	}
}


bool CGameCharaEquipContext::DropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos)
{
	I3ASSERT( m_pEquipCurrentControl != nullptr);
	if( m_pEquipCurrentControl->OnDropEquip( type, pSceneNode, pGameNode, pVecPos) != nullptr)
		return true;

	return false;
}

bool CGameCharaEquipContext::Attach( EQUIP::ePART type, bool bReset)
{
	return m_pEquipCurrentControl->OnAttach( type, bReset);
}

bool CGameCharaEquipContext::Detach( EQUIP::ePART type)
{
	if( m_pEquipCurrentControl->OnDetach( type))
	{
		if( getEquip( type ) != nullptr)
		{
			// Ageia 문제. 해당 액터의 컨텍트 상태가 없는 상태에서 Kinematic으로 변경이 이루어져야 제대로 동작합니다. 따라서 위치를 띄워줍니다.
			VEC3D vec;
			i3Vector::Set( &vec, (REAL32)(m_pOwner->getCharaNetIndex()), g_pConfigEx->GetPhysix().HeightEquip, (REAL32)(m_pOwner->getCharaNetIndex()));
			getEquip( type )->SetPos( &vec );
			getEquip( type )->Reset();
		}

		return true;
	}

	return false;
}

void CGameCharaEquipContext::__SetDummyBoneRef( const char* szBoneName, i3BoneRef* destBoneRef)
{
	i3Body * pBody = m_pOwner->getSceneObject()->GetBody();
	i3LOD* pLOD = pBody->getFrameLOD();

	INT32 idx = pLOD->getSkeleton()->FindBoneByName(szBoneName);

	if (destBoneRef)
	{
		destBoneRef->setBoneIndex(idx);
#if defined( I3_DEBUG)
		destBoneRef->SetName(szBoneName);
#endif

		pBody->AddChild(destBoneRef);
	}
}

void CGameCharaEquipContext::CreateEquipmentDummy( void)
{
	setDummy( EQUIP::ePART_HEADGEAR, i3BoneRef::new_object_ref());
	setDummy( EQUIP::ePART_FACEGEAR, i3BoneRef::new_object_ref());
	setDummy( EQUIP::ePART_HOLSTER, i3BoneRef::new_object_ref());
	setDummy( EQUIP::ePART_BELT, i3BoneRef::new_object_ref());
	setDummy( EQUIP::ePART_BERET, getDummy( EQUIP::ePART_HEADGEAR) );

	__SetDummyBoneRef("Bone_HeadGearAttach", getDummy( EQUIP::ePART_HEADGEAR) );
	__SetDummyBoneRef("Bone_FaceGearAttach", getDummy( EQUIP::ePART_FACEGEAR) );
	__SetDummyBoneRef("Bone_HolsterAttach", getDummy( EQUIP::ePART_HOLSTER) );
	__SetDummyBoneRef("Bone_BeltAttach", getDummy( EQUIP::ePART_BELT) );
	//__SetDummyBoneRef("Bone_HeadGearAttach", getDummy( EQUIP::ePART_BERET) );
}



