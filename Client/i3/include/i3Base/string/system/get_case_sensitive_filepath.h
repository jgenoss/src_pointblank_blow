#pragma once

#include "../stack_string.h"
#include <Windows.h>
#include <WinDef.h>

namespace i3
{

	namespace detail
	{
		// 
		// 여러 방법이 있지만, 일단은 GetShortPathName과 GetLongPathName 연속사용으로 처리한다.
		// 

		inline
		::DWORD get_case_sensitive_filepath_api(const char* strInput, char* out, std::size_t out_buff_size)
		{
			i3::stack_string strShort(out_buff_size, 0);

			::DWORD res_len = ::GetShortPathNameA(strInput, &strShort[0], out_buff_size);

			if (res_len == 0)		// 오류거나 아니면 입력 문자열이 없으면..err or ok..
				return 0;

			strShort.resize(res_len);

			return ::GetLongPathNameA(strShort.c_str(), out, out_buff_size);
		}

		inline
		::DWORD get_case_sensitive_filepath_api(const wchar_t* strInput, wchar_t* out, std::size_t out_buff_size)
		{
			i3::stack_wstring strShort(out_buff_size, 0);

			::DWORD res_len = ::GetShortPathNameW(strInput, &strShort[0], out_buff_size);

			if (res_len == 0)		// 오류거나 아니면 입력 문자열이 없으면..err or ok..
				return 0;

			strShort.resize(res_len);

			return ::GetLongPathNameW(strShort.c_str(), out, out_buff_size);
		}

		template<class Range, class Lit> inline
		bool get_case_sensitive_filepath_lit(const Range& inputPath, Lit case_corrected_path, size_t out_buff_size)
		{
			if (inputPath.empty())
			{
				case_corrected_path[0] = 0;	return true;
			}
			
			DWORD res_len = get_case_sensitive_filepath_api(&inputPath[0], case_corrected_path, out_buff_size);

			if (res_len == 0)
			{
				i3::safe_string_copy(case_corrected_path, inputPath, out_buff_size);
				return false;
			}

			case_corrected_path[res_len] = 0;
			return true;
		}

		template<class Range, class Seq> inline
		bool get_case_sensitive_filepath_seq(const Range& inputPath, Seq& case_corrected_path)
		{
			if (inputPath.empty())
			{
				case_corrected_path.clear(); return true;
			}

			case_corrected_path = Seq(inputPath.size(), 0);

			DWORD res_len = get_case_sensitive_filepath_api(&inputPath[0], &case_corrected_path[0], inputPath.size()+1);

			if (res_len == 0)
			{
				case_corrected_path.assign(inputPath.begin(), inputPath.end());
				return false;
			}

			case_corrected_path.resize(res_len);
			return true;
		}
	}
	
	template<class Range, class Lit> inline typename enable_if_iterator<Lit, 
	bool>::type get_case_sensitive_filepath(const Range& inputPath, Lit case_corrected_path, size_t out_buff_size) 
	{ 
		return detail::get_case_sensitive_filepath_lit(i3::as_literal(inputPath), case_corrected_path, out_buff_size);
	}

	// 배열 지원..
	template<class Range, class Ch, size_t N> inline 
	bool get_case_sensitive_filepath(const Range& inputPath, Ch (&case_corrected_path)[N])
	{
		return detail::get_case_sensitive_filepath_lit(i3::as_literal(inputPath), case_corrected_path, N);
	}

	template<class Range, class Seq>  inline typename disable_if_iterator<Seq,
	bool>::type get_case_sensitive_filepath(const Range& inputPath, Seq& case_corrected_path) 
	{ 
		return detail::get_case_sensitive_filepath_seq(i3::as_literal(inputPath), case_corrected_path);
	}

	/////////////////

	template<class Range, class Lit> inline typename enable_if_iterator<Lit, 
	bool>::type get_case_sensitive_unix_filepath(const Range& inputPath, Lit case_corrected_path, size_t out_buff_size)
	{
		bool res = detail::get_case_sensitive_filepath_lit(i3::as_literal(inputPath), case_corrected_path, out_buff_size);
		i3::make_unix_path(case_corrected_path);
		return res;
	}

	// 배열 지원..
	template<class Range, class Ch, size_t N> inline
	bool get_case_sensitive_unix_filepath(const Range& inputPath, Ch(&case_corrected_path)[N])
	{
		bool res = detail::get_case_sensitive_filepath_lit(i3::as_literal(inputPath), case_corrected_path, N);
		i3::make_unix_path(case_corrected_path);
		return res;
	}

	template<class Range, class Seq>  inline typename disable_if_iterator<Seq, 
	bool>::type	get_case_sensitive_unix_filepath(const Range& inputPath, Seq& case_corrected_path)
	{
		bool res = detail::get_case_sensitive_filepath_seq(i3::as_literal(inputPath), case_corrected_path);
		i3::make_unix_path(case_corrected_path);
		return res;
	}



}