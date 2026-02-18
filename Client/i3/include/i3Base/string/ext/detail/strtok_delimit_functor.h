#pragma once

#include "../../../itl/range/value_type.h"
#include "../../../itl/algorithm/detail/memchr_trait.h"

namespace i3
{
	namespace detail
	{
		namespace detail_strtok_delimit_functor
		{
			template<class InIt, class Val> inline
			typename i3::disable_if< is_memchr_wmemchr_enable_it_v<InIt, Val>,
			bool>::type call(InIt f, InIt l, Val v)
			{
				for ( ; f != l; ++f)
					if ( v == *f ) return true;
				return false;
			}
			
			template<class InIt, class Val> inline
			typename i3::enable_if< is_memchr_enable_it_v<InIt, Val>,
			bool>::type call(InIt f, InIt l, Val v)
			{
				return ::memchr(f, int(v), l-f) != 0;
			}

			template<class InIt, class Val> inline
			typename i3::enable_if< is_wmemchr_enable_it_v<InIt, Val>,
			bool>::type call(InIt f, InIt l, Val v)
			{
				return ::wmemchr( (const wchar_t*)f, wchar_t(v), l-f) != 0;
			}
		}

#pragma warning(push)
#pragma warning(disable : 4512)
		
		template<class Range>
		struct strtok_delimit_functor : 
			i3::unary_function<typename range_value<Range>::type, bool>
		{

			typedef typename range_value<Range>::type	 value_type;
			typedef typename range_iterator<Range>::type iterator;

			strtok_delimit_functor(const Range& r) : m_r(r) {}	const Range&	m_r;

			bool operator()(value_type c) const
			{
				return detail_strtok_delimit_functor::call(i3::begin(m_r) , i3::end(m_r), c);
			}
		};

#pragma warning(pop)

		template<class Range> inline
		strtok_delimit_functor<Range> make_strtok_delimit_functor(const Range& r) 
		{
			return strtok_delimit_functor<Range>(r);
		}

		template<class C>
		struct strtok_delimit_c_functor
		{
			strtok_delimit_c_functor(C c) : m_c(c) {} C m_c;
			bool operator()(C c) const
			{
				return m_c == c;
			}
		};

		template<class C> inline
		strtok_delimit_c_functor<C> make_strtok_delimit_c_functor(C c)
		{
			return strtok_delimit_c_functor<C>(c);
		}
	}
}