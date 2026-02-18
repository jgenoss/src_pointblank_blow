// PackScriptLeftView.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptLeftView.h"

#include "LoadPackScriptTree.h"
#include "PackScriptTree.h"

#include "PackScriptRightView.h"
#include "PathMgr.h"

#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/extract_filetitle_fileext.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/itl/range/iterator_range.h"

#include "ExceptionDlg.h"

#include "PackScriptMgr.h"

#include "ExecuteTSVN.h"
#include "PackDiffView.h"
#include "LocaleStringDiffView.h"

// PackScriptLeftView

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PackScriptLeftView, CView)

namespace 
{

	enum 
	{
		COMMAND_ADDCHILD_PACKSCRIPT_DIR = 1,
		COMMAND_DELETE_PACKSCRIPT_DIR = 2,
		COMMAND_TSVN_UPDATE_MEDIA,
		COMMAND_TSVN_UPDATE_QA,
		COMMAND_TSVN_UPDATE_LIVE,
		COMMAND_TSVN_UPDATE_ALL,
	};

	struct TreeCallback : i3MFCTreeCtrlCallback
	{
		TreeCallback(PackScriptLeftView* Owner) : m_Owner(Owner) {}		PackScriptLeftView*	m_Owner;

		virtual void OnSelChanged(i3MFCTreeCtrl* Ctrl, HTREEITEM hItem, void* ItemData)
		{
			PackScriptRightView* pRightView = m_Owner->GetRightView();
			
			pRightView->UpdateListCtrl(reinterpret_cast<PackScriptFolder*>(ItemData) );
		}

		virtual void OnContextMenu(i3MFCTreeCtrl* Ctrl, const CPoint& pt) 
		{
			// 여기서 그냥 구현...
			PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

			if ( root == NULL )		// 팩스크립트 폴더가 없다면 아예 스킵..
				return;

			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);
			HTREEITEM htarget = Ctrl->HitTest(ptClient);
			Ctrl->SelectItem(htarget);

			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING, COMMAND_ADDCHILD_PACKSCRIPT_DIR, L"Add Child");
			menu.AppendMenu(MF_STRING, COMMAND_DELETE_PACKSCRIPT_DIR, L"Delete Folder");

			bool bMediaPath = PathMgr::i()->IsMediaPathExist();
			bool bQAPath = PathMgr::i()->IsQAPathExist();
			bool bLivePath = PathMgr::i()->IsLivePathExist(); 

			bool bSeparationNeed = false;

			if (bMediaPath || bQAPath || bLivePath)
			{
				menu.AppendMenu(MF_SEPARATOR);
			}

			if (bMediaPath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_MEDIA, L"TortoiseSVN Update : Media Path");
				bSeparationNeed = true;
			}
			if (bQAPath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_QA, L"TortoiseSVN Update : QA Path");
				bSeparationNeed = true;
			}
			if (bLivePath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_LIVE, L"TortoiseSVN Update : Live Path");
				bSeparationNeed = true;
			}
			if (bMediaPath && bQAPath && bLivePath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_ALL, L"TortoiseSVN Update : ALL 3 Paths");
				bSeparationNeed = true;
			}


			long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
			menu.DestroyMenu();
						
			switch( result_cmd)
			{
			case COMMAND_ADDCHILD_PACKSCRIPT_DIR:
				{
					HTREEITEM new_item = Ctrl->AddItem(htarget, L"", NULL);		//NULL값 user_data를 토대로 새것인지 아닌지 구분..
					Ctrl->SelectItem(new_item);
					Ctrl->EditLabel(new_item);
				}
			break;
			
			case COMMAND_DELETE_PACKSCRIPT_DIR:
				{
					PackScriptFolder* folder = (PackScriptFolder*) Ctrl->GetItemData(htarget);
					
					if (folder->GetParent() )
					{
						i3::wstring absPath;
						folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), absPath);
						BOOL bRes = ::RemoveDirectoryW(absPath.c_str());

						if (bRes)
						{
							folder->GetParent()->DeleteChild(folder);
							Ctrl->DeleteItem(htarget);
						}
					}
//					// 으음...이 경우에도 일단 테스트삼아 부가적 코드를 다른데서 처리한다..
				}

			case COMMAND_TSVN_UPDATE_MEDIA:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()));
				break;

			case COMMAND_TSVN_UPDATE_QA:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()));
				break;

			case COMMAND_TSVN_UPDATE_LIVE:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()));
				break;

			case COMMAND_TSVN_UPDATE_ALL:
				{
					HANDLE ah[3];
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()), ah[0]);
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()), ah[1]);
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()), ah[2]);
					tsvn::WaitExecuteEnd(ah, 3);
				}
			break;
			default:
			break;

			}

		}
	

		virtual void OnEndLabelEdit(i3MFCTreeCtrl* Ctrl, HTREEITEM hItem, const wchar_t* pszText, bool& apply) 
		{

			apply = false;

			if (pszText == NULL || pszText[0] == 0)	
			{
				
				PackScriptFolder* folder = (PackScriptFolder*)Ctrl->GetItemData(hItem);
				if (folder == NULL)		// 아이템 취소한다..제거하는 함수를 선택..문제가 생기면 지연처리한다.
				{
					Ctrl->DeleteItem(hItem);
				}	
				return;
			}
			else // Rename or CreateDir...
			{
				PackScriptFolder* folder = (PackScriptFolder*)Ctrl->GetItemData(hItem);
				if (folder == NULL) // CreateDir
				{
					HTREEITEM hParent = Ctrl->GetParentItem(hItem);
					PackScriptFolder* parent_folder = (PackScriptFolder*)Ctrl->GetItemData(hParent);
					
					i3::wstring new_dir_path;
					parent_folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), new_dir_path);
					
					if (!new_dir_path.empty() )
						new_dir_path += L'/';

					new_dir_path += pszText;

					BOOL res = CreateDirectoryW( new_dir_path.c_str(), NULL);

					if (res == FALSE)
						Ctrl->DeleteItem(hItem);
					else
					{
						PackScriptFolder* new_folder = new PackScriptFolder(i3::as_literal(pszText));
						parent_folder->AddChild(new_folder);
						Ctrl->SetItemData(hItem, DWORD_PTR(new_folder));

						apply = true;
					}
				}
				else	// Rename ..  폴더 이름 바꾸는 api처리함..
				{
					PackScriptFolder* folder = (PackScriptFolder*)Ctrl->GetItemData(hItem);
					
					if (folder)
					{
						i3::wstring strNew, strOld;

						folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), strOld);

						if ( folder->GetParent() )
						{
							folder->GetParent()->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), strNew);
						}
						else
						{
							strNew = PathMgr::i()->GetPackScriptPath();
						}
						
						strNew += L'/';
						strNew += pszText;
																							
						BOOL res = MoveFileW(strOld.c_str(), strNew.c_str());

						if (res)
							apply = true;
					}
				}
				
			}
		}
		
	};
	
//	bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, wchar_t* outstrPath_MAXPATH);
//	bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, char* outstrPath_MAXPATH);

	struct PackScriptFolder_FindCallback : i3MFCTreeCtrl_FindCallback
	{
		PackScriptLeftView* m_owner;
		PackScriptFolder_FindCallback( PackScriptLeftView* owner) : m_owner(owner) {}
		virtual void OnNewItem( i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) 
		{
			PackScriptFolder* parent_folder = (PackScriptFolder*)ctrl->GetItemData(hParent);

			if (parent_folder->FindChild(item_name))
				return;

			PackScriptFolder* new_folder = new PackScriptFolder(i3::as_literal(item_name));
			parent_folder->AddChild(new_folder);
			ctrl->SetItemData(hItem, (DWORD_PTR) new_folder);
		}

		virtual void OnAlreadyItemCreated( i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) 
		{
			PackScriptFolder* parent_folder = (PackScriptFolder*)ctrl->GetItemData(hParent);

			if (parent_folder->FindChild(item_name))
				return;

			PackScriptFolder* new_folder = new PackScriptFolder(i3::as_literal(item_name));
			parent_folder->AddChild(new_folder);
			ctrl->SetItemData(hItem, (DWORD_PTR) new_folder);
		}

		virtual void OnRenameItem(i3MFCTreeCtrl* ctrl, const i3::wstring& old_name, const i3::wstring& new_name, 
			HTREEITEM hParent, HTREEITEM hItem, int iLevel) 
		{
			PackScriptFolder* folder = (PackScriptFolder*)ctrl->GetItemData(hItem);
			folder->SetFolderName(new_name);
		}

		virtual void OnAlreadyItemRenamed(i3MFCTreeCtrl* ctrl, const i3::wstring& old_name, const i3::wstring& new_name, 
			HTREEITEM hParent, HTREEITEM hItem, int iLevel) 
		{
			PackScriptFolder* folder = (PackScriptFolder*)ctrl->GetItemData(hItem);
			folder->SetFolderName(new_name);
		}

		virtual void BeforeDeleteItem(i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) 
		{
			PackScriptFolder* folder = (PackScriptFolder*)ctrl->GetItemData(hItem);
			
			if ( folder->GetParent() )
			{
				folder->GetParent()->DeleteChild(folder);
			}
		}

	};

	struct PackScriptFolderWatcherCallback : ShellChangeCallback
	{
		PackScriptLeftView* m_owner;
		PackScriptFolderWatcherCallback(PackScriptLeftView* owner) : m_owner(owner) {}

		virtual void OnCreate_Directory( PIDLIST_ABSOLUTE pidl) 
		{
			i3::wstring wstrPath;
			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(pidl, wstr);
				i3::make_relative_path(PathMgr::i()->GetPackScriptPath(), wstr, wstrPath);
			}

			i3MFCTreeCtrl* tree_ctrl = m_owner->GetTreeCtrl();
			PackScriptFolder_FindCallback cb(m_owner);
			
			HTREEITEM item = tree_ctrl->FindOrCreateItemByNameList(wstrPath, &cb);
		}	
		virtual void OnDelete_Directory( PIDLIST_ABSOLUTE pidl) 
		{
			i3::wstring wstrPath;
			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(pidl, wstr);
				i3::make_relative_path(PathMgr::i()->GetPackScriptPath(), wstr, wstrPath);
			}

			i3MFCTreeCtrl* tree_ctrl = m_owner->GetTreeCtrl();
			PackScriptFolder_FindCallback cb(m_owner);
			BOOL bRes = tree_ctrl->DeleteItemByNameList(wstrPath, &cb);

		}	

		virtual void OnRename_Directory( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl ) 
		{
			i3::wstring wstrFromPath;
			i3::wstring wstrTargetName;
			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(from_pidl, wstr);
				i3::make_relative_path( PathMgr::i()->GetPackScriptPath(), wstr, wstrFromPath);
			}

			{
				wchar_t wstr[MAX_PATH];
				GetDisplayNameFromAbsPidl(to_pidl, wstr);
				wstrTargetName = wstr;
			}

			i3MFCTreeCtrl* tree_ctrl = m_owner->GetTreeCtrl();
			PackScriptFolder_FindCallback cb(m_owner);
			HTREEITEM item = tree_ctrl->FindOrRenameItemByNameList(wstrFromPath, wstrTargetName, &cb);
		}	

		virtual void OnCreate_File( PIDLIST_ABSOLUTE pidl) 
		{
			i3::wstring wstrFromPath;	
			i3::wstring wstrFileTitle;
			i3::wstring wstrSourceExt;
			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(pidl, wstr);
				i3::make_relative_path( PathMgr::i()->GetPackScriptPath(), wstr, wstrFromPath);
				i3::extract_filetitle_fileext(wstrFromPath, wstrFileTitle, wstrSourceExt);
			}

			i3::const_wchar_range rng_ext = i3::as_literal(L"i3packscript");
			bool SrcIsPackScript = i3::generic_is_iequal(wstrSourceExt, rng_ext);
			
			if (!SrcIsPackScript)
				return;

			PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

			PackScriptFolder* folder = root->FindFolder( i3::as_literal(wstrFromPath));
			
			if (!folder)
				return;
			
			PackScriptFile* file = folder->FindChildFile(wstrFileTitle);

			if (file)
			{
				m_owner->GetRightView()->Update_OnAddFile( file );	
			}
			else
			{
				PackScriptFile* new_file = new PackScriptFile( i3::as_literal(wstrFileTitle) );
				folder->AddFile( new_file  );
				m_owner->GetRightView()->Update_OnAddFile( new_file );
			}
		}	

		virtual void OnDelete_File( PIDLIST_ABSOLUTE pidl) 
		{
			i3::wstring wstrFromPath;	
			i3::wstring wstrFileTitle;
			i3::wstring wstrSourceExt;
			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(pidl, wstr);
				i3::make_relative_path( PathMgr::i()->GetPackScriptPath(), wstr, wstrFromPath);
				i3::extract_filetitle_fileext(wstrFromPath, wstrFileTitle, wstrSourceExt);
			}

			i3::const_wchar_range rng_ext = i3::as_literal(L"i3packscript");
			bool SrcIsPackScript = i3::generic_is_iequal(wstrSourceExt, rng_ext);

			if (!SrcIsPackScript)
				return;

			PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

			PackScriptFolder* folder = root->FindFolder( i3::as_literal(wstrFromPath));

			if (!folder)
				return;

			PackScriptFile* file = folder->FindChildFile(wstrFileTitle);

			if (file)
			{
				m_owner->GetRightView()->Update_BeforeDelete( file );	
				file->GetParent()->DeleteFile(file);
			}
		}	

		virtual void OnRename_File( PIDLIST_ABSOLUTE from_pidl, PIDLIST_ABSOLUTE to_pidl )  
		{																					// 확장자 변경에 유의해야한다..
						
			i3::wstring wstrFromPath;				
			i3::wstring wstrSourceTitle;	i3::wstring wstrTargetTitle;
			i3::wstring wstrSourceExt;		i3::wstring wstrTargetExt;

			{
				wchar_t wstr[MAX_PATH];
				GetPathFromAbsPidl(from_pidl, wstr);
				i3::make_relative_path( PathMgr::i()->GetPackScriptPath(), wstr, wstrFromPath);
				i3::extract_filetitle_fileext(wstrFromPath, wstrSourceTitle, wstrSourceExt);
			}

			{
				wchar_t wstr[MAX_PATH];
				GetDisplayNameFromAbsPidl(to_pidl, wstr);
				i3::extract_filetitle_fileext(wstr, wstrTargetTitle, wstrTargetExt);
			}
						
			i3::const_wchar_range rng_ext = i3::as_literal(L"i3packscript");
			bool SrcIsPackScript = i3::generic_is_iequal(wstrSourceExt, rng_ext);
			bool TgtIsPackScript = i3::generic_is_iequal(wstrTargetExt, rng_ext);

			if (!SrcIsPackScript && !TgtIsPackScript)
				return;
			
			PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();
			
			if ( SrcIsPackScript)
			{
				PackScriptFile* file = root->FindFile( i3::as_literal(wstrFromPath) );			// 존재해야한다??
				
				if (file == NULL)
					return;

				if ( TgtIsPackScript )
				{
					file->SetFileTitle(wstrTargetTitle);
					m_owner->GetRightView()->Update_OnRename( file);
				}
				else
				{
					m_owner->GetRightView()->Update_BeforeDelete(file);
					file->GetParent()->DeleteFile(file);
				}
			}
			else		// 생성 ( 기 생성을 가정해야함..)
			{
				PackScriptFolder* folder = root->FindFolder( i3::as_literal(wstrFromPath));
				
				if (!folder)
					return;
				
				if ( folder->FindChildFile(wstrTargetTitle) == NULL )
				{
					PackScriptFile* new_file = new PackScriptFile( i3::as_literal(wstrTargetTitle) );
					folder->AddFile( new_file  );
					m_owner->GetRightView()->Update_OnAddFile( new_file );
				}
			}
												
		}	


		virtual void OnUpdate_Directory(PIDLIST_ABSOLUTE pidl) {} 
		virtual void OnUpdate_File( PIDLIST_ABSOLUTE pidl) {}

	};
	
	struct ButtonExceptionCallback : i3MFCButtonCallback
	{
		ButtonExceptionCallback(PackScriptLeftView* owner) : owner(owner) {}	PackScriptLeftView* owner;
		virtual void OnClick(i3MFCButton* ctrl) 
		{
			ExceptionDlg dlg;
			dlg.SetExceptionFileString( PathMgr::i()->GetExceptionString() );
			if (dlg.DoModal() == IDOK)
			{
				PathMgr::i()->SetExceptionString( dlg.GetExceptionFileString() );
			}
		}		
	};
}

PackScriptLeftView::PackScriptLeftView() : m_pRightView(NULL), m_pPackDiffView(NULL) 
{
	
	m_treeCtrl.SetCallback(new TreeCallback(this));
	
	m_button_exception.SetCallback(new ButtonExceptionCallback(this));

	m_watcher.SetCallback( new PackScriptFolderWatcherCallback(this) );
	
}



PackScriptLeftView::~PackScriptLeftView()
{
	delete m_watcher.GetCallback();
	delete m_button_exception.GetCallback();
	delete m_treeCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(PackScriptLeftView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()

	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// PackScriptLeftView drawing

void PackScriptLeftView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// PackScriptLeftView diagnostics

#ifdef _DEBUG
void PackScriptLeftView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackScriptLeftView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackScriptLeftView message handlers

void PackScriptLeftView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	CRect rcButton;
	m_button_exception.GetClientRect(rcButton);

	CRect rcTree;
	rcTree.left = 0;
	rcTree.top  = rcButton.bottom + 10;
	rcTree.right = cx;
	rcTree.bottom = cy;
	//
	CRect rcErase(0, 0, cx, rcButton.bottom + 20);
	CDC* pDC = this->GetDC();

	if (m_button_exception.IsWindowVisible())
		pDC->ExcludeClipRect( rcButton );

	pDC->FillSolidRect( rcErase, RGB(255,255,255));

//	this->Erase(rcErase);
	//
	m_treeCtrl.SetRect(rcTree);
	// TODO: Add your message handler code here
}


namespace
{
	void	AddItem_PackScriptFolder_Recursive(i3MFCTreeCtrl* treectrl, PackScriptFolder* parent, HTREEITEM parentItem)
	{
		const i3::vector<PackScriptFolder*>& child_list =	parent->GetChildList();
		const std::size_t num_child = child_list.size();
		for (std::size_t i = 0 ; i < num_child; ++i )
		{
			PackScriptFolder* child = child_list[i];
			HTREEITEM myItem = treectrl->AddItem( parentItem, child->GetFolderName().c_str(), child); //, child->HasChildFolder() );
			AddItem_PackScriptFolder_Recursive(treectrl, child, myItem);
		}
	}
}

int PackScriptLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_button_exception.Create(L"Exception Dialog", WS_VISIBLE|WS_CHILD, CRect(0,0, 120, 30), this, 1);
	
	m_treeCtrl.CreateEx(this, 1, TVS_EDITLABELS);
	
	PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();

	if (root)
	{
		HTREEITEM root_item = m_treeCtrl.AddRootItem( root->GetFolderName().c_str(), root); //, m_root->HasChildFolder() );

		AddItem_PackScriptFolder_Recursive(&m_treeCtrl, root, root_item);

		m_treeCtrl.Expand( m_treeCtrl.GetRootItem(), TVE_EXPAND);	// 루트는 미리 펼쳐둔다.

		m_watcher.Start( this->GetSafeHwnd(), PathMgr::i()->GetPackScriptPath().c_str() );
	}
	else
	{
		m_button_exception.ShowWindow(SW_HIDE);
	}

	// TODO:  Add your specialized creation code here
	return 0;
}


void	PackScriptLeftView::UpdatePackScriptFolder() 
{

	PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();
	
	if ( this->GetSafeHwnd() )
	{
		m_treeCtrl.DeleteAllItems();
		this->GetRightView()->UpdateListCtrl(NULL);

		if (root != NULL)
		{
			HTREEITEM root_item = m_treeCtrl.AddRootItem( root->GetFolderName().c_str(), root); //, m_root->HasChildFolder() );
			AddItem_PackScriptFolder_Recursive(&m_treeCtrl, root, root_item);
			m_treeCtrl.Expand( m_treeCtrl.GetRootItem(), TVE_EXPAND);	// 루트는 미리 펼쳐둔다.
			m_watcher.Start( this->GetSafeHwnd(), PathMgr::i()->GetPackScriptPath().c_str() );
			
			m_button_exception.ShowWindow(SW_SHOW);
		}
		else
		{
			m_button_exception.ShowWindow(SW_HIDE);
			CRect rc; m_button_exception.GetClientRect(rc);
			CDC* pDC = this->GetDC();
			pDC->FillSolidRect( rc, RGB(255,255,255));
		}

		m_pPackDiffView->UpdatePefItems();
		m_pLocaleStringDiffView->UpdateLocaleTextFiles();
	}
}


void PackScriptLeftView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);
}


BOOL PackScriptLeftView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcClient;
	this->GetClientRect(rcClient);

	CRect rcButton;
	m_button_exception.GetClientRect(rcButton);

	CRect rcTree;
	rcTree.left = 0;
	rcTree.top  = rcButton.bottom + 10;
	rcTree.right = rcClient.Width();
	rcTree.bottom = rcClient.Height();
	//
	CRect rcErase(0, 0, rcClient.Width(), rcButton.bottom + 20);

//	CDC* pDC = this->GetDC();

	if (m_button_exception.IsWindowVisible())
		pDC->ExcludeClipRect( rcButton );

	pDC->FillSolidRect( rcErase, RGB(255,255,255));

	//	this->Erase(rcErase);
	//
	m_treeCtrl.SetRect(rcTree);

	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}
