#include "stdafx.h"
#include "i3PackToolTree.h"
#include "UIDefines.h"
#include <boost/format.hpp>
#include <stack>
#include "resource/i3pack.h"
#include "i3PackToolDlg.h"
#include "util/ThreadPool.hpp"
#include "RscTool/FindPefDiff.h"	// i3Pack 파일 검색 함수 사용 위해.
#include <boost/filesystem.hpp>
#include "i3PackNameSettingDlg.h"
#include <boost/format.hpp>
#include "MainFrm.h"
#include "FileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	std::vector<ui::i3PackToolTreeItemPtr> g_itemList;

	rsc::i3pack g_curri3Pack;
	std::string g_curri3PackAbsPath;

	void CheckPackPath(ui::i3PackToolDlg& dlg)
	{
		const std::string& strPackSrcWorkdir = dlg.GetPackSrcWorkdir();
		if (strPackSrcWorkdir.empty())
		{
			::AfxMessageBox(L"팩 파일에 들어갈 원본 파일이 있는 경로(Source Path)를 설정해야 합니다.", MB_OK);

			CFolderPickerDialog pickDlg(L"Select Source Path", 0, NULL, 0);
			if (pickDlg.DoModal() == IDOK)
				dlg.SetBrowseSourcePathText(pickDlg.GetPathName().GetString());
		}

		const std::string& strPackTgtPath = dlg.GetPackTgtPath();
		if (strPackTgtPath.empty())
		{
			::AfxMessageBox(L"팩 파일 저장 경로(Pack Path)를 설정해야 합니다.", MB_OK);

			CFolderPickerDialog pickDlg(L"Select Pack Path", 0, NULL, 0);
			if (pickDlg.DoModal() == IDOK)
				dlg.SetBrowsePackPathText(pickDlg.GetPathName().GetString());
		}
	}

	bool Checki3PackLoad(ui::i3PackToolDlg& dlg, const std::string& strCurri3PackAbsPath)
	{
		if (strCurri3PackAbsPath.empty())
			return false;

		if (strCurri3PackAbsPath != g_curri3PackAbsPath)
		{
			CheckPackPath(dlg);

			if (dlg.GetPackTgtPath().empty())
				return false;

			g_curri3Pack.LoadPack(strCurri3PackAbsPath.c_str());
			g_curri3Pack.SetPackTgtPath(dlg.GetPackTgtPath());
			g_curri3Pack.SetPackSrcWorkdir(dlg.GetPackSrcWorkdir());
			g_curri3PackAbsPath = strCurri3PackAbsPath;
		}

		return true;
	}

	void SavePack(ui::i3PackToolTree& tree)
	{
		const std::string stri3PackName = Path(g_curri3PackAbsPath).filename().string();
		g_curri3Pack.SavePack(stri3PackName.c_str());
		g_curri3Pack.SetChecksumToPack(stri3PackName.c_str());	// 기존에 체크섬 안되어 있는 팩도 체크섬을 넣어버린다.

		tree.Expandi3PackDetailInfoByPath(g_curri3PackAbsPath);	// 팩 저장이 완료되면 offset이 정렬되므로 다시 로드하여 출력한다.

		std::wstring wstri3PackName;
		i3::mb_to_utf16(stri3PackName, wstri3PackName);
		std::wstring wstrMsg = boost::io::str(boost::wformat(L"성공적으로 저장되었습니다. [%s]") % wstri3PackName);
		::AfxMessageBox(wstrMsg.c_str());

		GetMainFrame()->GetPackInnerFileSearchBar().SetChangedState(true);
	}

	void AddFilesToPack(ui::i3PackToolDlg& dlg , ui::i3PackToolTree& tree)
	{
		std::vector<std::wstring> addList;

		CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT);

		const DWORD buffSize = 40960;	// 40K, 경로 하나당 최대 100Byte 가정하면 400개 정도의 파일 수용 가능.

		fileDlg.m_ofn.nMaxFile = buffSize;

		wchar_t* buff = new wchar_t[buffSize];	::memset(buff, 0, sizeof(buff));
		fileDlg.m_ofn.lpstrFile = buff;

		if (fileDlg.DoModal() == IDOK)
		{
			POSITION pos = fileDlg.GetStartPosition();
			while (pos)
			{
				addList.push_back(fileDlg.GetNextPathName(pos).GetString());
			}
		}	

		const std::wstring wstrPackSrcWorkdir = Path(dlg.GetPackSrcWorkdir()).wstring();
		for (const auto& path : addList)
		{
			if (path.find(wstrPackSrcWorkdir) == std::wstring::npos)
			{
				const std::wstring wstrErr =
					boost::io::str(boost::wformat(L"유효하지 않은 경로입니다. %s 에서 파일을 선택하여 주십시오.") % wstrPackSrcWorkdir);

				::AfxMessageBox(wstrErr.c_str());
				delete[] buff;
				return;
			}

			g_curri3Pack.AddFileToPack(Path(path.substr(wstrPackSrcWorkdir.size() + 1)).string().c_str());
		}

		tree.Expandi3PackDetailInfoByNode(g_curri3Pack.GetRootNode(), g_curri3PackAbsPath);	// 실제 팩 저장을 한 상태는 아니므로 추가된 노드의 offset 은 유효하지 않음에 유의. 팩 파일 저장이 이루어져야 유효한 offset 값을 얻을 수 있다.

		delete[] buff;
	}

	bool SearchDirectory(CString search_path, std::vector<std::wstring>& addList)
	{
		CFileFind finder;
		BOOL isAvailable = finder.FindFile(search_path + _T("\\*.*"));
		if (TRUE != isAvailable)
			return false;

		CString fileName;

		while (isAvailable)
		{
			isAvailable = finder.FindNextFile();

			if (TRUE == finder.IsDots())
				continue;

			// 폴더
			if (TRUE == finder.IsDirectory())			
			{
				SearchDirectory(finder.GetFilePath(), addList); // 재귀
			}
			// 파일
			else if (TRUE == finder.IsArchived())									
			{
				addList.push_back(finder.GetFilePath().GetString());
			}					

		}

		return true;
	}

	void AddFoldersToPack(ui::i3PackToolDlg& dlg, ui::i3PackToolTree& tree)
	{
		std::vector<std::wstring> addList;

		const std::string& strPackSrcWorkdir = dlg.GetPackSrcWorkdir();
		if (strPackSrcWorkdir.empty())
		{
			::AfxMessageBox(L"팩 파일에 들어갈 원본 파일이 있는 경로(Source Path)를 설정해야 합니다.", MB_OK);
			return;
		}

		CFolderPickerDialog pickDlg(L"Select Source Path", 0, NULL, OFN_ALLOWMULTISELECT);
		if (IDOK != pickDlg.DoModal())
			return;

		if (true != SearchDirectory(pickDlg.GetPathName().GetString(), addList))
		{
			::AfxMessageBox(L"디렉토리 추가에 실패하였습니다. 유효하지 않은 경로입니다.", MB_OK);
			return;
		}

		for (const auto& path : addList)
		{
			g_curri3Pack.AddFileToPack(Path(path.substr(strPackSrcWorkdir.size() + 1)).string().c_str(), true);
		}

		tree.Expandi3PackDetailInfoByNode(g_curri3Pack.GetRootNode(), g_curri3PackAbsPath);

	}
	

	void DeleteFilesInPack(i3MFCTreeCtrl* treeCtrl, HTREEITEM hItem)
	{
		std::string strFileRelPath = CTreeCtrlUtil::ExtractFileRelPathByItem(treeCtrl, hItem);
		const size_t pos = strFileRelPath.find("   [enc:");
		if (pos != std::string::npos)
			strFileRelPath = strFileRelPath.substr(0, pos);

		g_curri3Pack.RemoveFileFromPack(strFileRelPath.c_str());

		treeCtrl->DeleteItem(hItem);
		treeCtrl->SetRedraw(TRUE);
		treeCtrl->RedrawWindow();
	}

	struct i3PackToolTreeCallback : i3MFCTreeCtrlCallback
	{
		ui::i3PackToolDlg& dlg;
		ui::i3PackToolTree& tree;
		i3PackToolTreeCallback(ui::i3PackToolDlg& d, ui::i3PackToolTree& t) : dlg(d), tree(t) {}

		virtual void OnContextMenu(i3MFCTreeCtrl* ctrl, const CPoint& pt) override
		{
			TV_HITTESTINFO hitInfo;

			CPoint ptTree;
			ptTree.x = pt.x;
			ptTree.y = pt.y;

			ctrl->ScreenToClient(&ptTree);
			hitInfo.pt.x = ptTree.x;
			hitInfo.pt.y = ptTree.y;

			ctrl->HitTest(&hitInfo);

			HTREEITEM hCurrItem = ctrl->HitTest(&hitInfo);
			if ((hitInfo.flags & TVHT_ONITEM) && hCurrItem)
			{
				HTREEITEM hPrevItem = ctrl->GetSelectedItem();
				if (hPrevItem != hCurrItem)
					ctrl->Select(hCurrItem, TVGN_CARET);

				CMenu menu;
				menu.CreatePopupMenu();

				std::wstring wstrItemText(ctrl->GetItemText(hCurrItem));
				if (wstrItemText.find(L".i3Pack") != std::wstring::npos)
				{
					menu.AppendMenu(MF_STRING, 1, L"Save Pack");
					menu.AppendMenu(MF_STRING, 2, L"Add Files");
					menu.AppendMenu(MF_STRING, 3, L"Add Folder");
				}
				else
				{
					menu.AppendMenu(MF_STRING, 4, L"Delete");
					menu.AppendMenu(MF_STRING, 2, L"Add Files");
					menu.AppendMenu(MF_STRING, 3, L"Add Folder");
					menu.AppendMenu(MF_STRING, 5, L"Encrypt/Decrypt");
				}

				CPoint pt;	::GetCursorPos(&pt);

				const int menuID = menu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, tree.m_ctrl.GetOwner(), NULL);
				
				if (g_curri3PackAbsPath != tree.m_stri3PackAbsPath)
				{
					CheckPackPath(dlg);

					g_curri3Pack.LoadPack(tree.m_stri3PackAbsPath.c_str());
					g_curri3Pack.SetPackTgtPath(dlg.GetPackTgtPath());
					g_curri3Pack.SetPackSrcWorkdir(dlg.GetPackSrcWorkdir());
					g_curri3PackAbsPath = tree.m_stri3PackAbsPath;
				}

				switch (menuID)
				{
				case 1:	// "Save Pack"
				{
					SavePack(tree);
					GetMainFrame()->GetFileView().GetListCtrl()->Refresh();
				}break;

				case 2:	// "Add Files"
				{
					AddFilesToPack(dlg, tree);
				}break;

				case 3:	// "Add Folder"
				{
					AddFoldersToPack(dlg, tree);
				}break;

				case 4:	// "Delete"
				{
					DeleteFilesInPack(&tree.GetCtrl(), hCurrItem);
				}break;

				case 5:	// "Encrypt/Decrypt"
				{
					std::string strFileRelPath = CTreeCtrlUtil::ExtractFileRelPathByItem(&tree.GetCtrl(), hCurrItem);
					
					size_t pos = strFileRelPath.find("   [enc:");
					if (pos != std::string::npos)
						strFileRelPath = strFileRelPath.substr(0, pos);

					std::wstring wstrFileRelPath;
					i3::mb_to_utf16(strFileRelPath, wstrFileRelPath);

					auto it = std::find_if(g_itemList.begin(), g_itemList.end(),
						[&wstrFileRelPath](const ui::i3PackToolTreeItemPtr& src)
					{
						return i3::generic_is_iequal(Path(src->strFileRelPath).make_preferred().wstring(), wstrFileRelPath);
					}
					);

					if (it != g_itemList.end())
					{
						std::wstring wstrItemName(ctrl->GetItemText(hCurrItem).GetString());

						std::wstring wstrEncTextInItemName(L"   [enc: O");
						std::wstring wstrDecTextInItemName(L"   [enc: X");

						size_t pos = 0;
						if ((*it)->isENC == true)
						{
							pos = wstrItemName.find(wstrEncTextInItemName);
							wstrItemName.replace(pos, wstrEncTextInItemName.size(), wstrDecTextInItemName);
						}
						else
						{
							pos = wstrItemName.find(wstrDecTextInItemName);
							wstrItemName.replace(pos, wstrDecTextInItemName.size(), wstrEncTextInItemName);
						}

						ctrl->SetItemText(hCurrItem, wstrItemName.c_str());
						ctrl->SetRedraw(TRUE);
						ctrl->RedrawWindow();

						// 실제 데이터 변경.
						(*it)->isENC = !(*it)->isENC;
						g_curri3Pack.EncryptFileInPack(strFileRelPath.c_str(), (*it)->isENC);
					}
				}break;
				}
			}
		}
	};

	struct i3PackToolTreeFindCallback : i3MFCTreeCtrl_FindCallback
	{
		ui::i3PackToolTree& tree;

		i3PackToolTreeFindCallback(ui::i3PackToolTree& t) : tree(t) {}
		virtual ~i3PackToolTreeFindCallback() {}

		virtual void OnNewItem(i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel)
		{
			if (item_name.empty())
				return;
			
			if (item_name.find(L".i3Pack") != std::wstring::npos)
			{
				std::vector<UINT32> checksumCodes;
				rsc::i3packUtil::GetChecksumCode(tree.m_stri3PackAbsPath.c_str(), checksumCodes);

				if (!checksumCodes.empty())
				{
					const std::wstring wstrItemText =
						boost::io::str(boost::wformat(L"%s   [Checksum: %08X %08X %08X %08X]") % item_name.c_str() % checksumCodes[0] % checksumCodes[1] % checksumCodes[2] % checksumCodes[3]);

					ctrl->SetItemText(hItem, wstrItemText.c_str());
				}
				return;
			}

			std::stack<std::wstring> stackItemNames;

			HTREEITEM hRootItem = ctrl->GetRootItem();
			HTREEITEM hParentItem = hParent;

			stackItemNames.push(item_name.c_str());

			CString wstr;
			while (hParentItem != nullptr && hParentItem != hRootItem)
			{
				wstr = ctrl->GetItemText(hParentItem);
				stackItemNames.push(wstr.GetString());
				hParentItem = ctrl->GetParentItem(hParentItem);
			}

			std::wstring wstrFileRelPath;
			while (!stackItemNames.empty())
			{
				wstrFileRelPath += stackItemNames.top();
				wstrFileRelPath += L"\\";
				
				stackItemNames.pop();
			}

			if (wstrFileRelPath.empty())
				return;

			wstrFileRelPath.erase(wstrFileRelPath.end() - 1);	// 마지막 L"\\" 제거.

			auto it = std::find_if(g_itemList.begin(), g_itemList.end(),
				[&wstrFileRelPath](const ui::i3PackToolTreeItemPtr& src)
				{
					return i3::generic_is_iequal(Path(src->strFileRelPath).make_preferred().wstring(), wstrFileRelPath);
				}
			);

			if (it != g_itemList.end())
			{
				const size_t size = (*it)->size;
				const size_t offset = (*it)->offset;
				const wchar_t* pwszIsENC = (*it)->isENC ? L"O" : L"X";
				if (size != -1 && offset != -1)
				{
					const std::wstring wstrItemText =
						boost::io::str(boost::wformat(L"%s   [enc: %s, size: %d, offset: %d]") % item_name.c_str() % pwszIsENC % size % offset);

					ctrl->SetItemText(hItem, wstrItemText.c_str());
				}
			}
		}
	};

	void GatherInPackFileNode(i3PackNode* n, const i3::string& curr_dir_path, std::vector<ui::i3PackToolTreeItemPtr>& inout_list)
	{
		const INT32 num_file = n->getFileCount();

		for (INT32 i = 0; i < num_file; ++i)
		{
			I3_PACK_FILE_REF* info = n->getFile(i);

			auto itemPtr = std::make_shared<ui::i3PackToolTreeItem>();

			itemPtr->strFileRelPath = curr_dir_path.c_str();
			itemPtr->strFileRelPath += '/';
			itemPtr->strFileRelPath += info->m_szName;

			itemPtr->offset = info->m_Offset;
			itemPtr->size = info->m_Size;
			itemPtr->isENC = (info->m_Attr & I3_PACK_FILE_ATTR_SECURE) != 0;

			inout_list.push_back(itemPtr);
		}

		const INT32 num_child = n->getChildCount();

		for (INT32 i = 0; i < num_child; ++i)
		{
			i3PackNode* child = static_cast<i3PackNode*>(n->getChild(i));
			i3::string child_dir_path;

			if (!curr_dir_path.empty())
			{
				child_dir_path = curr_dir_path;
				child_dir_path += '/';
			}

			const i3::rc_string& child_name = child->GetNameString();
			child_dir_path.append(child_name.begin(), child_name.end());

			GatherInPackFileNode(child, child_dir_path, inout_list);
		}
	}
}

namespace ui
{
	i3PackToolTree::i3PackToolTree(i3PackToolDlg& dlg) : m_dlg(dlg), m_thpool(std::make_unique<ThreadPool>(1))
	{
		m_ctrl.CreateEx(&dlg, 1);
		m_ctrl.SetCallback(new i3PackToolTreeCallback(dlg, *this));
		m_ctrl.SetBkColor(COLOR_IVORY_BLACK);
		m_ctrl.SetTextColor(COLOR_WHITE);
	}

	i3PackToolTree::~i3PackToolTree()
	{
		delete m_ctrl.GetCallback();
	}

	void i3PackToolTree::AdjustLayout(const RECT& rt)
	{
		if (m_ctrl)
			m_ctrl.MoveWindow(&rt);
	}

	void i3PackToolTree::Expandi3PackDetailInfoByPath(const std::string& stri3PackAbsPath)
	{
		m_thpool->ClearTasks();	// 큐에 쌓아놓지 않고 클릭한건 바로바로 처리.

		m_stri3PackAbsPath = stri3PackAbsPath;

		// i3Pack 파일 내부 파일들을 트리로 시각화 하는 과정는 시간비용이 많이 든다. 매끄러운 메인 UI 처리를 위해 이 작업은 스레드로 처리한다.
		struct Task_Showi3PackDetailInfo
		{
			void operator()(const std::string& stri3PackAbsPath, i3MFCTreeCtrl* ctrl, i3PackToolTree* owner)
			{
				i3PackVolume* vol = LoadPackVolume(i3::string(stri3PackAbsPath.c_str()));
				if (vol == nullptr)
					return;

				if (vol->getRoot() == nullptr)
					return;

				g_itemList.clear();
				GatherInPackFileNode(vol->getRoot(), "", g_itemList);

				ctrl->DeleteAllItems();

				i3PackToolTreeFindCallback cb(*owner);

				ctrl->FindOrCreateItemByNameList(Path(stri3PackAbsPath).filename().wstring().c_str(), &cb);

				std::wstring wstr;
				for (const auto& item : g_itemList)
				{
					if (item->strFileRelPath.empty())
						continue;

					i3::mb_to_utf16(item->strFileRelPath, wstr);
					ctrl->FindOrCreateItemByNameList(wstr.c_str(), &cb);
				}

				CTreeCtrlUtil::ExpandTreeAllItems(*ctrl);

				ctrl->SetRedraw(TRUE);
				ctrl->RedrawWindow();

				vol->Release();
			}
		};
		m_thpool->Enqueue(Task_Showi3PackDetailInfo(), stri3PackAbsPath, &m_ctrl, this);
	}

	void i3PackToolTree::Expandi3PackDetailInfoByNode(const i3PackNode* rootNode, const std::string& stri3PackAbsPath)
	{
		m_thpool->ClearTasks();

		m_stri3PackAbsPath = stri3PackAbsPath;

		struct Task_Showi3PackDetailInfo
		{
			void operator()(const i3PackNode* rootNode, const std::string& stri3PackAbsPath, i3MFCTreeCtrl* ctrl, i3PackToolTree* owner)
			{
				if (rootNode == nullptr)
					return;

				std::vector<i3PackToolTreeItemPtr> itemList;
				GatherInPackFileNode(const_cast<i3PackNode*>(rootNode), "", itemList);

				ctrl->DeleteAllItems();

				i3PackToolTreeFindCallback cb(*owner);

				ctrl->FindOrCreateItemByNameList(Path(stri3PackAbsPath).filename().wstring().c_str(), &cb);

				std::wstring wstr;
				for (const auto& item : itemList)
				{
					if (item->strFileRelPath.empty())
						continue;

					i3::mb_to_utf16(item->strFileRelPath, wstr);
					ctrl->FindOrCreateItemByNameList(wstr.c_str(), &cb);
				}

				CTreeCtrlUtil::ExpandTreeAllItems(*ctrl);

				ctrl->SetRedraw(TRUE);
				ctrl->RedrawWindow();
			}
		};
		m_thpool->Enqueue(Task_Showi3PackDetailInfo(), rootNode, stri3PackAbsPath, &m_ctrl, this);
	}

	void i3PackToolTree::ShutdownThreads()
	{
		m_thpool->ClearTasks();
		m_thpool->Shutdown();
	}

	void i3PackToolTree::AddFilesToCurrPack(const std::vector<std::string>& fileAbsPathList)
	{
		if (!Checki3PackLoad(m_dlg, m_stri3PackAbsPath))
			return;

		if (g_curri3PackAbsPath.empty())
			return;

		std::string strPackSrcWorkdir = m_dlg.GetPackSrcWorkdir();
		strPackSrcWorkdir += '\\';

		std::wstring wstrPackSrcWorkdir;
		i3::mb_to_utf16(strPackSrcWorkdir, wstrPackSrcWorkdir);

		// 상대경로 검출.
		size_t pos = 0;
		std::vector<std::string> fileRelPathList;
		for (const auto& path : fileAbsPathList)
		{
			if ((pos = path.find(strPackSrcWorkdir)) == std::string::npos)
			{
				const std::wstring wstrErr =
					boost::io::str(boost::wformat(L"유효하지 않은 경로입니다. %s 에서 파일을 선택하여 주십시오.") % wstrPackSrcWorkdir);

				::AfxMessageBox(wstrErr.c_str());
				return;
			}

			fileRelPathList.push_back(path.substr(pos + strPackSrcWorkdir.size()));
		}

		// 삽입.
		for (const auto& path : fileRelPathList)
		{
			g_curri3Pack.AddFileToPack(path.c_str());
		}

		Expandi3PackDetailInfoByNode(g_curri3Pack.GetRootNode(), g_curri3PackAbsPath);
	}

	void i3PackToolTree::OnNewPack()
	{
		if (!Checki3PackLoad(m_dlg, m_stri3PackAbsPath))
			return;

		i3PackNameSettingDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.GetNewi3PackName().empty())
				return;

			std::string stri3PackName = dlg.GetNewi3PackName();
			size_t pos = 0;
			if ((pos = stri3PackName.find('.')) != std::string::npos)
				stri3PackName = stri3PackName.substr(0, pos);

			stri3PackName += ".i3Pack";

			const std::string& strPackTgtPath = m_dlg.GetPackTgtPath();
			const std::string& strPackSrcWorkdir = m_dlg.GetPackSrcWorkdir();

			rsc::i3pack newPack(strPackSrcWorkdir.c_str(), strPackTgtPath.c_str());
			g_curri3Pack.CopyFrom(newPack);

			std::string stri3PackAbsPath(strPackTgtPath);
			stri3PackAbsPath += '\\';
			stri3PackAbsPath += stri3PackName;

			Expandi3PackDetailInfoByNode(g_curri3Pack.GetRootNode(), stri3PackAbsPath);
		}
	}

	void i3PackToolTree::OnSavePack()
	{
		if (!Checki3PackLoad(m_dlg, m_stri3PackAbsPath))
			return;

		SavePack(*this);
		GetMainFrame()->GetFileView().GetListCtrl()->Refresh();
	}

	void i3PackToolTree::OnAddFilesToPack()
	{
		if (!Checki3PackLoad(m_dlg, m_stri3PackAbsPath))
			return;

		AddFilesToPack(m_dlg, *this);
	}

	void i3PackToolTree::OnAddFolderFilesToPack()
	{
		if (!Checki3PackLoad(m_dlg, m_stri3PackAbsPath))
			return;

		AddFoldersToPack(m_dlg, *this);

	}
}