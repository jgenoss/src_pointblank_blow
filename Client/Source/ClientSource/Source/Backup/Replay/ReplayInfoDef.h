#if !defined( __REPLAY_INFO_DEF_H__)
#define __REPLAY_INFO_DEF_H__

#if defined( USE_REPLAY_SYSTEM)

#define IS_SAME_TITLELIST(a,b)		((a[0]==b[0])&&(a[1]==b[1])&&(a[2]==b[2]))
#define IS_SAME_WEAPONLIST(a,b)		((a[0]==b[0])&&(a[1]==b[1])&&(a[2]==b[2])&&(a[3]==b[3])&&(a[4]==b[4]))

enum REC_TYPE
{
	REC_TYPE_NONE = 0,
	REC_TYPE_GAMEEVENT,
	REC_TYPE_STAGE,
	REC_TYPE_CHARA,
	REC_TYPE_WEAPON,
	REC_TYPE_OBJECT,
	REC_TYPE_EFFECT,

	REC_TYPE_COUNT
};

enum REC_STAGE_EVENT
{
	REC_STAGE_NONE = 0,
	REC_STAGE_START,
	REC_STAGE_END,
	REC_STAGE_ROUNDSTART,
	REC_STAGE_ROUNDEND,
	REC_STAGE_BATTLE_END,
	REC_STAGE_KILL,
	REC_STAGE_ENTER_USER,
	REC_STAGE_LEAVE_USER,

	REC_STAGE_EVENT_COUNT
};

enum REC_CHARA_EVENT
{
	REC_CHARA_NONE = 0,
	REC_CHARA_ENTER,
	REC_CHARA_LEAVE,
	REC_CHARA_RESPAWN,
	REC_CHARA_INVINCIBLE,
	REC_CHARA_DEATH,
	REC_CHARA_ACTION_BODY,
	REC_CHARA_ACTION_UPPER,
	REC_CHARA_ACTION_LOWER,
	REC_CHARA_ACTION_ZOOM,
	REC_CHARA_POS,
	REC_CHARA_DROPWEAPON,
	REC_CHARA_GETWEAPON,
	REC_CHARA_HIDE,
	REC_CHARA_EQUIPMENT,

	REC_CHARA_EVENT_COUNT

};

enum REC_WEAPON_EVENT
{
	REC_WEAPON_NONE = 0,
	REC_WEAPON_CREATE,
	REC_WEAPON_DELETE,
	REC_WEAPON_TRANSFORM,
	REC_WEAPON_HIDE,

	REC_WEAPON_EVENT_COUNT
};

enum REC_OBJECT_EVENT
{
	REC_OBJECT_NONE = 0,
	REC_OBJECT_INIT,
	REC_OBJECT_STATE,
	REC_OBJECT_ANIMATION,
	REC_OBJECT_TRANSFORM,

	REC_OBJECT_EVENT_COUNT
};




#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

typedef struct ALIGN4 __tagRecordInfo_Header
{
	char	_szName[ MAX_PATH];
	UINT64	_date;
	UINT64	_time;
	UINT32	_stageType;				///< Stage mode
	UINT32	_stageMapIndex;			///< Stage map index
	UINT32	_recordCount;			///< total recorded info table count
	REAL32	_recordTime;			///< total time

	// membuffer에 사용된 data count
	UINT32	_vec2dCount;
	UINT32	_vec3dCount;
	UINT32	_quatCount;
	UINT32	_vertexCount;
	UINT32	_weaponListCount;
	UINT32	_weaponInfoCount;
	UINT32	_stageCount;
	UINT32	_charaCount;
	UINT32	_weaponCount;
	UINT32	_objectCount;
	UINT32	_stringCount;

} REC_INFO_HEADER;

typedef struct ALIGN4 __tagRecordInfo_GameEvent
{	
	INT32		_arg;
} REC_INFO_GAME_EVENT;

typedef struct ALIGN4 __tagRecordInfo_Stage_Start
{
	UINT32	_recordIndex;
	REAL32	_Time;	
} REC_INFO_STAGE_START;

typedef REC_INFO_STAGE_START	REC_INFO_STAGE_END;

typedef struct ALIGN4 __tagRecordInfo_Stage_RoundInfo
{ 
	UINT8	_RoundNum;
	UINT8	_RoundFlag;
	UINT16	_PlayingUser;
	REAL32	_BattleTime;
} REC_INFO_STAGE_ROUNDINFO;

typedef struct ALIGN4 __tagRecordInfo_Stage_Kill
{
	UINT8	_killUser;
	UINT8	_deathUser;
	UINT8	_weaponClass;
	UINT8	_weaponNum;
	UINT8	_deathType;

	bool	isSame( __tagRecordInfo_Stage_Kill * pDest)
	{
		if( _killUser != pDest->_killUser)			return false;
		if( _deathUser != pDest->_deathUser)		return false;
		if( _weaponClass != pDest->_weaponClass)	return false;
		if( _weaponNum != pDest->_weaponNum)		return false;
		if( _deathType != pDest->_deathType)		return false;

		return true;
	}

} REC_INFO_STAGE_KILL;

typedef struct ALIGN4 __tagRecordInfo_Stage_UseEnter
{
	UINT8	_slot;
	char	_szNick[ NET_NICK_NAME_SIZE];
} REC_INFO_STAGE_USER_ENTER;

typedef REC_INFO_STAGE_USER_ENTER	REC_INFO_STAGE_USER_LEAVE;


typedef struct ALIGN4 __tagRecordInfo_WeaponInfo
{
	UINT8	_weaponClass;
	UINT8	_weaponNum;
	UINT8	_loadedBullet;
	UINT8	_loadedDualBullet;
	UINT8	_totalBullet;

	bool	isSame( __tagRecordInfo_WeaponInfo * pDest)
	{
		if( _weaponClass != pDest->_weaponClass)		return false;
		if( _weaponNum != pDest->_weaponNum)			return false;
		if( _loadedBullet != pDest->_loadedBullet)		return false;
		if( _loadedDualBullet != pDest->_loadedDualBullet)	return false;
		if( _totalBullet != pDest->_totalBullet)		return false;

		return true;
	}

} REC_INFO_WEAPON_INFO;

//--------------------------------------------------------------------------------------------------//
//										character event info										//

typedef struct ALIGN4 __tagRecordInfo_Character_Enter
{
	UINT8	_index;							///< user index
	UINT32	_resType;						///< resource type
	UINT8	_team;							///< team flag
	INT32	_title[ MAX_EQUIP_USER_TITLE];	///< title
	UINT32	_hair;							///< hair resource type (helmet)
	UINT8	_item;							///< item resource type
	UINT32	_clan;							///< clan mark index
	UINT8	_respawnIndex;					///< respawn index
	UINT32 *_weapon;						///< equipment weapon info [ WEAPON_SLOT_COUNT]

	bool	isSame( __tagRecordInfo_Character_Enter * pDest)
	{
		if( _index != pDest->_index)		return false;
		if( _resType != pDest->_resType)	return false;
		if( _team != pDest->_team)			return false;
		if( !IS_SAME_TITLELIST( _title, pDest->_title))	return false;
		if( _hair != pDest->_hair)			return false;
		if( _item != pDest->_item)			return false;
		if( _clan != pDest->_clan)			return false;
		if( _respawnIndex != pDest->_respawnIndex)	return false;
		if( !IS_SAME_WEAPONLIST( _weapon, pDest->_weapon) )	return false;

		return true;
	}

} REC_INFO_CHARA_ENTER;

typedef REC_INFO_CHARA_ENTER	REC_INFO_CHARA_LEAVE;

typedef struct ALIGN4 __tagRecordInfo_Character_Respawn
{
	UINT8	_index;
	UINT8	_RespawnIdx;
	UINT32 *_weapon;
	REAL32	_invincibleTime;

	bool	isSame( __tagRecordInfo_Character_Respawn * pDest)
	{
		if( _index != pDest->_index)			return false;
		if( _RespawnIdx != pDest->_RespawnIdx)	return false;
		if( !IS_SAME_WEAPONLIST( _weapon, pDest->_weapon))	return false;
		if( _invincibleTime != pDest->_invincibleTime)	return false;

		return true;
	}
} REC_INFO_CHARA_RESPAWN;

typedef struct ALIGN4 __tagRecordInfo_Character_Invincible
{
	UINT8	_index;
	REAL32	_time;

	bool	isSame( __tagRecordInfo_Character_Invincible * pDest)
	{
		if( _index != pDest->_index)		return false;
		if( _time != pDest->_time)			return false;

		return true;
	}
} REC_INFO_CHARA_INVINCIBLE;

typedef struct ALIGN4 __tagRecordInfo_Character_Death
{
	UINT8	_index;
	UINT8	_deathType;
	INT8	_deathMotion;
	UINT32 *_weapon;
	VEC3D *	_pDir;

	bool	isSame( __tagRecordInfo_Character_Death * pDest)
	{
		if( _index != pDest->_index)			return false;
		if( _deathType != pDest->_deathType)	return false;
		if( _deathMotion != pDest->_deathMotion)	return false;
		if( !IS_SAME_WEAPONLIST(_weapon, pDest->_weapon))	return false;
		if( !i3Vector::IsSame( _pDir, pDest->_pDir))	return false;

		return true;
	}

} REC_INFO_CHARA_DEATH;

typedef struct ALIGN4 __tagRecordInfo_Character_BodyAction
{
	UINT8	_index;
	INT8	_currentState;
	INT8	_nextState;
	REAL32	_currentSpeed;
	REAL32	_nextSpeed;

	bool	isSame( __tagRecordInfo_Character_BodyAction * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( _currentState != pDest->_currentState)	return false;
		if( _nextState != pDest->_nextState)		return false;
		if( _currentSpeed != pDest->_currentSpeed)	return false;
		if( _nextSpeed != pDest->_nextSpeed)		return false;

		return true;
	}
} REC_INFO_CHARA_BODY_ACTION;

typedef REC_INFO_CHARA_BODY_ACTION REC_INFO_CHARA_ZOOM_ACTION;
typedef REC_INFO_CHARA_BODY_ACTION REC_INFO_CHARA_UPPER_ACTION;
typedef REC_INFO_CHARA_BODY_ACTION	REC_INFO_CHARA_LOWER_ACTION;

typedef struct ALIGN4 __tagRecordInfo_Character_Position
{
	UINT8	_index;
	VEC3D *	_pPos;
	REAL32	_theta;
	REAL32	_rho;

	bool	isSame( __tagRecordInfo_Character_Position * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( !i3Vector::IsSame( _pPos, pDest->_pPos))	return false;
		if( _theta != pDest->_theta)				return false;
		if( _rho != pDest->_rho)					return false;

		return true;
	}

} REC_INFO_CHARA_POS;

typedef struct ALIGN4 __tagRecordInfo_Character_DropWeapon
{
	UINT8	_index;
	UINT8	_dropIndex;
	REC_INFO_WEAPON_INFO * _pWeaponInfo;

	bool	isSame( __tagRecordInfo_Character_DropWeapon * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( _dropIndex != pDest->_dropIndex)		return false;
		if( !_pWeaponInfo->isSame( pDest->_pWeaponInfo))	return false;

		return true;
	}

} REC_INFO_CHARA_DROPWEAPON;

typedef REC_INFO_CHARA_DROPWEAPON REC_INFO_CHARA_GETWEAPON;

typedef struct ALIGN4 __tagRecordInfo_Character_Hide
{
	UINT8	_index;
	INT8	_deathMotion;
	REAL32	_time;
	REAL32	_deathIntTime;

	bool	isSame( __tagRecordInfo_Character_Hide * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( _deathMotion != pDest->_deathMotion)	return false;
		if( _time != pDest->_time)					return false;
		if( _deathIntTime != pDest->_deathIntTime)	return false;

		return true;
	}

} REC_INFO_CHARA_HIDE;

typedef struct ALIGN4 __tagRecordInfo_Character_Equipment
{
	UINT8	_index;
	INT8	_helmetState;
	INT8	_weaponExtension;

	bool	isSame( __tagRecordInfo_Character_Equipment * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( _helmetState != pDest->_helmetState)	return false;
		if( _weaponExtension != pDest->_weaponExtension)	return false;

		return true;
	}

} REC_INFO_CHARA_EQUIPMENT;


//----------------------------------------------------------------------------------------------//
//										weapon event info										//
//----------------------------------------------------------------------------------------------//
typedef struct ALIGN4 __tagRecordInfo_Weapon_Create
{
	UINT8	_index;
	UINT8	_netIndex;
	REC_INFO_WEAPON_INFO * _pWeaponInfo;
	REAL32	_time;

	bool	isSame( __tagRecordInfo_Weapon_Create * pDest)
	{
		if( _index != pDest->_index)				return false;
		if( _netIndex != pDest->_netIndex)			return false;
		if( !_pWeaponInfo->isSame( pDest->_pWeaponInfo))	return false;
		if( _time != pDest->_time)					return false;

		return true;
	}

} REC_INFO_WEAPON_CREATE;

typedef REC_INFO_WEAPON_CREATE REC_INFO_WEAPON_DELETE;

typedef struct ALIGN4 __tagRecordInfo_Weapon_Transform
{
	UINT8		_index;
	VEC3D *		_pPos;
	QUATERNION *_pRot;
} REC_INFO_WEAPON_TRANSFORM;

typedef REC_INFO_WEAPON_DELETE REC_INFO_WEAPON_HIDE;


//------------------------------------------------------------------------------------------------------//
//											Object Event info											//

typedef struct ALIGN4 __tagRecordInfo_Object_Init
{
	INT32	_index;
	INT32	_state;
	REAL32	_hp;

	bool	isSame( __tagRecordInfo_Object_Init * pDest)
	{
		if( _index != pDest->_index)					return false;
		if( _state != pDest->_state)					return false;
		if( _hp != pDest->_hp)							return false;

		return true;
	}
} REC_INFO_OBJECT_INIT;

typedef struct ALIGN4 __tagRecordInfo_Object_State
{
	INT32	_index;
	INT32	_currentState;
	INT32	_nextState;
	REAL32	_hp;

	bool	isSame( __tagRecordInfo_Object_State * pDest)
	{
		if( _index != pDest->_index)					return false;
		if( _currentState != pDest->_currentState)		return false;
		if( _nextState != pDest->_nextState)			return false;
		if( _hp != pDest->_hp)							return false;

		return true;
	}

} REC_INFO_OBJECT_STATE;

typedef struct ALIGN4 __tagRecordInfo_Object_Animation
{
	INT32	_index;
	INT32	_flag;

	bool	isSame( __tagRecordInfo_Object_Animation * pDest)
	{
		if( _index != pDest->_index)					return false;
		if( _flag != pDest->_flag)						return false;

		return true;
	}
} REC_INFO_OBJECT_ANIMATION;

typedef struct ALIGN4 __tagRecordInfo_Object_Transform
{
	INT32		_index;
	VEC3D *		_pPos;
	QUATERNION * _pRot;

	bool	isSame( __tagRecordInfo_Object_Transform * pDest)
	{
		if( _index != pDest->_index)					return false;
		if( !i3Vector::IsSame( _pPos, pDest->_pPos))	return false;
		if( !i3Vector::IsSame( _pRot, pDest->_pRot))	return false;

		return true;
	}
} REC_INFO_OBJECT_TRANSFORM;



#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

#endif	// USE_REPLAY_SYSTEM

#endif
