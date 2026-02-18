#include "stdafx.h"
#include "WeaponContentsCrawler.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	WeaponContentsCrawler::WeaponContentsCrawler(const Path& PEFWorkdir, const Path& PEFRelPath)
		: IPefable(PEFWorkdir, PEFRelPath)
	{}

	WeaponContentsCrawler::~WeaponContentsCrawler() 
	{}

	bool WeaponContentsCrawler::__Load()
	{
		return OpenPef();
	}

	bool WeaponContentsCrawler::__Gather()
	{
		i3ResourceFileObjectInfo2* key = GetPef()->Findi3RegKeyRoot();

		if (key == nullptr) return false;

		key = GetPef()->FindChildi3TreeElement("WeaponList");
		if (key == nullptr) return false;

		key = GetPef()->FindChildi3TreeElement(key, "Weapon");
		if (key == nullptr) return false;

		i3::vector<i3ResourceFileObjectInfo2*> weaponSubcategoryInfoList;
		GetPef()->ReadChildListi3TreeElement(weaponSubcategoryInfoList);

		i3::rc_string strWeaponName;
		i3::rc_string strWeaponSubcategoryName;
		i3::vector<i3ResourceFileObjectInfo2*> weaponInfoList;

		for (auto &weaponSubcategoryInfo : weaponSubcategoryInfoList)
		{
			GetPef()->ReadNamei3NamedElement(weaponSubcategoryInfo, strWeaponSubcategoryName);

			weaponInfoList.clear();
			GetPef()->ReadChildListi3TreeElement(weaponInfoList);

			for (auto &weaponInfo : weaponInfoList)
			{
				if (weaponInfo == nullptr)
					continue;

				GetPef()->ReadNamei3NamedElement(weaponInfo, strWeaponName);

				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), strWeaponName.c_str(), "Weapon", strWeaponSubcategoryName.c_str());
			}
		}

		return true;
	}

	bool WeaponContentsCrawler::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Weapon");

		return true;
	}

	bool WeaponContentsCrawler::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Weapon");

		return true;
	}
}