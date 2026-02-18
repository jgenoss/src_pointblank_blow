#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelUndoSystem.h"
#include "i3LevelUndoInfoCreate.h"
#include "i3LevelUndoInfoDelete.h"
#include "i3LevelUndoInfoTransform.h"
#include "i3LevelUndoInfoEdit.h"
#include "i3Base/itl/container_util.h"


#include "i3NavMeshUndoInfoGenerateMesh.h"
#include "i3NavMeshUndoInfoAddVertex.h"
#include "i3NavMeshUndoInfoRemoveVertex.h"
#include "i3NavMeshUndoInfoMoveVertex.h"
#include "i3NavMeshUndoInfoInvertPolyNormal.h"
#include "i3NavMeshUndoInfoCreatePoly.h"
#include "i3NavMeshUndoInfoRemovePoly.h"
#include "i3NavMeshUndoInfoPolyCut.h"

I3_CLASS_INSTANCE( i3LevelUndoSystem);

i3LevelUndoSystem::~i3LevelUndoSystem(void)
{
	i3::cu::for_each_container(m_UndoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));		
	i3::cu::for_each_container(m_RedoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));		

	I3_SAFE_RELEASE( m_pCurrentScene);
}

void i3LevelUndoSystem::setCurrentScene( i3LevelScene * pScene)
{
	I3_REF_CHANGE( m_pCurrentScene, pScene);
}

void i3LevelUndoSystem::BeginBatch(void)
{
	I3ASSERT( m_bUndoBatchEnable == false);
	I3ASSERT( m_pBatchParentInfo == nullptr);

	m_pBatchParentInfo = i3LevelUndoInfo::new_object();

	m_bUndoBatchEnable = true;
}

void i3LevelUndoSystem::EndBatch(void)
{
	I3ASSERT( m_bUndoBatchEnable == true);
	I3ASSERT( m_pBatchParentInfo != nullptr);
	
	m_bUndoBatchEnable = false;

	if( m_pBatchParentInfo->GetChildInfoCount() > 0)
	{
		AddUndoInfo( m_pBatchParentInfo);
		m_pBatchParentInfo = nullptr;
	}
	else
	{
		I3_SAFE_RELEASE( m_pBatchParentInfo);
	}
}

bool i3LevelUndoSystem::AddUndoInfo( i3LevelUndoInfo * pInfo)
{
	if( m_bUndoBatchEnable == false)
	{
		//새로 액션이 실행되면 UndoList에는 추가되고 , RedoList는 초기화 된다.
	//	m_RedoList.SafeReleaseAll();
		i3::cu::for_each_container(m_RedoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));	
		m_RedoList.clear();

		m_UndoList.push_back( pInfo);
	}
	else
	{
		I3ASSERT( m_pBatchParentInfo != nullptr);

		m_pBatchParentInfo->AddChildUndoInfo( pInfo);
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT, pInfo, i3LevelUndoInfo::static_meta());

	//undo list는 100개 이하로 제한
	if( m_UndoList.size() > 100)
	{
		i3LevelUndoInfo * pInfo2 = m_UndoList[0];

		I3_SAFE_RELEASE( pInfo2);

		m_UndoList.erase(m_UndoList.begin());
	}

	return true;
}

void i3LevelUndoSystem::ResetSystem(void)
{

//	m_UndoList.SafeReleaseAll();
//	m_RedoList.SafeReleaseAll();
	i3::cu::for_each_container(m_UndoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));		
	m_UndoList.clear();
	i3::cu::for_each_container(m_RedoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));		
	m_RedoList.clear();


	i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT, nullptr, i3LevelUndoInfo::static_meta());
}

bool	i3LevelUndoSystem::Undo(void)
{
	size_t lastUndo = m_UndoList.size();

	if( lastUndo == 0)	return false;

	m_pCurrentScene->UnselectAll();

	i3LevelUndoInfo * pUndo = m_UndoList[lastUndo - 1];
	I3ASSERT( pUndo != nullptr);

	_BeginUndo();
	{
		pUndo->Undo( m_pCurrentScene);
	}
	_EndUndo();

	m_UndoList.erase(m_UndoList.begin() + lastUndo - 1);
	m_RedoList.push_back( pUndo);

	//변경되면 dlg update를 호출해주기 위하야!!
	i3LevelElement * pElement = pUndo->getObject();
	if( i3::kind_of<i3LevelElement3D*>( pElement))
	{
		m_pCurrentScene->SelectElement((i3LevelElement3D*) pElement);
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT, pUndo, i3LevelUndoInfo::static_meta());

	return true;
}

bool i3LevelUndoSystem::Redo(void)
{
	size_t lastRedo = m_RedoList.size();

	if( lastRedo == 0)	return false;

	m_pCurrentScene->UnselectAll();

	i3LevelUndoInfo * pRedo = m_RedoList[lastRedo - 1];
	I3ASSERT( pRedo != nullptr);

	_BeginUndo();
	{
		pRedo->Redo( m_pCurrentScene);
	}
	_EndUndo();

	m_RedoList.erase( m_RedoList.begin() + lastRedo - 1);
	m_UndoList.push_back( pRedo);

	//변경되면 dlg update를 호출해주기 위하야!!
	i3LevelElement * pElement = pRedo->getObject();
	if( i3::kind_of< i3LevelElement3D*>( pElement))
	{
		m_pCurrentScene->SelectElement((i3LevelElement3D*) pElement);
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT, pRedo, i3LevelUndoInfo::static_meta());

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
bool i3LevelUndoSystem::ADDUNDO_Create( i3LevelElement3D * pElm)
{
	if( !m_isUserAction)	return false;

	i3LevelUndoInfo * pInfo = i3LevelUndoInfoCreate::new_object();
	pInfo->setObject( pElm);

	AddUndoInfo( pInfo);

	return true;
}

bool i3LevelUndoSystem::ADDUNDO_Delete( i3LevelElement3D * pElm)
{
	if( !m_isUserAction)	return false;

	i3LevelUndoInfo * pInfo = i3LevelUndoInfoDelete::new_object();
	pInfo->setObject( pElm);

	AddUndoInfo( pInfo);

	return true;
}

bool i3LevelUndoSystem::ADDUNDO_Transform( i3LevelElement3D * pElm)
{
	if( !m_isUserAction)	return false;

	i3LevelUndoInfo * pInfo = i3LevelUndoInfoTransform::new_object();
	pInfo->setObject( pElm);

	AddUndoInfo( pInfo);

	return true;
}

bool i3LevelUndoSystem::ADDUNDO_Transform( i3LevelScene * pScene)
{
	if( !m_isUserAction)	return false;

	i3::vector<i3LevelElement3D*> list;

	pScene->GetSelectedElements( list);

	INT32 nSelCnt = (INT32)list.size();
	if( nSelCnt <= 0)
	{
		return false;	//선택된것이 없다...
	}

	i3LevelElement3D * pElm = list[0];
	
	//위치와 회전, 크기상의 변화가 없다면 무시
	if( i3Matrix::IsSame( pElm->GetTransform(), pElm->getSavedMatrix()) == TRUE)
	{
		return false;
	}

	if( nSelCnt == 1)
	{
		return ADDUNDO_Transform( pElm);
	}
		
	BeginBatch();
	for( size_t i = 0;i < list.size(); ++i)
	{
		pElm = list[i];

		ADDUNDO_Transform( pElm);
	}
	EndBatch();

	return true;
}

bool i3LevelUndoSystem::ADDUNDO_Edit( i3LevelElement * pElm)
{
	if( !m_isUserAction)	return false;

	i3LevelUndoInfoEdit * pInfo = i3LevelUndoInfoEdit::new_object();
	pInfo->setObject( pElm);

	AddUndoInfo( pInfo);

	return true;
}

bool i3LevelUndoSystem::ADDUNDO_Edit( i3LevelScene * pScene)
{
	if( !m_isUserAction)	return false;

	i3::vector<i3LevelElement3D*> list;

	pScene->GetSelectedElements( list);

	INT32 nSelCnt = (INT32)list.size();
	if( nSelCnt <= 0)
	{
		return false;	//선택된것이 없다...
	}

	i3LevelElement * pElm = list[0];
	
	if( nSelCnt == 1)
	{
		return ADDUNDO_Edit( pElm);
	}
		
	BeginBatch();
	for( size_t i = 0;i < list.size(); ++i)
	{
		pElm = list[i];

		ADDUNDO_Edit( pElm);
	}
	EndBatch();

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_AddVertex( int idx )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoAddVertex* pInfo = i3NavMeshUndoInfoAddVertex::new_object();

	pInfo->SetUndoInfo( idx );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_RemoveVertex( int idx )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoRemoveVertex* pInfo = i3NavMeshUndoInfoRemoveVertex::new_object();

	pInfo->SetUndoInfo( idx );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_RemoveVertex( const i3::vector< int >& undoList )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoRemoveVertex* pInfo = i3NavMeshUndoInfoRemoveVertex::new_object();

	pInfo->SetUndoInfo( undoList );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_MoveVertex( int idx, const VEC3D& v )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoMoveVertex* pInfo = i3NavMeshUndoInfoMoveVertex::new_object();

	pInfo->SetUndoInfo( idx, v );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_MoveVertex( const i3::set< int >& idxList, const VEC3D& v )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoMoveVertex* pInfo = i3NavMeshUndoInfoMoveVertex::new_object();

	pInfo->SetUndoInfo( idxList, v );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_CreatePolygon( int idx )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoCreatePoly* pInfo = i3NavMeshUndoInfoCreatePoly::new_object();

	pInfo->SetUndoInfo( idx );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_RemovePolygon( int idx )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoRemovePoly* pInfo = i3NavMeshUndoInfoRemovePoly::new_object();

	pInfo->SetUndoInfo( idx );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_RemovePolygon( const i3::set< int >& undoList )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoRemovePoly* pInfo = i3NavMeshUndoInfoRemovePoly::new_object();

	pInfo->SetUndoInfo( undoList );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_GenerateMesh( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoGenerateMesh* pInfo = i3NavMeshUndoInfoGenerateMesh::new_object();

	pInfo->SetUndoInfo( prev, next );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_InvertPolyNormal( const i3::set< int >& undoList )
{	
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoInvertPolyNormal* pInfo = i3NavMeshUndoInfoInvertPolyNormal::new_object();

	pInfo->SetUndoInfo( undoList );
	AddUndoInfo( pInfo );

	return true;
}

bool	i3LevelUndoSystem::ADDUNDO_NavMesh_PolyCut( int addVertex, const i3::vector< int >& removePoly, const i3::vector< int >& createPoly )
{
	if( !m_isUserAction)	return false;

	i3NavMeshUndoInfoPolyCut* pInfo = i3NavMeshUndoInfoPolyCut::new_object();

	pInfo->SetUndoInfo( addVertex, removePoly, createPoly );
	AddUndoInfo( pInfo );

	return true;
}