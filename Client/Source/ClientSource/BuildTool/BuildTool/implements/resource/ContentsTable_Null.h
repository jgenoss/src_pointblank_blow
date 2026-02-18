#pragma once

#include "IContentsTable.h"

namespace rsc
{
	class ContentsTable_Null : public IContentsTable
	{
	public:
		ContentsTable_Null() : IContentsTable("NULL Contents Table") {}
		virtual ~ContentsTable_Null() {}

		virtual bool	Insert(const std::string& strName, const std::string& strCategoryName, 
							   const std::string& strSubCategoryName, const ContentsStatusFlag& status) override { return false; }

		virtual bool	Delete(const std::string& strName) override { return false; }
		
		virtual void	Clear() override {}

		virtual bool	GetAllContentsDatas(std::vector<ContentsDataPtr>& outList) const override { return false; };
		virtual bool	GetContentsDatas(const std::string& strContentsName, std::vector<ContentsDataPtr>& outList) const override { return false; }
		virtual bool	GetContentsDatasByCategory(const std::string& strCategoryName, std::vector<ContentsDataPtr>& outList) const override { return false; }
		virtual bool	GetContentsDatasBySubCategory(const std::string& strSubCategoryName, std::vector<ContentsDataPtr>& outList) const override { return false; }

		virtual bool	SetContentsStatus(const std::string& strContentsName, const std::string& strCategoryName,
										  const std::string& strSubCategoryName, const ContentsStatusFlag& status) override { return false; }
		
		virtual size_t	GetSize() const override { return 0; }
	};
}