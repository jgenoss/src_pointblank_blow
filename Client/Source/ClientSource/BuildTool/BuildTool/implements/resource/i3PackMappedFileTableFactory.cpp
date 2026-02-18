#include "stdafx.h"
#include "i3PackMappedFileTableFactory.h"
#include "ResourceController.h"
#include "util/Creator.h"

namespace rsc
{
	void i3PackMappedFileTableFactory::Buildup()
	{
		Clear();

		std::vector<CountryDataPtr> countryDataList;
		CountrySettings::i()->GetDataList(countryDataList);

		for (auto &currCountryData : countryDataList)
		{
			Regist(currCountryData->GetLiveWorkdir().string(), Creator< Ii3PackMappedFileTable, i3PackMappedFileTable >::Call);
		}
	}
}