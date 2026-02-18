#pragma once

#include "static_visitor.h"

namespace i3
{
	
	template<class R, class T>
	struct visitor_ptr_t : public static_visitor<R>
	{
		typedef R (*visitor_t)(T);
		visitor_t m_visit;

		typedef R result_type;

		typedef typename i3::make_parameter<T>::type	parameter_type;

		explicit visitor_ptr_t(visitor_t visit) : m_visit(visit) {}
		
		result_type operator()( parameter_type p ) const
		{
			return m_visit(p);
		}
	};

	template<class R, class T> inline
	visitor_ptr_t<R, T> visitor_ptr( R (*visit)(T))
	{
		return visitor_ptr_t<R, T>(visit);
	}
		
}