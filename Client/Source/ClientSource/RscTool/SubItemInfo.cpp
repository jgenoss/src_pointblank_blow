#include "stdafx.h"
#include "SubItemInfo.h"

#include "i3RscPartialFile.h"
#include "ItemData.h"

#include "i3Base/string/compare/generic_is_iless.h"

namespace
{
	void Pushback_ItemDataList_Recursive(SubItemInfo* pThis, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo, 
						i3::vector<ItemDataPtr>&	WeaponDataList,
						ItemDataPtr (*fnCreateChild)(SubItemInfo* , i3RscPartialFile*,i3ResourceFileObjectInfo2*) )
	{
		f->SkipNamei3NamedElement(objInfo);		// 서브 키 이름은 기록하지 않을것임..

		i3::vector<i3ResourceFileObjectInfo2*> info_list;
		f->ReadChildListi3TreeElement(info_list);
		
		const size_t num = info_list.size();
		WeaponDataList.reserve( WeaponDataList.size() + info_list.size() );

		for (size_t i = 0 ; i < num ; ++i )
		{
			if ( f->Geti3RegDataCount(info_list[i]) > 2 )						// i3RegData가 없으면 자식쪽으로 넘겨본다..
				WeaponDataList.push_back( fnCreateChild(pThis, f, info_list[i]) );		
			else
				Pushback_ItemDataList_Recursive(pThis, f, info_list[i], WeaponDataList, fnCreateChild);			
		}
		
	}
}


SubItemInfo::SubItemInfo(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo, 
						 ItemDataPtr (*fnCreateChild)(SubItemInfo*, i3RscPartialFile*,i3ResourceFileObjectInfo2*) ) : 
m_file(f), m_objInfo(objInfo)
{
	f->ReadNamei3NamedElement(objInfo, m_name);

	i3::vector<i3ResourceFileObjectInfo2*> info_list;
	f->ReadChildListi3TreeElement(info_list);

	const size_t num = info_list.size();
	m_ItemDataList.reserve(num);
	for (size_t i = 0 ; i < num ; ++i)
	{
		if ( f->Geti3RegDataCount(info_list[i]) > 2 )						// i3RegData가 없으면 자식쪽으로 넘겨본다..
			m_ItemDataList.push_back( fnCreateChild(this, f, info_list[i]) );		
		else
			Pushback_ItemDataList_Recursive(this, f, info_list[i], m_ItemDataList, fnCreateChild);
	}
}

SubItemInfo::SubItemInfo(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
						 ItemDataPtr (*fnCreateChild)(SubItemInfo*, i3RscPartialFile*,i3ResourceFileObjectInfo2*), i3::identity<SubItemInfo_World_Tag>) :
	m_file(f), m_objInfo(objInfo)
{
	m_name = "StageBattle";

	i3::vector<i3ResourceFileObjectInfo2*> tgt_info_list;	
	
	{
		i3::vector<i3ResourceFileObjectInfo2*> stage_info_list;
		m_file->CollectObjectInfosByMeta("i3StageInfo", stage_info_list);

		const size_t num = stage_info_list.size();
		i3::rc_string stage_class_name;
		i3::rc_string target_class_name = "CStageBattle";

		for (size_t i = 0 ; i < num ; ++i )
		{
			if ( m_file->ReadStageClassNamei3StageInfo(stage_info_list[i], stage_class_name) )
			{
				if (stage_class_name == target_class_name)
				{
					tgt_info_list.push_back(stage_info_list[i]);
				}
			}
		}

		// 알파벳 순 정렬이 안되있으므로, non-sensitive로 정렬을 수행하는게 좋겠다.
		
		struct sort_fn
		{
			sort_fn( i3RscPartialFile* f) : f(f) {}		i3RscPartialFile* f;

			bool operator()(const i3ResourceFileObjectInfo2* lhs, const i3ResourceFileObjectInfo2* rhs) const
			{
				i3::rc_string lhs_name, rhs_name;
				f->ReadNamei3NamedElement(lhs, lhs_name);
				f->ReadNamei3NamedElement(rhs, rhs_name);
				
				return i3::generic_is_iless(lhs_name, rhs_name);
			}
		};

		i3::sort(tgt_info_list.begin(), tgt_info_list.end(), sort_fn(m_file));
						
		//
	}
	
	const size_t num = tgt_info_list.size();
	m_ItemDataList.reserve(num);
	for (size_t i = 0 ; i < num ; ++i)
	{
		m_ItemDataList.push_back( fnCreateChild(this, f, tgt_info_list[i]) );			
	}
		
}
