#include "stdafx.h"
#include "ContentsCrawlerFactory.h"
#include "WeaponContentsCrawler.h"
#include "EquipmentContentsCrawler.h"
#include "WorldContentsCrawler.h"
#include "UIContentsCrawler_V2_UIRe.h"
#include "CharacterContentsCrawler_V1.h"
#include "CharacterContentsCrawler_V2.h"
#include "GoodsContentsCrawler.h"
#include "Settings.h"
#include "UIContentsCrawler_V3.h"
#include "ShaderContentsCrawler.h"

namespace rsc
{
	ContentsCrawlers* ContentsCrawlerCreator_V1(const Path& workdir, const std::string& strCountryName)
	{
		ContentsCrawlers* p = new ContentsCrawlers;

		p->list.push_back(std::make_shared<WeaponContentsCrawler>(workdir, "config/Weapon.pef"));
		p->list.push_back(std::make_shared<EquipmentContentsCrawler>(workdir, "config/Equipment.pef"));
		p->list.push_back(std::make_shared<WorldContentsCrawler>(workdir, "config/Stage.pef"));
		p->list.push_back(std::make_shared<CharacterContentsCrawler_V1>(workdir, "config/Character.pef"));
		p->list.push_back(std::make_shared<GoodsContentsCrawler>(workdir, "config/Goods.pef"));
		p->list.push_back(std::make_shared<ShaderContentsCrawler>());

		return p;
	}

	ContentsCrawlers* ContentsCrawlerCreator_V2(const Path& workdir, const std::string& strCountryName)
	{
		ContentsCrawlers* p = new ContentsCrawlers;

		p->list.push_back(std::make_shared<WeaponContentsCrawler>(workdir, "config/Weapon.pef"));
		p->list.push_back(std::make_shared<EquipmentContentsCrawler>(workdir, "config/Equipment_Backup.pef"));
		p->list.push_back(std::make_shared<WorldContentsCrawler>(workdir, "config/Stage.pef"));
		p->list.push_back(std::make_shared<CharacterContentsCrawler_V2>(workdir, "config/Character.pef"));
		p->list.push_back(std::make_shared<UIContentsCrawler_V2_UIRe>(workdir, "UIRe\\"));
		p->list.push_back(std::make_shared<GoodsContentsCrawler>(workdir, "config/Goods.pef"));
		p->list.push_back(std::make_shared<ShaderContentsCrawler>());

		return p;
	}

	ContentsCrawlers* ContentsCrawlerCreator_V3(const Path& workdir, const std::string& strCountryName)
	{
		ContentsCrawlers* p = new ContentsCrawlers;

		p->list.push_back(std::make_shared<WeaponContentsCrawler>(workdir, "config/Weapon.pef"));
		p->list.push_back(std::make_shared<CharacterContentsCrawler_V2>(workdir, "config/Character.pef"));
		p->list.push_back(std::make_shared<WorldContentsCrawler>(workdir, "config/Stage.pef"));
		p->list.push_back(std::make_shared<EquipmentContentsCrawler>(workdir, "config/Equipment.pef"));
		p->list.push_back(std::make_shared<GoodsContentsCrawler>(workdir, "config/Goods.pef"));

		const CountryDataPtr countryData = CountrySettings::i()->FindData(strCountryName);
		std::vector<Path> UIFolderRelPathList;

		if (countryData->IsEnableV3UI_1_0())
			UIFolderRelPathList.push_back("UI_V10\\");

		if (countryData->IsEnableV3UI_1_1())
			UIFolderRelPathList.push_back("UI_V11\\");

		if (countryData->IsEnableV3UI_1_5())
			UIFolderRelPathList.push_back("UI_V15\\");

		p->list.push_back(std::make_shared<UIContentsCrawler_V3>(workdir, UIFolderRelPathList));

		p->list.push_back(std::make_shared<ShaderContentsCrawler>());

		return p;
	}

	ContentsCrawlerFactory::ContentsCrawlerFactory()
	{
		Regist("v1", ContentsCrawlerCreator_V1);
		Regist("v2", ContentsCrawlerCreator_V2);
		Regist("v3", ContentsCrawlerCreator_V3);
	}

	ContentsCrawlerFactory::~ContentsCrawlerFactory() 
	{}
}