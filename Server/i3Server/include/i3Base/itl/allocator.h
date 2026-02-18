#pragma once

#include "type_traits/remove_const.h"

#pragma push_macro("new")

#undef new


namespace i3
{

	template<class alloc, class T>
	struct rebind_wrap {  typedef typename alloc::template rebind<T>::other		type;  };

	template<class T>
	class allocator	
	{	
	public:
		typedef typename remove_const<T>::type	value_type;
		typedef value_type*						pointer;
		typedef value_type&						reference;
		typedef const value_type*				const_pointer;
		typedef const value_type&				const_reference;
		typedef size_t							size_type;
		typedef ptrdiff_t						difference_type;

		template<class Other>
		struct rebind {	typedef allocator<Other> other;	};

	
		allocator() {}
		allocator(const allocator<T>&) {}
		template<class Other>	allocator(const allocator<Other>&) {}
		template<class Other>
		allocator<T>& operator=(const allocator<Other>&){	return (*this);	}

		pointer			address(reference _Val) const {	return (&_Val);	}
		const_pointer	address(const_reference _Val) const {	return (&_Val);	}

		void			deallocate(pointer _Ptr, size_type) {	::operator delete(_Ptr); }// deallocate object at _Ptr, ignore size
		pointer			allocate(size_type _Count)	{	return (pointer) ::operator new( _Count * sizeof(T) ) ;	}	// allocate array of _Count elements
		pointer			allocate(size_type _Count, const void* ) {	return (allocate(_Count));	}	// allocate array of _Count elements, ignore hint

		void construct(pointer _Ptr, const T& _Val) {	new ((void*)_Ptr) T(_Val); 	}	// construct object at _Ptr with value _Val
		void destroy(pointer _Ptr) 	{	i3::destroy(_Ptr);  }	// destroy object at _Ptr	

		size_type max_size() const 
		{	
			size_type _Count = (size_type) (-1) / sizeof (T);
			return (0 < _Count ? _Count : 1);
		}	// estimate maximum array size
	};

	template<class T, class Other> inline
	bool operator==(const allocator<T>&, const allocator<Other>&) {	return (true); }// test for allocator equality (always true)

	template<class T, class Other> inline
	bool operator!=(const allocator<T>&, const allocator<Other>&) { return (false); }// test for allocator inequality (always false)

	template<> class allocator<void>
	{	// generic allocator for type void
	public:
		typedef void			T;					// !!!
		typedef T*				pointer;
		typedef const T*		const_pointer;
		typedef T				value_type;

		template<class Other> struct rebind	{	typedef allocator<Other> other; };

		allocator() {}
		allocator(const allocator<T>&){}
		template<class Other> allocator(const allocator<Other>&){}
		template<class Other> allocator<T>& operator=(const allocator<Other>&) {	return (*this);	}
	};


}

#pragma pop_macro("new")
