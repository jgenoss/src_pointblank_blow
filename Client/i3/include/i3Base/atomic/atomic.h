#pragma once

#include "detail/atomic_base_32.h"
#include "detail/atomic_base_64.h"
#include "detail/casted_atomic_base.h"
#include "cstdint.h"

namespace i3
{
	
	template<class T>
	class atomic : public detail::internal_atomic<T> 
	{
	public:
		typedef detail::internal_atomic<T> super;
		atomic() {}
		explicit atomic(T v) : super(v) {}
	private:
		atomic(const atomic &);
		void operator=(const atomic &);
	};

	template<>
	class atomic<bool> : private detail::internal_atomic<bool> 
	{
	public:
		typedef detail::internal_atomic<bool> super;
		atomic() {}
		explicit atomic(bool v) : super(v) {}
	
		using super::load_with_cst;
		using super::load_with_relaxed;

		using super::store_with_cst;
		using super::store_with_relaxed;

		using super::compare_exchange_strong;
		using super::compare_exchange_weak;
		using super::exchange;
		using super::is_lock_free;
	
		operator bool(void) const volatile {return load_with_cst();}
		bool operator=(bool v) volatile { store_with_cst(v); return v;}	
	private:
		atomic(const atomic &);
		void operator=(const atomic &);
	};

	// void*는 정수타입처럼 취급..
	template<>
	class atomic<void*> : private detail::internal_atomic<void*, sizeof(void*), true> 
	{
	public:
		typedef detail::internal_atomic<void *, sizeof(void *), true> super;
		
		atomic() {}
		explicit atomic(void * p) : super(p) {}
		using super::load_with_cst;
		using super::load_with_relaxed;
		using super::store_with_cst;
		using super::store_with_relaxed;

		using super::compare_exchange_strong;
		using super::compare_exchange_weak;
		using super::exchange;
		using super::is_lock_free;
		
		operator void *(void) const volatile {return load_with_cst();}
		void * operator=(void * v) volatile { store_with_cst(v); return v;}
	private:
		atomic(const atomic &);
		void * operator=(const atomic &);
	};

/* FIXME: pointer arithmetic still missing */

	template<class T>
	class atomic<T*> : private detail::internal_atomic<intptr_t> 
	{
	public:
		typedef detail::internal_atomic<intptr_t> super;
		atomic() {}
		explicit atomic(T* p) : super((intptr_t)p) {}
	
		T* load_with_cst() const volatile { return (T*)super::load_with_cst(); }
		T* load_with_relaxed() const volatile { return (T*)super::load_with_relaxed(); }
		void store_with_cst(T *v) volatile { super::store_with_cst((intptr_t)v); }
		void store_with_relaxed(T *v) volatile { super::store_with_relaxed((intptr_t)v); }


		bool compare_exchange_weak(T* &expected, T* desired) volatile
		{
			intptr_t expected_=(intptr_t)expected, desired_=(intptr_t)desired;
			bool success=super::compare_exchange_weak(expected_, desired_);
			expected=(T*)expected_;
			return success;
		}

		bool compare_exchange_strong(T*& expected, T* desired) volatile
		{
			intptr_t expected_=(intptr_t)expected, desired_=(intptr_t)desired;
			bool success=super::compare_exchange_strong(expected_, desired_);
			expected=(T*)expected_;
			return success;
		}

		T* exchange(T * replacement) volatile { return (T*)super::exchange((intptr_t)replacement); }
		
		using super::is_lock_free;
	
		operator T* (void) const volatile {return load_with_cst();}
		T* operator=(T* v) volatile { store_with_cst(v); return v; }
	
		T* fetch_add(ptrdiff_t diff) volatile { return (T*)super::fetch_add(diff*sizeof(T)); }
		T* fetch_sub(ptrdiff_t diff) volatile { return (T*)super::fetch_sub(diff*sizeof(T)); }
	
		T* operator++(void) volatile {return fetch_add(1)+1;}
		T* operator++(int) volatile {return fetch_add(1);}
		T* operator--(void) volatile {return fetch_sub(1)-1;}
		T* operator--(int) volatile {return fetch_sub(1);}

		T* operator+=(ptrdiff_t c) volatile {return fetch_add(c)+c;}		// 추가..
		T* operator-=(ptrdiff_t c) volatile {return fetch_sub(c)-c;}
	
	private:
		atomic(const atomic& );
		T * operator=(const atomic& );
	};

	class atomic_flag : private atomic<int> 
	{
	public:
		typedef atomic<int> super;
		using super::is_lock_free;
		
		atomic_flag(bool initial_state) : super(initial_state?1:0) {}
		atomic_flag() {}
		
		bool test_and_set() volatile { return super::exchange(1) != 0; }

		void clear_with_cst() volatile { super::store_with_cst(0); }
		void clear_with_relaxed() volatile { super::store_with_relaxed(0); }
	};

	typedef atomic<char> atomic_char;
	typedef atomic<unsigned char> atomic_uchar;
	typedef atomic<signed char> atomic_schar;
	typedef atomic<uint8_t> atomic_uint8_t;
	typedef atomic<int8_t> atomic_int8_t;
	typedef atomic<unsigned short> atomic_ushort;
	typedef atomic<short> atomic_short;
	typedef atomic<uint16_t> atomic_uint16_t;
	typedef atomic<int16_t> atomic_int16_t;
	typedef atomic<unsigned int> atomic_uint;
	typedef atomic<int> atomic_int;
	typedef atomic<uint32_t> atomic_uint32_t;
	typedef atomic<int32_t> atomic_int32_t;
	typedef atomic<unsigned long> atomic_ulong;
	typedef atomic<long> atomic_long;
	typedef atomic<uint64_t> atomic_uint64_t;
	typedef atomic<int64_t> atomic_int64_t;
	typedef atomic<unsigned long long> atomic_ullong;
	typedef atomic<long long> atomic_llong;
	typedef atomic<void*> atomic_address;
	typedef atomic<bool> atomic_bool;

	static inline void atomic_thread_fence() { _ReadWriteBarrier();	}
	
}
