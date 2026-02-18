#pragma once

#include "util/ObjectFactory.h"
#include <functional>

namespace ui
{
	using ToolCreateFuncType = std::function<void()>;

	class ToolFactory final : public i3::shared_ginst<ToolFactory>
	{
	public:
		ToolFactory();
		~ToolFactory();

		const ToolCreateFuncType& Request(const std::string& strToolName);
		void GetAllToolName(std::vector<std::string>& out) const;

	private:
		void _Regist(const std::string& strToolName, const ToolCreateFuncType& f);

		struct Creator
		{
			std::string	strToolName;
			ToolCreateFuncType createFunc;
			Creator(const std::string& name, const ToolCreateFuncType& f) : strToolName(name), createFunc(f) {}
		};
		using CreatorPtr = std::shared_ptr<Creator>;

		std::vector<CreatorPtr> m_creators;
	};
}