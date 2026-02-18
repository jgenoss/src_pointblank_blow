#include "stdafx.h"
#include "PackRule_EquipMask.h"
#include "i3pack.h"
#include "util/PathTokenizer.h"
#include <boost/filesystem.hpp>
#include <array>
#include "../util/CustomUtil.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_EquipMask::PackRule_EquipMask(const std::string& strEquipMaskFolderRelPath,
										   const std::vector<PathPtr>& srcList, 
										   const Path& srcWorkdir, 
										   const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strEquipMaskFolderRelPath(strEquipMaskFolderRelPath)
	{}

	PackRule_EquipMask::~PackRule_EquipMask()
	{}

	void PackRule_EquipMask::Creator(std::vector<PathPtr>& inout,
									 const Path& srcWorkdir,
									 const Path& dstWorkdir,
									 std::vector<PackRulePtr>& out)
	{
		struct PickoutEquipMaskPred : std::unary_function<Path, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() <= 1)
					return true;

				if (i3::generic_is_iequal(strTokenList[0], "Equip") &&
					i3::generic_is_iequal(strTokenList[1], "Mask"))
				{
					if (!src->has_extension())
						return true;

					if (i3::generic_is_iequal(src->parent_path().filename().string(), "Resource"))
						return true;

					if (GetNumberOfTokens(*src) > 2)	// 'Equip/Mask' 하위 폴더 대상 유효.
						return false;
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(),inout.end(), std::back_inserter(pickoutList), PickoutEquipMaskPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::string strEquipMaskFolderRelPath;
		for (auto &pickout : pickoutList)
		{
			strEquipMaskFolderRelPath = pickout->parent_path().string();

			auto it = tbl.find(strEquipMaskFolderRelPath);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ClassifyMap::value_type(strEquipMaskFolderRelPath, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strEquipMaskFolderRelPath = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_EquipMask>(strEquipMaskFolderRelPath, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_EquipMask::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		std::vector<std::string> tokenList;
		GetPathTokenList(m_strEquipMaskFolderRelPath, tokenList);

		// 마지막 폴더명에서 "Mask_" 또는 "_Mask" 제외.
		const std::array<std::string, 2> strEraseKeyList = { "Mask_", "_Mask" };
		std::string& lastToken = tokenList.back();

		for (auto &strEraseKey : strEraseKeyList)
		{
			auto pos = lastToken.find(strEraseKey);
			if (pos != std::string::npos)
			{
				lastToken.erase(pos, pos + strEraseKey.size());
			}
		}
		
		std::string strEquipMaski3PackName;
		for (auto &token : tokenList)
		{
			strEquipMaski3PackName += token;
			strEquipMaski3PackName += "_";
		}

		if (!strEquipMaski3PackName.empty())
			strEquipMaski3PackName.erase(strEquipMaski3PackName.end() - 1); // 마지막 '_' 제거.
		
		strEquipMaski3PackName += ".i3Pack";

		outAnalysis.stri3PackName = strEquipMaski3PackName;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = m_strEquipMaskFolderRelPath;
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}