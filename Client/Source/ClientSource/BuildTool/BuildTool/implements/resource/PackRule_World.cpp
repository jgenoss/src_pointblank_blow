#include "stdafx.h"
#include "PackRule_World.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_World::PackRule_World(const std::string& strWorldName, 
								   const std::vector<PathPtr>& srcList,
								   const Path& srcWorkdir, 
								   const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strWorldName(strWorldName)
	{}

	PackRule_World::~PackRule_World() 
	{}

	void PackRule_World::Creator(std::vector<PathPtr>& inout,
								 const Path& srcWorkdir,
								 const Path& dstWorkdir,
								 std::vector<PackRulePtr>& out)
	{
		struct PickoutWorldPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				// ---------원본 팩스크립트 규칙---------------
				// add World/AngkorRuins -nosub
				//
				// remove World/AngkorRuins/AngkorRuins_Col_*.I3S
				// remove World/AngkorRuins/AngkorRuins_Light.i3s
				//
				// save AngkorRuins.i3Pack
				// checksum AngkorRuins.i3Pack
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
					const std::string strColI3SPartialName = boost::io::str(boost::format("%s_Col_") % strWorldName); // ..._Col_*.I3S.
					if (strFileName.find(strColI3SPartialName))
						return true;

					const std::string strLightI3SPartialName = boost::io::str(boost::format("%s_Light") % strWorldName); // ...*_Light.I3S.
					if (strFileName.find(strLightI3SPartialName))
						return true;
				}

				return false;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutWorldPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::string strWorldName;
		for (auto &pickout : pickoutList)
		{
			if (!GetPathToken(*(pickout.get()), 1, strWorldName))
				continue;

			auto it = tbl.find(strWorldName);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ClassifyMap::value_type(strWorldName, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strWorldName = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_World>(strWorldName, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_World::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = 
			boost::io::str(boost::format("%s.i3Pack") % m_strWorldName);;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("World/%s") % m_strWorldName);
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// remove.
		outAnalysis.strRemoveRelPathList.push_back(
			boost::io::str(boost::format("World/%s/%s_Col_*.I3S") % m_strWorldName % m_strWorldName));
		outAnalysis.strRemoveRelPathList.push_back(
			boost::io::str(boost::format("World/%s/%s_Light.I3S") % m_strWorldName % m_strWorldName));

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}