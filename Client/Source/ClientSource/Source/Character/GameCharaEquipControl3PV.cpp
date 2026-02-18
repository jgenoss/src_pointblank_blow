#include "pch.h"
#include "GameCharaEquipControl3PV.h"
#include "GameCharaEquip.h"

I3_CLASS_INSTANCE( CGameCharaEquipControl3PV);

CGameCharaEquip * CGameCharaEquipControl3PV::OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos)
{
	CGameCharaEquip * pCharaEquip = CGameCharaEquipControl::OnDropEquip( type, pSceneNode, pGameNode, pVecPos);
	
	//	Å»Âø À§Ä¡ ÁöÁ¤
	if( pCharaEquip != nullptr && pVecPos != nullptr )
	{
		pCharaEquip->SetPos( pVecPos );
	}
	
	return pCharaEquip;
}

