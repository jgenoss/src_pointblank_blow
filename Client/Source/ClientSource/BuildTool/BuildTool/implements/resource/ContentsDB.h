#pragma once

#include "ContentsTable_Map.h"
#include "ContentsTable_Null.h"

namespace rsc
{
	class IContentsTable;

	using ContentsTablePtr = std::shared_ptr < IContentsTable >;

	template <class ContentsTableType = ContentsTable_Map>
	class ContentsDB
	{
	public:
		ContentsDB() = default;
		ContentsDB(const std::string& name);
		virtual ~ContentsDB();

		bool				CreateTable(const std::string& strName);
		void				DeleteTable(const std::string& strName);
		ContentsTablePtr	FindTable(const std::string& strName);
		void				GetTableList(std::vector<ContentsTablePtr>& out);
		size_t				GetNumOfTable() const;
		void				Clear();

		void				SetName(const std::string& strName);
		const std::string&	GetName() const;

	private:
		std::string m_strName;

		using ContentsTableMapType = std::map < std::string, ContentsTablePtr >;
		ContentsTableMapType m_tableList;
	};

	template <class ContentsTableType>
	ContentsDB<ContentsTableType>::ContentsDB(const std::string& strName) : m_strName(strName)
	{}

	template <class ContentsTableType>
	ContentsDB<ContentsTableType>::~ContentsDB()
	{}

	template <class ContentsTableType>
	bool ContentsDB<ContentsTableType>::CreateTable(const std::string& strName)
	{
		auto it = m_tableList.find(strName);
		if (it == m_tableList.end())
		{
			auto pr = m_tableList.insert(
				ContentsTableMapType::value_type(strName, std::make_shared<ContentsTableType>(strName)));
			return pr.second;
		}

		return false;
	}

	template <class ContentsTableType>
	void ContentsDB<ContentsTableType>::DeleteTable(const std::string& strName)
	{
		auto it = m_tableList.find(strName);
		if (it != m_tableList.end())
		{
			m_tableList.erase(it);
		}
	}

	template <class ContentsTableType>
	std::shared_ptr<IContentsTable> ContentsDB<ContentsTableType>::FindTable(const std::string& strName)
	{
		auto it = m_tableList.find(strName);
		if (it != m_tableList.end())
		{
			return it->second;
		}

		return std::make_shared<ContentsTable_Null>();
	}

	template <class ContentsTableType>
	void ContentsDB<ContentsTableType>::Clear()
	{
		m_tableList.clear();
	}

	template <class ContentsTableType>
	void ContentsDB<ContentsTableType>::GetTableList(std::vector<ContentsTablePtr>& out)
	{
		for (auto &it : m_tableList)
		{
			out.push_back(it->second);
		}
	}

	template <class ContentsTableType>
	size_t ContentsDB<ContentsTableType>::GetNumOfTable() const
	{
		return m_tableList.size();
	}

	template <class ContentsTableType>
	void ContentsDB<ContentsTableType>::SetName(const std::string& strName)
	{
		m_strName = strName;
	}

	template <class ContentsTableType>
	const std::string& ContentsDB<ContentsTableType>::GetName() const
	{
		return m_strName;
	}
}