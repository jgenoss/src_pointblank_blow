#pragma once

//
// 대충 queue쓰고, 대충 크리티컬섹션 넣는다..
// 저장쓰레드쪽에서 파일입출력이 상당하기 때문에, 락프리자료구조나 더블버퍼링같은건 유용하지 않다...
//
#include "i3Base/itl/queue.h"
#include "i3Base/itl/function/function.h"
#include "PackMode.h"

typedef i3::shared_ptr<class i3MFCThread>		i3MFCThreadPtr;

struct PackQueue_ProgressCallback 
{
	virtual ~PackQueue_ProgressCallback() {}
//
	virtual void OnPack_Start( const i3::string& strPackFilePath, int total_file, PackMode mode) {}
	
	virtual void OnProgress_BeforePack( const i3::string& strResFilePath, bool is_enc, int curr_file_count, int total_file) {}
	virtual void OnProgress_AfterPack(const i3::string& strResFilePath, bool is_enc, bool is_success, int curr_file_count, int success_count, int total_file) {}

	virtual void OnPack_End(const i3::string& strPackFilePath, int curr_file_count, int success_count, int total_file, PackMode mode) {}
//

//
	virtual void OnCopy_Start(const i3::string& strPackFilePath, int total_file, PackMode mode) {}

	virtual void OnProgress_BeforeCopy( const i3::string& strResFilePath, int curr_file_count, int total_file) {}
	virtual void OnProgress_AfterCopy( const i3::string& strResFilePath, bool is_success, int curr_file_count, int success_count, int total_file) {}

	virtual void OnCopy_End(const i3::string& strPackFilePath, int curr_file_count, int success_count, int total_file, PackMode mode) {}

	virtual void OnEnterIdle() {}
//
};

namespace
{
	struct _init_sp_counted_impl_empty_ 
	{
		_init_sp_counted_impl_empty_()
		{
			i3::detail::sp_counted_impl_empty<true>::create_explicit();
			i3::detail::sp_counted_impl_empty<false>::create_explicit();
		}
	} _init_sp_counted_impl_empty_var_;
}


typedef i3::function<void (PackQueue_ProgressCallback*) >  progress_function_type;

typedef i3::function<>	save_function_type;


struct PackPathUnit;
typedef i3::shared_ptr<PackPathUnit>		PackPathUnitPtr;

typedef i3::shared_ptr< i3::vector<i3::string> >  vec_string_ptr;

typedef i3::shared_ptr< struct PackScriptExceptionFilterList > PackScriptExceptionFilterListPtr;

__declspec( align(64) )
class PackQueue : public i3::shared_ginst<PackQueue>
{
public:

	PackQueue();
	~PackQueue();
	
	void				set_curr_path_unit(PackPathUnit* ptr);		// copy처리해야함..
	void				set_curr_filter_list(const PackScriptExceptionFilterList& filter_list);
	void				set_curr_nation_folder_filter_list(const i3::vector<i3::string>& filter_list);

	void				push_save_function_block_start();

	void				push_save_function(const i3::wstring& name, PackMode mode);
	void				push_save_function( const i3::vector<i3::rc_string>& resFileList, 
											const i3::string& pack_filename, PackMode mode);
	void				push_save_function(const i3::rc_string& rel_rsc_path, 
											const i3::vector<i3::rc_string>& rscnode_pathlist);

	void				push_save_function_block_end();

//  다음은 메인쓰레드에서 쓰는 업데이트 프로그래스처리이다...
	void				UpdateProgress_InMainThread(PackQueue_ProgressCallback* cb);

private:
	
	bool				pop_save_function( save_function_type& fun);


	bool				is_exit() const { return m_exit; }
	void				set_exit_end() { m_exit_end = true; }
	// 이건 부 쓰레드에서 주쓰레드쪽으로 넘기는 처리이다..
	void				push_function(const progress_function_type& fun);
	bool				pop_function(progress_function_type& out_fun);
	bool				pop_function();


	i3::queue<save_function_type>		m_queue_save_function;
	i3::queue<progress_function_type>	m_queue_progress;

	PackPathUnitPtr				m_curr_path_unit;		// 메인쓰레드에서만 변경하고..
														// 팩스크립트파일 넘길때 레퍼런스카운팅.
	PackScriptExceptionFilterListPtr	m_curr_filter_list_ptr;	//
	vec_string_ptr						m_curr_nation_filter_list_ptr;

	i3::cs						m_cs;
	i3::cs						m_cs_progress;
	
	i3MFCThreadPtr				m_pthread;
	HANDLE						m_queue_empty_event;
	
	volatile bool				m_exit;
	volatile bool				m_exit_end;
	
	friend struct SaveThreadCallback;
	friend struct i3PackFileProgressCallback_Concrete;
};

