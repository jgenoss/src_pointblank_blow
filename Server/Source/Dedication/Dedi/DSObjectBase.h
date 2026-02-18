#ifndef __DS_OBJECT_BASE_H__
#define __DS_OBJECT_BASE_H__

#pragma pack( push, 1)

// DS_GAME_OBJECT는 리소스 파일로부터 바이너리를 직접 읽어서 셋팅되기 때문에 멤버 수정시 오브젝트 파일을 다시 만들어야 합니다.
class DS_GAME_OBJECT 
{
public:
	UINT16					m_iSType;				// 특정 오브젝트 예외처리 { OBJECT_S_TYPE_TARGET|OBJECT_S_TYPE_W_BOX|OBJECT_S_TYPE_HELICOPTER }
	// common
	INT16					m_i16NowHP;			// Now HP	
	INT16					m_i16MaxHP;				// Max HP
		
	INT16					m_i16NetType;			// enum I3_NETWORK_GAMEOBJECT_TYPE (엔진에 정의되어 있는 값)
	INT16					m_i16NetIdx;		
	NxVec3					m_vCenterPos;			// Shape's center position (PhysX 호환) : 현재 사용되지 않음
	// weapon box 예외처리
	REAL32					m_r32RespawnTime;		// OBJECT_S_TYPE_W_BOX
	INT32					m_i32MaxShellCount;		// OBJECT_S_TYPE_W_BOX
	UINT8					m_eWeaponType;			// OBJECT_S_TYPE_W_BOX ( enum EWEAPON_TYPE { WT_RPG7_OUTPOST = 0, WT_RPG7_BOSS,	WT_K201, };
	// animation 예외처리
	REAL32					m_pDurations[ 9 ];		// 헬리콥터 예외처리를 위해서 배열로 선언
													// Respawn 헬리콥터 path key: ESPAWN_HELICOPTER_PATH_COUNT=9 (인덱스: 1..4:0팀, 5..8:1팀)
													// Riding 헬리콥터  path key: 4개 
													// StormTubeRepairTrain: path key: 3개 (처리안함)
													// 나머지는 1개로 가정 : 0번만 사용
	UINT8					m_ui8PathKeyTemp;		// respawn, riding 헬리콥터에서만 사용
	INT8					m_i8GroupIdx;			// OBJECT_S_TYPE_RESPAWN_HELICOPTER에서 팀인덱스, OBJECT_S_TYPE_W_BOX에서 그룹
	// 폭발 처리
	UINT8					m_ui8Temp;				// 라운드 시작시 0으로 초기화 되어야 한다.
	//점프대 속도
	REAL32					m_r32JumpHoldPower;		// 점프대Z
	REAL32					m_r32JumpHoldHeight;	// 점프대Y

	UINT8					m_ui8UseItemCount;				// Battle Use Item Count

	UINT32					m_ui32UseItemID[5];				// Use ItemID // 우선 5개 잡아두고 생각...
	UINT32					m_ui32UseItemProbability[5];	// Use Item 확률
	
	REAL32					m_r32UseItemStartTime;
	REAL32					m_r32UseItemRespawnTime;

public:
	DS_GAME_OBJECT()	{};
	~DS_GAME_OBJECT()	{};

	void					CopyTo( DS_GAME_OBJECT* pObject )
	{
		pObject->m_iSType				= m_iSType;
		pObject->m_i16MaxHP				= m_i16MaxHP;
		pObject->m_i16NetType			= m_i16NetType;
		pObject->m_i16NetIdx			= m_i16NetIdx;
		pObject->m_vCenterPos			= m_vCenterPos;
		pObject->m_r32RespawnTime		= m_r32RespawnTime;
		pObject->m_i32MaxShellCount		= m_i32MaxShellCount;
		pObject->m_eWeaponType			= m_eWeaponType;		
		pObject->m_i8GroupIdx			= m_i8GroupIdx;
		pObject->m_ui8Temp				= m_ui8Temp;
		pObject->m_r32JumpHoldPower		= m_r32JumpHoldPower;
		pObject->m_r32JumpHoldHeight = m_r32JumpHoldHeight;

		i3mem::Copy( pObject->m_pDurations, m_pDurations, sizeof(REAL32)*9 );

		// Use Item Copy
		pObject->m_ui8UseItemCount			= m_ui8UseItemCount;
		pObject->m_r32UseItemStartTime		= m_r32UseItemStartTime;
		pObject->m_r32UseItemRespawnTime	= m_r32UseItemRespawnTime;

		i3mem::Copy(pObject->m_ui32UseItemID,			m_ui32UseItemID,			sizeof(UINT32) * 5);
		i3mem::Copy(pObject->m_ui32UseItemProbability,	m_ui32UseItemProbability,	sizeof(UINT32) * 5);


	}
};

#pragma pack( pop )

#endif