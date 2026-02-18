#pragma once

#include <functional>
#include "IFilePathable.h"

namespace rsc
{
	class IPackCommand;

	class PackScriptParser final : public IFilePathable
	{
	public:
		PackScriptParser(const Path& i3PackScriptAbsPath);
		PackScriptParser(const Path& i3PackScriptWorkdir, const Path& i3PackScriptRelPath);
		~PackScriptParser();

		void operator()(std::vector<std::vector<std::string>>& outCmdParams);

	private:
		i3FileStream m_fstream;
		bool m_openOK = false;
	};
}