#include "stdafx.h"
#include "PackSync_Factory.h"

namespace rsc
{
	PackSync_Factory::PackSync_Factory()
	{
		// 컨텐츠 카테고리별 패킹 수행 객체를 등록. 
		_Regist(std::make_pair("v1", "Weapon"), Creator<IPackSync, PackSync_Weapon_V1>::Call);
		_Regist(std::make_pair("v2", "Weapon"), Creator<IPackSync, PackSync_Weapon_V2>::Call);
		_Regist(std::make_pair("v3", "Weapon"), Creator<IPackSync, PackSync_Weapon_V3>::Call);

		_Regist(std::make_pair("v1", "Equipment"), Creator<IPackSync, PackSync_Equipment>::Call);
		_Regist(std::make_pair("v2", "Equipment"), Creator<IPackSync, PackSync_Avatar>::Call);
		_Regist(std::make_pair("v3", "Equipment"), Creator<IPackSync, PackSync_Avatar>::Call);

		_Regist(std::make_pair("v1", "World"), Creator<IPackSync, PackSync_World_V1>::Call);
		_Regist(std::make_pair("v2", "World"), Creator<IPackSync, PackSync_World_V2>::Call);
		_Regist(std::make_pair("v3", "World"), Creator<IPackSync, PackSync_World_V3>::Call);

		_Regist(std::make_pair("v1", "Character"), Creator<IPackSync, PackSync_Character_V1>::Call);
		_Regist(std::make_pair("v2", "Character"), Creator<IPackSync, PackSync_Character_V2>::Call);
		_Regist(std::make_pair("v3", "Character"), Creator<IPackSync, PackSync_Character_V2>::Call);

		_Regist(std::make_pair("v1", "Goods"), Creator<IPackSync, PackSync_ShopGoods>::Call);

		_MakeUnique();
	}

	bool PackSync_Factory::GetCategoryNamesByVer(const std::string& strVer, std::vector<std::string>& out) const
	{
		auto it = std::find_if(m_categoriesByVer.begin(), m_categoriesByVer.end(),
			[&strVer](const CategoriesByVerPtr& src)
		{
			return i3::generic_is_iequal(strVer, src->strVer);
		}
		);

		if (it != m_categoriesByVer.end())
		{
			std::vector<std::string>& list = (*it)->strCategories;
			out.assign(list.begin(), list.end());
			return true;
		}

		return false;
	}

	void PackSync_Factory::_Regist(const std::pair<std::string, std::string>& key, rsc::IPackSync* (*creator)())
	{
		FlyweightFactory::Regist(key, creator);

		const std::string strVer = key.first;

		auto it = std::find_if(m_categoriesByVer.begin(), m_categoriesByVer.end(),
			[&strVer](const CategoriesByVerPtr& src)
		{
			return i3::generic_is_iequal(strVer, src->strVer);
		}
		);

		CategoriesByVerPtr ptr;
		if (it == m_categoriesByVer.end())
		{
			ptr = std::make_shared<CategoriesByVer>();
			ptr->strVer = strVer;
			ptr->strCategories.push_back(key.second);
			m_categoriesByVer.push_back(ptr);
		}
		else
		{
			ptr = *it;
			ptr->strCategories.push_back(key.second);
		}
	}

	void PackSync_Factory::_MakeUnique()
	{
		for (auto& ptr : m_categoriesByVer)
		{
			auto& categories = ptr->strCategories;
			categories.erase(std::unique(categories.begin(), categories.end()), categories.end());
		}
	}
}