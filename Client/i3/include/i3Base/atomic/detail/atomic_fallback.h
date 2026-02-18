#pragma once

#include "../../smart_ptr/detail/spinlock_pool.h" // 

namespace i3
{
	// 호환성 처리..
	// spin_lock은 busy_wait으로 다른쓰레드의 실행에 지장을 줄수 있음...
	//

	namespace detail
	{
		template<class T>
		class atomic_fallback
		{
		public:
			atomic_fallback(void) {}
			explicit atomic_fallback(const T &t) { ::memcpy(&i, &t, sizeof(T)); }
	
			void store_with_cst(const T &t) volatile
			{
				detail::spinlock_pool<0>::scoped_lock guard(const_cast<T*>(&i)); ::memcpy((void*)&i, &t, sizeof(T));
			}
			void store_with_relaxed(const T &t) volatile { store_with_release(t); }

			T load_with_cst() volatile const { detail::spinlock_pool<0>::scoped_lock guard(const_cast<T*>(&i)); T tmp;	::memcpy(&tmp, (T*)&i, sizeof(T));	return tmp;	}
			T load_with_relaxed() volatile const { detail::spinlock_pool<0>::scoped_lock guard(const_cast<T*>(&i)); T tmp;	::memcpy(&tmp, (T*)&i, sizeof(T));	return tmp;	}

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				detail::spinlock_pool<0>::scoped_lock guard(const_cast<T*>(&i));
				if (memcmp((void*)&i, &expected, sizeof(T))==0) {	::memcpy((void*)&i, &desired, sizeof(T));	return true;  } 
				memcpy(&expected, (void*)&i, sizeof(T));	return false; 
			}

			bool compare_exchange_weak(T &expected, T desired) volatile {	return compare_exchange_strong(expected, desired);	}
			
			T exchange(T replacement) volatile
			{
				detail::spinlock_pool<0>::scoped_lock guard(const_cast<T*>(&i)); 
				T tmp;	::memcpy(&tmp, (void*)&i, sizeof(T)); ::memcpy((void*)&i, &replacement, sizeof(T));
				return tmp;
			}
			bool is_lock_free(void) const volatile {return false;}
		protected:
			T i;
			typedef T integral_type;
		};
	}
}
