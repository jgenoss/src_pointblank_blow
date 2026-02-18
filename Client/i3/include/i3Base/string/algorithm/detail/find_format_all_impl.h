#pragma once

#include "../../itl/range/generic_insert_iterators.h"
#include "../../itl/range/generic_erase_iterators.h"


namespace i3
{
	namespace detail
	{
		template<class OutIt, class Input, class Finder, class Formatter, 
			class FindResult, class FormatResult> 
		OutIt find_format_all_copy_impl2(OutIt out, const Input& input, Finder finder, 
			Formatter formatter, const FindResult& find_res, const FormatResult& format_res)
		{
			typedef typename range_const_iterator<Input>::type input_iterator_type;
			
			input_iterator_type last_match = begin(input);
			FindResult   tmp_find_res    = find_res;
			FormatResult tmp_format_res  = format_res;

			while ( !empty( tmp_find_res )  )
			{
				out = i3::copy( last_match, begin(tmp_find_res), out);
				out = i3::copy( begin(tmp_format_res), end(tmp_format_res), out);
				
				last_match = end(tmp_find_res);
				tmp_find_res = finder(last_match, end(input));
				tmp_format_res = formatter( tmp_find_res );
			}
			
			out = i3::copy( last_match, end(input), out);
			return out;
		}

		template<class OutIt, class Input, class Finder, class Formatter, class FindResult> inline
		OutIt find_format_all_copy_impl(OutIt out, const Input& input, Finder finder, 
				Formatter formatter, const FindResult& find_res)
		{		
			if (i3::empty(find_res)) 
				return i3::copy( begin(input), end(input), out);
			
			return find_format_all_copy_impl2(out, input, finder, formatter, find_res, formatter(find_res));
		}
		
		template<class Input, class Finder, class Formatter, class FindResult, class FormatResult> 
		Input find_format_all_copy_impl2( const Input& input, Finder finder, Formatter formatter,
			const FindResult& find_res, const FormatResult& format_res)
		{
			typedef typename range_const_iterator<Input>::type input_iterator_type;

			input_iterator_type last_match = begin(input);

			Input	output;			// 객체를 세운다.
			FindResult   tmp_find_res    = find_res;
			FormatResult tmp_format_res  = format_res;

			while ( !empty( tmp_find_res )  )
			{
				i3::generic_insert_iterators(output, end(output), last_match, begin(tmp_find_res));
				i3::generic_insert_iterators(output, end(output), begin(tmp_format_res), end(tmp_format_res));
				last_match = end(tmp_find_res);
				tmp_find_res = finder(last_match, end(input));
				tmp_format_res = formatter( tmp_find_res );
			}
			
			i3::generic_insert_iterators(output, end(output), last_match, end(input));
			return output;
		}

		template<class Input, class Finder, class Formatter, class FindResult> inline
			Input find_format_all_copy_impl( const Input& input, Finder finder, Formatter formatter,
			const FindResult& find_res)
		{
			if (i3::empty(find_res)) 
				return input;

			return find_format_all_copy_impl2(input, finder, formatter, find_res, formatter(find_res));
		}
		
		template<class Storage, class StorageDiff, class FwdIt>
		FwdIt process_segment(Storage& storage, StorageDiff& storage_startoff, FwdIt insert_it, FwdIt search_it, FwdIt search_next)
		{
			typedef typename Storage::difference_type		storage_diff;

			storage_diff num_storage = i3::distance( storage.begin() + storage_startoff, storage.end() );
			storage_diff dist_insert_and_search = i3::distance( insert_it, search_it);

			if ( num_storage > 0)
			{
				if ( num_storage <= dist_insert_and_search )		// storage가 더 적으므로 그만큼만 카피..이런 경우엔 메모리를 비운다.
				{
					insert_it = i3::copy( storage.begin() + storage_startoff, storage.end(), insert_it);
					
					storage.clear();
					storage_startoff = 0;
				
					insert_it = i3::copy( search_it, search_next, insert_it);
				}
				else
				{
					insert_it = i3::copy(	storage.begin() + storage_startoff, 
						storage.begin() + storage_startoff + dist_insert_and_search, insert_it);

					storage_startoff += dist_insert_and_search;
				
					// 기존값은 벡터에 넣어야하고, 벡터에 있던값은 insert_it에 넣어서 빼야함..
					// 벡터 확장부터 먼저하고, 스토리지값은 나중에 뺀다..

					storage_diff num_insert = i3::distance(insert_it, search_next );
					storage.insert( storage.end(), insert_it, search_next );

					insert_it = i3::copy( storage.begin() + storage_startoff, storage.begin() + storage_startoff + num_insert, insert_it);
					storage_startoff += num_insert;
				}
			}
			else
			{
				if ( insert_it == search_it )
				{
					insert_it = search_next;
				}
				else
				{
					insert_it = i3::copy( search_it, search_next, insert_it);
				}
				
			}

			return insert_it;

		}

		template<class Input, class Finder, class Formatter, class FindResult, class FormatResult> 
		void find_format_all_impl2( Input& inout, Finder finder, Formatter formatter, 
			const FindResult& find_res, const FormatResult& format_res)
		{
			typedef typename range_iterator<Input>::type		input_iterator_type;
			
			// 계속 연달아 generic_replace가 호출되는 상태..
			// 최적화를 위해서는 겹쳐씌워지는 부분은 원소 수정이 이루어지더라도 
			// 원소insert가 안되도록 조정하고 맨 나중에 원소 증가나 삭제가 일어나도록 조정해야한다...
			// 이렇게 하기 위해서는 중간에 결과를 보관하는 컨테이너가 필요하다..
			// boost버전은 뒤삽입/앞삭제를 쓰기 때문에, deque를 사용했는데, 그렇게 하지 말고...
			// vector를 그냥 쓰고, begin에 해당되는 부분을 offset으로 조정해서 처리한다..
			//

			input_iterator_type insert_it = begin(find_res);		// 삽입 시작위치부터 하면 된다..
			input_iterator_type search_it = begin(find_res);		

			FindResult   tmp_find_res    = find_res;
			FormatResult tmp_format_res  = format_res;
			
			typedef typename range_value<Input>::type				value_type;

			typedef i3::vector< value_type >					storage_type;
		//	typedef typename storage_type::iterator					storage_iterator;
			typedef typename storage_type::difference_type		storage_diff;

			storage_type						storage;
			storage_diff						storage_startoff = 0;
				
			while ( !empty( tmp_find_res )  )
			{

				insert_it = detail::process_segment(storage, storage_startoff, 
					 insert_it, search_it, begin(tmp_find_res) );

				search_it = end(tmp_find_res);
				
#pragma warning(push)
#pragma warning(disable : 4127)
				
				if (sizeof(value_type) > 1024 )	// 컴파일 후 조건문이 삭제된다..
				{	// 4개로 자른다..
					if ( storage_startoff > 4 )
					{
						storage.erase( storage.begin(), storage.begin() + storage_startoff);
						storage_startoff = 0;
					}
				}
				else
				{	// 4K Byte 기준으로 자른다..
					if ( storage_startoff > ( 4096 / sizeof( value_type ) ) )
					{
						storage.erase( storage.begin(), storage.begin() + storage_startoff);
						storage_startoff = 0;
					}
				}

#pragma warning(pop)

				storage.insert( storage.end(), begin(tmp_format_res), end(tmp_format_res) );	// 재할당되면 반복자 무효화로 바뀐다..
				 			
				tmp_find_res = finder( search_it, end(inout) );
				tmp_format_res = formatter(tmp_find_res);

			}

			insert_it = detail::process_segment(storage, storage_startoff, insert_it, search_it,
				end(inout));

			typedef typename storage_type::difference_type		storage_diff;
			storage_diff num_storage = i3::distance( storage.begin() + storage_startoff, storage.end() );

			if ( num_storage == 0 )
			{
				i3::generic_erase_iterators( inout, insert_it, end(inout) );
			}
			else
			{
				i3::generic_insert_iterators( inout, end(inout), storage.begin() + storage_startoff, storage.end());
			}
		}
		
		template<class Input, class Finder, class Formatter, class FindResult> inline
		void find_format_all_impl( Input& inout, Finder finder, Formatter formatter, 
			const FindResult& find_res)
		{
			if (i3::empty(find_res)) 
				return;
			find_format_all_impl2(inout, finder, formatter, find_res, formatter(find_res));
		}
		
	}



}