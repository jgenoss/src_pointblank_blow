//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

//#if defined( I3_DISTRIBUTE)

#include "UIUndoSystem.h"
#include "UIUndoInfoCreate.h"
#include "UIUndoInfoDelete.h"
#include "UIUndoInfoTransform.h"
#include "UIUndoInfoEdit.h"

#include "i3Base/itl/container_util.h"


I3_CLASS_INSTANCE( UIUndoSystem); //, i3ElementBase);

UIUndoSystem::UIUndoSystem(void)
{
	
	m_pScene = NULL;

	m_isUserAction		= true;
	m_bUndoBatchEnable	= false;

	m_pBatchParentInfo	= NULL;
	m_bPendingEnable	= false;
}

UIUndoSystem::~UIUndoSystem(void)
{
	i3::cu::for_each_safe_release(m_UndoList, &UIUndoInfo::Release);
	i3::cu::for_each_safe_release(m_RedoList, &UIUndoInfo::Release);
}

void UIUndoSystem::SetCurrentScene( i3UIScene * pScene)
{
	m_pScene = pScene;
}

void UIUndoSystem::getInfoList( i3::vector<UIUndoInfo*>& Out)
{
	
	Out.clear();

	for( size_t i=0; i< m_UndoList.size(); i++)
	{
		UIUndoInfo* pInfo = m_UndoList[i];
		Out.push_back( pInfo);
	}

	for( size_t i=0; i< m_RedoList.size(); i++)
	{
		UIUndoInfo* pInfo = m_RedoList[i];
		Out.push_back( pInfo);
	}
}

void UIUndoSystem::BeginBatch(void)
{
	I3ASSERT( m_bUndoBatchEnable == false);
	I3ASSERT( m_pBatchParentInfo == NULL);

	m_pBatchParentInfo = UIUndoInfo::new_object();

	m_bUndoBatchEnable = true;
}

void UIUndoSystem::EndBatch(void)
{
	I3ASSERT( m_bUndoBatchEnable == true);
	I3ASSERT( m_pBatchParentInfo != NULL);
	
	m_bUndoBatchEnable = false;

	if( m_pBatchParentInfo->GetChildInfoCount() > 0)
	{
		AddUndoInfo( m_pBatchParentInfo);
		m_pBatchParentInfo = NULL;
	}
	else
	{
		I3_SAFE_RELEASE( m_pBatchParentInfo);
	}
}

bool UIUndoSystem::AddUndoInfo( UIUndoInfo * pInfo)
{
	if( m_bUndoBatchEnable == false)
	{
		//새로 액션이 실행되면 UndoList에는 추가되고 , RedoList는 초기화 된다.

		i3::cu::for_each_safe_release_clear(m_RedoList, &UIUndoInfo::Release);
		
		m_UndoList.push_back( pInfo);
	}
	else
	{
		I3ASSERT( m_pBatchParentInfo != NULL);

		m_pBatchParentInfo->AddChildUndoInfo( pInfo);
	}

	//i3TDK::Update( NULL, I3_TDK_UPDATE_UI, NULL);

	//undo list는 100개 이하로 제한
	if( m_UndoList.size() > 100)
	{
		UIUndoInfo * pInfo2 = m_UndoList[0];
		I3_SAFE_RELEASE( pInfo2);
		i3::vu::erase_index(m_UndoList, 0);
	}

	return true;
}

void UIUndoSystem::ResetSystem(void)
{
	i3::cu::for_each_safe_release_clear(m_UndoList, &UIUndoInfo::Release);
	i3::cu::for_each_safe_release_clear(m_RedoList, &UIUndoInfo::Release);

	//i3TDK::Update( NULL, I3_TDK_UPDATE_UI, NULL);
}

bool	UIUndoSystem::Undo(void)
{
	size_t lastUndo = m_UndoList.size();

	if( lastUndo == 0)	return false;

	//m_pFramework->UnSelectAllObj();

	UIUndoInfo * pUndo = m_UndoList[lastUndo - 1];
	I3ASSERT( pUndo != NULL);

	i3::vector<i3UIControl*> selList;
	pUndo->GetElementList( selList);				// selList가 정보를 수집하지만, 활용되는 부분은 주석으로 되어있어 활용이되지 않고 있다.(2012.05.30.수빈)

	_BeginUndo();
	{
		pUndo->Undo( m_pScene);
	}
	_EndUndo();

	i3::vu::erase_index(m_UndoList, lastUndo - 1);

	m_RedoList.push_back( pUndo);

	//변경되면 dlg update를 호출해주기 위하야!!
	//{
		//m_pFramework->SetSelectedObjects( &selList);

		//i3TDK::Update( NULL, I3_TDK_UPDATE_UI, NULL);
	//}

	//I3TRACE("Undo: %d, %d\n", m_UndoList.GetCount(), m_RedoList.GetCount());

	return true;
}

bool UIUndoSystem::Redo(void)
{
	size_t lastRedo = m_RedoList.size();

	if( lastRedo == 0)	return false;

	//m_pFramework->UnSelectAllObj();

	UIUndoInfo * pRedo = m_RedoList[lastRedo - 1];
	I3ASSERT( pRedo != NULL);

	i3::vector<i3UIControl*> selList;
	pRedo->GetElementList( selList);			// selList가 정보를 수집하지만, 활용되는 부분은 주석으로 되어있어 활용이되지 않고 있다.(2012.05.30.수빈)
	_BeginUndo();
	{
		pRedo->Redo( m_pScene);
	}
	_EndUndo();

	i3::vu::erase_index(m_RedoList, lastRedo - 1);
	m_UndoList.push_back( pRedo);

	//변경되면 dlg update를 호출해주기 위하야!!
	{
		//m_pFramework->SetSelectedObjects( &selList);

		//i3TDK::Update( NULL, I3_TDK_UPDATE_UI, NULL);
	}

	//I3TRACE("Redo: %d, %d\n", m_UndoList.GetCount(), m_RedoList.GetCount());

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void UIUndoSystem::_rec_ADDUNDO_Create( UIUndoInfo* pInfo, i3UIControl* pObj)
{
	I3ASSERT( pInfo != NULL);
	I3ASSERT( pObj != NULL);

	// 자기부터 넣고
	UIUndoInfo* pInfoCreate = UIUndoInfoCreate::new_object();
	pInfoCreate->setObject( pObj);
	pInfo->AddChildUndoInfo( pInfoCreate);

	////////////////////////////////////////////////////////////////////////
	// Undo info에서 object가 가지는 하위 컨트롤 들에대해서 일일이 Undoinfo를 
	// 만들 필요는 없다. 왜냐하면, Object는 자신의 child에 대한 정보를
	// 이미 충분하게 가지고 있기 때문이다. - raja -
	////////////////////////////////////////////////////////////////////////
	// 그 다음 Children
	/*i3GameNode* pChild = pObj->getFirstChild();
	while( pChild != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl* pChildCtrl = (i3UIControl*)pChild;
			if( (pChildCtrl->getState() & I3UI_STYLE_SLAVE) == 0 && (pChildCtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
			{
				_rec_ADDUNDO_Create( pInfo, pChildCtrl);
			}
		}

		pChild = pChild->getNext();
	}*/
}

void UIUndoSystem::_rec_ADDUNDO_Delete( UIUndoInfo* pInfo, i3UIControl* pObj)
{
	I3ASSERT( pInfo != NULL);
	I3ASSERT( pObj != NULL);

	////////////////////////////////////////////////////////////////////////
	// Undo info에서 object가 가지는 하위 컨트롤 들에대해서 일일이 Undoinfo를 
	// 만들 필요는 없다. 왜냐하면, Object는 자신의 child에 대한 정보를
	// 이미 충분하게 가지고 있기 때문이다. - raja -
	////////////////////////////////////////////////////////////////////////
	// Children부터 넣고
	/*i3GameNode* pChild = pObj->getFirstChild();
	while( pChild != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl* pChildCtrl = (i3UIControl*)pChild;
			if( (pChildCtrl->getState() & I3UI_STYLE_SLAVE) == 0 && (pChildCtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
			{
				_rec_ADDUNDO_Delete( pInfo, pChildCtrl);
			}
		}

		pChild = pChild->getNext();
	}*/
	
	// 그 다음 자신
	UIUndoInfo* pInfoDelete = UIUndoInfoDelete::new_object();
	pInfoDelete->setObject( pObj);
	pInfo->AddChildUndoInfo( pInfoDelete);
	
}

bool UIUndoSystem::ADDUNDO_Create( i3UIControl * pObj)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	////////////////////////////////////////////////////////////////////////
	// Undo info에서 object가 가지는 하위 컨트롤 들에대해서 일일이 Undoinfo를 
	// 만들 필요는 없다. 왜냐하면, Object는 자신의 child에 대한 정보를
	// 이미 충분하게 가지고 있기 때문이다. - raja -
	////////////////////////////////////////////////////////////////////////
	/*
	//유효한 Child가 있는지 검사
	INT32 nChildCount = 0;
	i3GameNode* pChild = pObj->getFirstChild();
	
	while( pChild != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl* pChildCtrl = (i3UIControl*)pChild;
			if( (pChildCtrl->getState() & I3UI_STYLE_SLAVE) == 0 && (pChildCtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
			{
				nChildCount++;
			}
		}
		
		pChild = pChild->getNext();
	}

	if( nChildCount < 1)
	{
		UIUndoInfo * pInfo = UIUndoInfoCreate::new_object();
	
		pInfo->setObject( pObj);

		AddUndoInfo( pInfo);
	}
	else
	{
		//BeginBatch()를 이중으로 호출할 수 없으므로 수동으로
		//UIUndoInfoCreate를 Child로 가지는 UIUndoInfo를 만들어서 추가한다.
		UIUndoInfo* pInfo = UIUndoInfo::new_object();

		_rec_ADDUNDO_Create( pInfo, pObj);

		AddUndoInfo( pInfo);
	}
*/
	UIUndoInfo * pInfo = UIUndoInfoCreate::new_object();

	pInfo->setObject( pObj);

	AddUndoInfo( pInfo);

	return true;
}

bool UIUndoSystem::ADDUNDO_Create( const i3::vector<i3UIControl*>&  ObjList)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	size_t nCount = ObjList.size();

	if( nCount <= 0)
	{
		return false;
	}
	else if( nCount == 1)
	{
		i3UIControl* pObj = ObjList[0];
		return ADDUNDO_Create( pObj);
	}
	else
	{
		bool bResult = true;
		
		BeginBatch();
		for( size_t i=0; i< nCount; i++)
		{
			i3UIControl* pObj = ObjList[i];
			bResult &= ADDUNDO_Create( pObj);
		}
		EndBatch();

		return bResult;
	}
}

bool UIUndoSystem::ADDUNDO_Delete( i3UIControl * pObj)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	////////////////////////////////////////////////////////////////////////
	// Undo info에서 object가 가지는 하위 컨트롤 들에대해서 일일이 Undoinfo를 
	// 만들 필요는 없다. 왜냐하면, Object는 자신의 child에 대한 정보를
	// 이미 충분하게 가지고 있기 때문이다. - raja -
	////////////////////////////////////////////////////////////////////////
	/*	
	//유효한 Child가 있는지 검사
	INT32 nChildCount = 0;
	i3GameNode* pChild = pObj->getFirstChild();
	
	while( pChild != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl* pChildCtrl = (i3UIControl*)pChild;
			if( (pChildCtrl->getState() & I3UI_STYLE_SLAVE) == 0 && (pChildCtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
			{
				nChildCount++;
			}
		}
		
		pChild = pChild->getNext();
	}

	if( nChildCount < 1)
	{
		UIUndoInfo * pInfo = UIUndoInfoDelete::new_object();
	
		pInfo->setObject( pObj);

		AddUndoInfo( pInfo);
	}
	else
	{
		//BeginBatch()를 이중으로 호출할 수 없으므로 수동으로
		//UIUndoInfoDelete를 Child로 가지는 UIUndoInfo를 만들어서 추가한다.
		UIUndoInfo* pInfo = UIUndoInfo::new_object();

		_rec_ADDUNDO_Delete( pInfo, pObj);

		AddUndoInfo( pInfo);
	}
	*/

	UIUndoInfo * pInfo = UIUndoInfoDelete::new_object();

	pInfo->setObject( pObj);

	AddUndoInfo( pInfo);

	return true;
}

bool UIUndoSystem::ADDUNDO_Delete( const i3::vector<i3UIControl*>&  ObjList)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	size_t nCount = ObjList.size();

	if( nCount <= 0)
	{
		return false;
	}
	else if( nCount == 1)
	{
		i3UIControl* pObj = ObjList[0];
		return ADDUNDO_Delete( pObj);
	}
	else
	{
		bool bResult = true;
		
		BeginBatch();
		for( size_t i=0; i< nCount; i++)
		{
			i3UIControl* pObj = ObjList[i];
			bResult &= ADDUNDO_Delete( pObj);
		}
		EndBatch();

		return bResult;
	}
}

bool UIUndoSystem::ADDUNDO_Transform( i3UIControl * pObj)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	UIUndoInfo * pInfo = UIUndoInfoTransform::new_object();
	
	pInfo->setObject( pObj);

	AddUndoInfo( pInfo);

	return true;
}

bool UIUndoSystem::ADDUNDO_Transform( const i3::vector<i3UIControl*>& ObjList)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	size_t nCount = ObjList.size();

	if( nCount <= 0)
	{
		return false;
	}
	else if( nCount == 1)
	{
		i3UIControl* pObj = ObjList[0];
		return ADDUNDO_Transform( pObj);
	}
	else
	{
		bool bResult = true;
		
		BeginBatch();
		for( size_t i=0; i< nCount; i++)
		{
			i3UIControl* pObj = ObjList[i];
			bResult &= ADDUNDO_Transform( pObj);
		}
		EndBatch();

		return bResult;
	}
}

bool UIUndoSystem::ADDUNDO_Edit( i3UIControl * pObj)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	UIUndoInfo * pInfo = UIUndoInfoEdit::new_object();
	
	pInfo->setObject( pObj);

	AddUndoInfo( pInfo);

	return true;
}

bool UIUndoSystem::ADDUNDO_Edit( const i3::vector<i3UIControl*>&  ObjList)
{
	if( !m_isUserAction)			return false;
	if( m_bPendingEnable == true)	return false;

	size_t nCount = ObjList.size();

	if( nCount <= 0)
	{
		return false;
	}
	else if( nCount == 1)
	{
		i3UIControl* pObj = ObjList[0];
		return ADDUNDO_Edit( pObj);
	}
	else
	{
		bool bResult = true;
		
		BeginBatch();
		for( size_t i=0; i< nCount; i++)
		{
			i3UIControl* pObj = ObjList[i];
			bResult &= ADDUNDO_Edit( pObj);
		}
		EndBatch();

		return bResult;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for namespace UIUndo
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
UIUndoSystem * g_pUIControlUndoSystem = NULL;

UIUndoSystem* UIUndo::getUndoSystem( void)
{
	return g_pUIControlUndoSystem;	
}


bool UIUndo::Init(i3UIScene * pScene)
{
	Close();

	if( pScene != NULL)
	{
		g_pUIControlUndoSystem = UIUndoSystem::new_object();

		g_pUIControlUndoSystem->SetCurrentScene( pScene);

		return true;
	}

	return false;
}

bool UIUndo::Reset(void)
{
	g_pUIControlUndoSystem->ResetSystem();

	return true;
}

bool UIUndo::PendingEnable(bool bPending)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->SetPendingEnable( bPending);
	}

	return false;
}

bool UIUndo::Close(void)
{
	I3_SAFE_RELEASE( g_pUIControlUndoSystem);

	return true;
}

bool UIUndo::BeginBatch(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		g_pUIControlUndoSystem->BeginBatch();

		return true;
	}

	return false;
}

bool UIUndo::EndBatch(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		g_pUIControlUndoSystem->EndBatch();

		return true;
	}

	return false;
}

bool UIUndo::Undo(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		g_pUIControlUndoSystem->Undo();

		return true;
	}

	return false;
}

bool UIUndo::Redo(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		g_pUIControlUndoSystem->Redo();

		return true;
	}

	return false;
}

INT32 UIUndo::GetUndoCount(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->getUndoCount();
	}

	return 0;
}

INT32 UIUndo::GetRedoCount(void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->getRedoCount();
	}

	return 0;
}

bool UIUndo::ADD_Create( i3UIControl * pObj)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Create( pObj);
	}

	return false;
}

bool UIUndo::ADD_Create( const i3::vector<i3UIControl*>& ObjList)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Create( ObjList);
	}

	return false;
}

bool UIUndo::ADD_Delete( i3UIControl * pObj)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Delete( pObj);
	}

	return false;
}

bool UIUndo::ADD_Delete( const i3::vector<i3UIControl*>& ObjList)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Delete( ObjList);
	}

	return false;
}

bool UIUndo::ADD_Transform( i3UIControl * pObj)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Transform( pObj);
	}

	return false;
}

bool UIUndo::ADD_Transform( const i3::vector<i3UIControl*>& ObjList)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Transform( ObjList);
	}

	return false;
}

bool UIUndo::ADD_Edit( i3UIControl * pObj)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Edit( pObj);
	}

	return false;
}

bool UIUndo::ADD_Edit( const i3::vector<i3UIControl*>& ObjList)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		return g_pUIControlUndoSystem->ADDUNDO_Edit( ObjList);
	}

	return false;
}

#ifdef _DEBUG
void UIUndo::TraceCurrentStack( void)
{
	if( g_pUIControlUndoSystem != NULL)
	{
		I3TRACE("----------------------------------------\n");
		
		const i3::vector<UIUndoInfo*>& UndoList = g_pUIControlUndoSystem->getUndoList();
		for( size_t i=0; i< UndoList.size(); i++)
		{
			I3TRACE( "%d: ", i);
			UIUndoInfo* pInfo = UndoList[i];
			pInfo->PrintUndoInfo();
		}

		I3TRACE("----------------------------------------\n");
		
		const i3::vector<UIUndoInfo*>& RedoList = g_pUIControlUndoSystem->getRedoList();
		for( size_t i=0; i< RedoList.size(); i++)
		{
			I3TRACE( "%d: ", i);
			UIUndoInfo* pInfo = RedoList[i];
			pInfo->PrintUndoInfo();
		}
		
		I3TRACE("----------------------------------------\n");
	}
}
#endif

//bool UIUndo::Create( i3UIControl* pObj)
//{
//	i3UIControl* pCtrl = i3UI::CreateControl( pObj);
//	I3ASSERT( pCtrl != NULL);
//
//	if( pCtrl != NULL)
//		return ADD_Create( pObj);
//
//	return false;
//}
//
//bool UIUndo::Delete( i3UIControl* pObj)
//{
//	bool bResult = i3UI::DeleteControl( pObj);
//	I3ASSERT( bRes == true);
//	
//	if( bRes == true)
//		return ADD_Delete( pObj);
//
//	return false;
//}


//#endif