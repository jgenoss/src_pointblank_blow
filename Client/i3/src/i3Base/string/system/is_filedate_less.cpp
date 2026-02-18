#include "i3CommonType.h"
#include "i3Base/string/system/is_filedate_less.h"

namespace i3
{
	namespace detail
	{
		FILETIME getModFileTime(const char* szFullPath)
		{
			FILETIME ft;
			HANDLE h_fileNew = ::CreateFileA(szFullPath,
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			::GetFileTime(h_fileNew, NULL, NULL, &ft);
			::CloseHandle(h_fileNew);
			return ft;
		}

		FILETIME getModFileTime(const wchar_t* szFullPath)
		{
			FILETIME ft;
			HANDLE h_fileNew = ::CreateFileW(szFullPath,
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			::GetFileTime(h_fileNew, NULL, NULL, &ft);
			::CloseHandle(h_fileNew);
			return ft;
		}
	}
}