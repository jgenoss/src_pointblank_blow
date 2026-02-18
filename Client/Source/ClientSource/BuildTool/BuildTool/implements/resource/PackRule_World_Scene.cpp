#include "stdafx.h"
#include "PackRule_World_Scene.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_World_Scene::PackRule_World_Scene(const std::string& strWorldName,
											   const std::vector<PathPtr>& srcList,
											   const Path& srcWorkdir,
											   const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strWorldName(strWorldName)
	{}

	PackRule_World_Scene::~PackRule_World_Scene()
	{}

	void PackRule_World_Scene::Creator(std::vector<PathPtr>& inout,
									   const Path& srcWorkdir,
									   const Path& dstWorkdir,
									   std::vector<PackRulePtr>& out)
	{
		struct PickoutWorldScenePred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				// ---------원본 팩스크립트 규칙---------------
				//	add Scene/AngkorRuins
				//	save AngkorRuins_Scene.i3Pack
				//	checksum AngkorRuins_Scene.i3Pack
				// --------------------------------------------

				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (!src->has_extension())	// 폴더 대상 X.
					return true;

				if (strTokenList.size() < 3)	// "Scene/폴더명/파일명" 또는 그 하위 폴더 파일들이므로 깊이는 3 이상 유효.
					return true;

				const std::string& strRootFolderName = strTokenList[0];	// "Scene/..." 대상으로만.
				if (!i3::generic_is_iequal(strRootFolderName, "Scene"))
					return true;

				return false;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutWorldScenePred());

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

			out.push_back(std::make_shared<PackRule_World_Scene>(strWorldName, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_World_Scene::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = boost::io::str(boost::format("%s_Scene.i3Pack") % m_strWorldName);

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("Scene/%s") % m_strWorldName);
		add->includeSubFolderOK = true;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}