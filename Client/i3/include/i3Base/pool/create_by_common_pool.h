#if !defined(pp_is_iterating)

#ifndef create_by_common_pool_h
#define create_by_common_pool_h

//
//
//  다중상속이 된 두번째이후 기본클래스 포인터로 소거할 경우라면 작동이 되지 않을 것이라는 단점이 있다.(소멸자작동은 되지만, 메모리포인터가 다른 문제가 있음)
//  예외적인 문제를 해소하려면 인수에 명시적으로 소거할 포인터타입을 캐스팅해주어야한다..
//  그외에는 잘 작동...
//

#pragma push_macro("new")

#undef new

#define create_by_thread_arity_limit		5

#include "../itl/preprocessor/ext/pp_enum_params_1_cat_params.h"
#include "../itl/preprocessor/pp_iterate.h"

namespace i3
{
	namespace detail 
	{
		struct array_header_for_custom_memory 
		{
			void*	head;
			size_t	num_elem;
		};
	}

	template<class T> __forceinline
	T*			create_by_common_pool(i3::identity<T> t)
	{
		return ::new ( static_pool_type_malloc(t , sizeof(T)) ) T;
	}

	template<class T> 
	T*			create_array_by_common_pool(i3::identity<T> t, size_t num_elem)
	{
		static const size_t adjusted_align = static_calculate_adjusted_align<alignment_of<T>::value>::value;
		size_t raw_size = num_elem*sizeof(T) + adjusted_align + sizeof(detail::array_header_for_custom_memory);
		void*  raw_data = static_pool_type_malloc(t , raw_size);
		uintptr_t result = ( uintptr_t(raw_data) + sizeof(detail::array_header_for_custom_memory) + adjusted_align - 1 ) & ~(adjusted_align - 1);
		
		detail::array_header_for_custom_memory* ah = ( (detail::array_header_for_custom_memory*)result) - 1;
		ah->head = raw_data;		ah->num_elem = num_elem;
		return new ( (void*)result ) T[num_elem];
	}

	template<class T> __forceinline
	void		delete_by_common_pool(T* data) 
	{
		if (!data) return;
		data->~T();
		static_pool_type_free(i3::identity<T>(), data);
	}
	template<class T> 
	void		delete_array_by_common_pool(T* data) 
	{
		if (!data) return;

		detail::array_header_for_custom_memory* ah = (detail::array_header_for_custom_memory*) ( uintptr_t(data) & ~(sizeof(uintptr_t) - 1) ) - 1;
		size_t num_elem = ah->num_elem;
		while ( num_elem != 0 )
		{
			(data + num_elem - 1)->~T();	
			--num_elem;
		}
		static_pool_type_free(i3::identity<T>(), ah->head);
	}
	
	
	
	template<class T> __forceinline
	T*			create_by_common_pool_dbg(i3::identity<T> t, const char* file, int line)
	{
		return ::new ( static_pool_type_malloc_dbg(t, sizeof(T), file, line) ) T;
	}

	template<class T> 
	T*			create_array_by_common_pool_dbg(i3::identity<T> t, size_t num_elem, const char* file, int line)
	{
		static const size_t adjusted_align = static_calculate_adjusted_align<alignment_of<T>::value>::value;
		size_t raw_size = num_elem*sizeof(T) + adjusted_align + sizeof(detail::array_header_for_custom_memory);
		void*  raw_data = static_pool_type_malloc_dbg(t , raw_size, file, line);
		uintptr_t result = ( uintptr_t(raw_data) + sizeof(detail::array_header_for_custom_memory) + adjusted_align - 1 ) & ~(adjusted_align - 1);
		detail::array_header_for_custom_memory* ah = ( (detail::array_header_for_custom_memory*)result) - 1;
		ah->head = raw_data;		ah->num_elem = num_elem;

		return new ( (void*)result ) T[num_elem];
	}

	template<class T> __forceinline
	void		delete_by_common_pool_dbg(T* data) 
	{
		if (!data) return;

		data->~T();
		static_pool_type_free_dbg(i3::identity<T>(), data);
	}
	template<class T> 
	void		delete_array_by_common_pool_dbg(T* data) 
	{
		if (!data) return;

		detail::array_header_for_custom_memory* ah = (detail::array_header_for_custom_memory*) ( uintptr_t(data) & ~(sizeof(uintptr_t) - 1) ) - 1;
		size_t num_elem = ah->num_elem;
		while ( num_elem != 0 )
		{
			(data + num_elem - 1)->~T();	
			--num_elem;
		}
		static_pool_type_free_dbg(i3::identity<T>(), ah->head);
	}
	
}

#define		pp_iteration_params_1	(3, (1, create_by_thread_arity_limit, "create_by_common_pool.h"))				// 1단계 파일반복..
#include	pp_iterate()

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)

#define create_by_common_pool(t, ...)		create_by_common_pool_dbg(t, __VA_ARGS__, __FILE__, __LINE__)
#define delete_by_common_pool(data)			delete_by_common_pool_dbg(data)

#define create_array_by_common_pool(t, n, ...)	create_array_by_common_pool_dbg(t, n, __VA_ARGS__, __FILE__, __LINE__)
#define delete_array_by_common_pool(data)	delete_array_by_common_pool_dbg(data)

#endif

}

#pragma pop_macro("new")

#endif		// create_by_common_pool_h

#else		// pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{


	template<class T, pp_enum_params_1(i_, class A)> __forceinline
	T*			create_by_common_pool(i3::identity<T> t, pp_enum_params_1_cat_params(i_, const A, &, a) ) 
	{
		return new ( static_pool_type_malloc(t , sizeof(T)) ) T(pp_enum_params_1(i_, a));
	}

	template<class T, pp_enum_params_1(i_, class A)>  __forceinline
	T*			create_by_common_pool_dbg(i3::identity<T> t, pp_enum_params_1_cat_params(i_, const A, &, a), const char* file, int line)
	{
		return new (static_pool_type_malloc_dbg(t, sizeof(T), file, line) ) T(pp_enum_params_1(i_, a));
	}

	template<class T, pp_enum_params_1(i_, class A)> 
	T*			create_array_by_common_pool(i3::identity<T> t, size_t num_elem, pp_enum_params_1_cat_params(i_, const A, &, a) ) 
	{
		static const size_t adjusted_align = static_calculate_adjusted_align<alignment_of<T>::value>::value;
		size_t raw_size = num_elem*sizeof(T) + adjusted_align + sizeof(detail::array_header_for_custom_memory);
		void*  raw_data = static_pool_type_malloc(t , raw_size);
		uintptr_t result = ( uintptr_t(raw_data) + sizeof(detail::array_header_for_custom_memory) + adjusted_align - 1 ) & ~(adjusted_align - 1);

		detail::array_header_for_custom_memory* ah = ( (detail::array_header_for_custom_memory*)result) - 1;
		ah->head = raw_data;		ah->num_elem = num_elem;

		T*  data = static_cast<T*>(result);
		const size_t cnum_elem = num_elem;
		for (size_t i = 0; i < cnum_elem ; ++i)
		{
			new ( data + i ) T( pp_enum_params_1(i_, a));
		}
		return data;
	}

	template<class T, pp_enum_params_1(i_, class A)>  
	T*			create_array_by_common_pool_dbg(i3::identity<T> t, size_t num_elem, pp_enum_params_1_cat_params(i_, const A, &, a), const char* file, int line)
	{
		static const size_t adjusted_align = static_calculate_adjusted_align<alignment_of<T>::value>::value;
		size_t raw_size = num_elem*sizeof(T) + adjusted_align + sizeof(detail::array_header_for_custom_memory);
		void*  raw_data = static_pool_type_malloc_dbg(t , raw_size, file, line);
		uintptr_t result = ( uintptr_t(raw_data) + sizeof(detail::array_header_for_custom_memory) + adjusted_align - 1 ) & ~(adjusted_align - 1);
		detail::array_header_for_custom_memory* ah = ( (detail::array_header_for_custom_memory*)result) - 1;
		ah->head = raw_data;		ah->num_elem = num_elem;

		T*  data = static_cast<T*>(result);
		const size_t cnum_elem = num_elem;
		for (size_t i = 0; i < cnum_elem ; ++i)
		{
			new ( data + i ) T( pp_enum_params_1(i_, a));
		}
		return data;
	}


}


#undef i_

#endif		// pp_is_iterating
