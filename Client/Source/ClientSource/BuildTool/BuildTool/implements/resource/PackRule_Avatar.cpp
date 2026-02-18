#include "stdafx.h"
#include "PackRule_Avatar.h"
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include <boost/filesystem.hpp>
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_Avatar::PackRule_Avatar(const std::string& strAvatarFolderRelPath, 
									 const std::vector<PathPtr>& srcList, 
									 const Path& srcWorkdir, 
									 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strAvatarFolderRelPath(strAvatarFolderRelPath)
	{}

	PackRule_Avatar::~PackRule_Avatar()
	{}

	void PackRule_Avatar::Creator(std::vector<PathPtr>& inout, 
								  const Path& srcWorkdir, 
								  const Path& dstWorkdir, 
								  std::vector<PackRulePtr>& out)
	{
		struct PickoutAvatarPred : std::unary_function<Path, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() <= 2)	// Avatar/.../ 의 하위 파일.
					return true;

				if (!i3::generic_is_iequal(strTokenList[0], "Avatar"))
					return true;

				if (!src->has_extension())
					return true;

				if (i3::generic_is_iequal(src->parent_path().filename().string(), "RES"))
					return true;

				return false;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutAvatarPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using AvatarClassifiedMap = std::map<std::string, PathListPtr>;
		AvatarClassifiedMap tbl;

		std::string strAvatarFolderRelPath;
		for (auto &pickout : pickoutList)
		{
			strAvatarFolderRelPath = pickout->parent_path().string();

			auto it = tbl.find(strAvatarFolderRelPath);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(AvatarClassifiedMap::value_type(strAvatarFolderRelPath, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strAvatarFolderRelPath = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Avatar>(strAvatarFolderRelPath, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Avatar::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		std::vector<std::string> strTokenList;
		GetPathTokenList(m_strAvatarFolderRelPath, strTokenList);
		std::string strAvatarName(strTokenList.back());

		outAnalysis.stri3PackName = boost::io::str(boost::format("%s.i3Pack") % strAvatarName);

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = m_strAvatarFolderRelPath;
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}