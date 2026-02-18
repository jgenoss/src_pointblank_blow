#include "stdafx.h"
#include "CustomUtil.h"
#include <boost/tokenizer.hpp>

void RemoveDuplicates(std::vector<PathPtr>& inoutPathPtrList)
{
	struct fn_sort : std::unary_function<PathPtr, bool>
	{
		bool operator()(const PathPtr& lhsPtr, const PathPtr& rhsPtr) const
		{
			return lhsPtr->string() < rhsPtr->string();
		}
	};

	struct fn_equal_to : std::unary_function<PathPtr, bool>
	{
		bool operator()(const PathPtr& lhsPtr, const PathPtr& rhsPtr) const
		{
			return i3::generic_is_iequal(lhsPtr->string(), rhsPtr->string());
		}
	};

	std::sort(inoutPathPtrList.begin(), inoutPathPtrList.end(), fn_sort());
	inoutPathPtrList.erase(std::unique(inoutPathPtrList.begin(), inoutPathPtrList.end(), fn_equal_to()), inoutPathPtrList.end());
}

bool ConvertBufferToLineTexts(const std::vector<BYTE>& buff, std::vector<std::string>& out)
{
	if (buff.empty())
		return false;

	std::string strText(buff.begin(), buff.end());

	using tok = boost::tokenizer < boost::char_separator<char> >;
	boost::char_separator<char> sep("\n");

	tok tokens(strText, sep);

	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
	{
		out.push_back(*it);
	}

	return true;
}

bool ConvertBufferToLineTexts(const std::vector<BYTE>& buff, std::vector<std::wstring>& out)
{
	std::vector<std::string> strTextList;
	bool ok = ConvertBufferToLineTexts(buff, strTextList);

	std::wstring wstrText;
	for (auto &strText : strTextList)
	{
		i3::mb_to_utf16(strText, wstrText);
		out.push_back(wstrText);
	}

	return ok;
}

void ConvertLineTextsToBuffer(const std::vector<std::wstring>& in, std::vector<BYTE>& out)
{
	std::wstring wstr;
	for (const auto& text : in)
	{
		wstr += text;
		wstr += L"\n";
	}

	std::string str;
	i3::utf16_to_mb(wstr, str);

	std::vector<BYTE>().swap(out);
	std::copy(str.begin(), str.end(), std::back_inserter(out));
	out.push_back('\0');
}

void ConvertLineTextsToBuffer(const std::vector<std::string>& in, std::vector<BYTE>& out)
{
	if (in.empty())
		return;

	std::vector<std::wstring> wstrTexts;
	std::wstring wstr;
	for (auto& text : in)
	{
		if (text.empty())
		{
			wstrTexts.push_back(L"");
			continue;
		}

		i3::mb_to_utf16(text, wstr);
		wstrTexts.push_back(wstr);
	}

	ConvertLineTextsToBuffer(wstrTexts, out);
}

void RemoveNullTerminates(std::vector<BYTE>& inout)
{
	inout.erase(std::remove_if(inout.begin(), inout.end(),
		[](const BYTE src) -> bool
	{
		return src == '\0';
	}),
		inout.end());
}