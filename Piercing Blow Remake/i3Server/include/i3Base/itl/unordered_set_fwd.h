#pragma once

namespace i3
{
	template<class> class pool_allocator;
	template <class T> struct hash;
	template <class T> struct equal_to;
	
	template	<	
		class T,  
		class Hash = i3::hash<T>,	
		class Pred = i3::equal_to<T>, 
		class Alloc = i3::pool_allocator<T> 
	>
	class unordered_set;

	template	<	
		class T, 
		class Hash = i3::hash<T>,	
		class Pred = i3::equal_to<T>, 
		class Alloc = i3::pool_allocator<T > 
	>
	class unordered_multiset;
	
	
}
