#include "stdafx.h"
#include "PTTreeworkLive.h"
#include "PTFile.h"
#include "PTStorage.h"
#include "PTTreeworkMediator.h"
#include "DragDropTreeCtrl.h"
#include "resource.h"
#include "PTPacking.h"
#include "NewPackDlg.h"
#include "i3PackTool2.0Dlg.h"
#include "../../include/i3Base/string/ext/make_relative_path.h"

PTTreeworkLive::PTTreeworkLive()
{
}

PTTreeworkLive::~PTTreeworkLive() 
{
	delete m_storage; m_storage = 0;
}

void PTTreeworkLive::Create(const char* workdir, CDragDropTreeCtrl* treeCtrl)
{
	m_workdir	= workdir;
	m_treeCtrl	= treeCtrl;
	m_storage	= new PTStorage(treeCtrl, workdir, LOAD_WORKDIR_LIVE);

	// 트리뷰의 Pack 폴더를 강제로 Expand(확장)
	treeCtrl->EnsureVisible(treeCtrl->GetRootItem());
	treeCtrl->Expand(treeCtrl->GetRootItem(), TVE_EXPAND);

	PTFile* packFolder = TreeUtil::FindPackFolder(this);
	if (packFolder)
	{
		treeCtrl->Expand(packFolder->GetTreeItem(), TVE_EXPAND);
	}

	//TreeUtil::SortTreeItem(treeCtrl, treeCtrl->GetRootItem());
}

void PTTreeworkLive::Destroy()
{
	if (m_treeCtrl)
	{
		m_treeCtrl->DeleteAllItems();
		m_treeCtrl = 0;
	}
	delete m_storage; m_storage = 0;
}

void PTTreeworkLive::OpenWorkingFolder(CDragDropTreeCtrl* treeCtrl)
{
	TCHAR workdir[MAX_PATH] = {0,};

	BROWSEINFO bi; ::ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
	bi.lpszTitle = _T("LIVE폴더를 선택해주세요.");
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&bi);

	if(::SHGetPathFromIDList(pItemIDList, workdir))
	{
		if (::strcmp(m_workdir.c_str(), workdir) == 0) return;

		AfxGetMainWnd()->BeginWaitCursor();
		{
			Destroy();
			Create(workdir, treeCtrl);

			SetState(LS_WORKINGFOLDER_OPENED);
			m_mediator->CollegueChanged(this);
		}
		AfxGetMainWnd()->EndWaitCursor();
	}
}

void PTTreeworkLive::Selchanged(NMHDR* nmhdr)
{
	NM_TREEVIEW* nmtv = (NM_TREEVIEW*)nmhdr;

	HTREEITEM item = nmtv->itemNew.hItem;
	if (item)
	{
		m_treeCtrl->Select(item, TVGN_DROPHILITE);

		SetState(LS_SELCHANGED);
		m_mediator->CollegueChanged(this);
	}
}

void PTTreeworkLive::RClicked(NMHDR* nmhdr)
{
	CPoint ptInTree, ptInSrceen;

	GetCursorPos(&ptInSrceen);
	ptInTree = ptInSrceen;

	ScreenToClient(m_treeCtrl->m_hWnd, &ptInTree);

	HTREEITEM hit = m_treeCtrl->HitTest(ptInTree);    
	m_treeCtrl->SelectItem(hit);

	CMenu menu, *pMenu;
	menu.LoadMenuA(IDR_MENU2);

	if (hit)  
	{
		pMenu = menu.GetSubMenu(0);

		PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(hit));
		if (file->IsUnpackFile()) // i3Pack은 삭제 대상 아님.
		{
			pMenu->EnableMenuItem(ID_NEW_PACK, MF_ENABLED);
			pMenu->EnableMenuItem(ID_DELETE, MF_ENABLED);
			pMenu->EnableMenuItem(ID_PACKING, MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem(ID_EXPORT, MF_DISABLED | MF_GRAYED);
		}
		else if (file->IsPackFile())
		{
			pMenu->EnableMenuItem(ID_NEW_PACK, MF_ENABLED);
			pMenu->EnableMenuItem(ID_PACKING, MF_ENABLED);
			pMenu->EnableMenuItem(ID_DELETE, MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem(ID_EXPORT, MF_ENABLED);
		}
		else
		{
			pMenu->EnableMenuItem(ID_NEW_PACK, MF_ENABLED);
			pMenu->EnableMenuItem(ID_PACKING, MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem(ID_EXPORT, MF_DISABLED | MF_GRAYED);
			pMenu->EnableMenuItem(ID_DELETE, MF_DISABLED | MF_GRAYED);
		}
	}
	else
		pMenu = menu.GetSubMenu(1); //둘째메뉴

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
		ptInSrceen.x, ptInSrceen.y, static_cast<CWnd*>(m_treeCtrl), NULL);

	SetState(LS_RCLICKED);
	m_mediator->CollegueChanged(this);
}

HTREEITEM PTTreeworkLive::AddItem(HTREEITEM itemParent, PTFile* fileSelected, HTREEITEM hInsAfter, int iSelImage, int iNonSelImage, long lParam)
{
	HTREEITEM ret = 0;

	if (fileSelected == 0) return 0;

	// 경로 기반 작업.
	i3::string packfilePath;
	HTREEITEM item = itemParent;
	while (item)
	{
		PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(item));
		if (file)
		{
			if (file->IsPackFile())
			{
				packfilePath = "\\Pack\\";
				packfilePath += file->GetFileName();
				break;
			}
		}
		item = m_treeCtrl->GetNextItem(item, TVGN_PARENT);
	}

	i3::string fullpath(GetWorkDir());
	if (::strcmp(packfilePath.c_str(), "") != 0)
		fullpath += packfilePath;
	fullpath += TreeUtil::PathExceptWorkdir(fileSelected->GetFullpath(), fileSelected->GetTopLevelFolderPath());

	// 아이템 삽입.
	char relativePath[MAX_PATH] = {0,};
	i3::make_relative_path(GetWorkDir(), fullpath, relativePath);

	i3::vector<i3::rc_string> token;
	i3::resolve_path_token(relativePath, token);

	PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(itemParent));
	PTFile* fparent = 0; 
	if (file)
	{
		i3::string fullpath(GetWorkDir());

		for (size_t i=0; i<token.size(); i++)
		{
			fullpath += "\\";
			fullpath += token[i].c_str();

			file = m_storage->Find(fullpath.c_str());
			if (file) fparent = file;

			if (file == 0)
				fparent = m_storage->Add(fullpath.c_str(), fparent);
		}
	}

	return ret;
}

void PTTreeworkLive::RePackMenuClicked()
{
	HTREEITEM item = m_treeCtrl->GetSelectedItem();
	PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(item));
	if (file)
	{
		PackWork::Repack(file);

		SetState(LS_REPACK_MENU_CLICKED);
		m_mediator->CollegueChanged(this);
	}
}

void PTTreeworkLive::DeleteMenuClicked()
{
	HTREEITEM item = m_treeCtrl->GetSelectedItem();
	NodeTraverDel(item);
	m_treeCtrl->DeleteItem(item); // 트리 아이템 삭제(하위 아이템까지 자동 삭제됨)

	SetState(LS_DELETE_MENU_CLICKED);
	m_mediator->CollegueChanged(this);
}

void PTTreeworkLive::NodeTraverDel(HTREEITEM item)
{
	if (item)
	{
		PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(item));
		if (file)
			m_storage->Delete(file->GetFullpath());
	}

	while (item)
	{
		if (m_treeCtrl->ItemHasChildren(item))
			NodeTraverDel(m_treeCtrl->GetNextItem(item, TVGN_CHILD));

		item = m_treeCtrl->GetNextItem(item, TVGN_NEXT);
	}
}

void PTTreeworkLive::ExportMenuClicked()
{
	TCHAR path[MAX_PATH] = {0,};

	BROWSEINFO bi; ::ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_treeCtrl->m_hWnd;
	bi.lpszTitle = _T("폴더를 선택해주세요.");
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&bi);

	if(::SHGetPathFromIDList(pItemIDList, path))
	{
		AfxGetMainWnd()->BeginWaitCursor();
		{
			HTREEITEM item = m_treeCtrl->GetSelectedItem();
			if (item)
			{
				PTFile* file = reinterpret_cast<PTFile*>(m_treeCtrl->GetItemData(item));
				PackWork::Export(file , path);
			}
		}
		AfxGetMainWnd()->EndWaitCursor();
	}

	SetState(LS_EXPORT_MENU_CLICKED);
	m_mediator->CollegueChanged(this);
}

void PTTreeworkLive::NewPackMenuClicked()
{
	CNewPackDlg dlg;
	dlg.DoModal();

	SetState(LS_NEWPACK_MENU_CLICKED);
	m_mediator->CollegueChanged(this);
}

void PTTreeworkLive::MakeNewPack(const char* name)
{
	i3PackNode* packnode = i3PackNode::new_object();
	packnode->SetName(name);

	PTFile* packfolder = TreeUtil::FindPackFolder(this);
	if (packfolder)
	{
		i3::string fullpath(packfolder->GetFullpath());
		fullpath += "\\";
		fullpath += name;

		PTFile* file = m_storage->Add(fullpath.c_str(), packfolder);
		if (file)
		{
			m_treeCtrl->SelectItem(file->GetTreeItem());

			SetState(LS_MAKE_NEW_PACK);
			m_mediator->CollegueChanged(this);
		}
	}
}