#pragma once

// 리터럴때문에 일단 리턴값을 전체 사이즈로 잡아주는쪽으로 한다..

#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "../type_traits/has_push_back_mem_fun.h"
#include "../type_traits/has_iter_value_insert_mem_fun.h"


namespace i3
{
	// 리터럴의 경우 
	template<class Lit, class Val> inline typename i3::enable_if_iterator<Lit, size_t>::type
	generic_push_back(Lit lit, const Val& val)
	{
		typedef Lit			out_iterator;
		typedef typename i3::iterator_value<Lit>::type	value_type;

		out_iterator end_it = lit;
		size_t prev_len = i3::char_traits<value_type>::length( &(*lit) );
		i3::advance(end_it, prev_len );
		
		*end_it = value_type(val);
		++end_it;
		*end_it = value_type();
		return prev_len + 1;
	}
	
	namespace detail
	{
		struct generic_push_back_mem_fun_tag;
		struct generic_push_back_insert_mem_fun_tag;
		struct generic_push_back_error_tag;

		template<class T>
		struct get_generic_push_back_tag
		{
			typedef typename mpl::multi_if< i3::has_push_back_mem_fun<T>, generic_push_back_mem_fun_tag,
											i3::has_iter_value_insert_mem_fun<T>, generic_push_back_insert_mem_fun_tag, 
											generic_push_back_error_tag >::type	type;
		};
		
		template<class Tag>
		struct generic_push_back_impl;

		template<>
		struct generic_push_back_impl<generic_push_back_mem_fun_tag>
		{
			template<class Seq, class Val> inline static
			size_t call(Seq& seq, const Val& val) 
			{
				seq.push_back(val);
				return seq.size();
			}
		};

		template<>
		struct generic_push_back_impl<generic_push_back_insert_mem_fun_tag>
		{
			template<class Seq, class Val> inline static
			size_t call(Seq& seq, const Val& val) 
			{
				seq.insert(i3::end(seq), val);
				return seq.size();
			}
		};
		
		template<>
		struct generic_push_back_impl<generic_push_back_error_tag>
		{
			template<class Seq, class Val> inline static
			size_t call(Seq& seq, const Val& val) 
			{
				compile_assert_msg(false, container_must_have_push_back_or_insert);
				return seq.size();
			}
		};

	}


	template<class Seq, class Val> inline typename i3::disable_if_iterator<Seq, size_t>::type		
	generic_push_back(Seq& seq, const Val& val)
	{
		typedef typename i3::detail::get_generic_push_back_tag<Seq>::type		tag_type;
		return i3::detail::generic_push_back_impl<tag_type>::call(seq, val);
	}

}
