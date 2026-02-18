#pragma once

#include "RscTool/i3MFCTreeCtrl.h"
#include "QLContentsStatusBar.h"
#include "resource/ContentsDefines.h"

namespace { struct QLContentsStatusTreeCallback; }
namespace { struct QLContentsStatusTreeFindCallback; }

namespace ui
{
	class QLContentsStatusTree final
	{
	public:
		QLContentsStatusTree(QLContentsStatusBar& owner);
		~QLContentsStatusTree();

		void AdjustLayout(const int cx, const int cy);

		void InsertItems(const std::wstring& wstrContentsName, const std::wstring& wstrCountryName, 
			const std::vector<std::wstring>& wstrFoundRelPathList, const std::vector<std::wstring>& wstrNotFoundRelPathList);

		void ClearItems();

		void SelectItem(const std::wstring& wstrPath);

	private:
		i3MFCTreeCtrl m_ctrl;
		std::string m_strCountryName;

		using ContentsStatusFlagPtr = std::shared_ptr<rsc::ContentsStatusFlag>;
		std::vector<ContentsStatusFlagPtr> m_statusList;

		friend struct QLContentsStatusTreeCallback;
		friend struct QLContentsStatusTreeFindCallback;
	};
}

