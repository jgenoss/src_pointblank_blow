#pragma once

#include "atomic_fallback.h"
#include "atomic_builder.h"

#include "valid_atomic_integral.h"
#include "../itl/type_traits/alignment_of.h"

namespace i3
{
	namespace detail
	{
		
		template<class T, unsigned short Size = sizeof(T), unsigned short Align = i3::alignment_of<T>::value >
		class atomic_base : public atomic_fallback<T> 
		{
		public:
			typedef atomic_fallback<T> super;
			atomic_base() {}
			explicit atomic_base(T v) : super(v) {}
		protected:
			typedef typename super::integral_type integral_type;
		};
		
		template<class T, unsigned short Size = sizeof(T) >
		class atomic_base_integral : public build_atomic_from_exchange<atomic_fallback<T> > {
		public:
			typedef build_atomic_from_exchange<atomic_fallback<T> > super;
			
			explicit atomic_base_integral(T v) : super(v) {}
			atomic_base_integral() {}
		protected:
			typedef typename super::integral_type integral_type;
		};
		
		template<class T, unsigned short Size = sizeof(T), bool IsInt = is_valid_atomic_integral<T>::value >
		class internal_atomic;

		template<class T, unsigned short Size>
		class internal_atomic<T, Size, false> : private detail::atomic_base<T> 
		{
		public:
			typedef detail::atomic_base<T> super;
	
			internal_atomic() {}
			explicit internal_atomic(T v) : super(v) {}
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile { store_with_cst(v); return v;}	
	
			using super::is_lock_free;
			using super::load_with_cst;
			using super::load_with_relaxed;
			using super::store_with_cst;
			using super::store_with_relaxed;
			using super::exchange;
	
			bool compare_exchange_strong(T &expected, T desired) volatile { return super::compare_exchange_strong(expected, desired); }
			bool compare_exchange_weak(T &expected, T desired) volatile { return super::compare_exchange_strong(expected, desired); }
		private:
			internal_atomic(const internal_atomic &);
			void operator=(const internal_atomic &);
		};


		template<class T, unsigned short Size>
		class internal_atomic<T, Size, true> : private detail::atomic_base_integral<T> 
		{
		public:
			typedef detail::atomic_base_integral<T> super;
			typedef typename super::integral_type integral_type;
	
			internal_atomic() {}
			explicit internal_atomic(T v) : super(v) {}
	
			using super::is_lock_free;
			using super::load_with_cst;
			using super::load_with_relaxed;
			using super::store_with_cst;
			using super::store_with_relaxed;

			using super::exchange;
			using super::fetch_add;		using super::fetch_sub;		using super::fetch_and;		
			using super::fetch_or;		using super::fetch_xor;
	
			operator integral_type(void) const volatile {return load_with_cst();}
			integral_type operator=(integral_type v) volatile { store_with_cst(v); return v; }	
	
			integral_type operator&=(integral_type c) volatile {return fetch_and(c)&c;}
			integral_type operator|=(integral_type c) volatile {return fetch_or(c)|c;}
			integral_type operator^=(integral_type c) volatile {return fetch_xor(c)^c;}
	
			integral_type operator+=(integral_type c) volatile {return fetch_add(c)+c;}
			integral_type operator-=(integral_type c) volatile {return fetch_sub(c)-c;}
	
			integral_type operator++(void) volatile {return fetch_add(1)+1;}
			integral_type operator++(int) volatile {return fetch_add(1);}
			integral_type operator--(void) volatile {return fetch_sub(1)-1;}
			integral_type operator--(int) volatile {return fetch_sub(1);}
	
			bool compare_exchange_strong(integral_type &expected, integral_type desired) volatile
			{
				return super::compare_exchange_strong(expected, desired);
			}
			
			bool compare_exchange_weak(integral_type &expected, integral_type desired) volatile
			{
				return super::compare_exchange_strong(expected, desired);
			}
		private:
			internal_atomic(const internal_atomic &);
			void operator=(const internal_atomic &);
		};
	}
	

}