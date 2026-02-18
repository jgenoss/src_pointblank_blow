// MapData.cpp
//
// YouJong Ha
//	Last update : 2011-01-13 (yyyy:mm:dd)
//	
// Description:
//	CMapData.h 구현 파일
//
//
// Dependency: 
//		NxShapeSet/i3scolFile
//		
#include "pch.h"
#include "i3sobjFile_s.h"
#include "i3scolFile_s.h"
#include "i3sRpnFile_s.h"

#include "MapData.h"
#include "MapManager.h"
#include <crtdbg.h>
#include "InterlockedList.h"
#include "GroupMaker.h"
#include "DediRoom.h"

extern TCHAR g_pCCurrentPath[SERVER_STRING_COUNT];
extern NxBounds3 g_StageBounds[ STAGE_UID_MAX ];
void CreateCube(NxScene* pScene, const NxVec3& posMin, const NxVec3& posMax, const NxVec3* initial_velocity=NULL);

// RPG7 Weapon Box를 예외처리 하기 위한 테이블
// 맵에 따라 처리 방식이 달라짐
static RPG7_ITEM s_RPG7Box_Helispot[] = 
{
	{"RPG7Box", 0 , 15 , WT_RPG7_OUTPOST },             
	{"RPG7Box0", 0 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box00", 4 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000", 4 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0000", 4 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box00000", 2 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000000", 2 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000001", 3 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0000010", 3 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box00000100", 3 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0000001", 1 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box00000010", 1 , 15 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000000100", 1 , 15 , WT_RPG7_OUTPOST } 	
};

static RPG7_ITEM s_RPG7Box_Outpost[] =
{
	{"RPG7Box", 3 , 600 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0", 2 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box00", 3 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box1", 1 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box000", 4 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box0000", 4 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box0001", 6 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box00010", 5 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box000100", 5 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box0001000", 0 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box00010000", 0 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box000100000", 0 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box10", 2 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box11", 1 , 600 , WT_RPG7_OUTPOST },        
	{"RPG7Box0001001", 5 , 600 , WT_RPG7_OUTPOST } 
};

static RPG7_ITEM s_RPG7Box_Blackpanther[] =
{
	{"RPG7Box00", 0 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000", 2 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0000", 1 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box001", 4 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box0001", 3 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box00000", 5 , 60 , WT_RPG7_OUTPOST }, 
	{"RPG7Box000000", 6 , 60 , WT_K201 },
	{"RPG7Box0000000", 7 , 60 , WT_K201 }
};

static RPG7_ITEM s_RPG7Box_HoluDay[] = 
{
	{"CrownWaterMelon0", 0 , 6000 , WT_CROWNWATERMELON }, 
	{"ChineseCleaver0", 1 , 6000 , WT_CHINESE_CLEAVER },             
	{"ChineseCleaver1", 2 , 6000 , WT_CHINESE_CLEAVER }, 
};
// respawn helicopter는 9개의 path key 갖고,
// riding helicopter는 4개의 path key를 갖는다.
// 레드팀과 블루팀의 duration time이 달라지면 문제가 생김.
static UH60_ITEM	s_UH60_DurationTime[] =
{
	{ "",				  0.0f		 }, // path key index를 일치시키기 위해서임
	{ "UH60_R_Path1.i3a", 3600/30.0f },	// client code s_PathName_Breakdown2_Helicopter과 순서 맞춤
	{ "UH60_R_Path2.i3a", 3750/30.0f }, 
	{ "UH60_B_Path1.i3a", 3600/30.0f }, 
	{ "UH60_B_Path2.i3a", 3750/30.0f }, 
	
	{ "UH60_R_Path3.i3a", 2500/30.0f }, 
	{ "UH60_R_Path4.i3a", 1800/30.0f }, 
	{ "UH60_B_Path3.i3a", 2500/30.0f }, 
	{ "UH60_B_Path4.i3a", 1800/30.0f }, 
};

// outpost data
static UH60_ITEM	s_Outpost_UH60_DurationTime[] =
{
//	{ "",				  0.0f		 },			// path key index를 일치시키기 위해서임
	{ "OutPost_UH60_Idle.i3a",  3.3333333f },	// client code s_PathName_Outpost_Helicopter과 순서 맞춤
	{ "OutPost_UH60_Path1.i3a", 103.73333f }, 
	{ "OutPost_UH60_Path2.i3a", 122.73333f }, 
	{ "OutPost_UH60_Path3.i3a", 129.53334f }, 
};

static UH60_ITEM	s_RepairTrain_DurationTime[] =
{
	{ "Train_Repair_Idle.i3a",	3.0f },
	{ "Train_Repair_Move.i3a",	10.0f },
	{ "Train_Repair_End.i3a",	3.0f },
};

// 지하철 예외처리를 위한 데이터
/*
각 맵 별 애니메이션 프레임은 다음과 같습니다.

스톰 튜브

지하철 1 - 300f
지하철 2 - 300f
설치지점 지하철 - 300f

업타운

트램 - 856f
*/


CMapData::CMapData( UINT32 uniqueIdx, const char* szCodeName )
{
	// unique information
	strcpy_s( m_szCodeName, sizeof( m_szCodeName ), szCodeName );
	m_szCodeName[ sizeof( m_szCodeName ) - 1 ] = '\0';

	m_ui8StageUID = (UINT8)uniqueIdx;
	
	// stage information
	i3mem::FillZero( m_ppCollisionObjects,	sizeof(m_ppCollisionObjects));	// pointer array
	i3mem::FillZero( m_ppShapes,			sizeof(m_ppShapes) );			// pointer array
	i3mem::FillZero( m_pGameObjects,		sizeof(m_pGameObjects));			// struct of arrary
	i3mem::FillZero( m_aTotalRespawnObjects,sizeof( m_aTotalRespawnObjects ));	// struct of arrary

	i3mem::FillZero( m_ui32Checksum,		sizeof(UINT32)*4 );

	// Battle Use Item Object Idx
	i3mem::FillZero( m_pBattleUseItemObjectIdx, sizeof(UINT8) * DS_MAX_OBJECT);
	m_ui8BattleUseItemObjectCount = 0;

	m_ui32CollisionObjectCount  	= 0;

	m_ui32GameObjectCount			= 0;
	m_ui32AnimationObjectCount		= 0;
	m_ui32ControlledObjectCount 	= 0;

	m_ui32GameObjectGroupCount		= 0;

	m_ui32TotalRespawnObjectCount	= 0;
	
	// physics state
	m_pNxScene			= NULL;							// simulation world
	m_pNxSceneQuery		= NULL;							// sceneQueryObject
	
	// time
	m_dwPreviousTime	= GetTickCount();
	
	m_bPhysicsActive				= FALSE;

	// matrix
	m_matWorld.id();
	m_bInitWorldMatrix = FALSE;
	m_SceneState = NONE;					// release flag. collection 오브젝트의 포인터를 포관하지 않기 때문에 필요하다.
	
	// query report
	m_queryReport.SetOutputBuffer(&m_hits[0], 1);
	
	m_pRequestQueue = NULL;
	if( g_pConfig->m_bPhysicsActive ) m_pRequestQueue = new CInterlockedList;

	m_pOctreeNode =NULL;

}

CMapData::~CMapData() 
{ 
	if( m_pNxScene )
	{
		if( m_SceneState == STARTED )
		{
			_GetPhysicsResults();
		}

		if ( m_SceneState >= LOADED )
		{
			NXU::NxuPhysicsCollection* c = NXU::extractCollectionScene( m_pNxScene );
			NXU::releaseCollection( c );
		}
		
		_ReleaseScene( m_pNxScene );
		m_pNxScene = NULL;
	}
	
	for( UINT32 i = 0; i < m_ui32CollisionObjectCount; i++ )
	{
		SAFE_DELETE( m_ppCollisionObjects[i] );
	}
	SAFE_DELETE(m_pOctreeNode);
	SAFE_DELETE( m_pRequestQueue );

	I3TRACE("STAGE ORD:%d DESTURCTED!\n",  m_ui8StageUID );

}


void CreateCube(NxScene* pScene, const NxVec3& pos, UINT32 size=2, const NxVec3* initial_velocity=NULL)
{
	/*if(!gSceneBounds.contain(pos))
	{
		printf("Trying to create an actor outside of the scene bounds - creation aborted!\n");
		return;
	}*/

	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.0f;
	BodyDesc.flags |= NX_BF_FROZEN;
	if(initial_velocity) BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions = NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;

	NxActor* a = pScene->createActor(ActorDesc);
	if(a)
	{
		a->userData = LongToPtr(size<<1);
	}
}

// create scene
BOOL CMapData::_CreateScene()
{
	NxSceneDesc sceneDesc;
	NxVec3		DefaultGravity(0.0f, -9.8f, 0.0f);
	sceneDesc.setToDefault();

	if( IsHardwarePresent() )
	{
 		sceneDesc.simType			 = NX_SIMULATION_HW;
	}

	sceneDesc.gravity				 = DefaultGravity;
	sceneDesc.backgroundThreadCount	 = 1;
	if( STAGE_UID_FACTORYRUINS	!= m_ui8StageUID &&
		STAGE_UID_TUTORIAL		!= m_ui8StageUID )
	{
		sceneDesc.maxBounds				 = &m_sceneBounds;
	}
	
	// Create a scene
	m_pNxScene = gPhysicsSDK->createScene( sceneDesc );
	if( m_pNxScene == NULL ) 
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		m_pNxScene = gPhysicsSDK->createScene( sceneDesc );
		if( m_pNxScene == NULL )
		{
			DSERROR("[CMapData::_CreateScene] Unable to create a PhysX scene(StageUID:%d)\n", m_ui8StageUID );
			return FALSE;
		}
		else
		{
			DSERROR("[CMapData::_CreateScene] NX_SIMULATION_SW Performance Warning(StageUID:%d)\n", m_ui8StageUID );
		}
	}

	// Create scene query object
	NxSceneQueryDesc sceneQueryDesc;
	sceneQueryDesc.setToDefault();
	sceneQueryDesc.executeMode = NX_SQE_ASYNCHRONOUS;
	sceneQueryDesc.report = &m_queryReport;
	m_pNxSceneQuery = m_pNxScene->createSceneQuery( sceneQueryDesc );
	if( m_pNxSceneQuery == NULL )
	{
		DSERROR("[CMapData::_CreateScene] Unable to create a PhysX scene query (StageUID:%d)\n", m_ui8StageUID );
		return FALSE;
	}

	// yjha test code
	//InitTerrain( m_pNxScene );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// SHARED RESOURCE LOADING PROCEDURE
// ----------------------------------
//  내부 함수를 호출해서 world collision, game objects, animation 테이블을
//	셋업하는 함수
BOOL CMapData::Load( const char* pszMapName ,UINT8 uniqueIdx)
{
	BOOL bRet = TRUE;
	
	// scene Bounds	
	I3ASSERT( ( uniqueIdx > STAGE_UID_NONE ) && ( uniqueIdx < STAGE_UID_MAX ) );
	if( uniqueIdx <= STAGE_UID_NONE || uniqueIdx >= STAGE_UID_MAX ) return FALSE;
		
	m_sceneBounds = g_StageBounds[ uniqueIdx ];
	// log map info
	{
		NxVec3 dims, center, extents;
		m_sceneBounds.getDimensions(dims);
		m_sceneBounds.getCenter(center);
		m_sceneBounds.getExtents(extents);
		DSMSG("\r\n[%s]\r\n"
				"\tdim   x:%f, y:%f, z:%f\r\n"
				"\tcenter x:%f, y:%f, z:%f\r\n"
				"\textent x:%f, y:%f, z:%f\n",
				pszMapName, dims.x, dims.y, dims.z, center.x, center.y, center.z, extents.x, extents.y, extents.z);
	}

	// create scene
	if( g_pConfig->m_bPhysicsActive )
	{
		if( _CreateScene() )
		{
			m_bPhysicsActive = TRUE;

			// load collision objects
			if( !_LoadCollisionObjects( pszMapName )) 
			{
				DSERROR("[MapData]LoadCollisionObjects Fail:[%d:%s]\n", m_ui8StageUID, pszMapName);
				return FALSE;
			}
		}
		else
		{
			DSERROR("[MapData]CreateScene  Fail:[%d:%s]\n", uniqueIdx, pszMapName );
			DSERROR(" maximum number of scenes  : %d \n", uniqueIdx);
			//return FALSE;
		}
	}

	switch( uniqueIdx )
	{
	case STAGE_UID_HOLIDAY:	
	case STAGE_UID_SANDSTORM:
	case STAGE_UID_STILLRAID:
		_LoadCollisionObjectsFoRUsurpation( pszMapName );
		break;
	}

	// 중력햄 범위 확인용 (삭제하지 마시오)
	//CreateCube( m_pNxScene, s_StageBounds[ m_stageOrd ].min, s_StageBounds[ m_stageOrd ].max, &NxVec3(0.0f, 0.0f, 0.0f) );
	
	// load stage objects
	if( !_LoadGameObjects( pszMapName ) ) 
	{
		DSERROR("[ERR] map:%s load failed : _loadGameObjects\n", pszMapName);
		return FALSE;
	}
	
	// build animation objects
	if( m_ui32GameObjectCount > 0)
	{
		bRet = _buildAnimationObjects();
		bRet = _buildControlledObjects();
	}

	// load respawn objects
	if( !_LoadRespawnObjects( pszMapName ) )
	{
		DSERROR("[ERR] map:%s load respawn objects failed : _loadGameObjects\n", pszMapName);
		return FALSE;
	}

	// create update thread
	if( g_pConfig->m_bPhysicsActive )
	{
		RAYCAST_THREAD_PARAM*	pParam = new RAYCAST_THREAD_PARAM;
		pParam->m_eStageUID			= static_cast<STAGE_UID>(uniqueIdx);
		pParam->m_pRequestQueue		= m_pRequestQueue;	

		m_hRaycastThread = CreateThread( NULL, 0, CMapData::RaycastThread, pParam, 0, NULL);
		if ( m_hRaycastThread == NULL)
		{
			DSERROR( "[CTaskMgr::ReCreateThread] CreateThread failed: TASK_PROCESSOR: %d", GetLastError());
			return FALSE;
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// COLLISION OBJECT LOAD PROCEDURE
// --------------------------------
// *.i3col에서 collision 데이터를 로드한다. 
BOOL CMapData::_LoadCollisionObjects(const char* mapName)
{
	char szPath[MAX_PATH];
	i3String::Format(szPath, sizeof(szPath), 
										"%s\\%s\\%s.%s", 
										g_pCCurrentPath,
										MEDIA_PATH,
										mapName,
										SCOL_FILE_EXTENSION);

	i3scolFile	file;

	if( INVALID_HANDLE_VALUE == file.Open(szPath, FALSE))
		return FALSE;

	I3ASSERT( file.IsValid() );
	if( FALSE == file.IsValid() ) return FALSE;
	
	INT32 i32Count = file.GetCountOfCollisionObject();
	if( i32Count >= DS_MAX_ACTORS ) return FALSE;

	m_ui32CollisionObjectCount = 0;
	
	//m_vMove.zero();
	for (INT32 i = 0; i < i32Count; i++)
	{
		m_ppCollisionObjects[ i ] = new NxShapeSet;
		if( NULL == m_ppCollisionObjects[i])
		{
			DSERROR("[LoadCollisionObjects]Memory\n");
			break;
		}

		if( 0 < file.Read( m_ppCollisionObjects[i]) )
		{
			if( FALSE == _CreateCollisionObjects( i, m_ppCollisionObjects[i] ))
			{
				DSERROR("[LoadCollisionObjects]_CreateCollisionObjects\n");
				break;
			}
			
			m_ui32CollisionObjectCount++;
		}
		else
		{
			DSERROR("[ERROR:_loadCollisionObjects] Stage:[%s]\n", mapName);
			SAFE_DELETE( m_ppCollisionObjects[ i ] );
			break;
		}
	}

	file.GetChecksum( m_ui32Checksum );
	file.Close();
	
	return TRUE;
}

BOOL CMapData::_LoadCollisionObjectsFoRUsurpation( const char* strMapName )
{

	char szPath[MAX_PATH];
	sprintf_s(szPath, sizeof(szPath), 
		"%s\\%s\\%s_Svr.%s", 
		g_pCCurrentPath, 
		MEDIA_PATH,
		strMapName,
		SCOL_FILE_EXTENSION);

	i3scolFile	file;

	if( INVALID_HANDLE_VALUE == file.Open(szPath, FALSE))
		return FALSE;

	I3ASSERT( file.IsValid() );
	if( FALSE == file.IsValid() )	return FALSE;

	int nCount = file.GetCountOfCollisionObject();
	if( nCount >= DS_MAX_ACTORS )	return FALSE;

	m_pOctreeNode = new COctreeNode;
	if( NULL == m_pOctreeNode )		return FALSE;

	m_pOctreeNode->CreateCollisionData();
	m_pOctreeNode->GetWorld()->setMaxObjCount(nCount);
	
	//m_vMove.zero();
	for (int i = 0; i < nCount; i++)
	{
		m_pOctreeNode->GetWorld()->CreateNxShapeSet(i);

		if( NULL == m_pOctreeNode->GetWorld()->getNxShapeData(i))
		{
			DSERROR("[LoadCollisionObjects]Memory\n");
			break;
		}

		if(	file.Read( m_pOctreeNode->GetWorld()->getNxShapeData(i)) < 1 )
		{
			DSERROR("[ERROR:_loadCollisionObjects] Stage:[Holiday]\n");
			//		SAFE_DELETE( m_pOctreeNode->getNxshapeSet(i) );
			break;
		}
		m_pOctreeNode->GetWorld()->CovertCollisionData(i);
	}

	m_pOctreeNode->FindPartitionPoint(m_pOctreeNode->GetWorld());

	INT32 i32TotalTriangleCount = m_pOctreeNode->GetSceneTriangleCount();

	m_pOctreeNode->CreateNode(m_pOctreeNode->GetWorld(),i32TotalTriangleCount,m_pOctreeNode->GetCenterPos(),m_pOctreeNode->GetWidth(),(OCTREE_NODES) 0 ,0);
	
	I3TRACE("i32TotalTriangleCount : %d \n", i32TotalTriangleCount);

	file.Close();

	return TRUE;

}
/////////////////////////////////////////////////////////////////////////////////////////
// GAME OBJECT LOAD PROCEDURE
// --------------------------------
// *.i3sobj에서 게임 오브젝트를 로드한다.
//
BOOL CMapData::_LoadGameObjects(const char* mapName)
{
	char szPath[MAX_PATH] = { '\0' };

	UINT32	ui32WeaponBoxCount = 0; // rpg7 예외처리를 위함

	i3String::Format(szPath, sizeof(szPath), 
										"%s\\%s\\%s.%s", 
										g_pCCurrentPath,
										MEDIA_PATH,
										mapName,
										SOBJ_FILE_EXTENSION);
	
	
	
	i3sobjFile	file;

	if( INVALID_HANDLE_VALUE == file.Open( szPath, FALSE) )
	{
		DSERROR("[CMapData::_loadGameObjects] File open error: %s\n", szPath );
		return FALSE;
	}
	
	
	UINT32 ui32Count = file.GetCountOfGameObject();
	if( ui32Count >= DS_MAX_OBJECT )
	{
		DSWARNING("[CMapData::_loadGameObjects] Max game object count: %d\n", ui32Count);
		ui32Count = DS_MAX_OBJECT;
	}

	m_ui32GameObjectCount = 0;
	
	CDSObject *pObject;
	for( UINT32 i = 0; i < ui32Count; i++)
	{
		pObject = &m_pGameObjects[ i ];

		if( file.Read( (DS_GAME_OBJECT*)(&pObject->m_iSType) ) )
		{
			pObject->Init();
			// [예외 처리] 오브젝트의 프로퍼티를 하드코딩된 값으로 대치한다
			
			// weapon box (rpg7) 
			// stage id에 따라 달라진다
			if ( pObject->m_iSType == OBJECT_S_TYPE_W_BOX )
			{
				pObject->m_i32MaxShellCount = 255; // UINT8로 전송해야 하기 때문 OUTPOST에서는 0으로 설정.
				pObject->m_fActionDistance	= 1.5f;
				pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_CONTROLED );
				pObject->SetObjectType( UDP_OBJECT_TYPE_HP| UDP_OBJECT_TYPE_POSITION | UDP_OBJECT_TYPE_OWN );
				pObject->SetSyncObjectType( UDP_OBJECT_TYPE_SHELLCOUNT | UDP_OBJECT_TYPE_RESPAWN );

				pObject->SetObjectUseType( OBJECT_USE_TYPE_RESPAWN );

				switch ( m_ui8StageUID )
				{
				case STAGE_UID_HELISPOT:
					{
						if ( ui32WeaponBoxCount < sizeof(s_RPG7Box_Helispot) / sizeof( s_RPG7Box_Helispot[0] ))
						{
							pObject->m_i32MaxShellCount		= 1; // 1로 바뀜. 로그보니 2010년 10월 15일 클라 네트워크 버그 수정이후.

							INT8 i8GroupIdx					= pObject->m_i8GroupIdx;
							m_pGameObjectGroup[ i8GroupIdx ].SetGroupType( DS_OBJECT_GROUP_TYPE_TURN );
							m_pGameObjectGroup[ i8GroupIdx ].InsertObject( i );
							m_ui32GameObjectGroupCount		= 5;
						}
						else
						{
							DSERROR("Error WeaponBoxCount STAGE_UID_HELISPOT " );
							I3ASSERT( 0 && "Error WeaponBoxCount STAGE_UID_HELISPOT"); 
						}
					}
					break;
				case STAGE_UID_OUTPOST:
					{
						if ( ui32WeaponBoxCount < sizeof(s_RPG7Box_Outpost) / sizeof( s_RPG7Box_Outpost[0] ))
						{
							pObject->m_i32MaxShellCount		= 0;

							INT8 i8GroupIdx					= pObject->m_i8GroupIdx;
							m_pGameObjectGroup[ i8GroupIdx ].SetGroupType( DS_OBJECT_GROUP_TYPE_TURN );
							m_pGameObjectGroup[ i8GroupIdx ].InsertObject( i );
							m_ui32GameObjectGroupCount		= 7;
						}
						else
						{
							DSERROR("Error WeaponBoxCount STAGE_UID_OUTPOST " );
							I3ASSERT( 0 && "Error WeaponBoxCount STAGE_UID_OUTPOST");  
						}
					}
					
					break;
				case STAGE_UID_BLACKPANTHER:
					{
						if ( ui32WeaponBoxCount < sizeof(s_RPG7Box_Blackpanther) / sizeof( s_RPG7Box_Blackpanther[0] ))
						{
							INT8 i8GroupIdx					= s_RPG7Box_Blackpanther[ ui32WeaponBoxCount ].m_i8GroupIdx;
							m_pGameObjectGroup[ i8GroupIdx ].SetGroupType( DS_OBJECT_GROUP_TYPE_TURN );
							m_pGameObjectGroup[ i8GroupIdx ].InsertObject( i );
							m_ui32GameObjectGroupCount		= 8;
						}
						else
						{
							DSERROR("Error WeaponBoxCount STAGE_UID_BLACKPANTHER " );
							I3ASSERT( 0 && "Error WeaponBoxCount STAGE_UID_BLACKPANTHER");  
						}
					}

					break; 
				case STAGE_UID_HOLIDAY:  
					{
						if( WT_CROWNWATERMELON == pObject->m_eWeaponType )
						{ 
							pObject->m_i32MaxShellCount		= 1;

							pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_MOVE );
							pObject->SetObjectType( UDP_OBJECT_TYPE_HP| UDP_OBJECT_TYPE_POSITION | UDP_OBJECT_TYPE_OWN  | UDP_OBJECT_TYPE_CONVOY);
							pObject->SetSyncObjectType( UDP_OBJECT_TYPE_OWN | UDP_OBJECT_TYPE_POSITION );

							pObject->SetObjectUseType( OBJECT_USE_TYPE_HAVE );
														
							pObject->m_eResetPosition  = OBJECT_RESET_POSITION_DIRECT;
						} 
					} 
					break;
				}
				
				ui32WeaponBoxCount++;
			}

			if( pObject->m_iSType == OBJECT_S_TYPE_CONVOY )
			{
				pObject->m_i32MaxShellCount = 1;

				pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_MOVE );
				pObject->SetObjectType( UDP_OBJECT_TYPE_OWN | UDP_OBJECT_TYPE_POSITION | UDP_OBJECT_TYPE_CONVOY);
				pObject->SetSyncObjectType( UDP_OBJECT_TYPE_OWN | UDP_OBJECT_TYPE_POSITION );

				pObject->SetObjectUseType( OBJECT_USE_TYPE_HAVE );
														
				pObject->m_eResetPosition  = OBJECT_RESET_POSITION_DIRECT;
			}

			if (pObject->m_iSType == OBJECT_S_TYPE_JUMPHOLD)
			{
				pObject->m_fActionDistance = 1.0f;
			}

			// battle Use Item Type 설정
			if (pObject->m_iSType == OBJECT_S_TYPE_USEITEMBOX)
			{
				// 순간적인 속도가 빨라서 5 Distance정도가 필요 할 듯 싶다.
				pObject->m_fActionDistance = 5.f;
				pObject->m_i32MaxShellCount = 1;
				pObject->SetHeadType(SEND_HEAD_TYPE_OBJ_CONTROLED);
				pObject->SetObjectType( UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_OWN);
				pObject->SetSyncObjectType(UDP_OBJECT_TYPE_STATE);

				pObject->SetObjectUseType(OBJECT_USE_TYPE_RESPAWN);

				// Index 추가
				m_pBattleUseItemObjectIdx[m_ui8BattleUseItemObjectCount++] = i;
			}

			switch ( m_ui8StageUID )
			{
				case STAGE_UID_MERRYWHITEVILLAGE:
				{
					// 스위치
					switch( i )
					{
					case 0:
					case 1:
						{
							pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_STATIC );
							pObject->SetObjectType( UDP_OBJECT_TYPE_ACTIVE | UDP_OBJECT_TYPE_SENDER );
							pObject->SetSyncObjectType( UDP_OBJECT_TYPE_ACTIVE );

							pObject->SetObjectUseType( OBJECT_USE_TYPE_SWITCH );
						}
						break;
					}
				}
				break;
			}
			
			if( I3_NETWORK_GAMEOBJECT_TYPE_STATIC == pObject->m_i16NetType )
			{
				if( 0 == pObject->GetObjectType()	)
				{
					pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_STATIC );

					pObject->SetObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER );
					pObject->SetSyncObjectType( UDP_OBJECT_TYPE_STATE );
				}
			}
			else if( I3_NETWORK_GAMEOBJECT_TYPE_ANIM == pObject->m_i16NetType )
			{
				pObject->SetHeadType( SEND_HEAD_TYPE_OBJ_ANIM );
							
				pObject->SetObjectUseType( OBJECT_USE_TYPE_RIDING );
				// respawn helicopter
				switch( pObject->m_iSType )
				{
				case OBJECT_S_TYPE_RESPAWN_HELICOPTER:
					{
						pObject->m_fActionDistance	= 3.f;
						pObject->SetObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY );
						pObject->SetSyncObjectType( UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY  );

						if ( pObject->m_i8GroupIdx == 1 ) // 리스폰 헬기는 1이 블루팀이다
						{
							pObject->setPathKey( 3 );
							pObject->m_i8TeamIdx	= TEAM_BLUE;
						}
						else
						{
							pObject->setPathKey( 1 ); 
							pObject->m_i8TeamIdx	= TEAM_RED;
						}
						pObject->SetAnimRepeat( TRUE );
						//pObject->m_eUseType	= OBJECT_USE_TYPE_RIDING;
					}
					break;			
				case OBJECT_S_TYPE_RIDING_HELICOPTER:
					{
						pObject->m_fActionDistance	= 3.f;
						pObject->SetObjectType( UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY );
						pObject->SetSyncObjectType( UDP_OBJECT_TYPE_STATE | UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_OWN | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY  );

						pObject->setPathKey( 0 );
						pObject->SetExplosionHP( 0, static_cast<UINT16>(0.3f * pObject->GetMaxHP()) );
						pObject->m_eUseType	= OBJECT_USE_TYPE_RIDING;
						pObject->SetAnimRepeat( FALSE );
					}
					break;
				case OBJECT_S_TYPE_REPAIR_TRAIN:
					{
						pObject->m_fActionDistance	= 2.f;
						pObject->SetObjectType( UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY );
						pObject->SetSyncObjectType( UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY );

						pObject->setPathKey( 0 );
						pObject->SetAnimRepeat( FALSE );
					}
					break;
				default:
					{
						pObject->SetObjectType( UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY );
						pObject->SetSyncObjectType( UDP_OBJECT_TYPE_ANIMPATH | UDP_OBJECT_TYPE_ANIMKEY  );

						pObject->setPathKey( 0 );
						pObject->SetAnimRepeat( TRUE );
					}
					break;
				}
			}
			
			// STAGE_ID_DOWNTOWN2 예외처리 12개 오브젝트에 netType이 설정되어 있지 않음.
			// STATIC으로 셋팅
			/*
			if ( GetStageOrd() == STAGE_ORD_BB_DOWNTOWN2 || 
				 GetStageOrd() == STAGE_ORD_BB_DOWNTOWN ) 
			{
				pObject->m_i16NetType = I3_NETWORK_GAMEOBJECT_TYPE_STATIC;
			}
			*/
			//
			//// 블랙팬저 탱크 예외처리(5월 2일 땜빵이었는데 6월 30일state 문제 생겨서 삭제)
			//if ( getStageId( m_mapId ) == STAGE_ID_BLACKPANTHER )
			//{
			//	if ( pObject->m_iSType == OBJECT_S_TYPE_TARGET )
			//	{
			//		pObject->m_i16NetType = I3_NETWORK_GAMEOBJECT_TYPE_ANIM;
			//	}
			//}

			// 삭제하지 마세요: 타겟 오브젝트를 구분하기 위함
			// 브레이크 다운 (순서로 구분해야 함)
			// net index: 0, team index: 0, net type: 1, hp: 6000
			// net index: 1, team index: 0, net type: 1, hp: 6000
			if ( m_ui8StageUID == STAGE_UID_BREAKDOWN ||
				 m_ui8StageUID == STAGE_UID_BREAKDOWN )
			{
				if ( pObject->m_iSType == OBJECT_S_TYPE_TARGET )
				{
					if ( i % 2 == 0 ) pObject->m_i8GroupIdx = 0; 
					if ( i % 2 == 1 ) pObject->m_i8GroupIdx = 1; 
					
					pObject->SetExplosionHP( 0, pObject->GetMaxHP() / 2 );
					pObject->SetExplosionHP( 1, 0 );
					pObject->m_i16NetType			= I3_NETWORK_GAMEOBJECT_TYPE_STATIC;
				}
			}
		
			m_ui32GameObjectCount++;
		} // read
	}

	file.Close();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// RESPAWN OBJECT LOAD PROCEDURE
// --------------------------------
// *.i3sRpn에서 리스폰 오브젝트를 로드한다.
//
BOOL CMapData::_LoadRespawnObjects( const char* mapName)
{

	char szPath[MAX_PATH] = { '\0' };

	i3String::Format(szPath, sizeof(szPath), 
										"%s\\%s\\%s.%s", 
										g_pCCurrentPath,
										MEDIA_PATH,
										mapName,
										SRPN_FILE_EXTENSION);
	
	
	
	i3sRespawnFile	file;

	if( INVALID_HANDLE_VALUE == file.Open( szPath, FALSE) )
	{
		DSERROR("[CMapData::_LoadRespawnObjects] File open error: %s\n", szPath );
		return FALSE;
	}
	
	
	UINT32 ui32Count = file.GetCount();
	if( ui32Count > DS_MAX_RESPAWN_OBJECT )
	{
		//I3ASSERT(0); // Dead Town
		DSWARNING("[CMapData::_LoadRespawnObjects] Max respawn object count: %d [%s]\n", ui32Count, mapName );
		ui32Count = DS_MAX_RESPAWN_OBJECT;
	}
	I3TRACE("RESPAWN OBJECT COUNT:%d\n", ui32Count );

	m_ui32TotalRespawnObjectCount = 0;
	
	for( UINT32 i = 0; i < ui32Count; i++)
	{
		if( file.Read( &m_aTotalRespawnObjects[ i ] ) )
		{
			m_ui32TotalRespawnObjectCount++;
		} // read
	}

	file.Close();
	return TRUE;
}

// ----------------------------------------------------
// m_pGameObjects로부터 animation object를 초기화 한다
//
BOOL CMapData::_buildAnimationObjects()
{
	m_ui32AnimationObjectCount = 0;
	
	for (UINT32 i = 0; i < m_ui32GameObjectCount; i++)
	{
		if ( m_pGameObjects[i].m_i16NetType ==  I3_NETWORK_GAMEOBJECT_TYPE_ANIM )
		{
			m_ui32AnimationObjectCount++;
		}
	}

	return BOOL( m_ui32AnimationObjectCount );
}

// ----------------------------------------------------
// m_pGameObjects로부터 controlled object를 초기화 한다
//
BOOL CMapData::_buildControlledObjects()
{
	m_ui32ControlledObjectCount = 0;
	
	for (UINT32 i = 0; i < m_ui32GameObjectCount; i++)
	{
		if ( m_pGameObjects[i].m_iSType ==  OBJECT_S_TYPE_W_BOX )
		{
			m_ui32ControlledObjectCount++;
		}
	}

	return BOOL( m_ui32ControlledObjectCount );
}

void CMapData::_ReleaseActor(NxActor* actor, NxScene* scene) 
{
	// releaseActor 호출 이후에 mesh를 삭제하기 위함
	NxArray< NxConvexMesh*>		convexList;
	NxArray< NxTriangleMesh*>	meshList;
	NxArray< NxCCDSkeleton*>	skeletons;

	if(actor) 
	{
		NxConvexMesh* convex = NULL;
		NxTriangleMesh* mesh = NULL;

		NxU32 n = actor->getNbShapes();
		if (n > 0) 
		{
			for(NxU32 i = 0; i < n; ++i)
			{
				NxShape* shape = actor->getShapes()[i];
				if (shape->isConvexMesh()) 
				{
					convex = &(shape->isConvexMesh()->getConvexMesh());
					convexList.push_back(convex);
				}
				else if(shape->isTriangleMesh()) 
				{
					mesh = &(shape->isTriangleMesh()->getTriangleMesh());
					meshList.push_back(mesh);
				}
				else if( shape->getCCDSkeleton() != NULL )
				{
					skeletons.pushBack(shape->getCCDSkeleton());
				}

				
				actor->releaseShape(*shape);
			}
		}
		
		scene->releaseActor(*actor);

		
		for( NxU32 i = 0; i < convexList.size(); i++)
		{
			if (convexList[i]->getReferenceCount() == 0) 
			{
				gPhysicsSDK->releaseConvexMesh(*convexList[i]);
			}
		}

		for( NxU32 i = 0; i < meshList.size(); i++)
		{
			if (meshList[i]->getReferenceCount() == 0) 
			{
				gPhysicsSDK->releaseTriangleMesh(*meshList[i]);
			}
		}

		for( NxU32 i = 0; i < skeletons.size(); i++)
		{
			if (skeletons[i]->getReferenceCount() == 0) 
			{
				gPhysicsSDK->releaseCCDSkeleton(*skeletons[i]);
			}
		}
	}
}

void CMapData::_ReleaseScene( NxScene* pScene )
{
	if( pScene )
	{
		NxU32 n = pScene->getNbActors();
		NxActor** pa = pScene->getActors();
		for( UINT32 i = 0; i < n; i++ )
		{
			_ReleaseActor( pa[i], pScene);
		}
		
		if( m_pNxSceneQuery )
		{
			pScene->releaseSceneQuery( *m_pNxSceneQuery );
			m_pNxSceneQuery = NULL;
		}
		
		gPhysicsSDK->releaseScene( *pScene );
	}
}

// Get results from m_pNxScene->simulate()
void CMapData::_GetPhysicsResults(void)
{
	if( m_pNxScene )
	{
		while (!m_pNxScene->fetchResults( NX_RIGID_BODY_FINISHED, false));
	}
}

// tirangle mesh actor와 collection 개체를 생성한다
BOOL CMapData::_CreateCollisionObjects( INT32 i32Idx, NxShapeSet* pShapeSet )
{
	if( pShapeSet == NULL) return FALSE;
	if(m_pNxScene == NULL) return FALSE;

	
	NXU::NxuPhysicsCollection * pColl = NULL;
	NxPtr		pNxData = NULL;
	NxActor*	pActor  = NULL;
	
	// initial pose
	NxVec3	   vCenter;
	NxMat34    matCol;
	m_sceneBounds.getCenter(vCenter);

	if( m_bInitWorldMatrix == FALSE )
	{
		m_bInitWorldMatrix = TRUE;
		m_matWorld	= pShapeSet->getTransform();
	}
	
	// build terrain actor
	if( pShapeSet->getNxDataSize() > 0)
	{
		pNxData = pShapeSet->getNxData();
		
		if( pShapeSet->m_ShapeType == I3_PHYSIX_SHAPE_TRIMESH )
		{
			pActor = _BuildTriMeshActor( i32Idx, pShapeSet );
			if( pActor == NULL )
			{				
				DSERROR("Cannot build tri mesh(%s)\n", m_szCodeName );
			}
			else
			{
				INT32 i32Count = pActor->getNbShapes();
				I3ASSERT( i32Count == 1);
				m_ppShapes[ m_ui32CollisionObjectCount ] = pActor->getShapes()[0];
			}
		}
		else
		{
			I3ASSERT(0);
		}

		// actor가 생성되었으면 리턴
		if( pActor ) return TRUE;

		// load ccd collection data 
		NXU::NXU_FileType type = NXU::FT_BINARY;
		pColl = NXU::loadCollection( "PX", type, pNxData, pShapeSet->getNxDataSize());
		if( pColl )
		{
			m_SceneState = LOADED; // 컬렉션 로드 성공
		
			// Meterial 
			NxMaterial * pMtl = _validateMaterial( pShapeSet->getRestitution(), 
												   pShapeSet->getStaticFriction(),
												   pShapeSet->getDynamicFriction());

			m_userNotify.m_MaterialIndex = pMtl->getMaterialIndex();
			m_userNotify.m_pShapeSet   = pShapeSet;
			m_userNotify.m_pCookedMesh = NULL;
			//m_userNotify.m_pCookedMesh = pMesh;
			m_userNotify.m_ShapeGroup  = pShapeSet->getShapeGroup();
		
			// instantiation
			if( NXU::instantiateCollection( pColl, *gPhysicsSDK, m_pNxScene, NULL, &m_userNotify) == false)
			{
				DSERROR("[CMapData::_CreateCollisionObjects] Could not instantiated PhysX data. StageUID:%d\n", m_ui8StageUID );
				NXU::releaseCollection( pColl);
				NXU::releasePersistentMemory();
				return FALSE;
			}
			
			NXU::releaseCollection(pColl);
			NXU::releasePersistentMemory();
			m_SceneState = INSTANTIATED;
			pColl = NULL;
		}
		
		return TRUE;
	}

	DSMSG("NO Collision object\n");
	
	return FALSE;
}

extern NxCookingInterface *gCooking;



NxActor* CMapData::_BuildTriMeshActor( INT32 i32Idx, NxShapeSet* shapeSet )
{
	NxTriangleMeshDesc  desc;
	NxTriangleMesh*		pNxMesh = NULL;
	
	if( IsHardwarePresent() )
	{
		desc.flags |= NX_MF_HARDWARE_MESH;
	}
	
	// vertex list
	desc.numVertices = shapeSet->getVertexCount();
	desc.points = shapeSet->getVertexList();
	desc.pointStrideBytes = sizeof(NxVec3);
	
	// face list
	desc.numTriangles = shapeSet->getFaceCount();
	desc.triangles	  = shapeSet->getFaceList();
	desc.triangleStrideBytes = sizeof(I3_PHYSIX_TRI);
	//desc.triangleStrideBytes = sizeof(NxU32) * 3;
	
	{	// I3_PHYSIX_TRI일 경우에만 사용
		I3_PHYSIX_TRI * pTri = shapeSet->getFaceList();

		desc.materialIndexStride = sizeof(I3_PHYSIX_TRI);
		desc.materialIndices = &pTri->m_MaterialIndex;
	}
	
	
	I3ASSERT( desc.isValid() );
	
	// init cooking
	NxInitCooking(); 
	MemoryWriteBuffer buf;
	// cook mesh
	if( NxCookTriangleMesh( desc, buf ) == false)
	{
		DSERROR( "[CMapData] Could not cook physix triangle mesh\n");
		NxCloseCooking();
		return NULL;
	}
	
	// create mesh
	pNxMesh = gPhysicsSDK->createTriangleMesh( MemoryReadBuffer(buf.data));
	if( pNxMesh == NULL )
	{
		DSERROR( "[CMapData] Cound not create triangle mesh\n");
		NxCloseCooking();
		return NULL;
	}

	NxCloseCooking();

	// save mesh desc.
	pNxMesh->saveToDesc(desc);
	
	// build shape desc.
	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData		= pNxMesh;
	terrainShapeDesc.userData		= &desc;
	terrainShapeDesc.meshPagingMode = NX_MESH_PAGING_AUTO;
	terrainShapeDesc.shapeFlags		|= NX_SF_FEATURE_INDICES;
	I3ASSERT( terrainShapeDesc.isValid() );

	// build actor desc.
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &terrainShapeDesc );
	
	if( ! shapeSet->getTransform().t.isZero() )
	{
		I3TRACE("StageUID=%d, translation:%f,%f,%f\n", m_ui8StageUID, shapeSet->getTransform().t.x, shapeSet->getTransform().t.y, shapeSet->getTransform().t.z );
	}

	if( i32Idx == 0 )
	{
		// translation에만 world matrix 적용
		NxVec3 trans = m_matWorld.M * shapeSet->getTransform().t; 
		actorDesc.globalPose    = shapeSet->getTransform();
		actorDesc.globalPose.t	= trans;
	}		
	else
	{
		actorDesc.globalPose	 = m_matWorld * shapeSet->getTransform();
		actorDesc.globalPose.t	-= m_matWorld.t;
	}
	
	I3ASSERT( actorDesc.isValid());

	// create actor
	return m_pNxScene->createActor(actorDesc);
}


NxMaterial * CMapData::_validateMaterial( NxF32 restitution, NxF32 sFriction, NxF32 dFriction)
{	
	NxMaterialIndex i;

	if( m_pNxScene == NULL)
		return NULL;

	for( i = 0; i <= m_pNxScene->getHighestMaterialIndex(); i++)
	{
		NxMaterial * pMtl = m_pNxScene->getMaterialFromIndex( i);

		if( pMtl != NULL)
		{
			if(	(pMtl->getRestitution() == restitution) &&
				(pMtl->getStaticFriction() == sFriction) &&
				(pMtl->getDynamicFriction() == dFriction))
			{
				return pMtl;
			}
		}
	}

	NxMaterialDesc desc;

	desc.setToDefault();

	desc.restitution = restitution;
	desc.staticFriction = sFriction;
	desc.dynamicFriction = dFriction;
	
	return _createMaterial( &desc);
}


NxMaterial * CMapData::_createMaterial( NxMaterialDesc * pDesc)	
{ 
	if( m_pNxScene == NULL)		return NULL;
	return m_pNxScene->createMaterial( (const NxMaterialDesc&)(*pDesc) ); 
}

void CMapData::Step_M()
{
	if( m_SceneState == STARTED )
	{
		_GetPhysicsResults();
		_ProcessInputs();
		_StartPhysics();
	}

}

void CMapData::_ProcessInputs(void)
{
#ifdef USE_VRD
	if (m_pNxScene)
	{
		
		gDebugRenderer.renderData(*m_pNxScene->getDebugRenderable());
		
	}
#endif
}
void CMapData::_StartPhysics(void)
{
	// Update the time step
	REAL32 r32DeltaTime = _UpdateTime();

	if(m_pNxScene)
	{
		// Start collision and dynamics for delta time since the last frame
		m_pNxScene->simulate( r32DeltaTime );
		m_pNxScene->flushStream();
	}
}

REAL32	CMapData::_UpdateTime()
{
	DWORD dwCurrentTime = GetTickCount();
	DWORD dwElapsedTime = dwCurrentTime - m_dwPreviousTime;
	m_dwPreviousTime = dwCurrentTime;
	return (REAL32)(dwElapsedTime)*0.001f;
}


BOOL	CMapData::EnqueueRaycastRequest( INT32 i32GroupIdx, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pFinalHit )
{
	if( FALSE == m_bPhysicsActive )	return TRUE;

	if( g_pConfig->GetCheckLevel(HACK_TYPE_WALLSHOT) )
	{
		CDediRoom* pRoom = g_pRoomManager->GetGroup( i32GroupIdx );
		if( ! pRoom || !pRoom->IsInBattle() ) return FALSE; 

		// make packet for hack checker
		PHYSICS_PACKET_HEADER head;
		head.m_uid				= pRoom->GetUID(ui32SlotIdx);
		head.m_i16GroupIdx		= (INT16)i32GroupIdx;
		head.m_ui8SlotIdx		= (UINT8)ui32SlotIdx;
		head.m_ui8StageUID		= (UINT8)pRoom->GetStageUID();
		head.m_ui8RoundNum		= pRoom->GetRoundNumber();
		head.m_ui8RespawnCount	= (UINT8)pRoom->GetRespawnCount( ui32SlotIdx );

		// insert to req queue
		return m_pRequestQueue->AddTail(&head, pFinalHit );
	}

	return TRUE;
}

DWORD WINAPI CMapData::RaycastThread(LPVOID lpParameter)
{
	I3ASSERT( g_hevtQuit != NULL );
	I3ASSERT( g_hevtQuit != INVALID_HANDLE_VALUE );
	I3ASSERT( g_pLogFile != NULL );
	
	RAYCAST_THREAD_PARAM param;
	memcpy_s( &param, sizeof(param), lpParameter, sizeof( RAYCAST_THREAD_PARAM ));
	delete (RAYCAST_THREAD_PARAM*)lpParameter;

#ifdef NEW_LOG
	//alloc private log buffer
	I3TRACE("RaycastThread=%d\n", GetCurrentThreadId() ); 
	if( NULL == g_pLogFile->AllocBuffer( GetCurrentThreadId() ) )
	{
		goto exit_loop;
	}
#endif 
	
	HANDLE	hQuitEvent	= g_hevtQuit;
	DWORD	wo			= 0;

	UINT32  ui32PollingTime = g_pConfig->m_ui32PhysicsUpdateTime;
	UINT32	ui32EpsilonTime = g_pConfig->m_ui32PhysicsEpsilonTime;
	DWORD	dwUpdateTic, dwLastUpdateTic, dwSleepTime; 
	
	dwUpdateTic		= dwLastUpdateTic = 0; 
	dwLastUpdateTic = GetTickCount(); 

	DWORD dwNowTime; 
	while( 1 )
	{
		dwNowTime		= GetTickCount(); 
		dwUpdateTic		= dwNowTime - dwLastUpdateTic; 
		dwLastUpdateTic = dwNowTime; 

		if(dwUpdateTic > ui32PollingTime) 
		{
			dwSleepTime = 0; 
			//DSMSG("Update TimeOut %d \n", dwUpdateTic); 
		}
		else 
		{
			dwSleepTime = ui32PollingTime - dwUpdateTic;
		}

		if( dwSleepTime > ui32EpsilonTime )
		{
			if( dwSleepTime == ui32PollingTime )
			{
				dwSleepTime -= ui32EpsilonTime;
			}
			
			wo = WaitForSingleObject( hQuitEvent, dwSleepTime);
			if( wo == WAIT_OBJECT_0 ) goto exit_loop;
			
		}
		
		OnUpdate( &param );
	}

exit_loop:
	
	I3TRACE("Task Processor (0x%x) Exited...\n", GetCurrentThreadId());
	 
	return 0;
}

// process ray quest request
void	CMapData::OnUpdate( RAYCAST_THREAD_PARAM* pParam )
{
	CMapData* pMap = g_pMapManager->GetMap( pParam->m_eStageUID );
	if( pMap == NULL ) return;
	
	CInterlockedList* pRequestQueue = pParam->m_pRequestQueue;
	UINT16 nCount = pRequestQueue->GetCount();
	for( UINT16 i = 0; i < nCount; i++ )
	{
		PHYSICS_ITEM*	pItem = pRequestQueue->RemoveHead();
		if ( pItem != NULL )
		{
			ProcessRaycastRequest( pMap, pItem );
			pRequestQueue->DeleteEntry((SLIST_ENTRY*) pItem );
		}
	}
}

void CMapData::ProcessRaycastRequest( CMapData* pMap, PHYSICS_ITEM* pItem ) 
{
	I3TRACE("Process...\n");

	PHYSICS_PACKET_HEADER*		pHead	 = &pItem->header;
	N_PCINFO_HIT_DEDICATED2*	pHitInfo = &pItem->hit;
	//STAGEORD	stageOrd = (STAGEORD)pHead->m_ui8StageOrd;
	
	NxRay		 ray;
	NxRaycastHit hit;
	
	UINT32 ui32WallShotCount = 0;
	UINT32 ui32RayCount = g_pConfig->m_ui32RayCount;
	
	NxVec3 vFire	= NxVec3( pHitInfo->_stFirePos.x,  pHitInfo->_stFirePos.y, pHitInfo->_stFirePos.z );
	NxVec3 vTarget	= NxVec3( pHitInfo->_HitPos.x,   pHitInfo->_HitPos.y,  pHitInfo->_HitPos.z );
	NxReal fDistanceSquare = 0.0f;
	NxVec3 vOffset;  
	
	I3TRACE("vFire  :(%f,%f,%f)\n",	vFire.x, vFire.y, vFire.z );
	I3TRACE("vTarget:(%f,%f,%f)\n",	vTarget.x, vTarget.y, vTarget.z );
	
	//---------------------------------
	// reverse direction( target->fire )
	//---------------------------------

	NxVec3 Eye( vFire.x - vTarget.x, vFire.y - vTarget.y, vFire.z - vTarget.z );
	NxVec3 N = Eye.cross( NxVec3( 0.0f, 1.0f, 0.0f) );
	if( ! IsFiniteNumber( (float)N.normalize() )) return;

	for( UINT32 i = 0; i < ui32RayCount; i++ ) 
	{
		if( ui32RayCount == 1 )
		{
			vOffset.zero();
		}
		else if( ui32RayCount == 3) // 상하로 레이를 2개 생성한다.
		{
			switch( i )
			{
			case 0: // Y + 0.5
				vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
			case 1: // Y - 0.5
				vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
			case 2: // ORIGINAL
				vOffset = NxVec3( 0.0f,  0.0f, 0.0f);	break;
			} // switch
		}
		else if( ui32RayCount == 5) // 상하좌우로 추가 레이를 4개 생성한다.
		{
			switch( i )
			{
			case 0: // Y + 1
				vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
			case 1: // Y - 1
				vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
			case 2: // +N
				vOffset =  N * 0.5f; break;
			case 3: // -N
				vOffset =  N * -0.5f; break;
			case 4: // ORIGINAL
				vOffset.zero(); break;
			} // switch
		}
		else
		{
			I3ASSERT(0);
			return;
		}

		// move start point
		NxVec3 vStart = vFire + vOffset;
	
		// build ray
		ray.orig = NxVec3( vStart.x, vStart.y, vStart.z );
		ray.dir	 = vTarget - vStart;
		if( ! IsFiniteNumber( (float)ray.dir.normalize())) return;
		
		fDistanceSquare = vStart.distanceSquared( vTarget );
		if( ! IsFiniteNumber( fDistanceSquare ) ) return;	

		if( ! pMap->CheckRaycastHit( ray, fDistanceSquare ) )
		{
			ui32WallShotCount++;
#ifdef _DEBUG
			DSERROR("[F->T] WallShotCount++!! count = %d, ray case = %d, ray.orig( %.2f, %.2f, %.2f )\n", ui32WallShotCount, i, vStart.x, vStart.y, vStart.z);
#endif
		}
	}
	
	//---------------------------------
	// Forward direction( fire->target )
	//---------------------------------
	if( g_pConfig->m_bBothDirection )
	{
		Eye *= -1;
		N = Eye.cross( NxVec3( 0.0f, 1.0f, 0.0f) );
		if( ! IsFiniteNumber( (float)N.normalize() )) return;

		for( UINT32 i = 0; i < ui32RayCount; i++ ) 
		{
			if( ui32RayCount == 1 )
			{
				vOffset.zero();
			}
			else if( ui32RayCount == 3) // 상하로 레이를 2개 생성한다.
			{
				switch( i )
				{
				case 0: // Y + 0.5
					vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
				case 1: // Y - 0.5
					vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
				case 2: // ORIGINAL
					vOffset = NxVec3( 0.0f,  0.0f, 0.0f);	break;
				} // switch
			}
			else if( ui32RayCount == 5) // 상하좌우로 추가 레이를 4개 생성한다.
			{
				switch( i )
				{
				case 0: // Y + 1
					vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
				case 1: // Y - 1
					vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
				case 2: // +N
					vOffset =  N * 0.5f; break;
				case 3: // -N
					vOffset =  N * -0.5f; break;
				case 4: // ORIGINAL
					vOffset.zero(); break;
				} // switch
			}
			else
			{
				I3ASSERT(0);
				return;
			}

			// move start point
			NxVec3 vStart = vTarget + vOffset;
		
			// build ray
			ray.orig = NxVec3( vStart.x, vStart.y, vStart.z );
			ray.dir	 = vFire - vStart;
			if( ! IsFiniteNumber( (float)ray.dir.normalize() )) return;

			fDistanceSquare = vStart.distanceSquared( vFire );
			if( !IsFiniteNumber( fDistanceSquare )) return;
			
			if( !pMap->CheckRaycastHit( ray, fDistanceSquare ))
			{
				ui32WallShotCount++;
#ifdef _DEBUG
				DSERROR("[T->F] WallShotCount++!! count = %d, ray case = %d, ray.orig( %.2f, %.2f, %.2f )\n", ui32WallShotCount, i, vStart.x, vStart.y, vStart.z);
#endif
			}
		}
	}

	BOOL bHack = FALSE;
	if( g_pConfig->m_bBothDirection )
	{
		bHack = ( ui32WallShotCount >= (ui32RayCount*2) );
		I3TRACE( "wall shot count = %d\n", ui32WallShotCount );
	}
	else
	{
		bHack = ( ui32WallShotCount == ui32RayCount );
	}

	if( bHack )
	{
		//yjha 로그 수정할 것(2013-01-04)
		DSERROR("WALL SHOT: stage=%d, group=%d, user=%d fire:(%.2f, %.2f, %.2f) hit:(%.2f, %.2f, %.2f)\n",
				pHead->m_ui8StageUID, pHead->m_i16GroupIdx, pHead->m_ui8SlotIdx, 
				vFire.x,  vFire.y, vFire.z,
				vTarget.x, vTarget.y, vTarget.z );

		{
#ifdef _DEBUG
			DS_CHECK_WALLSHOT_RESULT result;
			
			result.m_ui32Result			= HACK_TYPE_WALLSHOT;
			result.m_ui8ByteUID			= (UINT8)(pHead->m_uid & 0xff);
			result.m_ui8SlotIdx			= pHead->m_ui8SlotIdx;
			result.m_ui8RoundNum		= pHead->m_ui8RoundNum;
			result.m_ui8RespawnCount	= pHead->m_ui8RespawnCount;
#endif		
			CDediRoom* pRoom = g_pRoomManager->GetGroup( pHead->m_i16GroupIdx );

			char  szExt[MAX_PATH];
			HACK_LOG hl;
			sprintf_s( szExt, sizeof(szExt), "HACK_TYPE_WALLSHOT\t Fire:(%.2f, %.2f, %.2f)\t Hit:(%.2f, %.2f, %.2f)\n", vFire.x,  vFire.y, vFire.z,vTarget.x, vTarget.y, vTarget.z);
			if( pRoom && pRoom->IsInBattle() )
			{
#ifdef _DEBUG	// 디버그 모드일때만 kick
				pRoom->WriteWallShotResult( &result ); 
#endif
				hl.AddInfo("HACK_TYPE_WALLSHOT\t Fire:(%.2f, %.2f, %.2f)\t Hit:(%.2f, %.2f, %.2f)\t Count:%d\n", vFire.x, vFire.y, vFire.z, vTarget.x, vTarget.y, vTarget.z, ui32WallShotCount);
				char* msg = hl.MakeLogString( pRoom, 	pHead->m_ui8SlotIdx, HACK_TYPE_WALLSHOT );
				if( msg) DSHACK(msg);
			}
			else
				DSHACK(szExt);

			return;
		}
	}
	// 월샷이 아니면 아무일도 하지 않음.
	return;
}


#ifdef _DEBUG
static char* s_TERRAIN_TYPE_NAME[] = {
	"I3_TERRAIN_NA",							// Not-Assigned
	"I3_TERRAIN_CONCRETE",				// 콘크리트
	"I3_TERRAIN_STEEL",						// 철제
	"I3_TERRAIN_GROUND",						// 흙
	"I3_TERRAIN_WOOD",						// 나무
	"I3_TERRAIN_SNOW",						// 눈
	"I3_TERRAIN_WATER_DEEP",					// 물 (깊은)
	"I3_TERRAIN_WATER_SHALLOW",				// 물 (얕은)
	"I3_TERRAIN_WET_CLAY",					// 진흙
	"I3_TERRAIN_GRASS",						// 잔디
	"I3_TERRAIN_MARBLE",						// 대리석
	"I3_TERRAIN_FALLLEAF",					// 나뭇잎
	"I3_TERRAIN_CONCRETE_THIN",				// 얇은 콘크리트
	"I3_TERRAIN_STEEL_THIN",					//
	"I3_TERRAIN_WOOD_THIN",
	"I3_TERRAIN_MARBLE_THIN",
	"I3_TERRAIN_PAPER",						// 종이
	"I3_TERRAIN_GLASS",						// 유리
	"I3_TERRAIN_PLASTIC",						// 플라스틱
	"I3_TERRAIN_RUBBER",						// 고무
	"I3_TERRAIN_CLOTH",						// 천
	"I3_TERRAIN_GROUND_THIN",
	"I3_TERRAIN_SNOW_THIN",
	"I3_TERRAIN_WET_CLAY_THIN",
	"I3_TERRAIN_GRASS_THIN",
	"I3_TERRAIN_PAPER_THIN",
	"I3_TERRAIN_FALLLEAF_THIN",
	"I3_TERRAIN_GLASS_THIN",
	"I3_TERRAIN_PLASTIC_THIN",
	"I3_TERRAIN_RUBBER_THIN",
	"I3_TERRAIN_CLOTH_THIN",
	"I3_TERRAIN_BARBEDWIRE",

	"I3_TERRAIN_BLOOD",						// 피
	"I3_TERRAIN_TEMP1",						// 임시 1 ( 이것은 리니지)
	"I3_TERRAIN_TEMP2",						// 임시 2

	"I3_TERRAIN_CLOTH_NO_DECAL" 
};

static const char* GetTerrainTypeName( UINT32 ui32TerrainType )
{
	if( ui32TerrainType < I3_TERRAIN_TYPE_COUNT )
	{
		return s_TERRAIN_TYPE_NAME[ui32TerrainType];
	}
	return s_TERRAIN_TYPE_NAME[0];
}

#endif

BOOL	CMapData::CheckRaycastHit( NxRay& ray, NxReal fDistanceSquared )
{
	NxRaycastHit*	pHit   = _RaycastClosestShapes( ray );
	NxShape*		pShape = pHit->shape;
	
	NxShapeSet*		pShapeSet = NULL;
	I3_PHYSIX_TERRAIN* pTerrain = NULL;
	UINT16	nTerrainType = I3_TERRAIN_CONCRETE;
	BOOL	bPenetration = FALSE;
		
	if( pShape != NULL)
	{
		
		// find NxShapeSet
		UINT32 i = 0;
		for( i = 0; i < m_ui32CollisionObjectCount; i++ )
		{
			if( m_ppShapes[i] == pShape )
			{
				pShapeSet = m_ppCollisionObjects[i];
			}
		}
		
		if( pShapeSet )
		{
			pTerrain = pShapeSet->getTerrain( pHit->materialIndex );
			if( pTerrain )
			{
				// WeaponBase::_CheckPenetration 참조

				// 재질
				nTerrainType = pTerrain->m_Terrain;
				if( nTerrainType < I3_TERRAIN_CONCRETE_THIN )
				{
					bPenetration = FALSE;
				}
				else if( ((nTerrainType > I3_TERRAIN_MARBLE_THIN) &&	(nTerrainType < I3_TERRAIN_GROUND_THIN)) ||
					((nTerrainType > I3_TERRAIN_BARBEDWIRE) && (nTerrainType < I3_TERRAIN_CLOTH_NO_DECAL)) )
				{
					bPenetration = FALSE;
				}
				else if( pTerrain->m_Thickness <= 10)
				{
					// 두께가 아래보다 작으면 무조건 관통
					bPenetration = TRUE;
				}
				else
				{
					// 관통 확률에 의해서 관통될 수 있는 경우에는 관통된 것으로 합니다(해킹으로 판단하지 않음)
					bPenetration = TRUE;
				}
			}
			else
			{
				DSMSG("Material Info Not Found!(StageUID:%d)\n", m_ui8StageUID );
				return TRUE; // FALSE: wall shot
			}
		}

	
#ifdef WALLSHOT_DEBUG
		//fDistanceSquared += 100.0; // 월샷 테스트용 (캐릭터 사이의 거리를 늘려본다)
		
#endif 

		if( pHit->distance * pHit->distance < fDistanceSquared )
			return bPenetration;
	}

	return TRUE;
}

NxRaycastHit* CMapData::_RaycastClosestShapes( NxRay& ray )
{
	// 위치 표시 용 (삭제하지 마시오)
	//#ifdef _DEBUG
	//	CreateCube( m_pNxScene, ray.orig, 1, &NxVec3(0.0f, 0.0f, 0.0f) );
	//#endif
	
	NxRaycastHit hit;
	
	m_pNxSceneQuery->raycastClosestShape( ray, NX_ALL_SHAPES, hit, 0xffffffff, NX_MAX_F32, 0xffffffff, NULL, NULL, (void*)(0));
	m_pNxSceneQuery->execute();
	m_pNxSceneQuery->finish(true);

	if( m_hits[0].shape != NULL )
	{
	//	DSMSG("[PHYS] RAYCAST OK. distance=%f\n", m_hits[0].distance);
	}
	else
	{
		DSMSG("[PHYS] RAYCAST MISSING.\n");
	}

	return &m_hits[0];
}

#ifdef _DEBUG
void CreateCube(NxScene* pScene, const NxVec3& posMin, const NxVec3& posMax, const NxVec3* initial_velocity)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.0f;
	BodyDesc.flags |= NX_BF_FROZEN;
	if(initial_velocity) BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions = NxVec3(posMax.x - posMin.x + 1, posMax.y - posMin.y + 1, posMax.x - posMin.z + 1);

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = NxVec3((posMax.x + posMin.x)/2.0f, (posMax.y + posMin.y)/2.0f, (posMax.z + posMin.z)/2.0f);

	NxActor* a = pScene->createActor(ActorDesc);
	if(a)
	{
		//a->userData = (void*)(size<<1);
	}
}
#endif
