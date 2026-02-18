#pragma once

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< typename F > inline F& unwrap(F& f, long) {	return f;	}
			template< typename F > inline F& unwrap(reference_wrapper<F>& f, int) {	return f; }
			template< typename F > inline F& unwrap(reference_wrapper<F> const& f, int) { return f; }
		}
	}
}
