#include "stdafx.h"
#include "FileCheck_MD5Thread.h"

#include "i3MFCThread.h"
#include "PackPath.h"
#include "DiffResult_PackScriptResult.h"

#include "PathMode.h"
#include "PackMode.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

#include "FileCheck_PackScriptMgr.h"

#include "../Crypto/MD5.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/itl/svector.h"

#include "FindPefDiff.h"

#include <mmsystem.h>

namespace
{

	class MFCThreadDerived : public i3MFCThread
	{
	public:
		MFCThreadDerived(FileCheck_MD5Queue* owner) : owner(owner) {}
		
		FileCheck_MD5Queue* get_owner() const { return owner; }
	private:
		FileCheck_MD5Queue* owner;
	};


struct FileCheck_ThreadCallback : i3MFCThreadCallback<FileCheck_ThreadCallback>
{
	void operator()(i3MFCThread* t)
	{
		MFCThreadDerived* derived = static_cast<MFCThreadDerived*>(t);
		FileCheck_MD5Queue* owner = derived->get_owner();
		owner->Update_InSubThread();
	}
};

}

FileCheck_MD5Queue::FileCheck_MD5Queue() : m_interrupt(false), m_pause(false), m_idle(true),
	m_ps_result(NULL)
{
	m_h_resume = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pthread  = i3::make_shared<MFCThreadDerived>(this);
	m_path_mode[0] = PATHMODE_MEDIA;
	m_path_mode[1] = PATHMODE_QA;
}

FileCheck_MD5Queue::~FileCheck_MD5Queue()
{
	if (!m_idle)
	{
		Interrupt();

		while (!m_idle) {	::Sleep(10);	}
	}
}

void	FileCheck_MD5Queue::set_curr_path_unit(const PackPathUnit* ptr)		// copy처리해야함..
{
	m_path_unit = i3::make_shared<PackPathUnit>(*ptr);		// 복사생성...
}

void	FileCheck_MD5Queue::SetPackScriptResult(const DiffResult_PackScriptResult* res, PackMode m)
{
	m_ps_result = res;

	if (m == PACKMODE_MEDIA_TO_QA_COPY)
	{
		m_path_mode[0] = PATHMODE_MEDIA;
		m_path_mode[1] = PATHMODE_QA;
	}
	else
	{
		m_path_mode[0] = PATHMODE_QA;
		m_path_mode[1] = PATHMODE_LIVE;
	}
}


namespace
{

	static const DWORD	ReadBlockSize = 16384;
	#define				ENC_READ_UNIT	2048			// ENC용으로 i3PackFile.cpp 참고한다.


	bool GetMD5_Live(i3PackVolume* vol, const i3::string& rel_file_path, Crypto::MD5Handle md5h, 
		Crypto::MD5Result& out, bool& is_enc)
	{
		// LIVE의 경우 되도록 빠르게 i3Pack파일의 내부 파일 포인터 위치를 검색하는게 좋지만...
		// 일단은 신경 쓰지 않는다..
		// ENC여부에 의해서 분기해야한다..
		if (!Crypto::MD5_Init(md5h) )
			return false;

		I3_PACK_FILE_REF* file_info = NULL;
		i3PackNode* root = vol->getRoot();

		if (root == NULL)
			return false;

		i3PackNode* found_node = root->FindPath(rel_file_path.c_str(), &file_info);
		
		if (found_node == NULL || file_info == NULL)
			return false;

		is_enc = (file_info->m_Attr & I3_PACK_FILE_ATTR_SECURE) != 0;

		i3FileStream* stream = vol->getCurrThreadStream();

		stream->SetPosition( file_info->m_Offset, STREAM_BEGIN);
		
		bool bSuccess = true;
		if (is_enc)
		{
			i3::svector<BYTE>	buff(ENC_READ_UNIT);
			const DWORD FileSize = file_info->m_Size;
			INT32 to_read_size;		INT32 read_size;

			for ( DWORD i = 0 ; i < FileSize ; )
			{
				to_read_size = min(ENC_READ_UNIT, FileSize - i);

				read_size = stream->Read(&buff[0], to_read_size);
				
				if (read_size == STREAM_ERR)
				{
					bSuccess = false;	break;
				}

				BitRotateDecript(&buff[0], read_size, 3);
				Crypto::MD5_Update(md5h, &buff[0], read_size);

				i += to_read_size;
			}
		}
		else
		{
			const DWORD dwFileSize = file_info->m_Size;		// 귀찮아서 파일사이즈 구함..
			const DWORD numFixedBlock = dwFileSize / ReadBlockSize;
			const DWORD remBufferSize = dwFileSize % ReadBlockSize;
			INT32 read_size;

			i3::svector<BYTE>	buff(ReadBlockSize);

			for (DWORD i = 0 ; i < numFixedBlock ; ++i)
			{
				read_size = stream->Read(&buff[0], ReadBlockSize);
				
				if (read_size == STREAM_ERR)
				{
					bSuccess = false;	break;
				}
			
				Crypto::MD5_Update(md5h, &buff[0], read_size);
			}

			if (remBufferSize)
			{
				read_size = stream->Read(&buff[0], remBufferSize);
				if (read_size == STREAM_ERR)
					bSuccess = false; 
				else
					Crypto::MD5_Update(md5h, &buff[0], read_size);
			}
		}

		if (bSuccess)
			Crypto::MD5_Final(md5h, out);

		return bSuccess;
	}

	//
	// 일단 16K단위로 읽고 끊는 작업을 반복처리한다.
	//
	

	bool GetMD5_MediaQA(const i3::wstring& base_path, const i3::string& rel_file_path, 
		Crypto::MD5Handle md5h, Crypto::MD5Result& out)
	{
		
		if (!Crypto::MD5_Init(md5h) )
			return false;

		i3::wstring wrel_file_path;
		i3::mb_to_utf16(rel_file_path, wrel_file_path);

		i3::wstring full_path = base_path;
		full_path += L'/';	full_path += wrel_file_path;
		
		DWORD dwFileAccess = GENERIC_READ;
		DWORD dwShareMode  = FILE_SHARE_READ;
		HANDLE hFile = ::CreateFileW( full_path.c_str(), dwFileAccess, dwShareMode, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		const DWORD dwFileSize = ::GetFileSize(hFile, NULL);		// 귀찮아서 파일사이즈 구함..
		const DWORD numFixedBlock = dwFileSize / ReadBlockSize;
		const DWORD remBufferSize = dwFileSize % ReadBlockSize;
		
		i3::svector<BYTE>	buff(ReadBlockSize);
		DWORD dwReadSize;

		for (DWORD i = 0 ; i < numFixedBlock ; ++i)
		{
			BOOL res = ::ReadFile(hFile, &buff[0], ReadBlockSize, &dwReadSize, NULL);
			
			if (res == FALSE) 
			{		
				::CloseHandle(hFile); 
				return false;	
			}
			
			Crypto::MD5_Update(md5h, &buff[0], dwReadSize);
		}

		if (remBufferSize)
		{
			BOOL res = ::ReadFile(hFile, &buff[0], remBufferSize, &dwReadSize, NULL);
			if (res == FALSE ) 
			{ 
				::CloseHandle(hFile); 
				return false;	
			}

			Crypto::MD5_Update(md5h, &buff[0], dwReadSize);
		}
		
		::CloseHandle(hFile);

		Crypto::MD5_Final(md5h, out);
		
		return true;
	}

}


void	FileCheck_MD5Queue::Update_InSubThread()
{
	m_idle = false;

	if (m_ps_result == NULL)
	{
		push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnStart, i3::_1, 0) );
		push_progress_function(i3::bind(&FileCheck_MD5Thread_Callback::OnEnd, i3::_1));
		m_idle = true;
		return;
	}
	
//	const i3::vector<const PackScriptUnitResult*>& v_lhs_res = m_ps_result->in_out_found_lhs_res;
//	const i3::vector<size_t>& v_lhs_idx =  m_ps_result->in_out_found_lhs_string_idx;

//	const i3::vector<const PackScriptUnitResult*>& v_rhs_res =  m_ps_result->in_out_found_rhs_res;
//	const i3::vector<size_t>& v_rhs_idx = m_ps_result->in_out_found_rhs_string_idx;

	const i3::vector< PSUnitResult_MapResFilePtr>& lhs_res_list = m_ps_result->in_out_alist[DIFFRESULT_PS_FOUND_LEFT];
	const i3::vector< PSUnitResult_MapResFilePtr>& rhs_res_list = m_ps_result->in_out_alist[DIFFRESULT_PS_FOUND_RIGHT];

	
	const i3::wstring*		left_base_path;
	const i3::wstring*		right_base_path;

	switch(m_path_mode[0] )
	{
	case PATHMODE_MEDIA:	left_base_path = &m_path_unit->MediaPath;	break;
	case PATHMODE_QA:		left_base_path = &m_path_unit->QAPath;	break;
	case PATHMODE_LIVE:		left_base_path = &m_path_unit->LivePath;	break;
	default:				left_base_path = &m_path_unit->MediaPath;	break;
	}

	switch(m_path_mode[1] )
	{
	case PATHMODE_MEDIA:	right_base_path = &m_path_unit->MediaPath;	break;
	case PATHMODE_QA:		right_base_path = &m_path_unit->QAPath;		break;
	case PATHMODE_LIVE:		right_base_path = &m_path_unit->LivePath;	break;
	default:				right_base_path = &m_path_unit->MediaPath;	break;
	}
	

	const size_t num_list = lhs_res_list.size();
	
	push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnStart, i3::_1, num_list) );

	Crypto::MD5Result out1; 
	Crypto::MD5Result out2;
	Crypto::MD5Handle md5h = Crypto::MD5_CreateHandle();
	i3PackVolume* vol_cache = NULL;
	i3::string   volumename_cache;
	size_t i = 0;
	int    interrupt_l_r = 0;

	for ( ; i < num_list ; ++i)
	{
		
		if ( m_pause )
		{
			push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnPause, i3::_1, i, 0) );
			::WaitForSingleObject(m_h_resume, INFINITE);
			push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnResume, i3::_1, i, 0) );
		}

		if (m_interrupt)
		{
			interrupt_l_r = 0;	break;
		}

		const PSUnitResult_MapResFile* l_resFile = lhs_res_list[i].get();

//		const PackScriptUnitResult* l_res = v_lhs_res[i];
//		size_t						l_idx = v_lhs_idx[i];
//		const i3::string& l_res_name = l_res->result_file_list[l_idx];		// MEDIA or QA

		const PackScriptUnitResult* l_res_unit = l_resFile->unit;
		const i3::string& l_res_name = l_res_unit->result_file_list[ l_resFile->string_idx ];

		push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnLeftFileCheckStart, i3::_1, i) );
		bool bOK1 = GetMD5_MediaQA( *left_base_path, l_res_name, md5h, out1);
		push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnLeftFileCheckEnd, 
			i3::_1, i, bOK1, *reinterpret_cast<UINT128*>(out1.data)  ) );
			
		if ( m_pause )
		{
			push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnPause, i3::_1, i, 1) );
			::WaitForSingleObject(m_h_resume, INFINITE);
			push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnResume, i3::_1, i, 1) );
		}	

		if (m_interrupt)
		{
			interrupt_l_r = 1;	break;
		}

		const PSUnitResult_MapResFile* r_resFile = rhs_res_list[i].get();

//		const PackScriptUnitResult* r_res = v_rhs_res[i];
//		size_t						r_idx = v_rhs_idx[i];
//		const i3::string& r_res_name = r_res->result_file_list[r_idx];		// QA or LIVE(Pack or Not)

		const PackScriptUnitResult* r_res_unit = r_resFile->unit;
		const i3::string& r_res_name =  r_res_unit->result_file_list[ r_resFile->string_idx ];
		
		push_progress_function( i3::bind(&FileCheck_MD5Thread_Callback::OnRightFileCheckStart, i3::_1, i) );

		bool bOK2 = false;
		bool is_enc = false;

		if (m_path_mode[1] == PATHMODE_LIVE && !r_res_unit->pack_filename.empty() )
		{
			if ( r_res_unit->pack_filename != volumename_cache )
			{
				I3_SAFE_RELEASE(vol_cache);
								
				i3::string full_pack_path; i3::utf16_to_mb(*right_base_path, full_pack_path);
				full_pack_path += "/Pack/";		full_pack_path += r_res_unit->pack_filename;

				vol_cache = LoadPackVolume(full_pack_path);
				if (vol_cache)
					volumename_cache = r_res_unit->pack_filename;
				else
					volumename_cache.clear();
			}
			
			if (vol_cache)
			{
				bOK2 = GetMD5_Live(vol_cache, r_res_name, md5h, out2, is_enc);
			}
		}
		else
		{
			bOK2 = GetMD5_MediaQA( *right_base_path, r_res_name, md5h, out2);
		}
		
		bool same = false;
		if (bOK1 && bOK2)
		{
			same = ( *reinterpret_cast<UINT128*>(out1.data) == *reinterpret_cast<UINT128*>(out2.data) );
		}
		
		push_progress_function(i3::bind(&FileCheck_MD5Thread_Callback::OnRightFileCheckEnd, i3::_1, i, 
			bOK2, is_enc, *reinterpret_cast<UINT128*>(out2.data), same));
	}

	
	I3_SAFE_RELEASE(vol_cache);
	Crypto::MD5_DeleteHandle(md5h);

	if (!m_interrupt)
	{
		push_progress_function(i3::bind(&FileCheck_MD5Thread_Callback::OnEnd, i3::_1));
	}
	else
	{
		push_progress_function(i3::bind(&FileCheck_MD5Thread_Callback::OnInterrupt, i3::_1, i, interrupt_l_r));
	}

	m_idle = true;
}


void	FileCheck_MD5Queue::Start()
{
	if (m_idle)
	{
		m_idle = false;
		m_interrupt = false;

		m_pthread->Call( i3::identity<FileCheck_ThreadCallback>());
	}
	else
	{
		Interrupt();
		while (!m_idle) {	::Sleep(10);	}

		m_idle = false;
		m_interrupt = false;
		m_pthread->Call( i3::identity<FileCheck_ThreadCallback>());
	}
}



void	FileCheck_MD5Queue::Pause()
{
	m_pause = true;
}

void	FileCheck_MD5Queue::Interrupt()
{
	m_interrupt = true; 
	Resume();
}

void	FileCheck_MD5Queue::Resume()
{
	if (m_pause)
	{
		m_pause = false;
		::ResetEvent(m_h_resume);
		::SetEvent(m_h_resume);
	}
}

void	FileCheck_MD5Queue::push_progress_function(const FileCheck_MD5Queue::progress_fun_type& fun)
{
	i3::scoped_cs l(m_cs_progress);
	
	m_progress_queue.push(fun);
}

bool	FileCheck_MD5Queue::pop_progress_function(FileCheck_MD5Queue::progress_fun_type& fun)
{
	i3::scoped_cs l(m_cs_progress);

	if (!m_progress_queue.empty() )
	{
		i3::swap(m_progress_queue.front(), fun);
		m_progress_queue.pop();
		return true;
	}
	return false;
}

bool	FileCheck_MD5Queue::pop_progress_function()
{
	i3::scoped_cs l(m_cs_progress);
	if (! m_progress_queue.empty() ) {	m_progress_queue.pop();	return true; }
	return false;
}

void	FileCheck_MD5Queue::UpdateProgress_InMainThread(FileCheck_MD5Thread_Callback* cb)
{
	if (cb)
	{
		DWORD start_time_value = ::timeGetTime();
		DWORD curr_diff_time = 0;
		progress_fun_type fun;

		if (pop_progress_function(fun))
		{
			cb->OnRedrawBlockStart();

			fun(cb);								// 최초 1회에 그전에 RedrawBlock한다..
		
			while (pop_progress_function(fun))
			{
				fun(cb);

				curr_diff_time = ::timeGetTime() - start_time_value;
				
				// 대략 30fps정도 수준을 유지하게만 해주고, 그 이상이면 튕기게 해준다.
				if (curr_diff_time > 100)
					break;
			}

			cb->OnRedrawBlockEnd();
		}
	}
	else
	{
		while( pop_progress_function()) {}
	}
	
}
