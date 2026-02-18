#include "stdafx.h"
#include "GoodsContentsCrawler.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"
#include <array>

namespace rsc
{
	GoodsContentsCrawler::GoodsContentsCrawler(const Path& workdir, const Path& PEFRelPath)
		: IPefable(workdir, PEFRelPath)
	{}

	GoodsContentsCrawler::~GoodsContentsCrawler()
	{}

	bool GoodsContentsCrawler::__Load()
	{
		return OpenPef();
	}

	bool GoodsContentsCrawler::__Gather()
	{
		i3ResourceFileObjectInfo2* registryRootKey = GetPef()->Findi3RegKeyRoot();
		if (registryRootKey == nullptr) 
			return false;

		std::array<const char*, 2> arrSubCategoryRegKeyName = 
		{ 
			"SetGoods", 
			"ShopItem" 
		};
		i3ResourceFileObjectInfo2* subCatRegistryKey = nullptr;
		i3::vector<i3ResourceFileObjectInfo2*> subCatRegistryKeyList;
		for (auto &subCatKeyName : arrSubCategoryRegKeyName)
		{
			subCatRegistryKey = GetPef()->FindChildi3TreeElement(registryRootKey, subCatKeyName);
			if (subCatRegistryKey)
				subCatRegistryKeyList.push_back(subCatRegistryKey);
		}

		if (subCatRegistryKeyList.empty())
			return false;

		i3::rc_string strContentsName;
		i3::rc_string strSubCategoryName;
		i3::vector<i3ResourceFileObjectInfo2*> contentsRegKeyList;
		for (auto &currSubCatRegKey : subCatRegistryKeyList)
		{
			GetPef()->ReadNamei3NamedElement(currSubCatRegKey, strSubCategoryName);

			contentsRegKeyList.clear();
			GetPef()->ReadChildListi3TreeElement(contentsRegKeyList);
			for (auto &currContentsRegKey : contentsRegKeyList)
			{
				if (currContentsRegKey == nullptr)
					continue;

				GetPef()->ReadNamei3NamedElement(currContentsRegKey, strContentsName);

				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), strContentsName.c_str(), "Goods", strSubCategoryName.c_str());
			}
		}

		return true;
	}

	bool GoodsContentsCrawler::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Goods");

		return true;
	}

	bool GoodsContentsCrawler::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Goods");

		return true;
	}
}