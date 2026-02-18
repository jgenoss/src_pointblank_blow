/**
 * \file	Replay\ReplaySystem.h
 *
 * Declares the replay system class.
 */


#if !defined( __REPLAY_SYSTEM_H__)
#define __REPLAY_SYSTEM_H__

#if defined( USE_REPLAY_SYSTEM)

#include "ReplayInfoDef.h"

typedef struct __tagRecordTableInfo
{
	REAL32		_rTime;
	REC_TYPE	_uType;
	UINT32		_uEvent;
	void *		_pData;
} RECORD_TABLE_INFO;

typedef struct __tagRecordSummaryInfo
{
	INT32		_tableIndex;
	void *		_pData;
} RECORD_SUMMARY_INFO;

class CReplaySystem : public i3ElementBase
{
	I3_CLASS_DEFINE( CReplaySystem, i3ElementBase);

private:
	i3MemoryBuffer *	m_pDataBuffer;

	i3::vector<RECORD_TABLE_INFO*> m_RecordTable;

	INT32				m_iRecordIndex;
	UINT32				m_iRecordBufferPos;

	INT32				m_iStartorIdx;

	i3::vector<RECORD_SUMMARY_INFO*> m_IndexVEC2D;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexVEC3D;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexQuaternion;

	i3::vector<RECORD_SUMMARY_INFO*> m_IndexWeaponList;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexWeaponInfo;

	i3::vector<RECORD_SUMMARY_INFO*> m_IndexStage;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexChara;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexWeapon;
	i3::vector<RECORD_SUMMARY_INFO*> m_IndexObject;

	UINT32				m_stageType;
	UINT32				m_stageMapIndex;
	REAL32				m_recordTime;

private:
	RECORD_TABLE_INFO *	__getTableInfo( INT32 idx)
	{
		I3_BOUNDCHK( idx, (INT32)m_RecordTable.size());
		return m_RecordTable[ idx];
	}

	INT32		__getTableInfoCount( void)		{ return (INT32)m_RecordTable.size(); }

	UINT16		__getSummaryInfoSize( UINT32 uType, UINT32 uEvent)
	{
		switch( uType)
		{
		case REC_TYPE_GAMEEVENT :
			{
			}
			break;
		case REC_TYPE_STAGE :
			{
				switch( uEvent)
				{
				case REC_STAGE_START :		return sizeof( REC_INFO_STAGE_START);
				case REC_STAGE_END :		return sizeof( REC_INFO_STAGE_END);
				case REC_STAGE_ROUNDSTART :	return sizeof( REC_INFO_STAGE_ROUNDINFO);
				case REC_STAGE_ROUNDEND :	return sizeof( REC_INFO_STAGE_ROUNDINFO);
				case REC_STAGE_BATTLE_END :	return sizeof( REC_INFO_STAGE_END);
				case REC_STAGE_KILL :		return sizeof( REC_INFO_STAGE_KILL);
				case REC_STAGE_ENTER_USER :	return sizeof( REC_INFO_STAGE_USER_ENTER);
				case REC_STAGE_LEAVE_USER :	return sizeof( REC_INFO_STAGE_USER_LEAVE);
				default :
					I3FATAL( "must incode by event type");
					break;
				}
			}
			break;
		case REC_TYPE_CHARA :
			{
				switch( uEvent)
				{
				case REC_CHARA_ENTER :			return sizeof( REC_INFO_CHARA_ENTER);
				case REC_CHARA_LEAVE :			return sizeof( REC_INFO_CHARA_LEAVE);
				case REC_CHARA_RESPAWN :		return sizeof( REC_INFO_CHARA_RESPAWN);
				case REC_CHARA_INVINCIBLE :		return sizeof( REC_INFO_CHARA_INVINCIBLE);
				case REC_CHARA_DEATH :			return sizeof( REC_INFO_CHARA_DEATH);
				case REC_CHARA_ACTION_BODY :	return sizeof( REC_INFO_CHARA_BODY_ACTION);
				case REC_CHARA_ACTION_UPPER :	return sizeof( REC_INFO_CHARA_UPPER_ACTION);
				case REC_CHARA_ACTION_LOWER :	return sizeof( REC_INFO_CHARA_LOWER_ACTION);
				case REC_CHARA_ACTION_ZOOM :	return sizeof( REC_INFO_CHARA_ZOOM_ACTION);
				case REC_CHARA_POS :			return sizeof( REC_INFO_CHARA_POS);
				case REC_CHARA_DROPWEAPON :		return sizeof( REC_INFO_CHARA_DROPWEAPON);
				case REC_CHARA_GETWEAPON :		return sizeof( REC_INFO_CHARA_GETWEAPON);
				case REC_CHARA_HIDE :			return sizeof( REC_INFO_CHARA_HIDE);
				case REC_CHARA_EQUIPMENT :		return sizeof( REC_INFO_CHARA_EQUIPMENT);
				default:
					I3FATAL( "must incode by event type");
					break;
				}
			}
			break;
		case REC_TYPE_WEAPON :
			{
				switch( uEvent)
				{
				case REC_WEAPON_CREATE :	return sizeof( REC_INFO_WEAPON_CREATE);
				case REC_WEAPON_DELETE :	return sizeof( REC_INFO_WEAPON_DELETE);
				case REC_WEAPON_TRANSFORM :	return sizeof( REC_INFO_WEAPON_TRANSFORM);
				case REC_WEAPON_HIDE :		return sizeof( REC_INFO_WEAPON_HIDE);
				default :
					I3FATAL( "must incode by event type");
					break;
				}
			}
			break;
		case REC_TYPE_OBJECT :
			{
				switch( uEvent)
				{
				case REC_OBJECT_INIT :		return sizeof( REC_INFO_OBJECT_INIT);
				case REC_OBJECT_STATE :		return sizeof( REC_INFO_OBJECT_STATE);
				case REC_OBJECT_ANIMATION :	return sizeof( REC_INFO_OBJECT_ANIMATION);
				case REC_OBJECT_TRANSFORM : return sizeof( REC_INFO_OBJECT_TRANSFORM);
				default :
					I3FATAL( "must incode by event type");
					break;
				}
			}
			break;
		case REC_TYPE_EFFECT :
			{
			}
			break;
		default :
			I3FATAL( "invalid type!!!");
			break;
		}

		
		return 0;
	}

protected:
	bool		_CheckTableCount( void);
	void		_MoreCreateTable( INT32 nCount);
	void		_DeleteTable( void);

	bool		_CheckBuffer( UINT32 size);
	void		_MoreCreateBuffer( UINT32 size);

	void *		_GetDataBuffer( UINT32 size);
	void		_ReverseDataBufferPos( UINT32 size);
	
	VEC3D *		_GetVec3DBuffer( VEC3D * pVec);
	
	QUATERNION * _GetQuatBuffer( QUATERNION * pQuat);

	/** \brief Weaponinfo array에 대한 Buffer를 검사하고 필요하면 Buffer를 얻어온다.
		\param[in] pWeapon 버퍼에서 해당 WeaponInfo Array정보와 비교하기 위한 정보
		\return UINT32 [WEAPON_SLOT_COUNT] 의 array
	*/
	UINT32 *	_GetWeaponListBuffer( UINT32 * pWeapon);
	UINT32 *	_GetWeaponListBuffer( UINT32 prim, UINT32 sec, UINT32 melee, UINT32 thro, UINT32 item, UINT32 mission = 0);

	REC_INFO_WEAPON_INFO *	_GetWeaponInfoBuffer( WeaponBase * pWeapon);

	void		_WriteEvent( REC_TYPE type, UINT32 evt, void * pData);

	void		_SummaryData( void);

	void		_SummaryVec2D( void);
	void 		_SummaryVec3D( void);
	void		_SummaryQuaternion( void);
	void 		_SummaryWeaponList( void);
	void 		_SummaryWeaponInfo( void);

	void 		_SummaryStage( void);
	void 		_SummaryChara( void);
	void		_SummaryWeapon( void);
	void		_SummaryObject( void);

	INT32		_FindFromVec2D( VEC2D * pVec);
	INT32		_FindFromVec3D( VEC3D * pVec);
	INT32		_FindFromQuat( QUATERNION * pQuat);
	INT32		_FindFromWeaponList( UINT32 * pWeapon);
	INT32		_FindFromWeaponInfo( REC_INFO_WEAPON_INFO * pData);
	INT32		_FindFromStage( void * pData);
	INT32		_FindFromCharacter( void * pData);
	INT32		_FindFromWeapon( void * pData);
	INT32		_FindFromObject( void * pData);

	INT32		_FindFromData( UINT32 uType, void * pData);

	UINT32		_SaveVec2D( i3FileStream * pFile);
	UINT32		_SaveVec3D( i3FileStream * pFile);
	UINT32		_SaveQuat( i3FileStream * pFile);
	UINT32		_SaveWeaponList( i3FileStream * pFile);
	UINT32		_SaveWeaponInfo( i3FileStream * pFile);
	UINT32		_SaveStage( i3FileStream * pFile);
	UINT32		_SaveCharacter( i3FileStream * pFile);
	UINT32		_SaveWeapon( i3FileStream * pFile);
	UINT32		_SaveObject( i3FileStream * pFile);

public:
	CReplaySystem();
	virtual ~CReplaySystem();

	void		Create( INT32 nCount);
	void		Destroy( void);

	void		Record_GameEvent( GAME_EVENT gameEvent, INT32 arg);

	void		Record_Start( void);
	void		Record_End( void);

	void		Record_Stage_RoundStart( void);
	void		Record_Stage_RoundEnd( void);
	void		Record_Stage_Kill( INT32 killer, DEATH_INFO_CHARA * pDeathInfo, WEAPON_CLASS_TYPE weaponClass, INT32 weaponNum);

	void		Record_Chara_Enter( INT32 userIdx);
	void		Record_Chara_Leave( INT32 userIdx);
	void		Record_Chara_Respawn( INT32 userIdx, INT32 respawnIdx, REAL32 invincibleTime);
	void		Record_Chara_Invincible( INT32 userIdx, REAL32 tm);
	void		Record_Chara_Death( INT32 userIdx, UINT8 deathType, INT8 deathMotion, VEC3D * pDir);
	void		Record_Chara_BodyAction( UINT8 userIdx, INT8 currentState, INT8 nextState);
	void		Record_Chara_ZoomAction( UINT8 userIdx, INT8 currentState, INT8 nextState);
	void		Record_Chara_UpperAction( UINT8 userIdx, INT8 currentState, INT8 nextState, REAL32 curSpeed, REAL32 nextSpeed);
	void		Record_Chara_LowerAction( UINT8 userIdx, INT8 currentState, INT8 nextState, REAL32 curSpeed, REAL32 nextSpeed);
	void		Record_Chara_Position( UINT8 userIdx, VEC3D * pPos, REAL32 theta, REAL32 rho);
	void		Record_Chara_DropWeapon( UINT8 userIdx, WeaponBase * pWeapon);
	void		Record_Chara_GetWeapon( UINT8 userIdx, WeaponBase * pWeapon);
	void		Record_Chara_Hide( UINT8 userIdx, INT8 motion, REAL32 tm, REAL32 reverseTime);
	void		Record_Chara_Equipment( UINT8 userIdx, INT8 helmet, INT8 extension);

	void		Record_Weapon_Create( UINT8 idx, WeaponBase * pWeapon);
	void		Record_Weapon_Delete( UINT8 idx, WeaponBase * pWeapon, REAL32 hideTime);
	void		Record_Weapon_Transform( UINT8 idx, WeaponBase * pWeapon);
	void		Record_Weapon_Hide( UINT8 idx, WeaponBase * pWeapon, REAL32 time);

	void		Record_Object_Init( INT32 idx, i3Object * pObj);
	void		Record_Object_State( INT32 idx, i3Object * pObj);
	void		Record_Object_Animation( INT32 idx, i3Object * pObj);
	void		Record_Object_Transform( INT32 idx, i3Object * pObj);

	UINT32		Save( const char * pszPath, const char * pszName);
	UINT32		Load( const char * pszPath);


};

#endif	// USE_REPLAY_SYSTEM

#endif
