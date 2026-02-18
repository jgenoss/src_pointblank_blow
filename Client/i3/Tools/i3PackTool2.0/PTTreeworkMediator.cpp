#include "stdafx.h"
#include "PTTreeworkMediator.h"
#include "PTTreeworkQA.h"
#include "PTTreeworkLive.h"
#include "PTFile.h"
#include "PTStorage.h"
#include "DragDropTreeCtrl.h"
#include "resource.h"

PTTreeworkMediator& TheMediator()
{
	static PTTreeworkMediator p;
	return p;
}

PTTreeworkMediator::PTTreeworkMediator() : m_twQA(0), m_twLive(0)
{
}

PTTreeworkMediator::~PTTreeworkMediator() 
{
	delete m_twLive; m_twLive = 0;
	delete m_twQA; m_twQA = 0;
}

void PTTreeworkMediator::CreateCollegues()
{
	// create collegues
	m_twLive = new PTTreeworkLive;
	m_twQA = new PTTreeworkQA;

	// set mediator
	m_twLive->SetMediator(this);
	m_twQA->SetMediator(this);
}

void PTTreeworkMediator::CollegueChanged(PTCollegue* c)
{
	if (c == m_twQA)
	{
		QAChanged();
	}
	else if (c == m_twLive)
	{
		LiveChanged();
	}
}

void PTTreeworkMediator::QAChanged()
{
	switch (m_twQA->GetState())
	{
	case QS_WORKINGFOLDER_OPENED:
		{
			// 폴더 경로 표시.
			i3::string fullpath("[QA] ");
			fullpath += m_twQA->GetWorkDir();
			AfxGetMainWnd()->SetDlgItemText(IDC_EDIT_QA_FOLDERPATH, fullpath.c_str());
		}
		break;

	case QS_LCLICKED:
		break;

	default: break;
	}
}

void PTTreeworkMediator::LiveChanged()
{
	switch (m_twLive->GetState())
	{
	case LS_WORKINGFOLDER_OPENED:
		{
			// 폴더 경로 표시.
			i3::string fullpath("[LIVE] ");
			fullpath += m_twLive->GetWorkDir();
			AfxGetMainWnd()->SetDlgItemText(IDC_EDIT_LIVE_FOLDERPATH, fullpath.c_str());
		}
		break;

	case LS_SELCHANGED:
		ExpandBestfitTreeItem();
		break;

	case LS_RCLICKED:
		break;

	case LS_ITEM_ADDED:
		break;

	case LS_REPACK_MENU_CLICKED:
		break;

	case LS_DELETE_MENU_CLICKED:
		break;

	case LS_EXPORT_MENU_CLICKED:
		break;

	case LS_NEWPACK_MENU_CLICKED:
		break;

	case LS_MAKE_NEW_PACK:
		break;

	default: break;
	}
}

//void PTTreeworkMediator::OnQAClicked()
//{
	//if (m_twLive == 0 || m_twQA == 0) return;

	//CTreeCtrl* qt = m_twQA->m_treeCtrl;
	////CTreeCtrl* lt = m_twLive->m_treeCtrl; 

	//CPoint pt;
	//GetCursorPos(&pt);
	//::ScreenToClient(qt->m_hWnd, &pt);

	//UINT flags = 0;
	//HTREEITEM item = qt->HitTest(pt, &flags);
	//if (item)
	//{
	//	//// Live 트리에 있는 동일한 파일을 선택해서 보여준다.
	//	//PTFile* qaFile = reinterpret_cast<PTFile*>(qt->GetItemData(item)); 

	//	//i3::string path(qaFile->GetFullpath());
	//	//size_t length = ::strlen(m_twQA->GetWorkDir());
	//	//path = path.substr(length+1);

	//	//i3::string target(m_twLive->GetWorkDir());
	//	//target += "\\";
	//	//target += path;

	//	//// 대소문자 구별에 유의.
	//	//PTFile* fileFound = m_twLive->GetStorage()->Find(target.c_str());
	//	//if (fileFound)
	//	//{
	//	//	HTREEITEM item = fileFound->GetTreeitem();
	//	//	lt->Select(item, TVGN_FIRSTVISIBLE);
	//	//	lt->Expand(item, TVE_EXPAND);

	//	//	PTFile* findPackFile = fileFound;
	//	//	while (findPackFile)
	//	//	{
	//	//		//i3::string f(findPackFile->GetFullpath());
	//	//		if (i3::contain_string(findPackFile->GetFullpath(), ".i3Pack") != -1)
	//	//		{
	//	//			lt->SelectItem(findPackFile->GetTreeitem());
	//	//			break;
	//	//		}
	//	//		findPackFile = findPackFile->GetParent();
	//	//	}
	//	//}

	//	// 아이템 체크 토글
	//	/*if ((flags & TVHT_ONITEMSTATEICON) != 0)
	//	{
	//		qt->SelectItem(item);

	//		if (qt->GetCheck(item))
	//			CheckItemAll(qt, item, false);
	//		else
	//			CheckItemAll(qt, item, true);
	//	}*/
	//}
//}

void PTTreeworkMediator::ExpandBestfitTreeItem()
{
	if (m_twQA->GetStorage() == 0) return;

	HTREEITEM liveItemSelected = m_twLive->GetTreeCtrl()->GetSelectedItem();
	PTFile* file = reinterpret_cast<PTFile*>(m_twLive->GetTreeCtrl()->GetItemData(liveItemSelected));
	if (file)
	{
		if (file->IsPackFile())
		{
			HTREEITEM childItem 
				= m_twLive->GetTreeCtrl()->GetNextItem(file->GetTreeItem(), TVGN_CHILD);

			if (childItem)
			{
				PTFile* childFile
					= reinterpret_cast<PTFile*>(m_twLive->GetTreeCtrl()->GetItemData(childItem));

				file = TreeUtil::FindBestFitFolder(childFile);
			}
		}

		// 매칭되는 QA파일을 펼쳐서 보여준다.
		const char* fullpath = file->IsUnpackFile() ? file->GetUnpackPath() : file->GetFullpath();
		PTFile* target = TreeUtil::FindSameFile(fullpath, m_twLive, m_twQA);
		if (target)
			m_twQA->Expand(target);
	}
}