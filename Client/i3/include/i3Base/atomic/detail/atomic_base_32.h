#pragma once

#include <intrin.h>
#include "atomic_base.h"
#include "../cstdint.h"

extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)

namespace i3
{
	namespace detail
	{
		template<class T>
		class atomic_base_32 
		{
		public:
			explicit atomic_base_32(T v) : i(v) {}
			atomic_base_32() {}
			
			T load_with_relaxed() const volatile { return *reinterpret_cast<volatile const T *>(&i); }
			T load_with_cst() const volatile { T v=*reinterpret_cast<volatile const T *>(&i); 	_ReadWriteBarrier(); return v; }
			
			void store_with_relaxed(T v) volatile { *reinterpret_cast<volatile T *>(&i)=v; }
			void store_with_cst(T v) volatile { exchange(v); }
			
			bool compare_exchange_strong(T& expected, T desired) volatile
			{
				T prev = expected; expected=(T)::InterlockedCompareExchange((long *)(&i), (long)desired, (long)expected);
				bool success= (prev==expected);	return success;
			}

			bool compare_exchange_weak(T& expected, T desired) volatile {	return compare_exchange_strong(expected, desired);	}
			T exchange(T r) volatile { return (T)::InterlockedExchange((long *)&i, (long)r); }
			T fetch_add(T c) volatile {	return (T)::InterlockedExchangeAdd((long *)&i, c); }
			bool is_lock_free(void) const volatile { return true; }
			
			typedef T integral_type;
		private:
			T i;
		};
		
		template<class T>
		class atomic_base_integral<T, 4> : public build_atomic_from_add<atomic_base_32<T> > 
		{
		public:
			typedef build_atomic_from_add<atomic_base_32<T> > super;
			explicit atomic_base_integral(T v) : super(v) {}
			atomic_base_integral(void) {}
		};

		template<typename T>
		class atomic_base_integral<T, 1>: public build_atomic_from_larger_type<atomic_base_32<uint32_t>, T> 
		{
		public:
			typedef build_atomic_from_larger_type<atomic_base_32<uint32_t>, T> super;
			
			explicit atomic_base_integral(T v) : super(v) {}
			atomic_base_integral(void) {}
		};

		template<typename T>
		class atomic_base_integral<T, 2>: public build_atomic_from_larger_type<atomic_base_32<uint32_t>, T> 
		{
		public:
			typedef build_atomic_from_larger_type<atomic_base_32<uint32_t>, T> super;
			explicit atomic_base_integral(T v) : super(v) {}
			atomic_base_integral(void) {}
		};


	}
	
	
	
}