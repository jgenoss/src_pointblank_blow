#include "stdafx.h"
#include "PackScriptMgr.h"

#include "PackScriptTree.h"
#include "LoadPackScriptTree.h"
#include "PackScriptContents.h"

#include "PreprocessMessageCallback.h"
#include "PathMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/make_unix_path.h"


PackScriptMgr::PackScriptMgr() : m_root(NULL),
m_file_map(i3::unordered_map<i3::fixed_string, PackScriptFileMapElement>::calculate_bucket_size(20000))
{

}

PackScriptMgr::~PackScriptMgr()
{
	delete m_root;	
}

namespace
{
	
	void ProcessPackScriptFileMap(PackScriptFile* f, const i3::wstring& full_path,
		i3::unordered_map<i3::fixed_string, PackScriptFileMapElement>& inout)
	{

		PackScriptContents	contents;
		ImportPackScriptFile(full_path, contents);
			
		const i3::wstring& wstrMedia = PathMgr::i()->GetMainMediaPath();
		const PackScriptExceptionFilterList& filter_list = PathMgr::i()->GetExceptionList();
		const i3::vector<i3::string>& nation_folder_filter_list = PathMgr::i()->GetNationFolderFilterList();


		i3::string strMedia;
		i3::utf16_to_mb(wstrMedia, strMedia);

		i3::vector<PackScriptUnitPtr>& unit_list = contents.unit_list;
		const size_t num_unit = unit_list.size();
		
		i3::vector<i3::string> file_list;

		// pack_filter_list에서 qa_filter_list로 변경..
		for (size_t i = 0 ; i < num_unit ; ++i )
		{
			ImportResultFilesFromPackScriptUnitWithoutENC(strMedia, *unit_list[i], filter_list.qa_filter_list, nation_folder_filter_list, file_list);
		}
		
		if (contents.copy_unit)
		{
			ImportResultFilesFromPackScriptUnitWithoutENC(strMedia, *contents.copy_unit, filter_list.qa_filter_list, nation_folder_filter_list, file_list);
		}
		
		// file_list 처리 완...
		const size_t num_file = file_list.size();
		
		i3::fixed_string strKey;

		for (size_t i = 0 ; i < num_file ; ++i )
		{
			i3::string path = file_list[i];			// 값으로 복사...
			i3::to_upper(path);
			strKey = path;

			typedef i3::unordered_map<i3::fixed_string, PackScriptFileMapElement>::iterator	map_iterator;

			map_iterator it = inout.find(strKey);
			
			if ( it == inout.end() )
			{
				i3::pair<map_iterator, bool> pr = inout.insert( i3::make_pair( strKey, PackScriptFileMapElement()) );
				it = pr.first;
			}
			
			PackScriptFileMapElement& elem = it->second;
			elem.file_list.push_back(f);
		}
		
	}
	
}


bool	PackScriptMgr::LoadPackScriptFolder()
{
	delete m_root;
	m_file_map.clear();

	const i3::wstring& strFolder = PathMgr::i()->GetPackScriptPath();

	m_root = Load_PackScriptTree_FromFolder(strFolder);
	//
	return m_root != NULL ;
}

void			PackScriptMgr::BuildPackScriptMap(PreprocessMessageCallback* cb)
{
	m_file_map.clear();
	
	if (m_root == NULL)
		return;
	
	const i3::wstring& strFolder = PathMgr::i()->GetPackScriptPath();
	//
	// 일단은 이쪽에서 관리 처리한다.. 충분이 병목이 될수 있는 부분이므로..
	// 나중에는 필요시점때까지 지연 처리해야한다...
	//
	cb->OnTextMessage(L"Collecting PackScript Files...");

	i3::vector<PackScriptFile*> file_list;
	CollectPackScriptFiles(m_root, file_list);
	

	const size_t num_file = file_list.size();

	i3::wstring full_path;
	i3::wstring msg;
	i3::wliteral_formatter fmt(L"Gathering Contents From %s.i3PackScript ...");

	for (size_t i = 0; i < num_file ; ++i)
	{
		PackScriptFile* f = file_list[i];

		f->MakeAbsPath(strFolder, full_path);
		full_path += L".i3PackScript";
		
		i3::sprintf(msg, fmt, f->GetFileTitle());

		cb->OnTextMessage(msg.c_str());

		ProcessPackScriptFileMap(f, full_path, m_file_map);

	}		
	
	cb->OnTextMessage(L"i3PackScript File Mapping Done...");
}

const PackScriptFileMapElement*	PackScriptMgr::FindPackScriptFileMapElement_Upper(const i3::fixed_string& strPath) const
{
	i3::unordered_map<i3::fixed_string, PackScriptFileMapElement>::const_iterator it = m_file_map.find(strPath);

	if (it == m_file_map.end() )
		return NULL;
	
	return &it->second;
}

const PackScriptFileMapElement*	PackScriptMgr::FindPackScriptFileMapElement(const i3::rc_string& strPath) const
{
	i3::string str = strPath;
	i3::to_upper(str);
	i3::make_unix_path(str);
	
	return PackScriptMgr::FindPackScriptFileMapElement_Upper(i3::fixed_string(str) );
}
