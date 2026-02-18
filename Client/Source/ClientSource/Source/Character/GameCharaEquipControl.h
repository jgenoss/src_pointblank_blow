/**
 * \file	Character\GameCharaEquipControl.h
 *
 * Declares the game chara equip control class.
 */


#if !defined( __GAME_CHARA_EQUIP_CONTROL_H__ )
#define __GAME_CHARA_EQUIP_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaEquipDef.h"

class CGameCharaEquip;

class CGameCharaEquipControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaEquipControl, CGameCharaControlBase );
protected:
	CGameCharaEquip *	getEquip( EQUIP::ePART type)							{ return getEquipContext()->getEquip( type); }
	void			setEquip( EQUIP::ePART type, CGameCharaEquip * pEquip)	{ getEquipContext()->setEquip( type, pEquip); }

public:

	/** \brief 장비를 생성합니다. */
	virtual bool	OnCreateEquip( CHARA_RES_ID CharaType, T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype);

	/** \brief 장비를 제거합니다. */
	virtual bool	OnDeleteEquip( EQUIP::ePART type);

	/** \brief 장비를 캐릭터에 붙입니다. */
	virtual bool	OnAttach( CGameCharaEquip * pEquip);

	/** \brief 장비를 캐릭터에 붙입니다. */
	virtual bool	OnAttach( EQUIP::ePART type, bool bReset);

	/** \brief 장비를 띄어냅니다. */
	virtual CGameCharaEquip *	OnDetach( EQUIP::ePART type);

	/** \brief 이전 장비를 제거하고 새 장비로 설정합니다 
		\note lobby에서 사용합니다. */
	virtual bool	OnSet( CGameCharaEquip * pEquip);

	/** \brief 장비를 버립니다. */
	virtual CGameCharaEquip * OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos);
	
};

#endif
