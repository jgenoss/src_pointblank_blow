#pragma once

namespace i3
{

	template<class> struct less;
	template<class> class pool_allocator;

	template<class Key, class Pr = less<Key>, class Alloc = i3::pool_allocator<Key> >
	class vector_set;

	template<class Key, class Pr = less<Key>, class Alloc = i3::pool_allocator<Key> >
	class vector_multiset;
}
