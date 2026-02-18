#include "stdafx.h"
#include "CharacterContentsCrawler_V2.h"
#include "ContentsCrawlerCallback.h"
#include <array>

namespace rsc
{
	CharacterContentsCrawler_V2::CharacterContentsCrawler_V2(const Path& pefWorkdir, const Path& pefRelPath)
		: IPefable(pefWorkdir, pefRelPath)
	{}

	CharacterContentsCrawler_V2::~CharacterContentsCrawler_V2()
	{}

	bool CharacterContentsCrawler_V2::__Load()
	{
		return OpenPef();
	}

	bool CharacterContentsCrawler_V2::__Gather()
	{
		std::array<const char*, 2> arrCharacterTypeKeyName =
		{
			"Character", "Dino"
		};

		i3ResourceFileObjectInfo2* registryKey = nullptr;

		for (auto &charaKeyName : arrCharacterTypeKeyName)
		{
			registryKey = GetPef()->Findi3RegKeyRoot();
			if (registryKey == nullptr)
				continue;

			registryKey = GetPef()->FindChildi3TreeElement(charaKeyName);
			if (registryKey == nullptr)
				continue;

			i3::vector<i3ResourceFileObjectInfo2*> characterInfoList;
			GetPef()->ReadChildListi3TreeElement(characterInfoList);

			i3::rc_string characterName;
			i3::vector<i3ResourceFileObjectInfo2*> contentsInfoList;

			for (auto &characterInfo : characterInfoList)
			{
				GetPef()->ReadNamei3NamedElement(characterInfo, characterName);

				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), characterName.c_str(), "Character", charaKeyName);
			}
		}

		return true;
	}

	bool CharacterContentsCrawler_V2::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "Character");

		return true;
	}

	bool CharacterContentsCrawler_V2::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "Character");

		return true;
	}
}