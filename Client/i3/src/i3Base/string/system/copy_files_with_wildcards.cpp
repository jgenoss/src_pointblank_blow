#include "i3CommonType.h"

#include "i3Base/string/system/copy_files_with_wildcards.h"

#include "i3base/string/system/traverse_file.h"
#include "i3Base/string/system/copy_file.h"
#include "i3Base/string/ext/is_matched_wildcard.h"


namespace i3
{

	namespace detail
	{
		template<class Ch>
		struct traverse_callback_for_copy_files;

		template<>
		struct traverse_callback_for_copy_files<wchar_t> : public i3::traverse_file_callback_wc
		{
			const i3::vector<i3::wliteral_range>& wild_card_list;
			const i3::wliteral_range&			  dest_dir;
			bool								  overwrite_enable;

			traverse_callback_for_copy_files(const i3::vector<i3::wliteral_range>& wild_card_list,
				const i3::wliteral_range& dest_dir, bool overwrite_enable) :
				wild_card_list(wild_card_list), dest_dir(dest_dir), overwrite_enable(overwrite_enable) {}


			virtual void on_file(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
			{
				const size_t num_wild_card = wild_card_list.size();
				i3::wliteral_range only_file_name =	fd.cFileName;

				bool bMatched = false;
				for (size_t i = 0; i < num_wild_card; ++i)
				{
					if (i3::is_matched_wildcard(only_file_name, wild_card_list[i]))
					{
						bMatched = true;
						break;
					}
				}
				
				if (bMatched)
				{
					i3::copy_file(base_path, subdir_path, only_file_name, dest_dir, overwrite_enable);
				}
			}

			virtual bool on_dir(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return true; }

		};

		template<>
		struct traverse_callback_for_copy_files<char> : public i3::traverse_file_callback_mb
		{
			const i3::vector<i3::literal_range>& wild_card_list;
			const i3::literal_range&			  dest_dir;
			bool								  overwrite_enable;

			traverse_callback_for_copy_files(const i3::vector<i3::literal_range>& wild_card_list,
				const i3::literal_range& dest_dir, bool overwrite_enable) :
				wild_card_list(wild_card_list), dest_dir(dest_dir), overwrite_enable(overwrite_enable) {}
			
			virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
			{
				const size_t num_wild_card = wild_card_list.size();
				i3::literal_range only_file_name = fd.cFileName;

				bool bMatched = false;
				for (size_t i = 0; i < num_wild_card; ++i)
				{
					if (i3::is_matched_wildcard(only_file_name, wild_card_list[i]))
					{
						bMatched = true;
						break;
					}
				}

				if (bMatched)
				{
					i3::copy_file(base_path, subdir_path, only_file_name, dest_dir, overwrite_enable);
				}
			}

			virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path) { return true; }
		};

		template<class Ch>
		void copy_files_with_wildcard_imp(const i3::literal_base_range<const Ch*>& src_dir,
			const i3::vector< i3::literal_base_range<const Ch*> >& wildcard_file_list,
			const i3::literal_base_range<const Ch*>& dest_dir, bool recursive, bool overwrite_enable)
		{
			// src_dir의 전체 traverse 처리.. 처리 중 알맞는 파일이 나오면 해당 파일을 복사...
			// traverse_file과 copy_file함수를 연달아 쓰면 될것이다...

			traverse_callback_for_copy_files<Ch> cb(wildcard_file_list, dest_dir, overwrite_enable);
			i3::traverse_file(src_dir, &cb, recursive);
		}


	}



	void copy_files_with_wildcard(const i3::literal_range& src_dir,
		const i3::vector<i3::literal_range>& wildcard_file_list,
		const i3::literal_range& dest_dir, bool recursive, bool overwrite_enable)
	{
		detail::copy_files_with_wildcard_imp<char>(src_dir, wildcard_file_list, dest_dir, recursive, overwrite_enable);
	}

	void copy_files_with_wildcard(const i3::wliteral_range& src_dir,
		const i3::vector<i3::wliteral_range>& wildcard_file_list,
		const i3::wliteral_range& dest_dir, bool recursive, bool overwrite_enable)
	{
		detail::copy_files_with_wildcard_imp<wchar_t>(src_dir, wildcard_file_list, dest_dir, recursive, overwrite_enable);
	}


}