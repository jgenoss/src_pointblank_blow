#pragma once

namespace i3
{
	template<class> class pool_allocator;

	template<class T, class Alloc = i3::pool_allocator<T> >
	class slist;
}
