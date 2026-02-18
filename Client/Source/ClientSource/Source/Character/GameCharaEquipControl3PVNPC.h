#if !defined( __GAME_CHARA_EQUIP_CONTROL_3PV_NPC_H__ )
#define __GAME_CHARA_EQUIP_CONTROL_3PV_NPC_H__

#include "GameCharaEquipControl3PV.h"

class CGameCharaEquipControl3PVNPC : public CGameCharaEquipControl3PV
{
	I3_CLASS_DEFINE( CGameCharaEquipControl3PVNPC, CGameCharaEquipControl3PV);
protected:
public:
	CGameCharaEquipControl3PVNPC();
	virtual ~CGameCharaEquipControl3PVNPC();


	virtual bool	OnCreateEquip( CHARA_RES_ID CharaType, EQUIP::ePART part, INT32 ResID)		{ return false; }
	virtual bool	OnDeleteEquip( EQUIP::ePART type)						{ return false; }

	virtual bool	OnAttach( CGameCharaEquip * pEquip)						{ return false; }
	virtual bool	OnAttach( EQUIP::ePART type, bool bReset)				{ return false; }

	virtual CGameCharaEquip *	OnDetach( EQUIP::ePART type)					{ return NULL; }

	virtual bool	OnSet( CGameCharaEquip * pEquip)						{ return false; }

	virtual CGameCharaEquip * OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos){ return NULL;}
};

#endif
