#include "i3CommonType.h"

#include "string/ext/detail/sprint_item.h"
//
// 문제가 생기면 compiler옵션 프래그마처리 해야한다..
//

namespace i3
{
	namespace detail_sprintf
	{
		struct tss_item_list 
		{
			tss_item_list()
			{
				static const size_t num_init = 10;

				char_global_item_list.reserve(num_init);
				wchar_global_item_list.reserve(num_init);

				for (size_t i = 0; i < num_init; ++i)
					char_global_item_list.push_back(new sprint_item<char>);
				
				for (size_t i = 0; i < num_init; ++i)
					wchar_global_item_list.push_back(new sprint_item<wchar_t>);
			}
			
			~tss_item_list()
			{
				i3::cu::for_each_delete(wchar_global_item_list);	
				i3::cu::for_each_delete(char_global_item_list);
			}

			i3::vector<sprint_item<char>*>			char_global_item_list;
			i3::vector<sprint_item<wchar_t>*>		wchar_global_item_list;
		};

		tls_ptr<tss_item_list>					g_item_list_ptr;
		
		i3::vector<sprint_item<char>*>&			get_global_item_list_with_tag(char_item_tag) {  return g_item_list_ptr->char_global_item_list; }
		i3::vector<sprint_item<wchar_t>*>&		get_global_item_list_with_tag(wchar_item_tag) { return g_item_list_ptr->wchar_global_item_list; }
	}
}

