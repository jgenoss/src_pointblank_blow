#include "stdafx.h"
#include "i3aCrawler.h"
#include "RscTool/RSCFileMgr.h"
#include "i3Base/string/ext/extract_fileext.h"

namespace rsc
{
	i3aCrawler::i3aCrawler(const Path& workdir, const Path& i3aRelPath)
		: IFilePathable(workdir, i3aRelPath)
	{}

	i3aCrawler::~i3aCrawler() {}

	bool i3aCrawler::_Load()
	{
		if (!i3::generic_is_iequal(GetRelativePath().extension().string(), ".i3a"))
			return false;

		return true;
	}

	bool i3aCrawler::_Gather(std::vector<PathPtr>& out)
	{
		i3::vector<i3::rc_string> animpackRelPathList;
		RSCFileMgr::i()->FindAnimPackFile(GetRelativePath().string(), animpackRelPathList);

		for (auto &animpackRelPath : animpackRelPathList)
			out.push_back(std::make_shared<Path>(animpackRelPath.c_str()));

		return true;
	}
}