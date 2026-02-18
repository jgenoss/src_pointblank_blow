#pragma once

#include "IContentsTable.h"
#include <map>

namespace rsc
{
	class ContentsTable_Map final : public IContentsTable
	{
	public:
		ContentsTable_Map() = default;
		ContentsTable_Map(const std::string& name);
		virtual ~ContentsTable_Map() {}

		virtual bool	Insert(const std::string& strContentsName, const std::string& strCategoryName, 
							   const std::string& strSubcategoryName, const ContentsStatusFlag& status) override;

		virtual bool	Delete(const std::string& strContentsName) override;

		virtual void	Clear() override;

		virtual bool	GetAllContentsDatas(std::vector<ContentsDataPtr>& outList) const override;
		virtual bool	GetContentsDatas(const std::string& strContentsName, std::vector<ContentsDataPtr>& outList) const override;
		virtual bool	GetContentsDatasByCategory(const std::string& strCategoryName, std::vector<ContentsDataPtr>& outList) const override;
		virtual bool	GetContentsDatasBySubCategory(const std::string& strSubcategoryName, std::vector<ContentsDataPtr>& outList) const override;

		virtual bool	SetContentsStatus(const std::string& strContentsName, const std::string& strCategoryName,
										  const std::string& strSubCategoryName, const ContentsStatusFlag& status) override;

		virtual size_t	GetSize() const override;

	private:
		using ContentsDataPtrList = std::vector<std::shared_ptr<ContentsData>>;
		using MapTableType = std::map < std::string, std::shared_ptr<ContentsDataPtrList> >;
		MapTableType m_tbl;
	};
}