#include "stdafx.h"
#include "ToolFactory.h"
#include "ShaderSyncDlg.h"

namespace
{
	void NullFunc() {}

	std::function<void()> g_nullFunc(NullFunc);
}

namespace ui
{
	ToolFactory::ToolFactory()
	{
		_Regist("ShaderSync", &ShaderSyncDlg::Creator);
	}

	ToolFactory::~ToolFactory()
	{}

	const ToolCreateFuncType& ToolFactory::Request(const std::string& strToolName)
	{
		const auto it = std::find_if(m_creators.begin(), m_creators.end(),
			[&strToolName](const CreatorPtr& src) -> bool
			{
				return i3::generic_is_iequal(src->strToolName, strToolName);
			}
			);

		if (it != m_creators.end())
			return (*it)->createFunc;

		return g_nullFunc;
	}

	void ToolFactory::GetAllToolName(std::vector<std::string>& out) const
	{
		for (const auto& creator : m_creators)
			out.push_back(creator->strToolName);
	}

	void ToolFactory::_Regist(const std::string& strToolName, const ToolCreateFuncType& f)
	{
		m_creators.push_back(std::make_unique<Creator>(strToolName, f));
	}
}