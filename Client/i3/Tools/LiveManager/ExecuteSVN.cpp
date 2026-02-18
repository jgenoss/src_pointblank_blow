#include "stdafx.h"
#include "ExecuteSVN.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/atoi.h"

#include "PackScriptContents.h"				// ExtractLineRange때문에 사용..나중에 변경할 것..

#include "i3Base/string/ext/strtok_fast.h"
#include "i3Base/itl/range/algorithm/find.h"

namespace svn
{
	
bool CreatePipeProcess(const i3::const_char_range& command, PipeProcessHandle& out_handle)
{
	out_handle.hPipeRead = NULL;	out_handle.hProcess  = NULL;	

	HANDLE read_pipe = NULL;
	HANDLE write_pipe = NULL;
	
	{
		SECURITY_ATTRIBUTES attr;
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);	attr.bInheritHandle = TRUE;	attr.lpSecurityDescriptor = NULL;
	
		// 마지막 인수는 1회에 적재될 버퍼사이즈임.(0일때 디폴트).
		if ( ::CreatePipe(&read_pipe, &write_pipe, &attr, 0) == FALSE )		
		{
			return false;
		}
	}
	
	{
		PROCESS_INFORMATION pi;		::memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		STARTUPINFOA		si;		::memset(&si, 0, sizeof(STARTUPINFOA));
		
		si.cb = sizeof(STARTUPINFOA);
		si.hStdError = NULL;
		si.hStdOutput = write_pipe;
		si.hStdInput = NULL;
		si.dwFlags = STARTF_USESTDHANDLES;			// 콘솔 입출력에서는 이 옵션이 필수라고 함..
		si.wShowWindow = SW_HIDE;

		BOOL cp_res = ::CreateProcessA(NULL, const_cast<char*>(&*command.begin()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		
		if (cp_res == FALSE)
		{
			return false;
		}
		
		::CloseHandle(pi.hThread);
		::CloseHandle(write_pipe);
		
		out_handle.hPipeRead = read_pipe;
		out_handle.hProcess = pi.hProcess;
	}

	return true;
}

// 
bool WatchPipeProcess( PipeProcessHandle& handle, i3::vector<BYTE>& inout)
{
	if (handle.hProcess == 0)
		return false;

	DWORD dwTotal = 0;

	BOOL res = ::PeekNamedPipe( handle.hPipeRead, NULL, 0, NULL, &dwTotal, NULL);	// 게임루프당 peek한번만 처리..

	if (res && dwTotal > 0)
	{
		size_t start_pos = inout.size();
		inout.resize(start_pos + dwTotal);		// 일단 늘려놓고 덮어놓고 시작...
		
		DWORD dwRead = 0;	

		while ( dwTotal > 0 )
		{
			::ReadFile( handle.hPipeRead, &inout[start_pos], dwTotal, &dwRead, NULL);
			dwTotal   -= dwRead;
			start_pos += dwRead;
		}
	}
	
	if (::WaitForSingleObject(handle.hProcess, 0) == WAIT_OBJECT_0)		// 프로세스 실행 종료 시점..
	{
		::CloseHandle(handle.hProcess);		handle.hProcess = 0;
		::CloseHandle(handle.hPipeRead);	handle.hPipeRead = 0;
		return false;
	}

	return true;
}

bool ExecuteProcess(const i3::const_char_range& command, HANDLE& out_handle)
{
	out_handle = NULL;
	{
		PROCESS_INFORMATION pi;		::memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		STARTUPINFOA		si;		::memset(&si, 0, sizeof(STARTUPINFOA));

		si.cb = sizeof(STARTUPINFOA);
		si.hStdError = NULL;
		si.hStdOutput = NULL;
		si.hStdInput = NULL;
		si.dwFlags = STARTF_USESTDHANDLES;			// 콘솔 입출력에서는 이 옵션이 필수라고 함..
		si.wShowWindow = SW_HIDE;

		BOOL cp_res = ::CreateProcessA(NULL, const_cast<char*>(&*command.begin()), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

		if (cp_res == FALSE)
		{
			return false;
		}

		::CloseHandle(pi.hThread);
	
		out_handle = pi.hProcess;
	}

	return true;	
}

void WaitProcessDone(HANDLE handle)
{
	::WaitForSingleObject(handle, INFINITE);
}



void SleepForPipeProcess( const PipeProcessHandle& handle, DWORD millsec)
{
	::WaitForSingleObject(handle.hProcess, millsec);
}

bool LoadDataFromPipeSync(const i3::const_char_range& command, i3::vector<BYTE>& inout)
{
// 	PipeProcessHandle ph;
// 	if ( !CreatePipeProcess(command, ph) )
// 		return false;
// 	// 여기서는 동기처리한다.
// 
// 	while ( WatchPipeProcess(ph, inout) )
// 		SleepForPipeProcess(ph, 1);

	return true;
}

namespace
{
	
	void Process_LogInfo(const i3::const_char_range& line_rng, 
						 i3::const_char_range& inout_rem_rng, LogInfoList& inout_list)
	{
		
		i3::vector<i3::const_char_range>	token_list;
		token_list.reserve(3);
		i3::strtok_fast(token_list, line_rng, '|');
		
		const size_t num_token = token_list.size();

		if (num_token < 3) return;			// 뭔가 잘못된걸로 간주..
		
		LogInfoPtr log = i3::make_shared<LogInfo>();

		i3::const_char_range& rng0 = token_list[0];
		
		rng0.advance_begin(1);	rng0.advance_end(-1);	// 'r' 제외..
		
		log->revision = i3::atoi(rng0);
		
		i3::const_char_range& rng1 = token_list[1];
		rng1.advance_begin(1);	rng1.advance_end(-1);
		log->writer.assign(rng1.begin(), rng1.end());

		i3::const_char_range& rng2 = token_list[2];
		rng2.advance_begin(1);	rng2.advance_end(-1);
		log->date.assign(rng2.begin(), rng2.end());
	
		inout_list.info_list.push_back(log);
	}
	
	void Process_LogText(const i3::const_char_range& line_rng, i3::const_char_range& inout_rem_rng, i3::string& outText)
	{
		i3::const_char_range line_num_rng = line_rng;

		for (size_t i = 0 ; i < 3 ; ++i)
		{
			i3::const_char_range::iterator it =	i3::range::find(line_num_rng, '|');
			if (it == line_num_rng.end())
				return;

			line_num_rng.set_begin(++it);
		}
		
		line_num_rng.advance_begin(1);
		line_num_rng.set_end(i3::range::find(line_num_rng, ' '));

		const int num_data = i3::atoi(line_num_rng);
		
		i3::const_char_range  data_line_rng;

		for (int i = 0 ; i < num_data ; ++i )
		{
			::ExtractLineRange(inout_rem_rng, data_line_rng, inout_rem_rng);
			if ( data_line_rng.empty() )
				::ExtractLineRange(inout_rem_rng, data_line_rng, inout_rem_rng);

			outText.append(data_line_rng.begin(), data_line_rng.end());
			outText += "\r\n";
		}

	}
	
}


bool GetLogInfos( const i3::const_char_range& full_path, int num_read, LogInfoList& inout, int start_rev)
{
	i3::string strCmd;
	
	if (start_rev == INT_MAX && !inout.info_list.empty())
	{
		LogInfo* info = inout.info_list.back().get();

		if (info->revision > 0)
			start_rev = info->revision - 1;
	}
	
	if (start_rev != INT_MAX)
		strCmd = i3::format_string( "svn.exe log -q -l %d -r %d:0 \"%s\"", num_read, start_rev, full_path);
	else
		strCmd = i3::format_string( "svn.exe log -q -l %d -r HEAD:0 \"%s\"", num_read, full_path);
	
	i3::vector<BYTE> buff;

 	if (!LoadDataFromPipeSync(strCmd, buff))
 		return false;

	if (buff.empty())			// 결과가 없는것은 에러로 간주..
		return false;
	
	const size_t prev_list_size = inout.info_list.size();

	i3::const_char_range rng = i3::const_char_range(reinterpret_cast<const char*>(buff.begin()), 
		reinterpret_cast<const char*>(buff.end()));

	i3::const_char_range line_rng;

	while (!rng.empty())
	{
		::ExtractLineRange(rng, line_rng, rng);

		if (line_rng.empty())
			continue;
		if (line_rng[0] == 'r')
		{
			Process_LogInfo( line_rng, rng, inout);
		}
	}
	
	return inout.info_list.size() != prev_list_size;
}

bool GetLogText(const i3::const_char_range& full_path, i3::string& outText, int revision)
{
	outText.clear();

	i3::string strCmd;

	if (revision == INT_MAX )
	{
		strCmd = i3::format_string( "svn.exe log -r HEAD \"%s\"", full_path);
	}
	else
	{
		strCmd = i3::format_string( "svn.exe log -r %d \"%s\"", revision, full_path);
	}
		
	i3::vector<BYTE> buff;

 	if (!LoadDataFromPipeSync(strCmd, buff))
 		return false;

	if (buff.empty())			// 결과가 없는것은 에러로 간주..
		return false;

	i3::const_char_range rng = i3::const_char_range(reinterpret_cast<const char*>(buff.begin()), 
		reinterpret_cast<const char*>(buff.end()));

	i3::const_char_range line_rng;

	while (!rng.empty())
	{
		::ExtractLineRange(rng, line_rng, rng);

		if (line_rng.empty())
			continue;
		if (line_rng[0] == 'r')
		{
			Process_LogText( line_rng, rng, outText);
			break;
		}
	}

	return !outText.empty();
}

namespace
{
	bool FindRevisionWithString(const i3::const_char_range& full_path, const i3::const_char_range& str_rev,
		int& out_rev)
	{
		i3::string strCmd = i3::format_string( "svn.exe log -q -l 1 -r %s:0 \"%s\"", str_rev, full_path);

		i3::vector<BYTE> buff;
 
 		if (!LoadDataFromPipeSync(strCmd, buff))
 			return false;

		if (buff.empty())			// 결과가 없는것은 에러로 간주..
			return false;

		// 여기서는 문자열 파싱..
		i3::const_char_range rng = i3::const_char_range(reinterpret_cast<const char*>(buff.begin()), 
			reinterpret_cast<const char*>(buff.end()));
		i3::const_char_range line_rng;

		while (!rng.empty())
		{
			::ExtractLineRange(rng, line_rng, rng);

			if ( !line_rng.empty() )
			{
				if ( line_rng[0] == 'r' )
				{
					i3::const_char_range::iterator it_beg = line_rng.begin();	++it_beg;
					i3::const_char_range::iterator it_end = line_rng.end();
					i3::const_char_range::iterator it_found = i3::find( it_beg, it_end, ' ');

					if (it_found != it_end )
					{
						out_rev = i3::atoi( i3::const_char_range(it_beg, it_found));
						return true;
					}
				}
			}
		}

		return false;
	}

}

bool FindLatestRevision( const i3::const_char_range& full_path, int& out_rev)
{
	return FindRevisionWithString(full_path, i3::as_literal("HEAD"), out_rev);
}

bool FindBaseRevision( const i3::const_char_range& full_path, int& out_rev)
{
	return FindRevisionWithString(full_path, i3::as_literal("BASE"), out_rev);
}

bool FindURL(const i3::const_char_range& full_path, i3::string& out_url)
{
	i3::string wstrCmd = i3::format_string( "svn.exe info \"%s\"", full_path);
	
	i3::vector<BYTE> buff;

	if (!LoadDataFromPipeSync(wstrCmd, buff))
		return false;

	if (buff.empty())			// 결과가 없는것은 에러로 간주..
		return false;
	

	// 여기서는 문자열 파싱..
	i3::const_char_range rng = i3::const_char_range(reinterpret_cast<const char*>(buff.begin()), 
		reinterpret_cast<const char*>(buff.end()));
	i3::const_char_range line_rng;

	i3::const_char_range rngURL = i3::as_literal("URL: ");

	while (!rng.empty())
	{
		::ExtractLineRange(rng, line_rng, rng);

		if ( !line_rng.empty() )
		{
			if (i3::contain_string_head(line_rng, rngURL))
			{
				line_rng.advance_begin(rngURL.size());
				
				out_url.assign(line_rng.begin(), line_rng.end());
				
				return true;
			}
			
		}
	}

	return false;
	
}

bool ExportSync( const i3::const_char_range& source_url, const i3::const_char_range& dest_path)
{
	i3::string strCmd = i3::format_string( "svn.exe export --force -q \"%s\" \"%s\"", source_url, dest_path);
	
	HANDLE h;
	if (!ExecuteProcess(strCmd, h))
		return false;
	
	WaitProcessDone(h);
	::CloseHandle(h);
			
	return true;
}

bool ExportSync( int rev, const i3::const_char_range& source_url, const i3::const_char_range& dest_path)
{
	i3::string strCmd = i3::format_string( "svn.exe export --force -q -r %d \"%s\" \"%s\"", rev, source_url, dest_path);

	HANDLE h;
	if (!ExecuteProcess(strCmd, h))
		return false;

	WaitProcessDone(h);
	::CloseHandle(h);

	return true;

}


}
