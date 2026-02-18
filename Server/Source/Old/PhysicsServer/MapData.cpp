// MapData.cpp
//
// YouJong Ha
//	Last update : 2011-09-02 (yyyy:mm:dd)
//	
// Description:
//	CMapData.h 구현 파일
//
//
// Dependency: 
//		NxShapeSet/i3scolFile
//		
#include "pch.h"
#include "i3scolFile_s.h"
#include "MapData.h"
#include "MapMgr.h"
#include "LogFile.h"

#define WALLSHOT_DEBUG  1
extern void g_CommonGetMapBounds( STAGE_ORDINAL_ID id, REAL32* pBound );
extern  TCHAR g_pCCurrentPath[SERVER_STRING_COUNT];
void	InitTerrain(NxScene* pScene);

// boundary table
static NxBounds3 s_StageBounds[ STAGE_ORD_MAX ];

void CreateCube(NxScene* pScene, const NxVec3& posMin, const NxVec3& posMax, const NxVec3* initial_velocity=NULL);

void g_InitMapBounds()
{
	NxReal Boundry[ MAX_BOUNDRY_NAME ];

	g_CommonGetMapBounds( STAGE_ORD_PORT_AKABA,		Boundry ); s_StageBounds[ STAGE_ORD_PORT_AKABA		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_RED_ROCK,		Boundry ); s_StageBounds[ STAGE_ORD_RED_ROCK		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_LIBRARY,		Boundry ); s_StageBounds[ STAGE_ORD_LIBRARY			].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_MSTATION,		Boundry ); s_StageBounds[ STAGE_ORD_MSTATION		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_MIDNIGHT_ZONE,	Boundry ); s_StageBounds[ STAGE_ORD_MIDNIGHT_ZONE	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_UPTOWN,			Boundry ); s_StageBounds[ STAGE_ORD_UPTOWN			].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_BURNINGHALL,	Boundry ); s_StageBounds[ STAGE_ORD_BURNINGHALL		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DSQUAD,			Boundry ); s_StageBounds[ STAGE_ORD_DSQUAD			].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_CRACKDOWN,		Boundry ); s_StageBounds[ STAGE_ORD_CRACKDOWN		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_SAINTMANSION,	Boundry ); s_StageBounds[ STAGE_ORD_SAINTMANSION 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_EASTERNROAD,	Boundry ); s_StageBounds[ STAGE_ORD_EASTERNROAD		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_DOWNTOWN,		Boundry ); s_StageBounds[ STAGE_ORD_D_DOWNTOWN		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_LUXVILLE,		Boundry ); s_StageBounds[ STAGE_ORD_D_LUXVILLE		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_BLOWCITY,		Boundry ); s_StageBounds[ STAGE_ORD_D_BLOWCITY		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_STORMTUBE,	Boundry ); s_StageBounds[ STAGE_ORD_D_STORMTUBE		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_GIRAN2,		Boundry ); s_StageBounds[ STAGE_ORD_D_GIRAN2		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_BREAKDOWN,	Boundry ); s_StageBounds[ STAGE_ORD_D_BREAKDOWN 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_TRAININGCAMP,	Boundry ); s_StageBounds[ STAGE_ORD_TRAININGCAMP 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_SENTRYBASE,	Boundry ); s_StageBounds[ STAGE_ORD_D_SENTRYBASE 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DESERTCAMP,		Boundry ); s_StageBounds[ STAGE_ORD_DESERTCAMP 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_KICKPOINT,		Boundry ); s_StageBounds[ STAGE_ORD_KICKPOINT 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_FACEROCK,		Boundry ); s_StageBounds[ STAGE_ORD_FACEROCK 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_SUPPLYBASE,		Boundry ); s_StageBounds[ STAGE_ORD_SUPPLYBASE 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_SANDSTORM,	Boundry ); s_StageBounds[ STAGE_ORD_D_SANDSTORM 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DOWNTOWN,		Boundry ); s_StageBounds[ STAGE_ORD_DOWNTOWN 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_LUXVILLE,		Boundry ); s_StageBounds[ STAGE_ORD_LUXVILLE 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_OUTPOST,		Boundry ); s_StageBounds[ STAGE_ORD_OUTPOST 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_BLOWCITY,		Boundry ); s_StageBounds[ STAGE_ORD_BLOWCITY 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_STORMTUBE,		Boundry ); s_StageBounds[ STAGE_ORD_STORMTUBE 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_SENTRYBASE,		Boundry ); s_StageBounds[ STAGE_ORD_SENTRYBASE 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_HOSPITAL,		Boundry ); s_StageBounds[ STAGE_ORD_HOSPITAL 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DOWNTOWN2,		Boundry ); s_StageBounds[ STAGE_ORD_DOWNTOWN2 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_SHOPPINGCENTER,	Boundry ); s_StageBounds[ STAGE_ORD_SHOPPINGCENTER 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_SANDSTORM,		Boundry ); s_StageBounds[ STAGE_ORD_SANDSTORM 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_BREAKDOWN,		Boundry ); s_StageBounds[ STAGE_ORD_BREAKDOWN 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_GIRAN,			Boundry ); s_StageBounds[ STAGE_ORD_GIRAN 			].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_GIRAN2,			Boundry ); s_StageBounds[ STAGE_ORD_GIRAN2 			].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_HELISPOT,		Boundry ); s_StageBounds[ STAGE_ORD_HELISPOT 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_BLACKPANTHER,	Boundry ); s_StageBounds[ STAGE_ORD_BLACKPANTHER 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_BREEDING_NEST,	Boundry ); s_StageBounds[ STAGE_ORD_BREEDING_NEST 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_UPTOWN,		Boundry ); s_StageBounds[ STAGE_ORD_D_UPTOWN 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DINO_BREAKDOWN,	Boundry ); s_StageBounds[ STAGE_ORD_DINO_BREAKDOWN 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	//s_StageBounds[ STAGE_ORD_DEADTOWN 		].set(); // 예외처리 해야 함.							 
	//s_StageBounds[ STAGE_ORD_TUTORIAL 		].set();												 
	g_CommonGetMapBounds( STAGE_ORD_D_SHOPPINGCENTER,Boundry ); s_StageBounds[STAGE_ORD_D_SHOPPINGCENTER].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_D_SAFARI,		Boundry ); s_StageBounds[ STAGE_ORD_D_SAFARI 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_DRAGONALLEY,	Boundry ); s_StageBounds[ STAGE_ORD_DRAGONALLEY 	].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
	g_CommonGetMapBounds( STAGE_ORD_MACHUPICHU,		Boundry ); s_StageBounds[ STAGE_ORD_MACHUPICHU 		].set( Boundry[ X_START_BOUNDRY ], Boundry[ Y_START_BOUNDRY ], Boundry[ Z_START_BOUNDRY ], Boundry[ X_END_BOUNDRY ], Boundry[ Y_END_BOUNDRY ], Boundry[ Z_END_BOUNDRY ] );
};


CMapData::CMapData( STAGEORD stageOrd )
{
	m_stageOrd = stageOrd;									

	// stage collision object
	memset( m_ppCollisionObjects,	0, sizeof(m_ppCollisionObjects) );	// pointer array
	m_ui32CollisionObjectCount  = 0;
	// physics state
	m_pNxScene			= NULL;							// simulation world
	m_pNxSceneQuery		= NULL;							// sceneQueryObject
	// time
	m_dwPreviousTime	= GetTickCount();
	// matrix
	m_matWorld.id();
	m_bInitWorldMatrix = FALSE;
	m_SceneState = NONE;					// release flag. collection 오브젝트의 포인터를 포관하지 않기 때문에 필요하다.
	// update thread
	m_hThreadUpdate		= NULL;
	
	m_queryReport.SetOutputBuffer(&m_hits[0], 1);

}

CMapData::~CMapData()
{
	if( m_pNxScene )
	{
		if( m_SceneState == STARTED )
		{
			GetPhysicsResults();
		}
		
		if ( m_SceneState >= LOADED )
		{
			NXU::NxuPhysicsCollection *c = NXU::extractCollectionScene(m_pNxScene);
			NXU::releaseCollection( c );
		}
		
		_ReleaseScene( m_pNxScene );
		m_pNxScene = NULL;
	}
	
	for( UINT32 i = 0; i < m_ui32CollisionObjectCount; i++ )
	{
		SAFE_DELETE( m_ppCollisionObjects[i] );
	}

	I3TRACE("STAGE ORD:%d DESTURCTED!\n", m_stageOrd );
}

/////////////////////////////////////////////////////////////////////////////////////////
// SHARED RESOURCE LOADING PROCEDURE
// ----------------------------------
BOOL CMapData::Load( const char* pszMapName )
{
	if( m_stageOrd < 0 || m_stageOrd >= STAGE_ORD_MAX )
	{
		DSERROR("[MapData] Invalid Stage Ord[%d:%s]\n", m_stageOrd, pszMapName );
		return FALSE;
	}
	
	// scene Bounds
	m_sceneBounds = s_StageBounds[ m_stageOrd ];
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
	if( _CreateScene() == FALSE )
	{
		DSERROR("[MapData]CreateScene  Fail:[%d:%s]\n", m_stageOrd, pszMapName );
		return FALSE;
	}
	
	// load collision objects
	if( _LoadCollisionObjects( pszMapName ) == FALSE ) 
	{
		DSERROR("[MapData]LoadCollisionObjects Fail:[%d:%s]\n", m_stageOrd, pszMapName);
		return FALSE;
	}

	// 중력햄 범위 확인용 (삭제하지 마시오)
	//CreateCube( m_pNxScene, s_StageBounds[ m_stageOrd ].min, s_StageBounds[ m_stageOrd ].max, &NxVec3(0.0f, 0.0f, 0.0f) );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// COLLISION OBJECT LOAD PROCEDURE
// --------------------------------
// *.i3col에서 collision 데이터를 로드한다. 
BOOL CMapData::_LoadCollisionObjects(const char* mapName)
{
	char szPath[MAX_PATH];
	sprintf_s(szPath, sizeof(szPath), 
										"%s\\%s\\%s.%s", 
										g_pCCurrentPath,
										MEDIA_PATH,
										mapName,
										SCOL_FILE_EXTENSION);

	i3scolFile	file;

	if( INVALID_HANDLE_VALUE == file.Open(szPath, FALSE))
		return FALSE;
	
	int nCount = file.GetCountOfCollisionObject();
	if( nCount >= DS_MAX_ACTORS ) return FALSE;

	m_ui32CollisionObjectCount = 0;
	
	m_vMove.zero();
	for (int i = 0; i < nCount; i++)
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

	file.Close();
	
	return TRUE;
}

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
		a->userData = (void*)(size<<1);
	}
}

// create scene
BOOL CMapData::_CreateScene()
{
	// Create a scene
	NxSceneDesc sceneDesc;
	NxVec3		DefaultGravity(0.0f, -9.8f, 0.0f);
	sceneDesc.setToDefault();

	if( IsHardwarePresent() )
	{
 		sceneDesc.simType			 = NX_SIMULATION_HW;
	}

	sceneDesc.gravity				 = DefaultGravity;
	sceneDesc.backgroundThreadCount	 = 1;
	if( m_stageOrd != STAGE_ORD_DEADTOWN && m_stageOrd != STAGE_ORD_TUTORIAL )
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
			DSERROR("[CMapData::_CreateScene] Unable to create a PhysX scene(StageOrd:%d)\n", m_stageOrd );
			return FALSE;
		}
		else
		{
			DSERROR("[CMapData::_CreateScene] NX_SIMULATION_SW Performance Warning(StageOrd:%d)\n", m_stageOrd );
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
		DSERROR("[CMapData::_CreateScene] Unable to create a PhysX scene query (StageOrd:%d)\n", m_stageOrd);
		return FALSE;
	}

	// yjha test code
	//InitTerrain( m_pNxScene );
	
	return TRUE;
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
				char szCodeName[MAX_PATH] = { '\0', };
				::getStageCodeName( szCodeName, (INT32)m_stageOrd );
				DSERROR("Cannot build tri mesh(%s)\n", szCodeName );
			}
			else
			{
				int nCount = pActor->getNbShapes();
				I3ASSERT( nCount == 1);
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
				DSERROR("[CMapData::_CreateCollisionObjects] Could not instantiated PhysX data. ID:%d\n", m_stageOrd );
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
		I3TRACE("stageord=%d, translation:%f,%f,%f\n", m_stageOrd, shapeSet->getTransform().t.x, shapeSet->getTransform().t.y, shapeSet->getTransform().t.z );
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


void CMapData::step_T()
{

	if( m_SceneState == STARTED )
	{
		GetPhysicsResults();
		ProcessInputs();
		StartPhysics();
	}

}

// Get results from m_pNxScene->simulate()
void CMapData::GetPhysicsResults(void)
{
	if( m_pNxScene )
	{
		while (! m_pNxScene->fetchResults( NX_RIGID_BODY_FINISHED, false));
	}
}
void CMapData::ProcessInputs(void)
{
#ifdef USE_VRD
	if (m_pNxScene)
	{
		
		gDebugRenderer.renderData(*m_pNxScene->getDebugRenderable());
		
	}
#endif
}
void CMapData::StartPhysics(void)
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

REAL32	CMapData::_UpdateTime()
{
	DWORD dwCurrentTime = GetTickCount();
	DWORD dwElapsedTime = dwCurrentTime - m_dwPreviousTime;
	m_dwPreviousTime = dwCurrentTime;
	return (REAL32)(dwElapsedTime)*0.001f;
}

BOOL	CMapData::Start()
{
	if( m_SceneState == INSTANTIATED )
	{
		m_dwPreviousTime = GetTickCount();
		StartPhysics();
		m_SceneState = STARTED;
		return TRUE;
	}
	return FALSE;	
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
		DSMSG("[PHYS] RAYCAST OK. distance=%f\n", m_hits[0].distance);
	}
	else
	{
		DSMSG("[PHYS] RAYCAST MISSING.\n");
	}

	return &m_hits[0];
}

BOOL	CMapData::CheckPos( NxVec3& pos )
{
	// tutorial, deadtown 일 경우에는 체크하지 말아야 한다.
	if( m_stageOrd == STAGE_ORD_DEADTOWN ||
		m_stageOrd == STAGE_ORD_TUTORIAL 
	)
		return TRUE;
	
	if( m_sceneBounds.contain(pos) == FALSE )
	{
		I3TRACE("[PHYS] CheckPos HACK. StageOrd:%d x=%f,y=%f,z=%f\n", m_stageOrd, pos.x, pos.y, pos.z );
		return FALSE;
	}
	return TRUE;
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
				DSMSG("Material Info Not Found!(stage ord:%d)\n", m_stageOrd );
				return TRUE; // FALSE: wall shot
			}
		}

	
#ifdef WALLSHOT_DEBUG
		//fDistanceSquared += 100.0; // 월샷 테스트용 (캐릭터 사이의 거리를 늘려본다)
		//I3TRACE("Material : %s\n", GetTerrainTypeName( nTerrainType ) );
#endif 

		if( pHit->distance * pHit->distance < fDistanceSquared )
		{
			if( bPenetration )
			{
#ifdef _DEBUG
				I3TRACE("PENETRATED: Material : %s\n", GetTerrainTypeName( nTerrainType ) );
#endif
				return TRUE;
			}
			return FALSE; //wall shot
		}
	}
	return TRUE;
}