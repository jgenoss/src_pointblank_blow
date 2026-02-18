#pragma once

#include "RscTool/i3MFCTreeCtrl.h"

namespace { struct i3PackToolTreeCallback; }
namespace { struct i3PackToolTreeFindCallback; }
class ThreadPool;

namespace ui
{
	class i3PackToolDlg;

	struct i3PackToolTreeItem
	{
		std::string strFileRelPath;
		size_t offset = -1;
		size_t size = -1;
		bool isENC = false;
	};
	using i3PackToolTreeItemPtr = std::shared_ptr<i3PackToolTreeItem>;

	class i3PackToolTree final
	{
	public:
		i3PackToolTree(i3PackToolDlg& owner);
		~i3PackToolTree();

		void AdjustLayout(const RECT& rt);
		void Expandi3PackDetailInfoByPath(const std::string& stri3PackAbsPath);
		void Expandi3PackDetailInfoByNode(const i3PackNode* rootNode, const std::string& stri3PackAbsPath);
		void ShutdownThreads();
		void AddFilesToCurrPack(const std::vector<std::string>& fileAbsPathList);
		void OnNewPack();
		void OnSavePack();
		void OnAddFilesToPack();
		void OnAddFolderFilesToPack();

		i3MFCTreeCtrl& GetCtrl() { return m_ctrl; }

	private:
		i3PackToolDlg& m_dlg;
		i3MFCTreeCtrl m_ctrl;
		std::string m_stri3PackAbsPath;
		std::unique_ptr<ThreadPool> m_thpool;

		friend struct i3PackToolTreeCallback;
		friend struct i3PackToolTreeFindCallback;
	};
}