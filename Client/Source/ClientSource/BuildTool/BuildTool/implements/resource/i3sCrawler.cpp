#include "stdafx.h"
#include "i3sCrawler.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3aCrawler.h"
#include "RscTool/i3RscPartialFile.h"
#include <Util/PathTokenizer.h>

namespace rsc
{
	i3sCrawler::i3sCrawler(const Path& workdir, const Path& i3sRelPath)
		: IFilePathable(workdir, i3sRelPath)
	{}

	i3sCrawler::~i3sCrawler() {}

	bool i3sCrawler::_Load()
	{
		if (!i3::generic_is_iequal(GetRelativePath().extension().string(), ".I3S"))
			return false;

		return true;
	}

	bool i3sCrawler::_Gather(std::vector<PathPtr>& out)
	{
		const Path& i3sRelPath = GetRelativePath();
		const Path& i3sAbsPath = GetAbsolutePath();

		i3RscPartialFile f;
		if (!f.Open(i3sAbsPath.string().c_str()))
			return false;

		i3::vector<i3Rsc_ExternFilename> externFileList;
		f.CollectExternFileName(externFileList);

		Path externFilePath;
		std::vector<std::string> pathTokenList;

		for (auto &externFile : externFileList)
		{
			externFilePath = externFile.m_file_name.c_str();
			GetPathTokenList(externFilePath, pathTokenList);

			const size_t tokenCount = pathTokenList.size();
			if (tokenCount == 0)
				continue;

			if (tokenCount == 1)	// 익스턴 경로 중에 디렉토리가 없고 /File.ext 형식으로만 된것이 있어서 감안해 처리한다..
				externFilePath = i3sRelPath.parent_path() / externFilePath;

			out.push_back(std::make_shared<Path>(externFilePath));
		}

		return true;
	}
}