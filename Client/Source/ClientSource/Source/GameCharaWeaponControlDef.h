#if !defined( __GAME_CHARA_WEAPON_CONTROLDEF_H__)
#define __GAME_CHARA_WEAPON_CONTROLDEF_H__

enum CHARA_WEAPON_ATTACH_HAND
{
	CHARA_WEAPON_GRIP_RIGHT_HAND = 0,	// ¿À¸¥¼Õ ±×¸³
	CHARA_WEAPON_BARREL_LEFT_HAND,		// ¿Þ¼Õ ÃÑ¿­
	CHARA_WEAPON_GRIP_LEFT_HAND,		// ¿Þ¼Õ ±×¸³
	CHARA_WEAPON_BARREL_RIGHT_HAND,		// ¿À¸¥¼Õ ÃÑ¿­
};

enum WEAPON_COMMAND
{
	WEAPON_CMD_CREATE = 0,
	WEAPON_CMD_DELETE,
	WEAPON_CMD_ATTACH,
	WEAPON_CMD_DETACH,
	WEAPON_CMD_RESET,
	WEAPON_CMD_RETURNTOREADY,
	WEAPON_CMD_PURCHACE,
	WEAPON_CMD_SWAP,
	WEAPON_CMD_SELECT,
	WEAPON_CMD_DROP,
	WEAPON_CMD_TAKE,

	WEAPON_CMD_FIRE,
	WEAPON_CMD_SECFIRE,
	WEAPON_CMD_FIREREADY,

	WEAPON_CMD_ZOOM,
	WEAPON_CMD_EXTENSION,
	WEAPON_CMD_LOADMAG,
	WEAPON_CMD_LOADMAG_DUAL,
	WEAPON_CMD_LOADBULLET,

	WEAPON_CMD_REMOVESHELL,

	WEAPON_CMD_RESPAWN,
	WEAPON_CMD_DETACH_PURCHASEPRIM,
	WEAPON_CMD_DETACH_PURCHASESIDE,

	WEAPON_CMD_BLENDALPHAALL,
	WEAPON_CMD_CHANGECOLOR,

	WEAPON_CMD_LOADMAG_READY,
};

class WeaponBase;

struct WeaponParameter
{
	INT32				_nSlot = 0;
	WEAPON_CLASS_TYPE	_nType = WEAPON_CLASS_UNKNOWN;
	INT32				_nNumber = 0;
	VEC3D *				_pVec = nullptr;
	WeaponBase *		_pWeapon = nullptr;
	i3Node *			_pAttachSceneNode = nullptr;
	i3GameNode *		_pAttachGameNode = nullptr;
	bool				_extensionOn = false;
	VEC3D *				_pTargetPos = nullptr;
	bool				_bChangeNextWeapon = false;
	REAL32				_rBlendAlpha = 0.0f;
	COLORREAL *			_pDiffuse = nullptr;
	COLORREAL *			_pSpecular = nullptr;

	void	SetCreateWeapon( INT32 nSlot, WEAPON_CLASS_TYPE type, INT32 num)
	{
		_nSlot		= nSlot;
		_nType		= type;
		_nNumber	= num;
	}

	void	SetTake( WeaponBase * pWeapon)
	{
		_pWeapon = pWeapon;
	}

	void	SetDrop( INT32 nSlot, VEC3D * pPos, bool bChangeNextWeapon)
	{
		_nSlot				= nSlot;
		_pVec				= pPos;
		_bChangeNextWeapon	= bChangeNextWeapon;
	}
} ;

#endif

