#pragma once

#include "any.h"
#include "type_traits/functor_can_be_called.h"
#include "mpl/mpl_iterator_range.h"
#include "mpl/mpl_vector.h"
#include "preprocessor/ext/pp_enum_params_from_to_with_a_default.h"

#define any_visitor_limit	20


namespace i3
{
	namespace detail_any_visitor
	{
		// T를 인수 추론으로 쓰면 안됨..
		template<class Visitor, class T> __forceinline 
		typename i3::enable_if< i3::functor_can_be_called<Visitor, T>,
		typename Visitor::result_type>::type  invoke_visitor(Visitor& v, T& data)
		{
			return v(data);
		}

		template<class Visitor, class T> __forceinline 
		typename i3::disable_if< i3::functor_can_be_called<Visitor, T>,
		typename Visitor::result_type>::type  invoke_visitor(Visitor& , T& )
		{
			return typename Visitor::result_type();
		}

		template<class R, class ItRng>
		struct impl;
		
		template<class R, class Beg, class End>
		struct impl<R, mpl::iterator_range<Beg, End> >
		{
			typedef typename mpl::deref<Beg>::type		data_type;
			
			template<class Visitor> inline static 
			R invoke( Visitor& v, i3::any& a) 
			{
				if (typeid(data_type) == a.type())				
				{
					return invoke_visitor<Visitor, data_type>(v, *i3::unsafe_any_cast<data_type>(&a));
				}
				else
				{
					typedef typename mpl::next<Beg>::type	next_type;
					return impl<R, mpl::iterator_range<next_type, End> >::invoke<Visitor>(v, a);
				}
			}

			template<class Visitor> inline static 
			R invoke( Visitor& v, const i3::any& a) 
			{
				if (typeid(data_type) == a.type())				
				{
					return invoke_visitor<Visitor, const data_type>(v, *i3::unsafe_any_cast<data_type>(&a));
				}
				else
				{
					typedef typename mpl::next<Beg>::type	next_type;
					return impl<R, mpl::iterator_range<next_type, End> >::invoke<Visitor>(v, a);
				}
			}
		};

		template<class R, class End>
		struct impl<R, mpl::iterator_range<End, End> >
		{
			template<class Visitor> inline				// 
			static R	invoke( Visitor& , i3::any& ) {	return R();	}
			template<class Visitor> inline				// 
			static R	invoke( Visitor& , const i3::any& ) {	return R();	}
		};

		template<class R, class Seq>
		struct make_impl
		{
			typedef mpl::iterator_range< typename mpl::begin<Seq>::type, typename mpl::end<Seq>::type > range_type;
			typedef impl<R, range_type>		type;
		};
		
	}

	template<class R, class T0, pp_enum_params_from_to_with_a_default(1, any_visitor_limit, class T, mpl::na) >
	struct any_visitor
	{
		typedef R	result_type;

		typedef typename detail_any_visitor::make_impl<R, 
			typename mpl::vector<pp_enum_params(any_visitor_limit, T)>::type>::type impl_type;

		template<class Visitor> inline static R invoke( Visitor& v, i3::any& a) { return impl_type::invoke<Visitor>(v, a);}
		template<class Visitor> inline static R invoke( const Visitor& v, i3::any& a) { return impl_type::invoke<const Visitor>(v, a);}
		template<class Visitor> inline static R invoke( Visitor& v, const i3::any& a) { return impl_type::invoke<Visitor>(v, a);}
		template<class Visitor> inline static R invoke( const Visitor& v, const i3::any& a) { return impl_type::invoke<const Visitor>(v, a);}
	};

	template<class Visitor> __forceinline
	typename Visitor::result_type apply_visitor( Visitor& v, i3::any& obj) 
	{ 
		return Visitor::invoke(v, obj);
	}

	template<class Visitor> __forceinline
	typename Visitor::result_type apply_visitor( const Visitor& v, i3::any& obj) 
	{ 
		return Visitor::invoke(v, obj);
	}

	template<class Visitor> __forceinline
	typename Visitor::result_type apply_visitor( Visitor& v, const i3::any& obj) 
	{ 
		return Visitor::invoke(v, obj);
	}

	template<class Visitor> __forceinline
	typename Visitor::result_type apply_visitor( const Visitor& v, const i3::any& obj) 
	{ 
		return Visitor::invoke(v, obj);
	}

}
