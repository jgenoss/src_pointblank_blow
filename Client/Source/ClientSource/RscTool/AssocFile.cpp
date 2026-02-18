#include "stdafx.h"
#include "AssocFile.h"

#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/ext/make_unix_path.h"

namespace
{
	
	void AddResultFileList(AssocFileSet* owner, 
		const AssocResultFile_RscFile* elem,
		const i3::vector<AssocResultFile_RscFile*>& in)
	{
		struct FindPred
		{
			const i3::rc_string& rsc_file_path;
			FindPred(const i3::rc_string& rsc_file_path) : rsc_file_path(rsc_file_path) {}
			bool operator()(const AssocResultFile_RscFile* p) const
			{
				return	p->rsc_file_path.c_str() == rsc_file_path.c_str();
			}
		};
		
		FindPred pr( elem->rsc_file_path );
		i3::vector<AssocResultFile_RscFile*>::const_iterator it = i3::range::find_if(in, pr);

		if ( it != in.end())
		{
			AssocResultFile_RscFile* p = *it;
			p->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());	
			
			p->rscnode_rel_path_list.insert(p->rscnode_rel_path_list.end(), 
				elem->rscnode_rel_path_list.begin(), elem->rscnode_rel_path_list.end() );

			i3::vu::remove_duplicates(p->rscnode_rel_path_list);
		}
		else
		{
			AssocResultFile_RscFilePtr new_result = i3::make_shared<AssocResultFile_RscFile>();
			new_result->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());	
			new_result->rsc_file_path = elem->rsc_file_path;
			new_result->packscript_file = elem->packscript_file;

			new_result->rscnode_rel_path_list.assign( elem->rscnode_rel_path_list.begin(),
				elem->rscnode_rel_path_list.end());
			
			owner->result_file_list.push_back(new_result);
			// inout 갱신은 하지 않는다. (필요성 없어보임)
		}
	}

	void AddResultFileList(AssocFileSet* owner, 
						const AssocResultFile_PackScript* elem,
						const i3::vector<AssocResultFile_PackScript*>& in)
	{
		
		struct FindPred
		{
			const PackScriptFile* psf;
			FindPred(const PackScriptFile* psf) : psf(psf) {}

			bool operator()(const AssocResultFile_PackScript* p) const
			{
				return p->packscript_file == psf;
			}
		};
		
		FindPred pr( elem->packscript_file);
		i3::vector<AssocResultFile_PackScript*>::const_iterator it = i3::range::find_if(in, pr);
		
		if ( it != in.end())
		{
			AssocResultFile_PackScript* p = *it;
			p->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
			
			// p->file_path_list에 대해 중복검사해야한다..
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

			const size_t num_elem_file = elem->file_path_list.size();
			
			for (size_t i = 0 ; i < num_elem_file ; ++i)
			{
				FindPred2 pr2( elem->file_path_list[i] );
				
				i3::vector<i3::rc_string>::iterator it2 = i3::range::find_if(p->file_path_list, pr2);

				if (it2 == p->file_path_list.end())
				{
					p->file_path_list.push_back( elem->file_path_list[i] );
				}
			}

		}
		else
		{
			AssocResultFile_PackScriptPtr new_result = i3::make_shared<AssocResultFile_PackScript>();

			new_result->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
			new_result->packscript_file = elem->packscript_file;
			new_result->file_path_list  = elem->file_path_list;
			owner->result_file_list.push_back(new_result);
			// inout 갱신은 하지 않는다. (필요성 없어보임)
		}
	}

	void AddResultFileList(AssocFileSet* owner, 
		const AssocResultFile_AnimPack* elem,
		const i3::vector<AssocResultFile_AnimPack*>& in)
	{
		struct FindPred
		{
			const i3::rc_string& path;
			FindPred(const i3::rc_string& path) : path(path) {}

			bool operator()(const AssocResultFile_AnimPack* p) const
			{
				if ( p->midpack_path.c_str() == path.c_str() )
					return true;
				i3::stack_string lhs = p->midpack_path;	i3::make_unix_path(lhs);
				i3::stack_string rhs = path;	i3::make_unix_path(rhs);

				return i3::generic_is_iequal(lhs, rhs);				
			}
		};

		FindPred pr(elem->midpack_path);
		i3::vector<AssocResultFile_AnimPack*>::const_iterator it = i3::range::find_if(in, pr);
			
		if ( it != in.end())
		{
			(*it)->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
		}
		else
		{
			AssocResultFile_AnimPackPtr new_result = i3::make_shared<AssocResultFile_AnimPack>();

			new_result->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
			new_result->midpack_path = elem->midpack_path;
			owner->result_file_list.push_back(new_result);
			// inout 갱신은 하지 않는다. (필요성 없어보임)
		}
		
	}

	void AddResultFileList(AssocFileSet* owner, 
		const AssocResultFile_NormalFile* elem,
		const i3::vector<AssocResultFile_NormalFile*>& in)
	{
		struct FindPred
		{
			const i3::rc_string& path;
			FindPred(const i3::rc_string& path) : path(path) {}

			bool operator()(const AssocResultFile_NormalFile* p) const
			{
				if ( p->file_path.c_str() == path.c_str() )
					return true;
				i3::stack_string lhs = p->file_path;	i3::make_unix_path(lhs);
				i3::stack_string rhs = path;	i3::make_unix_path(rhs);

				return i3::generic_is_iequal(lhs, rhs);				
			}
		};

		FindPred pr(elem->file_path);

		i3::vector<AssocResultFile_NormalFile*>::const_iterator it = 
			i3::range::find_if(in, pr);

		if ( it != in.end())
		{
			(*it)->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
		}
		else
		{
			AssocResultFile_NormalFilePtr new_result = i3::make_shared<AssocResultFile_NormalFile>();

			new_result->m_assoc_fileset_index_list.push_back(owner->assoc_file_list.size());
			new_result->file_path = elem->file_path;				
			owner->result_file_list.push_back(new_result);
			// inout 갱신은 하지 않는다. (필요성 없어보임)
		}
		
	}

}



//
// AssocFileSet 덩어리 하나를 다른 AssocFileSet의 원소 하나로 포함시키도록 처리한다.
//

void  MergeAssocFileSetIntoAssocFileSetElement(
	AssocFileSet* owner, AssocFileSet* element, const i3::rc_string& reg_name,
	const i3::rc_string& reg_value)
{
	
	// new_assoc->vec_result_file_index는 가장 마지막에 처리..
	
	// 1. result_file_list부터 합친다..

	// 1.1 일단 기존 Result File을 항목별 분리한다.
	// 1.2 그다음 합칠 Result File들을 루프돌아서 기존것에 삽입한다..
	{
		i3::vector<AssocResultFile_RscFile*>	 vec_rsc;
		i3::vector<AssocResultFile_PackScript*>  vec_ps;	
		i3::vector<AssocResultFile_AnimPack*>	 vec_ap;	   
		i3::vector<AssocResultFile_NormalFile*>  vec_nf;
	
		const size_t num_owner_res_file = owner->result_file_list.size();
		
		for (size_t i = 0; i < num_owner_res_file ; ++i)
		{
			AssocResultFileBase* b = owner->result_file_list[i].get();
			
			if (b->is_rsc_file)
				vec_rsc.push_back(static_cast<AssocResultFile_RscFile*>(b));
			else
			if (b->is_packscript)
				vec_ps.push_back(static_cast<AssocResultFile_PackScript*>(b));
			else
			if (b->is_midpack)
				vec_ap.push_back(static_cast<AssocResultFile_AnimPack*>(b));
			else
			if (b->is_normal_file)
				vec_nf.push_back(static_cast<AssocResultFile_NormalFile*>(b));
			
		}

		const size_t num_elem_res_file = element->result_file_list.size();

		for (size_t i = 0 ; i < num_elem_res_file ; ++i )
		{
			AssocResultFileBase* b = element->result_file_list[i].get();

			if (b->is_rsc_file)
				AddResultFileList(owner, 
					static_cast<AssocResultFile_RscFile*>(b),
					vec_rsc);
			else
			if (b->is_packscript)
				AddResultFileList(owner, 
				static_cast<AssocResultFile_PackScript*>(b), 
				vec_ps);
			else
			if (b->is_midpack)
				AddResultFileList(owner,
				static_cast<AssocResultFile_AnimPack*>(b),
				vec_ap);
			else
			if (b->is_normal_file)
				AddResultFileList(owner,
				static_cast<AssocResultFile_NormalFile*>(b),
				vec_nf);
		}
	}

	AssocFilePtr new_assoc = i3::make_shared<AssocFile>(reg_name, reg_value);
	owner->assoc_file_list.push_back(new_assoc);
	
	const size_t num_res = owner->result_file_list.size();
	
	size_t target_assoc_index = owner->assoc_file_list.size()-1;
	
	for (size_t i = 0 ; i < num_res ; ++i)	
	{
		AssocResultFileBase* p = owner->result_file_list[i].get();

		const i3::vector<size_t>& idx_list = p->m_assoc_fileset_index_list;
		
		assert(!idx_list.empty());

		size_t result_index = idx_list.back();		// 겹친다면 하나뿐이다.
		
		if (result_index == target_assoc_index)
		{
			if (p->is_midpack)
			{
				AssocResultFile_AnimPack*  p2 = static_cast<AssocResultFile_AnimPack*>(p);
				
				new_assoc->midpack_path += p2->midpack_path;
				new_assoc->midpack_path += " ; ";
			}
			else
			if (p->is_normal_file)
			{
				AssocResultFile_NormalFile* p2 = static_cast<AssocResultFile_NormalFile*>(p);
				new_assoc->file_path += p2->file_path;
				new_assoc->file_path += " ; ";
			}

			new_assoc->vec_result_file_index.push_back(i);
		}		
	}

	if (new_assoc->midpack_path.size() > 2)
	{
		new_assoc->midpack_path.resize(new_assoc->midpack_path.size() - 2);
	}

	if ( new_assoc->file_path.size() > 2 )
	{
		new_assoc->file_path.resize(new_assoc->file_path.size() - 2);
	}
}

