#include "stdafx.h"
#include "MQContentsStatusTree.h"
#include "resource/MediaFileComparator.h"
#include "resource/Settings.h"
#include <stack>
#include <array>
#include <boost/filesystem.hpp>
#include "UIDefines.h"
#include "MQContentsStatusBar.h"
#include "QLContentsStatusBar.h"
#include "resource/ContentsSynchronizer.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	std::wstring ExtractFileRelPathByItem(i3MFCTreeCtrl* ctrl, HTREEITEM hItem)
	{
		if (hItem == NULL)
			return std::wstring();

		std::stack<CString> itemNameStack;

		HTREEITEM hRootItem = ctrl->GetRootItem();
		HTREEITEM hParentItem = ctrl->GetParentItem(hItem);

		CString wstrItemName = ctrl->GetItemText(hItem);
		itemNameStack.push(wstrItemName);

		CString wstr;
		while (hParentItem != NULL && hParentItem != hRootItem)
		{
			wstr = ctrl->GetItemText(hParentItem);
			itemNameStack.push(wstr);
			hParentItem = ctrl->GetParentItem(hParentItem);
		}

		std::wstring wstrRelPath;
		while (!itemNameStack.empty())
		{
			wstrRelPath += itemNameStack.top();
			wstrRelPath += L"\\";
			itemNameStack.pop();
		}

		if (!wstrRelPath.empty())
			wstrRelPath.erase(wstrRelPath.end() - 1);	// 마지막 L"\\" 제거.

		const size_t pos = wstrRelPath.find(L", ");	// 부가적인 설명(ex. , NOT EXISTS IN LIVE 삭제.
		if (pos != std::wstring::npos)
			wstrRelPath.erase(pos, wstrRelPath.size());

		return wstrRelPath;
	}

	void ExtractFileRelPaths(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, std::vector<PathPtr>& out)
	{
		// 하위 파일 긁어모으기.
		if (ctrl->GetChildItem(hItem))
		{
			for (HTREEITEM hCurrItem = ctrl->GetChildItem(hItem); hCurrItem != NULL; hCurrItem = ctrl->GetNextSiblingItem(hCurrItem))
			{
				if (ctrl->GetChildItem(hCurrItem))
				{
					ExtractFileRelPaths(ctrl, hCurrItem, out);
				}
				else
				{
					std::wstring fileRelPath = ExtractFileRelPathByItem(ctrl, hCurrItem);
					out.push_back(std::make_shared<Path>(fileRelPath));
				}
			}
		}
		else
		{
			std::wstring fileRelPath = ExtractFileRelPathByItem(ctrl, hItem);
			out.push_back(std::make_shared<Path>(fileRelPath));
		}
	}

	void AddContentsToSyncPlayer(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, const rsc::ContentsDataPtr& contents, const rsc::CountryDataPtr& country)
	{
		CString strItemName = ctrl->GetItemText(hItem);

		std::vector<PathPtr> fileRelPathList;
		ExtractFileRelPaths(ctrl, hItem, fileRelPathList);

		if (!fileRelPathList.empty())
		{
			rsc::ContentsSyncDataPtr syncData = std::make_shared<rsc::ContentsSyncData>();

			syncData->type = rsc::SYNCHRONIZER_TYPE_FILES;
			syncData->country = country;
			syncData->contents = contents;

			std::string strContentsName;	i3::utf16_to_mb(strItemName.GetString(), strContentsName);
			syncData->contents->strContentsName = strContentsName;

			syncData->fileRelPathList.assign(fileRelPathList.begin(), fileRelPathList.end());

			GetMainFrame()->GetSyncPlayerBar().GetDlg().AddContents({ syncData });
		}
	}

	struct MQContentsStatusTreeCallback : i3MFCTreeCtrlCallback
	{
		ui::MQContentsStatusTree& owner;

		MQContentsStatusTreeCallback(ui::MQContentsStatusTree& own) : owner(own)
		{}

		virtual ~MQContentsStatusTreeCallback()
		{}

		virtual void OnSelChanged(i3MFCTreeCtrl* Ctrl, HTREEITEM hItem, void* ItemData) 
		{
			if (hItem != NULL)
			{
				std::wstring wstrItemName = Ctrl->GetItemText(hItem).GetString();

				if (wstrItemName.find(L".") == std::wstring::npos)	// 폴더는 추적하지 않는다.
					return;

				size_t pos = wstrItemName.find(L",");
				if (pos != std::wstring::npos)
					wstrItemName.erase(pos, wstrItemName.length());

				std::stack<std::wstring> itemNameStack;

				HTREEITEM hRootItem = Ctrl->GetRootItem();
				HTREEITEM hParentItem = Ctrl->GetParentItem(hItem);

				itemNameStack.push(wstrItemName);
				while (hParentItem != NULL && hParentItem != hRootItem)
				{
					itemNameStack.push(Ctrl->GetItemText(hParentItem).GetString());
					hParentItem = Ctrl->GetParentItem(hParentItem);
				}

				std::wstring wstrRelPath;
				while (!itemNameStack.empty())
				{
					wstrRelPath += itemNameStack.top();
					wstrRelPath += L"\\";
					itemNameStack.pop();
				}

				if (!wstrRelPath.empty())
					wstrRelPath.erase(wstrRelPath.end() - 1);	// 마지막 L"\\" 제거.
				
				static_cast<ui::MQContentsStatusBar*>(owner.m_ctrl.GetOwner())->SelectQLTreeItem(wstrRelPath);
			}
		}

		virtual void OnNMCustomDraw(i3MFCTreeCtrl* ctrl, NMHDR* pNMHDR, LRESULT* pResult)
		{
			LPNMTVCUSTOMDRAW pNMTVCustomDraw = (LPNMTVCUSTOMDRAW)pNMHDR;

			HTREEITEM hItem = (HTREEITEM)pNMTVCustomDraw->nmcd.dwItemSpec;
			*pResult = CDRF_DODEFAULT;

			switch (pNMTVCustomDraw->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				*pResult = CDRF_NOTIFYITEMDRAW;
				break;

			case CDDS_ITEMPREPAINT:
			{
				rsc::ContentsStatusFlag* status = (rsc::ContentsStatusFlag*)ctrl->GetItemData(hItem);
				if (status)
				{
					if ((*status)[rsc::ContentsStatus::SYNC_OK] == false)
						pNMTVCustomDraw->clrText = COLOR_VIOLET_RED;
					else
						pNMTVCustomDraw->clrText = COLOR_GREEN;
				}
			}
			break;
			}
		}

		virtual void OnContextMenu(i3MFCTreeCtrl* ctrl, const CPoint& pt)
		{
			TV_HITTESTINFO hitTestInfo;

			CPoint ptTree;
			ptTree.x = pt.x;
			ptTree.y = pt.y;

			ctrl->ScreenToClient(&ptTree);
			hitTestInfo.pt.x = ptTree.x;
			hitTestInfo.pt.y = ptTree.y;

			ctrl->HitTest(&hitTestInfo);

			HTREEITEM hCurrItem = ctrl->HitTest(&hitTestInfo);

			if ((hitTestInfo.flags & TVHT_ONITEM) && hCurrItem)
			{
				HTREEITEM hPrevItem = ctrl->GetSelectedItem();
				if (hPrevItem != hCurrItem)
					ctrl->Select(hCurrItem, TVGN_CARET);

				CMenu menu;
				menu.CreatePopupMenu();
				menu.AppendMenu(MF_STRING, 1, L"-> SyncPlayer");

				CPoint pt;
				::GetCursorPos(&pt);

				const int menuID = menu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, owner.m_ctrl.GetOwner(), NULL);
				switch (menuID)
				{
				case 1:	// "-> SyncPlayer" 메뉴.
				{
					const auto& contents = static_cast<ui::MQContentsStatusBar*>(owner.m_ctrl.GetOwner())->m_currContentsData;
					const auto& country = static_cast<ui::MQContentsStatusBar*>(owner.m_ctrl.GetOwner())->m_currCountryData;
					AddContentsToSyncPlayer(&owner.m_ctrl, hCurrItem, contents, country);
				}break;
				}
			}
		}

		virtual void OnLButtonDblClk(i3MFCTreeCtrl* ctrl, CPoint pt)
		{
			TV_HITTESTINFO hitTestInfo;

			CPoint ptTree;
			ptTree.x = pt.x;
			ptTree.y = pt.y;

			ctrl->ScreenToClient(&ptTree);
			hitTestInfo.pt.x = ptTree.x;
			hitTestInfo.pt.y = ptTree.y;

			HTREEITEM hCurrItem = ctrl->HitTest(&hitTestInfo);

			if ((hitTestInfo.flags & TVHT_ONITEM) && hCurrItem)
			{
				const auto& contents = static_cast<ui::MQContentsStatusBar*>(owner.m_ctrl.GetOwner())->m_currContentsData;
				const auto& country = static_cast<ui::MQContentsStatusBar*>(owner.m_ctrl.GetOwner())->m_currCountryData;
				AddContentsToSyncPlayer(&owner.m_ctrl, hCurrItem, contents, country);
			}
		}
	};

	struct MQContentsStatusTreeFindCallback : i3MFCTreeCtrl_FindCallback
	{
		ui::MQContentsStatusTree& owner;

		MQContentsStatusTreeFindCallback(ui::MQContentsStatusTree& own) : owner(own)
		{}

		virtual ~MQContentsStatusTreeFindCallback()
		{}

		virtual void OnNewItem(i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel)
		{
			if (hItem == ctrl->GetRootItem())
				return;

			Path itemName(item_name.c_str());
			if (!itemName.has_extension())
				return;

			const rsc::CountryDataPtr countryData = rsc::CountrySettings::i()->FindData(owner.m_strCountryName);

			std::stack<std::wstring> itemNameStack;

			HTREEITEM hRootItem = ctrl->GetRootItem();
			HTREEITEM hParentItem = hParent;

			itemNameStack.push(item_name.c_str());
			while (hParentItem != NULL && hParentItem != hRootItem)
			{
				itemNameStack.push(ctrl->GetItemText(hParentItem).GetString());
				hParentItem = ctrl->GetParentItem(hParentItem);
			}

			std::wstring wstrRelPath;
			while (!itemNameStack.empty())
			{
				wstrRelPath += itemNameStack.top();
				wstrRelPath += L"\\";
				itemNameStack.pop();
			}
			
			if (wstrRelPath.empty())
				return;
				
			wstrRelPath.erase(wstrRelPath.end() - 1);	// 마지막 L"\\" 제거.

			rsc::MediaFileComparator compM(countryData->GetMediaWorkdir());
			compM.AddFileRelPathList({ std::make_shared<Path>(wstrRelPath) });

			rsc::QAFileComparator compQ(countryData->GetQAWorkdir());
			compQ.AddFileRelPathList({ std::make_shared<Path>(wstrRelPath) });

			std::shared_ptr<rsc::ContentsStatusFlag> status = std::make_shared<rsc::ContentsStatusFlag>();
			owner.m_statusList.push_back(status);

			ctrl->SetItemData(hItem, (DWORD_PTR)status.get());

			if (!compM.CheckFiles())
				status->set(rsc::ContentsStatus::NOT_EXISTS_IN_MEDIA);

			if (!compQ.CheckFiles())
				status->set(rsc::ContentsStatus::NOT_EXISTS_IN_QA);

			if (!status->any())
			{
				if (!compQ.IsEqualTo(compM))
					status->set(rsc::ContentsStatus::DIFF_MEDIA_QA);

				if (!status->any())
					status->set(rsc::ContentsStatus::SYNC_OK);
			}

			std::array<std::pair<rsc::ContentsStatus, const wchar_t*>, 3> arrStatusText =
			{
				std::make_pair(rsc::ContentsStatus::NOT_EXISTS_IN_MEDIA,	L", NOT_EXISTS_IN_MEDIA"),
				std::make_pair(rsc::ContentsStatus::NOT_EXISTS_IN_QA,		L", NOT_EXISTS_IN_QA"),
				std::make_pair(rsc::ContentsStatus::DIFF_MEDIA_QA,			L", DIFF")
			};

			CString strItemName = ctrl->GetItemText(hItem);
			bool redrawTextOK = false;

			for (auto &currStatusText : arrStatusText)
			{
				const rsc::ContentsStatus& flag = currStatusText.first;
				if ((*status)[flag])
				{
					const wchar_t* text = currStatusText.second;
					strItemName += text;

					redrawTextOK = true;
				}
			}

			if (redrawTextOK)
				ctrl->SetItemText(hItem, strItemName);
		}
	};
}

namespace ui
{
	MQContentsStatusTree::MQContentsStatusTree(MQContentsStatusBar& owner)
	{
		m_ctrl.CreateEx(&owner, 1);
		m_ctrl.SetCallback(new MQContentsStatusTreeCallback(*this));
		m_ctrl.SetBkColor(COLOR_IVORY_BLACK);
		m_ctrl.SetTextColor(COLOR_WHITE);
	}

	MQContentsStatusTree::~MQContentsStatusTree()
	{
		delete m_ctrl.GetCallback();
	}

	void MQContentsStatusTree::AdjustLayout(const int cx, const int cy)
	{
		RECT rt;
		rt.left = 0;	rt.right = cx;
		rt.top = 0;		rt.bottom = cy;

		m_ctrl.MoveWindow(&rt);
	}

	void MQContentsStatusTree::InsertItems(const std::vector<std::wstring>& wstrPathList, const std::wstring& wstrContentsName, const std::wstring& wstrCountryName)
	{
		ClearItems();

		i3::utf16_to_mb(wstrCountryName, m_strCountryName);

		MQContentsStatusTreeFindCallback cb(*this);
		m_ctrl.FindOrCreateItemByNameList(wstrContentsName, &cb);

		for (auto &wstrCurrPath : wstrPathList)
			m_ctrl.FindOrCreateItemByNameList(wstrCurrPath.c_str(), &cb);

		CTreeCtrlUtil::ExpandTreeAllItems(m_ctrl);

		m_ctrl.SetRedraw(TRUE);
		m_ctrl.RedrawWindow();
	}

	void MQContentsStatusTree::ClearItems()
	{
		m_ctrl.DeleteAllItems();
		m_statusList.clear();
	}

	void MQContentsStatusTree::SelectItem(const std::wstring& wstrPath)
	{
		HTREEITEM hItem = CTreeCtrlUtil::FindTreeItem_Path(m_ctrl, wstrPath);
		if (hItem != NULL)
			m_ctrl.SelectItem(hItem);
	}
}