#include "stdafx.h"
#include "ContentsTable_json.h"

namespace rsc
{
	/*ContentsTable_json::ContentsTable_json(const std::string& name) : IContentsTable(name)
	{
		if (m_file != nullptr) return;

		i3::stack_wstring fileName;
		i3::mb_to_utf16(GetName(), fileName);
		fileName += L".json";

		WIN32_FIND_DATA fd;
		HANDLE findHandle = ::FindFirstFile(fileName.c_str(), &fd);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			HANDLE fileHandle = ::CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			CloseHandle(fileHandle);
		}
		FindClose(findHandle);

		WCHAR currDir[MAX_PATH] = { 0, };
		GetCurrentDirectoryW(MAX_PATH, currDir);
		i3::stack_wstring filepath(currDir);
		filepath += L"\\";
		filepath += fileName;

		m_file = ::_wfsopen(filepath.c_str(), L"wb", _SH_DENYNO);
	}

	ContentsTable_json::~ContentsTable_json()
	{
		if (m_file)
			::fclose(m_file);  
	}

	bool ContentsTable_json::Insert(const std::string& name, const std::string& category, 
		const std::string& subCategory, const ContentsStateFlag& state)
	{
		if (m_file == nullptr) return false;

		Json::Value contents;
		contents.append(name.c_str());
		contents.append(category.c_str());
		contents.append(subCategory.c_str());
		contents.append(state.any());

		Json::StyledWriter writer;
		const std::string& output = writer.write(contents);
		
		fwrite(output.c_str(), 1, output.length(), m_file);

		++m_numberOfContents;

		return true;
	}*/
}