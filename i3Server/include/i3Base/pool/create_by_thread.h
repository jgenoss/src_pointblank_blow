#pragma once

#include "../itl/mpl/mpl_identity.h"
#include <new>

namespace i3
{
		
	template<class T> __forceinline
	T*			create_by_thread( DWORD threadID, identity<T> )
	{
		return new ( i3::tls_malloc(threadID, sizeof(T)) ) T();
	}

	template<class T> __forceinline
	void		delete_by_thread(DWORD threadID, T* data)
	{
		data->~T();
		i3::tls_free(threadID, data);
	}

///////////////////
	template<class T> __forceinline
	T*			create_by_thread_aligned( DWORD threadID, identity<T>, size_t align)
	{
		return new (i3::tls_aligned_malloc(threadID, sizeof(T), align)) T();
	}
	

	template<class T> __forceinline
	void		delete_by_thread_aligned(DWORD threadID, T* data)
	{
		data->~T();
		i3::tls_aligned_free(threadID, data);
	}
///////////////////

	template<class T, class P1> __forceinline
	T*			create_by_thread( DWORD threadID, identity<T>, const P1& p1)
	{
		return new ( i3::tls_malloc(threadID, sizeof(T)) ) T(p1);
	}
	template<class T, class P1, class P2> __forceinline
	T*			create_by_thread( DWORD threadID, identity<T>, const P1& p1, const P2& p2)
	{
		return new ( i3::tls_malloc(threadID, sizeof(T)) ) T(p1, p2);
	}
	template<class T, class P1, class P2, class P3> __forceinline
	T*			create_by_thread( DWORD threadID, identity<T>, const P1& p1, const P2& p2, const P3& p3)
	{
		return new ( i3::tls_malloc(threadID, sizeof(T)) ) T(p1, p2, p3);
	}		

	template<class T, class P1> __forceinline
	T*			create_by_thread_aligned( DWORD threadID, identity<T>, size_t align, const P1& p1)
	{
		return new (i3::tls_aligned_malloc(threadID, sizeof(T), align)) T(p1);
	}
	
	template<class T, class P1, class P2> __forceinline
	T*			create_by_thread_aligned( DWORD threadID, identity<T>, size_t align, const P1& p1, const P2& p2)
	{
		return new (i3::tls_aligned_malloc(threadID, sizeof(T), align)) T(p1, p2);
	}

	template<class T, class P1, class P2, class P3> __forceinline
	T*			create_by_thread_aligned( DWORD threadID, identity<T>, size_t align, const P1& p1, const P2& p2, const P3& p3)
	{
		return new (i3::tls_aligned_malloc(threadID, sizeof(T), align)) T(p1, p2, p3);
	}








}