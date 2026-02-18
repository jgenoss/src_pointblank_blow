#include "i3SceneDef.h"

#include "i3SGTexChangeManager.h"

#include "i3Base/itl/svector.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/itl/type_traits/remove_reference.h"

#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/extract_filename.h"

#include "i3RegistrySet.h"
#include "i3Registry.h"



#include "i3Base/string/ext/utf16_to_mb.h"		// i3::rc_wstring을 i3::rc_string으로 변경하는데쓰였음..(2014.07.02.수빈)


struct i3SGTexChangeInfo
{
	i3::rc_string ref_i3s_filename;
	i3::rc_string tgt_i3s_filename;
	i3::svector<i3::pair< i3::rc_string, i3::rc_string> >	texture_filelist;
	i3::svector< i3::svector< i3::pair<i3::rc_string, i3::rc_string> > >	lod_texture_filelist;
};

i3SGTexChangeManager::~i3SGTexChangeManager()
{
	i3::cu::for_each_delete(m_clonemap);
}

void	i3SGTexChangeManager::Clear()
{
	i3::cu::for_each_delete_clear(m_clonemap);
}

bool i3SGTexChangeManager::LoadRegistryFile(const char* strFilename)
{
//	i3SGTexChangeManager::Clear();

	i3RegistrySet RegFile;
	i3Registry::Set_LoadStamp(false);

	INT32 nReadBytes = RegFile.LoadRegistrySet( strFilename, REGISTRY_FILE_BINARY);
	if( nReadBytes == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", strFilename );		
		return false;
	}

	i3RegKey * pRootRegKey = RegFile.getRoot();

	
	if ( !i3::generic_is_equal( pRootRegKey->GetNameString(), "i3s reference") )
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s's file root key name is not 'i3s reference'.", strFilename);
		return false;
	}
	
	const INT32 num_folder_name = pRootRegKey->getChildCount();
	i3::stack_string	base_path;
	i3::stack_string	temp_path;
	i3::rc_string	ref_file_path;
	i3::rc_string	tgt_file_path;
	
	i3::stack_string	key_string;

	for (INT32 i = 0; i < num_folder_name ; ++i )
	{
		i3RegKey* folder_key = static_cast<i3RegKey*>(pRootRegKey->getChild(i));
		
		temp_path = folder_key->GetNameString();
		i3::make_unix_path(temp_path);
		base_path = temp_path;
		base_path += '/';
		
		const INT32 num_ref_path = folder_key->getChildCount();

		for ( INT32 j = 0 ; j < num_ref_path ; ++j )
		{
			i3RegKey* ref_path_key = static_cast<i3RegKey*>(folder_key->getChild(j));
			ref_file_path = base_path;
			temp_path = ref_path_key->GetNameString();
			i3::make_unix_path(temp_path);
			ref_file_path += temp_path;
			
			const INT32 num_tgt_path = ref_path_key->getChildCount();

			for (INT32 k = 0 ; k < num_tgt_path ; ++k )
			{
				i3RegKey* tgt_path_key = static_cast<i3RegKey*>(ref_path_key->getChild(k));
				tgt_file_path = base_path;
				temp_path = tgt_path_key->GetNameString();
				i3::make_unix_path(temp_path);
				tgt_file_path += temp_path;
				
				i3::extract_filetitle(temp_path, key_string);
				i3::to_lower(key_string);
				
				i3::pair< i3::unordered_map<i3::rc_string, i3SGTexChangeInfo*>::iterator, bool> it_pair = m_clonemap.insert( 
					i3::make_pair( i3::rc_string(key_string), (i3SGTexChangeInfo*)nullptr) );

				if ( it_pair.second == true )
				{
					i3SGTexChangeInfo* pNewObj = new i3SGTexChangeInfo;
					NetworkDump_Malloc( pNewObj, sizeof( i3SGTexChangeInfo), __FILE__, __LINE__);
					pNewObj->ref_i3s_filename = ref_file_path;
					pNewObj->tgt_i3s_filename = tgt_file_path;
					(it_pair.first)->second = pNewObj;

					FillTexturePairs( tgt_path_key, pNewObj );

				}
			}
		}
	}
			
	return true;
}

void	i3SGTexChangeManager::FillTexturePairs(const i3RegKey* tgt_key, i3SGTexChangeInfo* pInfo)
{
	const INT32 num_tex_pair = const_cast<i3RegKey*>(tgt_key)->GetDataCount();

	pInfo->texture_filelist.resize(num_tex_pair);		// 고정..
	
	i3::rc_string	to_lower_path1;
	i3::string		to_lower_path2;

	for (INT32 i = 0 ; i < num_tex_pair ; ++i )
	{
		if ( i3RegString* pData = i3::same_cast<i3RegString*>(  const_cast<i3RegKey*>(tgt_key)->GetData(i) ) )
		{
			i3::to_lower_copy(to_lower_path1, pData->GetNameString());
			i3::make_unix_path(to_lower_path1);
			
			
			i3::utf16_to_mb( pData->getValue(), to_lower_path2);
			i3::to_lower(to_lower_path2);
			i3::make_unix_path(to_lower_path2);

			i3::pair<i3::rc_string, i3::rc_string> p_v( to_lower_path1, i3::rc_string(to_lower_path2) ); 
			pInfo->texture_filelist[i] = p_v;
		}
	}
	
	const INT32 numLODKey = const_cast<i3RegKey*>(tgt_key)->getChildCount();
	
	if (numLODKey > 0)
	{
		pInfo->lod_texture_filelist.resize(numLODKey);

		for (INT32 i = 0; i < numLODKey ; ++i )
		{
			i3::svector<i3::pair<i3::rc_string, i3::rc_string> >& curr_lod_list = pInfo->lod_texture_filelist[i];

			i3RegKey* lodKey = static_cast<i3RegKey*>( const_cast<i3RegKey*>(tgt_key)->getChild(i) );
			
			const INT32 numData = lodKey->GetDataCount();
			curr_lod_list.resize(numData);

			for (INT32 j = 0 ; j <numData ; ++j)
			{
				if ( i3RegString* pData = i3::same_cast<i3RegString*>(  lodKey->GetData(j) ) )
				{
					i3::to_lower_copy(to_lower_path1, pData->GetNameString());
					i3::make_unix_path(to_lower_path1);

					i3::utf16_to_mb( pData->getValue(), to_lower_path2);
					i3::to_lower(to_lower_path2);
					i3::make_unix_path(to_lower_path2);

					i3::pair<i3::rc_string, i3::rc_string> p_v( to_lower_path1, i3::rc_string(to_lower_path2) ); 
					curr_lod_list[j] = p_v;
				}
			}
		}
	}

}	

const i3SGTexChangeInfo*	i3SGTexChangeManager::FindTexChangeInfo(const char* i3sFilePath) const		// 일단은 Weapon부터 시작하는 경로/파일명..
{
	i3::stack_string lower_title;
	i3::extract_filetitle(i3sFilePath, lower_title);
	i3::to_lower(lower_title);

	i3::unordered_map<i3::rc_string, i3SGTexChangeInfo*>::const_iterator it = 
		m_clonemap.find(i3::rc_string(lower_title));		// 원본을 값으로 가졌는지 조사..
	return (it == m_clonemap.end()) ? nullptr : it->second;
}

i3::rc_string	i3SGTexChangeManager::FindTargetTexturePath( const i3SGTexChangeInfo* info, const char* src_texture_path)
{
	i3::rc_string lower_path;
	i3::to_lower_copy(lower_path, src_texture_path);
	i3::make_unix_path(lower_path);

	const size_t num_tex_pair =	info->texture_filelist.size();
	for ( size_t i = 0 ; i < num_tex_pair ; ++i )
	{
		const i3::pair< i3::rc_string, i3::rc_string>& p = info->texture_filelist[i];

		if ( lower_path == p.first )
			return p.second;
	}
	return i3::rc_string();
}

i3::rc_string	i3SGTexChangeManager::FindLODTargetTexturePath(const i3SGTexChangeInfo* info, INT32 lod_idx, const char* src_texture_path)
{
	i3::rc_string lower_path;
	i3::to_lower_copy(lower_path, src_texture_path);
	i3::make_unix_path(lower_path);

	const i3::svector<i3::pair<i3::rc_string, i3::rc_string> >& curr_lod_list = info->lod_texture_filelist[lod_idx];

	const size_t num_tex_pair =	curr_lod_list.size();
	for ( size_t i = 0 ; i < num_tex_pair ; ++i )
	{
		const i3::pair< i3::rc_string, i3::rc_string>& p = curr_lod_list[i];

		if ( lower_path == p.first )
			return p.second;
	}
	return i3::rc_string();
}

INT32			i3SGTexChangeManager::GetNumLOD(const i3SGTexChangeInfo* info)
{
	return (INT32) info->lod_texture_filelist.size();
}

i3::rc_string	i3SGTexChangeManager::GetRefi3sFilename(const i3SGTexChangeInfo* info)
{
	return info->ref_i3s_filename;
}

i3::rc_string	i3SGTexChangeManager::GetTgti3sFilename( const i3SGTexChangeInfo* info)
{
	return info->tgt_i3s_filename;
}

void			i3SGTexChangeManager::GatherAllTargetTexturePaths(const i3SGTexChangeInfo* info, i3::vector<i3::rc_string>& out_texture_paths)
{
	out_texture_paths.clear();

	const i3::literal_range str_null = "(null)";

	const size_t num_non_lod_tex = info->texture_filelist.size();

	for (size_t i = 0 ; i < num_non_lod_tex ; ++i)
	{
		const i3::rc_string& tgt_tex_filename = info->texture_filelist[i].second;
		if ( i3::generic_is_equal(tgt_tex_filename, str_null) == false )
			out_texture_paths.push_back(tgt_tex_filename);
	}

	
	const size_t num_lod = info->lod_texture_filelist.size();

	for (size_t i = 0 ; i < num_lod ; ++i)
	{
		const i3::svector<i3::pair<i3::rc_string, i3::rc_string> >& vLod = info->lod_texture_filelist[i];

		const size_t num_lod_tex = vLod.size();

		for (size_t j = 0 ; j < num_lod_tex ; ++j)
		{
			const i3::rc_string& tgt_tex_filename = vLod[j].second;
			if ( i3::generic_is_equal(tgt_tex_filename, str_null) == false)
				out_texture_paths.push_back(vLod[j].second);
		}
	}
	
	// 모두 소문자에 unix directory char('/')를 쓰므로 그냥 단순이 duplicate 제거한다...
	i3::vu::remove_duplicates(out_texture_paths);	
	
}
