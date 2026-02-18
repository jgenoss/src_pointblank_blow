#include "i3CommonType.h"
#include "i3Base/string/system/delete_directory.h"
#include "i3Base/string/system/collect_directoryname_and_filename.h"

namespace
{

	bool remove_directory(const char* szDir) { return ::RemoveDirectoryA(szDir) != FALSE; }
	bool remove_directory(const wchar_t* szDir) { return ::RemoveDirectoryW(szDir) != FALSE; }

	bool remove_file(const char* szPath) { return ::DeleteFileA(szPath) != FALSE; }
	bool remove_file(const wchar_t* szPath) { return ::DeleteFileW(szPath) != FALSE; }

	template<class Ch>
	bool delete_directory_imp(const i3::literal_base_range<const Ch*>& strDir,
		const i3::vector<i3::rc_basic_string<Ch> >& vecSubDir,
		const i3::vector<i3::rc_basic_string<Ch> >& vecFilename)
	{
		typedef i3::stack_basic_string<mpl::size_t<128>, Ch>		stack_string_type;

		stack_string_type strFullPath;

		const size_t numFile = vecFilename.size();

		for (size_t i = 0; i < numFile; ++i)
		{
			strFullPath = strDir;
			strFullPath += '/';
			strFullPath += vecFilename[i];
			remove_file(strFullPath.c_str());
		}

		const size_t numSubDir = vecSubDir.size();

		for (size_t i = 0; i < numSubDir; ++i)
		{
			strFullPath = strDir;
			strFullPath += Ch('/');
			strFullPath += vecSubDir[i];
			i3::delete_directory(strFullPath);			// 상위 재귀 호출임..
		}

		return remove_directory(strDir.c_str());
	}
}


namespace i3
{
	bool delete_directory(const i3::literal_range& strDir)
	{
		i3::vector<i3::rc_string> vecSubDir, vecFileName;
		if (i3::collect_directoryname_and_filename(strDir, vecSubDir, vecFileName) == false)
			return false;

		i3::literal_range strDirArg;
		i3::stack_string str_temp_dir;

		if (strDir[strDir.size()] != 0)
		{
			str_temp_dir = strDir;
			strDirArg = str_temp_dir;
		}
		else
		{
			strDirArg = strDir;
		}

		return delete_directory_imp(strDirArg, vecSubDir, vecFileName);
	}

	bool delete_directory(const i3::wliteral_range& strDir)
	{
		i3::vector<i3::rc_wstring> vecSubDir, vecFileName;
		if (i3::collect_directoryname_and_filename(strDir, vecSubDir, vecFileName) == false)
			return false;

		i3::wliteral_range strDirArg = strDir;
		i3::stack_wstring str_temp_dir;
		if (strDir[strDir.size()] != 0)
		{
			str_temp_dir = strDir;
			strDirArg = str_temp_dir;
		}
		else
		{
			strDirArg = strDir;
		}

		return delete_directory_imp(strDirArg, vecSubDir, vecFileName);
	}
}
