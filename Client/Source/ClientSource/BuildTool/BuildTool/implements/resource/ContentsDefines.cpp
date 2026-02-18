#include "stdafx.h"
#include "ContentsDefines.h"

namespace rsc
{
	bool ContentsData::IsSameType(const ContentsData& src)
	{	
		return IsSameType(src.strContentsName, src.strCategoryName, src.strSubcategoryName);
	}

	bool ContentsData::IsSameType(const std::string& srcContentsName, 
								  const std::string& srcCategoryName, 
								  const std::string& srcSubCategoryName)
	{
		if (srcContentsName != strContentsName)			return false;
		if (srcCategoryName != strCategoryName)			return false;
		if (srcSubCategoryName != strSubcategoryName)	return false;

		return true;
	}

	bool operator==(const ContentsData& src, const ContentsData& dst)
	{
		if (src.strContentsName != dst.strContentsName)			return false;
		if (src.strCategoryName != dst.strCategoryName)			return false;
		if (src.strSubcategoryName != dst.strSubcategoryName)	return false;
		if (src.status != src.status)							return false;

		return true;
	}

	const wchar_t* ConvertContentsStatusToString(const rsc::ContentsStatusFlag& status)
	{
		if (status[rsc::ContentsStatus::SYNC_OK])
			return L"O";

		if (status.any())
			return L"X";

		return L"?";
	}
}