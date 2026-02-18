#include "stdafx.h"
#include "PackRule_Chara.h"
#include "FileSync_Copy(Deprecated).h"
#include <boost/filesystem.hpp>
#include "util/PathTokenizer.h"
#include "i3pack.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_Chara::PackRule_Chara(const std::string& strCharaName, 
								   const std::vector<PathPtr>& srcList, 
								   const Path& srcWorkdir, 
								   const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strCharaName(strCharaName)
	{}

	PackRule_Chara::~PackRule_Chara()
	{}

	void PackRule_Chara::Creator(std::vector<PathPtr>& inout,
								 const Path& srcWorkdir,
								 const Path& dstWorkdir,
								 std::vector<PackRulePtr>& out)
	{
		struct PickoutCharaFolderFilesPred : std::unary_function<Path, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::string strRootFolerName;
				const bool ok = GetPathToken(*src, 0, strRootFolerName);
				if (ok)
				{
					if (GetNumberOfTokens(*src) != 3)
						return true;

					if (!i3::generic_is_iequal(strRootFolerName, "Chara"))
						return true;

					const std::string& strExt = src->extension().string();

					if (i3::generic_is_iequal(strExt, ".i3AnimPack") ||
						i3::generic_is_iequal(strExt, ".I3CHR") ||
						i3::generic_is_iequal(strExt, ".I3S"))
					{
						return false;
					}
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutCharaFolderFilesPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::vector<std::string> strTokenList;
		std::string strCharaName;
		for (auto &pickout : pickoutList)
		{
			GetPathTokenList(*pickout, strTokenList);
			if (strTokenList.size() < 2)
				continue;

			strCharaName = strTokenList[1];
			if (strCharaName.find(".") != std::string::npos)
				continue;

			auto it = tbl.find(strCharaName);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ClassifyMap::value_type(strCharaName, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strCharaName = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Chara>(strCharaName, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Chara::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		std::vector<std::string> packTargetList;

		std::string strRootFolderName;

		// Chara 폴더 내 i3AnimPack/I3CHR 파일들은 패킹 대상이 아닌 단순 카피 대상이다.
		std::vector<PathPtr>::iterator it = m_srcPackTgtList.begin();
		for (; it != m_srcPackTgtList.end(); )
		{
			const std::string& strExt = (*it)->extension().string();

			if (i3::generic_is_iequal(strExt, ".i3AnimPack") ||
				i3::generic_is_iequal(strExt, ".I3CHR"))
			{
				outCopyList.push_back((*it));
				it = m_srcPackTgtList.erase(it);
				continue;
			}

			++it;
		}

		if (m_srcPackTgtList.empty())
			return true;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = boost::io::str(boost::format("Chara_%s.i3Pack") % m_strCharaName);;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("Chara/%s") % m_strCharaName);
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// remove.
		outAnalysis.strRemoveRelPathList.push_back(boost::io::str(boost::format("Chara/%s/*.i3CHR") % m_strCharaName));
		outAnalysis.strRemoveRelPathList.push_back(boost::io::str(boost::format("Chara/%s/*.i3AnimPack") % m_strCharaName));

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}