#include "stdafx.h"
#include "WorldContentsCrawler.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	WorldContentsCrawler::WorldContentsCrawler(const Path& workdir, const Path& PEFRelPath)
		: IPefable(workdir, PEFRelPath)
	{}

	WorldContentsCrawler::~WorldContentsCrawler()
	{}

	bool WorldContentsCrawler::__Load()
	{
		return OpenPef();
	}

	bool WorldContentsCrawler::__Gather()
	{
		i3ResourceFileObjectInfo2* key = GetPef()->Findi3RegKeyRoot();

		if (key == nullptr) return false;

		key = GetPef()->FindChildi3TreeElement("Stage");
		if (key == nullptr) return false;

		i3::vector<i3ResourceFileObjectInfo2*> stageInfoList;
		GetPef()->ReadChildListi3TreeElement(stageInfoList);

		i3::rc_string strStageName;

		for (auto &stageInfo : stageInfoList)
		{
			GetPef()->ReadNamei3NamedElement(stageInfo, strStageName);

			if (i3::generic_is_iequal("_N/A", strStageName))
				continue;

			if (GetCallback())
				GetCallback()->OnGather(IContentsCrawler::GetCountryName(), strStageName.c_str(), "World", "Map");
		}

		return true;
	}

	bool WorldContentsCrawler::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "World");

		return true;
	}

	bool WorldContentsCrawler::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "World");

		return true;
	}
}