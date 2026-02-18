#pragma once

#include "ContentsDefines.h"
#include <functional>

namespace rsc
{
	class IContentsTableCallback
	{
	public:
		virtual ~IContentsTableCallback() = 0;

		virtual void OnInsert(const ContentsDataPtr& contentsData, const std::string& strTableName) = 0;
		virtual void OnDelete(const ContentsDataPtr& contentsData, const std::string& strTableName) = 0;
		virtual void OnUpdate(const ContentsDataPtr& contentsData, const std::string& strTableName) = 0;
	};

	class ContentsTableCallback_Null : public IContentsTableCallback
	{
	public:
		virtual ~ContentsTableCallback_Null() {}

		virtual void OnInsert(const ContentsDataPtr& contentsData, const std::string& strTableName) override {}
		virtual void OnDelete(const ContentsDataPtr& contentsData, const std::string& strTableName) override {}
		virtual void OnUpdate(const ContentsDataPtr& contentsData, const std::string& strTableName) override {}
	};

	class ContentsTableCallback : public IContentsTableCallback
	{
	public:
		using InsertFuncType = std::function < void(const ContentsDataPtr& contentsData, const std::string& strTableName) >;
		using DeleteFuncType = std::function < void(const ContentsDataPtr& contentsData, const std::string& strTableName) >;
		using UpdateFuncType = std::function < void(const ContentsDataPtr& contentsData, const std::string& strTableName) >;

		ContentsTableCallback(const InsertFuncType& f1, const DeleteFuncType& f2, const UpdateFuncType& f3);
		virtual ~ContentsTableCallback() {}

		virtual void OnInsert(const ContentsDataPtr& contentsData, const std::string& strTableName) override;
		virtual void OnDelete(const ContentsDataPtr& contentsData, const std::string& strTableName) override;
		virtual void OnUpdate(const ContentsDataPtr& contentsData, const std::string& strTableName) override;

	private:
		InsertFuncType m_insertCallback;
		DeleteFuncType m_deleteCallback;
		UpdateFuncType m_updateCallback;
	};
}