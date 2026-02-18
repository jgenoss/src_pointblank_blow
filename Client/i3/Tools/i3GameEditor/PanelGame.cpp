// PanelGame.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "PanelGame.h"
#include ".\panelgame.h"
#include "MainFrm.h"
#include "i3Base/smart_ptr/make_shared.h"


// PanelGame dialog

IMPLEMENT_DYNAMIC(PanelGame, i3TDKDialogBase)
PanelGame::PanelGame(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(PanelGame::IDD, pParent)
{
	m_nAddStageNum = 0;
	m_nAddLayerNum = 0;

	m_hGameRoot = NULL;
	m_hStagesRoot = NULL;
	m_hLayersRoot = NULL;

	m_hCurItem = NULL;
	//m_pSelectedItemData = NULL;
}

PanelGame::~PanelGame()
{
}

BOOL PanelGame::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN)
	{
		if( pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE ||
			GetKeyState( VK_CONTROL))
		{
			::TranslateMessage( pMsg);
			::DispatchMessage( pMsg);
			return TRUE;
		}
	}

	return i3TDKDialogBase::PreTranslateMessage( pMsg);
}

void PanelGame::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RSC_TREE, m_TreeCtrl);
}

void PanelGame::ResetGame( BOOL bFrameworkClear)
{
	char * pszGameFramework = DEFAULT_GAME_NAME;
	char * pszStagesRoot = ROOT_STAGES_GROUP;
	char * pszLayersRoot = ROOT_LAYERS_GROUP;

	m_nAddStageNum = 0;
	m_nAddLayerNum = 0;

	//	Get Game Framework	
	__CHECKF_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//	All clear
	if( bFrameworkClear == TRUE)
	{
		g_pGameFramework->RemoveAllStageInfo();
		g_pGameFramework->RemoveAllLayer();

		g_pGameFramework->SetName( pszGameFramework);
	}

	m_TreeCtrl.DeleteAllItems();
	
	if( g_pGameFramework->GetName())
		pszGameFramework = (char *) g_pGameFramework->GetName();

	//	Add Game Framework
	m_hGameRoot = m_TreeCtrl.InsertItem( pszGameFramework, TVI_ROOT, TVI_LAST);
	__CHECKF_RT( m_hGameRoot == NULL, "Error! Game Root cannot insert.");	

	m_TreeCtrl.SetItemData( m_hGameRoot, (DWORD_PTR) g_pGameFramework);

	//	Add Stages Group
	m_hStagesRoot = m_TreeCtrl.InsertItem( pszStagesRoot, m_hGameRoot, TVI_LAST);
	__CHECKF_RT( m_hStagesRoot == NULL, "Error! Stages root cannot insert.");	

	//	Add Layers Group
	m_hLayersRoot = m_TreeCtrl.InsertItem( pszLayersRoot, m_hGameRoot, TVI_LAST);
	__CHECKF_RT( m_hLayersRoot == NULL, "Error! Layers root cannot insert.");	

	m_TreeCtrl.EnsureVisible( m_hStagesRoot);
	m_TreeCtrl.EnsureVisible( m_hLayersRoot);

}

void PanelGame::BuildTreeCtrl( i3Framework * pFramework)
{	//	i3Framework의 정보로 TreeView를 구성한다. (이전 Tree 아이템들은 삭제한다.)
	INT32 i = 0;
	i3StageInfo *	pAddStageInfo = NULL;
	i3SgLayer *		pAddLayer = NULL;

	ResetGame( FALSE);

	for( i = 0; i < pFramework->getStageInfoCount(); i++)
	{
		pAddStageInfo = pFramework->getStageInfo( i);
		__CHECKW_RT( pAddStageInfo == NULL, "Warning! Invalid StageInfo. Cannot build tree with Game Framework.");

		_addStage( m_hStagesRoot, pAddStageInfo);

		m_nAddStageNum++;
	}
	
	for( i = 0; i < pFramework->getLayerCount(); i++)
	{
		pAddLayer = pFramework->getLayer( i);
		__CHECKW_RT( pAddLayer == NULL, "Warning! Invalid Layer. Cannot build tree with Game Framework.");

		_addLayer( m_hLayersRoot, pAddLayer);

		m_nAddLayerNum++;
	}
}

BOOL PanelGame::UpdateSelectedItem( void)
{
	i3ElementBase * pBaseData = (i3ElementBase *) GetSelectedItemData();
	__CHECKW_RTV( pBaseData == NULL, "Invalid data", FALSE);
	
	char * pszLabel = "";

	if( i3::kind_of<i3Framework*>(pBaseData)) //->IsTypeOf( i3Framework::static_meta()))			//	Framework Label
	{
		i3Framework * pGameFramework = (i3Framework *) pBaseData;
		pszLabel = (char *) pGameFramework->GetName();
	}
	else if( i3::kind_of<i3StageInfo*>(pBaseData)) //->IsTypeOf( i3StageInfo::static_meta()))		//	StageInfo Label
	{
		i3StageInfo * pStageInfo = (i3StageInfo *) pBaseData;
		pszLabel = (char *) pStageInfo->GetName();
	}
	else if( i3::kind_of<i3SgLayer*>(pBaseData)) //->IsTypeOf( i3SgLayer::static_meta()))		//	Layer Label
	{
		i3SgLayer * pLayer = (i3SgLayer *) pBaseData;
		pszLabel = (char *) pLayer->GetName();
	}
	else if( i3::kind_of<i3StageInstanceInfo*>(pBaseData)) //->IsTypeOf( i3StageInstanceInfo::static_meta()))
	{
		i3StageInstanceInfo * pInfo = ( i3StageInstanceInfo *)pBaseData;
		pszLabel = (char *)pInfo->GetName();
	}
    
	return m_TreeCtrl.SetItemText( m_hCurItem, pszLabel);
}

UINT32 PanelGame::_trackPopupMenu( CPoint point)
{
	UINT32 Cmd = 0;
	CMenu Menu, * pMenuPopup;
	i3ElementBase * pSelObj		= NULL;
	i3ElementBase * pParentObj	= NULL;

	HTREEITEM pSelectedItem = m_TreeCtrl.GetSelectedItem();
	HTREEITEM pParentItem = m_TreeCtrl.GetParentItem( pSelectedItem);

	if( pSelectedItem != NULL )
	{
		pSelObj= (i3ElementBase *)m_TreeCtrl.GetItemData( pSelectedItem);
	}

	if( pParentItem != NULL)
	{
		pParentObj = (i3ElementBase *)m_TreeCtrl.GetItemData( pParentItem);
	}

	if( pSelectedItem == m_hGameRoot)
		return 0;
	else if( pSelectedItem == m_hStagesRoot || pParentItem == m_hStagesRoot)
		Menu.LoadMenu( IDR_POPUP_STAGE);
	else if ( pSelectedItem == m_hLayersRoot || pParentItem == m_hLayersRoot)
		Menu.LoadMenu( IDR_POPUP_LAYER);
	else if ( i3::same_of<i3StageInstanceInfo*>(pSelObj) //->IsExactTypeOf( i3StageInstanceInfo::static_meta()) 
		|| i3::same_of<i3StageInfo*>(pParentObj)) //->IsExactTypeOf( i3StageInfo::static_meta()))
	{
		Menu.LoadMenu( IDR_POPUP_INSTANCE);
	}
	else
		return 0;

	pMenuPopup = Menu.GetSubMenu( 0);

	//	선택가능한 메뉴 활성화
	if( pParentItem == m_hStagesRoot || pParentItem == m_hLayersRoot)
	{
		if( pParentItem == m_hStagesRoot)
		{
			pMenuPopup->EnableMenuItem( ID_POPUP_ADDSTAGE, MF_DISABLED | MF_GRAYED);
			pMenuPopup->EnableMenuItem( ID_POPUP_DELETESTAGE, MF_ENABLED);
		}
		else if( pParentItem == m_hLayersRoot)
		{
			pMenuPopup->EnableMenuItem( ID_POPUP_ADDLAYER, MF_DISABLED | MF_GRAYED);
			pMenuPopup->EnableMenuItem( ID_POPUP_DELETELAYER, MF_ENABLED);
		}		
	}
	else
	{
		if( pSelectedItem == m_hStagesRoot)
		{
			pMenuPopup->EnableMenuItem( ID_POPUP_ADDSTAGE, MF_ENABLED);

			if( m_TreeCtrl.ItemHasChildren( pSelectedItem))
			{
				pMenuPopup->EnableMenuItem( ID_POPUP_DELETESTAGE, MF_ENABLED);
			}
			else
			{
				pMenuPopup->EnableMenuItem( ID_POPUP_DELETESTAGE, MF_DISABLED | MF_GRAYED);
			}
		}
		else if( pSelectedItem == m_hLayersRoot)
		{
			pMenuPopup->EnableMenuItem( ID_POPUP_ADDLAYER, MF_ENABLED);

			if( m_TreeCtrl.ItemHasChildren( pSelectedItem))
				pMenuPopup->EnableMenuItem( ID_POPUP_DELETELAYER, MF_ENABLED);
			else
				pMenuPopup->EnableMenuItem( ID_POPUP_DELETELAYER, MF_DISABLED | MF_GRAYED);
		}
		//Stage 하위 리스트중에 Instance항목과 InstanceManagerList항목에대한 처리
		else if ( i3::same_of<i3StageInstanceInfo*>(pSelObj)) //->IsExactTypeOf( i3StageInstanceInfo::static_meta()) )
		{
			pMenuPopup->EnableMenuItem( ID_IDR_ADDNEWINSTANCE, MF_DISABLED | MF_GRAYED);
		}
		else if( i3::same_of<i3StageInfo*>(pParentObj)) //->IsExactTypeOf( i3StageInfo::static_meta()))
		{
			pMenuPopup->EnableMenuItem( ID_IDR_REMOVEINSTANCE, MF_DISABLED | MF_GRAYED);
		}
	}

	//	팝업 창을 띄운후 메뉴를 입력받는다.
	Cmd = pMenuPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
												point.x, point.y, (CWnd *)this, NULL);

	return Cmd;
}

void PanelGame::_clearPanel( void)
{
	HTREEITEM hNextItem;
	HTREEITEM hChildItem;
//	i3StageInfo * pStageInfo = NULL;
//	i3SgLayer * pLayer = NULL;
	
	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//Release All Stages
	{
		hChildItem = m_TreeCtrl.GetChildItem( m_hStagesRoot);
		
		while( hChildItem != NULL)
		{
			hNextItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);

			//	Tree에사 Item 제거
			m_TreeCtrl.DeleteItem( hChildItem);	

			hChildItem = hNextItem;
		}

		g_pGameFramework->RemoveAllStageInfo();
	}
	//	Release All Layers
	{
		hChildItem = m_TreeCtrl.GetChildItem( m_hLayersRoot);
		
		while( hChildItem != NULL)
		{
			hNextItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);

			//	Tree에 Item 제거
			m_TreeCtrl.DeleteItem( hChildItem);	

			hChildItem = hNextItem;
		}

		g_pGameFramework->RemoveAllLayer();
	}
}

void PanelGame::_addNewInsMng( HTREEITEM pTreeItem)
{
//	HTREEITEM pNewItem = NULL;
	
	i3AnyData* pListElem = ( i3AnyData*)m_TreeCtrl.GetItemData( pTreeItem);
	I3ASSERT( pListElem != NULL);
	
	StageInstanceInfoList** ppList = AnyDataCast<StageInstanceInfoList*>( pListElem );
	I3ASSERT( ppList != NULL);

	StageInstanceInfoList* pList = *ppList;

	{
		i3StageInstanceInfo * pNew = i3StageInstanceInfo::new_object();
		pNew->SetClassNameByMeta( i3GameNode::static_meta());

		HTREEITEM hNew = m_TreeCtrl.InsertItem(pNew->GetName(), pTreeItem, TVI_LAST);
		I3ASSERT( hNew != NULL);

		m_TreeCtrl.SetItemData( hNew, (DWORD_PTR)pNew);

		pList->push_back( pNew);
	}
}

void PanelGame::_deleteInsMng( HTREEITEM hTreeItem)
{
	I3ASSERT( hTreeItem != NULL);

	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem( hTreeItem);
	i3AnyData* pListElem = (i3AnyData *)m_TreeCtrl.GetItemData( hParentItem);

	I3ASSERT( i3::same_of<i3AnyData*>(pListElem)); //->IsExactTypeOf( i3AnyData::static_meta()) );

	StageInstanceInfoList** ppList = AnyDataCast<StageInstanceInfoList*>(pListElem);
	I3ASSERT( ppList != NULL);

	StageInstanceInfoList* pList = *ppList;

	i3StageInstanceInfo * pInfo = (i3StageInstanceInfo *)m_TreeCtrl.GetItemData( hTreeItem);
	I3ASSERT( i3::same_of<i3StageInstanceInfo*>(pInfo)); //->IsExactTypeOf( i3StageInstanceInfo::static_meta()) );

	// pList->Remove( pInfo);
	i3::vu::remove(*pList, pInfo);
	m_TreeCtrl.DeleteItem( hTreeItem);

	I3_SAFE_RELEASE( pInfo);
}

void PanelGame::_addNewStage( HTREEITEM pTreeItem, char * pszName)
{
//	HTREEITEM		pNewItem = NULL;
	i3StageInfo *	pNewStage = NULL;
	char			szStageName[256];	

	__CHECKW_RT( pTreeItem == NULL, "Invalid Stage...");	
	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//	New Stage create
	pNewStage = i3StageInfo::new_object();	
	__CHECKW_RT( pNewStage == NULL, "Fail to created new stage.");

	//	StageInfo 삽입
	g_pGameFramework->AddStageInfo( pNewStage);

	if( pszName == NULL)
	{
		sprintf( szStageName, "Stage (%d)", ++m_nAddStageNum);
	}
	else
	{
		i3::safe_string_copy( szStageName, pszName, 256 );
	}
	
	//	StageInfo 이름지정
	pNewStage->SetName( szStageName);
	
	_addStage( pTreeItem, pNewStage);
	/*
	//	Tree Last에 item 삽입
	pNewItem = m_TreeCtrl.InsertItem( szStageName, pTreeItem, TVI_LAST);
	__CHECKW_RT( pNewItem == NULL, "Fail to insert new stage.");
	
	//	Tree에 데이타 삽입
	m_TreeCtrl.SetItemData( pNewItem, (DWORD_PTR) pNewStage);
	
	m_TreeCtrl.EnsureVisible( pNewItem);
	*/
	I3_SAFE_RELEASE( pNewStage);
}

void PanelGame::_addStage( HTREEITEM pTreeItem, i3StageInfo * pStageInfo)
{
	HTREEITEM		pNewItem = NULL;	

	__CHECKW_RT( (pTreeItem == NULL) && (pStageInfo == NULL), "Invalid Stage...");	

	//	Tree Last에 Item 삽입
	pNewItem = m_TreeCtrl.InsertItem( pStageInfo->GetName(), pTreeItem, TVI_LAST);
	__CHECKW_RT( pNewItem == NULL, "Fail to insert new stage.");
	
	//	Tree에 데이타 삽입
	m_TreeCtrl.SetItemData( pNewItem, (DWORD_PTR) pStageInfo);

	m_TreeCtrl.EnsureVisible( pNewItem);

	_addInstancelist( pNewItem, pStageInfo);
}

void PanelGame::_addInstancelist( HTREEITEM pTreeItem, i3StageInfo * pStageInfo)
{
	I3ASSERT( pStageInfo != NULL);
	I3ASSERT( pTreeItem != NULL);

	// Prepare for add instance
//	StageInstanceInfoList* pInstList = ;
	
	if( pStageInfo->getInstanceManagerInfoListPtr().get() == NULL)
	{
		pStageInfo->setInstanceManagerInfoList( i3::make_shared<StageInstanceInfoList>() );
//		pInstList = pStageInfo->getInstanceManagerInfoListPtr().get(); 		//List::new_object();
	}
	
	i3AnyData* pAnyInstList = pStageInfo->getElementInstanceManagerInfoList();
	StageInstanceInfoList** ppInstList = AnyDataCast<StageInstanceInfoList*>(pAnyInstList);
	I3ASSERT(ppInstList != NULL);
	
	StageInstanceInfoList* pInstList = *ppInstList;

    HTREEITEM hChildItem = m_TreeCtrl.GetNextItem( pTreeItem, TVGN_CHILD);
	HTREEITEM hInsRoot = NULL;
	while( hChildItem != NULL)
	{
		i3ElementBase * pItem = (i3ElementBase*)m_TreeCtrl.GetItemData( hChildItem);

		if(pItem != NULL && i3::same_of<i3AnyData*>(pItem)) //->IsExactTypeOf( i3AnyData::static_meta()) ) 
		{
			i3AnyData* pAny = (i3AnyData*)pItem;
			if (pAny->type() == typeid( StageInstanceInfoList*) )
			{
				hInsRoot	= hChildItem;
				hChildItem	= NULL;
			}
			else
			{
				I3ASSERT(0);
			}
		}
		else
		{
			hChildItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);
		}
	}

	if( hInsRoot == NULL)
	{
		hInsRoot = m_TreeCtrl.InsertItem( "Instance", pTreeItem, TVI_LAST);
		__CHECKW_RT( hInsRoot == NULL, "Fail to insert Instance Root.");
		
		//	Tree에 데이타 삽입
		m_TreeCtrl.SetItemData( hInsRoot, (DWORD_PTR) pAnyInstList);
	}

	size_t nInsMngCount = pInstList->size();
	for( size_t i = 0; i< nInsMngCount; ++i)
	{
		i3StageInstanceInfo * pChild = (*pInstList)[i];

		_addInsMng( hInsRoot, pChild);
	}
}

void PanelGame::_addInsMng( HTREEITEM pTreeItem, i3StageInstanceInfo * pInfo)
{
	HTREEITEM hNew = m_TreeCtrl.InsertItem( pInfo->GetName(), pTreeItem, TVI_LAST);
	I3ASSERT( hNew != NULL);

	m_TreeCtrl.SetItemData( hNew, (DWORD_PTR)pInfo);
}

void PanelGame::_deleteStage( HTREEITEM pTreeItem, BOOL bQuestion, BOOL bUpdateSpec)
{
	HTREEITEM		pParentItem = NULL;
//	i3StageInfo *	pStageInfo = NULL;
	BOOL			ret = FALSE;

	__CHECKW_RT( pTreeItem == NULL, "Invalid Stage.");

	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//	삭제 대상에 Child가 존재하면 유저에게 물어본다. 단독이라면 묻지 않는다.
	if( m_TreeCtrl.ItemHasChildren( pTreeItem) == TRUE && bQuestion == TRUE)
	{
		if( MessageBox( "하위 모든 내용을 삭제하시겠습니까?", "WARNING", MB_YESNO | MB_ICONWARNING) == IDNO)
			return;
	}

	//	Stage Root면 하위 모든 Stage를 지운다.
	if( pTreeItem == m_hStagesRoot)
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_TreeCtrl.GetChildItem( m_hStagesRoot);
		
		while( hChildItem != NULL)
		{
			hNextItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);

			//	Tree에사 Item 제거
			m_TreeCtrl.DeleteItem( hChildItem);	

			hChildItem = hNextItem;
		}

		g_pGameFramework->RemoveAllStageInfo();
		g_pGameFramework->SetStartStageInfo( NULL);
	}
	//	개별 선택된 Stage만 지운다.
	else
	{
		pParentItem = m_TreeCtrl.GetParentItem( pTreeItem);
		__CHECKW_RT( pParentItem != m_hStagesRoot, "This not Stage Root.");
				
		i3StageInfo * pStageInfo = (i3StageInfo *) m_TreeCtrl.GetItemData( pTreeItem);
		__CHECKW_RT( pStageInfo == NULL, "Invalid Stage.");
		__CHECKW_RT( !i3::kind_of<i3StageInfo*>(pStageInfo) //->IsTypeOf( i3StageInfo::static_meta()) == FALSE
			, "Not Stage.");
				
		//	Tree에 데이타 제거
		g_pGameFramework->RemoveStageInfo( pStageInfo);	

		//	Tree에사 Item 제거
		ret = m_TreeCtrl.DeleteItem( pTreeItem);
		__CHECKW_RT( ret == FALSE, "Fail to deleted stage.");

		m_TreeCtrl.EnsureVisible( pParentItem);

	}

	//	삭제된 Stage중 NextStage로 연결되어 있는 Stage가 있다면 수정한다.
	{
		for( INT32 i = 0; i < g_pGameFramework->getStageInfoCount(); i++)
		{
			i3StageInfo * pCurStage = g_pGameFramework->getStageInfo( i);
			i3StageInfo * pNextStage = pCurStage->getNextStage();

			//	NextStage로 설정되어 있는 Stage가 있다면 실제로 존재하는지 체크하여 정리
			if( pNextStage)
			{
				i3StageInfo * pCheckStage = NULL;
				i3StageInfo * pLinkStage = NULL;

				for( INT32 j = 0; j < g_pGameFramework->getStageInfoCount(); j++)
				{
					pCheckStage = g_pGameFramework->getStageInfo( j);

					if( pNextStage == pCheckStage)
					{
						pLinkStage = pCheckStage;
						break;
					}
				}

				pCurStage->setNextStage( pLinkStage);
			}			
		}
	}

	//	GameFramework의 StartStage가 실제로 존재하는지 체크
	{
		i3StageInfo * pCurStartStage = g_pGameFramework->GetStartStageInfo();

		if( pCurStartStage)
		{
			i3StageInfo * pCheckStage = NULL;
			i3StageInfo * pLinkStage = NULL;

			for( INT32 i = 0; i < g_pGameFramework->getStageInfoCount(); i++)
			{
				pCheckStage = g_pGameFramework->getStageInfo( i);
				if( pCurStartStage == pCheckStage)
				{
					pLinkStage = pCheckStage;
					break;
				}
			}

			g_pGameFramework->SetStartStageInfo( pLinkStage);
		}
	}

	m_hCurItem = m_TreeCtrl.GetSelectedItem();

	//	Property 화면 갱신
	if( bUpdateSpec)
	{
		i3ElementBase * pBaseData = (i3ElementBase *) GetItemData( m_hCurItem);	//	NULL이면 빈페이지다.
		theApp.GetMainFrm()->m_PanelSpec.SelectPage( pBaseData);
	}
}

void PanelGame::_addNewLayer( HTREEITEM pTreeItem, char * pszName)
{	
	HTREEITEM pNewItem = NULL;
	i3SgLayer * pNewLayer = NULL;
	char szLayerName[256];	

	__CHECKW_RT( pTreeItem == NULL, "Invalid Layer...");	
	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//	New Layer create
	pNewLayer = i3SgLayer::new_object();	
	__CHECKW_RT( pNewLayer == NULL, "Fail to created new layer.");

	//	Layer 삽입
	g_pGameFramework->AddLayer( pNewLayer);

	if( pszName == NULL)
	{
		sprintf( szLayerName, "Layer (%d)", ++m_nAddLayerNum);
	}
	else
	{
		i3::safe_string_copy( szLayerName, pszName, 256 );
	}
	
	//	Layer 이름지정
	pNewLayer->SetName( szLayerName);
		
	//	Tree에 삽입
	pNewItem = m_TreeCtrl.InsertItem( szLayerName, pTreeItem, TVI_LAST);
	__CHECKW_RT( pNewItem == NULL, "Fail to insert new layer.");
	
	//	Tree에 데이타 삽입
	m_TreeCtrl.SetItemData( pNewItem, (DWORD_PTR) pNewLayer);

	m_TreeCtrl.EnsureVisible( pNewItem);
	I3_SAFE_RELEASE( pNewLayer);
}

void PanelGame::_addLayer( HTREEITEM pTreeItem, i3SgLayer * pLayer)
{
	HTREEITEM		pNewItem = NULL;	

	__CHECKW_RT( (pTreeItem == NULL) && (pLayer == NULL), "Invalid Layer...");
	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	//	Tree에 삽입
	pNewItem = m_TreeCtrl.InsertItem( pLayer->GetName(), pTreeItem, TVI_LAST);
	__CHECKW_RT( pNewItem == NULL, "Fail to insert new layer.");

	//	Tree에 데이타 삽입	
	m_TreeCtrl.SetItemData( pNewItem, (DWORD_PTR) pLayer);

	m_TreeCtrl.EnsureVisible( pNewItem);
}

void PanelGame::_deleteLayer( HTREEITEM pTreeItem, BOOL bQuestion, BOOL bUpdateSpec)
{
	HTREEITEM		pParentItem = NULL;
//	i3SgLayer *		pLayer = NULL;
	BOOL			ret = FALSE;

	__CHECKW_RT( pTreeItem == NULL, "Invalid Layer.");
	__CHECKW_RT( g_pGameFramework == NULL, "Error! Game Framework is NULL.");

	if( m_TreeCtrl.ItemHasChildren( pTreeItem) == TRUE && bQuestion == TRUE)
	{
		if( MessageBox( "하위 모든 내용을 삭제하시겠습니까?", "WARNING", MB_YESNO | MB_ICONWARNING) == IDNO)
			return;
	}

	//	Layers Root 하위 모든 Layer를 지운다.
	if( pTreeItem == m_hLayersRoot)
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_TreeCtrl.GetChildItem( m_hLayersRoot);
		
		while( hChildItem != NULL)
		{
			hNextItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);

			//	Tree에 데이타 제거
			m_TreeCtrl.DeleteItem( hChildItem);	

			hChildItem = hNextItem;
		}

		g_pGameFramework->RemoveAllLayer();
	}
	//	개별 선택된 Layer만 지운다.
	else
	{
		pParentItem = m_TreeCtrl.GetParentItem( pTreeItem);
		__CHECKW_RT( pParentItem != m_hLayersRoot, "This not Layer Root.");

		// 선택한 Layer
		i3SgLayer * pLayer = (i3SgLayer *) m_TreeCtrl.GetItemData( pTreeItem);
		__CHECKW_RT( pLayer == NULL, "Invalid Layer.");
		__CHECKW_RT( !i3::kind_of<i3SgLayer*>(pLayer) //->IsTypeOf( i3SgLayer::static_meta()) == FALSE
			, "Not Layer.");
		
		g_pGameFramework->RemoveLayer( pLayer);

		ret = m_TreeCtrl.DeleteItem( pTreeItem);
		__CHECKW_RT( ret == FALSE, "Fail to deleted layer.");

		m_TreeCtrl.EnsureVisible( pParentItem);
	}

	m_hCurItem = m_TreeCtrl.GetSelectedItem();

	//	Property 화면 갱신
	if( bUpdateSpec)
	{
		i3ElementBase * pBaseData = (i3ElementBase *) GetItemData( m_hCurItem);	//	NULL이면 빈페이지다.
		theApp.GetMainFrm()->m_PanelSpec.SelectPage( pBaseData);
	}
}

HTREEITEM PanelGame::_setCurItem( void)
{
	HTREEITEM pSelectedItem = NULL;
	POINT CurMousePos, pt;
	UINT nFlag = 0;

	GetCursorPos( &CurMousePos);

	pt = CurMousePos;

	m_TreeCtrl.ScreenToClient( &pt);
	
	//	현재 마우스 좌표의 현재 선택된 아이템
	pSelectedItem = m_TreeCtrl.HitTest( pt, &nFlag);

	if( (pSelectedItem != NULL) && (nFlag & TVHT_ONITEM ))
	{
		m_TreeCtrl.Select( pSelectedItem, TVGN_CARET);

		//	현재 선택된 아이템
		m_hCurItem = m_TreeCtrl.GetSelectedItem();
		//m_pSelectedItemData = (ItemData *) m_TreeCtrl.GetItemData( m_hCurItem);
	}
	else
	{
		m_hCurItem = NULL;
	//	m_pSelectedItemData = NULL;
	}

	return pSelectedItem;
}


BEGIN_MESSAGE_MAP(PanelGame, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_RSC_TREE, OnTvnEndlabeleditRscTree)
	ON_NOTIFY(NM_RCLICK, IDC_RSC_TREE, OnNMRclickRscTree)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_RSC_TREE, OnNMClickRscTree)
	ON_WM_CREATE()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PanelGame message handlers

BOOL PanelGame::OnInitDialog()
{	
	i3TDKDialogBase::OnInitDialog();

	{
		m_TreeCtrl.SetBkColor( RGB(255, 255, 255));

		//	트리 초기화
		ResetGame();		
	}
			
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int PanelGame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

void PanelGame::OnDestroy()
{
	i3TDKDialogBase::OnDestroy();

	_clearPanel();	//	메모리 제거
		
}

void PanelGame::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void PanelGame::OnTvnEndlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void PanelGame::OnNMRclickRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	//	마우스 좌표에 있는 아이템을 강제로 선택
	_setCurItem();
	
	//	Spec 창 갱신
	i3ElementBase * pBaseData = (i3ElementBase *) GetItemData( m_hCurItem);	//	NULL인 경우 에러아님. NULL인 Item은 빈 페이지로

	theApp.GetMainFrm()->m_PanelSpec.SelectPage( pBaseData);
	
	//HTREEITEM hCurItem = m_TreeCtrl.GetNextItem( m_hItemRoot, TVGN_CARET);
	//m_TreeCtrl.SelectItem( hCurItem);	
}

void PanelGame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//if( m_TreeCtrl.ItemHasChildren( m_hItemRoot))
	{
		HTREEITEM hCurItem;

		//	현재 선택된 노드
		hCurItem = m_TreeCtrl.GetSelectedItem();//m_TreeCtrl.GetNextItem( m_hItemRoot, TVGN_CARET);
		
		if( hCurItem != NULL)
		{
			//	선택한 팝업 메뉴의 ID
			UINT32 nSelectedMenuID = _trackPopupMenu( point);

			switch( nSelectedMenuID)
			{
			case ID_POPUP_ADDSTAGE:			_addNewStage( m_hStagesRoot);			break;
			case ID_POPUP_DELETESTAGE:		_deleteStage( hCurItem);				break;
			case ID_POPUP_ADDLAYER:			_addNewLayer( m_hLayersRoot);			break;
			case ID_POPUP_DELETELAYER:		_deleteLayer( hCurItem);				break;
			case ID_IDR_ADDNEWINSTANCE:		_addNewInsMng( hCurItem);				break;
			case ID_IDR_REMOVEINSTANCE:		_deleteInsMng( hCurItem);				break;
			}  
		}
	
		/*HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_TreeCtrl.GetChildItem( m_hItemRoot);

		while( hChildItem != NULL)
		{
			hNextItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);

			hChildItem = hNextItem;
		}		
		*/
	}
}

void PanelGame::OnNMClickRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	SetFocus();

	_setCurItem();	//	마우스 좌표에 있는 아이템을 강제로 선택

	//	Spec 창 갱신
	i3ElementBase * pBaseData = (i3ElementBase *) GetItemData( m_hCurItem);	//	NULL인 경우 에러아님. NULL인 Item은 빈 페이지로

	theApp.GetMainFrm()->m_PanelSpec.SelectPage( pBaseData);
}

LRESULT PanelGame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}
