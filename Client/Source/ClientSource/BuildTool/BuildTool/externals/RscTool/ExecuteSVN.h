#pragma once

namespace svn
{
	struct LogInfo
	{
		int							revision;
		i3::rc_string				writer;
		i3::rc_string				date;
	//	i3::vector<i3::rc_string>	log_data_list;
	};
	typedef i3::shared_ptr<LogInfo>	LogInfoPtr;	
	
	struct LogInfoList
	{
		i3::vector<LogInfoPtr>		info_list;
	};
	

	bool GetLogInfos( const i3::const_char_range& full_path, int num_read, LogInfoList& inout, int start_rev = INT_MAX);
	
	bool GetLogText(const i3::const_char_range& full_path, i3::string& outText, int revision = INT_MAX);

	bool GetConflictStatusFiles(const i3::const_char_range& full_path, i3::vector<i3::rc_string>& outFiles);
	
	void Cleanup(const i3::rc_string& full_path, const i3::rc_string& sqlite3_fullpath);
	
	void ProcessConflictStatus(const i3::rc_string& full_path, const i3::rc_string& sqlite3_fullpath, i3::vector<i3::rc_string>& out_conflict_abspaths);


	bool FindURL(const i3::const_char_range& full_path, i3::string& out_url);

	bool FindLatestRevision( const i3::const_char_range& full_path, int& out_rev);
	bool FindBaseRevision( const i3::const_char_range& full_path, int& out_rev);

//	bool FindRevision( const i3::const_wchar_range& full_path, i3::vector<int>& out_rev);
//	bool FindRevision( const i3::const_wchar_range& full_path, int limit_num, i3::vector<int>& out_rev);

	// URL의 경우 case_sensitive가 조건이 된다...(이게 상당이 까다로운 제약임)
	// 일단 FindURL과 섞어서 처리하도록 한다..
	bool ExportSync( const i3::const_char_range& source_url, const i3::const_char_range& dest_path);
	bool ExportSync( int rev, const i3::const_char_range& source_url, const i3::const_char_range& dest_path);

	struct PipeProcessHandle
	{
		HANDLE hPipeRead;
		HANDLE hProcess;
	};

	bool LoadDataFromPipeSync(const i3::const_char_range& command, i3::vector<BYTE>& inout);


	bool CreatePipeProcess(const i3::const_char_range& command, PipeProcessHandle& out_handle);
	bool WatchPipeProcess( PipeProcessHandle& handle, i3::vector<BYTE>& inout);
	void SleepForPipeProcess( const PipeProcessHandle& handle, DWORD millsec);

	bool ExecuteProcess(const i3::const_char_range& command, HANDLE& out_handle);
	void WaitProcessDone(HANDLE handle);

}

