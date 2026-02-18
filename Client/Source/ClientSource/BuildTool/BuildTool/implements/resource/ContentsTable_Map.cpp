#include "stdafx.h"
#include "ContentsTable_Map.h"
#include "ContentsTableCallback.h"
#include "ContentsDB.h"

namespace rsc
{
	ContentsTable_Map::ContentsTable_Map(const std::string& strName) : IContentsTable(strName)
	{
		SetCallback(std::make_shared<ContentsTableCallback_Null>());
	}

	bool ContentsTable_Map::Insert(const std::string& strContentsName,
								   const std::string& strCategoryName,
								   const std::string& strSubcategoryName,
								   const ContentsStatusFlag& status)
	{
		auto it = m_tbl.find(strContentsName);
		if (it == m_tbl.end())
		{
			auto pr = m_tbl.insert(MapTableType::value_type(strContentsName, std::make_shared<ContentsDataPtrList>()));
			if (pr.second == false)
				return false;

			it = pr.first;
		}

		std::shared_ptr<ContentsData> data = std::make_shared<ContentsData>();
		data->strContentsName = strContentsName;
		data->strCategoryName = strCategoryName;
		data->strSubcategoryName = strSubcategoryName;
		data->status = status;

		std::shared_ptr<ContentsDataPtrList>& list = it->second;
		list->push_back(data);

		GetCallback()->OnInsert(data, GetName());

		return true;
	}

	bool ContentsTable_Map::Delete(const std::string& strContentsName)
	{
		auto it = m_tbl.find(strContentsName);
		if (it == m_tbl.end())
			return false;

		std::shared_ptr<ContentsDataPtrList>& list = it->second;

		m_tbl.erase(it);

		for (auto &currContentsData : *list)
		{
			GetCallback()->OnDelete(currContentsData, GetName());
		}

		return true;
	}

	void ContentsTable_Map::Clear()
	{
		m_tbl.clear();
	}

	bool ContentsTable_Map::GetAllContentsDatas(std::vector<ContentsDataPtr>& outList) const
	{
		for (auto &it : m_tbl)
		{
			const std::shared_ptr<ContentsDataPtrList>& list = it.second;

			for (auto &currContentsData : *list)
				outList.push_back(currContentsData);
		}

		return !outList.empty();
	}

	bool ContentsTable_Map::GetContentsDatas(const std::string& strContentsName, std::vector<ContentsDataPtr>& outList) const
	{
		auto it = m_tbl.find(strContentsName);
		if (it == m_tbl.end())
			return false;

		const std::shared_ptr<ContentsDataPtrList>& list = it->second;

		for (auto &currContentsData : *list)
			outList.push_back(currContentsData);

		return !list->empty();
	}

	bool ContentsTable_Map::GetContentsDatasByCategory(const std::string& strCategoryName, std::vector<ContentsDataPtr>& outList) const
	{
		for (auto &it : m_tbl)
		{
			const std::shared_ptr<ContentsDataPtrList>& list = it.second;
			for (auto &currContentsData : *list)
			{
				if (currContentsData->strCategoryName == strCategoryName)
					outList.push_back(currContentsData);
			}
		}

		return !outList.empty();
	}

	bool ContentsTable_Map::GetContentsDatasBySubCategory(const std::string& strSubcategoryName, std::vector<ContentsDataPtr>& outList) const
	{
		for (auto &it : m_tbl)
		{
			const std::shared_ptr<ContentsDataPtrList>& list = it.second;
			for (auto &currContentsData : *list)
			{
				if (currContentsData->strSubcategoryName == strSubcategoryName)
					outList.push_back(currContentsData);
			}
		}

		return !outList.empty();
	}

	bool ContentsTable_Map::SetContentsStatus(const std::string& strContentsName, 
											  const std::string& strCategoryName, 
											  const std::string& strSubCategoryName, 
											  const ContentsStatusFlag& status)
	{
		auto it = m_tbl.find(strContentsName);
		if (it == m_tbl.end())
			return false;

		bool ok = false;

		const std::shared_ptr<ContentsDataPtrList>& list = it->second;
		for (auto &currContentsData : *list)
		{
			if (currContentsData->IsSameType(strContentsName, strCategoryName, strSubCategoryName))
			{
				currContentsData->status = status;
				ok = true;

				GetCallback()->OnUpdate(currContentsData, GetName());
			}
		}

		return ok;
	}

	size_t ContentsTable_Map::GetSize() const
	{
		return m_tbl.size();
	}
}