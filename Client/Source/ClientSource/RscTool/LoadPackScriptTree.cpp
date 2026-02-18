#include "stdafx.h"
#include "LoadPackScriptTree.h"

#include "PackScriptTree.h"
#include "i3Base/string/system/traverse_file.h"

#include "i3Base/string/compare/generic_is_iequal.h"
//#include "i3Base/itl/stack.h"
#include "i3Base/string/ext/find_fileext_iterator.h"

namespace
{
	struct traverse_file_callback_packscript : i3::traverse_file_callback_wc
	{
		traverse_file_callback_packscript(PackScriptFolder*	root) : 
			m_current_parent(root)
		{
		
		}

		virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )		// 히든 파일 
				return;		
			
			i3::const_wchar_range rngFilename = i3::as_literal(fd.cFileName);
			i3::const_wchar_range::iterator it = i3::find_fileext_iterator(rngFilename);
			
			i3::const_wchar_range rngFileExt = i3::const_wchar_range(it, rngFilename.end());

			if ( !i3::generic_is_iequal(rngFileExt, L"i3packscript") )		// .i3packscript가 아니면 무시된다..
				return;
			
			--it;			// '.' 위치로 땡김..

			i3::const_wchar_range rngFileTitle = i3::const_wchar_range(rngFilename.begin(), it);
		
			PackScriptFile* new_node = new PackScriptFile(rngFileTitle);
			
			m_current_parent->AddFile(new_node);
							
		}

		virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )		// 히든 폴더?? 
				return false;		

			PackScriptFolder*	new_node = new PackScriptFolder( fd.cFileName);	
						
			// 원 검색함수가 폴더들은 모두 큐에 담고, 너비우선검색하므로, 그것을 따라 큐를 만드는게 좋겠다..
			// 이렇게 해놓으면 추가 검색할 필요가 없다..  (너비 우선 검색의 경우에 한해서 구현된 것임)
			m_current_parent->AddChild(new_node);
			//
		
			return true;
		}
		
		// 
		virtual void on_enter_sub_dir(const i3::rc_wstring& base_path, const i3::rc_wstring& child_path) 
		{
			m_current_parent = m_current_parent->GetChildList().back();
		}

		virtual void on_leave_sub_dir(const i3::rc_wstring& base_path, const i3::rc_wstring& parent_path) 
		{
			m_current_parent = m_current_parent->GetParent();
		}

		PackScriptFolder*				m_current_parent;
	};

	
	PackScriptFolder* Load_PackScriptTree_FromFolder_Impl(const i3::wstring& strFolder, 
		const i3::vector<i3::rc_wstring>* filter_list)
	{
		DWORD path_attr = GetFileAttributesW(strFolder.c_str());
		if (path_attr == INVALID_FILE_ATTRIBUTES)
			return NULL;

		PackScriptFolder*	root = new PackScriptFolder(L"Root");		// 그냥 이 이름이 더 무난하다..

		traverse_file_callback_packscript cb(root);

		traverse_file(strFolder, &cb);
		
		return root;
	}
		
}


PackScriptFolder* Load_PackScriptTree_FromFolder(const i3::wstring& strFolder)
{
	return Load_PackScriptTree_FromFolder_Impl(strFolder, NULL);
}


