#if !defined( __GAME_CHARA_EQUIP_CONTROL_3PV_H__)
#define __GAME_CHARA_EQUIP_CONTROL_3PV_H__

#include "GameCharaEquipControl.h"

class CGameCharaEquipControl3PV : public CGameCharaEquipControl
{
	I3_CLASS_DEFINE( CGameCharaEquipControl3PV, CGameCharaEquipControl);

public:

	virtual CGameCharaEquip * OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos) override;
};

#endif
