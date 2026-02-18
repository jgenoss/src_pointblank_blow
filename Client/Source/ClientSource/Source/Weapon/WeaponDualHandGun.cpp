#include "pch.h"
#include "WeaponDualHandGun.h"
#include "GameCharaBase.h"

#include "MainWeapon_DualHandGun.h"

I3_CLASS_INSTANCE( WeaponDualHandGun);//, WeaponDual);

WeaponDualHandGun::WeaponDualHandGun()
{

}

WeaponDualHandGun::~WeaponDualHandGun()
{
}

void WeaponDualHandGun::Create(CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponDual::Create(pOwner, Itemid, bCreateInWeapon);

	m_SilencerMaterialList_Right.clear();
	m_SilencerMaterialList_Left.clear();

	// 각 LOD에서 해당 Material을 찾습니다. 장착되는 Extension에 대해 Visible을 판별하기 위해서입니다.
	i3Body * pBody = getSceneObject()->GetBody();
	I3ASSERT(pBody != nullptr);

	for (UINT32 i = 0; i < pBody->getLODCount(); i++)
	{
		i3LOD * pLOD = pBody->getLOD(i);

		i3Scene::FindNodeByName(pLOD->getShapeNode(), "Silencer_Material", m_SilencerMaterialList_Right);
		i3Scene::FindNodeByName(pLOD->getShapeNode(), "Model_Silencer_Material", m_SilencerMaterialList_Right);
	}

	pBody = m_pLeftHandWeapon->getSceneObject()->GetBody();
	I3ASSERT(pBody != nullptr);

	for (UINT32 i = 0; i < pBody->getLODCount(); i++)
	{
		i3LOD * pLOD = pBody->getLOD(i);

		i3Scene::FindNodeByName(pLOD->getShapeNode(), "Silencer_Material", m_SilencerMaterialList_Left);
		i3Scene::FindNodeByName(pLOD->getShapeNode(), "Model_Silencer_Material", m_SilencerMaterialList_Left);
	}

	if (m_SilencerMaterialList_Right.size() > 0 || m_SilencerMaterialList_Left.size() > 0)
	{
		OnSetVisibleExtension(false);
	}
}

MainWeapon* WeaponDualHandGun::CreateMainWeapon()
{
	MainWeapon_DualHandGun * p = new MainWeapon_DualHandGun(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualHandGun));
	return p;
}

/*virtual*/bool WeaponDualHandGun::Fire( WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualHandGun* mainWeapon = static_cast<MainWeapon_DualHandGun*>(GetMainWeapon());
	return mainWeapon->Fire_DualHandGun(type);
}


/*virtual*/void WeaponDualHandGun::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualHandGun* mainWeapon = static_cast<MainWeapon_DualHandGun*>(GetMainWeapon());
	mainWeapon->NET_Fire_DualHandGun(pStartPos, pTargetPos, type);
}

/*virtual*/void	WeaponDualHandGun::OnPlayLoadMagAnimation( void)
{
	MainWeapon_DualHandGun* mainWeapon = static_cast<MainWeapon_DualHandGun*>(GetMainWeapon());
	mainWeapon->OnPlayLoadMagAnimation_DualHandGun();
}

/*virtual*/ bool WeaponDualHandGun::OnLoadMag( i3AIContext * pCtx, REAL32 tm )
{
	bool bRv = WeaponBase::OnLoadMag( pCtx, tm);

	MainWeapon_DualHandGun* mainWeapon = static_cast<MainWeapon_DualHandGun*>(GetMainWeapon());

	mainWeapon->OnLoadMag_DualHandGun(pCtx, tm, bRv);

	return bRv;
}

void WeaponDualHandGun::OnSetVisibleExtension(bool bVal)
{
	WeaponBase::OnSetVisibleExtension(bVal);

	// Extension 장비
	if (m_SilencerMaterialList_Right.empty() != true)
	{
		if (bVal)
		{
			for (size_t i = 0; i < m_SilencerMaterialList_Right.size(); i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*)m_SilencerMaterialList_Right[i];
				pAttrSet->RemoveFlag(I3_NODEFLAG_DISABLE);
			}
		}
		else
		{
			for (size_t i = 0; i < m_SilencerMaterialList_Right.size(); i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*)m_SilencerMaterialList_Right[i];
				pAttrSet->AddFlag(I3_NODEFLAG_DISABLE);
			}
		}
	}

	if (m_SilencerMaterialList_Left.empty() != true)
	{
		if (bVal)
		{
			for (size_t i = 0; i < m_SilencerMaterialList_Left.size(); i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*)m_SilencerMaterialList_Left[i];
				pAttrSet->RemoveFlag(I3_NODEFLAG_DISABLE);
			}
		}
		else
		{
			for (size_t i = 0; i < m_SilencerMaterialList_Left.size(); i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*)m_SilencerMaterialList_Left[i];
				pAttrSet->AddFlag(I3_NODEFLAG_DISABLE);
			}
		}
	}
}

void WeaponDualHandGun::_OnInitVairable(void)
{
	m_SilencerMaterialList_Right.clear();
	m_SilencerMaterialList_Left.clear();
}



