#include "stdafx.h"
#include "ContentsTable_Vector.h"

namespace rsc
{
	/*bool ContentsDBImpl_Vector::Insert(
		const std::string& name,
		const std::string& category,
		const std::string& subCategory,
		const std::bitset<ContentsState::MAX_COUNT>& state)
	{
		m_dataList.push_back(VectorType::value_type(std::make_unique<ContentsData>()));

		ContentsData* data = m_dataList.back().get();
		if (data != nullptr)
		{
			data->name = name;
			data->category = category;
			data->subCategory = subCategory;
			data->state = state;

			return true;
		}

		return false;
	}

	bool ContentsDBImpl_Vector::Delete(const std::string& name)
	{
		auto it = m_dataList.begin();
		for (; it != m_dataList.end(); ++it)
		{
			if ((*it)->name == name)
			{
				m_dataList.erase(it);
				return true;
			}
		}

		return false;
	}

	void ContentsDBImpl_Vector::FindNamesByCategory(const std::string& category, std::vector<std::string>& outNames)
	{
		auto it = m_dataList.begin();
		for (; it != m_dataList.end(); ++it)
		{
			if ((*it)->category == category)
				outNames.push_back((*it)->name);
		}
	}

	void ContentsDBImpl_Vector::FindNamesBySubCategory(const std::string& subCategory, std::vector<std::string>& outNames)
	{
		auto it = m_dataList.begin();
		for (; it != m_dataList.end(); ++it)
		{
			if ((*it)->subCategory == subCategory)
				outNames.push_back((*it)->name);
		}
	}

	void ContentsDBImpl_Vector::FindNamesByState(const std::bitset<ContentsState::MAX_COUNT>& state, std::vector<std::string>& outNames)
	{
		auto it = m_dataList.begin();
		for (; it != m_dataList.end(); ++it)
		{
			if ((*it)->state == state)
				outNames.push_back((*it)->name);
		}
	}

	bool ContentsDBImpl_Vector::SetState(const std::string& name, const std::bitset<ContentsState::MAX_COUNT>& state)
	{
		auto it = m_dataList.begin();
		for (; it != m_dataList.end(); ++it)
		{
			if ((*it)->name == name)
			{
				if ((*it)->state == state)
				{
					(*it)->state = state;
					return true;
				}
			}
		}

		return false;
	}

	const ContentsData& ContentsDBImpl_Vector::GetContentsData(const std::string& name) const
	{
		auto it = m_dataList.find(name);
		if (it != m_dataList.end())
			return *((it->second).get());

		return GetContentsNullData();
	}

	void ContentsDBImpl_Vector::GetAllContentsData(std::vector<const ContentsData*>& outList) const
	{
		for (auto &it : m_dataList)
			outList.push_back((it.second).get());
	}

	size_t ContentsDBImpl_Vector::GetSize() const
	{
		return m_dataList.size();
	}*/
}