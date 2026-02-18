#include "i3CommonType.h"
#include "i3Base/string/system/collect_directoryname.h"
#include "i3Base/string/system/traverse_file.h"
#include "i3Base/string/system/is_directory.h"

namespace i3
{
	namespace
	{
		struct collect_directory_mb : i3::traverse_file_callback_mb
		{
			i3::vector<i3::rc_string>& out_vecSubDir;
		
			collect_directory_mb(i3::vector<i3::rc_string>& out_vecSubDir)
				: out_vecSubDir(out_vecSubDir) {}

			virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
			{
			}

			virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
			{
				out_vecSubDir.push_back(fd.cFileName);
				return true;
			}
		};

		struct collect_directory_wc : i3::traverse_file_callback_wc
		{
			i3::vector<i3::rc_wstring>& out_vecSubDir;
			collect_directory_wc(i3::vector<i3::rc_wstring>& out_vecSubDir)
				: out_vecSubDir(out_vecSubDir) {}

			virtual void on_file(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
			{
			}

			virtual bool on_dir(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
			{
				out_vecSubDir.push_back(fd.cFileName);
				return true;
			}
		};

	}


	bool collect_directoryname(const i3::literal_range& strDir, i3::vector<i3::rc_string>& out_vecSubDir)
	{
		if (i3::is_directory(strDir) == false)
			return false;

		collect_directory_mb  cb(out_vecSubDir);

		i3::traverse_file(strDir, &cb, false);
		return true;
	}

	bool collect_directoryname(const i3::wliteral_range& strDir, i3::vector<i3::rc_wstring>& out_vecSubDir)
	{
		if (i3::is_directory(strDir) == false)
			return false;

		collect_directory_wc  cb(out_vecSubDir);

		i3::traverse_file(strDir, &cb, false);
		return true;
	}
}
