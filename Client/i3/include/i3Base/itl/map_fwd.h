#pragma once

namespace i3
{
	template<class> struct less;
	template<class> class pool_allocator;
	template<class, class> struct pair;

	template<class Key, class T, class Pr = less<Key>, class Alloc = i3::pool_allocator< i3::pair<const Key, T> > >
	class map;

	template<class Key,	class T, class Pr = less<Key>, class Alloc = i3::pool_allocator< i3::pair<const Key, T> > >
	class multimap;

}

