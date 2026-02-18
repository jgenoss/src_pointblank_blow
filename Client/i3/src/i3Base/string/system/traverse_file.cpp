#include "i3CommonType.h"

#include "i3Base/string/system/traverse_file.h"
#include "i3Base/itl/stack.h"

#include "i3Base/itl/range/as_literal.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/make_generic_literal.h"
#include "i3Base/string/ext/generic_find_last_of.h"

namespace i3
{

namespace
{
	template<class Ch>
	struct traverse_file_folder
	{
		traverse_file_folder(HANDLE h) : folder_handle(h) {}
		traverse_file_folder(HANDLE h, const i3::rc_basic_string<Ch>& rel_path) : folder_handle(h), folder_rel_path(rel_path) {}
	
		HANDLE						folder_handle;
		i3::rc_basic_string<Ch>		folder_rel_path;
	};
	
	template<class Ch, class CallBack>
	bool interrupt_check(  i3::stack< traverse_file_folder<Ch> >& inout_path_stack, const i3::rc_basic_string<Ch>& BasePath, 
		CallBack* cb)
	{
		bool interrupt = false;
		cb->on_interruptible(interrupt);
		if (interrupt)
		{
			::FindClose(inout_path_stack.top().folder_handle);
			inout_path_stack.pop();
			
			while( !inout_path_stack.empty() )
			{
				traverse_file_folder<Ch>* path = &inout_path_stack.top();
				cb->on_leave_sub_dir(BasePath, path->folder_rel_path);
				::FindClose(path->folder_handle);
				inout_path_stack.pop();
			}
		}

		return interrupt;
	}
	
	__forceinline DWORD GetFileAttributesImp(const char* str) { return ::GetFileAttributesA(str); }
	__forceinline DWORD GetFileAttributesImp(const wchar_t* str) { return ::GetFileAttributesW(str); }
	__forceinline HANDLE FindFirstFileImp(const char* str, WIN32_FIND_DATAA* wfd) { return ::FindFirstFileA(str, wfd); }
	__forceinline HANDLE FindFirstFileImp(const wchar_t* str, WIN32_FIND_DATAW* wfd) { return ::FindFirstFileW(str, wfd); }
	__forceinline bool   FindNextFileImp(HANDLE h, WIN32_FIND_DATAA* wfd) { return ::FindNextFileA(h, wfd) == TRUE; }
	__forceinline bool	 FindNextFileImp(HANDLE h, WIN32_FIND_DATAW* wfd) { return ::FindNextFileW(h, wfd) == TRUE; }

	template<class Ch, class WIN32_FIND_DATA_IMP, class CallBack>
	void traverse_file_imp( const i3::literal_base_range<const Ch*>& Path, CallBack* cb, bool recursive)
	{

		// 실제 존재하는 디렉토리이나 파일인지 점검... 
		// 와일드카드 존재시 걸러서 점검해야한다.. (Path 인수 사용은 그뒤로 하지 않도록 조정한다.)
		typedef i3::iterator_range<const Ch*>					range_type;
		typedef i3::stack_basic_string<mpl::size_t<128>, Ch>	stack_string_type;
		typedef i3::rc_basic_string<Ch>							rc_string_type;

		range_type rng_input_directory;
		range_type rng_input_filename;

		range_type::iterator it_pos = i3::generic_find_last_of(Path, i3::make_generic_literal(Ch, "\\/*?") );			//	Path.find_last_of(L"\\/*?");

		if ( it_pos != Path.end() && ( *it_pos == Ch('*') || *it_pos == Ch('?') ) )
		{
			i3::extract_directoryname_filename_iterator_range(Path, rng_input_directory, rng_input_filename);
		}
		else
		{
			DWORD path_attr = GetFileAttributesImp(Path.c_str());
			if (path_attr == INVALID_FILE_ATTRIBUTES)
				return;
			
			if ( path_attr & FILE_ATTRIBUTE_DIRECTORY )
			{
				rng_input_directory = range_type(Path.begin(), Path.end() );
				rng_input_filename = i3::as_literal( i3::make_generic_literal(Ch,"*.*") );
			}
			else
			{
				i3::extract_directoryname_filename_iterator_range(Path, rng_input_directory, rng_input_filename);
			}
		}
		
		WIN32_FIND_DATA_IMP findFileData;	// File Data 구조 Structure
		i3::stack< traverse_file_folder<Ch> >	PathStack;
		stack_string_type TmpRelPath;
		stack_string_type TmpFindPath;
		
		rc_string_type		BasePath(rng_input_directory.begin(), rng_input_directory.end());
		
		TmpFindPath.assign( BasePath );
		TmpFindPath += Ch('\\');
		TmpFindPath.append( rng_input_filename.begin(), rng_input_filename.end() );

		HANDLE h = FindFirstFileImp( TmpFindPath.c_str(), &findFileData);

		if ( h == INVALID_HANDLE_VALUE)
			return;

		traverse_file_folder<Ch>	RootFolder(h);
		PathStack.push(RootFolder);	
		const traverse_file_folder<Ch>* CurrentFolder = &PathStack.top();
		
		range_type rng_dot = i3::as_literal(i3::make_generic_literal(Ch, "."));
		range_type rng_dotdot = i3::as_literal(i3::make_generic_literal(Ch, ".."));

		for ( ; ; ) 
		{
		
			// 1. 자신/상위 폴더명은 제외
			if ( !(i3::generic_is_equal(findFileData.cFileName, rng_dot ) || i3::generic_is_equal(findFileData.cFileName, rng_dotdot ) ) )
			{
				// 2. 파일 속성을 참조하여 파일이면 출력 
				if( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					bool directory_traverse = true;

					directory_traverse = cb->on_dir( findFileData, BasePath, CurrentFolder->folder_rel_path);

					if ( interrupt_check(PathStack, BasePath, cb) )
						break;

					if( directory_traverse && recursive )
					{
						TmpRelPath.assign( CurrentFolder->folder_rel_path );				// 이 짓거리를 조건문안으로 지연시킴.
						if (!TmpRelPath.empty() )
							TmpRelPath.append( 1, Ch('\\') );
						TmpRelPath.append( findFileData.cFileName );
						
						TmpFindPath.assign( BasePath );
						TmpFindPath += Ch('\\');
						TmpFindPath += TmpRelPath;
						TmpFindPath += Ch('\\');
						TmpFindPath.append( rng_input_filename.begin(), rng_input_filename.end() );

						HANDLE h = FindFirstFileImp( TmpFindPath.c_str(), &findFileData);
						if (h != INVALID_HANDLE_VALUE)
						{
							traverse_file_folder<Ch>	folder(h, TmpRelPath);
							PathStack.push( folder );
							CurrentFolder = &PathStack.top();
							cb->on_enter_sub_dir(BasePath, CurrentFolder->folder_rel_path);
							continue;
						}
					}

				}
				else	//일반 파일
				{
					cb->on_file(findFileData, BasePath, CurrentFolder->folder_rel_path);
					if ( interrupt_check(PathStack, BasePath, cb) )
						break;
				}
			}
			else
			{
				if ( interrupt_check(PathStack, BasePath, cb) )
					break;
			}

			// 3. 다음 File Handle로 접근
			bool res = FindNextFileImp( CurrentFolder->folder_handle, &findFileData);
			
			if ( res == false )
			{
				::FindClose(CurrentFolder->folder_handle);
				PathStack.pop();
				if ( PathStack.empty() ) break;

				for ( ; ; )
				{
					CurrentFolder = &PathStack.top();
					cb->on_leave_sub_dir(BasePath, CurrentFolder->folder_rel_path);
					if ( FindNextFileImp(CurrentFolder->folder_handle, &findFileData) )
						break;
					PathStack.pop();
					
					if (PathStack.empty()) break;
				} 
				
				if ( PathStack.empty() ) break;
			}
		}	
			
	}


}

void	traverse_file( const i3::wliteral_range& Path, traverse_file_callback_wc* cb, bool recursive )
{
	traverse_file_imp<wchar_t, WIN32_FIND_DATAW>( Path, cb, recursive);
}

void	traverse_file( const i3::literal_range& Path, traverse_file_callback_mb* cb, bool recursive )
{
	traverse_file_imp<char, WIN32_FIND_DATAA>(Path, cb, recursive);
}

}
