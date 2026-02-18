#include "stdafx.h"
#include "FileCheck_PackScriptMgr.h"

#include "PackScriptMgr.h"
#include "PathMgr.h"
#include "PackScriptTree.h"

#include "PreprocessMessageCallback.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/make_unix_path.h"

#include "FindPefDiff.h"
#include "DiffResult_PackScriptResult.h"
#include "PathMode.h"
#include "PackMode.h"

#include "LoadRSCFile.h"


namespace
{
	//
	// LIVE쪽은 copy_unit은 기존 것을 쓰고, unit_list쪽은 직접 i3Pack파일을 열어 처리한다..
	//

	void		BuildPackScriptMapImpl(const i3::wstring& base_path, int l_r, PathMode m, 
		const i3::vector<i3::wstring>& rel_ps_path_list, const i3::vector<PackScriptContents>& contents_list,
		i3::vector<PackScriptResultPtr>& ps_result_list)
	{
		i3::string str_base_path;	i3::utf16_to_mb(base_path, str_base_path);

		const PackScriptExceptionFilterList& filter_list = PathMgr::i()->GetExceptionList();
		const i3::vector<i3::string>& nation_folder_filter_list = PathMgr::i()->GetNationFolderFilterList();
		
		i3::wstring msg;

		const size_t num_list = contents_list.size();
		
		ps_result_list.resize(num_list);

		for (size_t i = 0 ; i < num_list ; ++i )
		{
			
			PackScriptResultPtr& ps_result_ptr = ps_result_list[i];

			if (ps_result_ptr == NULL)
			{
				ps_result_ptr = i3::make_shared<PackScriptResult>();
				ps_result_ptr->packscript_path = rel_ps_path_list[i];
			}
			
			PackScriptResult* ps_result = ps_result_ptr.get();
				
			if ( ps_result->contents[l_r] == NULL )
			{
				ps_result->contents[l_r] = i3::make_shared<PackScriptContentsResult>();
				ps_result->contents[l_r]->parent = ps_result;
				ps_result->contents[l_r]->path_mode = m;
			}
			
			PackScriptContentsResult* ps_contents_result = ps_result->contents[l_r].get();

			const PackScriptContents& contents = contents_list[i];

			const i3::vector<PackScriptUnitPtr>& src_unit_list = contents.unit_list;
			i3::vector<PackScriptUnitResultPtr>& dest_unit_list = ps_contents_result->unit_list;

			const size_t num_unit = src_unit_list.size();
			
			dest_unit_list.resize(num_unit);

			for (size_t j = 0 ; j < num_unit; ++j )
			{
				dest_unit_list[j] = i3::make_shared<PackScriptUnitResult>();
				
				PackScriptUnitResult* dest_unit = dest_unit_list[j].get();
				
				dest_unit->parent = ps_contents_result;
				dest_unit->pack_filename = src_unit_list[j]->pack_filename;
				
				if (m != PATHMODE_LIVE)
				{
					ImportResultFilesFromPackScriptUnitWithoutENC(str_base_path, *src_unit_list[j], 
						filter_list.pack_filter_list, nation_folder_filter_list, dest_unit->result_file_list);
				}
				else
				{
					i3::string pack_file_path = str_base_path;
					pack_file_path += "/Pack/";
					pack_file_path += dest_unit->pack_filename;
					ImportResultFilesFromi3PackFile_Acc(pack_file_path, dest_unit->result_file_list);
				}
			}

			if (contents.copy_unit)
			{
				ps_contents_result->copy_unit = i3::make_shared<PackScriptUnitResult>();
				PackScriptUnitResult* dest_unit = ps_contents_result->copy_unit.get();
				dest_unit->parent = ps_contents_result;
				
				ImportResultFilesFromPackScriptUnitWithoutENC(str_base_path, *contents.copy_unit, filter_list.pack_filter_list,
					nation_folder_filter_list, dest_unit->result_file_list);
			}
			
		}
				
	}





}



FileCheck_PackScriptMgr::FileCheck_PackScriptMgr() : m_pack_mode(PACKMODE_MEDIA_TO_QA_COPY)
{
	m_l_r_path_mode[0] = PATHMODE_MEDIA;
	m_l_r_path_mode[1] = PATHMODE_QA;
}

FileCheck_PackScriptMgr::~FileCheck_PackScriptMgr()
{

}

void	FileCheck_PackScriptMgr::ClearPackScriptMap()
{
	m_psf_result_map.clear();
	m_filename_result_map.clear();
	m_rsc_result_list.clear();
	m_filename_rsc_result_map.clear();
}


void	FileCheck_PackScriptMgr::BuildPackScriptMap(PackMode m, PreprocessMessageCallback* cb)
{
	m_pack_mode = m;
	
	if ( m_pack_mode == PACKMODE_MEDIA_TO_QA_COPY)
	{
		m_l_r_path_mode[0] = PATHMODE_MEDIA;
		m_l_r_path_mode[1] = PATHMODE_QA;
	}
	else
	{
		m_l_r_path_mode[0] = PATHMODE_QA;
		m_l_r_path_mode[1] = PATHMODE_LIVE;
	}

	m_psf_result_map.clear();
	m_filename_result_map.clear();
	m_rsc_result_list.clear();
	m_filename_rsc_result_map.clear();


	i3::vector<PackScriptFile*>	psf_list;
	{
		PackScriptFolder* root = PackScriptMgr::i()->GetPackScriptRoot();
		::CollectPackScriptFiles(root, psf_list);
	}

	const i3::wstring& strPackScriptPath = PathMgr::i()->GetPackScriptPath();
	const size_t num_file = psf_list.size();

	cb->OnTextMessage(L"Reading i3PackScripts in PackScript Folder...");

	i3::vector<i3::wstring> rel_ps_path_list(num_file);
	i3::vector<PackScriptContents>	contents_list(num_file);
	
	i3::wstring full_path;

	for (size_t i = 0 ; i < num_file ; ++i )
	{
		PackScriptFile* f = psf_list[i];
		f->MakeRelPath(rel_ps_path_list[i]);
		rel_ps_path_list[i] += L".i3PackScript";
		
		full_path = strPackScriptPath;
		full_path += L'/';
		full_path += rel_ps_path_list[i];
	
		ImportPackScriptFile(full_path, contents_list[i]);	
	}

	i3::vector<PackScriptResultPtr>	vecPSResult;		// 멤버변수로 보관은 하지 않는다..
		
	
	if (m_pack_mode == PACKMODE_MEDIA_TO_QA_COPY)
	{
		cb->OnTextMessage(L"Gathering MediaPath FileName From i3PackScript Path...");
		BuildPackScriptMapImpl(PathMgr::i()->GetMainMediaPath(), 0, PATHMODE_MEDIA, rel_ps_path_list, contents_list, vecPSResult);
	
		cb->OnTextMessage(L"Gathering QAPath FileName From i3PackScript Path ...");
		BuildPackScriptMapImpl(PathMgr::i()->GetQAPath(), 1, PATHMODE_QA, rel_ps_path_list, contents_list, vecPSResult);
	}
	else
	{
		cb->OnTextMessage(L"Gathering QAPath FileName From i3PackScript Path ...");
		BuildPackScriptMapImpl(PathMgr::i()->GetQAPath(), 0, PATHMODE_QA, rel_ps_path_list, contents_list, vecPSResult);

		cb->OnTextMessage(L"Gathering LivePath FileName From i3PackScript Path ...");
		BuildPackScriptMapImpl(PathMgr::i()->GetLivePath(), 1, PATHMODE_LIVE, rel_ps_path_list, contents_list, vecPSResult);
	}

	{
		cb->OnTextMessage(L"Building PackScriptFileMap ...");

		typedef i3::unordered_map<const PackScriptFile*, PackScriptResultPtr> map_type;

		for (size_t i = 0 ; i < num_file ; ++i)
		{
			const PackScriptResultPtr& ptr = vecPSResult[i];
			if (m_psf_result_map.insert(map_type::value_type(psf_list[i], ptr)).second == false )
			{
				// 문제가 있는 경우 (중복이 없는걸 가정했기 때문에 이 경우 문제이다..)
				I3ASSERT(0);
			}
		}
	}
	
	
	{
		cb->OnTextMessage(L"Building PackScriptFile Resource Map ...");

		for (size_t i = 0 ; i < num_file ; ++i )
		{
			const PackScriptResult* ps_res = vecPSResult[i].get();
			
			for ( int l_r = 0 ; l_r < 2 ; ++l_r )			// j --> l_r
			{
				const PackScriptContentsResult* ps_contents = ps_res->contents[l_r].get();
				
				if (ps_contents)
				{
					const size_t num_unit = ps_contents->unit_list.size();
				
					for (size_t j = 0 ; j < num_unit ; ++j )
					{
						const PackScriptUnitResult* unit = ps_contents->unit_list[j].get();
						FileCheck_PackScriptMgr::FillFileNameResultMapImpl(unit, l_r);
					}
					
					const PackScriptUnitResult* unit = ps_contents->copy_unit.get();
					if (unit)
						FileCheck_PackScriptMgr::FillFileNameResultMapImpl(unit, l_r);
				}
			}
		}
	}
	

	{
		cb->OnTextMessage(L"Building RSCFile Result List ...");

		FileCheck_PackScriptMgr::AddRSCFileResult("Chara/AnimPack.rsc");
		FileCheck_PackScriptMgr::AddRSCFileResult("Weapon/WeaponAnimPack.rsc");
		FileCheck_PackScriptMgr::AddRSCFileResult("Weapon/Weapon.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Chara/Character.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Chara/CharaTexture.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Equip/Equip.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Equip/EquipTexture.RSC");
		
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Weapon.rsc");
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Character.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Dinosaur.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Interface.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Object.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Sound/Voice.RSC");

		FileCheck_PackScriptMgr::AddRSCFileResult("Effect/Define/Effects_High.RSC");
		FileCheck_PackScriptMgr::AddRSCFileResult("Effect/Define/EffectImage.RSC");
	}
	
	{
		cb->OnTextMessage(L"Building RSCFile Resource Map ...");
		i3::stack_string upper_string;
		i3::fixed_string key_string;

		typedef i3::unordered_map<i3::fixed_string, RSCFileResultMapElementPtr>	map_type;

		const size_t num_rsc_file = m_rsc_result_list.size();

		for (size_t i = 0 ; i < num_rsc_file ; ++i )
		{
			const RSCFileResult*  res = m_rsc_result_list[i].get();
			
			for ( int l_r = 0 ; l_r < 2 ; ++l_r )			// j --> l_r
			{
				const RSCFileContentsResult* contents = res->contents[l_r].get();
				
				const size_t num_resFile = contents->resFileList.size();
				
				for (size_t j = 0 ; j < num_resFile ; ++j)
				{
					const PSUnitResult_MapResFile* resFile = contents->resFileList[j].get();
					
					const i3::string& resFileName = resFile->unit->result_file_list[ resFile->string_idx ];
					
					upper_string = resFileName;	i3::to_upper(upper_string);		i3::make_unix_path(upper_string);
					
					key_string = upper_string;

					map_type::iterator it =	m_filename_rsc_result_map.find(key_string);	

					if ( it == m_filename_rsc_result_map.end() )
					{
						RSCFileResultMapElementPtr new_obj = i3::make_shared<RSCFileResultMapElement>();
						
						it = m_filename_rsc_result_map.insert(map_type::value_type(key_string, new_obj)).first;
					}

					RSCFileResultMapElement* elem =	it->second.get();
					
					elem->contents_result_list_by_path[l_r].push_back(contents);
					elem->resFileListIndex[l_r].push_back(j);
				}
			}
		}

	}


}

void		FileCheck_PackScriptMgr::FillFileNameResultMapImpl( const PackScriptUnitResult* unit, int l_r )
{
	typedef i3::unordered_map<i3::fixed_string, PackScriptResultMapElementPtr>	map_type;

	i3::string strTemp;

	const size_t num_file = unit->result_file_list.size();
	map_type::iterator it;

	for (size_t k = 0; k < num_file ; ++k)
	{
		const i3::string&  file_path = unit->result_file_list[k];

		strTemp = file_path;	i3::to_upper(strTemp);	i3::make_unix_path(strTemp);
		i3::fixed_string fixed_strTemp(strTemp);

		it = m_filename_result_map.find(fixed_strTemp);

		if ( it == m_filename_result_map.end() )
		{
			PackScriptResultMapElementPtr map_elem = i3::make_shared<PackScriptResultMapElement>();
			it = m_filename_result_map.insert(map_type::value_type(fixed_strTemp, map_elem)).first;

			map_elem->resource_file_path = file_path;
		}

		PackScriptResultMapElement* elem = it->second.get();
		elem->unit_result_list_by_path[l_r].push_back(unit);
		elem->unit_string_index_list[l_r].push_back(k);
	}
	
}

const PackScriptResultMapElement*	FileCheck_PackScriptMgr::FindPackScriptFileMapElement_Upper(const i3::fixed_string& strPath) const
{
	typedef i3::unordered_map<i3::fixed_string, PackScriptResultMapElementPtr>	map_type;
	
	map_type::const_iterator it = m_filename_result_map.find(strPath);

	if (it == m_filename_result_map.end())
		return NULL;

	return it->second.get();	
}

const PackScriptResultMapElement*	FileCheck_PackScriptMgr::FindPackScriptFileMapElement(const i3::const_char_range& strPath) const
{
	i3::stack_string str(strPath.begin(), strPath.end());
	i3::to_upper(str);
	i3::make_unix_path(str);
	
	return FileCheck_PackScriptMgr::FindPackScriptFileMapElement_Upper(i3::fixed_string(str) );
}

const RSCFileResultMapElement*	FileCheck_PackScriptMgr::FindRSCFileMapElement_Upper(const i3::fixed_string& strPath) const
{
	typedef i3::unordered_map<i3::fixed_string, RSCFileResultMapElementPtr>	map_type;

	map_type::const_iterator it = m_filename_rsc_result_map.find(strPath);

	if (it == m_filename_rsc_result_map.end())
		return NULL;

	return it->second.get();	
}

const RSCFileResultMapElement*	FileCheck_PackScriptMgr::FindRSCFileMapElement(const i3::const_char_range& strPath) const
{
	i3::stack_string str(strPath.begin(), strPath.end());
	i3::to_upper(str);
	i3::make_unix_path(str);

	return FileCheck_PackScriptMgr::FindRSCFileMapElement_Upper(i3::fixed_string(str) );
}



void		FileCheck_PackScriptMgr::AddRSCFileResult( const i3::rc_string& rel_rscfile_path )
{
	RSCFileResultPtr ptr = FileCheck_PackScriptMgr::CreateRSCFileResult(rel_rscfile_path );
	m_rsc_result_list.push_back(ptr);
}


RSCFileResultPtr   FileCheck_PackScriptMgr::CreateRSCFileResult( const i3::rc_string& rel_rscfile_path) const
{
	const i3::wstring* l_base_path, * r_base_path;
	const PackScriptUnitResult*  pack_file_unit = NULL;

	if (m_pack_mode == PACKMODE_MEDIA_TO_QA_COPY)
	{
		l_base_path = &PathMgr::i()->GetMainMediaPath();
		r_base_path = &PathMgr::i()->GetQAPath();
	}
	else
	{
		l_base_path = &PathMgr::i()->GetQAPath();
		r_base_path = &PathMgr::i()->GetLivePath();

		const PackScriptResultMapElement* elem = FileCheck_PackScriptMgr::FindPackScriptFileMapElement(rel_rscfile_path);
		if (elem)
		{
			const i3::vector<const PackScriptUnitResult*>& vec = elem->unit_result_list_by_path[1];		// RIGHT (1)
			if ( !vec.empty() )
				pack_file_unit = vec[0];				// 2개 이상인 경우는 어떻게 해야할지 모르겠음..(하나만 존재한다고 가정한다.)
		}
	}
	
	i3::string  str_base_path[2];
	i3::utf16_to_mb(*l_base_path, str_base_path[0]);
	i3::utf16_to_mb(*r_base_path, str_base_path[1]);


	RscFileInfoList a_info_list[2];

	{
		LoadRscFileList_Acc(str_base_path[0], rel_rscfile_path, a_info_list[0]);

		i3PackVolume* vol = NULL;
		if (pack_file_unit && !pack_file_unit->pack_filename.empty() )
		{
			i3::stack_string full_packfile_path = str_base_path[1];
			full_packfile_path += "/Pack/";
			full_packfile_path += pack_file_unit->pack_filename;
			vol = LoadPackVolume(full_packfile_path);
			if (vol) PushbackPackVolume(vol);
		}
	
		LoadRscFileList_Acc(str_base_path[1], rel_rscfile_path, a_info_list[1] );
		
		if (vol)
		{
			ClearAllPackVolume();
			vol->Release();
		}
	}
	
	RSCFileResultPtr new_obj = i3::make_shared<RSCFileResult>();
	
	new_obj->rsc_file_path = rel_rscfile_path;
	
	// 일단 다 생성해두도록 한다..-_-
	for (size_t i = 0; i < 2 ; ++i )
	{
		new_obj->contents[i] = i3::make_shared<RSCFileContentsResult>();

		RSCFileContentsResult* contents = new_obj->contents[i].get();
		contents->parent = new_obj.get();
		contents->path_mode = m_l_r_path_mode[i];
		
		const RscFileInfoList& info_list = a_info_list[i];

		const size_t num_info = info_list.m_path_list.size();
		
		contents->psr_holder_list.reserve(num_info);
		contents->resFileList.reserve(num_info);


		for (size_t j = 0; j < num_info ; ++j)
		{
			const i3::rc_string& str_resource = info_list.m_path_list[j];
			
			const PackScriptResultMapElement* elem = FileCheck_PackScriptMgr::FindPackScriptFileMapElement(str_resource);
			
			if ( elem != NULL && !elem->unit_result_list_by_path[i].empty() )
			{	// PackScriptUnitResult를 찾은 경우.. (.i3PACK안에 있음)
				const PackScriptUnitResult* unit = elem->unit_result_list_by_path[i][0];	
				const size_t				idx  = elem->unit_string_index_list[i][0];
				
				PackScriptResult* ps_result = unit->parent->parent;

				contents->psr_holder_list.push_back( ps_result->shared_from_this() );
				
				PSUnitResult_MapResFilePtr new_obj = i3::make_shared<PSUnitResult_MapResFile>();

				new_obj->unit = unit;
				new_obj->string_idx = idx; 
				new_obj->file_exist = true;			// PACKSCRIPT에 있으면 우선 존재한다고 가정..
				
				contents->resFileList.push_back(new_obj);
			}
			else // 못찾은 경우 ( 일반 파일임)
			{
				PackScriptUnitResult* unit = contents->non_pack_script_holder.get();

				if (!unit)
				{
					contents->non_pack_script_holder = i3::make_shared<PackScriptUnitResult>();
					unit = contents->non_pack_script_holder.get();
					unit->parent = NULL;		// 널값 확실이 초기화해야함..
				}
				unit->result_file_list.push_back(str_resource);
				
				contents->psr_holder_list.push_back( i3::shared_ptr<PackScriptResult>());

				PSUnitResult_MapResFilePtr new_obj = i3::make_shared<PSUnitResult_MapResFile>();
				
				new_obj->unit = unit;
				new_obj->string_idx = unit->result_file_list.size() - 1;
				new_obj->file_exist = true;							// 파일검사해서 없으면 뒤에서 false로 교체한다.

				i3::stack_string strFullPath = str_base_path[i];
				strFullPath += '/';
				strFullPath.append(str_resource.begin(), str_resource.end());
				
				if (::GetFileAttributesA(strFullPath.c_str()) == INVALID_FILE_ATTRIBUTES)
					new_obj->file_exist = false;
				
				contents->resFileList.push_back(new_obj);
			}
		}
	}
	
	return new_obj;
}


const PackScriptResultPtr	FileCheck_PackScriptMgr::FindPackScriptFileResult(const PackScriptFile* f) const
{
	typedef i3::unordered_map<const PackScriptFile*, PackScriptResultPtr> map_type;
	
	map_type::const_iterator it =	m_psf_result_map.find(f);
	
	return (it == m_psf_result_map.end()) ? PackScriptResultPtr() : it->second;
}

void  FindDiffString(const i3::vector<i3::string>& lhs, const i3::vector<i3::string>& rhs, 
					 i3::vector<size_t>& in_out_lhs_only, i3::vector<size_t>& in_out_rhs_only, 
					 i3::vector<size_t>& in_out_found_lhs, i3::vector<size_t>& in_out_found_rhs)
{
	// 	거의 대부분 일치한다고 가정하고 들어가므로 단순 소트처럼 그냥 처리한다.

	const size_t num_lhs = lhs.size();
	const size_t num_rhs = rhs.size();

	i3::vector<size_t>  tmp_rhs_only(num_rhs);
	for (size_t i = 0; i < num_rhs ; ++i) tmp_rhs_only[i] = i;

	for (size_t i = 0 ; i < num_lhs ; ++i )
	{
		const i3::string& l_val = lhs[i];
		
		const size_t num_tmp_rhs = tmp_rhs_only.size();
		size_t j = 0;

		for ( ; j < num_tmp_rhs ; ++j)
		{
			const i3::string& r_val = rhs[ tmp_rhs_only[j] ];
			
			if ( i3::generic_is_iequal(l_val, r_val) )										// 단순 비교한다..
			{
				in_out_found_lhs.push_back(i);
				in_out_found_rhs.push_back(tmp_rhs_only[j]);			// 일단 발견되었으면 rhs쪽은 제거해준다.
				tmp_rhs_only.erase( tmp_rhs_only.begin() + j);
				break;
			}
		}

		if ( j == num_tmp_rhs )		// 못찾은 것임..
		{
			in_out_lhs_only.push_back(i);
		}
	}
	
	in_out_rhs_only.insert(in_out_rhs_only.end(), tmp_rhs_only.begin(), tmp_rhs_only.end());
		
}

void  FindDiffString(const PackScriptUnitResult* lhs, const PackScriptUnitResult* rhs, 
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_lhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_rhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_lhs_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_rhs_unit)
{
	if ( !lhs || !rhs)
	{
		if ( !lhs && !rhs)
			return;
		
		const PackScriptUnitResult* target_unit = (lhs) ? lhs : rhs;
		i3::vector<PSUnitResult_MapResFilePtr>& target_list = (lhs) ? in_out_lhs_only_unit : in_out_rhs_only_unit;

		const size_t num_rhs = target_unit->result_file_list.size();
		for (size_t i = 0 ; i < num_rhs ; ++i) 
		{
			PSUnitResult_MapResFilePtr new_obj = i3::make_shared<PSUnitResult_MapResFile>();
			new_obj->unit = target_unit;
			new_obj->string_idx = i;
			new_obj->file_exist = true;
			target_list.push_back(new_obj);
		}
		
		return;
	}

	const size_t num_lhs = lhs->result_file_list.size();
	const size_t num_rhs = rhs->result_file_list.size();
	
	i3::vector<size_t>  tmp_rhs_only(num_rhs);
	for (size_t i = 0; i < num_rhs ; ++i) tmp_rhs_only[i] = i;

	for (size_t i = 0 ; i < num_lhs ; ++i )
	{
		const i3::string& l_val = lhs->result_file_list[i];

		const size_t num_tmp_rhs = tmp_rhs_only.size();

		size_t j = 0;
		for ( ; j < num_tmp_rhs ; ++j)
		{
			const i3::string& r_val = rhs->result_file_list[ tmp_rhs_only[j] ];
			
			if ( i3::generic_is_iequal(l_val, r_val) )										// 단순 비교한다..
			{
				PSUnitResult_MapResFilePtr l_new_obj = i3::make_shared<PSUnitResult_MapResFile>();
				l_new_obj->unit = lhs;
				l_new_obj->string_idx = i;
				l_new_obj->file_exist = true;			// 항상 true일 것이다..
				in_out_found_lhs_unit.push_back(l_new_obj);

				PSUnitResult_MapResFilePtr r_new_obj = i3::make_shared<PSUnitResult_MapResFile>();
				r_new_obj->unit = rhs;
				r_new_obj->string_idx = tmp_rhs_only[j];
				r_new_obj->file_exist = true;
				in_out_found_rhs_unit.push_back(r_new_obj);

				tmp_rhs_only.erase( tmp_rhs_only.begin() + j);
				break;
			}
		}

		if ( j == num_tmp_rhs )		// 못찾은 것임..
		{
			PSUnitResult_MapResFilePtr l_new_obj = i3::make_shared<PSUnitResult_MapResFile>();
			l_new_obj->unit = lhs;
			l_new_obj->string_idx = i;
			l_new_obj->file_exist = true;			// 항상 true일 것이다..
			in_out_lhs_only_unit.push_back(l_new_obj);
		}
	}

	const size_t num_tmp_rhs = tmp_rhs_only.size();
		
	for (size_t i = 0 ; i < num_tmp_rhs ; ++i)
	{
		PSUnitResult_MapResFilePtr r_new_obj = i3::make_shared<PSUnitResult_MapResFile>();
		r_new_obj->unit = rhs;
		r_new_obj->string_idx = tmp_rhs_only[i];
		r_new_obj->file_exist = true;
		in_out_rhs_only_unit.push_back(r_new_obj);
	}

}



void  FindDiffString(const i3::vector<PSUnitResult_MapResFilePtr>& unit_lhs, 
					 const i3::vector<PSUnitResult_MapResFilePtr>& unit_rhs, 
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_lhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_rhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_lhs_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_rhs_unit)
{
	
	const size_t num_lhs = unit_lhs.size();
	const size_t num_rhs = unit_rhs.size();

	i3::vector<PSUnitResult_MapResFilePtr> tmp_rhs_only(unit_rhs);

	
	for (size_t i = 0 ; i < num_lhs ; ++i )
	{
		const PSUnitResult_MapResFile* l_resFile = unit_lhs[i].get();
		
		const i3::string& l_val = l_resFile->unit->result_file_list[ l_resFile->string_idx ];

		const size_t num_tmp_rhs = tmp_rhs_only.size();
		
		size_t j = 0;

		for ( ; j < num_tmp_rhs ; ++j)
		{
			const PSUnitResult_MapResFile* r_resFile = tmp_rhs_only[j].get();

			const i3::string& r_val = r_resFile->unit->result_file_list[ r_resFile->string_idx ];

			if ( i3::generic_is_iequal(l_val, r_val) )										// 단순 비교한다..
			{
				in_out_found_lhs_unit.push_back( unit_lhs[i] );
				in_out_found_rhs_unit.push_back( tmp_rhs_only[j]);
						
				tmp_rhs_only.erase( tmp_rhs_only.begin() + j);
				break;
			}
		}
		
		if ( j == num_tmp_rhs)		// 못찾음 
		{
			in_out_lhs_only_unit.push_back( unit_lhs[i] );
		}
	}

	in_out_rhs_only_unit.insert(in_out_rhs_only_unit.end(), tmp_rhs_only.begin(), tmp_rhs_only.end());

}



bool  FindDiffPackScriptContentsResult(const RSCFileContentsResult* lhs, const RSCFileContentsResult* rhs, 
									   DiffResult_PackScriptResult& in_out_res)
{
	if (lhs->parent != rhs->parent)			// 같은 부모파일이 아니라면 평가할수 없음.
		return false;
	
	FindDiffString(lhs->resFileList, rhs->resFileList, 
					in_out_res.in_out_alist[DIFFRESULT_PS_LEFT_ONLY], 
					in_out_res.in_out_alist[DIFFRESULT_PS_RIGHT_ONLY], 
					in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_LEFT], 
					in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_RIGHT]);

	return true;
}


bool  FindDiffPackScriptContentsResult(const PackScriptContentsResult* lhs, const PackScriptContentsResult* rhs, 
									   DiffResult_PackScriptResult& in_out_res)
{
	if (lhs->parent != rhs->parent)				// 같은 부모(PackScriptResult)파일이 아니면 평가할수 없음...
		return false;
	
	const size_t num_unit_lhs = lhs->unit_list.size();
	
	for (size_t i = 0 ; i < num_unit_lhs ; ++i)
	{
		PackScriptUnitResult* unit_lhs = lhs->unit_list[i].get();
		PackScriptUnitResult* unit_rhs = rhs->unit_list[i].get();

		FindDiffString(unit_lhs, unit_rhs, 
			in_out_res.in_out_alist[DIFFRESULT_PS_LEFT_ONLY],
			in_out_res.in_out_alist[DIFFRESULT_PS_RIGHT_ONLY],
			in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_LEFT],
			in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_RIGHT]);
	}
	
	
	PackScriptUnitResult* unit_lhs = lhs->copy_unit.get();
	PackScriptUnitResult* unit_rhs = rhs->copy_unit.get();

	FindDiffString(unit_lhs, unit_rhs, 
		in_out_res.in_out_alist[DIFFRESULT_PS_LEFT_ONLY],
		in_out_res.in_out_alist[DIFFRESULT_PS_RIGHT_ONLY],
		in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_LEFT],
		in_out_res.in_out_alist[DIFFRESULT_PS_FOUND_RIGHT]);

	
	return true;
}

void	FileCheck_PackScriptMgr::Update_Progress()
{
	m_progress_cb_signal();
}
