#include "stdafx.h"
#include "resource.h"
#include "DragDropTreeCtrl.h"
#include "PTTreeworkBase.h"
#include "PTTreeworkMediator.h"
#include "PTFile.h"
#include "PTStorage.h"
#include "../../include/i3Base/string/ext/contain_string.h"

namespace TreeUtil
{
	PTFile* FindPackFolder(PTTreeworkBase* twbase)
	{
		i3::string path(twbase->GetWorkDir());
		path += "\\Pack";

		return twbase->GetStorage()->Find(path.c_str());
	}

	void CheckItemAll(CDragDropTreeCtrl* tree, HTREEITEM item, bool ok)
	{
		HTREEITEM child = tree->GetChildItem(item);

		while (child != NULL)
		{
			tree->SetCheck(child, ok);

			if(tree->ItemHasChildren(child))
				CheckItemAll(tree, child, ok);

			child = tree->GetNextItem(child, TVGN_NEXT);
		}
	}

	i3::string PathExceptWorkdir(const char* fullpath, const char* workdir)
	{
		i3::string path(fullpath);
		size_t length = ::strlen(workdir);
		return path = path.substr(::strlen(workdir));
	}

	PTFile* FindSameFile(const char* file, PTTreeworkBase* from, PTTreeworkBase* to)
	{
		i3::string targetFullpath(to->GetWorkDir());
		targetFullpath += PathExceptWorkdir(file, from->GetWorkDir());

		return to->GetStorage()->Find(targetFullpath.c_str());
	}

	PTFile* FindBestFitFolder(PTFile* packfile)
	{
		PTFile* file = packfile;
		while (file)
		{
			CTreeCtrl* treeCtrl = file->GetTreeCtrl();

			if (i3::contain_string(file->GetFileName(), ".") != -1) 
			{	
				HTREEITEM item = treeCtrl->GetNextItem(file->GetTreeItem(), TVGN_PARENT);	
				return reinterpret_cast<PTFile*>(treeCtrl->GetItemData(item));
			}

			HTREEITEM childItem = treeCtrl->GetNextItem(file->GetTreeItem(), TVGN_CHILD);
			file = reinterpret_cast<PTFile*>(treeCtrl->GetItemData(childItem));
		}

		return 0;
	}

	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CDragDropTreeCtrl* treeCtrl = (CDragDropTreeCtrl*)lParamSort;

		PTFile* fItem1 = (PTFile*)(CDragDropTreeCtrl*)treeCtrl->GetItemData((HTREEITEM)lParam1);
		PTFile* fItem2 = (PTFile*)(CDragDropTreeCtrl*)treeCtrl->GetItemData((HTREEITEM)lParam2);

		if (fItem1->IsFolder() && fItem2->IsFolder() == false)
		{
			return 1;
		}
		else if (fItem1->IsFolder() == false && fItem2->IsFolder())
		{
			return 0;
		}

		return strcmp(fItem1->GetFullpath(), fItem2->GetFullpath());

	}

	void SortTreeItem(CDragDropTreeCtrl* treeCtrl, HTREEITEM item)
	{
		if(item != NULL)
		{
			if(item == TVI_ROOT || treeCtrl->ItemHasChildren(item))
			{
				HTREEITEM child = treeCtrl->GetChildItem(item);

				while (child != NULL)
				{
					SortTreeItem(treeCtrl, child);
					child = treeCtrl->GetNextItem(child, TVGN_NEXT);
				}

				TVSORTCB tvs;
				tvs.hParent = item;
				tvs.lpfnCompare = CompareProc;
				tvs.lParam = reinterpret_cast<LPARAM>(&treeCtrl);

				treeCtrl->SortChildrenCB(&tvs);
			}
		}
	}
}

PTTreeworkBase::PTTreeworkBase() : m_treeCtrl(0), m_storage(0), m_mediator(0)
{
}