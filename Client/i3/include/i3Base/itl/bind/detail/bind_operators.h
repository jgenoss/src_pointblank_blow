#pragma once

namespace i3
{
	namespace bi
	{
		struct logical_not
		{
			template<class V> bool operator()(const V& v) const { return !v; }
		};
		
		template<class R, class F, class L>
		bind_t<bool, logical_not, list1< bind_t<R,F,L> > > operator!(const bind_t<R,F,L>& f)
		{
			typedef list1<bind_t<R,F,L> > list_type;
			return bind_t<bool, logical_not, list_type>( logical_not(), list_type(f) );
		};
	}
}

#define bind_operator_xxx (equal, ==)
#include "bind_operator_xxx.h"
#define bind_operator_xxx (not_equal, !=)
#include "bind_operator_xxx.h"

#define bind_operator_xxx (less, <)
#include "bind_operator_xxx.h"
#define bind_operator_xxx (less_equal, <=)
#include "bind_operator_xxx.h"
#define bind_operator_xxx (greater, >)
#include "bind_operator_xxx.h"
#define bind_operator_xxx (greater_equal, >=)
#include "bind_operator_xxx.h"

#define bind_operator_xxx (logical_and, &&)
#include "bind_operator_xxx.h"
#define bind_operator_xxx (logical_or, ||)
#include "bind_operator_xxx.h"

