#pragma once

#include "NxGlobal.h"

enum PX_SCENE_STATE
{
	NONE = 0,
	LOADED,
	INSTANTIATED,
	STARTED
};


// shared resource를 유지한다
class CMapData
{
public:
	NxMat34&			GetWorldMatrix()		{	return m_matWorld; }
	BOOL				CheckPos( NxVec3&	pos );
	BOOL				CheckRaycastHit( NxRay& ray, NxReal fDistanceSquared );
	// ------------------------------------------------
	// Objects initialize
	// ------------------------------------------------
	BOOL				Load( const char* pszMapName );
	BOOL				Start();
	// ------------------------------------------------
	// Task Processor interface functions
	// ------------------------------------------------
	void				step_T();					// called by UpdateThread(self)
	
	// ------------------------------------------------
	// Group interface functions
	// ------------------------------------------------
	NxScene*			GetScene()					{	return m_pNxScene;					}
	NxSceneQuery*		GetSceneQuery()				{	return m_pNxSceneQuery;				}
	NxRaycastHit*		_RaycastClosestShapes( NxRay& ray );
	
	// ------------------------------------------------
	// Physics simulation functions
	// ------------------------------------------------
	void 				StartPhysics();
	void 				ProcessInputs(void);
	void 				GetPhysicsResults(void);

public:
	// constructor & destructors
	CMapData( STAGEORD stageOrd );
	~CMapData();

private:
	// ------------------------------------------------
	// load resource : called by load()
	// ------------------------------------------------
	BOOL _LoadCollisionObjects	( const char* mapName );	// .i3scol로부터 NxShapeSet 리스트(m_pCollisionObject)를 로드하고, m_pNxScene에 추가한다.
	
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


	REAL32				_UpdateTime();

private:

	PX_SCENE_STATE		m_SceneState;								// collection data를 한 번만 release 시키기 위해 사용
																	// NXU 모듈이 컬렉션을 관리하기 때문에 플래그로 확인
																	// (2번 release하면 메모리 누수 발생)
	
	// ------------------------------------------------
	// state information
	// ------------------------------------------------
	// stage ordinal number (1..STAGE_ORD_MAX )
	STAGEORD				m_stageOrd;								// map stage ord
																
	// collision objects
	NxShapeSet*				m_ppCollisionObjects[DS_MAX_ACTORS];	// physX objects 
	NxShape*				m_ppShapes[DS_MAX_ACTORS];				// physX Shape
	UINT32					m_ui32CollisionObjectCount;				// # of objects
	
	// ------------------------------------------------
	// physics state
	// ------------------------------------------------
	NxScene*				m_pNxScene;								// simulation world
	NxSceneQuery*			m_pNxSceneQuery;						// sceneQueryObject
	PxRaycastQueryReport	m_queryReport;							// query report
	NxRaycastHit			m_hits[1];								// raycast hit (ref by query report)
	DWORD					m_dwPreviousTime;						// update time
	
	// ------------------------------------------------
	// create scene
	// ------------------------------------------------
	NxMat34					m_matWorld;								// world matrix
	NxVec3					m_vMove;								
	BOOL					m_bInitWorldMatrix;
	
	PxUserNotify			m_userNotify;							// user notify
	NxBounds3				m_sceneBounds;							// scene bounds
	HANDLE					m_hThreadUpdate;						// physics update thread
};
