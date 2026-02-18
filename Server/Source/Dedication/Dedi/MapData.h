#pragma once

#include "NxGlobal.h"
#include "InterlockedList.h"
#include "COctreeNode.h"
#include "DSObjectGroup.h"

class CInterlockedList;


enum PX_SCENE_STATE
{
	NONE = 0,
	LOADED,
	INSTANTIATED,
	STARTED
};

// raycast 스레드 파라미터:
struct RAYCAST_THREAD_PARAM
{
	STAGE_UID			m_eStageUID;
	CInterlockedList*	m_pRequestQueue;	

	RAYCAST_THREAD_PARAM()
	{
		m_eStageUID		= STAGE_UID_NONE;
		m_pRequestQueue = NULL;
	}
}; 


// RPG BOX를 예외처리하기 위한 테이블 
//Helispot
//RPG7Box / group index:0 / respawn time:15 / Weapontype:RPG7_OUTPOST            
//RPG7Box0 / group index:0 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box00 / group index:4 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box000 / group index:4 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box0000 / group index:4 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box00000 / group index:2 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box000000 / group index:2 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box000001 / group index:3 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box0000010 / group index:3 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box00000100 / group index:3 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box0000001 / group index:1 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box00000010 / group index:1 / respawn time:15 / Weapontype:RPG7_OUTPOST
//RPG7Box000000100 / group index:1 / respawn time:15 / Weapontype:RPG7_OUTPOST
// 
//Outpost
//RPG7Box / group index:3 / respawn time:600 / Weapontype:RPG7_OUTPOST
//RPG7Box0 / group index:2 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box00 / group index:3 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box1 / group index:1 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box000 / group index:4 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box0000 / group index:4 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box0001 / group index:6 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box00010 / group index:5 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box000100 / group index:5 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box0001000 / group index:0 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box00010000 / group index:0 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box000100000 / group index:0 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box10 / group index:2 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box11 / group index:1 / respawn time:600 / Weapontype:RPG7_OUTPOST       
//RPG7Box0001001 / group index:5 / respawn time:600 / Weapontype:RPG7_OUTPOST       
// 
//Blackpanther
//RPG7Box00 / group index:0 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box000 / group index:2 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box0000 / group index:1 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box001 / group index:4 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box0001 / group index:3 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box00000 / group index:5 / respawn time:60 / Weapontype:RPG7_OUTPOST
//RPG7Box000000 / group index:6 / respawn time:60 / Weapontype:K201
//RPG7Box0000000 / group index:7 / respawn time:60 / Weapontype:K201

// 헬리콥터 원자료
//UH60_B_Path1.i3a - 3600f - 2:00:00
//UH60_B_Path2.i3a - 3750f - 2:08:33
//UH60_B_Path3.i3a - 2500f - 1:38:88
//UH60_B_Path4.i3a - 1800f - 1:00:00
//
//UH60_R_Path1.i3a - 3600f - 2:00:00
//UH60_R_Path2.i3a - 3750f - 2:08:33
//UH60_R_Path3.i3a - 2500f - 1:38:88
//UH60_R_Path4.i3a - 1800f - 1:00:00

// RPG7 원데이터를 순서대로 넣기 위해서 멤버 Alignment 안함
struct RPG7_ITEM
{
	char*			m_szName;			// debugging 용
	INT8			m_i8GroupIdx;		// 같은 그룹에 소속된 아이템은 respawn할 때 1개만 생성되어야 한다
	UINT16			m_ui16ResapwnTime;	// sec
	EWEAPON_TYPE	m_i8WeaponType;		// enum EWEAPON_TYPE { WT_RPG7_OUTPOST = 0, WT_RPG7_BOSS,	WT_K201, };	
};

// HeliSpot에서 헬리콥터를 예외처리하기 위한 테이블 (duration 값 하드코딩 하기 위함)
struct UH60_ITEM
{
	char*			m_szName;			// debugging 용
	REAL32			m_r32DurationTime;	// sec
};

// shared resource를 유지한다
class CMapData
{
public:
	// ------------------------------------------------
	// Shared Resource Management
	// ------------------------------------------------
		
	char*	GetCodeName()	{ return m_szCodeName; }
		
	// ------------------------------------------------
	// Task Processor interface function
	// ------------------------------------------------
	BOOL					EnqueueRaycastRequest( INT32 i32GroupIdx, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pFinalHit );
	BOOL					CheckRaycastHit( NxRay& ray, NxReal fDistanceSquared ); // ray cast function
	
	// ------------------------------------------------
	// Main Thread interface function
	// ------------------------------------------------
	void					Step_M();

	// ------------------------------------------------
	// Objects initialize
	// ------------------------------------------------
	BOOL					Load( const char* pszStageName, UINT8 uniqueIdx );
	BOOL					InitialStart();						// start physX
	
	// ------------------------------------------------
	// Group interface functions
	// ------------------------------------------------
	CDSObject*				GetGameObjectList()			{	return m_pGameObjects;					}
	UINT32					GetGameObjectCount() 		{	return m_ui32GameObjectCount;			}
	UINT32					GetAnimationObjectCount() 	{	return m_ui32AnimationObjectCount;		}
	UINT32					GetControlledObjectCount()	{	return m_ui32ControlledObjectCount;		}

	UINT32					GetGameObjectGroupCount()	{	return m_ui32GameObjectGroupCount;		}
	CDSObjectGroupIdx*		GetGameObjectGroupList()	{	return m_pGameObjectGroup;				}

	UINT8					GetBattleUseItemObjectCount()	{	return m_ui8BattleUseItemObjectCount;	}
	UINT8*					GetBattleUseItemObjectIdx()		{	return m_pBattleUseItemObjectIdx;		}

	DS_RESPAWN_OBJECT*		GetTotalRespawnObjectList()	{	return m_aTotalRespawnObjects;			}
	UINT32					GetTotalRespawnObjectCount(){	return m_ui32TotalRespawnObjectCount;	}

public:
	// constructor & destructors
	CMapData ( UINT32 ui32uniqueIdx, const char* pstrCodeName);
	~CMapData();

private:
	// ------------------------------------------------
	// load resource : called by load()
	// ------------------------------------------------
	BOOL _LoadCollisionObjects	( const char* pstrmapName );	// .i3scol로부터 NxShapeSet 리스트(m_pCollisionObject)를 로드하고, m_pNxScene에 추가한다.
	BOOL _LoadGameObjects		( const char* pstrmapName );	// .i3sobj로부터 m_gameObjects 리스트를 로드한다.
	BOOL _buildAnimationObjects	();								// game objects로부터 animation object를 구성
	BOOL _buildControlledObjects();								// game objects로부터 controled object를 구성
	BOOL _LoadRespawnObjects	( const char* pstrmapName );
	
	// ------------------------------------------------
	// create scene()
	// ------------------------------------------------
	BOOL _CreateScene();											// called by load()
	void _ReleaseScene( NxScene* scene);							// called by destructor
	void _ReleaseActor( NxActor* actor, NxScene* scene);			// + called by _ReleaseScene()
	
	BOOL		_CreateCollisionObjects( INT32 i32Idx, NxShapeSet* shapeSet );	// called by _loadCollisionObjects()
	NxActor*	_BuildTriMeshActor( INT32 i32Idx, NxShapeSet* shapeSet );		// + called by _CreateCollisionObjects()
	NxMaterial* _validateMaterial( NxF32 restitution, NxF32 sFriction, NxF32 dFriction);
																	// + called by _CreateCollisionObjects
	NxMaterial* _createMaterial( NxMaterialDesc * pDesc);			// ++ called by _validateMaterial()


private:
	// ------------------------------------------------
	// state information
	// ------------------------------------------------
	
	UINT8					m_ui8StageUID;
	char					m_szCodeName[DS_MAX_CODENAME];	
	UINT32					m_ui32Checksum[ 4 ];						
	// collision objects
	NxShapeSet*				m_ppCollisionObjects[DS_MAX_ACTORS];		// physX objects 
	NxShape*				m_ppShapes[DS_MAX_ACTORS];					// physX Shapes
	UINT32					m_ui32CollisionObjectCount;					// # of objects

	// game objects
	UINT32					m_ui32GameObjectCount;						// # of objects
	CDSObject				m_pGameObjects[DS_MAX_OBJECT];				// index, pos, hp
	
	UINT32					m_ui32AnimationObjectCount;					// # of objects
	UINT32					m_ui32ControlledObjectCount;				// # of objects
		
	UINT32					m_ui32GameObjectGroupCount;
	CDSObjectGroupIdx		m_pGameObjectGroup[ DS_MAX_OBJECT ];

	UINT8					m_ui8BattleUseItemObjectCount;
	UINT8					m_pBattleUseItemObjectIdx[DS_MAX_OBJECT];

	// respawn objects
	DS_RESPAWN_OBJECT		m_aTotalRespawnObjects[DS_MAX_RESPAWN_OBJECT];	// respawn objects
	UINT32					m_ui32TotalRespawnObjectCount;					// # of objects

	BOOL					m_bPhysicsActive;								// 이 맵이 피직스가 오픈 되어있는지 확인
	// ------------------------------------------------
	// ray cast
	// ------------------------------------------------
	NxRaycastHit*			_RaycastClosestShapes( NxRay& ray );
	REAL32					_UpdateTime();

	NxScene*				m_pNxScene;									// simulation world
	NxSceneQuery*			m_pNxSceneQuery;							// sceneQueryObject
	PxRaycastQueryReport	m_queryReport;								// query report
	NxRaycastHit			m_hits[1];									// raycast hit (ref by query report)
	DWORD					m_dwPreviousTime;							// update time
		
	// ------------------------------------------------
	// Physics simulation functions : called by step()
	// ------------------------------------------------
	void 					_StartPhysics();
	void 					_ProcessInputs();
	void 					_GetPhysicsResults();
	
	// ------------------------------------------------
	// Scene
	// ------------------------------------------------
	NxMat34					m_matWorld;									// world matrix
	BOOL					m_bInitWorldMatrix;							// world matrix가 두번 설정되지 않도록
	PX_SCENE_STATE			m_SceneState;								// collection data를 한 번만 release 시키기 위해 사용
																		// : NXU 모듈이 컬렉션을 관리하기 때문에 플래그로 확인
																		// : (2번 release하면 메모리 누수 발생)
	
	PxUserNotify			m_userNotify;								// user notify
	NxBounds3				m_sceneBounds;								// scene bounds

	CInterlockedList*		m_pRequestQueue;							// internal queue
	HANDLE					m_hRaycastThread;							// ray cast thread

static	DWORD WINAPI RaycastThread(LPVOID pParam);
static	void OnUpdate( RAYCAST_THREAD_PARAM* pParam );
static  void ProcessRaycastRequest( CMapData* pMap, PHYSICS_ITEM* pItem );


private:
		COctreeNode *			m_pOctreeNode;
	
public :
		COctreeNode *			getOctTree(){return m_pOctreeNode;}
		BOOL _LoadCollisionObjectsFoRUsurpation	( const char* strMapName );	

	

};
