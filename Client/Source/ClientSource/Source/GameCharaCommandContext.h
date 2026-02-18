#if !defined( __GAME_CHARA_COMMAND_CONTEXT_H__)
#define __GAME_CHARA_COMMAND_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "./Character/GameCharaAnimControlDefine.h"

typedef struct __tagCommandParameter
{	
	INT32				_nUserIdx;
	INT32				_nSlotIdx;
	BOOL				_bVal;
	VEC3D *				_pVec;
	CHARA_DEATH_TYPE	_DeathType;
	CHARA_ANIMSET_TYPE	_AnimSetType;
	EQUIP_HEAD_TYPE		_EquipSubType;
	HEAD_RES_ID			_EquipID;
	ITEM_RES_ID			_EquipItemID;
	CHARARES_TYPE		_CharaResType;
	EQUIP_CLASS_TYPE	_EquipType;
	i3Node *			_pSceneNode;
	i3GameNode *		_pOwner;
	WeaponBase *		_pWeapon;
	INT32				_WeaponSlotNum;
	WEAPON_CLASS_TYPE	_WeaponType;
	INT32				_WeaponNumber;
	VEC3D *				_pVecTarget;
	WEAPON_FIRE_ORDER	_FireOrder;
	CHARA_HIT_PART		_HitPart;
	CHARA_DAMAGEINFO *	_pDamageInfo;

	void Clear(void)
	{
		_nUserIdx		= -1;
		_nSlotIdx		= -1;
		_bVal			= FALSE;
		_pVec			= NULL;
		_DeathType		= CHARA_DEATH_UN;
		_AnimSetType	= CHARA_ANIMSET_3PV;
		_EquipSubType	= EQUIP_HEAD_TYPE_UNKNOWN;
		_EquipID		= HEAD_RES_ID_UNKNOWN;
		_EquipItemID	= ITEM_RES_ID_UNKNOWN;
		_CharaResType	= CHARARES_UNKNOWN;
		_EquipType		= EQUIP_CLASS_TYPE_UNKNOWN;
		_pSceneNode		= NULL;
		_pOwner			= NULL;
		_pWeapon		= NULL;
		_WeaponSlotNum	= -1;
		_WeaponType		= WEAPON_CLASS_UNKNOWN;
		_WeaponNumber	= -1;
		_pVecTarget		= NULL;
		_FireOrder		= WEAPON_PRIMARY_FIRE;
		_HitPart		= CHARA_HIT_UNKNOWN;
		_pDamageInfo	= NULL;
	}

	void	SetCreateWeapon( INT32 nSlot, WEAPON_CLASS_TYPE nType, INT32 nNumber)
	{
		_WeaponSlotNum	= nSlot;
		_WeaponType		= nType;
		_WeaponNumber	= nNumber;
	}

	void	SetCreateEquip( CHARARES_TYPE charaResType, EQUIP_HEAD_TYPE EquipType, HEAD_RES_ID EquipID, ITEM_RES_ID EquipItemID)
	{
		_CharaResType	= charaResType;
		_EquipSubType	= EquipType;
		_EquipID		= EquipID;
		_EquipItemID	= EquipItemID;

	}

	void	SetDamageParam( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pVecDir, CHARA_HIT_PART hitPart, WEAPON_CLASS_TYPE weaponType )
	{
		_pDamageInfo	= pDamageInfo;
		_pVec			= pVecDir;
		_HitPart		= hitPart;
		_WeaponType		= weaponType;
	}

	void	SetDeathParam( INT32 nShooterIdx, CHARA_DEATH_TYPE deathType, VEC3D * pHitDir)
	{
		_nUserIdx	= nShooterIdx;
		_DeathType	= deathType;
		_pVec		= pHitDir;
	}

	void	SetTakeWeapon( WeaponBase * pWeapon)
	{
		_pWeapon		= pWeapon;
	}

	void	SetDropWeaponParam( INT32 nWeaponSlot, VEC3D * pDir, BOOL bNextWeapon)
	{
		_nSlotIdx		= nWeaponSlot;
		_pVec			= pDir;
		_bVal			= bNextWeapon;
	}

	void	SetWeaponSelect( INT32 nWeaponSlot)
	{
		_nSlotIdx	= nWeaponSlot;
	}

	void	SetDropEquipParam( EQUIP_CLASS_TYPE equipType, i3Node * pAttachSceneNode, i3GameNode * pAttachGameNode, VEC3D * pPos)
	{
		_EquipType		= equipType;
		_pSceneNode		= pAttachSceneNode;
		_pOwner			= pAttachGameNode;
		_pVec			= pPos;
	}

	void	SetFireParam( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON_FIRE_ORDER fireOrder)
	{
		_pVec		= pStartPos;
		_pVecTarget = pTargetPos;
		_FireOrder	= fireOrder;
	}

	void	SetValidWeapon( WEAPON_CLASS_TYPE classType, INT32 number)
	{
		_WeaponType		= classType;
		_WeaponNumber	= number;
	}

	void	CopyTo( __tagCommandParameter * pDest)
	{
		pDest->_nUserIdx		= _nUserIdx;
		pDest->_nSlotIdx		= _nSlotIdx;
		pDest->_bVal			= _bVal;
		pDest->_pVec			= _pVec;
		pDest->_DeathType		= _DeathType;
		pDest->_AnimSetType		= _AnimSetType;
		pDest->_EquipSubType	= _EquipSubType;
		pDest->_EquipID			= _EquipID;
		pDest->_EquipItemID		= _EquipItemID;
		pDest->_CharaResType	= _CharaResType;
		pDest->_pSceneNode		= _pSceneNode;
		pDest->_pOwner			= _pOwner;
		pDest->_pWeapon			= _pWeapon;
		pDest->_WeaponSlotNum	= _WeaponSlotNum;
		pDest->_WeaponType		= _WeaponType;
		pDest->_WeaponNumber	= _WeaponNumber;
		pDest->_pVecTarget		= _pVecTarget;
		pDest->_HitPart			= _HitPart;
		pDest->_pDamageInfo		= _pDamageInfo;
	}

} CommandParameter;


class CGameCharaCommandContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaCommandContext);
protected:
	CommandParameter	m_Param;

public:
	CommandParameter *	getParameter( void)					{ return &m_Param; }

public:
	CGameCharaCommandContext();
	virtual ~CGameCharaCommandContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0);
	virtual void	OnDestroy( void);

	virtual void	OnReset( void);

	// change view
	virtual void	OnChange1PV( void);
	virtual void	OnChange3PV( void);

	virtual void	OnChangeLocal( void);
	virtual void	OnChangeNetwork( void);
	
	virtual void	OnUpdate( REAL32 rDeltatime);

	BOOL			Command( CHARA_COMMAND cmd);

	BOOL			CmdCharaInvincible(BOOL bVal);			// 무적 타임 세팅
	BOOL			CmdToggleExtensionWeapon(void);			// weapon extension on/off
	BOOL			CmdToggleZoom(void);					// toggle weapon zoom
	BOOL			CmdReturnToReady(BOOL bWithCurrentWeapon);
	BOOL			CmdTurnBack(void);

	BOOL			CmdDeathChara( void);
	BOOL			CmdExtensionActive( void);
	BOOL			CmdExtensionDeactive( void);
	BOOL			CmdWeaponChange( void);
	BOOL			CmdDamageChara( void);
	BOOL			CmdRespawnChara( void);
	BOOL			CmdFallDamageChara( void);
	BOOL			CmdPrimaryAttack( void);

	// local에서만 호출되어야 합니다.
	BOOL			CmdActiveMission(WeaponBase * pWeapon);
	BOOL			CmdActiveDisarm(void);
	BOOL			CmdCancelMission(void);
	BOOL			CmdCancelDisarm(void);

	// network 호출입니다. GameContext에서 사용되는 함수들
	void			CmdFireBulletForNetwork( void);
};

#endif
