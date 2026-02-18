// PanelResource.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelResource.h"
#include "MainFrm.h"
#include "GlobalVariable.h"
#include "DlgReplaceRes.h"
#include "i3Base/pool/create_by_common_pool.h"

// CPanelResource dialog

IMPLEMENT_DYNAMIC(CPanelResource, i3TDKDialogBase)
CPanelResource::CPanelResource(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelResource::IDD, pParent)
{
	m_pRoot	= NULL;
	memset( m_hTypeRoot, NULL, sizeof( HTREEITEM) * I3LV_RESDB_TYPE_COUNT);

	m_bDragging = false;
	m_pDragImage = NULL;
	m_hTargetItem = m_hDragItem = NULL;

	m_szDB[0] = 0;
}

CPanelResource::~CPanelResource()
{
	I3_SAFE_RELEASE( m_pRoot);
	
//	m_ResTreeCtrl.DeleteAllItems();		// MainFrame소멸자호출시 따라 소멸되면, 여기서 그냥 조용이 종료되고, 릭만 왕창생김...따라서..
}										// 여기서 이 함수를 호출하면 안될것 같다....(2012.09.12.수빈)

void CPanelResource::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RESLIST, m_ResTreeCtrl);
	DDX_Control(pDX, IDC_ST_TITLE, m_ST_Title);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_ED_FILE, m_ED_File);
}


BEGIN_MESSAGE_MAP(CPanelResource, i3TDKDialogBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RESLIST, OnTvnSelchangedTreeReslist)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_RESLIST, OnTvnBegindragTreeReslist)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_RESLIST, OnNMRclickTreeReslist)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPanelResource message handlers

int CPanelResource::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	i3TDK::RegisterUpdate( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);

	return 0;
}

void CPanelResource::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_ResTreeCtrl.m_hWnd))
	{
		m_ResTreeCtrl.SetWindowPos( NULL, 4, 4, cx - 8, cy - 8, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelResource::BuildResTree()
{
	if( g_pResDB == NULL)
		return;

	i3LevelResRef * pNewRoot = g_pResDB->getResRoot();

	I3_REF_CHANGE( m_pRoot, pNewRoot);

	m_ResTreeCtrl.DeleteAllItems();

	if( m_pRoot == NULL)
		return;

	INT32 nCnt = m_pRoot->getChildCount();
	for( INT32 i = 0; i < nCnt; ++i)
	{
		m_hTypeRoot[i] = m_ResTreeCtrl.Rec_AddTreeItem( TVI_ROOT, g_pResDB->getResTypeRoot(i));
	}
}

void CPanelResource::BuildResTree( INT32 typeIdx)
{
	if( g_pResDB == NULL)
		return;

	if( (typeIdx >= I3LV_RESDB_TYPE_COUNT) || (typeIdx <= -1))
	{
		I3TRACE("TYPE INDEX가 유효하지 않습니다.\n");
		return;
	}

	i3LevelResRef * pTypeRoot = g_pResDB->getResTypeRoot(typeIdx);

	HTREEITEM hItem = m_hTypeRoot[typeIdx];
	if( hItem != NULL)
	{
		while( m_ResTreeCtrl.GetChildItem( hItem) != NULL)
		{
			HTREEITEM hChild = m_ResTreeCtrl.GetChildItem( hItem);

			m_ResTreeCtrl.DeleteItem( hChild);
		}
	}

	if( pTypeRoot != NULL)
	{
		for( INT32 i = 0;i < pTypeRoot->getChildCount(); ++i)
		{
			i3LevelResRef * pChildRef = (i3LevelResRef*)pTypeRoot->getChild( i);

			m_ResTreeCtrl.Rec_AddTreeItem( hItem, pChildRef);
		}
	}
}

BOOL CPanelResource::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();
	
	m_ResTreeCtrl.InitImageList();

	{
		m_ST_Title.ShowWindow( SW_HIDE);
		m_Progress.ShowWindow( SW_HIDE);
		m_ED_File.ShowWindow( SW_HIDE);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3LevelRes::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelResource::OnTvnSelchangedTreeReslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	i3LevelResRef * pRef = (i3LevelResRef *)m_ResTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
	I3ASSERT( pRef != NULL);

	CString str = m_ResTreeCtrl.GetItemText( pNMTreeView->itemNew.hItem);

	i3LevelRes	*	pRes = (i3LevelRes *)pRef->getRes();
	if( pRes != NULL)
	{
		i3LevelFramework * pFramework;

		pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

		pFramework->SetSelectedRes( pRes);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pRes);
	}
	else
	{
		m_hSelectedResourceItem = pNMTreeView->itemNew.hItem;
	}

	*pResult = 0;
}

void CPanelResource::OnTvnBegindragTreeReslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == NULL)
		return;

	i3LevelResRef * pResRef = (i3LevelResRef *) m_ResTreeCtrl.GetItemData( pNMTreeView->itemNew.hItem);
	i3LevelRes * pRes = pResRef->getRes();

	if((pRes != NULL) && (pRes->isConcretiveRes()))
	{
		CPoint pt = pNMTreeView->ptDrag;
		int cxCursor, cyCursor;

		cxCursor = 10;
		cyCursor = 10;

		//ClientToScreen( &pt);
		GetCursorPos( &pt);

		SetCapture();

		m_bDragging = true;
		//m_pDragObject = pObj;

		m_pDragImage = m_ResTreeCtrl.CreateDragImage( pNMTreeView->itemNew.hItem);

		m_pDragImage->DragShowNolock( FALSE);
		m_pDragImage->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
		m_pDragImage->DragEnter( NULL, pt);
	
		m_hTargetItem = NULL;
		m_hDragItem = pNMTreeView->itemNew.hItem;
	}
}

void CPanelResource::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		ClientToScreen( &point);

		m_pDragImage->DragMove( point);

		// Cursor
		/*
		HTREEITEM hTarget = m_ResTreeCtrl.HitTest( point);
		HCURSOR hNewCursor = m_hCursor[1];

		if( m_hTargetItem != NULL)
		{
			m_ResTreeCtrl.SetItemState( m_hTargetItem, 0, TVIS_DROPHILITED);
			m_hTargetItem = NULL;
		}

		if( hTarget != NULL)
		{
			i3ElementBase * pObj = (i3ElementBase *) m_ResTreeCtrl.GetItemData( hTarget);

			if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()) )
			{
				// OK
				if( pObj != m_pDragObject)
				{
					hNewCursor = m_hCursor[0];
					m_hTargetItem = hTarget;
				}
			}
			else
			{
				CString str = m_ResTreeCtrl.GetItemText( hTarget);

				if( str == "AI")
				{
					// OK
					hNewCursor = m_hCursor[0];
					m_hTargetItem = hTarget;
				}
			}
		}

		if( m_hTargetItem != NULL)
		{
			m_ResTreeCtrl.SetItemState( m_hTargetItem, 0xFFFFFFFF, TVIS_DROPHILITED);
		}

		i3TDK::SetCursor( m_hWnd, hNewCursor);
		*/
	}

	i3TDKDialogBase::OnMouseMove(nFlags, point);
}

void CPanelResource::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		m_bDragging = false;
		ReleaseCapture();

		m_pDragImage->DragLeave( &m_ResTreeCtrl);
		m_pDragImage->EndDrag();

		delete m_pDragImage;

		{
			i3LevelResRef * pResRef = (i3LevelResRef *) m_ResTreeCtrl.GetItemData( m_hDragItem);
			i3LevelRes * pRes = pResRef->getRes();

			if( pRes != NULL)
			{
				ClientToScreen( &point);

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
				pMainWnd->DropObject( pRes, point);
			}
		}
	}

	i3TDKDialogBase::OnLButtonUp(nFlags, point);
}

void CPanelResource::_OnPopupMenu( CMenu * pMenu, HTREEITEM hItem, i3LevelResRef * pRef)
{
	if( pRef == NULL)
		return;

	i3LevelRes * pRes = pRef->getRes();
	if( pRes == NULL)
		return;

	if( i3::kind_of<i3LevelResSceneGraph*>(pRes)) //->IsTypeOf( i3LevelResSceneGraph::static_meta()))
	{
		pMenu->AppendMenu( MF_SEPARATOR, 0, (LPCTSTR) NULL);
		pMenu->AppendMenu( MF_BYCOMMAND | MF_STRING, ID_RESPOPUP_NEWWORLD, "&New World resource");
	}
}

void CPanelResource::OnNMRclickTreeReslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CMenu	Menu, *  pPopup;
	POINT pt;
	UINT32	cmd;
	HTREEITEM	hSel = NULL;

	GetCursorPos( &pt);

	{
		POINT	cpt;
		UINT	flag;

		cpt = pt;

		m_ResTreeCtrl.ScreenToClient( &cpt);

		hSel = m_ResTreeCtrl.HitTest( cpt, &flag);

		if(( hSel != NULL) && ( TVHT_ONITEM & flag))
		{
			m_ResTreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	Menu.LoadMenu( IDR_I3LVRES_POPUP_RESOURCE);

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == NULL)
	{
		pPopup->EnableMenuItem( ID_I3LV_RES_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		i3LevelResRef	*	pResRef = (i3LevelResRef *)m_ResTreeCtrl.GetItemData( hSel);
		i3LevelRes * pRes = pResRef->getRes();

		if( pRes == NULL)
		{
			pPopup->EnableMenuItem( ID_I3LV_RES_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			pPopup->EnableMenuItem( ID_I3LV_RES_ADD, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_I3LV_RES_SCAN, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		_OnPopupMenu( pPopup, hSel, pResRef);
 	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_I3LV_RES_ADD:			_AddNewResource();		break;
		case ID_I3LV_RES_DELETE:		_DeleteResource(hSel);	break;
		case ID_I3LV_RES_SCAN:			_RescanResourceList();	break;
		case ID_I3LV_RES_SELECT_ALL:	_SelectAll();			break;
		case ID_I3LV_RES_COLLAPSE_ALL:	_CollapseAll();			break;
		case ID_I3LV_RES_EXPAND_ALL:	_ExpandAll();			break;
		case ID_RESPOPUP_NEWWORLD :		_AddNewWorld( hSel);	break;
		case ID_I3LV_RES_CHECKDB :		g_pResDB->CheckDuplicateID();	break;
		case ID_RESOURCE_REPLACERESOURCEOFELEMENTS :
			_ReplaceResource( hSel);
			break;

		default:
			break;
	}

	*pResult = 1;
}

/////////////////////////////////////////////////////////////////////////////
void CPanelResource::_AddNewResource(void)
{
// 	CFileDialog	Dlg( TRUE, "*.*", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
// 		"Resource Files( *.i3s;*.i3i;*.tga;*.wav;*.i3a)|*.i3s;*.i3i;*.tga;*.wav;*.i3a|Object Files(*.I3OBJ;*.I3CHR;*.I3WRD)|*.I3OBJ;*.I3CHR;*.I3WRD|UI Files(*.gui)|*.gui|Script Files(*.Lua;*.ini;*.dat)|*.Lua;*.ini;*.dat|All Files (*.*)|*.*||", this);
// 
// 	if( Dlg.DoModal() != IDOK)	return;

// 	char szExt[32];
// 	i3LevelRes * pRes = NULL;
// 	WIN32_FIND_DATA FindInfo;
// 
// 	HANDLE hFile = ::FindFirstFile( LPCTSTR( Dlg.GetPathName()), &FindInfo);
// 	if( hFile == NULL)
// 		return;
// 
// 	if( FindInfo.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
// 		return;
// 
// 	i3String::SplitFileExt( FindInfo.cFileName, szExt, sizeof(szExt) - 1);
// 
// 	if( i3String::Compare( szExt, "I3CHR") == 0)
// 	{
// 		pRes = i3LevelResChara::new_object();
// 	}
// 	else if( i3String::Compare( szExt, "I3OBJ") == 0)
// 	{
// 	}
// 	else if( i3String::Compare( szExt, "I3S") == 0)
// 	{
// 		pRes = i3LevelResSceneGraph::new_object();
// 	}
// 	else if( i3String::Compare( szExt, "I3A") == 0)
// 	{
// 		pRes = i3LevelResAnimation::new_object();
// 	}
// 	else if(	(i3String::Compare( szExt, "I3I") == 0) ||
// 		(i3String::Compare( szExt, "TGA") == 0) )
// 	{
// 		pRes = i3LevelResTexture::new_object();
// 	}
// 	else if( i3String::Compare( szExt, "WAV") == 0)
// 	{
// 		pRes = i3LevelResSound::new_object();
// 	}
// 
// 	if( pRes != NULL)
// 	{
// 		char szRelPath[MAX_PATH];
// 		i3String::MakeRelativePath( g_pProfile->m_szResPath, LPCTSTR( Dlg.GetPathName()), szRelPath);
// 
// 		pRes->setPath( szRelPath);
// 		pRes->setLastUpdateFileTime( &FindInfo.ftLastWriteTime);
// 
// 		if( pRes->OnValidateBrief( false))
// 		{
// 			g_pResDB->AddRes( pRes);
// 		}
// 
// 		pRes->Release();
// 	}
}

void CPanelResource::_DeleteResource( HTREEITEM hItem)
{
	

	i3LevelResRef * pResRef = (i3LevelResRef *) m_ResTreeCtrl.GetItemData( hItem);
	i3LevelRes * pSrcRes = pResRef->getRes();

	INT32 rv = MessageBox( "해당 Resource File을 영구히 삭제하겠습니까?", "Confirm", MB_YESNOCANCEL);
	if( rv == IDCANCEL)
		return;

	if( rv == IDYES)
	{
		char szPath[MAX_PATH];
		
		sprintf( szPath, "%s/%s", g_pProfile->m_szResPath, pSrcRes->getPath());
		::DeleteFile( szPath);

		sprintf( szPath, "%s/%s", g_pProfile->m_szWorkingDir, pSrcRes->getPath());
		::DeleteFile( szPath);
	}

	// 현재 Scene에서 해당 Resource를 사용하는 모든 Element들을 구하여 삭제한다.
	{
		i3::vector<i3LevelElement3D*> list;

		g_pScene->FindElementByResource( list, pSrcRes);

		if( list.size() > 1)	i3LevelUndo::BeginBatch();

		for(size_t i = 0; i < list.size(); i++)
		{
			i3LevelElement3D * pElm = list[i];

			g_pScene->RemoveElement( pElm);
		}

		if( list.size() > 1)	i3LevelUndo::EndBatch();
	}

	g_pResDB->RemoveRes( pSrcRes);

	g_pResDB->FlushDB();

	m_ResTreeCtrl.DeleteItem( hItem);
}

void CPanelResource::_SelectAll(void)
{

}

void CPanelResource::_RescanResourceList(void)
{
	char szPath[MAX_PATH];
	sprintf( szPath, "%s/%s", g_pProfile->m_szResPath, RESDB_FILENAME);

	//
	// 자동검색 전에 공통리소스를 미리 생성한다.. 스캔과정은 멀티쓰레드+WindowMsg조합이 사용되서, 
	// 스캔종료시, 생성되지 않은 공통리소스 접근이 일어날수 있다..(2012.09.11.수빈)
	//
	i3LevelScene::CreateCommonResource();
	//

	//Scan( szPath );
}

void CPanelResource::_ReplaceResource( HTREEITEM hItem)
{
	if( hItem == NULL)
		return;

	i3LevelResRef * pResRef = (i3LevelResRef *) m_ResTreeCtrl.GetItemData( hItem);
	i3LevelRes * pSrcRes = pResRef->getRes();
	i3LevelRes * pDestRes = NULL;

	{
		CDlgReplaceRes dlg;

		if( dlg.Execute( pSrcRes) == false)
			return;

		pDestRes = dlg.getDestRes();
		
		pDestRes->OnValidate( g_pScene, true);
	}

	g_pScene->ReplaceResOfElement( pSrcRes, pDestRes);
}

void	CPanelResource::_AddNewWorld( HTREEITEM hItem)
{
	char szName[256];
	char szPath[ MAX_PATH];

	if( hItem == NULL)
		return;

	i3LevelResRef * pResRef = (i3LevelResRef *) m_ResTreeCtrl.GetItemData( hItem);
	i3LevelResSceneGraph * pSgRes;

	if( !i3::kind_of<i3LevelResSceneGraph*>(pResRef->getRes())) //->IsTypeOf( i3LevelResSceneGraph::static_meta()) == FALSE )
	{
		AfxMessageBox( "잘못된 Resource입니다. World Resource를 생성하지 않습니다.", MB_OK);
		return;
	}

	pSgRes = (i3LevelResSceneGraph *) pResRef->getRes();

	{
		i3TDKDlgRename dlg;
		char szTemp[MAX_PATH];

		strcpy_s( szTemp, _countof( szTemp), pSgRes->GetName());

		// 이름 뒤에 "_World"가 붙어있다면 제거.
		int pos = i3::icontain_string( szTemp, "_world");
		if( pos > 0)
		{
			szTemp[pos] = 0;
		}

		if( dlg.Execute( "생성할 World Resource의 이름을 입력해주시오...", szTemp) == FALSE)
		{
			return;
		}

		strcpy( szName, dlg.getName());
	}

	if( pSgRes->OnValidate( g_pScene, false) == false)
	{
		NotifyBox( m_hWnd, "%s Resource를 활성화할 수 없습니다.", pSgRes->getPath());
		return;
	}

	i3LevelResWorld * pWorldRes = i3LevelResWorld::new_object();

	pWorldRes->SetName( szName);

	sprintf( szPath, "World/%s/%s.i3WRD", szName, szName);

	pWorldRes->setPath( szPath);
	pWorldRes->CreateEmptyWorld();
	pWorldRes->getWorld()->SetWorld( pSgRes->getSceneGraphInfo());

	{
		pWorldRes->OnSerialize();
	}

	g_pResDB->AddRes( pWorldRes);
}

/////////////////////////////////////////////////////////////////////////////
static BOOL _ExpandProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	if( pData == NULL)
		pCtrl->Expand( hItem, TVE_EXPAND);
	else
		pCtrl->Expand( hItem, TVE_COLLAPSE);

	return TRUE; 
}

void CPanelResource::_CollapseAll()
{
	i3TDK::TreeCtrlTraverse( &m_ResTreeCtrl, _ExpandProc, (void *)1);
}

void CPanelResource::_ExpandAll()
{
	i3TDK::TreeCtrlTraverse( &m_ResTreeCtrl, _ExpandProc, NULL);
}

void CPanelResource::_OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::kind_of<i3LevelRes*>(pInfo->m_pMeta)) //->IsTypeOf( i3LevelRes::static_meta()))
	{
		switch( pInfo->m_Event)
		{
			case	I3_TDK_UPDATE_ADD:
					{
						if( pInfo->m_pObject != NULL)
						{
							m_ResTreeCtrl.AddRes( (i3LevelRes *) pInfo->m_pObject);
						}
						else
						{
							BuildResTree();
						}
					}				
					break;

			case	I3_TDK_UPDATE_REMOVE:
					{
						if( NULL == pInfo || NULL == pInfo->m_pObject )
						{
							break;
						}

						if( pInfo->m_pObject != NULL)
						{
							m_ResTreeCtrl.DeleteRes( (i3LevelRes *) pInfo->m_pObject);
						}
						else
						{
							BuildResTree();
						}
					}
					break;
		}
	}
	else if( i3::kind_of< i3LevelScene *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_SCENE :
				BuildResTree();
				break;

			case I3_TDK_UPDATE_REMOVE :
				I3_SAFE_RELEASE( m_pRoot);
				m_ResTreeCtrl.DeleteAllItems();
				break;
		}
	}
}


LRESULT CPanelResource::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_TDK_UPDATE :
			_OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
			break;

		case WM_RESSCAN_START :
			OnScan_Start();
			break;

		case WM_RESSCAN_CONFIG :
			OnScan_Config( wParam);
			break;

		case WM_RESSCAN_END :
			OnScan_End( (i3::vector<i3::rc_string>*) wParam);
			break;

		case WM_RESSCAN_RES :
			OnScan_Res( (char *) wParam);
			break;
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanelResource::OnResourceCheckdatabase()
{
}

void CPanelResource::OnScan_Start(void)
{
	m_ResTreeCtrl.ShowWindow( SW_HIDE);

	m_ST_Title.ShowWindow( SW_SHOW);
	m_Progress.ShowWindow( SW_SHOW);
	m_ED_File.ShowWindow( SW_SHOW);

	m_ST_Title.SetWindowText( "Scanning files...");
}

void CPanelResource::OnScan_Config( INT32 maxCount)
{
	m_Progress.SetRange32( 0, maxCount);

	m_ST_Title.SetWindowText( "Loading files...");

	m_FileCount = 0;
}

void CPanelResource::OnScan_End( i3::vector<i3::rc_string>* pList)
{
	//i3Level::SaveDB( m_szDB);

	m_ResTreeCtrl.ShowWindow( SW_SHOW);
	m_ST_Title.ShowWindow( SW_HIDE);
	m_Progress.ShowWindow( SW_HIDE);
	m_ED_File.ShowWindow( SW_HIDE);

	BuildResTree();

//	pList->Release();
	i3::delete_by_common_pool( pList );

}

void CPanelResource::OnScan_Res( char * pszPath)
{
	g_pResDB->AddRes( pszPath, NULL);

	m_FileCount++;
	m_Progress.SetPos( m_FileCount);

	m_ED_File.SetWindowText( pszPath);
}

void CPanelResource::OnDestroy()
{
	i3TDKDialogBase::OnDestroy();

	// 어째서인지 소멸자가 호출이 되지 않아 여기다 추가 함
	I3_SAFE_RELEASE( m_pRoot);
	m_ResTreeCtrl.DeleteAllItems();
}
