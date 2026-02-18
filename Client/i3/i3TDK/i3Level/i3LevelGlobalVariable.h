#if !defined( __I3_LEVEL_GLOBAL_VARIABLE_H)
#define __I3_LEVEL_GLOBAL_VARIABLE_H

#include "i3LevelType.h"
#include "i3LevelScene.h"
#include "i3LevelResDatabase.h"
#include "i3LevelUndoSystem.h"

#include "i3NavMesh.h"

extern I3_EXPORT_TDK  i3AttrSet *			g_pSelBoxNode;
extern I3_EXPORT_TDK  i3AttrSet *			g_pTargetBoxNode;
extern I3_EXPORT_TDK  i3LevelScene *		g_pScene;
extern I3_EXPORT_TDK  char					g_CompareNameStr[][32];
extern I3_EXPORT_TDK  i3LevelResDatabase *	g_pResDB;
extern I3_EXPORT_TDK  char					g_szResPath[MAX_PATH];
extern I3_EXPORT_TDK  i3LevelUndoSystem	*	g_pUndoSystem;

enum I3_LEVEL_OBJ_SUBTYPE
{
	I3_LEVEL_OBJ_SUBTYPE_UNKNOWN	= -1,
	I3_LEVEL_OBJ_SUBTYPE_RIGIDOBJ = 0,
	I3_LEVEL_OBJ_SUBTYPE_FIXEDWPN,
	I3_LEVEL_OBJ_SUBTYPE_ITEM,
	I3_LEVEL_OBJ_SUBTYPE_BOMB,
};

struct I3_LEVEL_OBJECT_TYPE
{
	I3_LEVEL_OBJ_SUBTYPE	SubType = I3_LEVEL_OBJ_SUBTYPE::I3_LEVEL_OBJ_SUBTYPE_UNKNOWN;
	char	m_szName[32] = { 0 };
	char	m_szDisplayName[64] = { 0 };
} ;

class i3NavMeshDataMgr;
class i3LevelResEvent;

namespace i3Level
{
	I3_EXPORT_TDK BOOL	Init(void);
	I3_EXPORT_TDK void	Create( const char * pszResBasePath);
	//I3_EXPORT_TDK bool	LoadDB( const char * pszResPath);
	//I3_EXPORT_TDK bool	SaveDB( const char * pszResPath);
	//I3_EXPORT_TDK bool	ScanDB( HWND hwnd, const char * pszResPath, const i3::set< i3::string >& ignoreFolder);
	I3_EXPORT_TDK void	Close(void);

	I3_EXPORT_TDK i3LevelScene *	GetScene(void);
	I3_EXPORT_TDK void			SetScene( i3LevelScene * pScene);

	I3_EXPORT_TDK void	RegisterUpdateNotify( HWND hwnd, i3ClassMeta * pMeta, UINT32 event);

	I3_EXPORT_TDK void	PendingUpdate(void);
	I3_EXPORT_TDK void	ResumeUpdate(void);

	I3_EXPORT_TDK void	Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta = nullptr);

	I3_EXPORT_TDK void	LoadAllEventRes( const char * pszSceneName, i3::vector< i3LevelResEvent *> & resList);

	I3_EXPORT_TDK void	RegisterObjectType( I3_LEVEL_OBJECT_TYPE * pTable, INT32 nCount );
	I3_EXPORT_TDK INT32	GetObjectTypeCount(void);
	I3_EXPORT_TDK I3_LEVEL_OBJECT_TYPE *	GetObjectType( INT32 idx);


	I3_EXPORT_TDK i3NavMesh*	GetNavMesh();
};

namespace i3LevelUndo
{
	I3_EXPORT_TDK bool	Init(i3LevelScene * pScene);
	I3_EXPORT_TDK bool	Close(void);

	I3_EXPORT_TDK bool	SetPendingEnable( bool bPending = true);

	I3_EXPORT_TDK bool	BeginBatch(void);
	I3_EXPORT_TDK bool	EndBatch(void);

	I3_EXPORT_TDK bool	Undo(void);
	I3_EXPORT_TDK bool	Redo(void);

	I3_EXPORT_TDK bool	ResetSystem();

	I3_EXPORT_TDK INT32	GetUndoCount(void);
	I3_EXPORT_TDK INT32 GetRedoCount(void);

	I3_EXPORT_TDK bool	ADD_Create(i3LevelElement3D * pInfo);
	I3_EXPORT_TDK bool	ADD_Delete(i3LevelElement3D * pInfo);
	I3_EXPORT_TDK bool	ADD_Transform(i3LevelElement3D * pInfo);
	I3_EXPORT_TDK bool	ADD_Transform(i3LevelScene * pScene);
	I3_EXPORT_TDK bool	ADD_Edit( i3LevelElement * pInfo);
	I3_EXPORT_TDK bool	ADD_Edit( i3LevelScene * pScene);

	// navmesh
	I3_EXPORT_TDK bool	ADD_NavMesh_GenerateMesh( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next );
	I3_EXPORT_TDK bool	ADD_NavMesh_AddVertex( int idx );

	I3_EXPORT_TDK bool	ADD_NavMesh_RemoveVertex( int idx );
	I3_EXPORT_TDK bool	ADD_NavMesh_RemoveVertex( const i3::vector< int >& undoList );

	I3_EXPORT_TDK bool	ADD_NavMesh_MoveVertex( int idx, const VEC3D& v );
	I3_EXPORT_TDK bool	ADD_NavMesh_MoveVertex( const i3::set< int >& idxList, const VEC3D& v );

	I3_EXPORT_TDK bool	ADD_NavMesh_CreatePolygon( int idx  );

	I3_EXPORT_TDK bool	ADD_NavMesh_RemovePolygon( int idx );
	I3_EXPORT_TDK bool	ADD_NavMesh_RemovePolygon( const i3::set< int >& undoList );

	I3_EXPORT_TDK bool	ADD_NavMesh_InvertPolyNormal( const i3::set< int >& undoList );
	I3_EXPORT_TDK bool	ADD_NavMesh_PolyCut( int addVertex, const i3::vector< int >& removePoly, const i3::vector< int >& createPoly );
};

#endif