#include "stdafx.h"
#include "PackRule_GuiItem.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include <regex>
#include "util/PathTokenizer.h"
#include "../util/CustomUtil.h"
#include <boost/format.hpp>
#include <array>

namespace rsc
{
	std::array<std::pair<const char*, const char*>, 5> g_arrItemFilePackInfo =
	{
		std::make_pair("item_Beret",		"UI_Item-Beret_New.i3Pack"), 
		std::make_pair("item_cash",			"UI_Item-Cash.i3Pack"), 
		std::make_pair("item_cha",			"UI_Item-Cha.i3Pack"), 
		std::make_pair("item_head",			"UI_Item-Head.i3Pack"),
		std::make_pair("item_Mask",			"UI_Item-Mask.i3Pack")
	};

	PackRule_Gui_Item::PackRule_Gui_Item(const std::string& strFileHeadName,
										 const std::vector<PathPtr>& srcList,
										 const Path& srcWorkdir,
										 const Path& dstWorkdir)
		: IPackRule(srcList, srcWorkdir, dstWorkdir), m_strFileHeadName(strFileHeadName)
	{}

	PackRule_Gui_Item::~PackRule_Gui_Item()
	{}

	void PackRule_Gui_Item::Creator(std::vector<PathPtr>& inout,
									const Path& srcWorkdir,
									const Path& dstWorkdir,
									std::vector<PackRulePtr>& out)
	{
		struct PickoutGuiItemPred : std::unary_function<PathPtr, bool>
		{
			bool operator()(const PathPtr& src) const
			{
				std::vector<std::string> strTokenList;
				GetPathTokenList(*(src.get()), strTokenList);

				if (strTokenList.size() < 3)
					return true;

				if (!src->has_extension())
					return true;

				for (size_t i = 0; i < strTokenList.size()-1; i++)
				{
					if (i3::generic_is_iequal(strTokenList[i], "Gui") &&
						i3::generic_is_iequal(strTokenList[i+1], "Item"))
					{
						return false;
					}
				}

				return true;
			}
		};

		std::vector<PathPtr> pickoutList;
		std::remove_copy_if(inout.begin(), inout.end(), std::back_inserter(pickoutList), PickoutGuiItemPred());

		using PathListPtr = std::shared_ptr<std::vector<PathPtr>>;
		using ClassifyMap = std::map<std::string, PathListPtr>;
		ClassifyMap tbl;

		std::string strFileName;
		std::string strFileHeadName;
		for (auto &pickout : pickoutList)
		{
			strFileName = pickout->filename().string();
			
			for (auto &currPackInfo : g_arrItemFilePackInfo)
			{
				strFileHeadName = currPackInfo.first;
				if (strFileName.find(strFileHeadName) != std::string::npos)
				{
					auto it = tbl.find(strFileHeadName);
					if (it == tbl.end())
					{
						auto pr = tbl.insert(ClassifyMap::value_type(strFileHeadName, std::make_shared<std::vector<PathPtr>>()));
						if (pr.second == false)
							continue;

						it = pr.first;
					}

					(it->second)->push_back(pickout);
					break;
				}
			}
		}

		PathListPtr classifiedList;
		for (auto it = tbl.begin(); it != tbl.end(); ++it)
		{
			strFileHeadName = it->first;
			classifiedList = it->second;

			out.push_back(std::make_shared<PackRule_Gui_Item>(strFileHeadName, *classifiedList, srcWorkdir, dstWorkdir));
		}
	}

	bool PackRule_Gui_Item::Analyze(PackAnalysis& outAnalysis, std::vector<PathPtr>& outCopyList)
	{
		if (!fs::is_directory(m_srcWorkdir))
			return false;

		std::string stri3PackName;
		for (auto &currPackInfo : g_arrItemFilePackInfo)
		{
			if (m_strFileHeadName == currPackInfo.first)
			{
				stri3PackName = currPackInfo.second;
				break;
			}
		}

		if (stri3PackName.empty())
			return false;

		// pack target paths.
		std::copy(m_srcPackTgtList.begin(), m_srcPackTgtList.end(), std::back_inserter(outAnalysis.targetList));

		// name of pack.
		outAnalysis.stri3PackName = stri3PackName;

		// add.
		auto add = std::make_shared<PackAddTarget>();

		add->strRelPath = boost::io::str(boost::format("Gui/Item/%s*.i3i") % m_strFileHeadName);
		add->includeSubFolderOK = false;

		outAnalysis.addList.push_back(add);

		// checksum.
		outAnalysis.checksumOK = true;

		return true;
	}
}