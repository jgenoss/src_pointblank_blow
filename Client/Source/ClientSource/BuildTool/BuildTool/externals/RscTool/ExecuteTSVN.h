#pragma once


// 아래 항목을 주로 참고해야한다..
// http://tortoisesvn.net/docs/release/TortoiseSVN_en/tsvn-automation.html#tsvn-automation-basics

namespace tsvn
{
//	STILL_ACTIVE 
	enum CmdType
	{
		CMD_COMMIT, CMD_UPDATE, CMD_ADD, CMD_CLEANUP, CMD_NUM,
	};

	enum CloseType
	{
		CLOSE_MANUAL = 0,
		CLOSE_NO_ERR = 1,
		CLOSE_NO_ERR_CONFLICT = 2,
		CLOSE_NO_ERR_CONFLICT_MERGE = 3,
	};

	bool Execute(CmdType cmd, const i3::const_wchar_range& full_path, HANDLE& hProcess, CloseType c = CLOSE_NO_ERR_CONFLICT );
	bool Execute(CmdType cmd, const i3::const_char_range& full_path, HANDLE& hProcess, CloseType c = CLOSE_NO_ERR_CONFLICT );


	bool Execute(CmdType cmd, const i3::const_wchar_range& abs_path, 
				 const i3::vector<i3::rc_wstring>& rel_path_list, HANDLE& hProcess, CloseType c = CLOSE_NO_ERR_CONFLICT);
	bool Execute(CmdType cmd, const i3::const_char_range& abs_path, 
		const i3::vector<i3::rc_string>& rel_path_list, HANDLE& hProcess, CloseType c = CLOSE_NO_ERR_CONFLICT);
	
	
	void WaitExecuteEnd( HANDLE hProcess);
	void WaitExecuteEnd( const i3::vector<HANDLE>& process_list, bool bWaitAll = true);
	void WaitExecuteEnd( const HANDLE* handle_array, size_t num_handle, bool bWaitAll = true);


	//bool ExecuteSync(CmdType cmd, const i3::const_wchar_range& full_path, CloseType c = CLOSE_NO_ERR_CONFLICT );
	bool ExecuteSync(CmdType cmd, const i3::const_char_range& full_path, CloseType c = CLOSE_NO_ERR_CONFLICT );

	bool ExecuteSync(CmdType cmd, const i3::const_wchar_range& abs_path, 
		const i3::vector<i3::rc_wstring>& rel_path_list, CloseType c = CLOSE_NO_ERR_CONFLICT);
	bool ExecuteSync(CmdType cmd, const i3::const_char_range& abs_path, 
		const i3::vector<i3::rc_string>& rel_path_list, CloseType c = CLOSE_NO_ERR_CONFLICT);



}

// 
//Since some of the commands can take a list of target paths (e.g. committing several specific files) 
// the /path parameter can take several paths, separated by a * character. 
//
// 테스트는 해봐야겠지만...경로쪽은 '*'를 토대로 토큰 분리가 가능한 모양이다..
