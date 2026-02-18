// UIScreen.cpp : implementation file
//

#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneScreen.h"
#include "../MainFrm.h"
#include "../i3UIEditorView.h"
#include "../UIGlobalRes.h"

#include "../ScriptManager.h"

#include "../UIPaneDefines.h"

#include "../UndoSystem2/UIUndoSystem.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"


IMPLEMENT_DYNAMIC(CUIPaneScreen, CUIPaneBase)



CUIPaneScreen::CUIPaneScreen()
{
	m_pScene		= NULL;
	m_pRootItem		= NULL;

	m_hSrcItem		= NULL;
	m_hTargetItem	= NULL;
	m_bDragging		= false;
	m_bDropEnable	= false;
	m_pDragImage	= NULL;
//	m_OldClassCursor= NULL;
	m_bSkipMessage  = false;

	m_bHovering = false;
}

CUIPaneScreen::~CUIPaneScreen()
{
	Reset();

	I3_SAFE_RELEASE( m_pScene);
}

void CUIPaneScreen::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
//	DDX_Control(pDX, ID_SCREEN_TREECTRL, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CUIPaneScreen, CUIPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( TVN_SELCHANGED, ID_SCREEN_TREECTRL, OnTVNSelChanged)
	ON_NOTIFY( NM_RCLICK, ID_SCREEN_TREECTRL, OnRClickTree)
	ON_NOTIFY( TVN_BEGINDRAG, ID_SCREEN_TREECTRL, OnTVNBeginDrag)
	ON_NOTIFY( TVN_ENDLABELEDIT, ID_SCREEN_TREECTRL, OnTVNEndLabelEdit)
	ON_NOTIFY( TVN_KEYDOWN, ID_SCREEN_TREECTRL, OnTVNKeyDown)
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CLICK, ID_SCREEN_TREECTRL, &CUIPaneScreen::OnNMClickSceneTree)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_SYSKEYDOWN()
END_MESSAGE_MAP()


// CUIPaneScreen message handlers

static BOOL _setItemStatetoEdit( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3ElementBase * pItem	= (i3ElementBase*)pCtrl->GetItemData( hItem);

//	if( pItem == NULL)	return TRUE;
	if( !i3::kind_of<i3UIControl*>(pItem)) //->IsTypeOf( i3UIControl::static_meta()) == FALSE)	
		return TRUE;

	i3UIControl * pControl = (i3UIControl*)pItem;

	INT32 iImage	= pCtrl->GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;

	if( pControl->isEnable() != (iImage == 2))
	{
		pControl->EnableCtrl( iImage == 2);
	}

	if( iImage != 2 && iImage != 3)
	{
		I3TRACE("invalid itemstate image mask\n");
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

int CUIPaneScreen::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	// Tree Control
	{
		CRect rect;

		GetClientRect( &rect);

		// TVS_CHECKBOXES가 들어가게 되면 State image의 동작에 영향을 주게 됩니다. (TVIS_STATEIMAGEMASK로 검색)
		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_TRACKSELECT | TVS_FULLROWSELECT | TVS_SINGLEEXPAND |  /*TVS_CHECKBOXES |*/ 
								TVS_INFOTIP | TVS_LINESATROOT | TVS_EDITLABELS,
								rect, this, ID_SCREEN_TREECTRL) == FALSE)
		{
			I3ASSERT_0;
		}

		m_TreeCtrl.ModifyStyleEx( 0, TVS_EX_FADEINOUTEXPANDOS);
		m_TreeCtrl.InitImageList();
	}

	i3TDK::RegisterUpdate( m_hWnd, i3UIScene::static_meta(), I3_TDK_UPDATE_SET);

	UINT32 flag = I3_TDK_UPDATE_ADD | I3_TDK_UPDATE_REMOVE | I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT;
	i3TDK::RegisterUpdate( m_hWnd, i3UIControl::static_meta(), flag);
	i3TDK::RegisterUpdate( m_hWnd, i3UIScene::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT);

	return 0;
}

void CUIPaneScreen::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( pInfo->m_pObject != NULL)
		{
			if( i3::same_of<i3UIScene*>(pInfo->m_pObject)) //->IsExactTypeOf( i3UIScene::static_meta()))
			{
				SetScene( (i3UIScene*)pInfo->m_pObject);
			}
		}
		else
		{
			I3ASSERT( pInfo->m_pMeta != NULL);
			if( i3::same_of<i3UILibrary*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3UILibrary::static_meta()))
				SetScene( NULL);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		I3ASSERT( pInfo->m_pObject != NULL);
		if( i3::kind_of<i3UIControl*>(pInfo->m_pObject)) //->IsTypeOf( i3UIControl::static_meta()) )
		{
			AddControl( (i3UIControl*)pInfo->m_pObject);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		I3ASSERT( pInfo->m_pObject != NULL);
		if( i3::kind_of<i3UIControl*>(pInfo->m_pObject)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			RemoveControl( (i3UIControl*)pInfo->m_pObject);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		I3ASSERT( pInfo->m_pObject != NULL);
		if( i3::kind_of<i3UIControl*>(pInfo->m_pObject)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, pInfo->m_pObject);
			if( hItem != NULL)
			{
				m_bSkipMessage = true;
				m_TreeCtrl.SelectItem(hItem);
				m_TreeCtrl.SetItemState( hItem, TVIS_SELECTED | TVIS_BOLD | TVIS_DROPHILITED, TVIS_SELECTED | TVIS_BOLD | TVIS_DROPHILITED);
				m_bSkipMessage = false;
			}

			//I3TRACE("SELECT ITEM\n");
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		if( pInfo->m_pObject != NULL)
		{
			if( i3::kind_of<i3UIControl*>(pInfo->m_pObject)) //->IsTypeOf( i3UIControl::static_meta()) )
			{
				HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, pInfo->m_pObject);
				if( hItem != NULL)
				{
					m_bSkipMessage = true;
					m_TreeCtrl.SetItemState( hItem, 0, TVIS_SELECTED | TVIS_BOLD | TVIS_DROPHILITED);
					m_bSkipMessage = false;
				}

				//I3TRACE("UNSELECT ITEM\n");
			}
		}
		else
		{
			UnselectAll();
		}
	}
}

LRESULT CUIPaneScreen::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDockablePane::WindowProc(message, wParam, lParam);
}


void CUIPaneScreen::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

void CUIPaneScreen::Reset()
{
	if( m_pScene != NULL)
	{
		m_pScene->RemoveAllChild();
	}
}

void CUIPaneScreen::SetScene( i3UIScene * pSceneRoot)
{
	m_TreeCtrl.DeleteAllItems();

	I3_REF_CHANGE( m_pScene, pSceneRoot);

	ScriptManager::SetScene( m_pScene);

	if( m_pScene == NULL)
		return;

	m_pRootItem = AddTreeItem( m_pScene, TVI_ROOT);

	i3UIControl * pChild = (i3UIControl*)pSceneRoot->getFirstChild();
	while( pChild != NULL)
	{
		if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			Rec_SetTree( m_pRootItem, pChild);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

void CUIPaneScreen::AdjustTreeItem( i3GameNode * pNode)
{
	if( i3::kind_of<i3UIControl*>(pNode)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl * pCtrl = (i3UIControl*) pNode;

		HTREEITEM hItem = FindItem( (i3UIControl*) pNode);

		if( hItem != NULL)
		{
			INT32 iImage	= m_TreeCtrl.GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;

			if( pCtrl->isEnable() != (iImage == 2))
			{
				pCtrl->EnableCtrl( iImage == 2, true);
			}

			if( iImage != 2 && iImage != 3)
			{
				I3TRACE("invalid itemstate image mask\n");
			}
		}
	}

}

void CUIPaneScreen::_SetTreeItem( TVITEM * pInfo, i3NamedElement * pNode, HTREEITEM hItem)
{
	i3ClassMeta * pMeta = pNode->meta();

	pInfo->mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	pInfo->hItem = hItem;
	pInfo->lParam = (LPARAM) pNode;

	if( pNode->hasName())	pInfo->pszText = (LPSTR)pNode->GetName();
	else					pInfo->pszText = (LPSTR)pMeta->class_name().c_str();

	pInfo->iImage = m_TreeCtrl.GetIconIndex( pNode);
	pInfo->iSelectedImage = pInfo->iImage + 1;
	pInfo->cChildren = 0;
}

HTREEITEM CUIPaneScreen::AddTreeItem( i3NamedElement * pNode, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	TVINSERTSTRUCT info;
	HTREEITEM hItem;

	info.hInsertAfter = hInsertAfter;
	info.hParent = hParent;
	
	_SetTreeItem( &info.item, pNode);

	hItem = m_TreeCtrl.InsertItem( &info);

	I3ASSERT( hItem != NULL);		// when? 

	if( i3::same_of<i3UIScene*>(pNode)) //->IsExactTypeOf( i3UIScene::static_meta()))
	{
		i3UIScene* pScene = (i3UIScene*)pNode;
		const char* scriptFile = pScene->getScriptFile();
		if( scriptFile == NULL)
		{
			m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
		}
		else
		{
			ScriptManager::SetScrScript( scriptFile);
			m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
		}
	}
	else if( i3::kind_of<i3UIControl*>(pNode)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		m_TreeCtrl.setItemStateImage( hItem, NULL);
	}

	if((hItem != NULL) && ((hParent != NULL) && (hParent != TVI_ROOT)))
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hParent;

		m_TreeCtrl.GetItem( &item);

		if( item.lParam != NULL)
		{
			//item.cChildren = getChildItemCount( (i3TreeNode *) item.lParam);
			item.cChildren = 1;
		}
		else
		{
			item.cChildren = 1;
		}

		m_TreeCtrl.SetItem( &item);
	}

	return hItem;
}

void CUIPaneScreen::Rec_InsertTree( HTREEITEM hParentItem, i3GameNode * pNode, HTREEITEM hInsertAfter)
{
	HTREEITEM hItem;

	hItem = AddTreeItem( pNode, hParentItem, hInsertAfter);

	i3UIControl * pChild = (i3UIControl*)pNode->getFirstChild();

	while( pChild != NULL)
	{
		//Loading시 ListView Item과 Header를 Box에 Bind
		if( i3::same_of<i3UIListView_Box*>(pNode)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
		{
			i3UIListView_Box* pLV = (i3UIListView_Box*)pNode;
			if( i3::same_of<i3UIListView_Item*>(pChild)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
			{
				i3UIListView_Item* pLVI = (i3UIListView_Item*)pChild;
				pLV->BindItem( pLVI);	
			}
			else if( i3::same_of<i3UIListView_Header*>(pChild)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))
			{
				i3UIListView_Header* pLVH = (i3UIListView_Header*)pChild;
				pLV->BindHeader( pLVH);
			}
		}

		if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			Rec_SetTree( hItem, pChild);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

void CUIPaneScreen::Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode)
{
	HTREEITEM hItem;

	hItem = AddTreeItem( pNode, hParentItem);

	i3UIControl * pChild = (i3UIControl*)pNode->getFirstChild();

	while( pChild != NULL)
	{
		//Loading시 ListView Item과 Header를 Box에 Bind
		if( i3::same_of<i3UIListView_Box*>(pNode)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
		{
			i3UIListView_Box* pLV = (i3UIListView_Box*)pNode;
			if( i3::same_of<i3UIListView_Item*>(pChild)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
			{
				i3UIListView_Item* pLVI = (i3UIListView_Item*)pChild;
				pLV->BindItem( pLVI);	
			}
			else if( i3::same_of<i3UIListView_Header*>(pChild)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))
			{
				i3UIListView_Header* pLVH = (i3UIListView_Header*)pChild;
				pLV->BindHeader( pLVH);
			}
		}

		if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
			Rec_SetTree( hItem, pChild);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

HTREEITEM CUIPaneScreen::FindItem( i3UIControl* pCtrl)
{
	HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, (i3GameNode*)pCtrl);

	return hItem;
}

HTREEITEM CUIPaneScreen::_FindItemByName( HTREEITEM hRoot, const char* pszName)
{
	HTREEITEM hItem = NULL;

	if( hRoot == NULL)
		return NULL;
	
	if( i3::generic_is_iequal( pszName, (const char*)m_TreeCtrl.GetItemText( hRoot) ) )
		hItem = hRoot;		// found!
	else
	{
		HTREEITEM hcur = m_TreeCtrl.GetChildItem( hRoot);
		while( hcur != NULL && hItem == NULL)
		{
			hItem = _FindItemByName( hcur, pszName);	// recursive

			hcur = m_TreeCtrl.GetNextItem(hcur, TVGN_NEXT);
		}
	}
	
	return hItem;
}

void CUIPaneScreen::_SetUniqueName( i3UIControl* pCtrl)
{
	I3ASSERT( pCtrl != NULL);

	char conv[MAX_PATH];
	i3::string_ncopy_nullpad( conv, pCtrl->meta()->class_name().c_str(), sizeof( conv));

	INT32 i=0;
	while( _FindItemByName( m_pRootItem, conv) != NULL)
	{
		i3::snprintf( conv, sizeof(conv), "%s%d", pCtrl->meta()->class_name().c_str(), i);
		i++;
	}
	
	pCtrl->SetName( conv);
}

INT32 CUIPaneScreen::AddControl( i3UIControl	*	pCtrl)
{
	I3ASSERT( pCtrl != NULL);

	if( pCtrl->getParent() != NULL)
	{
		HTREEITEM hItem = FindItem((i3UIControl*)pCtrl->getParent());
		if( hItem != NULL)
		{
			// finds and sets a default name for a control
			//_SetUniqueName( pCtrl);

			//AddTreeItem( pCtrl, hItem);
			Rec_SetTree( hItem, pCtrl);
			return 0;
		}
	}

	if( m_pScene != NULL)
	{
		// finds and sets a default name for a control
		//_SetUniqueName( pCtrl);

		//AddTreeItem( pCtrl, m_pRootItem);
		Rec_SetTree( m_pRootItem, pCtrl);

		return 0;
	}

	I3ASSERT_0;

	return 0;
}

INT32 CUIPaneScreen::RemoveControl( i3UIControl	*	pCtrl)
{
	HTREEITEM hItem = FindItem( pCtrl);

	m_TreeCtrl.DeleteItem( hItem);

	return 0;
}

INT32 CUIPaneScreen::RemoveControl( INT32 idx)
{
	return 0;
}

INT32 CUIPaneScreen::FindControl( i3UIControl *pCtrl)
{
	return 0;
}

BOOL CUIPaneScreen::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask = TVIF_HANDLE;
	data.hItem = hItem;

	return m_TreeCtrl.GetItem( &data);
}

void CUIPaneScreen::SetScript( HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);

	i3GameNode * pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( i3::same_of<i3UIScene*>(pNode)); //->IsExactTypeOf( i3UIScene::static_meta()));

	i3UIScene* pScene = (i3UIScene*)pNode;

	{
		TCHAR szFilters[] = "Lua 파일(*.lua)|*.lua";
		
		CFileDialog fileDlg(FALSE, _T("lua"), (LPCTSTR)pNode->GetName(), OFN_HIDEREADONLY, szFilters);
		if( fileDlg.DoModal() == IDOK)
		{
			// If the file doesn't exist, create.
			{
				CFile cf;
				
				//Returns nonzero if the open was successful; otherwise 0. The pError parameter is meaningful only if 0 is returned.
				BOOL bResult = cf.Open( fileDlg.GetPathName(), CFile::modeCreate|CFile::modeNoTruncate );
				I3ASSERT( bResult);
				
				//If you have not closed the file before destroying the object, the destructor closes it for you
			}
			
			std::string filename = fileDlg.GetPathName();
			filename.find("script");
			size_t _npos1 = filename.find("\\UI");
			size_t _npos2 = filename.find("\\", _npos1 + 1);
			filename.assign(filename, _npos2+1, filename.length());

			pScene->setScriptFile(filename.c_str());
			ScriptManager::SetScrScript(filename.c_str());

			//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
			//pFrame->UpdatePropertyPane();				
			
			m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
		}
		else
		{			
			CFile cf;

			char conv[MAX_PATH];
			i3::snprintf( conv, sizeof(conv), "%s/%s", i3ResourceFile::GetWorkingFolder(), pScene->getScriptFile());
			BOOL bResult = cf.Open( conv, CFile::modeRead);
			
			if( bResult == FALSE)
			{
				RemoveScript( hItem);
			}
		}
	}
}

void CUIPaneScreen::RemoveScript( HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);

	i3GameNode* pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( i3::same_of<i3UIScene*>(pNode)); //->IsExactTypeOf( i3UIScene::static_meta()));

	i3UIScene* pScene = (i3UIScene*)pNode;
	pScene->setScriptFile(NULL);
	ScriptManager::SetScrScript(NULL);

	//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	//pFrame->UpdatePropertyPane();

	m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
}

void CUIPaneScreen::EditScript( HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);
	
	i3GameNode* pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pNode != NULL);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if( i3::same_of<i3UIScene*>(pNode)) //->IsExactTypeOf( i3UIScene::static_meta()))
	{
		i3UIScene* pScene = (i3UIScene*)pNode;
		const char* pszScript = pScene->getScriptFile();
		
		CScriptPane* pPane = pMainFrm->OnEditScript( pszScript, true, true);
		CScriptWnd* pWnd = pPane->getScriptWnd();
		
		
		INT32 nLen = pWnd->getTextLen();
		char* pBuf = new char[nLen + 1];
		
		pWnd->getTextBuf( pBuf, nLen+1);
		
		//Block 삽입(BLOCKFMT)
		INT32 nPos = ScriptManager::HasBlock( pBuf, pszScript);
		
		if( nPos == -1)
		{
			char szWholeBlock[MAX_PATH];
			i3::snprintf( szWholeBlock, MAX_PATH, BLOCKFMT, pszScript, pszScript);
			pWnd->AppendText( szWholeBlock);
		}

		delete[] pBuf;
	}
	else if( i3::kind_of<i3UIControl*>(pNode)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl* pControl = (i3UIControl*)pNode;
		const char* pszScript = pControl->GetName();

		CScriptPane* pPane = pMainFrm->OnEditScript( pszScript, false, true);

		//Block 삽입(Use)
		ScriptManager::InsertUseBlock( pPane->getScriptWnd());
	}
}

// 해당 filter or control제거
void CUIPaneScreen::Delete( HTREEITEM hItem)
{
	{
		i3Framework::SetCurrentFocus( NULL);
	}

	i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);
	
	I3ASSERT( pObj != NULL);
	I3ASSERT( IsValidItem( hItem));

	if( i3::kind_of<i3UIControl*>(pObj)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl * pControl = (i3UIControl*)pObj;

		UIUndo::ADD_Delete( pControl);

		i3UI::DeleteControl( pControl);
	}
}

void CUIPaneScreen::Rename( HTREEITEM hItem)
{
	::SendMessage( m_TreeCtrl.m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
}

void CUIPaneScreen::Cut( HTREEITEM hItem)
{
}

void CUIPaneScreen::Copy( HTREEITEM hItem)
{
}

void CUIPaneScreen::Paste( HTREEITEM hItem)
{
}

void CUIPaneScreen::OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPoint Pt;

	*pResult = 0;

	//Get Drag item
	m_hSrcItem = pNMTreeView->itemNew.hItem;
	if( m_hSrcItem == NULL)
		return;

	i3ElementBase * pSrcObj = (i3ElementBase *) m_TreeCtrl.GetItemData( m_hSrcItem);
	if( pSrcObj == NULL)
		return;
	if( !i3::kind_of<i3UIControl*>(pSrcObj)) //->IsTypeOf( i3UIControl::static_meta()) == FALSE)
		return;

	m_TreeCtrl.Select( m_hSrcItem, TVGN_CARET);
	m_bDragging		= true;
	m_bDropEnable	= true;
	m_hTargetItem	= NULL;

	SetCapture();

	//
	CPoint pt = pNMTreeView->ptDrag;
	int cxCursor, cyCursor;

	cxCursor = 10;
	cyCursor = 10;

	m_pDragImage = m_TreeCtrl.CreateDragImage( pNMTreeView->itemNew.hItem);
	if( m_pDragImage != NULL)
	{
		m_pDragImage->DragShowNolock( FALSE);
		m_pDragImage->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
		m_pDragImage->DragEnter( NULL, pt);
	}
}

void CUIPaneScreen::DragMove( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone)
{
	i3GameNode * pSrcObj, * pTargetObj;

	if( hSrcItem == hTargetItem)
		return;

	pSrcObj = (i3GameNode *) m_TreeCtrl.GetItemData( hSrcItem);
	pTargetObj = (i3GameNode *) m_TreeCtrl.GetItemData( hTargetItem);

	// move function은 동일한 child level에서만 가능하다.
	if( pSrcObj->getParent() != pTargetObj->getParent())
	{
		DragDrop( hSrcItem, hTargetItem, bClone);
		return;
	}

	I3ASSERT( i3::kind_of<i3UIControl*>(pSrcObj)); //->IsTypeOf( i3UIControl::static_meta()));
	i3UIControl * pSrcControl = (i3UIControl*)pSrcObj;

	// ListView의 Item과 Header를 떼고 붙이는 건 여기서 먼저 검사
	if( i3::same_of<i3UIListView_Item*>(pSrcControl)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
	{
		return;
	}
	else if( i3::same_of<i3UIListView_Header*>(pSrcControl)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))
	{
		return;	
	}

	i3GameNode * pParent = pSrcControl->getParent();
	//떼고
	{
		pSrcControl->AddRef();
	
		if( pParent != NULL)
		{
			pParent->RemoveChild( pSrcControl);

			m_TreeCtrl.DeleteItem( hSrcItem);
		}
	}

	// 뿥이고
	{
		//INT32 idx = 0;
		HTREEITEM hParentItem = FindItem((i3UIControl*) pParent);

		i3GameNode * pFirstChild = pParent->getFirstChild();
		if( pFirstChild == pTargetObj)
		{
			pParent->AddChildToHead( pSrcControl);

			Rec_InsertTree( hParentItem, pSrcControl, TVI_FIRST);
		}
		else
		{
			while( pFirstChild != NULL)
			{
				if( pFirstChild->getNext() == pTargetObj)
				{
					pTargetObj->AddRef();
					pFirstChild->SetNext( pSrcControl);
					pSrcControl->SetNext( pTargetObj);
					pSrcControl->setParent( pParent);
					pTargetObj->Release();

					HTREEITEM hInsertAfter = FindItem( (i3UIControl*)pFirstChild);
					if( hInsertAfter)
					{
						Rec_InsertTree( hParentItem, pSrcControl, hInsertAfter);
					}
					else
					{
						Rec_SetTree( hParentItem, pSrcControl);
					}

					break;
				}

				pFirstChild = pFirstChild->getNext();
			}
		}
		
		pSrcControl->Release();

		// Select Moved ITem
		{
			HTREEITEM hMovedItem = FindItem( (i3UIControl*)pSrcControl);

			I3ASSERT( IsValidItem( hMovedItem));

			m_TreeCtrl.SelectItem( hMovedItem);
		}

		// Adjust Sprite Order
		if( i3UI::getToolDevMode())
		{
			INT32 idxEnd = -1;
			if( i3::kind_of<i3UIControl*>(pParent)) //->IsTypeOf( i3UIControl::static_meta()))
			{	
				idxEnd = ((i3UIControl*)pParent)->getEndSpriteHandle();
			}

			i3UIControl * pChild = (i3UIControl*)pParent->getFirstChild();
			while( pChild != NULL)
			{
				I3ASSERT( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::static_meta()));

				idxEnd = pChild->AdjustSpriteOrder( idxEnd);

				pChild = (i3UIControl*)pChild->getNext();
			}
		}
	}
}

// 잡다 트리 컨트롤
void CUIPaneScreen::DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone)
{
	i3GameNode * pSrcObj, * pTargetObj;

	if( hSrcItem == hTargetItem)
		return;

	pSrcObj = (i3GameNode *) m_TreeCtrl.GetItemData( hSrcItem);
	pTargetObj = (i3GameNode *) m_TreeCtrl.GetItemData( hTargetItem);

	// Parent Child 관계를 확인
	i3GameNode * pParent = pTargetObj;
	while( pParent)
	{
		if( pSrcObj == pParent)
		{
			::AfxMessageBox( "자식 Node에는 붙일 수 없습니다.");
			return;
		}
		pParent = pParent->getParent();
	}

	I3ASSERT( i3::kind_of<i3UIControl*>(pSrcObj)); //->IsTypeOf( i3UIControl::static_meta()));
	i3UIControl * pSrcControl = (i3UIControl*)pSrcObj;

	// ListView의 Item과 Header를 떼고 붙이는 건 여기서 먼저 검사
	if( i3::same_of<i3UIListView_Item*>(pSrcControl)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
	{
		i3UIListView_Item* pLVI = (i3UIListView_Item*)pSrcControl;
		
		if( i3::same_of<i3UIListView_Box*>(pTargetObj)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
		{
			i3UIListView_Box* pLV = (i3UIListView_Box*)pTargetObj;
			
			if( pLV->getBoundItem() == NULL)
			{
				i3GameNode* pParent2 = pLVI->getParent();
				if( i3::same_of<i3UIListView_Box*>(pParent2)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
				{
					i3UIListView_Box* pPrevBox = (i3UIListView_Box*)pParent2;
					pPrevBox->UnBindItem();
				}

				pLV->BindItem( pLVI);	//Bind
			}
			else
				return;	//이미 Bind된 Item이 있다면 들어갈 수 없음
		}
		else if( i3::same_of<i3UIScene*>(pTargetObj)) //->IsExactTypeOf( i3UIScene::static_meta()))
		{
			i3GameNode* pParent2 = pLVI->getParent();
			if( i3::same_of<i3UIListView_Box*>(pParent2)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
			{
				i3UIListView_Box* pLV = (i3UIListView_Box*)pParent2;
				pLV->UnBindItem();	//UnBind
			}
		}
		else
		{
			return;	// ListView_Item은 ListView_Box나 Scene 밑으로만 들어갈 수 있음
		}
	}
	else if( i3::same_of<i3UIListView_Header*>(pSrcControl)) //->IsExactTypeOf( i3UIListView_Header::static_meta()))
	{
		i3UIListView_Header* pLVH = (i3UIListView_Header*)pSrcControl;
		
		if( i3::same_of<i3UIListView_Box*>(pTargetObj)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
		{
			i3UIListView_Box* pLV = (i3UIListView_Box*)pTargetObj;
			if( pLV->getHeader() == NULL)
				pLV->BindHeader( pLVH);	//Bind
			else
				return;	//이미 Bind된 Item이 있다면 들어갈 수 없음
		}
		else if( i3::same_of<i3UIScene*>(pTargetObj)) //->IsExactTypeOf( i3UIScene::static_meta()))
		{
			i3GameNode* pParent2 = pLVH->getParent();
			if( i3::same_of<i3UIListView_Box*>(pParent2)) //->IsExactTypeOf( i3UIListView_Box::static_meta()))
			{
				i3UIListView_Box* pLV = (i3UIListView_Box*)pParent2;
				pLV->UnBindHeader();	//UnBind
			}
		}
		else
		{
			return;	
		}
	}


	// 떼고
	if( bClone)
	{
		i3UIControl * pCtrl = (i3UIControl*)i3GameNode::CreateClone( pSrcControl, I3_COPY_INSTANCE);

		pCtrl->AddRef();

		pSrcControl = pCtrl;
	}
	else
	{
		i3GameNode * pParent2 = pSrcControl->getParent();

		pSrcControl->AddRef();
	
		if( pParent2 != NULL)
			pParent2->RemoveChild( pSrcControl);
	}

	// 뿥이고
	{
		if( i3::kind_of<i3UIControl*>(pTargetObj)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl * pTargetControl = (i3UIControl*)pTargetObj;

			pTargetControl->AddControl( pSrcControl);
		}
		else if( i3::kind_of<i3UIScene*>(pTargetObj)) //->IsTypeOf( i3UIScene::static_meta()))
		{
			i3UIScene * pTargetControl = (i3UIScene*)pTargetObj;

			pTargetControl->AddControl(pSrcControl);
		}
		
		pSrcControl->Release();

		Rec_SetTree( hTargetItem, pSrcControl);
	}

	if( bClone == false)
	{
		m_TreeCtrl.DeleteItem( hSrcItem);
	}

	// Parent가 바뀌면 강제로 한번 위치를 업데이트 해줘야 튀는 걸 막을 수 있다. 
	// (상대좌표이므로)
	pSrcControl->setPos( pSrcControl->getPos());
}

void CUIPaneScreen::Expand( HTREEITEM hItem)
{

}

void CUIPaneScreen::Collapse( HTREEITEM hItem)
{

}

// TreeCtrl Item  전체를 Select/Unselect 합니다.
static BOOL	_SelectTreeItem( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	bool bSelect = pData != NULL;

	UINT mask = TVIS_DROPHILITED | TVIS_SELECTED | TVIS_BOLD;

	bool bSelected = (pCtrl->GetItemState( hItem, mask) & mask) != 0;

	if( bSelect)
	{
		if( bSelected == false)
			pCtrl->SetItemState( hItem, 0xFFFFFFFF, mask);
	}
	else
	{
		if( bSelected)
			pCtrl->SetItemState( hItem, 0, mask);
	}

	return TRUE;
}

void CUIPaneScreen::SelectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectTreeItem, (void *) TRUE);
}

void CUIPaneScreen::UnselectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectTreeItem, NULL);
}

void CUIPaneScreen::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW *) pNMHDR;

	//OnChangeSelectItem( pNMTV->itemOld.hItem, pNMTV->itemNew.hItem);

	i3NamedElement * pOldObj = (i3NamedElement*) pNMTV->itemOld.lParam;
	i3NamedElement * pNewObj = (i3NamedElement*) pNMTV->itemNew.lParam;
	
	if( m_bSkipMessage)
		return;

	//Tree Multi-selection 하고 싶으면 참고
	//http://www.codeguru.com/cpp/controls/treeview/misc-advanced/article.php/c723

	if(pOldObj != pNewObj)
	{
		// 이전에 선택되어 있던 개체가 있는경우, 새로 선택된 개체와 다르다면 unselect broadcast
		if( pOldObj != NULL)
		{
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000 && m_bDragging == false)
			{
				m_TreeCtrl.SetItemState(pNMTV->itemOld.hItem, TVIS_DROPHILITED,  TVIS_DROPHILITED);
			}
			else
			{
				i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT, pOldObj);
				i3TDK::Update(m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIControl::static_meta());
				//i3TDK::Update(m_hWnd, I3_TDK_UPDATE_UNSELECT, pOldObj);

				i3TDK::TreeCtrlTraverse(&m_TreeCtrl, _setItemStatetoEdit, NULL, pNMTV->itemOld.hItem);
			}
		}

		// 새로 선택된 개체가 있다면, select broadcast
		if( pNewObj != NULL)
		{
			if( i3::kind_of<i3UIControl*>(pNewObj)) //->IsTypeOf( i3UIControl::static_meta()))
			{
				i3UIControl * pCtrl = (i3UIControl*)pNewObj;
				
				if( pCtrl->isEnable() == FALSE)
					pCtrl->EnableCtrl( true, true);

				pCtrl->ShowTop();
			}

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNewObj);
		}
	}

	//SetFocus();
	theApp.SwitchView( EVIEW_SCREEN );

	*pResult = 0;
}

void CUIPaneScreen::OnRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = NULL;
//	List MetaList;		// 쓰이지 않아 주석..

	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		Menu.LoadMenu( IDR_MENU_SCREEN);
	}

	pPopup = Menu.GetSubMenu( 0);
	
	char szBlockname[MAX_PATH] = {0};

	if( hSel == NULL )
	{
		pPopup->EnableMenuItem( ID_MOVEUP, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_MOVEDOWN, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SCREEN_DELCTRL, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SCREEN_RENCTRL, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SCREEN_SETSCRIPT, MF_DISABLED);	//MF_GRAYED 랑 차이가 뭐지
		pPopup->EnableMenuItem( ID_SCREEN_REMOVESCRIPT, MF_DISABLED);
		pPopup->EnableMenuItem( ID_SCREEN_EDITSCRIPT, MF_GRAYED);
		pPopup->EnableMenuItem( ID__SCREEN_IEH, MF_GRAYED);
		pPopup->DeleteMenu(ID_LISTBOXONOFF, MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_LISTBOXDUMMYLINE, MF_BYCOMMAND);
	}
	else
	{
		// ****** Preview모드이면 Script 관련 Menu Disable 시키기.. 
		CWnd* pMainFrm = AfxGetMainWnd();
		CView* pView = ((CMainFrame*)pMainFrm)->GetActiveView();
		
		if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView)))
		{
			Ci3UIEditorView* pActiveView = (Ci3UIEditorView*)pView;
			if( pActiveView->isPrevMode())
			{
				pPopup->EnableMenuItem( ID_SCREEN_SETSCRIPT, MF_DISABLED);		//MF_GRAYED 랑 차이가 뭐지
				pPopup->EnableMenuItem( ID_SCREEN_REMOVESCRIPT, MF_DISABLED);
				pPopup->EnableMenuItem( ID_SCREEN_EDITSCRIPT, MF_GRAYED);
				pPopup->EnableMenuItem( ID__SCREEN_IEH, MF_GRAYED);
			}
		}
		else if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template* pActiveView = (Ci3UIEditorView_Template*)pView;
			if( pActiveView->isPrevMode())
			{
				pPopup->EnableMenuItem( ID_SCREEN_SETSCRIPT, MF_DISABLED);	
				pPopup->EnableMenuItem( ID_SCREEN_REMOVESCRIPT, MF_DISABLED);
				pPopup->EnableMenuItem( ID_SCREEN_EDITSCRIPT, MF_GRAYED);
				pPopup->EnableMenuItem( ID__SCREEN_IEH, MF_GRAYED);
			}
		}

		// ****** Script가 설정되어 있지 않으면 Remove, Edit Disable
		i3GameNode* pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hSel);
		I3ASSERT( pNode != NULL);

		if( i3::same_of<i3UIScene*>(pNode)) //->IsExactTypeOf( i3UIScene::static_meta()))
		{
			i3UIScene* pScene = (i3UIScene*)pNode;
			const char* pszScriptFile = pScene->getScriptFile();
			
			if( pszScriptFile == NULL )
			{
				pPopup->EnableMenuItem( ID_SCREEN_REMOVESCRIPT, MF_DISABLED);
				pPopup->EnableMenuItem( ID_SCREEN_EDITSCRIPT, MF_GRAYED);
				pPopup->EnableMenuItem( ID__SCREEN_IEH, MF_GRAYED);
			}
		}
		else if( i3::kind_of<i3UIControl*>(pNode)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			CDocument* pDoc = ((CMainFrame*)pMainFrm)->GetActiveDocument();
			//Ci3UIEditorDoc* pUIDoc = dynamic_cast<Ci3UIEditorDoc*>(pDoc);
			Ci3UIEditorDoc* pUIDoc = (Ci3UIEditorDoc*)pDoc;
			i3UIScene* pScene = pUIDoc->getCurrentScene();
			const char* pszScriptFile = pScene->getScriptFile();
			
			if( pszScriptFile == NULL )
				pPopup->EnableMenuItem( ID_SCREEN_EDITSCRIPT, MF_DISABLED);

			pPopup->DeleteMenu( ID_SCREEN_SETSCRIPT, MF_BYCOMMAND);
			pPopup->DeleteMenu( ID_SCREEN_REMOVESCRIPT, MF_BYCOMMAND);

			i3UIControl* pCtrl = (i3UIControl*)pNode;
			i3::string_ncopy_nullpad( szBlockname, pCtrl->GetName(), MAX_PATH);

			if (i3::kind_of<i3UIComboBase*>(pNode))
			{
				pPopup->EnableMenuItem(ID_LISTBOXONOFF, MF_BYCOMMAND);
			}
			else
			{
				pPopup->DeleteMenu(ID_LISTBOXONOFF, MF_BYCOMMAND);
			}

			if (i3::kind_of<i3UIListView_Box*>(pNode))
			{
				pPopup->EnableMenuItem(ID_LISTBOXDUMMYLINE, MF_BYCOMMAND);
			}
			else
			{
				pPopup->DeleteMenu(ID_LISTBOXDUMMYLINE, MF_BYCOMMAND);
			}
		}
	}

	OnPopupMenu( pPopup, hSel);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
	case ID_MOVEUP :					MoveUp( hSel);			break;
	case ID_MOVEDOWN :					MoveDown( hSel);		break;
//	case ID_SCREEN_NEWCTRL:				Add( hSel);				break;
	case ID_SCREEN_DELCTRL:				Delete( hSel);			break;
	case ID_SCREEN_RENCTRL:				Rename( hSel);			break;
	case ID_SCREEN_SETSCRIPT:			SetScript( hSel);		break;
	case ID_SCREEN_REMOVESCRIPT:		RemoveScript( hSel);	break;
	case ID_SCREEN_EDITSCRIPT:			EditScript( hSel);		break;
	case ID__SCREEN_IEH:				ScriptManager::InsertEventHandler( szBlockname, true);		break;
	case ID__VIEWTEMPLATE :				EditTemplate( hSel);	break;
	case ID__IMPORTUISCENE :			ImportScene( hSel);		break;
	case ID__EXPORTUISCENE :			ExportScene( hSel);		break;
	case ID_LISTBOXONOFF:				ShowListBox(hSel);		break;
	case ID_LISTBOXDUMMYLINE:			ShowDummyLine(hSel);	break;
	}

	OnSelectMenu( cmd);
	
	*pResult = 1;
}

// 메뉴 
void CUIPaneScreen::OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem)
{

}

void CUIPaneScreen::OnSelectMenu( UINT id)
{

}

void CUIPaneScreen::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	theApp.SwitchView( EVIEW_SCREEN);
}

void CUIPaneScreen::OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDockablePane::OnSysKeyDown( nChar, nRepCnt, nFlags);

	if( nChar == VK_ESCAPE)
	{
		if( m_bDragging)
		{
			m_bDragging = FALSE;

			ReleaseCapture();

			if( m_pDragImage != NULL)
			{
				m_pDragImage->DragLeave( &m_TreeCtrl);
				m_pDragImage->EndDrag();

				delete m_pDragImage;
				m_pDragImage = NULL;
			}
		}
	}
}

BOOL CUIPaneScreen::PreTranslateMessage(MSG* pMsg)
{
	//// TODO: Add your specialized code here and/or call the base class
	//if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_END) // MoveDown
	//{
	//	//POINT pt;
	//	HTREEITEM hSel = NULL;
	//	//GetCursorPos( &pt);

	//	//// Select
	//	//POINT cpt;
	//	//UINT flag;
	//	//cpt = pt;
	//	//m_TreeCtrl.ScreenToClient( &cpt);
	//	//hSel = m_TreeCtrl.HitTest( cpt, &flag);
	//	//if ((hSel != NULL) && (TVHT_ONITEM & flag))
	//	//{
	//	//	m_TreeCtrl.Select( hSel, TVGN_CARET);
	//	//}
	//	hSel = m_TreeCtrl.GetSelectedItem();
	//	MoveDown(hSel);
	//	return TRUE;
	//}
	//else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_HOME) // MoveDown
	//{
	//	//POINT pt;
	//	HTREEITEM hSel = NULL;
	//	//GetCursorPos( &pt);

	//	// Select
	//	/*POINT cpt;
	//	UINT flag;
	//	cpt = pt;*/
	//	//m_TreeCtrl.GetSelectedItem()
	//	//m_TreeCtrl.ScreenToClient( &cpt);
	//	//hSel = m_TreeCtrl.HitTest( cpt, &flag);
	//	/*if ((hSel != NULL) && (TVHT_ONITEM & flag))
	//	{
	//		m_TreeCtrl.Select( hSel, TVGN_CARET);
	//	}*/
	//	hSel = m_TreeCtrl.GetSelectedItem();
	//	MoveUp(hSel);
	//	return TRUE;
	//}

	switch( pMsg->message )
	{
	case	WM_KEYDOWN:
			{				
				CEdit* pEdit = m_TreeCtrl.GetEditControl();

				if( NULL == pEdit  )
				{
					HTREEITEM hSel = m_TreeCtrl.GetSelectedItem();

					switch( LOWORD( pMsg->wParam ) )
					{
					case	VK_HOME:
							MoveUp( hSel );
							return TRUE;

					case	VK_END:
							MoveDown( hSel );
							return TRUE;
					}
				}
			}
			break;
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}


void CUIPaneScreen::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDockablePane::OnKeyDown( nChar, nRepCnt, nFlags);

	if( nChar == VK_ESCAPE)
	{
		if( m_bDragging)
		{
			m_bDragging = FALSE;

			ReleaseCapture();

			if( m_pDragImage != NULL)
			{
				m_pDragImage->DragLeave( &m_TreeCtrl);
				m_pDragImage->EndDrag();

				delete m_pDragImage;
				m_pDragImage = NULL;
			}
		}
	}
}

void CUIPaneScreen::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_bDragging)
	{
		if((m_hTargetItem != NULL) && (m_hSrcItem != NULL) && (m_bDropEnable == true))
		{
			if( nFlags & MK_SHIFT)
				DragDrop( m_hSrcItem, m_hTargetItem, true);
			else if( nFlags & MK_CONTROL)
				DragMove( m_hSrcItem, m_hTargetItem, false);
			else
				DragDrop( m_hSrcItem, m_hTargetItem, false);

			m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
		}

		m_bDragging = FALSE; 

		ReleaseCapture();

		if( m_pDragImage != NULL)
		{
			m_pDragImage->DragLeave( &m_TreeCtrl);
			m_pDragImage->EndDrag();

			delete m_pDragImage;
			m_pDragImage = NULL;
		}
	}

	CDockablePane::OnLButtonUp(nFlags, point);
}

void CUIPaneScreen::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bDragging)
	{
		HTREEITEM hNew;

		hNew = m_TreeCtrl.HitTest( point, NULL);

		if( hNew != NULL)	// 새 아이템과 현재 target이 다른경우
		{
			m_bDropEnable = true;

			if( m_hTargetItem != hNew)
			{
				if( m_hTargetItem != NULL)				// 현재 아이템의 hilight상태를 없애고
				{
					m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
				}

				m_hTargetItem = hNew;					// 새 아이템을 hilight시킨다.
				m_TreeCtrl.SetItemState( m_hTargetItem, 0xFFFFFFFF, TVIS_DROPHILITED);
			}
		}
		else
		{
			m_bDropEnable = false;

			if( m_hTargetItem != NULL)
			{
				m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
				m_hTargetItem = NULL;
			}
		}

		if( m_pDragImage != NULL)
		{
			// Cursor
			ClientToScreen( &point);

			m_pDragImage->DragMove( point);
		}

		// Scroll when hovering
		RECT rc;
		::GetWindowRect( m_TreeCtrl.m_hWnd, &rc);
		
		if( (short)point.y > rc.top - HOVERING_VALID_RANGE && (short)point.y < rc.top + HOVERING_VALID_RANGE)
		{
			::SetTimer( m_hWnd, TIMER_HOVERING, HOVERING_TIMEOUT, NULL);
			::SetTimer( m_hWnd, TIMER_SCROLLUP, SCROLLING_FREQUENCY, NULL);
			::KillTimer( m_hWnd, TIMER_SCROLLDOWN);

			if( m_bHovering)
				::SendMessage( m_TreeCtrl.m_hWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEUP, 0), NULL);
		}
		else if( (short)point.y > rc.bottom - HOVERING_VALID_RANGE && (short)point.y < rc.bottom + HOVERING_VALID_RANGE)
		{
			::SetTimer( m_hWnd, TIMER_HOVERING, HOVERING_TIMEOUT, NULL);
			::SetTimer( m_hWnd, TIMER_SCROLLDOWN, SCROLLING_FREQUENCY, NULL);
			::KillTimer( m_hWnd, TIMER_SCROLLUP);

			if( m_bHovering)
				::SendMessage( m_TreeCtrl.m_hWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEDOWN, 0), NULL);
		}
		else
		{
			m_bHovering = false;
			::KillTimer( m_hWnd, TIMER_HOVERING);
			::KillTimer( m_hWnd, TIMER_SCROLLUP);
			::KillTimer( m_hWnd, TIMER_SCROLLDOWN);
		}
	}
	else
	{
		CDockablePane::OnMouseMove(nFlags, point);
	}
}

void CUIPaneScreen::OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	i3NamedElement * pData = (i3NamedElement*)pDispInfo->item.lParam;
	if( pData == NULL)
		return;

	if( pDispInfo->item.pszText != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pData)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl * pControl = (i3UIControl*)pData;

			if( _FindItemByName( m_pRootItem, pDispInfo->item.pszText) == NULL)
			{
				// rename script block
				if( pControl->GetName() != NULL)	// or i3Sting::Compare ?
				{
					//pScriptManager->RenameBlock(pControl->GetName(), pDispInfo->item.pszText);
				}

				pControl->SetName( pDispInfo->item.pszText);
				m_TreeCtrl.SetItemText( pDispInfo->item.hItem, pDispInfo->item.pszText);

				CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->UpdatePropertyPane();
			}
			else
			{
				MessageBox( "컨트롤의 이름은 중복될 수 없습니다.", "Rename Failed", MB_OK);
			}
		}
		else if( i3::kind_of<i3UIScene*>(pData)) //->IsTypeOf( i3UIScene::static_meta()))
		{
			i3UIScene * pScene = (i3UIScene*)pData;

			pScene->SetName((char*)pDispInfo->item.pszText);

			m_TreeCtrl.SetItemText( pDispInfo->item.hItem, pDispInfo->item.pszText);

			CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->UpdatePropertyPane();
		}
	}
    
	*pResult = 0;
}

void CUIPaneScreen::_GetItemNames( HTREEITEM hRoot, i3::vector<i3::rc_string>& StrList)
{
	

	if( hRoot == NULL)
		return;
	else
	{
		char temp[64];
		i3::string_ncopy_nullpad( temp, m_TreeCtrl.GetItemText( hRoot), sizeof( temp));	//CString -> char*
		StrList.push_back( temp);

		HTREEITEM hCur = m_TreeCtrl.GetChildItem( hRoot);
		while( hCur != NULL)
		{
			_GetItemNames( hCur, StrList);					//DFS, recursive
			hCur = m_TreeCtrl.GetNextItem(hCur, TVGN_NEXT);
		}
	}
}

void CUIPaneScreen::GetItemNames( i3::vector<i3::rc_string>& StrList)
{
	_GetItemNames( m_pRootItem, StrList);
}

void CUIPaneScreen::OnNMClickSceneTree(NMHDR * pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CUIPaneScreen::OnTVNKeyDown( NMHDR * pNMHDR, LRESULT *pResult)
{	
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast <LPNMTVKEYDOWN> (pNMHDR);;
 
 	if(pTVKeyDown->wVKey == VK_ESCAPE)
 	{
 		if( m_bDragging)
 		{
 			m_bDragging = FALSE;
 
 			ReleaseCapture();
 
 			if( m_pDragImage != NULL)
 			{
 				m_pDragImage->DragLeave( &m_TreeCtrl);
 				m_pDragImage->EndDrag();
 
 				delete m_pDragImage;
 				m_pDragImage = NULL;
 			}
 		}
 	}
}

// tree view에서 설정된 enable state에 맞춰 control들의 enable state를 역으로 설정합니다.
void CUIPaneScreen::returnToEditState(void)
{
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3UIControl::static_meta());

	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _setItemStatetoEdit, NULL);
}

void CUIPaneScreen::_getAllChildControls( HTREEITEM hRoot,  i3::vector<i3UIControl*>& Out)
{
	i3ElementBase * pRoot	= (i3ElementBase*)m_TreeCtrl.GetItemData( hRoot);

	if( i3::kind_of<i3UIControl*>(pRoot)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		Out.push_back(static_cast<i3UIControl*>(pRoot));
	}

	HTREEITEM hChild = m_TreeCtrl.GetChildItem( hRoot);

	while( hChild != NULL)
	{
		_getAllChildControls( hChild, Out);

		hChild = m_TreeCtrl.GetNextItem( hChild, TVGN_NEXT);
	}
}

// pOut contains i3UIControl*
void CUIPaneScreen::getAllChildControls( i3ElementBase* pParent, i3::vector<i3UIControl*>& Out)
{
	if( pParent == NULL)
	{
		//Add all items
		HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();		//Scene
		HTREEITEM hChildItem = m_TreeCtrl.GetChildItem( hRootItem);

		while( hChildItem != NULL)
		{
			_getAllChildControls( hChildItem, Out);	//recursive

			hChildItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		//Add all items that are child of pParent
		if( i3::kind_of<i3UIControl*>(pParent)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			HTREEITEM hParentItem = FindItem( (i3UIControl*)pParent);
			
			_getAllChildControls( hParentItem, Out);		//recursive

			i3::vu::remove(Out, pParent);		//remove parent
		}
	}
}

void CUIPaneScreen::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	
	switch( nIDEvent)
	{
	case TIMER_HOVERING:	
		m_bHovering = true;		
		break;
	case TIMER_SCROLLUP:	
		if( m_bHovering)	
			::SendMessage( m_TreeCtrl.m_hWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEUP, 0), NULL);		
		break;
	case TIMER_SCROLLDOWN:	
		if( m_bHovering)
			::SendMessage( m_TreeCtrl.m_hWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEDOWN, 0), NULL);		
		break;
	default: 
		break;
	}

	//CDockablePane::OnTimer(nIDEvent);
}

void CUIPaneScreen::MoveUp( HTREEITEM hItem)
{
	if( NULL == hItem )
	{
		return;
	}

	// 바로 위의 Item부터 띄어낸다.
	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem( hItem);
	HTREEITEM hPrevItem = m_TreeCtrl.GetNextItem( hItem, TVGN_PREVIOUS);

	if( NULL == hPrevItem )
	{
		return;
	}

	{
		i3Framework::SetCurrentFocus( NULL);
	}

	i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);
	
	I3ASSERT( pObj != NULL);
	I3ASSERT( IsValidItem( hItem));

	// Parent에서 자신의 위치를 찾는다.
	i3GameNode * pParent = pObj->getParent();

	//INT32 currentIdx = pParent->GetChildIndex( pObj);

	i3::vector<i3GameNode*> replaceList;

	

	// 이전 것을 띄어내고
	i3GameNode * pPrevObj = (i3GameNode*) m_TreeCtrl.GetItemData( hPrevItem);
	pPrevObj->AddRef();
	pParent->RemoveChild( pPrevObj);
	m_TreeCtrl.DeleteItem( hPrevItem);

	replaceList.push_back( pPrevObj);

	

	// 다음 것들을 띄었다 붙인다.
	HTREEITEM hNextItem;
	HTREEITEM hDeleteItem = m_TreeCtrl.GetNextItem( hItem, TVGN_NEXT);
	while( hDeleteItem)
	{
		hNextItem = m_TreeCtrl.GetNextItem( hDeleteItem, TVGN_NEXT);

		// detach node
		i3GameNode * pNextObj = (i3GameNode*) m_TreeCtrl.GetItemData( hDeleteItem);
		pNextObj->AddRef();
		pParent->RemoveChild( pNextObj);
		m_TreeCtrl.DeleteItem( hDeleteItem);

		replaceList.push_back( pNextObj);

		hDeleteItem = hNextItem;
	}

	
	for( size_t i = 0; i < replaceList.size(); i++)
	{
		i3GameNode * pReplaceObj = replaceList[ i];

		pParent->AddChild( pReplaceObj);
		pReplaceObj->Release();
		HTREEITEM root = AddTreeItem( pReplaceObj, hParentItem);

		i3UIControl * pChild = (i3UIControl*) pReplaceObj->getFirstChild();
		
		while( pChild)
		{
			if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
				Rec_SetTree( root, pChild);

			pChild = (i3UIControl*)pChild->getNext();
		}
	}

	m_TreeCtrl.SelectItem( hItem);

}

void CUIPaneScreen::MoveDown( HTREEITEM hItem)
{
	if( NULL == hItem )
	{
		return;
	}

	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem( hItem);
	HTREEITEM hNextItem = m_TreeCtrl.GetNextItem( hItem, TVGN_NEXT);

	if( NULL == hNextItem )
	{
		return;
	}

	i3::vector<i3GameNode*> replaceList;

	i3Framework::SetCurrentFocus( NULL);

	i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);
	
	I3ASSERT( pObj != NULL);
	I3ASSERT( IsValidItem( hItem));

	// Parent
	i3GameNode * pParent = pObj->getParent();
	

	// 현재 것을 띄어낸다.
	pObj->AddRef();
	pParent->RemoveChild( pObj);
	m_TreeCtrl.DeleteItem( hItem);

	replaceList.push_back( pObj);
	

	// 다음 것들을 띄었다 붙인다.
	HTREEITEM hDeleteItem = m_TreeCtrl.GetNextItem( hNextItem, TVGN_NEXT);
	while( hDeleteItem)
	{
		hNextItem = m_TreeCtrl.GetNextItem( hDeleteItem, TVGN_NEXT);

		// detach node
		i3GameNode * pNextObj = (i3GameNode*) m_TreeCtrl.GetItemData( hDeleteItem);
		pNextObj->AddRef();
		pParent->RemoveChild( pNextObj);
		m_TreeCtrl.DeleteItem( hDeleteItem);

		replaceList.push_back( pNextObj);

		hDeleteItem = hNextItem;
	}

	
	for(size_t i = 0; i < replaceList.size(); i++)
	{
		i3GameNode * pReplaceObj = replaceList[ i];

		pParent->AddChild( pReplaceObj);
		pReplaceObj->Release();
		HTREEITEM root = AddTreeItem( pReplaceObj, hParentItem);

		if( i == 0)
		{
			m_TreeCtrl.SelectItem( root);
		}

		i3UIControl * pChild = (i3UIControl*) pReplaceObj->getFirstChild();
		
		while( pChild)
		{
			if( pChild->isStyle( I3UI_STYLE_SLAVE) == false)
				Rec_SetTree( root, pChild);

			pChild = (i3UIControl*)pChild->getNext();
		}
	}
}

void CUIPaneScreen::EditTemplate( HTREEITEM hItem)
{
	i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);
	if(  i3::kind_of<i3UIControl*>(pObj)) //->IsTypeOf( i3UIControl::static_meta()) )
	{
		i3UIControl * pControl = (i3UIControl*) pObj;

		CMainFrame * pFrame = (CMainFrame*) ::AfxGetMainWnd();

		pFrame->GotoViewTemplateEdit( pControl->getTemplate());
	}
}

void CUIPaneScreen::ImportScene( HTREEITEM hItem)
{
	CMainFrame * pFrame = (CMainFrame*) ::AfxGetMainWnd();

	pFrame->ImportUIScene();

}

void CUIPaneScreen::ExportScene( HTREEITEM hItem)
{
	i3GameNode * pObj = (i3GameNode *) m_TreeCtrl.GetItemData( hItem);

	if( i3::kind_of<i3UIControl*>(pObj)) //->IsTypeOf( i3UIControl::static_meta()) )
	{
		i3UIControl * pControl = (i3UIControl*) pObj;

		CMainFrame * pFrame = (CMainFrame*) ::AfxGetMainWnd();

		pFrame->ExportUIScene( pControl);
	}
}

void CUIPaneScreen::ShowListBox(HTREEITEM hItem)
{
	i3GameNode * pObj = (i3GameNode *)m_TreeCtrl.GetItemData(hItem);

	if (i3::kind_of<i3UIComboBase*>(pObj)) //->IsTypeOf( i3UIControl::static_meta()) )
	{
		i3UIComboBase * pControl = (i3UIComboBase*)pObj;
		i3UIListBox* pListBox = pControl->getListBox();
		pListBox->EnableCtrl(!pListBox->isEnable());
	}
}

void CUIPaneScreen::ShowDummyLine(HTREEITEM hItem)
{
	i3GameNode * pObj = (i3GameNode *)m_TreeCtrl.GetItemData(hItem);

	if (i3::kind_of<i3UIListView_Box*>(pObj)) //->IsTypeOf( i3UIControl::static_meta()) )
	{
		i3UIListView_Box * pControl = (i3UIListView_Box*)pObj;
		pControl->SetDummyLine(!pControl->IsDummyLine());
	}
}