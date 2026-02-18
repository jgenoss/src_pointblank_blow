#pragma once

#include <intrin.h>
#include "atomic_base.h"
extern "C" __int64 __cdecl _InterlockedCompareExchange64(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);
#pragma intrinsic (_InterlockedCompareExchange64)

namespace i3
{
	namespace detail
	{
		template<class T>
		class atomic_base_64
		{
		public:
			explicit atomic_base_64(T v) : i(v) {}
			atomic_base_64() {}
			
			T load_with_relaxed() const volatile { return *reinterpret_cast<volatile const T *>(&i); }
			T load_with_cst() const volatile { T v=*reinterpret_cast<volatile const T *>(&i); 	_ReadWriteBarrier(); return v; }
			
			void store_with_relaxed(T v) volatile { *reinterpret_cast<volatile T *>(&i)=v; }
			void store_with_cst(T v) volatile { exchange(v); }
			
			bool compare_exchange_strong(T& expected, T desired) volatile
			{
				T prev = expected; expected=(T)::_InterlockedCompareExchange64((__int64 *)(&i), (__int64)desired, (__int64)expected);
				bool success= (prev==expected);	return success;
			}

			bool compare_exchange_weak(T& expected, T desired) volatile {	return compare_exchange_strong(expected, desired);	}

			T exchange(T r) volatile 
			{ 
#if defined(_M_IX86)
				__int64 Old;
				do 
				{
					Old = (__int64)i;
				} while (::_InterlockedCompareExchange64((__int64*)(&i), (__int64)r, Old) != Old);
				return T(Old);
#elif defined(_M_AMD64)
				return (T)::InterlockedExchange64((__int64*)&i, (__int64)r);
#endif
			}

			T fetch_add(T c) volatile 
			{	
#if defined(_M_IX86)
				__int64 Old;
				do 
				{
					Old = (__int64)i;
				} while (::_InterlockedCompareExchange64((__int64*)(&i), Old + (__int64)c, Old) != Old);
 
				return T(Old);
#elif defined(_M_AMD64)
				return (T)::InterlockedExchangeAdd64((__int64 *)&i, (__int64)c); 
#endif
			}

			bool is_lock_free(void) const volatile { return true; }
			
			typedef T integral_type;
		private:
			T i;
		};
		

		template<class T>
		class atomic_base_integral<T, 8> : public build_atomic_from_add<atomic_base_64<T> > 
		{
		public:
			typedef build_atomic_from_add<atomic_base_64<T> > super;
			explicit atomic_base_integral(T v) : super(v) {}
			atomic_base_integral(void) {}
		};


	}
	
	
	
}