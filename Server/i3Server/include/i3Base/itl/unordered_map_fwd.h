#pragma once

namespace i3
{
	template<class> class pool_allocator;
	template <class T> struct hash;
	template <class T> struct equal_to;
	template<class T1, class T2> struct pair;

	template	<	
		class Key, class T, 
		class Hash = i3::hash<Key>,	
		class Pred = i3::equal_to<Key>, 
		class Alloc = i3::pool_allocator<i3::pair<const Key, T> > 
	>
	class unordered_map;

	template	<	
		class Key, class T, 
		class Hash = i3::hash<Key>,	
		class Pred = i3::equal_to<Key>, 
		class Alloc = i3::pool_allocator<i3::pair<const Key, T> > 
	>
	class unordered_multimap;
	
	
}