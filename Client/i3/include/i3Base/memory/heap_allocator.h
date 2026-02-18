#pragma once

#pragma warning(push)
#pragma warning(disable : 4100 )

#include "memory_alloc.h"
#include "../itl/allocator.h"

namespace i3
{
	//
	//  디폴트 생성자를 제거한다..(항상 heap 멤버를 갖고 있어야함)
	//
	template<class T>
	class heap_allocator : public allocator<T>
	{	
	public:
		template<class Other>
		struct rebind {	typedef heap_allocator<Other> other;	};
	
		heap_allocator(heap_handle heap) : m_heap(heap) {}
		heap_allocator(const heap_allocator<T>& rhs) : m_heap(rhs.m_heap) {}
		template<class Other>	heap_allocator(const heap_allocator<Other>& rhs) : m_heap(rhs.get_heap()) {}
		template<class Other>
		heap_allocator<T>& operator=(const heap_allocator<Other>& rhs){	 m_heap = rhs.m_heap;  return (*this);	}

		heap_handle		get_heap() const { return m_heap; }

		void			deallocate(pointer _Ptr, size_type) 
		{	
			heap_aligned_free(m_heap, _Ptr);
		}// deallocate object at _Ptr, ignore size

		pointer			allocate(size_type _Count)	
		{	
			return (pointer)heap_aligned_malloc(m_heap, _Count * sizeof(T), __alignof(T) );
		}	// allocate array of _Count elements
		
		pointer			allocate(size_type _Count, const void* ) {	return (allocate(_Count));	}	// allocate array of _Count elements, ignore hint
	
	private:
		heap_handle		m_heap;				// 멤버를 가진다...
	};

	template<class T, class Other> inline
	bool operator==(const heap_allocator<T>& lhs, const heap_allocator<Other>& rhs) {	return lhs.get_heap() == rhs.get_heap(); }// test for allocator equality (always true)

	template<class T, class Other> inline
	bool operator!=(const heap_allocator<T>& lhs, const heap_allocator<Other>& rhs) { return !( lhs == rhs); }// test for allocator inequality (always false)

}

#pragma warning(pop)