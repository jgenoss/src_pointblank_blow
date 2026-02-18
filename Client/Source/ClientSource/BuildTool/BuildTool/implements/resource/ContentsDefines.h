#pragma once

#include <bitset>

namespace rsc
{
	enum ContentsStatus
	{
		NOT_EXISTS_IN_MEDIA,
		NOT_EXISTS_IN_QA,
		NOT_EXISTS_IN_LIVE,
		DIFF_MEDIA_QA,
		DIFF_QA_LIVE,
		SYNC_OK,
		MAX_COUNT,
	};

	using ContentsStatusFlag = std::bitset < ContentsStatus::MAX_COUNT > ;

	struct ContentsData
	{
		std::string				strContentsName;
		std::string				strCategoryName;
		std::string				strSubcategoryName;
		ContentsStatusFlag		status;

		bool IsSameType(const ContentsData& src);
		bool IsSameType(const std::string& strContentsName, const std::string& strCategoryName, const std::string& strSubCategoryName);
	};

	bool operator==(const ContentsData& src, const ContentsData& dst);

	using ContentsDataPtr = std::shared_ptr < ContentsData >;

	const wchar_t* ConvertContentsStatusToString(const rsc::ContentsStatusFlag& status);
}