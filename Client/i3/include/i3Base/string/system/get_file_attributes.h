#pragma once

namespace i3
{
	
	namespace detail
	{
		__forceinline DWORD GetFileAttributesImp(const char* psz) { return GetFileAttributesA(psz); }
		__forceinline DWORD GetFileAttributesImp(const wchar_t* psz) { return GetFileAttributesW(psz); }

		template<class Range> inline
		DWORD get_file_attributes_imp(const Range& rng) 
		{
			typedef typename Range::value_type		Ch;
			const size_t num = rng.size();
			if (num == 0) return false;
			
			if (rng[num] == 0) 
				return GetFileAttributesImp(&rng[0]);
			else
			{
				i3::stack_basic_string<mpl::size_t<128>, Ch>	stack_str;
				stack_str.assign(rng.begin(), rng.end());
				return GetFileAttributesImp(&stack_str[0]);
			}
		}
	}

	template<class Range> __forceinline
	DWORD get_file_attributes(const Range& rng) { return detail::get_file_attributes_imp(i3::as_literal(rng)); }
			
}