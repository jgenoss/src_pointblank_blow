#include "stdafx.h"
#include "EquipmentContentsCrawler.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	EquipmentContentsCrawler::EquipmentContentsCrawler(const Path& workdir, const Path& PEFRelPath)
		: IPefable(workdir, PEFRelPath)
	{}

	EquipmentContentsCrawler::~EquipmentContentsCrawler() 
	{}

	bool EquipmentContentsCrawler::__Load()
	{
		return OpenPef();
	}

	bool EquipmentContentsCrawler::__Gather()
	{
		i3ResourceFileObjectInfo2* key = GetPef()->Findi3RegKeyRoot();

		if (key == nullptr) return false;

		key = GetPef()->FindChildi3TreeElement("Equipment");
		if (key == nullptr) return false;

		i3::vector<i3ResourceFileObjectInfo2*> subCategoryInfoList;
		GetPef()->ReadChildListi3TreeElement(subCategoryInfoList);

		i3::rc_string strContentsName;
		i3::rc_string strContentsSubCatName;
		i3::vector<i3ResourceFileObjectInfo2*> contentsInfoList;

		for (auto &subCategoryInfo : subCategoryInfoList)
		{
			GetPef()->ReadNamei3NamedElement(subCategoryInfo, strContentsSubCatName);

			contentsInfoList.clear();
			GetPef()->ReadChildListi3TreeElement(contentsInfoList);

			for (auto &contentsInfo : contentsInfoList)
			{
				if (contentsInfo == nullptr)
					continue;

				GetPef()->ReadNamei3NamedElement(contentsInfo, strContentsName);

				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), strContentsName.c_str(), "Equipment", strContentsSubCatName.c_str());
			}
		}

		return true;
	}

	bool EquipmentContentsCrawler::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Equipment");

		return true;
	}

	bool EquipmentContentsCrawler::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Equipment");

		return true;
	}
}