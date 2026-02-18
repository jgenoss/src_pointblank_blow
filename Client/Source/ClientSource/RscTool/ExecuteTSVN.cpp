#include "stdafx.h"
#include "ExecuteTSVN.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/sprintf_formatter.h"

namespace tsvn
{
	
namespace
{
	i3::const_char_range g_strCommand[CMD_NUM];
	i3::const_wchar_range g_wstrCommand[CMD_NUM];
	
	i3::string_formatter	g_fmtCommand( "/command:%s /path:%s /closeonend:%d" );

	i3::wstring_formatter  g_wfmtCommand( L"/command:%s /path:%s /closeonend:%d");

	struct init
	{
		init()
		{
			g_strCommand[CMD_ADD] = i3::as_literal("add");
			g_strCommand[CMD_COMMIT] = i3::as_literal("commit");
			g_strCommand[CMD_UPDATE] = i3::as_literal("update");		// /skipprechecks 옵션살펴볼것.

			g_wstrCommand[CMD_ADD] = i3::as_literal(L"add");
			g_wstrCommand[CMD_COMMIT] = i3::as_literal(L"commit");
			g_wstrCommand[CMD_UPDATE] = i3::as_literal(L"update");
		}
	} _init_ ;
			

	bool ExecuteImpl(CmdType cmd, const i3::const_char_range& full_path, CloseType c, bool async, HANDLE* hProcess)
	{
	
		i3::string str_param = i3::format_string( g_fmtCommand  , g_strCommand[cmd], full_path, int(c) );
		
		if (cmd == CMD_UPDATE)
		{
			str_param += " /skipprechecks";
		}

		SHELLEXECUTEINFOA si = {0}; 

		si.cbSize = sizeof(SHELLEXECUTEINFOA);
		si.lpVerb = "open";
		si.lpFile = "TortoiseProc.exe";
		si.lpParameters = str_param.c_str();
		si.lpDirectory = NULL;
		si.nShow      = SW_SHOWNORMAL;
		si.fMask      = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_NOASYNC;   //SEE_MASK_NOASYNC; 

		BOOL res = ::ShellExecuteExA(&si);
		
		if (hProcess)
			*hProcess = si.hProcess;
		
		if (si.hProcess)
		{
			if (!async)
				::WaitForSingleObject(si.hProcess, INFINITE);
		}

		return res != FALSE;
	}

	bool ExecuteImpl(CmdType cmd, const i3::const_wchar_range& full_path, CloseType c, bool async, HANDLE* hProcess)
	{
		i3::wstring str_param = i3::format_string( g_wfmtCommand, g_wstrCommand[cmd], full_path, int(c) );

		if (cmd == CMD_UPDATE)
		{
			str_param += L" /skipprechecks";
		}

		SHELLEXECUTEINFOW si = {0}; 

		si.cbSize = sizeof(SHELLEXECUTEINFOW);
		si.lpVerb = L"open";
		si.lpFile = L"TortoiseProc.exe";
		si.lpParameters = str_param.c_str();
		si.lpDirectory = NULL;
		si.nShow      = SW_SHOWNORMAL;
		si.fMask      = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_NOASYNC;

		BOOL res = ::ShellExecuteExW(&si);

		if (hProcess)
			*hProcess = si.hProcess;
		
		if (si.hProcess)
		{
			if (!async)
				::WaitForSingleObject(si.hProcess, INFINITE);
		}

		return res != FALSE;
	}

	template<class Ch, class StrRange> inline
	bool ExecuteImpl(CmdType cmd, const i3::iterator_range<const Ch*>& abs_path, 
		const i3::vector<StrRange>& rel_path_list, CloseType c, bool async, HANDLE* hProcess)
	{
		if ( rel_path_list.empty() )
			return false;				// 없으면 일단 false로 리턴..

		i3::basic_string<Ch> path_param;
		const size_t num_list = rel_path_list.size();

		size_t i = 0;
		for ( ; i < num_list-1 ; ++i)
		{
			path_param.append( abs_path.begin(), abs_path.end());
			path_param += Ch('/');
			path_param.append( rel_path_list[i].begin(), rel_path_list[i].end());
			path_param += Ch('*');
		}

		path_param.append( abs_path.begin(), abs_path.end());
		path_param += Ch('/');
		path_param.append( rel_path_list[i].begin(), rel_path_list[i].end());

		return ExecuteImpl(cmd, i3::iterator_range<const Ch*>(path_param), c, async, hProcess);
	}

}

bool Execute(CmdType cmd, const i3::const_wchar_range& full_path, HANDLE& hProcess, CloseType c)
{
	return ExecuteImpl(cmd, full_path, c, true, &hProcess);
}

bool Execute(CmdType cmd, const i3::const_char_range& full_path, HANDLE& hProcess, CloseType c)
{
	return ExecuteImpl(cmd, full_path, c, true, &hProcess);
}

bool Execute(CmdType cmd, const i3::const_char_range& abs_path, 
			 const i3::vector<i3::rc_string>& rel_path_list, HANDLE& hProcess, CloseType c )
{
	return ExecuteImpl(cmd, abs_path, rel_path_list, c, true, &hProcess);	
}

bool Execute(CmdType cmd, const i3::const_wchar_range& abs_path, 
			 const i3::vector<i3::rc_wstring>& rel_path_list, HANDLE& hProcess, CloseType c)
{
	
	return ExecuteImpl(cmd, abs_path, rel_path_list, c, true, &hProcess);
}

bool ExecuteSync(CmdType cmd, const i3::const_wchar_range& full_path, CloseType c )
{
	return ExecuteImpl(cmd, full_path, c, false, NULL);
}

bool ExecuteSync(CmdType cmd, const i3::const_char_range& full_path, CloseType c )
{
	return ExecuteImpl(cmd, full_path, c, false, NULL);
}

bool ExecuteSync(CmdType cmd, const i3::const_wchar_range& abs_path, 
				 const i3::vector<i3::rc_wstring>& rel_path_list, CloseType c)
{
	return ExecuteImpl(cmd, abs_path, rel_path_list, c, false, NULL);	
}

bool ExecuteSync(CmdType cmd, const i3::const_char_range& abs_path, 
				 const i3::vector<i3::rc_string>& rel_path_list, CloseType c)
{
	return ExecuteImpl(cmd, abs_path, rel_path_list, c, false, NULL);	
}

void WaitExecuteEnd( HANDLE hProcess)
{
	::WaitForSingleObject(hProcess, INFINITE);
}

void WaitExecuteEnd( const i3::vector<HANDLE>& process_list, bool bWaitAll )
{
	::WaitForMultipleObjects(process_list.size(), &process_list[0], bWaitAll, INFINITE);
}

void WaitExecuteEnd( const HANDLE* handle_array, size_t num_handle, bool bWaitAll )
{
	::WaitForMultipleObjects(num_handle, handle_array, bWaitAll, INFINITE);
}

}