#if !defined( __I3_LEVEL_UNDO_SYSTEM_H)
#define __I3_LEVEL_UNDO_SYSTEM_H

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class i3NavMeshDataMgr;

class I3_EXPORT_TDK i3LevelUndoSystem : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3LevelUndoSystem, i3ElementBase);

protected:
	i3::vector<i3LevelUndoInfo*> m_UndoList;
	i3::vector<i3LevelUndoInfo*> m_RedoList;

	i3LevelScene	*	m_pCurrentScene = nullptr;

	bool				m_isUserAction = true;			//User에 의한 Action여부
	bool				m_bUndoBatchEnable = false;		//여러개의 동작을 한번에 수행하는경우 플래그

	i3LevelUndoInfo	*	m_pBatchParentInfo = nullptr;

public:
	virtual ~i3LevelUndoSystem(void);

	void		ResetSystem(void);
	void		setCurrentScene( i3LevelScene * pScene);

protected:
	void		_BeginUndo(void)							{ m_isUserAction = false;}
	void		_EndUndo(void)								{ m_isUserAction = true;}

public:
	void		BeginBatch(void);
	void		EndBatch(void);

	bool		AddUndoInfo( i3LevelUndoInfo * pInfo);

	bool		Undo(void);
	bool		Redo(void);

	INT32		getUndoCount(void)	{	return (INT32)m_UndoList.size();}
	INT32		getRedoCount(void)	{	return (INT32)m_RedoList.size();}

	// User의 Action에의한 행동인지 Undo/Redo에의한 행동인지 
	bool		isUserAction(void)							{ return m_isUserAction;}

public:
	bool		ADDUNDO_Create( i3LevelElement3D * pElm);
	bool		ADDUNDO_Delete( i3LevelElement3D * pElm);
	bool		ADDUNDO_Transform( i3LevelElement3D * pElm);
	bool		ADDUNDO_Transform( i3LevelScene * pScene);
	bool		ADDUNDO_Edit( i3LevelElement * pElm);
	bool		ADDUNDO_Edit( i3LevelScene * pScene);

	// navmesh
public:
	bool		ADDUNDO_NavMesh_GenerateMesh( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next );
	bool		ADDUNDO_NavMesh_AddVertex( int idx );
	
	bool		ADDUNDO_NavMesh_RemoveVertex( int idx );
	bool		ADDUNDO_NavMesh_RemoveVertex( const i3::vector< int >& undoList );

	bool		ADDUNDO_NavMesh_MoveVertex( int idx, const VEC3D& v );
	bool		ADDUNDO_NavMesh_MoveVertex( const i3::set< int >& idxList, const VEC3D& v );

	bool		ADDUNDO_NavMesh_CreatePolygon( int idx );

	bool		ADDUNDO_NavMesh_RemovePolygon( int idx );
	bool		ADDUNDO_NavMesh_RemovePolygon( const i3::set< int >& undoList );

	bool		ADDUNDO_NavMesh_InvertPolyNormal( const i3::set< int >& undoList );
	bool		ADDUNDO_NavMesh_PolyCut( int addVertex, const i3::vector< int >& removePoly, const i3::vector< int >& createPoly );
};

#endif
