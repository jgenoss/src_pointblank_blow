#include "stdafx.h"
#include "ItemData.h"

#include "i3RscPartialFile.h"
#include "AssocFile.h"

#include "RSCFileMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/extract_fileext.h"

#include "ItemInfoMgr.h"
#include "PackScriptMgr.h"
#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/extract_filename.h"

#include "PathMgr.h"

// 
//size_t temp_nation_idx = 0;



ItemData::ItemData(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : m_parent(parent), m_file(f),
m_objInfo(objInfo), m_itemID(0)
{
	f->ReadNamei3NamedElement(objInfo, m_name);
}

ItemData::ItemData(const i3::rc_string& name) : m_name(name), m_parent(NULL), m_file(NULL), m_objInfo(NULL), m_itemID(0)
{
	//
}

void	AddMoreAssocRscFilesRecursive(AssocFileSet* new_obj, const i3::rc_string& regName, const i3::string& resPath, INT32* out_total_found_count)
{
	i3::stack_string str_ext;

	i3::extract_fileext(resPath, str_ext);

	if ( i3::generic_is_iequal(str_ext, "i3i") || i3::generic_is_iequal(str_ext, "wav") )
	{
		return;				// i3i나 wav는 내부 익스턴파일이 없을테니...처리하지 않는다.
	}
	
	i3::stack_string resPath2 = resPath;

	if ( i3::generic_is_iequal(str_ext, "I3S") )
	{
		const i3SGTexChangeInfo* info = i3SGTexChangeManager::i()->FindTexChangeInfo(resPath.c_str());
		
		if (info)
		{
			i3::stack_string filename;
			i3::extract_filename(resPath2, filename);

			i3::stack_string before_change_reg_name = regName;
			before_change_reg_name += '/';
			before_change_reg_name += filename;
			
			i3::rc_string ref_i3s_filepath = i3SGTexChangeManager::i()->GetRefi3sFilename(info);	
			
			new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>( before_change_reg_name, "I3S_FILE_CHANGED", ref_i3s_filepath ) );
			
			resPath2 = ref_i3s_filepath;

			i3::vector<i3::rc_string>	vecTargetTextures;
			i3SGTexChangeManager::GatherAllTargetTexturePaths(info, vecTargetTextures);
			
			const size_t num_tex = vecTargetTextures.size();
			
			i3::extract_filename(ref_i3s_filepath, filename);
			
			const i3::rc_string strModified_i3Texture_Literal = "I3S Changed i3Texture";

			for (size_t i = 0 ; i < num_tex ; ++i )
			{
				new_obj->assoc_file_list.push_back( i3::make_shared<AssocFile>( filename, strModified_i3Texture_Literal, vecTargetTextures[i]) );
			}
		
		}
	}
			
	
	i3::string strResFullPath;
	i3::utf16_to_mb(ItemInfoMgr::i()->GetMediaPath(), strResFullPath);
	strResFullPath += '/';		strResFullPath += resPath2;
	
	i3RscPartialFile f;

	if ( f.Open(strResFullPath.c_str()) )
	{
		i3::stack_string dirname;
		i3::stack_string filename;
		i3::extract_directoryname_filename(resPath2, dirname, filename);

		i3::stack_string	strPefRscCat2 = regName;
		strPefRscCat2 += '/';
		strPefRscCat2 += filename;
		i3::rc_string strRegName = strPefRscCat2;
		
		i3::vector<i3Rsc_ExternFilename> list;

		f.CollectExternFileName(list);

		const size_t num = list.size();
		
	
		i3::const_char_range rngDir, rngFilename;

		for (size_t i = 0 ; i < num ; ++i )
		{
			i3::extract_directoryname_filename_iterator_range(list[i].m_file_name, rngDir, rngFilename);
			
			if (!rngDir.empty())
			{
				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(strRegName, list[i].m_class_name,
					list[i].m_file_name));
				if (out_total_found_count)
					++(*out_total_found_count);
			}
			else		// 익스턴 경로 중에 디렉토리가 없고 /File.ext 형식으로만 된것이 있어서 감안해 처리한다..
			{
				i3::stack_string val_path = dirname;
				val_path.append( list[i].m_file_name.begin(), list[i].m_file_name.end());

				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(strRegName, list[i].m_class_name,
					val_path));
				if (out_total_found_count)
					++(*out_total_found_count);
			}
			
			AssocFile* last_val = new_obj->assoc_file_list.back().get();

			i3::extract_fileext(last_val->file_path, str_ext);

			if ( i3::generic_is_iequal(str_ext, "i3a") )
			{
				i3::vector<i3::rc_string> vec_midpack_path;
				RSCFileMgr::i()->FindAnimPackFile(last_val->file_path, vec_midpack_path);
				
				const size_t num_midpack = vec_midpack_path.size();

				if (num_midpack > 0)
				{
					last_val->midpack_path = vec_midpack_path[0];		// 첫번째 원소는 새 삽입이 아니라 기존 삽입..

					for (size_t j = 1; j < num_midpack ; ++j)
					{
						new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(strRegName, list[i].m_class_name,
							list[i].m_file_name, vec_midpack_path[j] ) );
						if (out_total_found_count)
							++(*out_total_found_count);
					}
				}
			}
			else
			{
				AddMoreAssocRscFilesRecursive(new_obj, strRegName, list[i].m_file_name);
			}
		}
	}
}
	
namespace
{	
	void    FillResultFileList( AssocFileSet* new_obj, size_t assoc_file_index, 
								AssocFile* f, const PackScriptFileMapElement* elem,
								i3::vector<AssocResultFile_PackScriptPtr>& inout)
	{
		// 기목록에서 동일 파일 조사...
		struct FindPred 
		{
			const PackScriptFile* psf;
			FindPred(const PackScriptFile* psf) : psf(psf) {}
			bool operator()(const AssocResultFile_PackScriptPtr& ptr) const 
			{
				return ptr->packscript_file == this->psf;
			}
		};

		const size_t num_ps_file = elem->file_list.size();
		
		for (size_t i = 0 ; i < num_ps_file ; ++i)
		{
			PackScriptFile* psf = elem->file_list[i];
			
			FindPred   pr(psf);
			i3::vector<AssocResultFile_PackScriptPtr>::iterator it = i3::range::find_if(inout, pr);
			
			if ( it != inout.end() )
			{
				AssocResultFile_PackScript* p = (*it).get();
				p->m_assoc_fileset_index_list.push_back(assoc_file_index);
				// p->file_path_list 삽입 전 중복 체크 해야함..
				struct FindPred2
				{
					const i3::rc_string& rhs;
					FindPred2(const i3::rc_string& rhs) : rhs(rhs) {}
					bool operator()(const i3::rc_string& lhs) const
					{
						if (lhs.c_str() == rhs.c_str() )
							return true;
						i3::stack_string lhs2 = lhs;	i3::make_unix_path(lhs2);
						i3::stack_string rhs2 = rhs;	i3::make_unix_path(rhs2);
						return i3::generic_is_iequal(lhs2, rhs2);
					}
				};
				
				const i3::rc_string& tgt_file_name = f->midpack_path.empty() ? f->file_path : f->midpack_path;

				FindPred2 pr2( tgt_file_name );
				i3::vector<i3::rc_string>::iterator it2 = i3::range::find_if(p->file_path_list, pr2);
				
				if (it2 == p->file_path_list.end())
				{
					p->file_path_list.push_back( tgt_file_name );
				}
			}
			else
			{
				AssocResultFile_PackScriptPtr new_result = i3::make_shared<AssocResultFile_PackScript>();
				
				new_result->m_assoc_fileset_index_list.push_back(assoc_file_index);
				new_result->packscript_file = psf;
				
				if (f->midpack_path.empty())
					new_result->file_path_list.push_back( f->file_path );
				else
					new_result->file_path_list.push_back( f->midpack_path);
				
				new_obj->result_file_list.push_back(new_result);
				inout.push_back(new_result);
			}
		}
		
	}
	
	void    FillResultFileList( AssocFileSet* new_obj, size_t assoc_file_index, 
		AssocFile* f, i3::vector<AssocResultFile_AnimPackPtr>& inout)
	{
		struct FindPred 
		{
			const i3::rc_string&  path;
			FindPred(const i3::rc_string& path) : path(path) {}

			bool operator()( const AssocResultFile_AnimPackPtr& ptr) const
			{
				if ( ptr->midpack_path.c_str() == path.c_str() )
					return true;
				i3::stack_string lhs = ptr->midpack_path;	i3::make_unix_path(lhs);
				i3::stack_string rhs = path;	i3::make_unix_path(rhs);
				
				return i3::generic_is_iequal(lhs, rhs);
			}
		};
		
		FindPred	pr( f->midpack_path);

		i3::vector<AssocResultFile_AnimPackPtr>::iterator it = 
			i3::range::find_if(inout, pr);

		if ( it != inout.end() )
		{
			(*it)->m_assoc_fileset_index_list.push_back(assoc_file_index);
		}
		else
		{
			AssocResultFile_AnimPackPtr new_result = i3::make_shared<AssocResultFile_AnimPack>();

			new_result->m_assoc_fileset_index_list.push_back(assoc_file_index);
			new_result->midpack_path = f->midpack_path;
			new_obj->result_file_list.push_back(new_result);
			inout.push_back(new_result);
		}
	}
	
	void    FillResultFileList( AssocFileSet* new_obj, size_t assoc_file_index, 
		AssocFile* f, i3::vector<AssocResultFile_NormalFilePtr>& inout)
	{
		
		struct FindPred 
		{
			const i3::rc_string&  path;
			FindPred(const i3::rc_string& path) : path(path) {}

			bool operator()( const AssocResultFile_NormalFilePtr& ptr) const
			{
				if ( ptr->file_path.c_str() == path.c_str() )
					return true;
				i3::stack_string lhs = ptr->file_path;	i3::make_unix_path(lhs);
				i3::stack_string rhs = path;	i3::make_unix_path(rhs);

				return i3::generic_is_iequal(lhs, rhs);
			}
		};

		FindPred	pr( f->file_path );

		i3::vector<AssocResultFile_NormalFilePtr>::iterator it = 
			i3::range::find_if(inout, pr);

		if ( it != inout.end() )
		{
			(*it)->m_assoc_fileset_index_list.push_back(assoc_file_index);
		}
		else
		{
			AssocResultFile_NormalFilePtr new_result = i3::make_shared<AssocResultFile_NormalFile>();

			new_result->m_assoc_fileset_index_list.push_back(assoc_file_index);
			new_result->file_path = f->file_path;
			new_obj->result_file_list.push_back(new_result);
			inout.push_back(new_result);
		}
				
	}
	
	void    FillRSCFileToResultFileList(AssocFileSet* new_obj)
	{
		// 중복체크쪽은 RscFileInfo*를 토대로 하는게 편할것이다.. (포인터비교..)
		struct FindRSCPred
		{
			const RscFileInfo* info;
			FindRSCPred(const RscFileInfo* info) : info(info) {}
			
			bool operator()( const AssocResultFile_RscFilePtr& ptr) const
			{
				return ptr->rsc_file_path.c_str() == info->m_RSCFileName.c_str();
			}
		};
		
		i3::vector<AssocFilePtr>& assoc_file_list = new_obj->assoc_file_list;
		const size_t num_assoc_file = assoc_file_list.size();
		
		i3::vector<AssocResultFile_RscFilePtr>	vec_rsc;
		
		i3::rc_string src_filepath;
		i3::string	  src_filename;
		RscFileInfo*  info = NULL;
		
		for (size_t i = 0 ; i < num_assoc_file ; ++i )
		{
			AssocFile* f = assoc_file_list[i].get();		

			if (f->midpack_path.empty())	
				src_filepath = f->file_path;
			else
				src_filepath = f->midpack_path;		// ANIMPACK인 경우..

			// 검색시엔 디렉토리 이름 제거하고 처리..
			i3::extract_filename(src_filepath, src_filename);

			if ( RSCFileMgr::i()->FindRscFilePath(src_filename, info) == false)
				continue;
			
			if (info->m_IsNameUnique == false)		// 찾았더라도 2개 이상이면 경로를 키로 검색해야한다..
			{
				if ( RSCFileMgr::i()->FindRscFilePath( src_filepath, info) == false )
					continue;
			}

			// rsc확장자 파일의 중복체크는 그냥 brute하게 처리..
			// 대소문자 sensitive 처리도 상관없는데다..포인터비교도 가능할 정도임...

			i3::vector<AssocResultFile_RscFilePtr>::iterator it = 
				i3::range::find_if(vec_rsc, FindRSCPred(info) );

			if ( it == vec_rsc.end() )
			{
				AssocResultFile_RscFilePtr new_ptr = i3::make_shared<AssocResultFile_RscFile>();
				new_ptr->m_assoc_fileset_index_list.push_back(i);
				new_ptr->rsc_file_path = info->m_RSCFileName;
				const PackScriptFileMapElement* ps_elem = PackScriptMgr::i()->FindPackScriptFileMapElement(info->m_RSCFileName);
				
				new_ptr->packscript_file = (ps_elem) ? ps_elem->file_list[0] : NULL;

				new_ptr->rscnode_rel_path_list.push_back(info);
				vec_rsc.push_back(new_ptr);
			}
			else
			{
				AssocResultFile_RscFilePtr& ptr = *it;
				ptr->m_assoc_fileset_index_list.push_back(i);
				
				if ( ptr->rscnode_rel_path_list.back() != info )		// 최적화용도..조건문..
					ptr->rscnode_rel_path_list.push_back(info);
			}
		}
		
		const size_t num_vec_rsc = vec_rsc.size();

		for (size_t i = 0 ; i < num_vec_rsc; ++i)
		{
			AssocResultFile_RscFile* f = vec_rsc[i].get();
			i3::vu::remove_duplicates(f->rscnode_rel_path_list);		// 중복제거..
		}
		
		i3::vector<AssocResultFileBasePtr>&	result_file_list = new_obj->result_file_list;
		result_file_list.insert(result_file_list.end(), vec_rsc.begin(), vec_rsc.end());
	}
	
	void	FillResultFileList(AssocFileSet* new_obj)
	{
		// packscript, animpack, normal_file 선별하며, 모두 컨텐츠가 존재해야 적용되도록 처리한다.
		i3::vector<AssocFilePtr>& list = new_obj->assoc_file_list;
		
		const size_t num = list.size();
		
		{
			// 1. 팩스크립트파일 추적 루틴..
			// 2. 팩스크립트 안된 것중 애님팩파일,노멀파일 추적 루틴..
			// 3. 기존 result_file_list 회전을 거친 후 존재하지 않으면 새로 입력 처리한다..

			i3::vector<AssocResultFile_PackScriptPtr>  vec_ps;	
			i3::vector<AssocResultFile_AnimPackPtr>	   vec_ap;	   
			i3::vector<AssocResultFile_NormalFilePtr>  vec_nf;

			for (size_t i = 0 ; i < num ; ++i )
			{
				AssocFile* f = list[i].get();		

				if ( !f->midpack_path.empty() )
				{		
					const PackScriptFileMapElement* elem = PackScriptMgr::i()->FindPackScriptFileMapElement(f->midpack_path);

					if (elem != NULL)
					{
						FillResultFileList( new_obj, i, f, elem, vec_ps);
						continue;
					}
					
				}

				const PackScriptFileMapElement* elem = PackScriptMgr::i()->FindPackScriptFileMapElement(f->file_path);
				if (elem != NULL)
				{
					FillResultFileList(new_obj, i, f, elem, vec_ps);
					continue;
				}

				if ( !f->midpack_path.empty())
				{
					FillResultFileList(new_obj, i, f, vec_ap);
				}
				else if ( !f->file_path.empty() )
				{
					FillResultFileList(new_obj, i, f, vec_nf);
				}
			}

		}

		// 4.  애님팩과 노멀은 실 파일 존재 검증이 필요한데..일단 중복제거후 처리한다...
		
		i3::vector<AssocResultFileBasePtr>&	result_file_list = new_obj->result_file_list;
		const i3::wstring& mediaPath = PathMgr::i()->GetMainMediaPath();
		const size_t num_res = result_file_list.size();
		
		for (size_t i = 0; i < num_res ; ++i)
		{
			size_t idx = num_res - i - 1;
			
			AssocResultFileBase* p = result_file_list[idx].get();

			if ( p->is_midpack || p->is_normal_file )
			{
				i3::wstring str;	
				
				if ( p->is_midpack )
					i3::mb_to_utf16( ((AssocResultFile_AnimPack*)p)->midpack_path, str);
				else
					i3::mb_to_utf16( ((AssocResultFile_NormalFile*)p)->file_path, str);

				i3::stack_wstring path = mediaPath;
				path += L'/';
				path.append( str.begin(), str.end());
				
				if (::GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES )
				{
					result_file_list.erase( result_file_list.begin() + idx);
				}
			}
		}
		
		// 	5.  assoc_file_list에서 아직 result_file_index 적재가 안되있으므로 이걸 처리한다..
		const size_t num_res2 = result_file_list.size();

		for (size_t i = 0 ; i < num_res2 ; ++i )
		{
			AssocResultFileBase* p = result_file_list[i].get();

			const i3::vector<size_t>& idx_list = p->m_assoc_fileset_index_list;
			
			const size_t num_idx_list = idx_list.size();

			for (size_t j = 0 ; j < num_idx_list ; ++j )
			{
				AssocFile* f = list[ idx_list[j] ].get();
				f->vec_result_file_index.push_back(i);
			}
		}
	}


}

AssocFileSetPtr		ItemData::CreateRscFileSetFragment() const
{
	AssocFileSetPtr new_obj_ptr = i3::make_shared<AssocFileSet>();
	AssocFileSet* new_obj = new_obj_ptr.get();

	new_obj->item_name = m_name;

	this->FillAssocFileList(new_obj, m_file, m_objInfo);		// 이 부분은 각 아이템 내용별로 다를수 있다.. 가상함수 분기가 적당하다..

	FillRSCFileToResultFileList(new_obj);	// RSC파일 전용 result_list 취합..
	FillResultFileList(new_obj);			// 이것은 기본클래스에서 공통적으로 사용가능하다..

	return new_obj_ptr;
}


AssocFileSetPtr	ItemData::CreateRscFileSet() const
{
	AssocFileSetPtr ptr = ItemData::CreateRscFileSetFragment();
	
	this->OnProcessRscFileSetWithItemID(ptr.get(), m_file, m_objInfo);
	this->OnProcessMergeCommonAssocFileSet(ptr.get());	
	return ptr;
}

void		ItemData::OnInsertItemIDMap( i3::unordered_map<INT32, ItemData*>& inout ) // 반드시 private..
{
	this->OnInsertItemIDMap(m_file, m_objInfo, m_itemID, inout);
}

void	ItemData::OnBuildRelatedDataList()
{
	this->OnBuildRelatedDataList(this->m_file, this->m_objInfo);
}

void	ItemData::AddRelatedData(const i3::rc_string& reg_name, ItemData* data)
{
	m_relatedDataList.push_back( i3::make_shared<RelatedItemData>(reg_name, data) );
}

