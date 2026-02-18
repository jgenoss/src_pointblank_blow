#include "stdafx.h"
#include "PackRule_World_Col_Hero.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_World_Col_Hero::PackRule_World_Col_Hero(const std::string& strWorldName,
													 const std::vector<PathPtr>& srcList,
													 const Path& srcWorkdir,
													 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strWorldName(strWorldName)
	{}

	PackRule_World_Col_Hero::~PackRule_World_Col_Hero()
	{}

	void PackRule_World_Col_Hero::Creator(std::vector<PathPtr>& inout,
										  const Path& srcWorkdir,
										  const Path& dstWorkdir,
										  std::vector<PackRulePtr>& out)
	{
		struct PickoutWorldColHeroPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				// ---------원본 팩스크립트 규칙---------------
				//	add World/AngkorRuins/AngkorRuins_Col_Hero.I3S
				//	enc World/AngkorRuins/AngkorRuins_Col_Hero.I3S
				//
				//	save AngkorRuins_Col_Hero.i3Pack
				//	checksum AngkorRuins_Col_Hero.i3Pack
				// --------------------------------------------

				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (!src->has_extension())	// 폴더 대상 X.
					return true;

				if (strTokenList.size() != 3)	// "World/폴더명/파일명" 이므로 깊이는 3만 유효.
					return true;

				const std::string& strRootFolderName = strTokenList[0];	// "World/..." 대상으로만.
				if (!i3::generic_is_iequal(strRootFolderName, "World"))
					return true;

				const std::string& strWorldName = strTokenList[1];
				const std::string& strFileName = strTokenList[2];
				const std::string& strExt = src->extension().string();

				if (i3::generic_is_iequal(strExt, ".I3S"))
				{
					const std::string strColHeroI3S = boost::io::str(boost::format("%s_Col_Hero.I3S") % strWorldName);
					if (i3::generic_is_iequal(strFileName, strColHeroI3S))
						return false;
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutWorldColHeroPred());

		std::string strWorldName;
		for (auto &pickout : pickoutList)
		{
			if (!GetPathToken(*pickout, 1, strWorldName))
				continue;

			out.push_back(std::make_shared<PackRule_World_Col_Hero>(
				strWorldName, std::vector<PathPtr>({ pickout }), srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_World_Col_Hero::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;
		
		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = boost::io::str(boost::format("%s_Col_Hero.i3Pack") % m_strWorldName);;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("World/%s/%s_Col_Hero.I3S") % m_strWorldName % m_strWorldName);
		add->includeSubFolderOK = true;

		outAnalysis.addList.push_back(add);

		// enc.
		outAnalysis.strEncryptRelPathList.push_back(add->strRelPath);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}