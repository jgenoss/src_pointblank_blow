#pragma once

#include "../../../itl/range/as_literal.h"

namespace i3
{
	namespace detail_sprintf
	{
		template<class Ch>
		struct sprint_item : i3::class_common_pool<sprint_item<Ch> >	
		{
			typedef i3::iterator_range<const Ch*>							str_ptr_range;				
			
			str_ptr_range	m_conv_range1;	// 모든 컨테이너와 문자열의 호환성을 찾기 힘들기 때문에 웬간하면 포인터만 따옴.
			str_ptr_range	m_conv_range2;	// 
			str_ptr_range	m_conv_range3;	//
			str_ptr_range	m_src_range;	// 포맷과 포맷 사이 구간 (mid~end)..
											// 최소한 자기 자신의 번호는 따고 들어가야한다..(초반 파싱병행)
			int				m_arg_idx;		// 타입유형, precision, 정렬,패딩,동적삭제 여부 등 정보 포함해야함..(위치문제때문에 필요하다)
			int				m_width;		// 일단 정수형으로..
			int				m_precision;
			int				m_flags;
		};

		struct char_item_tag {};
		struct wchar_item_tag {};
		struct error_item_tag {};

		I3_EXPORT_BASE i3::vector<sprint_item<char>*>&			get_global_item_list_with_tag(char_item_tag);
		I3_EXPORT_BASE i3::vector<sprint_item<wchar_t>*>&		get_global_item_list_with_tag(wchar_item_tag);
		
		template<class Ch> inline
		i3::vector<sprint_item<Ch>*>&			get_item_list() 
		{
			typedef typename mpl::multi_if<	is_same<Ch, char>, char_item_tag,
						is_same<Ch, wchar_t>, wchar_item_tag, error_item_tag>::type		tag;
			return get_global_item_list_with_tag(tag());
		}

	}


//	template<class Ch> struct is_multi_thread_storage_pool< detail_sprintf::sprint_item<Ch> > : true_type {};	
	template<class Ch> struct enable_pool_dbg<detail_sprintf::sprint_item<Ch> > : false_type {};

}
