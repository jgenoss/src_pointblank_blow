#pragma once

#include "IContentsTable.h"
#include "jsoncpp/json.h"

namespace rsc
{
	//class ContentsTable_json final : public IContentsTable	// json 포맷 파일 DB. 테스트 용도.
	//{
	//public:
	//	ContentsTable_json(const std::string& name);
	//	virtual ~ContentsTable_json();

	//	virtual bool				Insert(const std::string& name,
	//									   const std::string& category,
	//									   const std::string& subCategory,
	//									   const ContentsStateFlag& state) override;

	//	virtual bool				Delete(const std::string& name) override { return false; }
	//	virtual void				Clear() override {}

	//	virtual bool				FindIdx(const std::string& name, size_t& idx) override { return true; }
	//	virtual void				FindNamesByCategory(const std::string& category, std::vector<std::string>& outNames) override {}
	//	virtual void				FindNamesBySubCategory(const std::string& subCategory, std::vector<std::string>& outNames) override {}
	//	virtual void				FindNamesByState(const ContentsStateFlag& state, std::vector<std::string>& outNames) override {}

	//	virtual bool				SetState(const std::string& name, const ContentsStateFlag& state) override { return false; }

	//	virtual ContentsDataPtr		GetContentsData(const std::string& name) const override { return std::make_shared<ContentsData>(); }
	//	virtual ContentsDataPtr		GetContentsData(const size_t idx) const override { return std::make_shared<ContentsData>(); }
	//	virtual void				GetAllContentsData(std::vector<ContentsDataPtr>& outList) const override {}
	//	virtual void				GetAllContentsDataByCategory(const std::string& category, std::vector<ContentsDataPtr>& outList) const override {}

	//	virtual size_t				GetSize() const override { return m_numberOfContents; }

	//private:
	//	FILE* m_file = nullptr;
	//	size_t m_numberOfContents = 0;
	//};
}