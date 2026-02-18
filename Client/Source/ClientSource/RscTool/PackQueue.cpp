#include "stdafx.h"
#include "PackQueue.h"
#include "i3Base/smart_ptr/shared_ptr.h"

#include "i3MFCThread.h"
#include "PackScriptContents.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/extract_fileext_iterator_range.h"

#include "i3Base/string/ext/resolve_path_token.h"

#include "i3Base/itl/utility/ref.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/function/function.h"
#include "i3Base/itl/bind/placeholders.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "PackPath.h"
#include "i3Base/string/system/copy_file.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3RSCGenSrc/RSGFile.h"

struct	i3PackFileProgressCallback_Concrete : i3PackFileProgressCallback
{
	const i3::string&				m_PackFileName;		
	PackMode						m_mode;
	i3PackFileProgressCallback_Concrete(const i3::string& strPackFileName, PackMode mode) : 
		m_PackFileName(strPackFileName), m_mode(mode) {}

	virtual void OnProgress_Start(int total_file) 
	{
		using namespace i3;
		PackQueue::i()->push_function(i3::bind(&PackQueue_ProgressCallback::OnPack_Start, _1, m_PackFileName, total_file, m_mode) );
	}

	virtual void OnProgress_BeforePack( const char* szResFilePath, bool is_enc, int curr_file_count, int total_file) 
	{
		using namespace i3;
		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_BeforePack, _1,
			i3::string(szResFilePath), is_enc, curr_file_count, total_file) );
	}

	virtual void OnProgress_AfterPack(const char* szResFilePath, bool is_enc, bool is_success, int curr_file_count, int success_count, int total_file)
	{
		using namespace i3;
		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_AfterPack, _1,
			i3::string(szResFilePath), is_enc, is_success, curr_file_count, success_count, total_file) );
	}

	virtual void OnProgress_End(int curr_file_count, int success_count, int total_file)
	{
		using namespace i3;

		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnPack_End, _1,
			m_PackFileName, curr_file_count, success_count, total_file, m_mode) );
	}
		
};

namespace
{
	void null_fun() {}
}

struct SaveThreadCallback : public i3MFCThreadCallback<SaveThreadCallback>
{

	void operator()(i3MFCThread* owner)
	{
		PackQueue* volatile inst = PackQueue::i();
		while ( !inst ) {	inst = PackQueue::i();	}

		// 무한루프를 굴린다.. 탈출조건을 심고...
		// 큐가 비어있으면 대기 시킨다...WaitForSingleObject / SetEvent 조합..
		{
			save_function_type  fun = &null_fun;		// i3::function에 치명적 버그가 있어.
														// 일단 이걸로 틀어막는다..

			while ( ! inst->is_exit() )
			{
				if (!inst->pop_save_function(fun) )	// 팩파일 아니라, 팩스크립트파일이름이다..
				{
					continue;
				}
				fun();
			}
		}

		inst->set_exit_end();
		return;
	}
	
	static void ProcessQueue_Filename( const PackPathUnitPtr& path_unit_ptr, 
		const PackScriptExceptionFilterListPtr& filter_list_ptr, const vec_string_ptr& nation_filter_list_ptr, const i3::wstring& wstrPath, PackMode curr_mode)
	{
		// 패킹 수행...하면 됨...
		PackScriptContents contents;

		i3::wstring full_path_packscript = path_unit_ptr->PackScriptPath;
		full_path_packscript += L'/';
		full_path_packscript += wstrPath;

		bool res = ImportPackScriptFile( full_path_packscript, contents);
		if (!res) return;

		if ( contents.copy_unit )		// 카피항목의 경우엔 팩스크립트파일이름을 대신 넣어줌.
		{								// 저장용도는 아님...
			i3::utf16_to_mb(wstrPath, contents.copy_unit->pack_filename);
		}

		// 읽은걸 토대로 목록 꾸린다.. (팩파일이 2개이상일수도 있다..)
		ExecutePackScriptContents(contents, *filter_list_ptr, *nation_filter_list_ptr, *path_unit_ptr, curr_mode);
		//
	}

	static void ProcessQueue_ResFileList( const PackPathUnitPtr& path_unit_ptr, 
		const i3::vector<i3::rc_string>& resFileList, const i3::string& pack_filename, PackMode curr_mode)
	{
		ExecuteResFileList_CopyOnly(resFileList, pack_filename, *path_unit_ptr, curr_mode);
	}
	
	static void ProcessQueue_RscTask( 
		const PackPathUnitPtr& path_unit_ptr, 
		const i3::rc_string& rsc_rel_path,
		const i3::vector<i3::rc_string>& rscnode_pathlist)
	{
		// 모드에 따라 양상이 많이 달라지므로, 일단 모두 별개 처리해둔다..
		// 1. Media -> QA .. -> 양쪽 RSG파일을 모두 읽고 그 뒤에 RSG에 대한 노드전달이 우선이다...
		
		static const PackMode curr_mode = PACKMODE_MEDIA_TO_QA_COPY;

	
		i3::string rsg_rel_path = rsc_rel_path;
		i3::change_file_ext(rsg_rel_path, "RSG");

		i3::string strMedia_rsg; i3::utf16_to_mb(path_unit_ptr->MediaPath, strMedia_rsg);
		i3::string strQA_rsg;	 i3::utf16_to_mb(path_unit_ptr->QAPath, strQA_rsg);
		strMedia_rsg += '/';	strMedia_rsg += rsg_rel_path;
		strQA_rsg    += '/';	strQA_rsg += rsg_rel_path;
		
		RscNodeSet  rsgset_media , rsgset_qa;

		if ( ::LoadRSG(strMedia_rsg, rsgset_media) == false )	
		{
			// 로딩실패 .. 일단 놔둠.. (이쪽은 에러에 가까움)
		}

		if ( ::LoadRSG(strQA_rsg, rsgset_qa) == false)
		{
			// 로딩실패 .. 이건 파일의 새 생성으로 볼수도 있다.(에러 아닌걸로 간주)
		}
		
		const size_t num_node = rscnode_pathlist.size();

		//
		using namespace i3;

		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnCopy_Start, _1,
			rsc_rel_path, num_node, curr_mode ) );
		//

		i3::string str_node_path_name;
		bool bRes = false;
		int success_count = 0;

		for (size_t i = 0 ; i < num_node ; ++i)
		{
			const i3::rc_string& node_path = rscnode_pathlist[i];
			
			str_node_path_name = node_path;

			PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_BeforeCopy, 
				_1, str_node_path_name, i + 1, num_node) );

			RSCNode* media_node = ::FindRscNodeByRelPath(rsgset_media, node_path);
			RSCNode* qa_node    = ::FindRscNodeByRelPath(rsgset_qa, node_path);
			
			if (media_node != NULL)			// media rsg파일에 node 존재시만 처리.
			{
				::ReplaceRSCNode(rsgset_qa, qa_node, media_node);	
				bRes = true;	++success_count;
			}
			else
			{
				bRes = false;
			}

			using namespace i3;

			PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_AfterCopy, 
				_1, str_node_path_name, bRes, i + 1, success_count, num_node));
		}

		::SaveRSG(strQA_rsg, rsgset_qa.m_root);
		::ExportIntoRSC(strQA_rsg, rsgset_qa.m_root);

		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnCopy_End, _1, 
			rsc_rel_path, num_node, success_count, num_node, curr_mode) );
				
			
	}

	
	// 기존툴과의 호환성 문제때문에 좀더 상세하게 짠다..(구 i3PackTool 소스를 꼼꼼이 참고해야한다)
	// 이미 존재하는 파일들이 대상이므로 이 안에 파일검증 루틴은 다 뺀다..
	// ENC설정의 경우 찾지 않고 바로 입력되도록 조정한다..
	
	static void ExecutePackScriptContents(const PackScriptContents& contents,
		const PackScriptExceptionFilterList& filter_list, 
		const i3::vector<i3::string>& nation_filter_list,
		const PackPathUnit& path_unit, PackMode mode )
	{
		i3::string src_path;		i3::string dest_path;
		void (*fn)( const PackScriptUnit* , const i3::string& , 
			const PackScriptExceptionFilterList& ,	const i3::vector<i3::string>&,
			const i3::string&, PackMode mode) = NULL;

		switch( mode )
		{
		case PACKMODE_MEDIA_TO_QA_COPY:
			i3::utf16_to_mb(path_unit.MediaPath, src_path);
			i3::utf16_to_mb(path_unit.QAPath, dest_path);
			fn = &SaveThreadCallback::CopyPackScriptUnit;
		break;

		case PACKMODE_QA_TO_LIVE_PACK:
			i3::utf16_to_mb(path_unit.QAPath, src_path);
			i3::utf16_to_mb(path_unit.LivePath, dest_path);
			fn = &SaveThreadCallback::PackPackScriptUnit;
		break;

		}
		
		const size_t num = contents.unit_list.size();
		
		for (size_t i = 0; i < num ; ++i )
		{
			PackScriptUnit* unit = contents.unit_list[i].get();
			fn(unit, src_path, filter_list, nation_filter_list, dest_path, mode);
		}

		PackScriptUnit* unit = contents.copy_unit.get();
		if (unit)
			CopyPackScriptUnit(unit, src_path, filter_list, nation_filter_list, dest_path, mode);
		
	}
		
	static void ExecuteResFileList_CopyOnly(const i3::vector<i3::rc_string>& resFileList, 
		const i3::string& pack_filename, 
		const PackPathUnit& path_unit, PackMode mode )
	{
		
		i3::string src_path;		i3::string dest_path;
	
		switch( mode )
		{
		case PACKMODE_MEDIA_TO_QA_COPY:
			i3::utf16_to_mb(path_unit.MediaPath, src_path);
			i3::utf16_to_mb(path_unit.QAPath, dest_path);
		break;

		case PACKMODE_QA_TO_LIVE_PACK:
			i3::utf16_to_mb(path_unit.QAPath, src_path);
			i3::utf16_to_mb(path_unit.LivePath, dest_path);
		break;

		}	
		
		CopyResFileList(resFileList, pack_filename, src_path, dest_path, mode);
	}

	
	static void CopyPackScriptUnit( const PackScriptUnit* unit, const i3::string& mediaPath, 
		const PackScriptExceptionFilterList& filter_list, 
		const i3::vector<i3::string>& nation_filter_list,
		const i3::string& destCopyPath, PackMode mode)
	{
		i3::vector<i3::string>	resFileList;	

		ImportResultFilesFromPackScriptUnitWithoutENC(mediaPath, *unit, filter_list.qa_filter_list, nation_filter_list, resFileList);
		
		CopyResFileList(resFileList, unit->pack_filename, mediaPath, destCopyPath, mode);
	}
	
	template<class StringContainer>
	static void CopyResFileList( const i3::vector<StringContainer>&	resFileList, 
		const i3::string& pack_filename,
		const i3::string& mediaPath, 
		const i3::string& destCopyPath, PackMode mode)
	{
		
		if (resFileList.empty())
			return;

		const int num_file = resFileList.size();
		//
		using namespace i3;
		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnCopy_Start, _1,
			pack_filename, num_file, mode ) );
		//

		i3::string strSrcPath;
		int success_count = 0;

		for (int i = 0 ; i < num_file ; ++i )
		{
			strSrcPath = mediaPath;
			strSrcPath += '/';		strSrcPath.append(resFileList[i].begin(), resFileList[i].end());

			PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_BeforeCopy, 
				_1, strSrcPath, i + 1, num_file) );

			bool bRes = i3::copy_file(mediaPath, resFileList[i], destCopyPath );

			if (bRes) ++success_count;

			PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnProgress_AfterCopy, 
				_1, strSrcPath, bRes, i + 1, success_count, num_file));
		}

		PackQueue::i()->push_function( i3::bind(&PackQueue_ProgressCallback::OnCopy_End, _1, 
			pack_filename, num_file, success_count, num_file, mode) );
	}


	static void PackPackScriptUnit( const PackScriptUnit* unit, const i3::string& mediaPath, 
		const PackScriptExceptionFilterList& filter_list, 
		const i3::vector<i3::string>& nation_filter_list,
		const i3::string& destPackPath, PackMode mode)
	{
		i3::vector<i3::string>	resFileList;	i3::vector<bool>		resEncList;
		ImportResultFilesFromPackScriptUnit(mediaPath, *unit, filter_list.pack_filter_list, nation_filter_list, resFileList, resEncList);

		if ( resFileList.empty() )
			return;

		i3PackNode* pRootPack = i3PackNode::new_object();
		pRootPack->SetName(mediaPath.c_str());

		const size_t numFile = resFileList.size();

		for (size_t j = 0 ; j < numFile ; ++j )
		{
			AddFileToPack(mediaPath, pRootPack, resFileList[j], resEncList[j]);
		}

		i3::string destPackFilePath = destPackPath;
		destPackFilePath += "/Pack/";
		destPackFilePath += unit->pack_filename;

		i3PackFileProgressCallback_Concrete progress_cb(unit->pack_filename, mode);

		{
			i3PackVolume file;
			file.setRoot(pRootPack);
			pRootPack->Release();

			bool bRes = file.SaveToFile(destPackFilePath.c_str(), &progress_cb);

			if (bRes == false)
			{
				// 저장 실패시..
			}
			else
			{
				// checksum 처리 병행해야한다..
				i3ResourceFile::SetProtectionCode(destPackFilePath.c_str(), "Zepetto Inc.", 12);
				//
			}
		}
		
	}

	static void AddFileToPack(const i3::string& mediaPath, i3PackNode* pRootPack, const i3::string& filePath, bool is_enc)
	{
		i3::const_char_range rng_ext = i3::extract_fileext_iterator_range(filePath); 

		if (i3::generic_is_iequal(rng_ext, "i3Pack"))		// 팩파일 안에 팩파일..
		{
			// 팩 파일은 패킹 대상에서 배제되어야 한다... 일단 비워두고...
			// 필요하다면 에러 구현 추가..
		}
		else
		{
			// 1. 디렉토리노드를 찾거나 없으면 구성한다.
			i3::vector<i3::rc_string> token;
			i3::resolve_path_token(filePath, token);
			
			if ( token.empty() )	// 에러이다..없을리 없다..
				return;
			
			const size_t num_dir = token.size() - 1;
			i3PackNode* node_target = pRootPack;

			for (size_t i = 0 ; i < num_dir ; ++i )
			{
				i3PackNode* node = (i3PackNode*)node_target->FindChildByName( token[i].c_str() );

				if (node == NULL)
				{
					node = i3PackNode::new_object_ref();
					node->SetName(token[i]);
					node_target->AddChild(node);
				}
				node_target = node;
			}
			
			// 2. 파일 사이즈를 구해야한다.. -_-
			
			i3::stack_string full_path = mediaPath;
			full_path += '\\';
			full_path += filePath;

			HANDLE hFile = ::CreateFileA( full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE )
				return;						// 귀찮아서, 일단 false로 끝냄... 못 읽음..
			DWORD dwFileSize = ::GetFileSize(hFile, NULL);		// 귀찮아서 파일사이즈 구함..
			::CloseHandle(hFile);

			node_target->AddFile(token[num_dir].c_str(), 
								(is_enc) ? I3_PACK_FILE_ATTR_SECURE : 0, dwFileSize, 0);
		}
		
	}	

};



PackQueue::PackQueue() : m_exit(false), m_exit_end(false)
{

	m_queue_empty_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pthread = Createi3MFCThread();
	m_pthread->Call( i3::identity<SaveThreadCallback>() );	
}

PackQueue::~PackQueue()
{
	m_exit = true;
	::SetEvent(m_queue_empty_event);

	while( !m_exit_end )
	{
		::SwitchToThread();
	}
	
	m_pthread.reset();
}

void	PackQueue::push_save_function_block_start()
{
	m_cs.lock();
}

void	PackQueue::push_save_function_block_end()
{
	m_cs.unlock();
}

void	PackQueue::push_save_function(const i3::wstring& name, PackMode mode)
{
	if (m_curr_path_unit == NULL)
		return;

	size_t old_num = 0;

	{
		i3::scoped_cs l(m_cs);
		old_num = m_queue_save_function.size();
		m_queue_save_function.push( i3::bind(&SaveThreadCallback::ProcessQueue_Filename, 
			m_curr_path_unit, m_curr_filter_list_ptr, m_curr_nation_filter_list_ptr, name, mode)  );
	}

	if (!old_num)
		::SetEvent(m_queue_empty_event);

}

void	PackQueue::push_save_function( const i3::vector<i3::rc_string>& resFileList, 
									   const i3::string& pack_filename, PackMode mode)
{
	if (m_curr_path_unit == NULL)
		return;

	size_t old_num = 0;

	{
		i3::scoped_cs l(m_cs);
		old_num = m_queue_save_function.size();
		m_queue_save_function.push(i3::bind(&SaveThreadCallback::ProcessQueue_ResFileList, 
			m_curr_path_unit, resFileList, pack_filename, mode) );

	}

	if (!old_num)
		::SetEvent(m_queue_empty_event);	
}

void	PackQueue::push_save_function(const i3::rc_string& rel_rsc_path, 
									   const i3::vector<i3::rc_string>& rscnode_pathlist)
{
	if (m_curr_path_unit == NULL)
		return;

	size_t old_num = 0;
	{
		i3::scoped_cs l(m_cs);
		old_num = m_queue_save_function.size();		
		m_queue_save_function.push(i3::bind(&SaveThreadCallback::ProcessQueue_RscTask, 
			m_curr_path_unit, rel_rsc_path, rscnode_pathlist) );
	}

	if (!old_num)
		::SetEvent(m_queue_empty_event);
}




bool	PackQueue::pop_save_function( save_function_type& fun)
{
	{
		i3::scoped_cs l(m_cs);

		if ( !m_queue_save_function.empty() )
		{	
			fun = m_queue_save_function.front();
			m_queue_save_function.pop();
			::ResetEvent(m_queue_empty_event);	// 리셋해두지 않으면 셋팅 횟수만큼..Wait이 무시된다...
			return true;
		}
	}

	using namespace i3;
	PackQueue::push_function( i3::bind(&PackQueue_ProgressCallback::OnEnterIdle, _1) );

	::WaitForSingleObject(m_queue_empty_event, INFINITE);
	return false;
}



// 부쓰레드에서..
void	PackQueue::push_function(const progress_function_type& fun)
{
	{
		i3::scoped_cs l(m_cs_progress);
		m_queue_progress.push(fun);
	}
}
// 메인 쓰레드에서.
bool	PackQueue::pop_function(progress_function_type& out_fun)
{
	{
		i3::scoped_cs l(m_cs_progress);
		if (! m_queue_progress.empty() )
		{
			out_fun = m_queue_progress.front();
			m_queue_progress.pop();
			return true;
		}
	}

	return false;
}

bool	PackQueue::pop_function()
{
	i3::scoped_cs l(m_cs_progress);
	if (! m_queue_progress.empty() ) {	m_queue_progress.pop();	return true; }
	return false;
}

void	PackQueue::UpdateProgress_InMainThread(PackQueue_ProgressCallback* cb)
{
	if (cb)
	{
		progress_function_type fun;
		while ( pop_function(fun) )		// while로 했더니..렌더링이 너무 지연되는 경우가 있음..
		{								// 일단 if 로 끊어 처리한다..
			fun(cb);
		}
	}
	else
	{
		while( pop_function()) {}
	}
}

void	PackQueue::set_curr_path_unit(PackPathUnit* ptr)	// copy처리해야함..
{
	m_curr_path_unit = i3::make_shared<PackPathUnit>(*ptr);		// 복사생성...
}

void	PackQueue::set_curr_filter_list(const PackScriptExceptionFilterList& filter_list)
{
	m_curr_filter_list_ptr = i3::make_shared<PackScriptExceptionFilterList>(filter_list);		// 복사생성이다..
}

void	PackQueue::set_curr_nation_folder_filter_list(const i3::vector<i3::string>& filter_list)
{
	m_curr_nation_filter_list_ptr = i3::make_shared<i3::vector<i3::string> >(filter_list);
}

