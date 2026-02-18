#pragma once

namespace i3
{
	// lhs가 더 오래된 것이면 true이다..
	namespace detail
	{
		I3_EXPORT_BASE FILETIME getModFileTime(const char* szFullPath);
		I3_EXPORT_BASE FILETIME getModFileTime(const wchar_t* szFullPath);
		

		template<class Range1, class Range2> 
		bool is_modified_filedate_less(const Range1& lhs_filename, const Range2& rhs_filename)
		{
			typedef typename Range1::value_type			Ch1;
			typedef typename Range2::value_type			Ch2;
			i3::stack_basic_string<mpl::size_t<128>, Ch1>	stack_str_lhs;
			i3::stack_basic_string<mpl::size_t<128>, Ch2>	stack_str_rhs;

			Range1 lhs;		Range2 rhs;
			if (lhs_filename[lhs_filename.size()] != Ch1(0)) 
			{	
				stack_str_lhs.assign(lhs_filename.begin(), lhs_filename.end());	
				lhs = stack_str_lhs; 
			}
			else {	lhs = lhs_filename;	}

			if (rhs_filename[rhs_filename.size()] != Ch2(0)) 
			{	
				stack_str_rhs.assign(rhs_filename.begin(), rhs_filename.end());	
				rhs = stack_str_rhs; 
			}
			else {	rhs = rhs_filename;	}

			FILETIME ft_l = getModFileTime(&lhs[0]);
			FILETIME ft_r = getModFileTime(&rhs[0]);

			return ::CompareFileTime(&ft_l, &ft_r) < 0;
		}
	}


	template<class Range1, class Range2> inline
	bool is_modified_filedate_less(const Range1& lhs_filename, const Range2& rhs_filename)
	{
		return detail::is_modified_filedate_less(i3::as_literal(lhs_filename), i3::as_literal(rhs_filename));
	}
	
}