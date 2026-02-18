#pragma once

// replace 대체함수가 아니라서 replace_range라는 이름이 붙음..

//
// 원본구간을 대체 구간으로 변경한다..  
// (replace알고리즘 함수의 경우는 대체값으로만 변경 가능하다.)
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "generic_insert_iterators.h"
#include "generic_erase_iterators.h"
#include "../type_traits/has_mutable_iterator.h"

namespace i3
{
	
	template<class Lit, class FwdIt> inline typename i3::enable_if_iterator<Lit>::type
	generic_replace_range( Lit lit, typename range_iterator<Lit>::type from, typename range_iterator<Lit>::type to,
							FwdIt f, FwdIt l)
	{
		if ( from == to )		// 이렇게 되면 insert와 같아진다..insert함수로 돌림..
		{
			i3::generic_insert_iterators(lit, from, f, l);
			return;
		}
		
		// 길이를 재서, 안전할때까지는 카피를 쓴다.
		typedef typename range_difference<Lit>::type	diff_t;
		diff_t src_diff = i3::distance(from, to);
		diff_t dest_diff = i3::distance(f, l);
		FwdIt  insert_it = f;
		if (src_diff < dest_diff)
		{
			i3::advance(insert_it, src_diff);
			from = i3::copy( f, insert_it, from );
			i3::generic_insert_iterators(lit, from, insert_it, l);			// 나머지는 삽입하고 종료..
		}
		else
		{
			i3::advance(insert_it, dest_diff);
			from = i3::copy( f, insert_it, from);
			
			if (src_diff > dest_diff)		// 이 경우에는 삭제..길이가 같다면 복사외에는 일이 없음..
			{
				i3::generic_erase_iterators(lit, from, to);
			}
		}	
	}
	
	namespace detail
	{
		template<class Seq, class FwdIt, bool has_mutable_iterator>
		struct generic_replace_range_impl3
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
				typename range_iterator<Seq>::type to, FwdIt f, FwdIt l)
			{
				// 길이를 재서, 안전할때까지는 카피를 쓴다.
				typedef typename range_difference<Seq>::type	diff_t;
				diff_t src_diff = i3::distance(from, to);
				diff_t dest_diff = i3::distance(f, l);
				FwdIt  insert_it = f;
				if (src_diff < dest_diff)
				{
					i3::advance(insert_it, src_diff);
					from = i3::copy( f, insert_it, from );
					i3::generic_insert_iterators(s, from, insert_it, l);			// 나머지는 삽입하고 종료..
				}
				else
				{
					i3::advance(insert_it, dest_diff);
					from = i3::copy( f, insert_it, from);

					if (src_diff > dest_diff)		// 이 경우에는 삭제..길이가 같다면 복사외에는 일이 없음..
					{
						i3::generic_erase_iterators(s, from, to);
					}
				}				
			}
		};

		template<class Seq, class FwdIt>
		struct generic_replace_range_impl3<Seq, FwdIt, false>
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
				typename range_iterator<Seq>::type to, FwdIt f, FwdIt l)
			{
				//만만한 컨테이너를 세우고, copy / swap을 써서 처리한다..
				typedef typename range_value<Seq>::type		value_type;

				i3::vector<value_type>		tmp_vec( begin(s), from);		// 앞부분만 우선 복사하고..
				tmp_vec.insert(tmp_vec.end(), f, l);
				tmp_vec.insert(tmp_vec.end(), to, end(s));
				
				Seq tmp(begin(tmp_vec), end(tmp_vec));
				i3::copy_or_swap(s, tmp);
			}
		};


		
		template<class Seq, class FwdIt, bool const_insert_erase_mem_fun>
		struct generic_replace_range_impl2
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
				typename range_iterator<Seq>::type to, FwdIt f, FwdIt l)
			{
				from = s.erase( from, to );
				if ( f != l) 			
					s.insert(from, f, l);			
			}
		};
		
		template<class Seq, class FwdIt>
		struct generic_replace_range_impl2<Seq, FwdIt, false>
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
				typename range_iterator<Seq>::type to, FwdIt f, FwdIt l)
			{
				static const bool has_mutable_iterator = i3::has_mutable_iterator<Seq>::value;
				generic_replace_range_impl3<Seq, FwdIt, has_mutable_iterator>::call(s, from, to, f, l);
			}
		};

		template<class Seq, class FwdIt, bool replace_mem_fun_friendly>
		struct generic_replace_range_impl1
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
									 typename range_iterator<Seq>::type to,
									 FwdIt f, FwdIt l)
			{
				s.replace( from, to, f, l);			// replace 멤버함수 직접 사용..
			}
		};
		
		template<class Seq, class FwdIt>
		struct generic_replace_range_impl1<Seq, FwdIt, false>
		{
			static void call(Seq& s, typename range_iterator<Seq>::type from, 
				typename range_iterator<Seq>::type to,
				FwdIt f, FwdIt l)
			{
				static const bool const_insert_erase_mem_fun = is_template_same< i3::list<mpl_, mpl_>, Seq>::value;
				generic_replace_range_impl2<Seq, FwdIt, const_insert_erase_mem_fun>::call(s, from, to, f, l);
			}
		};

	}

	template<class Seq, class FwdIt> inline typename i3::disable_if_iterator<Seq>::type
	generic_replace_range( Seq& s, typename range_iterator<Seq>::type from, typename range_iterator<Seq>::type to,
		FwdIt f, FwdIt l)
	{
		if ( from != to )
		{
			static const bool replace_mem_fun_friendly = mpl::or_< 
				is_template_same< i3::basic_string<mpl_, mpl_, mpl_>, Seq> , 
				is_template_same< i3::stack_basic_string<mpl_, mpl_, mpl_, mpl_>, Seq> >::type::value;
			
			detail::generic_replace_range_impl1<Seq, FwdIt, replace_mem_fun_friendly>::call(s, from, to, f, l);
		}
		else
		{
			i3::generic_insert_iterators(s, from, f, l);
		}

	}


}