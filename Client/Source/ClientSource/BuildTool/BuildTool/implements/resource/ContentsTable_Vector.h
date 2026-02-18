#pragma once

#include "IContentsTable.h"

namespace rsc
{
	/*class ContentsDBImpl_Vector final : public IContentsDBImpl
	{
	public:
		virtual ~ContentsDBImpl_Vector() {}

		virtual bool				Insert(const std::string& name,
										   const std::string& category,
										   const std::string& subCategory,
										   const std::bitset<ContentsState::MAX_COUNT>& state) override;

		virtual bool				Delete(const std::string& name) override;

		virtual void				FindNamesByCategory(const std::string& category, std::vector<std::string>& outNames) override;
		virtual void				FindNamesBySubCategory(const std::string& subCategory, std::vector<std::string>& outNames) override;
		virtual void				FindNamesByState(const std::bitset<ContentsState::MAX_COUNT>& state, std::vector<std::string>& outNames) override;

		virtual bool				SetState(const std::string& name, const std::bitset<ContentsState::MAX_COUNT>& state) override;

		virtual const ContentsData& GetContentsData(const std::string& name) const override;
		virtual void				GetAllContentsData(std::vector<const ContentsData*>& outList) const override;

		virtual size_t				GetSize() const override;

	private:
		using VectorType = std::vector < std::unique_ptr<ContentsData> >;
		VectorType m_dataList;
	};*/
}