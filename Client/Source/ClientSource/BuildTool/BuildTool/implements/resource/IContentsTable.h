#pragma once

#include "ContentsDefines.h"

namespace rsc
{
	class IContentsTableCallback;
	using ContentsTableCallbackPtr = std::shared_ptr < IContentsTableCallback >;

	class IContentsTable
	{
	public:
		IContentsTable() = default;
		IContentsTable(const std::string& name);

		virtual ~IContentsTable() = 0;

		virtual bool				Insert(const std::string& strContentsName, 
										   const std::string& strCategoryName,
										   const std::string& strSubCategoryName,
										   const ContentsStatusFlag& status) = 0;
		virtual bool				Delete(const std::string& strContentsName) = 0;
		virtual void				Clear() = 0;

		virtual bool				GetAllContentsDatas(std::vector<ContentsDataPtr>& outList) const = 0;
		virtual bool				GetContentsDatas(const std::string& strContentsName, std::vector<ContentsDataPtr>& outList) const = 0;
		virtual bool				GetContentsDatasByCategory(const std::string& strCategoryName, std::vector<ContentsDataPtr>& outList) const = 0;
		virtual bool				GetContentsDatasBySubCategory(const std::string& strSubCategoryName, std::vector<ContentsDataPtr>& outList) const = 0;

		virtual bool				SetContentsStatus(const std::string& strContentsName, const std::string& strCategoryName, 
													  const std::string& strSubCategoryName, const ContentsStatusFlag& status) = 0;

		virtual size_t				GetSize() const = 0;
		
		void						SetName(const std::string& strName);
		const std::string&			GetName() const;

		void						SetCallback(ContentsTableCallbackPtr callback);
		ContentsTableCallbackPtr	GetCallback() const;

	private:
		std::string m_strName;
		ContentsTableCallbackPtr m_callback;
	};

	namespace ContentsTableStuff
	{
		void SearchContentsByKeyword(const IContentsTable& tbl, 
			const std::wstring& wstrKeyword, std::vector<ContentsDataPtr>& out);

		void SearchContentsByKeyword(const IContentsTable& tbl, 
			const std::wstring& wstrKeyword, const std::string& strCategory, std::vector<ContentsDataPtr>& out);
	}
}