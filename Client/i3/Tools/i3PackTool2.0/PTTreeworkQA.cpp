#include "stdafx.h"
#include "PTTreeworkQA.h"
#include "PTFile.h"
#include "PTStorage.h"
#include "PTTreeworkMediator.h"
#include "DragDropTreeCtrl.h"
#include "i3PackTool2.0Dlg.h"

PTTreeworkQA::PTTreeworkQA()
{
}

PTTreeworkQA::~PTTreeworkQA()
{
	delete m_storage; m_storage = 0;
}

void PTTreeworkQA::Create(const char* workdir, CDragDropTreeCtrl* treeCtrl)
{
	m_workdir	= workdir;
	m_treeCtrl	= treeCtrl;
	m_storage	= new PTStorage(treeCtrl, workdir, LOAD_WORKDIR_QA);

	treeCtrl->EnsureVisible(treeCtrl->GetRootItem());
	treeCtrl->Expand(treeCtrl->GetRootItem(), TVE_EXPAND);
}

void PTTreeworkQA::Destroy()
{
	if (m_treeCtrl)
	{
		m_treeCtrl->DeleteAllItems();
		m_treeCtrl = 0;
	}
	delete m_storage; m_storage = 0;
}

void PTTreeworkQA::LClicked(NMHDR *nmhdr)
{
}

void PTTreeworkQA::Expand(const PTFile* file)
{
	HTREEITEM item = file->GetTreeItem();
	m_treeCtrl->Select(item, TVGN_FIRSTVISIBLE);
	m_treeCtrl->Expand(item, TVE_EXPAND);
}

void PTTreeworkQA::OpenWorkingFolder(CDragDropTreeCtrl* treeCtrl)
{
	TCHAR workdir[MAX_PATH] = {0,};

	BROWSEINFO bi; ::ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
	bi.lpszTitle = _T("QA폴더를 선택해주세요.");
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&bi);

	if(::SHGetPathFromIDList(pItemIDList, workdir))
	{
		if (::strcmp(m_workdir.c_str(), workdir) == 0) return;

		AfxGetMainWnd()->BeginWaitCursor();
		{
			Destroy();
			Create(workdir, treeCtrl);

			SetState(QS_WORKINGFOLDER_OPENED);
			m_mediator->CollegueChanged(this);
		}
		AfxGetMainWnd()->EndWaitCursor();
	}
}