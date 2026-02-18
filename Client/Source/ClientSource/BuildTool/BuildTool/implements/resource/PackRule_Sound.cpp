#include "stdafx.h"
#include "PackRule_Sound.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_Sound::PackRule_Sound(const std::string& strSoundFolderRelPath,
								   const std::vector<PathPtr>& srcList,
								   const Path& srcWorkdir, 
								   const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strSoundFolderRelPath(strSoundFolderRelPath)
	{}

	PackRule_Sound::~PackRule_Sound() 
	{}

	void PackRule_Sound::Creator(std::vector<PathPtr>& inout,
								 const Path& srcWorkdir,
								 const Path& dstWorkdir,
								 std::vector<PackRulePtr>& out)
	{
		struct PickoutSoundPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				if (GetNumberOfTokens(*(src.get())) == 1)	// 단순히 Sound 폴더일 경우 패킹 대상에서 제외.
					return true;

				if (src->has_extension() && GetNumberOfTokens(*(src.get())) <= 2) // Sound 폴더의 하위 폴더가 아닌 RSG, RSC 파일들일 경우는 팩스크립트 활용 패킹하므로 조건에서 제외.
					return true;

				std::string strRootFolderName;
				GetPathToken(*(src.get()), 0, strRootFolderName);
				if (i3::generic_is_iequal(strRootFolderName, "Sound"))
					return false;

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutSoundPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using SoundClassifiedMap = std::map<std::string, PathListPtr>;
		SoundClassifiedMap tbl;

		std::string strSoundRelPath;
		for (auto &pickout : pickoutList)
		{
			strSoundRelPath = (pickout->has_extension()) ? pickout->parent_path().string() : pickout->string();

			auto it = tbl.find(strSoundRelPath);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(SoundClassifiedMap::value_type(strSoundRelPath, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strSoundRelPath = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Sound>(strSoundRelPath, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Sound::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		std::string strSoundi3PackName;

		std::vector<std::string> tokenList;
		GetPathTokenList(m_strSoundFolderRelPath, tokenList);
		for (auto &token : tokenList)
		{
			strSoundi3PackName += token;
			strSoundi3PackName += "_";
		}

		if (!strSoundi3PackName.empty())
			strSoundi3PackName.erase(strSoundi3PackName.end() - 1);	// 마지막 '_' 제거.

		strSoundi3PackName += ".i3Pack";

		outAnalysis.stri3PackName = strSoundi3PackName;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = m_strSoundFolderRelPath;
		add->includeSubFolderOK	= true;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}