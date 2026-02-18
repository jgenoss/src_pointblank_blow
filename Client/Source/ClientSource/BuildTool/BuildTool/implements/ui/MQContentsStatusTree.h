#pragma once

#include "RscTool/i3MFCTreeCtrl.h"
#include "MQContentsStatusBar.h"
#include "resource/ContentsDefines.h"

namespace { struct MQContentsStatusTreeCallback; }
namespace { struct MQContentsStatusTreeFindCallback; }

namespace ui
{
	class MQContentsStatusTree final
	{
	public:
		MQContentsStatusTree(MQContentsStatusBar& owner);
		~MQContentsStatusTree();

		void AdjustLayout(const int cx, const int cy);

		void InsertItems(const std::vector<std::wstring>& wstrPathList, 
			const std::wstring& wstrContentsName, const std::wstring& wstrCountryName);

		void ClearItems();

		void SelectItem(const std::wstring& wstrPath);

	private:
		i3MFCTreeCtrl m_ctrl;
		std::string m_strCountryName;

		using ContentsStatusFlagPtr = std::shared_ptr<rsc::ContentsStatusFlag>;
		std::vector<ContentsStatusFlagPtr> m_statusList;

		friend struct MQContentsStatusTreeCallback;
		friend struct MQContentsStatusTreeFindCallback;
	};
}
