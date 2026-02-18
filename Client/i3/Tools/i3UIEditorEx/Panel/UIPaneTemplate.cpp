// UITemplateWindow.cpp : implementation file
//

#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneTemplate.h"
#include "../Dlg_SelTemplateType.h"
#include "../MainFrm.h"
#include "../DragDropOptionDlg.h"

#include "../ScriptPane.h"
#include "../ScriptManager.h"
// CUITemplateWindow

static HTREEITEM s_hCopiedTreeItem;

IMPLEMENT_DYNAMIC(CUIPaneTemplate, CUIPaneBase)

CUIPaneTemplate::CUIPaneTemplate()
{
	m_pLibrary	= NULL;
	m_pRoot		= NULL;

	m_bDragging	= false;
	m_bDropEnable = false;

	m_pDragImage	= NULL;
	m_hTargetItem	= NULL;
	m_hDragItem		= NULL;

	s_hCopiedTreeItem = NULL;
}

CUIPaneTemplate::~CUIPaneTemplate()
{
	I3_SAFE_RELEASE( m_pLibrary);
	I3_SAFE_RELEASE( m_pRoot);
}


BEGIN_MESSAGE_MAP(CUIPaneTemplate, CUIPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, ID_TEMPLATE_TREECTRL, OnRClickTree)
	ON_NOTIFY( TVN_ENDLABELEDIT, ID_TEMPLATE_TREECTRL, OnTVNEndLabelEdit)
	ON_NOTIFY( TVN_SELCHANGED, ID_TEMPLATE_TREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_KEYDOWN,		ID_TEMPLATE_TREECTRL, OnTVNKeyDown)
	ON_NOTIFY( TVN_BEGINDRAG, ID_TEMPLATE_TREECTRL, OnTVNBeginDragTree)
	ON_NOTIFY( NM_DBLCLK, ID_TEMPLATE_TREECTRL, OnNMDblClickTree)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CUITemplateWindow message handlers
void CUIPaneTemplate::SetUILibrary( i3UILibrary * pLibrary)
{
	I3_REF_CHANGE( m_pLibrary, pLibrary);

	ScriptManager::SetLibrary( m_pLibrary);

	if( m_pLibrary != NULL)
	{
		setUILibRoot( m_pLibrary->getLibRoot());
	}
	else
	{
		setUILibRoot( NULL);
	}
}

bool CUIPaneTemplate::setUILibRoot( i3UILTreeFilter * pFilter)
{
	I3_REF_CHANGE( m_pRoot, pFilter);

	m_TreeCtrl.DeleteAllItems();
	m_pRootItem = NULL;

	if( m_pRoot != NULL)
	{
		m_pRootItem	= AddTreeItem( m_pRoot, TVI_ROOT);

		INT32 i;
		for( i = 0;i < m_pRoot->getChildCount(); ++i)
		{
			i3TreeNode * pNode = m_pRoot->getChild( i);

			Rec_SetTree( m_pRootItem, pNode);
		}
	}

	return true;
}

void CUIPaneTemplate::_SetTreeItem( TVITEM * pInfo, i3NamedElement * pNode, HTREEITEM hItem)
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

HTREEITEM CUIPaneTemplate::AddTreeItem( i3NamedElement * pNode, HTREEITEM hParent)
{
	TVINSERTSTRUCT info;
	HTREEITEM hItem;

	info.hInsertAfter = TVI_LAST;
	info.hParent = hParent;
	
	_SetTreeItem( &info.item, pNode);

	hItem = m_TreeCtrl.InsertItem( &info);

	if( hParent == TVI_ROOT)	
	{
		i3UIManager* pUIManager = i3UI::getManager();
		i3UILibrary* pLib = pUIManager->getUILibrary();
		if( pLib != NULL)
		{
			const char* script = pLib->getScriptFile();
			if( script != NULL)
			{
				ScriptManager::SetLibScript( script);
				m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
			}
		}
	}

	if((hItem != NULL) && ((hParent != NULL) && (hParent != TVI_ROOT)))
	{
		TVITEM item;
 		item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hParent;

		m_TreeCtrl.GetItem( &item);

		if( item.lParam != NULL)
		{
			item.cChildren = getChildItemCount( (i3TreeNode *) item.lParam);
		}
		else
		{
			item.cChildren = 1;
		}

		m_TreeCtrl.SetItem( &item);

		//if( i3::same_of<i3UITemplate_User*>(pNode)) //->IsExactTypeOf( i3UITemplate_User::static_meta()))
		//{
		//	m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK( 1), TVIS_STATEIMAGEMASK);	//Test
		//}
	}

	//m_TreeCtrl.EnsureVisible( hItem);

	return hItem;
}

INT32 CUIPaneTemplate::getChildItemCount( i3NamedElement * pNode)
{
	if( i3::same_of<i3UILTreeElement*>(pNode)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
	{
		i3UILTreeElement * pElm = (i3UILTreeElement*)pNode;

		return pElm->getElementCount();
	}
	else if( i3::kind_of<i3TreeNode*>(pNode)) //->IsTypeOf( i3TreeNode::static_meta()))
	{
		return ((i3TreeNode*)pNode)->getChildCount();
	}
	else if( i3::kind_of<i3UITemplate*>(pNode)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		return ((i3UITemplate*)pNode)->getSubTemplateCount();
	}

	//I3TRACE("========================= 니가 진짜로 원하는게 뭐야~ =========\n");
	return 0;
}

void CUIPaneTemplate::Rec_SetTree( HTREEITEM hParentItem, i3NamedElement * pNode)
{
	INT32 i;
	HTREEITEM hItem;
	i3TreeNode * pChild;

	hItem = AddTreeItem( pNode, hParentItem);

	if( i3::same_of<i3UILTreeElement*>(pNode)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
	{
		i3UITemplate * pTemplate = NULL;
		i3UILTreeElement * pElm = (i3UILTreeElement*)pNode;
		for( i = 0; i < pElm->getElementCount(); ++i)
		{
			pTemplate = (i3UITemplate*)pElm->getElement(i);

			Rec_SetTree( hItem, pTemplate);
		}
	}
	else if( i3::kind_of<i3TreeNode*>(pNode)) //->IsTypeOf( i3TreeNode::static_meta()))
	{
		i3TreeNode * pTNode = (i3TreeNode*)pNode;
		for( i = 0; i < pTNode->getChildCount(); i++)
		{
			pChild = pTNode->getChild( i);

			Rec_SetTree( hItem, pChild);
		}
	}
	else if( i3::kind_of<i3UITemplate*>(pNode)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		i3UITemplate * pTemplate = (i3UITemplate*)pNode;
		for( i = 0;i < pTemplate->getSubTemplateCount(); ++i)
		{
			i3UITemplate * pChild2 = pTemplate->getSubTemplate( i);

			Rec_SetTree( hItem, pChild2);
		}
	}
}

void CUIPaneTemplate::OnRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = NULL;
//	List MetaList;			// 쓰이지 않아 주석..
	i3ElementBase * pSelItem = NULL;

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

	if( hSel == NULL)
		return;

	//
	pSelItem = (i3ElementBase*)m_TreeCtrl.GetItemData( hSel);

	//
	{
		Menu.LoadMenu( IDR_MENU_TEMPLATE);
	}

	pPopup = Menu.GetSubMenu( 0);
	pPopup->AppendMenu( MF_STRING, ID_SCRIPT_TESTSCRIPT, "Test Script");

	char szBlockname[MAX_PATH] = {0};

	if( pSelItem == NULL)
	{
		pPopup->EnableMenuItem( ID_TEMPLATE_EDIT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_NEW_FILTER, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_NEW_TEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_COPY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_CUT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_PASTE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_TEMPLATE_RENAME, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		if( m_pRootItem == hSel)
		{
			pPopup->EnableMenuItem( ID_TEMPLATE_EDIT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SELECTED_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_TEMPLATE_COPY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_TEMPLATE_CUT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_TEMPLATE_PASTE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_TEMPLATE_RENAME, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);


			i3UIManager* pUI = i3UI::getManager();
			i3UILibrary* pLib = pUI->getUILibrary();

			if( pLib != NULL)
			{
				//현재 선택된 노드가 i3UILibrary인 경우
				CMenu menuScript;
				menuScript.CreateMenu();

				menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_EDITSCRIPT, "Edit Script");
				menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_SETSCRIPT, "Set Script");
				menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_REMOVESCRIPT, "Remove Script");
				menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_IEH, "Insert Event Handler");

				int nPos = 0;
				while( nPos < pPopup->GetMenuItemCount())
				{
					if( pPopup->GetMenuItemID(nPos) == ID_TEMPLATE_EDIT)
						break;
					nPos++;
				}

				pPopup->InsertMenu( nPos+1, MF_POPUP | MF_BYPOSITION, (UINT_PTR)menuScript.m_hMenu, _T("Script"));
				menuScript.Detach();
	
				const char* script = pLib->getScriptFile();
				if( script == NULL)
				{
					pPopup->EnableMenuItem( ID_TEMPLATE_EDITSCRIPT, MF_DISABLED | MF_BYCOMMAND);	
					pPopup->EnableMenuItem( ID_TEMPLATE_REMOVESCRIPT, MF_DISABLED | MF_BYCOMMAND);
					pPopup->EnableMenuItem( ID_TEMPLATE_IEH, MF_DISABLED | MF_BYCOMMAND);
				}
			}
		}
		else
		{
			// filter 아래에만 filter가 추가 될수 있다.
			if( !i3::same_of<i3UILTreeFilter*>(pSelItem)) //->IsExactTypeOf( i3UILTreeFilter::static_meta()) == FALSE)
				pPopup->EnableMenuItem( ID_TEMPLATE_NEW_FILTER, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

			// Tree Node를 상속 받지 않은 아이템에는 template의 추가를 명령할수 없다.
			if( !i3::kind_of<i3TreeNode*>(pSelItem)) //->IsTypeOf( i3TreeNode::static_meta()) == FALSE)
			{
				if( i3::kind_of<i3UITemplate_User*>(pSelItem) //->IsTypeOf( i3UITemplate_User::static_meta()) 
					|| i3::kind_of<i3UITemplate_MessageBox*>(pSelItem)) //->IsTypeOf( i3UITemplate_MessageBox::static_meta()))
				{
					//현재 선택된 노드가 i3UITemplate_User인 경우
					CMenu menuScript;
					menuScript.CreateMenu();
			
					int nPos = 0;
					while( nPos < pPopup->GetMenuItemCount())
					{
						if( pPopup->GetMenuItemID(nPos) == ID_TEMPLATE_EDIT)
							break;
						nPos++;
					}
	
					menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_EDITSCRIPT, "Edit Script");
					menuScript.AppendMenu( MF_STRING, ID_TEMPLATE_IEH, "Insert Event Handler");

					pPopup->InsertMenu( nPos+1, MF_POPUP | MF_BYPOSITION, (UINT)menuScript.m_hMenu, _T("Script"));
					menuScript.Detach();

					i3UIManager* pUI = i3UI::getManager();
					I3ASSERT( pUI != NULL);

					i3UILibrary* pLib = pUI->getUILibrary();
					I3ASSERT( pLib != NULL);

					if( pLib->getScriptFile() == NULL)
					{
						pPopup->EnableMenuItem( ID_TEMPLATE_EDITSCRIPT, MF_DISABLED | MF_BYCOMMAND);
						pPopup->EnableMenuItem( ID_TEMPLATE_IEH, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
					}

					i3UITemplate_User* pTemplate = (i3UITemplate_User*)pSelItem;
					i3::string_ncopy_nullpad( szBlockname, pTemplate->GetName(), MAX_PATH);
				}
				else
				{
					pPopup->EnableMenuItem( ID_TEMPLATE_NEW_TEMPLATE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
				}
			}

			// UITemplate를 상속 받지 않았다면 편집할수 없다.
			if( !i3::kind_of<i3UITemplate*>(pSelItem)) //->IsTypeOf( i3UITemplate::static_meta()) == FALSE)
				pPopup->EnableMenuItem( ID_TEMPLATE_EDIT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
	}

	OnPopupMenu( pPopup, hSel);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_TEMPLATE_EDIT:						Edit((i3UITemplate*)pSelItem);	break;
		case ID_TEMPLATE_NEW_FILTER:				AddFilter( hSel);	break;
		case ID_TEMPLATE_NEW_TEMPLATE:				Add( hSel);			break;

		case ID_TEMPLATE_COPY:						Copy( hSel);		break;
		case ID_TEMPLATE_CUT:						Cut( hSel);			break;
		case ID_TEMPLATE_PASTE:						Paste( hSel);		break;
		case ID_TEMPLATE_RENAME:					Rename( hSel);		break;
		case ID_SELECTED_DELETE:					Delete( hSel);		break;

		case ID_TEMPLATE_EXPAND :					Expand( hSel);		break;
		case ID_TEMPLATE_COLLAPSE :					Collapse( hSel);	break;
		case ID_TEMPLATE_SELECTALL :				SelectAll();		break;
		case ID_TEMPLATE_UNSELECTALL :				UnselectAll();		break;

		case ID_TEMPLATE_EDITSCRIPT:				EditScript( hSel);		break;
		case ID_TEMPLATE_SETSCRIPT:					SetScript( hSel);		break;
		case ID_TEMPLATE_REMOVESCRIPT:				RemoveScript( hSel);	break;
		case ID_TEMPLATE_IEH:						ScriptManager::InsertEventHandler( szBlockname, false);	break;
		case ID_SCRIPT_TESTSCRIPT:					TestScript( hSel);		break;
	}

	OnSelectMenu( cmd);
	
	*pResult = 1;
}

void CUIPaneTemplate::EditScript(HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);

	i3GameNode* pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pNode != NULL);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	if( hItem == m_pRootItem)
	{
		i3UIManager* pUIManager = i3UI::getManager();
		I3ASSERT( pUIManager != NULL);

		i3UILibrary* pLib = pUIManager->getUILibrary();
		I3ASSERT( pLib != NULL);

		const char* pszScript = pLib->getScriptFile();

		pMainFrm->OnEditScript( pszScript, true, false);
	}
	else if( i3::same_of<i3UITemplate_User*>(pNode)) //->IsExactTypeOf( i3UITemplate_User::static_meta()))
	{
		i3UITemplate_User* pUser = (i3UITemplate_User*)pNode;
		const char* pszScript = pUser->GetName();

		pMainFrm->OnEditScript( pszScript, false, false);
	}
	else if( i3::same_of<i3UITemplate_MessageBox*>(pNode)) //->IsExactTypeOf( i3UITemplate_MessageBox::static_meta()))
	{
		i3UITemplate_MessageBox* pBox = (i3UITemplate_MessageBox*)pNode;
		const char* pszScript = pBox->GetName();

		pMainFrm->OnEditScript( pszScript, false, false);
	}
}

void CUIPaneTemplate::SetScript( HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);
	I3ASSERT( m_pRootItem == hItem);

	i3UIManager* pUIManager = i3UI::getManager();
	I3ASSERT( pUIManager != NULL);

	i3UILibrary* pLib = pUIManager->getUILibrary();
	I3ASSERT( pLib != NULL);

	i3GameNode * pNode = (i3GameNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pNode != NULL);

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
		
		CString filename = fileDlg.GetPathName();
		pLib->setScriptFile( (LPCTSTR)filename);
		ScriptManager::SetLibScript( (LPCTSTR)filename);

		//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		//pFrame->UpdatePropertyPane();

		m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		CFile cf;

		char conv[MAX_PATH];
		i3::snprintf( conv, sizeof(conv), "%s/%s", i3ResourceFile::GetWorkingFolder(), pLib->getScriptFile());
		BOOL bResult = cf.Open( conv, CFile::modeRead);
		
		if( bResult == FALSE)
		{
			RemoveScript( hItem);
		}

		//Close?
	}
}

void CUIPaneTemplate::RemoveScript( HTREEITEM hItem)
{
	I3ASSERT( hItem != NULL);

	i3UIManager* pUIManager = i3UI::getManager();
	I3ASSERT( pUIManager != NULL);

	i3UILibrary* pLibrary = pUIManager->getUILibrary();
	I3ASSERT( pLibrary != NULL);

	pLibrary->setScriptFile(NULL);
	ScriptManager::SetLibScript( NULL);

	//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	//pFrame->UpdatePropertyPane();

	m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
}

void CUIPaneTemplate::TestScript( HTREEITEM hItem)
{
	//CScriptPane* pPane = new CScriptPane();
	//pPane->Create( "Pane1", this, CRect(0,0,200,200), TRUE, 25874, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI,
	//	AFX_CBRS_REGULAR_TABS);// AFX_CBRS_CLOSE | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE);// | CBRS_LEFT | CBRS_FLOAT_MULTI);
	//pPane->EnableDocking( CBRS_ALIGN_ANY);
	//pPane->DockToFrameWindow( CBRS_ALIGN_RIGHT);

	//CRect rc;
	//GetClientRect( rc);
	
	////LPCTSTR pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW);

	////m_wnd.CreateEx( 0, pszClass, "asf", WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_THICKFRAME, CRect( 50, 50, 100, 100), 0, 0);

	//CDockPaneScript* pPane = new CDockPaneScript();
	//pPane->Create( "Pane1", this, CRect(0,0,200,200), TRUE, 25874, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI,
	//	AFX_CBRS_REGULAR_TABS);// AFX_CBRS_CLOSE | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE);// | CBRS_LEFT | CBRS_FLOAT_MULTI);
	//pPane->EnableDocking( CBRS_ALIGN_RIGHT);
	//pPane->DockToFrameWindow( CBRS_ALIGN_RIGHT);
	//DockPane( pPane);
	//pPane->AttachToTabWnd( this, DM_SHOW, TRUE, NULL);
	//m_Pane1.AttachToTabWnd( &m_Pane2, DM_SHOW, TRUE, NULL);
}

// template edit mode enable. work space에 통지하여 template control edit mode로 설정
void CUIPaneTemplate::Edit( i3UITemplate * pTemplate)
{
	if( pTemplate != NULL)
	{
		I3ASSERT( i3::kind_of<i3UITemplate*>(pTemplate)); //->IsTypeOf( i3UITemplate::static_meta()));

		theApp.SwitchView( EVIEW_TEMPLATE);

		CView * pActiveView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
		
		if( pActiveView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)))
		{
			Ci3UIEditorView_Template * pView = (Ci3UIEditorView_Template*)pActiveView;

			pView->SetCurrentItem( pTemplate);
		}
	}
}

void CUIPaneTemplate::AddFilter( HTREEITEM hItem)
{
	i3TreeNode * pNode = (i3TreeNode*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pNode != NULL);

	if( i3::same_of<i3UILTreeFilter*>(pNode)) //->IsExactTypeOf( i3UILTreeFilter::static_meta()))
	{
		i3UILTreeFilter * pFilter = i3UILTreeFilter::new_object_ref();

		pFilter->SetName("New Filter");

		pNode->AddChild( pFilter);

		AddTreeItem( pFilter, hItem);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN,"해당 노드에는 Filter를 추가할수 없습니다.");
	}
}

void CUIPaneTemplate::OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	i3NamedElement * pData = (i3NamedElement*)pDispInfo->item.lParam;
	if( pData == NULL)
		return;

	if( pDispInfo->item.pszText != NULL)
	{
		if( i3::kind_of<i3UITemplate*>(pData)) //->IsTypeOf( i3UITemplate::static_meta()))
		{
			i3UITemplate * pTemplate = m_pLibrary->FindByName( pDispInfo->item.pszText);
			if( pTemplate == NULL)
			{
				m_pLibrary->RenameElement((i3UITemplate*)pData, pDispInfo->item.pszText);

				m_TreeCtrl.SetItemText( pDispInfo->item.hItem, pDispInfo->item.pszText);
			}
			else
			{
				::MessageBox( m_hWnd, "해당 이름을 가진 Template가 이미 존재 합니다.", "이름 중복", MB_OK);
			}
		}
		else
		{
			pData->SetName( pDispInfo->item.pszText);

			m_TreeCtrl.SetItemText( pDispInfo->item.hItem, pDispInfo->item.pszText);
		}
	}
    
	*pResult = 0;
}

void CUIPaneTemplate::Rename( HTREEITEM hItem)
{
	::SendMessage( m_TreeCtrl.m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
}

i3NamedElement * CUIPaneTemplate::_Rec_FindParentByChild( i3NamedElement * pRoot, i3NamedElement * pItem)
{
	i3NamedElement * pParent = NULL;
	INT32 i;
	if( i3::same_of<i3UILTreeElement*>(pRoot)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
	{
		i3UILTreeElement * pElm		= (i3UILTreeElement*)pRoot;
		for( i = 0;i < pElm->getElementCount(); ++i)
		{
			i3UITemplate * pTemplate = (i3UITemplate*)pElm->getElement(i);
			if( pTemplate == pItem)
				return pRoot;

			pParent = _Rec_FindParentByChild( pTemplate, pItem);
			if( pParent != NULL)
			{
				return pParent;
			}
		}
	}
	else if( i3::kind_of<i3TreeNode*>(pRoot)) //->IsTypeOf( i3TreeNode::static_meta()))
	{
		i3TreeNode * pTNode = (i3TreeNode*)pRoot;
		for( i = 0;i < pTNode->getChildCount(); ++i)
		{
			i3TreeNode * pChild = pTNode->getChild( i);
			if( pChild == pItem)
				return pRoot;

			pParent = _Rec_FindParentByChild( pChild, pItem);
			if( pParent != NULL)
			{
				return pParent;
			}
		}
	}
	else if( i3::kind_of<i3UITemplate*>(pRoot)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		i3UITemplate * pTemplate = (i3UITemplate*)pRoot;
		for( i = 0;i < pTemplate->getSubTemplateCount(); ++i)
		{
			i3UITemplate * pSubTemplate = pTemplate->getSubTemplate(i);
			if( pSubTemplate == pItem)
				return pRoot;

			pParent = _Rec_FindParentByChild( pSubTemplate, pItem);
			if( pParent != NULL)
			{
				return pParent;
			}
		}
	}

	return NULL;
}

bool CUIPaneTemplate::_Rec_DeleteItems( i3NamedElement * pItem)
{
	if( pItem == NULL)
		return false;

	if( i3::same_of<i3UILTreeElement*>(pItem)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
	{
		i3UILTreeElement * pElm		= (i3UILTreeElement*)pItem;
		while( pElm->getElementCount() > 0)
		{
			i3UITemplate * pTemplate = (i3UITemplate*)pElm->getElement(0);

			if( !i3::kind_of<i3UITemplate_User*>(pTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()) == FALSE)
			{
				_Rec_DeleteItems((i3NamedElement*)pTemplate);
			}

			pElm->removeElement( pTemplate);
		}
	}
	else if( i3::kind_of<i3TreeNode*>(pItem)) //->IsTypeOf( i3TreeNode::static_meta()))
	{
		i3TreeNode * pTNode = (i3TreeNode*)pItem;
		while( pTNode->getChildCount() > 0)
		{
			i3TreeNode * pChild = pTNode->getChild( 0);

			_Rec_DeleteItems((i3NamedElement*)pChild);

			pTNode->RemoveChild( pChild);
		}
	}
	else if( i3::kind_of<i3UITemplate*>(pItem)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		if( m_pLibrary != NULL)
		{
			m_pLibrary->RemoveElement((i3UITemplate*)pItem);
		}
	}

	return true;
}

bool CUIPaneTemplate::Delete( HTREEITEM hItem)
{
	if( m_pRootItem == hItem)
	{
		MessageBox("라이브러리의 최상위 아이템은 제거할 수 없습니다.\n");
		return false;
	}

	INT32 ret = MessageBox( "템플릿을 제거 하시겠습니까? \n이 템플릿을 사용하고 있는 컨트롤이 존재하는 경우 오동작을 일으킬 수 있습니다.전혀 사용하고 있지 않은 것을 확신하는 경우 이거나, 템플릿 라이브러리를 제작 하는 경우에만 사용하여 주십시오", "확인", MB_YESNO|MB_ICONINFORMATION );
	if( ret != IDYES)
	{
		return false;
	}

	// 
	i3NamedElement * pElement = (i3NamedElement*)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pElement != NULL);

	// remove from parent
	i3NamedElement * pParentElement = _Rec_FindParentByChild( m_pRoot, pElement);
	I3ASSERT( pParentElement != NULL);

	if( i3::kind_of<i3UITemplate*>(pElement)) //->IsTypeOf( i3UITemplate::static_meta()) )
	{
		if( i3::kind_of<i3UITemplate*>(pParentElement)) //->IsTypeOf( i3UITemplate::static_meta()) )
		{
			if(!i3::kind_of<i3UITemplate_User*>(pParentElement)) //->IsTypeOf( i3UITemplate_User::static_meta()) == FALSE)
			{
				MessageBox("SubTemplate는 Custom Template만 추가/삭제 할 수 있습니다.", "오류", MB_OK);
				return false;
			}
		}
	}

	if(!i3::kind_of<i3UITemplate_User*>(pParentElement)) //->IsTypeOf( i3UITemplate_User::static_meta()) == FALSE)
	{
		_Rec_DeleteItems( pElement);
	}

	if( i3::same_of<i3UILTreeElement*>(pParentElement)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
	{
		I3ASSERT( i3::kind_of<i3UITemplate*>(pElement)); //->IsTypeOf( i3UITemplate::static_meta()));

		i3UILTreeElement * pElm		= (i3UILTreeElement*)pParentElement;

		pElm->removeElement((i3UITemplate*)pElement);
	}
	else if( i3::kind_of<i3TreeNode*>(pParentElement)) //->IsTypeOf( i3TreeNode::static_meta()))
	{
		I3ASSERT( i3::kind_of<i3TreeNode*>(pElement)); //->IsTypeOf( i3TreeNode::static_meta()));

		i3TreeNode * pTNode = (i3TreeNode*)pParentElement;

		pTNode->RemoveChild((i3TreeNode*)pElement);
	}
	else if( i3::kind_of<i3UITemplate*>(pParentElement)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		I3ASSERT( i3::kind_of<i3UITemplate*>(pElement)); //->IsTypeOf( i3UITemplate::static_meta()));

		i3UITemplate * pTemplate = (i3UITemplate*)pParentElement;

		pTemplate->removeSubTemplate((i3UITemplate*)pElement);
	}

	m_TreeCtrl.DeleteItem( hItem);

	return true;
}

// filter면 class select dialog를 element이면 바로 add후 tree에 추가.
void CUIPaneTemplate::Add( HTREEITEM hParentItem)		
{
	i3NamedElement * pElement = (i3NamedElement*)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pElement != NULL);

	char conv[MAX_PATH], conv1[MAX_PATH];

	HTREEITEM			hItem			= hParentItem;
	i3ClassMeta		*	pMeta			= NULL;
	i3UILTreeElement *	pOwnerElm		= NULL;

	if( i3::kind_of<i3TreeNode*>(pElement)) //->IsTypeOf( i3TreeNode::static_meta()))	// tree node인 경우
	{
		// Filter를 선택한 경우
		if( i3::same_of<i3UILTreeFilter*>(pElement)) //->IsExactTypeOf( i3UILTreeFilter::static_meta()))
		{
			//추가할 uitemplate의 클래스를 지정해주고
			CDlg_SelTemplateType dlg;

			if( dlg.DoModal() == IDOK)
			{
				pMeta = i3ClassMetaSystem::i()->find_class_meta(dlg.m_szSelectedMeta);
					//i3ClassMeta::FindClassMetaByName( dlg.m_szSelectedMeta);
				if( pMeta == NULL)	// 메타를 선택하지 않고 취소하였다면, 아무것도 하지 않는다.
					return;
			}
			else 
				return;

			//하위에 동일한 classmeta를 가진 element가 있는지 확인한후.
			i3UILTreeFilter * pFilter = (i3UILTreeFilter *)pElement;

			pOwnerElm = pFilter->FindElementByMeta( pMeta);
			
			// 없다면, 새로 element를 하나 추가해준다.
			if( pOwnerElm == NULL)
			{
				i3::string_ncopy_nullpad( conv, pMeta->class_name().c_str(), MAX_PATH);
				i3::snprintf( conv1, MAX_PATH -1, "%s", &conv[13]);

				pOwnerElm = i3UILTreeElement::new_object_ref();
				pOwnerElm->setElementMeta( pMeta);
				pOwnerElm->SetName( conv1);

				pFilter->AddChild( pOwnerElm);

				hItem = AddTreeItem( pOwnerElm, hParentItem);
			}
			else
			{
				HTREEITEM hChildItem = m_TreeCtrl.GetChildItem( hParentItem);
				while( hChildItem != NULL)
				{
					if( m_TreeCtrl.GetItemData( hChildItem) == (DWORD_PTR) pOwnerElm)
					{
						break;
					}

					hChildItem = m_TreeCtrl.GetNextItem( hChildItem, TVGN_NEXT);
				}

				I3ASSERT( hChildItem != NULL);
				I3ASSERT( hChildItem != hItem);

				hItem = hChildItem;
			}
		}
		else if( i3::kind_of<i3UILTreeElement*>(pElement)) //->IsTypeOf( i3UILTreeElement::static_meta()))
		{
			pOwnerElm = (i3UILTreeElement*)pElement;
			
			pMeta = pOwnerElm->getElementMeta();
		}

		i3::string_ncopy_nullpad( conv, pMeta->class_name().c_str(), MAX_PATH);
		m_pLibrary->FindUniqueName( &conv[13], conv1);// (char*)&conv[13] = "i3UITemplate_ <- 요걸 제거한 template 이름

		i3UITemplate * pNewTemplate = (i3UITemplate*)pMeta->create_instance(); //	CREATEINSTANCE( pMeta);

		pNewTemplate->Create();
		pNewTemplate->SetResourcePath( conv1);

		pOwnerElm->AddElement( pNewTemplate);

		// library에 추가
		m_pLibrary->AddElement( pNewTemplate);

		Rec_SetTree( hItem, pNewTemplate);
	}
	else if( i3::kind_of<i3UITemplate_User*>(pElement)) //->IsTypeOf( i3UITemplate_User::static_meta()))
	{
		//추가할 uitemplate의 클래스를 지정해주고
		CDlg_SelTemplateType dlg;

		if( dlg.DoModal() == IDOK)
		{
			pMeta = i3ClassMetaSystem::i()->find_class_meta(dlg.m_szSelectedMeta);
				// i3ClassMeta::FindClassMetaByName( dlg.m_szSelectedMeta);
			if( pMeta == NULL)	// 메타를 선택하지 않고 취소하였다면, 아무것도 하지 않는다.
				return;
		}
		else 
			return;

		i3::string_ncopy_nullpad( conv, pMeta->class_name().c_str(), MAX_PATH);

		i3UITemplate * pNewTemplate = (i3UITemplate*)pMeta->create_instance(); //CREATEINSTANCE( pMeta);

		pNewTemplate->Create();
		pNewTemplate->SetResourcePath( conv);

		i3UITemplate * pParentTemplate = (i3UITemplate*)pElement;

		pParentTemplate->addSubTemplate( pNewTemplate);

		Rec_SetTree( hItem, pNewTemplate);
	}
}

// template locate mode enable. work space에 통지하여 template control locate mode로 설정
void CUIPaneTemplate::Locate( HTREEITEM hItem)
{

}

BOOL CUIPaneTemplate::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask = TVIF_HANDLE;
	data.hItem = hItem;

	return m_TreeCtrl.GetItem( &data);
}

// 해당 filter or control제거
void CUIPaneTemplate::Cut( HTREEITEM hItem)		// filter
{

}

void CUIPaneTemplate::Copy( HTREEITEM hItem)
{
	i3ElementBase* pItem = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);

	//Template을 Copy하였다. 
	if(  i3::kind_of<i3UITemplate*>(pItem)) //->IsTypeOf( i3UITemplate::static_meta()))
	{
		I3TRACE("Item Copied!!!!\n");
		s_hCopiedTreeItem = hItem;
	}
	else
	{
		s_hCopiedTreeItem = NULL;
	}
}

void CUIPaneTemplate::Paste( HTREEITEM hItem)
{
	if( s_hCopiedTreeItem == NULL)
	{
		I3TRACE("No Copied Item\n");
		return;
	}

	i3ElementBase* pItem = (i3ElementBase*)m_TreeCtrl.GetItemData( hItem);

	if( pItem != NULL)
	{
		if( i3::kind_of<i3UITemplate*>(pItem)) //->IsTypeOf( i3UITemplate::static_meta()))		//Template에 Paste
		{
			DragDrop( s_hCopiedTreeItem, hItem, false);
		}
		//else if( i3::kind_of<i3UILTreeElement*>(pItem)) //->IsTypeOf( i3UILTreeElement::static_meta()))	//Filter에 Paste
		//{
			//DragDrop( s_hCopiedTreeItem, hItem, true);	
		//}
	}
}

// 잡다 트리 컨트롤
void CUIPaneTemplate::DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone)
{
	i3ElementBase * pSrcObj, * pTargetObj;

	pSrcObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hSrcItem);
	pTargetObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hTargetItem);

	I3ASSERT( i3::kind_of<i3UITemplate*>(pSrcObj)); //->IsTypeOf( i3UITemplate::static_meta()));

	i3UITemplate * pDragTemplate = (i3UITemplate*)pSrcObj;
	if( pDragTemplate->same_of( pTargetObj->meta()))
	{
		i3UITemplate * pTargetTemplate = (i3UITemplate*)pTargetObj;

		if( i3::same_of<i3UITemplate_User*>(pDragTemplate)) //->IsExactTypeOf( i3UITemplate_User::static_meta()))
		{
			CDragDropOptionDlg Dlg;
			if( Dlg.DoModal() == IDOK)
			{
				if( Dlg.m_nSelectedItem == 0)
				{
					pDragTemplate->CopyTo( pTargetTemplate, I3_COPY_INSTANCE);
				}
				else if( Dlg.m_nSelectedItem == 1)
				{
					pTargetTemplate->addSubTemplate( pDragTemplate);
					Rec_SetTree( hTargetItem, pDragTemplate);
				}
			}
		}
		else
		{
			INT32 ret = MessageBox( "덮어 쓰시겠습니까?", "알림", MB_YESNO|MB_ICONINFORMATION );
			if( ret != IDYES)
				return;

			pDragTemplate->CopyTo( pTargetTemplate, I3_COPY_INSTANCE);
		}
	}
	else
	{

	}

	// 이미 그 템플릿이 열려 있는 경우도 있기 때문에 
	// Update 를 쳐줘야 하는데.. 
	// ??()
}

void CUIPaneTemplate::Expand( HTREEITEM hItem)
{

}

void CUIPaneTemplate::Collapse( HTREEITEM hItem)
{

}

void CUIPaneTemplate::SelectAll(void)
{

}

void CUIPaneTemplate::UnselectAll(void)
{

}

// 메뉴 
void CUIPaneTemplate::OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem)
{

}

void CUIPaneTemplate::OnSelectMenu( UINT id)
{

}

// save / load
bool CUIPaneTemplate::Export( const char * pszFileName)
{
	return true;
}

bool CUIPaneTemplate::Import( const char * pszFileName)
{
	return true;
}


int CUIPaneTemplate::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	// Tree Control
	{
		CRect rect;

		GetClientRect( &rect);

		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_TRACKSELECT |
								TVS_INFOTIP | TVS_LINESATROOT | TVS_EDITLABELS,
								rect, this, ID_TEMPLATE_TREECTRL) == FALSE)
		{
			I3ASSERT_0;
		}

		m_TreeCtrl.InitImageList();
	}

	i3TDK::RegisterUpdate( m_hWnd, i3UILibrary::static_meta(), I3_TDK_UPDATE_SET);

	return 0;
}

void CUIPaneTemplate::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( i3::same_of<i3UILibrary*>(pInfo->m_pMeta))
				SetUILibrary( (i3UILibrary*)pInfo->m_pObject);
	}
}

LRESULT CUIPaneTemplate::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDockablePane::WindowProc(message, wParam, lParam);
}

void CUIPaneTemplate::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}


void CUIPaneTemplate::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW *) pNMHDR;

	//OnChangeSelectItem( pNMTV->itemOld.hItem, pNMTV->itemNew.hItem);
	i3NamedElement * pOldObj = (i3NamedElement*) pNMTV->itemOld.lParam;
	i3NamedElement * pNewObj = (i3NamedElement*) pNMTV->itemNew.lParam;

	if(pOldObj != pNewObj)
	{
		// 이전에 선택되어 있던 개체가 있는경우, 새로 선택된 개체와 다르다면 unselect broadcast
		if( pOldObj != NULL)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pOldObj);

		// 새로 선택된 개체가 있다면, select broadcast
		if( pNewObj != NULL)
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNewObj);
	}

	*pResult = 0;
}

void CUIPaneTemplate::OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVKEYDOWN * pNMTV = (NMTVKEYDOWN *) pNMHDR;
	
	//HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

	switch( pNMTV->wVKey)
	{
	case 'D':
	default:
		break;
	}

	*pResult = 0;
}

void CUIPaneTemplate::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
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

		if((m_hTargetItem != NULL) && (m_hDragItem != NULL) && (m_bDropEnable == true))
		{
			if( nFlags & MK_SHIFT)
				DragDrop( m_hDragItem, m_hTargetItem, true);
			else
				DragDrop( m_hDragItem, m_hTargetItem, false);

			m_TreeCtrl.SetItemState( m_hTargetItem, 0x00000000, TVIS_DROPHILITED);
		}
		else
		{
			i3UITemplate * pTemplate = (i3UITemplate *)m_TreeCtrl.GetItemData( m_hDragItem);
			if( pTemplate != NULL)
			{
				CPoint pt;
				pt = point;

				ClientToScreen( &pt);

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
				pMainWnd->DropObject( pTemplate, pt);
			}
		}
	}

	I3TRACE("UP\n");

	CDockablePane::OnLButtonUp(nFlags, point);
}

void CUIPaneTemplate::OnMButtonDown( UINT nFlags, CPoint point)
{
	//Test
	//HTREEITEM				m_pRootItem;
	//CTemplateTree			m_TreeCtrl;

	//m_TreeCtrl.SetItemState( m_pRootItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);

	CDockablePane::OnMButtonDown(nFlags, point);
}

void CUIPaneTemplate::OnTVNBeginDragTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == NULL)
		return;

	i3ElementBase * pItemData = (i3ElementBase*) m_TreeCtrl.GetItemData( pNMTreeView->itemNew.hItem);
	if( pItemData == NULL)
		return;

	if( !i3::kind_of<i3UITemplate*>(pItemData)) //->IsTypeOf( i3UITemplate::static_meta()) == FALSE)
		return;

	I3TRACE("DRAG START\n");
	{
		CPoint pt = pNMTreeView->ptDrag;
		int cxCursor, cyCursor;

		cxCursor = 10;
		cyCursor = 10;

		GetCursorPos( &pt);

		SetCapture();

		m_bDragging = true;

		m_pDragImage = m_TreeCtrl.CreateDragImage( pNMTreeView->itemNew.hItem);
		if( m_pDragImage != NULL)
		{
			m_pDragImage->DragShowNolock( FALSE);
			m_pDragImage->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
			m_pDragImage->DragEnter( NULL, pt);
		}
	
		m_hTargetItem = NULL;
		m_hDragItem = pNMTreeView->itemNew.hItem;
	}
}

void CUIPaneTemplate::OnNMDblClickTree(NMHDR * pNMHDR, LRESULT *pResult)
{
	CPoint point;
	HTREEITEM hSel;

	GetCursorPos( &point);
	m_TreeCtrl.ScreenToClient( &point);

	hSel = m_TreeCtrl.HitTest( point, NULL);

	if( hSel != NULL)	// 새 아이템과 현재 target이 다른경우
	{
		i3ElementBase * pSelItem = (i3ElementBase*)m_TreeCtrl.GetItemData( hSel);
		if( pSelItem != NULL)
		{
			if( i3::kind_of<i3UITemplate*>(pSelItem)) //->IsTypeOf( i3UITemplate::static_meta()))
			{
				Edit((i3UITemplate*)pSelItem);
			}
		}
	}
}

void CUIPaneTemplate::OnMouseMove(UINT nFlags, CPoint point)
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

			{
				CPoint pt;
				pt = point;

				ClientToScreen( &pt);

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
				pMainWnd->DragMoveObjectTemplate( pt);
			}
		}

		if( m_pDragImage != NULL)
		{
			// Cursor
			ClientToScreen( &point);

			m_pDragImage->DragMove( point);
		}
	}
	else
	{
		CDockablePane::OnMouseMove(nFlags, point);
	}
}

void CUIPaneTemplate::ShowTemplateTree( i3UITemplate* pTemplate)
{
	m_TreeCtrl.DeleteAllItems();

	if( pTemplate != NULL)
		Rec_SetTree( NULL, pTemplate);
}

void CUIPaneTemplate::OverWriteTemplate( i3UITemplate* pTemplate, CPoint point)
{
	CPoint pt(point);
	m_TreeCtrl.ScreenToClient( &pt);

	HTREEITEM hTarget = m_TreeCtrl.HitTest( pt);

	if( hTarget != NULL)
	{
		i3ElementBase* pTarget = (i3ElementBase*)m_TreeCtrl.GetItemData( hTarget);

		if( pTarget->same_of( pTemplate->meta()))
		{
			INT32 nAnswer = ::MessageBox( m_hWnd, "Overwrite?", "Warning", MB_YESNO);
			if( nAnswer == IDYES)
			{
				i3UITemplate* pDest = (i3UITemplate*)pTarget;

				char buf[MAX_PATH];
				i3::string_ncopy_nullpad( buf, pDest->GetName(), MAX_PATH);

				pTemplate->CopyTo( pDest, I3_COPY_INSTANCE);

				pDest->SetName( buf);
			}
		}
		else
		{
			::MessageBox( m_hWnd, "Cannot overwrite between different types", "Warning", MB_OK);
		}

		m_TreeCtrl.SetItemState( hTarget, 0, TVIS_DROPHILITED);
	}
}

void CUIPaneTemplate::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	theApp.SwitchView( EVIEW_TEMPLATE);
}

void CUIPaneTemplate::HighLightItemAtPoint( POINT point, bool bHighlight)
{
	static HTREEITEM hOldTarget = NULL;

	CPoint pt(point);
	m_TreeCtrl.ScreenToClient( &pt);

	HTREEITEM hTarget = m_TreeCtrl.HitTest( pt);

	if( hTarget != NULL)
	{
		UINT nState = bHighlight ? TVIS_DROPHILITED : 0;
		m_TreeCtrl.SetItemState( hTarget, nState, TVIS_DROPHILITED);
	}
	
	if( hOldTarget != NULL && hOldTarget != hTarget)
	{
		m_TreeCtrl.SetItemState( hOldTarget, 0, TVIS_DROPHILITED);
	}

	hOldTarget = hTarget;
}