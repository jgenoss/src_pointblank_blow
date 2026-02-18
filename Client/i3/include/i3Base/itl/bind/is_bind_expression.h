#pragma once

namespace i3
{
	template<class T> struct is_bind_expression : false_type {};
	template<class R, class F, class L> struct is_bind_expression< bi::bind_t<R,F,L> > : true_type {};
}

