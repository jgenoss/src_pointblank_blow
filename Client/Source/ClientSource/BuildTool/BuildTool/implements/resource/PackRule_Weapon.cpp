#include "stdafx.h"
#include "PackRule_Weapon.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "../util/CustomUtil.h"
#include "../util/PathTokenizer.h"
#include <boost/format.hpp>

namespace rsc
{
	PackRule_Weapon::PackRule_Weapon(const std::string& strWeaponName, 
									 const std::vector<PathPtr>& srcList, 
									 const Path& srcWorkdir, 
									 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strWeaponName(strWeaponName)
	{}

	PackRule_Weapon::~PackRule_Weapon() 
	{}

	void PackRule_Weapon::Creator(std::vector<PathPtr>& inout,
								  const Path& srcWorkdir,
								  const Path& dstWorkdir,
								  std::vector<PackRulePtr>& out)
	{
		struct PickoutWeaponPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::string strRootFolerName;
				const bool ok = GetPathToken(*(src.get()), 0, strRootFolerName);
				if (ok)
				{
					if (!i3::generic_is_iequal(strRootFolerName, "Weapon"))
						return true;

					const std::string strExtension = src->extension().string();
					if (i3::generic_is_iequal(strExtension, ".RSC") ||
						i3::generic_is_iequal(strExtension, ".RSG"))
					{
						return true;
					}

					// 메시 LOD 및 i3a 원본 파일들은 패킹 대상 아님.
					const std::string strParentFolderName = src->parent_path().filename().string();
					if (i3::generic_is_iequal(strParentFolderName, "Resource") ||
						i3::generic_is_iequal(strParentFolderName, "Res"))
					{
						return true;
					}

					return false;
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutWeaponPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::string strWeaponName;
		for (auto &pickout : pickoutList)
		{
			if (!GetPathToken(*(pickout.get()), 1, strWeaponName))
				continue;

			auto it = tbl.find(strWeaponName);
			if (it == tbl.end())
			{
				auto pr = tbl.insert(ClassifyMap::value_type(strWeaponName, std::make_shared<std::vector<PathPtr>>()));
				if (pr.second == false)
					continue;

				it = pr.first;
			}

			(it->second)->push_back(pickout);
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strWeaponName = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Weapon>(strWeaponName, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Weapon::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = boost::io::str(boost::format("Weapon_%s.i3Pack") % m_strWeaponName);

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("Weapon/%s") % m_strWeaponName);
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}