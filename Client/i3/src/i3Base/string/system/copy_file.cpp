#include "i3CommonType.h"

#include "i3Base/string/system/copy_file.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/strtok_fast.h"


namespace i3
{
	namespace
	{
		__forceinline bool CopyFileImp(const char* src, const char* dest, bool fail_if_exist) { return ::CopyFileA(src, dest, fail_if_exist) == TRUE; }
		__forceinline bool CopyFileImp(const wchar_t* src, const wchar_t* dest, bool fail_if_exist) { return ::CopyFileW(src, dest, fail_if_exist) == TRUE; }

		__forceinline DWORD GetFileAttributesImp(const char* str) { return ::GetFileAttributesA(str); }
		__forceinline DWORD GetFileAttributesImp(const wchar_t* str) { return ::GetFileAttributesW(str); }

		__forceinline bool CreateDirectoryImp(const char* str) { return ::CreateDirectoryA(str, nullptr) == TRUE; }
		__forceinline bool CreateDirectoryImp(const wchar_t* str) { return ::CreateDirectoryW(str, nullptr) == TRUE; }


		template<class Ch>
		bool copy_file_imp( const i3::literal_base_range<const Ch*>& SrcRootPath, 
			const i3::literal_base_range<const Ch*>& SrcRelPath, 
			const i3::literal_base_range<const Ch*>& DestRootPath, bool overwrite_enable)
		{
			// 1. 일단 곧바로 파일복사 함수 호출

			bool fail_if_exist = !overwrite_enable;

			typedef i3::stack_basic_string<mpl::size_t<128>, Ch>		stack_string_type;
			typedef i3::literal_base_range<const Ch*>					literal_range_type;


			stack_string_type src_path = SrcRootPath;
			src_path += Ch('/');	src_path += SrcRelPath;

			stack_string_type dest_path = DestRootPath;
			dest_path += Ch('/');	dest_path += SrcRelPath;

			bool bRes = CopyFileImp( src_path.c_str(), dest_path.c_str(), fail_if_exist);

			if ( bRes )
				return true;

			if ( GetFileAttributesImp(src_path.c_str()) == INVALID_FILE_ATTRIBUTES )
				return false;

			if ( GetFileAttributesImp(dest_path.c_str()) != INVALID_FILE_ATTRIBUTES )	// 이미 존재한 경우인데, 실패된 것..
				return false;

			// 2. dest 쪽 루트 경로는 존재한다고 일단 가정, dest쪽 상대 경로의 폴더들은 존재하지 않을수 있으므로 모두 생성해준다.
			stack_string_type SrcRelDir;
			i3::extract_directoryname( SrcRelPath, SrcRelDir);
			i3::vector<literal_range_type>	rel_dir_list;

			literal_range_type dir_delimit = i3::make_generic_literal(Ch, "/\\");

			i3::strtok_fast(rel_dir_list, SrcRelDir, dir_delimit);

			const size_t num_dir = rel_dir_list.size();

			stack_string_type temp_dir = DestRootPath;
			temp_dir += Ch('/');

			for ( size_t i = 0 ; i < num_dir ; ++i )
			{
				temp_dir += rel_dir_list[i];

				if (GetFileAttributesImp( temp_dir.c_str() ) == INVALID_FILE_ATTRIBUTES )
				{
					if ( CreateDirectoryImp( temp_dir.c_str() ) == false) 
						return false;
				}
				temp_dir += Ch('/');
			}

			// 3. 다시 카피 시도 하고 종료..
			return CopyFileImp( src_path.c_str(), dest_path.c_str(), fail_if_exist) != false;
		}

	}

	bool copy_file( const i3::wliteral_range& src_root_path, 
		const i3::wliteral_range& src_rel_path_filename, 
		const i3::wliteral_range& dest_root_path, bool overwrite_enable)
	{
		return copy_file_imp<wchar_t>(src_root_path, src_rel_path_filename, dest_root_path, overwrite_enable);
	}

	bool copy_file( const i3::literal_range& src_root_path, 
		const i3::literal_range& src_rel_path_filename, 
		const i3::literal_range& dest_root_path, bool overwrite_enable)
	{
		return copy_file_imp<char>(src_root_path, src_rel_path_filename, dest_root_path, overwrite_enable);
	}

	bool copy_file( const i3::wliteral_range& src_root_path, 
		const i3::wliteral_range& src_rel_path, const i3::wliteral_range& only_filename,
		const i3::wliteral_range& dest_root_path, bool overwrite_enable)
	{
		i3::stack_wstring src_rel_path_filename = src_rel_path; 
		if (!src_rel_path_filename.empty())
			src_rel_path_filename += L'/';

		src_rel_path_filename += only_filename;
		return copy_file_imp<wchar_t>(src_root_path, src_rel_path_filename, dest_root_path, overwrite_enable);
	}

	bool copy_file( const i3::literal_range& src_root_path, 
		const i3::literal_range& src_rel_path, const i3::literal_range& only_filename,
		const i3::literal_range& dest_root_path, bool overwrite_enable)
	{
		i3::stack_string src_rel_path_filename = src_rel_path; 
		if (!src_rel_path_filename.empty())
			src_rel_path_filename += '/';
		src_rel_path_filename += only_filename;
		return copy_file_imp<char>(src_root_path, src_rel_path_filename, dest_root_path, overwrite_enable);
	}

}
