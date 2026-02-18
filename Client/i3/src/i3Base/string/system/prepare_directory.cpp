#include "i3CommonType.h"

#include "i3Base/string/system/prepare_directory.h"

#include "i3Base/string/system/is_directory.h"

#include "i3Base/string/ext/resolve_path_token.h"

namespace
{
	__forceinline bool CreateDirectoryImp(const char* str) { return ::CreateDirectoryA(str, NULL) == TRUE; }
	__forceinline bool CreateDirectoryImp(const wchar_t* str) { return ::CreateDirectoryW(str, NULL) == TRUE; }

	template<class Range>
	bool prepare_directory_imp(const Range& str_dir)
	{
		if (i3::is_directory(str_dir))		// 이미 존재하면 바로 out..
			return true;
		
		i3::vector<Range>	path_token_list;
		i3::resolve_path_token(str_dir, path_token_list);

		const size_t num_dir = path_token_list.size();

		typedef typename i3::range_value<const Range>::type Ch;
		typedef i3::stack_basic_string<mpl::size_t<128>, Ch>		stack_string_type;

		stack_string_type str_temp_dir;

		for (size_t i = 0; i < num_dir; ++i)
		{
			str_temp_dir += path_token_list[i];

			if (i3::is_directory(str_dir) == false)
			{
				if (CreateDirectoryImp(str_dir.c_str()) == FALSE)
					return false;
			}
			str_temp_dir += Ch('/');
		}

		return true;
	}
}

namespace i3
{
	bool prepare_directory(const i3::wliteral_range& str_dir)
	{
		return prepare_directory_imp(str_dir);
	}

	bool prepare_directory(const i3::literal_range& str_dir)
	{
		return prepare_directory_imp(str_dir);
	}
}
