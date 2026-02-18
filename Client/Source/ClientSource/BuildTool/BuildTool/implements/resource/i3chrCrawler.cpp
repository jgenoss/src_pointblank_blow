#include "stdafx.h"
#include "i3chrCrawler.h"
#include "i3sCrawler.h"
#include "i3aCrawler.h"
#include <array>
#include <algorithm>
#include <Util/PathTokenizer.h>

namespace rsc
{
	i3chrCrawler::i3chrCrawler(const Path& workdir, const Path& i3chrRelPath)
		: IFilePathable(workdir, i3chrRelPath)
	{}

	i3chrCrawler::~i3chrCrawler() {}

	bool i3chrCrawler::_Load()
	{
		if (!i3::generic_is_iequal(GetAbsolutePath().extension().string(), ".i3chr"))
			return false;

		return true;
	}

	bool i3chrCrawler::_Gather(std::vector<PathPtr>& out)
	{
		const Path& i3chrRelPath = GetRelativePath();
		const Path& i3chrAbsPath = GetAbsolutePath();

		i3RscPartialFile f;
		if (!f.Open(i3chrAbsPath.string().c_str()))
			return false;

		i3::vector<i3Rsc_ExternFilename> externFileList;
		f.CollectExternFileName(externFileList);

		i3sCrawler i3sCwl;	
		dynamic_cast<IFilePathable*>(&i3sCwl)->SetWorkdir(GetWorkdir());
		
		i3aCrawler i3aCwl;	
		dynamic_cast<IFilePathable*>(&i3aCwl)->SetWorkdir(GetWorkdir());

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
				externFilePath = i3chrRelPath.parent_path() / externFilePath;

			out.push_back(std::make_shared<Path>(externFilePath));

			const std::string& ext = externFilePath.extension().string();

			if (i3::generic_is_iequal(".i3s", ext))
			{
				dynamic_cast<IFilePathable*>(&i3sCwl)->SetRelativePath(externFilePath);
				i3sCwl.operator()(out);
			}
			else if (i3::generic_is_iequal(".i3a", ext))
			{
				dynamic_cast<IFilePathable*>(&i3aCwl)->SetRelativePath(externFilePath);
				i3aCwl.operator()(out);
			}
			
		}

		return true;
	}
}