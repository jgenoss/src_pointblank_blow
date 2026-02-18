#include "stdafx.h"
#include "CharacterContentsCrawler_V1.h"
#include "ContentsCrawlerCallback.h"
#include <array>

namespace rsc
{
	CharacterContentsCrawler_V1::CharacterContentsCrawler_V1(const Path& pefWorkdir, const Path& pefRelPath)
		: IPefable(pefWorkdir, pefRelPath)
	{}

	CharacterContentsCrawler_V1::~CharacterContentsCrawler_V1()
	{}

	bool CharacterContentsCrawler_V1::__Load()
	{
		return OpenPef();
	}

	bool CharacterContentsCrawler_V1::__Gather()
	{
		i3ResourceFileObjectInfo2* key = GetPef()->Findi3RegKeyRoot();
		if (key == nullptr) return false;

		key = GetPef()->FindChildi3TreeElement("Character");
		if (key == nullptr) return false;

		i3::vector<i3ResourceFileObjectInfo2*> characterInfoList;
		GetPef()->ReadChildListi3TreeElement(characterInfoList);

		i3::rc_string characterName;
		i3::rc_string subCategoryName;
		i3::vector<i3ResourceFileObjectInfo2*> contentsInfoList;

		for (auto &characterInfo : characterInfoList)
		{
			GetPef()->ReadNamei3NamedElement(characterInfo, characterName);

			if (GetCallback())
			{
				subCategoryName = (characterName.find("Dino") != i3::string::npos) ? "Dinosaur": "Human";
				
				GetCallback()->OnGather(IContentsCrawler::GetCountryName(), characterName.c_str(), "Character", subCategoryName.c_str());
			}
		}

		return true;
	}

	bool CharacterContentsCrawler_V1::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Character");

		return true;
	}

	bool CharacterContentsCrawler_V1::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Character");

		return true;
	}
}