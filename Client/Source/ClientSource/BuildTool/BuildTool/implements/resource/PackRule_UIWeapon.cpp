#include "stdafx.h"
#include "PackRule_UIWeapon.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include <regex>
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>
#include <array>

namespace rsc
{
	void CreateRuleByClassifiedList(const std::vector<PathPtr> pickoutList,
									const Path& srcWorkdir,
									const Path& dstWorkdir,
									std::vector<PackRulePtr>& out)
	{
		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::string strFileParentFolderName;
		for (auto &pickout : pickoutList)
		{
			strFileParentFolderName = pickout->parent_path().string();

			auto it = tbl.find(strFileParentFolderName);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ClassifyMap::value_type(strFileParentFolderName, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		PathPtr tempPathPtr;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			classifiedList = it->second;

			std::string strUIFileName;
			for (auto vit = classifiedList->begin(); vit != classifiedList->end();)
			{
				tempPathPtr = *vit;

				if (GetNumberOfTokens(*tempPathPtr) < 4)	// 파일 하나만 패킹되는 상황. UI 파일을 패킹할 때 예전에 쓰던 방식. 호환성을 위해.
				{
					out.push_back(std::make_shared<PackRule_UIWeapon>(*tempPathPtr, std::vector<PathPtr>({ tempPathPtr }), srcWorkdir, dstWorkdir));

					vit = classifiedList->erase(vit);
					continue;
				}

				++vit;
			}

			// 폴더 내 파일들이 한꺼번에 패킹되는 상황.
			if (!classifiedList->empty())
				out.push_back(std::make_shared<PackRule_UIWeapon>(*classifiedList->front(), *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	PackRule_UIWeapon::PackRule_UIWeapon(const Path& representPath,
										 const std::vector<PathPtr>& srcList,
										 const Path& srcWorkdir, 
										 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_representPath(representPath)
	{}

	PackRule_UIWeapon::~PackRule_UIWeapon() 
	{}

	void PackRule_UIWeapon::CreatorV1(std::vector<PathPtr>& inout,
									  const Path& srcWorkdir,
									  const Path& dstWorkdir,
									  std::vector<PackRulePtr>& out)
	{
		struct PickoutGuiPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() < 2)
					return true;

				if (!src->has_extension())
					return true;

				if (!i3::generic_is_iequal(strTokenList[0], "Gui"))
					return true;

				const std::string& strFolderName = strTokenList[1];
				const std::string strMatchKey("Weapon");

				if (strFolderName.find(strMatchKey) == std::string::npos)
					return true;

				if (i3::generic_is_iequal(strFolderName.substr(0, strMatchKey.size()), strMatchKey))
					return false;

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutGuiPred());

		CreateRuleByClassifiedList(pickoutList, srcWorkdir, dstWorkdir, out);
	}

	void PackRule_UIWeapon::CreatorV2(std::vector<PathPtr>& inout,
									  const Path& srcWorkdir,
									  const Path& dstWorkdir,
									  std::vector<PackRulePtr>& out)
	{
		struct PickoutUIPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() < 3)
					return true;

				if (!src->has_extension())
					return true;

				const bool isImageFolder =
					i3::generic_is_iequal(strTokenList[0], "UIRe") && i3::generic_is_iequal(strTokenList[1], "Image");
				if (!isImageFolder)
					return true;

				if (i3::generic_is_iequal(strTokenList[2], "Weapon_detail") ||
					i3::generic_is_iequal(strTokenList[2], "Weapon_Kill"))
				{
					return false;
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutUIPred());

		CreateRuleByClassifiedList(pickoutList, srcWorkdir, dstWorkdir, out);
	}

	void PackRule_UIWeapon::CreatorV3(std::vector<PathPtr>& inout,
									  const Path& srcWorkdir,
									  const Path& dstWorkdir,
									  std::vector<PackRulePtr>& out)
	{
		struct PickoutUIPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() < 3)
					return true;

				if (!src->has_extension())
					return true;

				std::array<const char*, 3> arrRootFolderName = { "UI_V10", "UI_V11", "UI_V15" };
				
				bool ok = false;
				for (auto &strFolderName : arrRootFolderName)
				{
					if (i3::generic_is_iequal(strTokenList[0], strFolderName))
					{
						ok = true;
						break;
					}
				}
				if (ok == false)
					return true;

				if (!i3::generic_is_iequal(strTokenList[1], "Image"))
					return true;

				if (i3::generic_is_iequal(strTokenList[2], "Weapon_detail") ||
					i3::generic_is_iequal(strTokenList[2], "Weapon_Kill"))
				{
					return false;
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutUIPred());

		CreateRuleByClassifiedList(pickoutList, srcWorkdir, dstWorkdir, out);
	}

	bool PackRule_UIWeapon::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		std::string strHeadName;
		if (!GetPathToken(m_representPath, 0, strHeadName))
			return false;

		if (i3::generic_is_iequal(strHeadName, "UIRe") ||
			i3::generic_is_iequal(strHeadName, "Gui"))
		{
			strHeadName = "UI";	// V1, V2 버전일 경우.
			// V3 버전일 경우 폴더명인 'UI_V10', 'UI_V11', 'UI_V15' 로 시작.
		}

		std::string stri3PackName(strHeadName);	// Weapon_무기폴더.i3Pack

		Path repPath(m_representPath);

		std::string strCategoryName;
		std::string strUniqueName;

		size_t tokenCount = GetNumberOfTokens(m_representPath);
		if (tokenCount >= 4)	// 폴더 통째로 패킹하는 상황.
		{
			repPath = repPath.parent_path();
			strCategoryName = repPath.parent_path().filename().string();
		}
		else
		{
			strCategoryName = repPath.parent_path().filename().string();
		}

		stri3PackName += "_";
		stri3PackName += strCategoryName;

		const std::string strWeaponName = repPath.filename().replace_extension("").string();
		const size_t pos_underLine = strWeaponName.find("_");
		if (pos_underLine != std::string::npos)
		{
			// "WeaponAssault/Assault_9" 와 같이 폴더명이 넘버로 구분되는 경우
			// "UI_WeaponAssault_9.i3Pack" 이런식으로 팩파일을 생성한다.
			std::regex reg("_(\\d)+");
			std::smatch sm;

			std::string& strFolderNumberTag = strWeaponName.substr(pos_underLine);
			if (std::regex_match(strFolderNumberTag, sm, reg))
			{
				stri3PackName += strFolderNumberTag;
			}
			else
			{
				stri3PackName += "_";
				stri3PackName += strWeaponName;
			}
		}
		else
		{
			stri3PackName += "_";
			stri3PackName += strWeaponName;
		}

		stri3PackName += ".i3Pack";

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = stri3PackName;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = repPath.string();
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}