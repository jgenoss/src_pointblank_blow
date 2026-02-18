#include "stdafx.h"
#include "FileCrawlerFactory.h"
#include "WeaponFileCrawler.h"
#include "EquipmentFileCrawler.h"
#include "CompatiblePackScriptsCrawler.h"
#include "WorldFileCrawler.h"
#include "UIFileCrawler.h"
#include "CharacterFileCrawler.h"
#include "GoodsFileCrawler.h"

namespace rsc
{
	FileCrawlerFactory::FileCrawlerFactory()
	{
		Regist(std::make_pair("v1", "Weapon"),		WeaponFileCrawler::Creator_V1);
		Regist(std::make_pair("v1", "Equipment"),	EquipmentFileCrawler::Creator_V1);
		Regist(std::make_pair("v1", "World"),		WorldFileCrawler::Creator_V1);
		Regist(std::make_pair("v1", "UI"),			UIFileCrawler::Creator_V1);
		Regist(std::make_pair("v1", "Character"),	CharacterFileCrawler::Creator_V1);
		Regist(std::make_pair("v1", "Goods"),		GoodsFileCrawler::Creator_V1);

		Regist(std::make_pair("v2", "Weapon"),		WeaponFileCrawler::Creator_V2);
		Regist(std::make_pair("v2", "Equipment"),	EquipmentFileCrawler::Creator_V2);
		Regist(std::make_pair("v2", "World"),		WorldFileCrawler::Creator_V2);
		Regist(std::make_pair("v2", "UI"),			UIFileCrawler::Creator_V2);
		Regist(std::make_pair("v2", "Character"),	CharacterFileCrawler::Creator_V2);
		Regist(std::make_pair("v2", "Goods"),		GoodsFileCrawler::Creator_V2);

		Regist(std::make_pair("v3", "Weapon"),		WeaponFileCrawler::Creator_V3);
		Regist(std::make_pair("v3", "Equipment"),	EquipmentFileCrawler::Creator_V3);
		Regist(std::make_pair("v3", "World"),		WorldFileCrawler::Creator_V3);
		Regist(std::make_pair("v3", "UI"),			UIFileCrawler::Creator_V3);
		Regist(std::make_pair("v3", "Character"),	CharacterFileCrawler::Creator_V3);
		Regist(std::make_pair("v3", "Goods"),		GoodsFileCrawler::Creator_V3);
	}

	FileCrawlerFactory::~FileCrawlerFactory() 
	{}



	CompatiblePackScriptsCrawlerFactory::CompatiblePackScriptsCrawlerFactory()
	{
		Regist("v1", CompatiblePackScriptsCrawler::Creator_V1);
		Regist("v2", CompatiblePackScriptsCrawler::Creator_V2);
		Regist("v3", CompatiblePackScriptsCrawler::Creator_V3);
	}

	CompatiblePackScriptsCrawlerFactory::~CompatiblePackScriptsCrawlerFactory() 
	{}
}