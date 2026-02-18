#pragma once

#include "util/ObjectFactory.h"
#include "util/Creator.h"
#include "PackSync_Null.h"
#include "PackSync_Weapon.h"
#include "PackSync_Equipment.h"
#include "PackSync_World.h"
#include "PackSync_Character.h"
#include "PackSync_ShopGoods.h"

namespace rsc
{
	class PackSync_Factory final :

		public FlyweightFactory<IPackSync, 
								std::pair<std::string, std::string>, 
								PackSync_Null>,		// 미등록 대상에게는 팩스크립트만 이용하여 패킹.
		
		public i3::shared_ginst<PackSync_Factory>
	{
	public:
		PackSync_Factory();

		bool GetCategoryNamesByVer(const std::string& strVer, std::vector<std::string>& out) const;

	private:
		void _Regist(const std::pair<std::string, std::string>& key, rsc::IPackSync* (*creator)());
		void _MakeUnique();

		struct CategoriesByVer
		{
			std::string strVer;
			std::vector<std::string> strCategories;
		};
		using CategoriesByVerPtr = std::shared_ptr<CategoriesByVer>;

		std::vector<CategoriesByVerPtr> m_categoriesByVer;
	};
}