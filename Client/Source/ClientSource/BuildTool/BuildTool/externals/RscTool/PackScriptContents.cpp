#include "stdafx.h"

#include "PackScriptContents.h"
#include "i3Base/i3StreamFile.h"			// 이름을 고쳤으면 좋겠다.

#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/compare/generic_is_niequal.h"

#include "i3Base/itl/vector_util.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/extract_directoryname.h"

#include "i3Base/string/system/traverse_file.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "i3Base/string/ext/is_matched_wildcard.h"

#include "i3Base/itl/range/algorithm/remove_if.h"
#include "i3Base/itl/range/algorithm/remove_copy_if.h"
#include "i3Base/itl/algorithm/rfind_if.h"


void CopyPackScriptContents(const PackScriptContents& src, PackScriptContents& dest)
{
	dest.unit_list.clear();
	dest.copy_unit.reset();

	const size_t num = src.unit_list.size();
	dest.unit_list.reserve(num);

	for (size_t i = 0 ; i < num ; ++i )
	{
		PackScriptUnitPtr new_unit = i3::make_shared<PackScriptUnit>(*src.unit_list[i]);
		dest.unit_list.push_back(new_unit);
	}

	if (src.copy_unit != NULL)
		dest.copy_unit = i3::make_shared<PackScriptUnit>(*src.copy_unit);
}

bool operator==(const PackScriptContents& lhs, const PackScriptContents& rhs)
{
	const size_t num = lhs.unit_list.size();

	if (num != rhs.unit_list.size() )
		return false;
	
	for (size_t i = 0 ; i < num ; ++i )
	{
		if ( *lhs.unit_list[i] != *rhs.unit_list[i] )
			return false;
	}
	
	if ( lhs.copy_unit == rhs.copy_unit ) 
		return true;

	if ( lhs.copy_unit == NULL)
		return IsPackScriptUnitEmpty(*rhs.copy_unit); 
	if ( rhs.copy_unit == NULL) 
		return IsPackScriptUnitEmpty(*lhs.copy_unit);

	return *lhs.copy_unit == *rhs.copy_unit;
}


bool operator!=(const PackScriptContents& lhs, const PackScriptContents& rhs)
{
	return !(lhs == rhs);
}


bool operator==(const PackScriptUnit& lhs, const PackScriptUnit& rhs)
{
	if ( lhs.pack_filename != rhs.pack_filename) return false;
	if ( lhs.add_list != rhs.add_list) return false;
	if ( lhs.remove_list != rhs.remove_list) return false;
	if ( lhs.enc_list != rhs.enc_list) return false;
	if ( lhs.add_opt_list != rhs.add_opt_list) return false;

	return true;
}

bool operator!=(const PackScriptUnit& lhs, const PackScriptUnit& rhs)
{
	return !(lhs==rhs);
}

bool IsPackScriptUnitEmpty(const PackScriptUnit& unit)
{
	return unit.pack_filename.empty() &&
		   unit.add_list.empty() &&
		   unit.remove_list.empty() &&
		   unit.enc_list.empty();
}



bool LoadFileIntoBuffer( const i3::wstring& full_path, i3::vector<char>& buff)
{
	buff.clear();

	DWORD dwFileAccess = GENERIC_READ;
	DWORD dwShareMode  = FILE_SHARE_READ;
	HANDLE hFile = ::CreateFileW( full_path.c_str(), dwFileAccess, dwShareMode, 
								NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE )
		return false;						// 귀찮아서, 일단 false로 끝냄... 못 읽음..

	DWORD dwFileSize = ::GetFileSize(hFile, NULL);		// 귀찮아서 파일사이즈 구함..
	buff.resize(dwFileSize);

// 귀찮아서 일단 utf-16은 고려하지 않고 mb만 처리한다.. (2개 모두 따지려면, 범용모듈 구축 + C++템플릿을 쓰는게 좋겠다.)
// utf-16처리시, PackScriptContents역시 utf-16스타일로 변경하고, 변환함수 마련해야함..
	DWORD dwReadSize;
	BOOL res = ::ReadFile(hFile, &buff[0], dwFileSize, &dwReadSize, NULL);

	::CloseHandle(hFile);

	if (!res || dwReadSize != dwFileSize)
	{
		buff.clear();
		return false;		
	}
	return true;
}

bool SaveFileFromBuffer( const i3::vector<char>& in_buff, const i3::wstring& full_path)
{
	DWORD dwFileAccess = GENERIC_WRITE;
	DWORD dwShareMode = 0;

	HANDLE hFile = ::CreateFileW(full_path.c_str(), dwFileAccess, dwShareMode, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		I3PRINTLOG(I3LOG_FATAL, L"File Save Failed : %s \n", full_path);
		return false;
	}

	DWORD dwWriteSize;
	BOOL res = ::WriteFile(hFile, &in_buff[0], in_buff.size(), &dwWriteSize, NULL);

	::CloseHandle(hFile);

	if (!res || DWORD(in_buff.size()) != dwWriteSize )
	{
		return false;
	}
	return true;
}


// 해당 줄에 리턴 문자 외에 아무것도 없는 경우도 한 줄로 고려해서 일단 처리한다..

void ExtractLineRange( const i3::const_char_range& input_range, i3::const_char_range& out_line_range, 
					  i3::const_char_range& out_left_range)
{
	typedef i3::const_char_range::iterator	iterator;
	
	iterator it_line_end = i3::range::find(input_range, '\n');
	
	out_line_range = i3::const_char_range( input_range.begin(), it_line_end );
	
	out_left_range = i3::const_char_range( it_line_end, input_range.end() );
	
	if ( it_line_end == input_range.end() )		// 사실상 구분자를 못찾은 것으로 이대로 종료해도 좋음..
		return;
	
	iterator it_prev = it_line_end;	--it_prev;
	if ( *it_prev == '\r' )
		out_line_range.advance_end(-1);		// 하나 더 줄임..
	
	out_left_range.advance_begin(1);		// '\n'이 있는게 확실하므로 이건 제외...
}

namespace
{
	
	struct find_non_blank { template <class C> inline bool operator()(C c) const { return c > 32; }	};
	struct find_blank {  template <class C> inline bool operator()(C c) const { return c <= 32; }	};

	namespace cmd_type 
	{
		enum type { add, remove, enc, save, checksum, add_copy, remove_copy };
	}

	const i3::string astrCmd[] = { "ADD", "REMOVE", "ENC", "SAVE", "CHECKSUM", "ADDCOPY", "REMOVECOPY" };
	const i3::string str_no_sub = "-nosub";
		
	bool		process_cmd_add( const i3::const_char_range& param_range, PackScriptContents& inout)
	{
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );
		
		PackScriptUnitPtr& unit_ptr = inout.unit_list.back();		// 현재 것에 그대로 add
		PackScriptUnit&	unit = *unit_ptr;

		unit.add_list.push_back( i3::string() );
		unit.add_list.back().assign( param_range.begin(), it1 );

		if ( it1 != param_range.end() )
		{
			it1 = i3::find_if( it1, param_range.end(), find_non_blank() );
			i3::const_char_range::iterator it2 = i3::find_if(it1, param_range.end(), find_blank());
			unit.add_opt_list.push_back( i3::generic_is_iequal( i3::const_char_range( it1, it2), str_no_sub) == false );
		}
		else
		{
			unit.add_opt_list.push_back(true);			
		}
		return true;
	}

	bool		process_cmd_remove( const i3::const_char_range& param_range, PackScriptContents& inout)
	{
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );
	
		PackScriptUnitPtr& unit_ptr = inout.unit_list.back();
		PackScriptUnit&	unit = *unit_ptr;
			
		unit.remove_list.push_back( i3::string() );
		unit.remove_list.back().assign( param_range.begin(), it1 );
		return true;
	}

	bool		process_cmd_enc( const i3::const_char_range& param_range, PackScriptContents& inout)
	{
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );
		
		PackScriptUnitPtr& unit_ptr = inout.unit_list.back();
		PackScriptUnit&	unit = *unit_ptr;
		
		unit.enc_list.push_back( i3::string() );
		unit.enc_list.back().assign( param_range.begin(), it1 );		
		return true;
	}

	bool		process_cmd_save( const i3::const_char_range& param_range, PackScriptContents& inout) 
	{ 
		// save가 여러번 들어가게 되면 컨텐츠를 쪼개야한다...
		// 또한 save 이후 remove가 그 이전 add된것과 비교되서 중복된건 제거될 필요가 있다..
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );

		PackScriptUnitPtr& unit_ptr = inout.unit_list.back();
		PackScriptUnit&	unit = *unit_ptr;
		
		unit.pack_filename.assign( param_range.begin(), it1);		// 패킹 파일 이름 보관..
		
		if (inout.unit_list.size() > 1 )		// 2번째 저장때, 1번째것과 2번째것을 비교하는 루틴 필요.
		{										// 2번째 내용중 add-remove중복은 제거되어야한다...
			const std::size_t num_unit_list = inout.unit_list.size();		
			PackScriptUnit& prev_unit = *inout.unit_list[num_unit_list - 2];
			PackScriptUnit& curr_unit = unit;
			
			// 급한대로 2중루프(이전add목록/현재remove목록 비교해서 일치하는 경우, 
			//                  현재 add목록과 현재 remove 목록을 제거처리 )

			/////////
			int num_add = (int)prev_unit.add_list.size();
			
			if ( (int)curr_unit.add_list.size() <  num_add )
				num_add = (int)curr_unit.add_list.size();	

			int num_remove = (int) curr_unit.remove_list.size();
			
			for ( int i = 0 ; i < num_remove ; ++i )
			{
				const i3::string& str_remove = curr_unit.remove_list[i];

				for ( int j = 0 ; j < num_add ; ++j )
				{
					const i3::string& str_add = curr_unit.add_list[j];
					
					if (i3::generic_is_iequal(str_add, str_remove ) )
					{
						i3::vu::erase_index(curr_unit.add_list, j);
						i3::vu::erase_index(curr_unit.add_opt_list, j);
						--j;	--num_add;

						i3::vu::erase_index(curr_unit.remove_list, i);
						--i;	--num_remove;
						break;
					}
				}
			}
			////////

		}

		PackScriptUnitPtr new_unit_ptr = i3::make_shared<PackScriptUnit>();

		inout.unit_list.push_back( new_unit_ptr );	// 또 하나 만든다...새것..
		
		PackScriptUnit&	new_unit = *new_unit_ptr;	//
		new_unit.add_list = unit.add_list;			// 그대로 복사해야한다..
		new_unit.add_opt_list = unit.add_opt_list;	// 그대로 복사해야한다..
													//
		return true; 
	}

	bool		process_cmd_checksum( const i3::const_char_range& param_range, PackScriptContents& inout) 
	{ 
		return false;  
	}
	
	bool		process_cmd_addcopy( const i3::const_char_range& param_range, PackScriptContents& inout) 
	{ 
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );

		if ( inout.copy_unit == NULL )
			inout.copy_unit = i3::make_shared<PackScriptUnit>();
		
		PackScriptUnitPtr& unit_ptr = inout.copy_unit;		// 현재 것에 그대로 add
		PackScriptUnit&	unit = *unit_ptr;

		unit.add_list.push_back( i3::string() );
		unit.add_list.back().assign( param_range.begin(), it1 );

		if ( it1 != param_range.end() )
		{
			it1 = i3::find_if( it1, param_range.end(), find_non_blank() );
			i3::const_char_range::iterator it2 = i3::find_if(it1, param_range.end(), find_blank());
			unit.add_opt_list.push_back( i3::generic_is_iequal( i3::const_char_range( it1, it2), str_no_sub) == false );
		}
		else
		{
			unit.add_opt_list.push_back(true);			
		}
		return true;
  	}

	bool		process_cmd_removecopy( const i3::const_char_range& param_range, PackScriptContents& inout) 
	{ 
		i3::const_char_range::iterator it1 = i3::range::find_if(param_range, find_blank() );

		if ( inout.copy_unit == NULL )
			inout.copy_unit = i3::make_shared<PackScriptUnit>();

		PackScriptUnitPtr& unit_ptr = inout.copy_unit;
		PackScriptUnit&	unit = *unit_ptr;

		unit.remove_list.push_back( i3::string() );
		unit.remove_list.back().assign( param_range.begin(), it1 );
		return true;
	  
	}


	bool (*afnProcessCmd[ ])(const i3::const_char_range&, PackScriptContents&) = 
	{ 
		process_cmd_add,
		process_cmd_remove,
		process_cmd_enc,
		process_cmd_save,
		process_cmd_checksum,
		process_cmd_addcopy,
		process_cmd_removecopy,
	};


	bool		process_cmd( const i3::const_char_range& line_range, PackScriptContents& inout)
	{
	
		i3::const_char_range::iterator it_cmd_end = i3::range::find_if(line_range, find_blank() );

		if ( it_cmd_end == line_range.end() )
			return false;							// 추가 인수가 없으면 false로 간주..			
		
		i3::const_char_range::iterator it_param_start = it_cmd_end;	++it_param_start;

		it_param_start = i3::find_if( it_param_start, line_range.end(), find_non_blank() );

		if ( it_param_start == line_range.end() )
			return false;							// 추가 인수가 없으면 false로 간주..			

		i3::const_char_range cmd_rng(line_range.begin(), it_cmd_end);

		const std::size_t num_cmd = _countof(astrCmd);

		for ( size_t i = 0 ; i < num_cmd ; ++i )
		{
			if ( i3::generic_is_iequal( cmd_rng, astrCmd[i] ) )
			{
				return (*afnProcessCmd[i])( i3::const_char_range(it_param_start, line_range.end()) , inout);
			}
		}

		return false;
	}

}

bool ParsePackScriptLine(const i3::const_char_range& line_range, PackScriptContents& inout)
{
	// 줄 하나에 대해서 처리..
	if (line_range.empty())
		return false;

	typedef i3::const_char_range::iterator iterator;

	iterator it_non_blank = i3::range::find_if(line_range, find_non_blank());		// 앞쪽 블랭크 제거..
	i3::const_char_range line_range2( it_non_blank, line_range.end());
	
	if (line_range2.empty() )
		return false;

	if ( line_range[0] == '#' )		// 주석이라고 한다..
		return false;
	
	return process_cmd( line_range2, inout);
}

//
// 스크립트는 커맨드가 루아 방식처럼 스택에 담아놓는 방식으로 적용되므로...
// 충분이 감안하고, 자료구조에 담을때는 필요한 것 위주로 담도록 수정...
//

bool ImportPackScriptBuffer( const i3::vector<char>& buff, PackScriptContents& out)
{
	i3::const_char_range ir(buff.begin(), buff.end());			// iterator_range를 사용하는게 편하다...

	i3::const_char_range ir_line; 
	
	out.unit_list.push_back( i3::make_shared<PackScriptUnit>() );		// 하나 만들어놓고 시작..

	while ( !ir.empty() )
	{
		::ExtractLineRange(ir, ir_line, ir);
		::ParsePackScriptLine( ir_line, out);
	}

	//
	// 맨 마지막 unit_list 마지막 원소값에 팩파일 이름이 없다면 비운다..
	// 패킹 파일 이름이 없으면 읽기/쓰기 모두 무시한다...
	PackScriptUnitPtr& unit_ptr = out.unit_list.back();
	PackScriptUnit&	unit = *unit_ptr;
	if ( unit.pack_filename.empty() )
	{
		out.unit_list.pop_back();

		if ( out.unit_list.empty() )
			return false;
	}

	return true;
}


namespace
{

	void process_export_list_into_buffer(cmd_type::type t, const i3::vector<i3::string>& in_list, i3::vector<char>& out_buff,
										 const i3::vector<bool>* in_opt_list = NULL)
	{
		const size_t num = in_list.size();
		if (!num) return;

		const i3::string& str_cmd = astrCmd[t];
		
		for (size_t i = 0 ; i < num ; ++i )
		{
			const i3::string& str_value = in_list[i];
		
			out_buff.insert( out_buff.end(), str_cmd.begin(), str_cmd.end() );	// add
			out_buff.push_back('\t');											// 공백대신 탭채택..			
			out_buff.insert( out_buff.end(), str_value.begin(), str_value.end() );	// 팩할 리소스 경로..

			if (in_opt_list && (*in_opt_list)[i] == false)	// -nosub만 표시해둔다..
			{
				out_buff.push_back(' ');
				out_buff.insert(out_buff.end(), str_no_sub.begin(), str_no_sub.end() );		// -nosub
			}
			out_buff.push_back('\r');	out_buff.push_back('\n');
		}		
		out_buff.push_back('\r');	out_buff.push_back('\n');
	}
	
	void process_export_name_into_buffer(const i3::string& pack_filename, i3::vector<char>& out_buff)
	{
		// 팩파일 이름과 SAVE/CHECKSUM 문자열 삽입..

		out_buff.insert(out_buff.end(), astrCmd[cmd_type::save].begin(), astrCmd[cmd_type::save].end() );
		out_buff.push_back(' ');		// 이경우엔 탭대신 그냥 빈칸..
		out_buff.insert(out_buff.end(), pack_filename.begin(), pack_filename.end() );
		out_buff.push_back('\r');	out_buff.push_back('\n');

		out_buff.insert(out_buff.end(), astrCmd[cmd_type::checksum].begin(), astrCmd[cmd_type::checksum].end() );
		out_buff.push_back(' ');		// 이경우엔 탭대신 그냥 빈칸..
		out_buff.insert(out_buff.end(), pack_filename.begin(), pack_filename.end() );
		out_buff.push_back('\r');	out_buff.push_back('\n');

		out_buff.push_back('\r');	out_buff.push_back('\n');		// 가독성을 위한 한줄띄움..
	}

}

void ExportPackScriptIntoBuffer(const PackScriptContents& in, i3::vector<char>& out_buff)
{
	const size_t num_pack_file = in.unit_list.size();

	if (num_pack_file > 0)
	{
		for (size_t i = 0 ; i < num_pack_file-1 ; ++i )
		{
			PackScriptUnit* unit = in.unit_list[i].get();

			ExportPackScriptUnitIntoBuffer(unit, out_buff);
			process_export_list_into_buffer(cmd_type::remove, unit->add_list, out_buff); // add한것을 remove하는것으로 저장(호환성문제 처리)
			out_buff.push_back('\r'); out_buff.push_back('\n');	
			out_buff.push_back('\r'); out_buff.push_back('\n');
		}							

		ExportPackScriptUnitIntoBuffer(in.unit_list[num_pack_file-1].get(), out_buff);
	}
	
	if ( in.copy_unit != NULL)
	{
		process_export_list_into_buffer(cmd_type::add_copy, in.copy_unit->add_list, out_buff, &in.copy_unit->add_opt_list);
		process_export_list_into_buffer(cmd_type::remove_copy, in.copy_unit->remove_list, out_buff);
	}
		
	return;
}

void ExportPackScriptUnitIntoBuffer(const PackScriptUnit* unit, i3::vector<char>& out_buff)
{
	// add 처리..
	process_export_list_into_buffer(cmd_type::add, unit->add_list, out_buff, &unit->add_opt_list);
	// remove 처리..
	process_export_list_into_buffer(cmd_type::remove, unit->remove_list, out_buff);
	// enc 처리..
	process_export_list_into_buffer(cmd_type::enc, unit->enc_list, out_buff);
	// 팩 파일 이름..
	process_export_name_into_buffer(unit->pack_filename, out_buff);
}



bool ImportPackScriptFile( const i3::wstring& full_path, PackScriptContents& out)
{
	i3::vector<char> buffer;

	if ( ::LoadFileIntoBuffer(full_path, buffer) == false )
		return false;
	
	return ::ImportPackScriptBuffer(buffer, out);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool ExportPackScriptFile( const PackScriptContents& in, const i3::wstring& file_path)
{
	i3::vector<char> buffer;		// 버퍼 적재후 파일로 통으로 write 목표..
	buffer.reserve(4096);	// 일단 4K 예비할당을 잡는다..

	::ExportPackScriptIntoBuffer(in, buffer);			// 버퍼 저장 완료...
	
	// 버퍼를 file_path이름으로 통으로 저장하면 된다..
	return ::SaveFileFromBuffer(buffer, file_path);
}



void AddResultFilesFromPackScriptString(const i3::string& mediaPath,
										const i3::string& input, 
										const i3::vector<i3::string>& filter_list, 
										const i3::vector<i3::string>& nation_filter_list,
										i3::vector<i3::string>& inoutFileList,
										bool recursive )
{
	
	i3::string input_full_path(mediaPath);
	input_full_path += '/';
	input_full_path += input;				// 이걸로 막바로 조사..
	i3::make_unix_path(input_full_path);
	
	i3::string str_dir;

	DWORD attr = ::GetFileAttributesA(input_full_path.c_str() );
	
	if ( attr == INVALID_FILE_ATTRIBUTES )
	{
		size_t pos = input_full_path.find_last_of("/?*");

		if (pos != i3::string::npos && input_full_path[pos] != '/')
		{
			i3::extract_directoryname(input_full_path, str_dir);

			attr = ::GetFileAttributesA(str_dir.c_str() );

			if (attr == INVALID_FILE_ATTRIBUTES )
				return;
		}
		else
		{
			return;
		}
	}
	else
	{
		if ( attr & FILE_ATTRIBUTE_DIRECTORY)
			str_dir = input_full_path;
		else
			i3::extract_directoryname(input_full_path, str_dir);
	}

	// 경로의 OS원형의 케이스를 얻기 위해서 일단..
	// GetShortPathName과 GetLongPathName을 호출해서 변환한다..(트릭??).. 일반화 요구된다.
	{
		char strTemp[MAX_PATH];
		::GetShortPathNameA(str_dir.c_str(), strTemp, MAX_PATH-1);
		::GetLongPathNameA(strTemp, strTemp, MAX_PATH-1);
		str_dir = strTemp;
	}

	//
	i3::string rel_dir;
	i3::make_relative_path(mediaPath, str_dir, rel_dir);
	i3::make_unix_path(rel_dir);
	//
	
	struct internal_callback : i3::traverse_file_callback_wc
	{
		i3::vector<i3::string>	vecResult;	
		const i3::string&	rel_dir;
		const i3::vector<i3::string>& filter_list;
		const i3::vector<i3::string>& nation_filter_list;

		internal_callback( const i3::string& rel_dir, const i3::vector<i3::string>& filter_list, 
			const i3::vector<i3::string>& nation_filter_list) : 
			rel_dir(rel_dir), filter_list(filter_list), nation_filter_list(nation_filter_list) {}

		virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) 
		{ 
			i3::string strFoldername;	i3::utf16_to_mb(fd.cFileName, strFoldername);
			
			const size_t num_filter = nation_filter_list.size();
			for (size_t i = 0 ; i < num_filter ; ++i)
			{
				if ( i3::generic_is_iequal( strFoldername, nation_filter_list[i]) )
					return false;
			}
			return true; 
		}

		virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
		{
			i3::string strFilename;  i3::utf16_to_mb(fd.cFileName, strFilename);

			// 이 수준에서 익셉션 필터로 파일을 걸러낸다..
			const size_t num_filter = filter_list.size();

			for (size_t i = 0; i < num_filter; ++i )
			{
				if ( i3::is_matched_wildcard( strFilename, filter_list[i]) )			// 걸러냄..
					return;
			}
			//

			vecResult.push_back(i3::string());
			i3::string& res = vecResult.back();
			res = rel_dir;
			res += '/';

			if ( !subdir_path.empty() )
			{
				i3::string char_parent_path;
				i3::utf16_to_mb(subdir_path, char_parent_path);
				i3::make_unix_path(char_parent_path);
				res.append(char_parent_path.begin(), char_parent_path.end());
				res += '/';
			}

			res += strFilename;
		}
	};

	internal_callback cb(rel_dir, filter_list, nation_filter_list);
	
	i3::wstring winput_full_path;
	i3::mb_to_utf16(input_full_path, winput_full_path);

	traverse_file(winput_full_path, &cb, recursive);
		
	i3::vector<i3::string> vecResult;
	const size_t num_cb_result = cb.vecResult.size();

	// 중복제거..(이 경우 반드시..case sensitive해야한다..)
	for (size_t i = 0; i < num_cb_result ; ++i )
	{
		if ( i3::range::find(inoutFileList, cb.vecResult[i]) == 
			inoutFileList.end() )
		{
			vecResult.push_back(cb.vecResult[i]);
		}
	}
		
	inoutFileList.insert( inoutFileList.end(), vecResult.begin(), vecResult.end());

	return ;
}

void AddResultFilesFromPackScriptStrings(const i3::string& mediaPath,
										 const i3::vector<i3::string>& input, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& inoutFileList, 
										 const i3::vector<bool>* recursive_list)
{
	const size_t num = input.size();

	if ( recursive_list )
	{
		for (size_t i = 0 ; i < num ; ++i )
			AddResultFilesFromPackScriptString(mediaPath, input[i], filter_list, nation_filter_list, inoutFileList, (*recursive_list)[i] );
	}
	else
	{
		for (size_t i = 0 ; i < num ; ++i )
			AddResultFilesFromPackScriptString(mediaPath, input[i], filter_list, nation_filter_list, inoutFileList);
	}
}

void ImportResultFilesFromAddRemoveList( const i3::vector<i3::string>& addResultList,
										const i3::vector<i3::string>& removeResultList,
										i3::vector<i3::string>& outList)
{
	outList.clear();
	
	// 일단 이렇게 처리...(	만약 속도부하가 심각하다면 size를 토대로 분기를 때리고,
	//						해시테이블로 전환해서 처리한다..)

	const size_t num_add = addResultList.size();

	for (size_t i = 0 ; i < num_add ; ++i )
	{
		i3::vector<i3::string>::const_iterator it = 
			i3::range::find(removeResultList, addResultList[i]);

		if (it == removeResultList.end() )		
			outList.push_back( addResultList[i] );		
	}
	
}


void ImportEncMappingsFromEncList( const i3::vector<i3::string>& AddRemoveResultList,
								  const i3::vector<i3::string>& EncResultlist,
								  i3::vector<bool>& outEncMappingList )
{
	// Enc와 일치하는 문자열이 발견되면 mappinglist에 true로 기록해주면 된다..
	
	outEncMappingList.clear();
	outEncMappingList.resize(AddRemoveResultList.size(), false);

	const size_t num = EncResultlist.size();

	for (  size_t i = 0 ; i < num ; ++i )
	{
		i3::vector<i3::string>::const_iterator it = 
			i3::range::find( AddRemoveResultList, EncResultlist[i]);
		
		if ( it != AddRemoveResultList.end() )
		{
			size_t dist = i3::distance(AddRemoveResultList.begin(), it);
			outEncMappingList[dist] = true;
		}
	}

}

// 최종 함수...
void ImportResultFilesFromPackScriptUnit(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list,
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& outFileList,
										 i3::vector<bool>&		outEncMappingList,
										 i3::vector<i3::string>& outAddResultList, 
										 i3::vector<i3::string>& outRemoveResultList,
										 i3::vector<i3::string>& outEncResultList)
{

	// 1. add목록 긁어오기
	AddResultFilesFromPackScriptStrings(mediaPath, unit.add_list, filter_list, nation_filter_list, outAddResultList, &unit.add_opt_list);

	// 2. remove목록 긁어오기.
	AddResultFilesFromPackScriptStrings(mediaPath, unit.remove_list, filter_list, nation_filter_list, outRemoveResultList, NULL);

	// 3. enc 목록 긁어오기..
	AddResultFilesFromPackScriptStrings(mediaPath, unit.enc_list, filter_list, nation_filter_list, outEncResultList, NULL);

	// 4. add - remove 처리..
	ImportResultFilesFromAddRemoveList(outAddResultList, outRemoveResultList, outFileList);

	// 5. enc매핑 리스트 획득 처리.
	ImportEncMappingsFromEncList(outFileList, outEncResultList, outEncMappingList);	

}



void ImportResultFilesFromPackScriptUnit(const i3::string& mediaPath, const PackScriptUnit& unit, 
										 const i3::vector<i3::string>& filter_list, 
										 const i3::vector<i3::string>& nation_filter_list,
										 i3::vector<i3::string>& outFileList,
										 i3::vector<bool>&		outEncMappingList)
{
	i3::vector<i3::string>	add_result_list, remove_result_list, enc_result_list;
	
	ImportResultFilesFromPackScriptUnit(mediaPath, unit, filter_list, nation_filter_list, outFileList, outEncMappingList,
		add_result_list, remove_result_list, enc_result_list);
}

void ImportResultFilesFromPackScriptUnitWithoutENC(const i3::string& mediaPath, const PackScriptUnit& unit, 
												   const i3::vector<i3::string>& filter_list,
												   const i3::vector<i3::string>& nation_filter_list,
												   i3::vector<i3::string>& outFileList,
												   i3::vector<i3::string>& outAddResultList, 
												   i3::vector<i3::string>& outRemoveResultList)
{
	
	// 1. add목록 긁어오기
	AddResultFilesFromPackScriptStrings(mediaPath, unit.add_list, filter_list, nation_filter_list, outAddResultList, &unit.add_opt_list);

	// 2. remove목록 긁어오기.
	AddResultFilesFromPackScriptStrings(mediaPath, unit.remove_list, filter_list, nation_filter_list, outRemoveResultList, NULL);

	// 3. add - remove 처리..
	ImportResultFilesFromAddRemoveList(outAddResultList, outRemoveResultList, outFileList);
}

void ImportResultFilesFromPackScriptUnitWithoutENC(const i3::string& mediaPath, const PackScriptUnit& unit, 
												   const i3::vector<i3::string>& filter_list,
												   const i3::vector<i3::string>& nation_filter_list,
												   i3::vector<i3::string>& outFileList)
{
	i3::vector<i3::string>	add_result_list, remove_result_list;

	ImportResultFilesFromPackScriptUnitWithoutENC(mediaPath, unit, filter_list, nation_filter_list, outFileList, 
		add_result_list, remove_result_list);
}

namespace
{
	const char* const g_strDefaultException = "*.sif;*.log;*.i3CharaEditor;*.i3LevelDesign;*.dump;*.svn;*.RSG;*.exe;*.i3Reg;*.ini;*.xml;*.i3RegXML";
}

void InitPackScriptException(PackScriptExceptionBuffer& out1, PackScriptExceptionString& out2)
{
	out1.line_list.clear();
	out1.line_list.push_back(i3::string());
	out1.line_list.back() = "@set WORKING_DIR=\"/W %CD%\"";

	out1.line_list.push_back(i3::string());
	out1.exception_line = 1;
	out1.force_copy_line = std::size_t(-1);

	out2.str_exception = g_strDefaultException;
	out2.str_force_copy.clear();
}

bool ParsePackScriptExceptionLine_Set(const i3::const_char_range& line_range, i3::const_char_range& rem_range);
bool ParsePackScriptExceptionLine_ExceptionCommand( const i3::const_char_range& line_range, i3::string& out);
bool ParsePackScriptExceptionLine_ForceCopyCommand( const i3::const_char_range& line_range, i3::string& out);


bool LoadPackScriptExceptionFile(const i3::wstring& full_path, PackScriptExceptionBuffer& out1, PackScriptExceptionString& out2)
{
	out1.line_list.clear();	out1.exception_line = 0;	out1.force_copy_line = std::size_t(-1);

	out2.str_exception.clear();	out2.str_force_copy.clear();

	i3::vector<char> buff;
	if (!LoadFileIntoBuffer(full_path, buff) )
		return false;
	
	i3::const_char_range rng = i3::as_literal(buff);
	i3::const_char_range line_rng;
	i3::const_char_range rem_rng;

	bool bFoundException = false;
	bool bFoundForceCopy = false;

	while (!rng.empty())
	{
		::ExtractLineRange(rng, line_rng, rng);
		
		out1.line_list.push_back(i3::string());
		
		if ( ParsePackScriptExceptionLine_Set(line_rng, rem_rng) )
		{
			if (!bFoundException && ParsePackScriptExceptionLine_ExceptionCommand(rem_rng, out2.str_exception) )
			{
				out1.exception_line = out1.line_list.size() - 1;
				bFoundException = true;
			}
			else
			if (!bFoundForceCopy && ParsePackScriptExceptionLine_ForceCopyCommand(rem_rng, out2.str_force_copy) )
			{
				out1.force_copy_line = out1.line_list.size() - 1;
				bFoundForceCopy = true;
			}
			else
			{
				out1.line_list.back().assign(line_rng.begin(), line_rng.end());
			}
		}
		else
		{
			out1.line_list.back().assign(line_rng.begin(), line_rng.end());
		}
	}
	
	if ( !bFoundException )
	{
		out1.exception_line = out1.line_list.size();
		out1.line_list.push_back(i3::string());

		out2.str_exception = g_strDefaultException; 
	}

	return true;
}

namespace
{
	bool process_exception_cmd_impl_set(const i3::const_char_range& in_line_range, i3::const_char_range& rem_range)
	{
		if (in_line_range.empty())
			return false;

		i3::const_char_range::iterator it_non_blank = i3::range::find_if(in_line_range, find_non_blank());		// 앞쪽 블랭크 제거..
		i3::const_char_range line_range2( it_non_blank, in_line_range.end());

		if (line_range2.empty() )
			return false;

		i3::const_char_range::iterator it_cmd_end = i3::range::find_if(line_range2, find_blank() );

		if ( it_cmd_end == line_range2.end() )
			return false;							// 추가 인수가 없으면 false로 간주..			

	//   @set 걸러냄..

		i3::const_char_range literal_cmd = i3::as_literal("@set");

		i3::const_char_range cmd_rng(line_range2.begin(), it_cmd_end);

		if ( !i3::generic_is_iequal( cmd_rng, literal_cmd ) )
			return false;
		

	//  다음번 non_blank_char로 이동..없으면 false...

		i3::const_char_range::iterator it_cmd_start = it_cmd_end;	++it_cmd_start;

		it_cmd_start = i3::find_if( it_cmd_start, line_range2.end(), find_non_blank() );

		if ( it_cmd_start == line_range2.end())
			return false;

		rem_range = i3::const_char_range( it_cmd_start, line_range2.end() );

		return true;
	}

	bool process_exception_cmd_impl_CommandName( const i3::const_char_range& in_line_range, i3::const_char_range& rem_range, const char* strCmd)
	{
		i3::const_char_range literal_cmd = i3::as_literal(strCmd);

		if (!i3::generic_is_niequal( in_line_range, literal_cmd, literal_cmd.size() ) )
			return false;

		rem_range = in_line_range;
		rem_range.advance_begin( literal_cmd.size());

		return true;		
	}

	bool process_exception_cmd_impl_skip_quote( const i3::const_char_range& in_line_range, i3::const_char_range& rem_range)
	{
		i3::const_char_range::iterator it_param_start = i3::range::find( in_line_range, '"');
		if (it_param_start == in_line_range.end())
			return false;

		++it_param_start;
		
		rem_range = i3::const_char_range(it_param_start, in_line_range.end());

		return true;
	}

	bool process_exception_cmd_impl_skip_E( const i3::const_char_range& in_line_range, i3::const_char_range& rem_range )
	{
		i3::const_char_range::iterator it_param_start = i3::range::find_if(in_line_range, find_non_blank());
		
		i3::const_char_range literal_cmd = i3::as_literal("/E");

		i3::const_char_range line_range(it_param_start, in_line_range.end());

		if (!i3::generic_is_niequal(line_range, literal_cmd, literal_cmd.size()))
			return false;

		it_param_start += literal_cmd.size();

		if (it_param_start == line_range.end())
			return false;

		if ( !find_blank()(*it_param_start) )
			return false;

		++it_param_start;
		
		rem_range = i3::const_char_range(it_param_start, line_range.end());

		return true;
	}

	void process_exception_cmd_impl_assign( const i3::const_char_range& in_line_range, i3::string& out)
	{
		i3::const_char_range::iterator it_param_start = i3::range::find_if( in_line_range, find_non_blank());

		i3::const_char_range::iterator it_param_end = i3::find(it_param_start, in_line_range.end(), '"');

		out.assign(it_param_start, it_param_end);
		
	}

	bool process_exception_cmd(const i3::const_char_range& line_range, i3::string& out)
	{
		i3::const_char_range rem_rng;

		// 2. Exception 글자 걸러냄..
		if (process_exception_cmd_impl_CommandName(line_range, rem_rng, "EXCEPTION") == false)
			return false;

		// 3. 문자열 블록 얻기..
		// '"' 제거..
		if (process_exception_cmd_impl_skip_quote(rem_rng, rem_rng) == false)
			return false;

		if (process_exception_cmd_impl_skip_E(rem_rng, rem_rng) == false)
			return false;

		process_exception_cmd_impl_assign(rem_rng, out);

		return true;
	}

	bool process_force_copy_cmd(const i3::const_char_range& line_range, i3::string& out)
	{
		i3::const_char_range rem_rng;

		// 2. Exception 글자 걸러냄..
		if (process_exception_cmd_impl_CommandName(line_range, rem_rng, "FORCECOPY_QAPATH") == false)
			return false;

		// 3. 문자열 블록 얻기..
		// '"' 제거..
		if (process_exception_cmd_impl_skip_quote(rem_rng, rem_rng) == false)
			return false;

		process_exception_cmd_impl_assign(rem_rng, out);

		return true;
	}

}

void ExtractPackScriptExceptionFileList(const PackScriptExceptionString& in, PackScriptExceptionFilterList& out)
{
	out.pack_filter_list.clear();	out.qa_filter_list.clear();		out.qa_force_copy_list.clear();

	i3::strtok_fast(out.pack_filter_list, in.str_exception, " ;", true);
	
	i3::strtok_fast(out.qa_force_copy_list, in.str_force_copy, " ;", true);

	if (!out.qa_force_copy_list.empty())
	{
		struct Pred
		{
			const i3::vector<i3::string>& rhs;
			Pred(const i3::vector<i3::string>& rhs) : rhs(rhs) {}
			bool operator()(const i3::string& str) const
			{
				i3::vector<i3::string>::const_iterator it =	i3::find( rhs.begin(), rhs.end(), str);
				return it == rhs.end();			// 찾지 못하면 지운다..
			}
		};

		i3::range::remove_if( out.qa_force_copy_list, Pred(out.pack_filter_list) );
	}
	
	{
		struct Pred
		{
			const i3::vector<i3::string>& rhs;
			Pred(const i3::vector<i3::string>& rhs) : rhs(rhs) {}
			bool operator()(const i3::string& str) const
			{
				i3::vector<i3::string>::const_iterator it =	i3::find( rhs.begin(), rhs.end(), str);
				return it != rhs.end();			// 찾으면 지운다..
			}
		};
		
		i3::range::remove_copy_if(out.pack_filter_list, out.qa_filter_list, Pred(out.qa_force_copy_list) );
	}
	
}

// 현재 이 함수는 쓰이고 있지 않는 것 같다...언젠가 재활용될수 있어 함수를 남겨둔다..
void MakePackScriptExceptionFile(const i3::vector<i3::string>& in, i3::string& out)
{
	out.clear();
	const size_t num = in.size();
	
	if (num == 0) return;

	size_t i = 0;
	for ( ; i < num-1 ; ++i)
	{
		out += in[i];
		out += ';';
	}

	out += in[i];
}

bool ParsePackScriptExceptionLine_Set(const i3::const_char_range& line_range, i3::const_char_range& rem_range)
{
	return process_exception_cmd_impl_set(line_range, rem_range);
}

bool ParsePackScriptExceptionLine_ExceptionCommand( const i3::const_char_range& line_range, i3::string& out)
{
	// 줄 하나에 대해서 처리..
	return process_exception_cmd(line_range, out);
}

bool ParsePackScriptExceptionLine_ForceCopyCommand( const i3::const_char_range& line_range, i3::string& out)
{
	return process_force_copy_cmd(line_range, out);
}


bool SavePackScriptExceptionFile(const i3::wstring& full_path, const PackScriptExceptionBuffer& in1, const PackScriptExceptionString& in2)
{
	i3::vector<char>	buff;

	const size_t num_line = in1.line_list.size();		// 
	size_t i = 0;

	for ( ; i < num_line ; ++i )
	{
		if (in1.exception_line == i)
		{
			i3::const_char_range first_lit = i3::as_literal("@set EXCEPTION=\"/E ");
			buff.insert(buff.end(), first_lit.begin(), first_lit.end());
			buff.insert(buff.end(), in2.str_exception.begin(), in2.str_exception.end());
			buff.push_back('"');	buff.push_back('\r'); buff.push_back('\n');
		}
		else
		if (in1.force_copy_line == i)
		{
			i3::const_char_range first_lit = i3::as_literal("@set FORCECOPY_QAPATH=\"");
			buff.insert(buff.end(), first_lit.begin(), first_lit.end());
			buff.insert(buff.end(), in2.str_force_copy.begin(), in2.str_force_copy.end());
			buff.push_back('"');	buff.push_back('\r'); buff.push_back('\n');
		}
		else
		{
			const i3::string& strData = in1.line_list[i];
			buff.insert(buff.end(), strData.begin(), strData.end());
			buff.push_back('\r');	buff.push_back('\n');
		}
	}
	
	if (in1.exception_line == i )
	{
		i3::const_char_range first_lit = i3::as_literal("@set EXCEPTION=\"/E ");
		buff.insert(buff.end(), first_lit.begin(), first_lit.end());
		buff.insert(buff.end(), in2.str_exception.begin(), in2.str_exception.end());
		buff.push_back('"');	buff.push_back('\r'); buff.push_back('\n');		
	}
	
	if (in1.force_copy_line == std::size_t(-1) && !in2.str_force_copy.empty() )
	{
		i3::const_char_range first_lit = i3::as_literal("@set FORCECOPY_QAPATH=\"");
		buff.insert(buff.end(), first_lit.begin(), first_lit.end());
		buff.insert(buff.end(), in2.str_force_copy.begin(), in2.str_force_copy.end());
		buff.push_back('"');	buff.push_back('\r'); buff.push_back('\n');
	}

	SaveFileFromBuffer(buff, full_path);

	return true;
}


bool ParseNationLine(const i3::const_char_range& line_range, NationList& out);

bool LoadNationList(const i3::wstring& full_path, NationList& out)
{
	out.list.clear();

	i3::vector<char> buff;
	if (!LoadFileIntoBuffer(full_path, buff) )
		return false;
	
	i3::const_char_range rng = i3::as_literal(buff);
	i3::const_char_range line_rng;

	while (!rng.empty())
	{
		::ExtractLineRange(rng, line_rng, rng);
		::ParseNationLine(line_rng, out);		
	}

	return true;
}

bool ParseNationLine(const i3::const_char_range& line_range, NationList& out)
{
	// 줄 하나에 대해서 처리..
	if (line_range.empty())
		return false;

	typedef i3::const_char_range::iterator iterator;

	iterator it_start = i3::range::find_if(line_range, find_non_blank());		// 앞쪽 블랭크 제거..
	iterator it_end       = i3::range::find(line_range, ';');			// ;가 있다면 그것이 end..
	
	iterator it_end_non_blank = i3::rfind_if(it_start, it_end, find_non_blank());

	if (it_end_non_blank != it_end )
		it_end = ++it_end_non_blank;

	if (it_start == it_end)
		return false;
	
	out.list.push_back(i3::string());
	out.list.back().assign(it_start, it_end);
	return true;
}
