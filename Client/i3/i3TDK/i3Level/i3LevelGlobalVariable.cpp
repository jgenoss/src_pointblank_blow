#include "stdafx.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelViewport.h"
#include "i3LevelProperty.h"
#include "i3LevelControl.h"
#include "i3LevelAxis.h"
#include "i3LevelAxisMove.h"
#include "i3LevelAxisRotate.h"
#include "i3LevelAxisScale.h"
#include "i3LevelIcon.h"
#include "i3LevelStaticObject.h"
#include "i3LevelPrimitive.h"
#include "i3LevelPrimitive_Box.h"
#include "i3LevelPrimitive_Sphere.h"
#include "i3LevelPrimitive_Capsule.h"
#include "i3LevelLight.h"
#include "i3LevelScene.h"
#include "i3LevelLink.h"
#include "i3LevelChara.h"
#include "i3LevelObject.h"
#include "i3LevelPath.h"
#include "i3LevelWorld.h"
#include "i3LevelPrim_Respawn.h"
#include "i3LevelElementList.h"
#include "i3LevelCameraObject.h"
#include "i3LevelPathPoint.h"
#include "i3LevelPathLink.h"

#include "i3LevelRes.h"
#include "i3LevelResPrim.h"
#include "i3LevelResPrimitive_Box.h"
#include "i3LevelResPrimitive_Sphere.h"
#include "i3LevelResPrimitive_Capsule.h"
#include "i3LevelResPrimitive_Respawn.h"
#include "i3LevelResSceneGraph.h"
#include "i3LevelResSceneGraphClone.h"
#include "i3LevelResAnimation.h"
#include "i3LevelResSound.h"
#include "i3LevelResTexture.h"
#include "i3LevelResCamera.h"
#include "i3LevelResChara.h"
#include "i3LevelResObject.h"
#include "i3LevelResWorld.h"
#include "i3LevelResDatabase.h"
#include "i3LevelResLight.h"
#include "i3LevelResPath.h"
#include "i3LevelResRef.h"

#include "i3LevelResEvent.h"

//undo info
#include "i3LevelUndoInfo.h"
#include "i3LevelUndoInfoCreate.h"
#include "i3LevelUndoInfoDelete.h"
#include "i3LevelUndoInfoTransform.h"
#include "i3LevelUndoInfoEdit.h"
#include "i3LevelUndoSystem.h"

#include "i3LevelLayer.h"
#include "i3LevelGroup.h"

#include "i3NavMeshPrimitiveVertex.h"
#include "i3NavMeshPrimitiveSelBox.h"
#include "i3NavMeshPrimitivePolyNormal.h"
#include "i3NavMeshPrimitivePolygon.h"
#include "i3NavMeshPrimitivePolyCut.h"

#include "i3NavMeshDataMgr.h"
#include "i3NavMeshGenerator.h"
#include "i3NavMesh.h"

#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"

////

I3_EXPORT_TDK i3LevelScene *			g_pScene;
I3_EXPORT_TDK i3AttrSet *				g_pSelBoxNode = nullptr;
I3_EXPORT_TDK i3AttrSet *				g_pTargetBoxNode = nullptr;
static BOOL				s_bUpdate = TRUE;

I3_EXPORT_TDK I3_LEVEL_OBJECT_TYPE *	g_pObjectTypeTable = nullptr;
I3_EXPORT_TDK INT32					g_ObjectTypeCount = 0;
I3_EXPORT_TDK i3LevelResDatabase *	g_pResDB = nullptr;
I3_EXPORT_TDK char					g_szResPath[MAX_PATH];

I3_EXPORT_TDK i3LevelUndoSystem	*	g_pUndoSystem = nullptr;

char g_CompareNameStr[][32] =
{
	"EQ",
	"NE",
	"GT",
	"GE",
	"LT",
	"LE"
};

BOOL i3Level::Init(void)
{
#if defined( DO_VLOG)
	i3VLog::Init();
#endif

	return TRUE;
}

void i3Level::Close(void)
{
	PendingUpdate();

	I3_SAFE_RELEASE( g_pUndoSystem);
	I3_SAFE_RELEASE( g_pSelBoxNode);
	I3_SAFE_RELEASE( g_pTargetBoxNode);
	I3_SAFE_RELEASE( g_pScene);
	I3_SAFE_RELEASE( g_pResDB);
#if defined( DO_VLOG)
	i3VLog::Finish();
#endif
}

void i3Level::Create( const char * pszResBasePath)
{
	i3::string_ncopy_nullpad( g_szResPath, pszResBasePath, _countof( g_szResPath));

	I3_SAFE_RELEASE( g_pResDB);

	g_pResDB = i3LevelResDatabase::new_object();
}

/*
bool i3Level::LoadDB( const char * pszResPath)
{
	I3_SAFE_RELEASE( g_pResDB);

	g_pResDB = i3LevelResDatabase::Load( pszResPath);

	if( g_pResDB == nullptr)
	{
		return false;
	}

	i3::extract_directoryname(pszResPath, g_szResPath);

	return true;
}

bool i3Level::SaveDB( const char * pszResPath)
{
	return i3LevelResDatabase::Save( g_pResDB, pszResPath);
}

bool i3Level::ScanDB( HWND hwnd, const char * pszResPath, const i3::set< i3::string >& ignoreFolder )
{
	I3_SAFE_RELEASE( g_pResDB);

	g_pResDB = i3LevelResDatabase::new_object();
	I3ASSERT( g_pResDB != nullptr);

	strcpy( g_szResPath, pszResPath);

	g_pResDB->StartScan( hwnd, pszResPath, ignoreFolder, nullptr );

	return true;
}
*/

i3LevelScene *	i3Level::GetScene(void)
{
	return g_pScene;
}

i3NavMesh *	i3Level::GetNavMesh(void)
{
	if( g_pScene )
		return g_pScene->GetNavMeshRoot();

	return nullptr;
}

void i3Level::SetScene( i3LevelScene * pScene)
{
	// 기존 해제...
	{
		if( g_pScene != nullptr)
		{
			Update( nullptr, I3_TDK_UPDATE_REMOVE, g_pScene);
		}

		i3LevelUndo::Close();

		if( pScene == nullptr)
		{
			// nullptr 인자로 들어 온 경우는 Scene을 삭제하는 경우로, 특별 처리.
			if( g_pResDB != nullptr)
			{
				g_pResDB->Reset();
			}
		}
	}

	PendingUpdate();

	I3_SAFE_RELEASE( g_pScene);

	{
		i3LevelViewport * pViewport;
		i3LevelFramework * pFramework;

		pViewport = i3LevelViewport::GetCurrentFocusViewport();

		if( pViewport != nullptr)
		{
			pFramework = pViewport->getFramework();

			pFramework->SetScene( pScene);
		}
	}


	g_pScene = pScene;

	I3_SAFE_ADDREF(g_pScene);

	if( g_pResDB != nullptr)
	{
		g_pResDB->BindScene( pScene);
	}

	i3LevelUndo::Init( pScene);

	ResumeUpdate();

	Update( nullptr, I3_TDK_UPDATE_SCENE, g_pScene, i3LevelScene::static_meta());
}

void i3Level::RegisterUpdateNotify( HWND hwnd, i3ClassMeta * pMeta, UINT32 event)
{
	i3TDK::RegisterUpdate( hwnd, pMeta, event);
}

void	i3Level::PendingUpdate(void)
{
	i3TDK::SetPendingUpdateState( TRUE);
}

void	i3Level::ResumeUpdate(void)
{
	i3TDK::SetPendingUpdateState( FALSE);
}

void i3Level::Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta)
{
	i3TDK::Update( hwnd, event, pObj, pMeta);
}

void i3Level::RegisterObjectType( I3_LEVEL_OBJECT_TYPE * pTable, INT32 nCount )
{
	g_pObjectTypeTable = pTable;

	g_ObjectTypeCount = nCount;
}

INT32 i3Level::GetObjectTypeCount(void)
{
	return g_ObjectTypeCount;
}

I3_LEVEL_OBJECT_TYPE * i3Level::GetObjectType( INT32 idx)
{
	return & g_pObjectTypeTable[ idx];
}

static BOOL EventProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	char szFull[ MAX_PATH], szName[ MAX_PATH], szRelPath[MAX_PATH];
	i3::vector< i3LevelResEvent *> * pList = (i3::vector< i3LevelResEvent *> *) pUserData;

	i3LevelResEvent * pRes;
	

	// Loading
	{
		sprintf_s( szFull, _countof( szFull)- 1, "%s/%s", pszPath, pFileInfo->cFileName);

		i3::make_relative_path( g_szResPath, szFull, szRelPath);

		pRes = (i3LevelResEvent *) g_pResDB->FindResByPath( szRelPath);

		if( pRes == nullptr)
		{
			pRes = i3LevelResEvent::new_object_ref();
			pRes->Load( szFull);

			i3::extract_filetitle( pFileInfo->cFileName, szName);

			pRes->SetName( szName);

			g_pResDB->AddRes( pRes);
		}
	}

	pList->push_back( pRes);

	return TRUE;
}

void i3Level::LoadAllEventRes( const char * pszSceneName, i3::vector< i3LevelResEvent *> & resList)
{
	char szPath[ MAX_PATH];

	sprintf( szPath, "%s/Scene/%s/_Generated", i3ResourceFile::GetWorkingFolder(), pszSceneName);

	i3System::LookForFiles( szPath, "*.i3Evt", (FINDFILEPROC) EventProc, (void *) & resList);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Undo / Redo System
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool i3LevelUndo::Init(i3LevelScene * pScene)
{
	Close();

	if( pScene != nullptr)
	{
		g_pUndoSystem = i3LevelUndoSystem::new_object();

		g_pUndoSystem->setCurrentScene( pScene);

		return true;
	}

	return false;
}

bool i3LevelUndo::Close(void)
{
	I3_SAFE_RELEASE( g_pUndoSystem);

	return true;
}

bool i3LevelUndo::BeginBatch(void)
{
	if( g_pUndoSystem != nullptr)
	{
		g_pUndoSystem->BeginBatch();

		return true;
	}

	return false;
}

bool i3LevelUndo::EndBatch(void)
{
	if( g_pUndoSystem != nullptr)
	{
		g_pUndoSystem->EndBatch();

		return true;
	}

	return false;
}

bool i3LevelUndo::Undo(void)
{
	if( g_pUndoSystem != nullptr)
	{
		g_pUndoSystem->Undo();

		return true;
	}

	return false;
}

bool i3LevelUndo::Redo(void)
{
	if( g_pUndoSystem != nullptr)
	{
		g_pUndoSystem->Redo();

		return true;
	}

	return false;
}

bool	i3LevelUndo::ResetSystem()
{
	if( g_pUndoSystem )
	{
		g_pUndoSystem->ResetSystem();
		return true;
	}

	return false;
}

INT32 i3LevelUndo::GetUndoCount(void)
{
	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->getUndoCount();
	}

	return 0;
}

INT32 i3LevelUndo::GetRedoCount(void)
{
	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->getRedoCount();
	}

	return 0;
}

bool i3LevelUndo::ADD_Create(i3LevelElement3D * pElement)
{
	if( i3::kind_of<i3LevelControl* >(pElement))
		return false;

	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Create( pElement);
	}

	return true;
}

bool i3LevelUndo::ADD_Delete(i3LevelElement3D * pElement)
{
	if( i3::kind_of<i3LevelControl* >(pElement) && !i3::kind_of<i3LevelHitPoint* >(pElement))
		return false;

	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Delete( pElement);
	}

	return true;
}

bool i3LevelUndo::ADD_Transform(i3LevelElement3D * pElement)
{
	if( i3::kind_of<i3LevelControl* >(pElement))
		return false;

	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Transform( pElement);
	}

	return true;
}

bool i3LevelUndo::ADD_Transform(i3LevelScene * pScene)
{
	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Transform( pScene);
	}

	return true;
}

bool i3LevelUndo::ADD_Edit( i3LevelElement * pElm)
{
	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Edit( pElm);
	}

	return true;
}

bool i3LevelUndo::ADD_Edit( i3LevelScene * pScene)
{
	if( g_pUndoSystem != nullptr)
	{
		return g_pUndoSystem->ADDUNDO_Edit( pScene);
	}

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_GenerateMesh( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_GenerateMesh( prev, next );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_AddVertex( int idx )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_AddVertex( idx );		

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_RemoveVertex( int idx )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_RemoveVertex( idx );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_RemoveVertex( const i3::vector< int >& undoList )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_RemoveVertex( undoList );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_MoveVertex( int idx, const VEC3D& v )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_MoveVertex( idx, v );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_MoveVertex( const i3::set< int >& idxList, const VEC3D& v )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_MoveVertex( idxList, v );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_CreatePolygon( int idx )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_CreatePolygon( idx );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_RemovePolygon( int idx )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_RemovePolygon( idx );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_RemovePolygon( const i3::set< int >& undoList )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_RemovePolygon( undoList );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_InvertPolyNormal( const i3::set< int >& undoList )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_InvertPolyNormal( undoList );

	return true;
}

bool	i3LevelUndo::ADD_NavMesh_PolyCut( int addVertex, const i3::vector< int >& removePoly, const i3::vector< int >& createPoly )
{
	if( nullptr != g_pUndoSystem )
		return g_pUndoSystem->ADDUNDO_NavMesh_PolyCut( addVertex, removePoly, createPoly );

	return true;
}