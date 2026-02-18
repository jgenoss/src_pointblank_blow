#include "stdafx.h"
#include "IContentsTable.h"
#include "ContentsTableCallback.h"

namespace rsc
{
	IContentsTable::IContentsTable(const std::string& strName) : m_strName(strName) {}

	IContentsTable::~IContentsTable() {}

	void IContentsTable::SetName(const std::string& strName)
	{ 
		m_strName = strName;
	}

	const std::string& IContentsTable::GetName() const
	{ 
		return m_strName;
	}

	void IContentsTable::SetCallback(ContentsTableCallbackPtr cb)
	{
		m_callback = cb;
	}

	ContentsTableCallbackPtr IContentsTable::GetCallback() const
	{
		return m_callback;
	}

	namespace ContentsTableStuff
	{
		void SearchContents(const std::wstring& wstrKeyword, std::vector<ContentsDataPtr>& inContentsDataList, std::vector<ContentsDataPtr>& out)
		{
			std::wstring wstrKeywordUpper(wstrKeyword);
			std::transform(wstrKeywordUpper.begin(), wstrKeywordUpper.end(), wstrKeywordUpper.begin(), toupper);

			std::wstring wstrContentsNameUpper;
			for (auto &currContentsData : inContentsDataList)
			{
				wstrContentsNameUpper.assign(currContentsData->strContentsName.begin(), currContentsData->strContentsName.end());
				std::transform(wstrContentsNameUpper.begin(), wstrContentsNameUpper.end(), wstrContentsNameUpper.begin(), toupper);

				if (wstrContentsNameUpper.find(wstrKeywordUpper) != std::string::npos)
					out.push_back(currContentsData);
			}
		}

		void SearchContentsByKeyword(const IContentsTable& tbl, const std::wstring& wstrKeyword, std::vector<ContentsDataPtr>& out)
		{
			std::vector<ContentsDataPtr> contentsDataList;
			tbl.GetAllContentsDatas(contentsDataList);

			SearchContents(wstrKeyword, contentsDataList, out);
		}

		void SearchContentsByKeyword(const IContentsTable& tbl, const std::wstring& wstrKeyword, const std::string& strCategory, std::vector<ContentsDataPtr>& out)
		{
			std::vector<ContentsDataPtr> contentsDataList;
			tbl.GetContentsDatasByCategory(strCategory, contentsDataList);

			SearchContents(wstrKeyword, contentsDataList, out);
		}
	}
}