#pragma once

namespace i3
{
	namespace detail
	{	
		// 주의점 : 정렬이 맞아야함...정렬이 맞지 않으면 기존방식으로 돌리고...
		//          정렬이 맞으면 캐스팅 최적화로 코드를 짜도록 함...
		// 1바이트의 비정수 ..
		template<class T>
		class atomic_base<T, 1, 1> : private atomic_base_integral<uint8_t> 
		{
		public:
			typedef atomic_base_integral<uint8_t> super;
			
			atomic_base() {}
			explicit atomic_base(T t) : super(reinterpret_cast<uint8_t&>(t)) {}
			
			void store_with_cst(T t) volatile { super::store_with_cst(reinterpret_cast<uint8_t&>(t)); }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(reinterpret_cast<uint8_t&>(t)); }
			T load_with_cst() volatile const { uint8_t v = super::load_with_cst(); return reinterpret_cast<T&>(v); }
			T load_with_relaxed() volatile const { uint8_t v = super::load_with_relaxed(); return reinterpret_cast<T&>(v); }

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				return super::compare_exchange_strong(reinterpret_cast<uint8_t&>(expected), reinterpret_cast<uint8_t&>(desired));
			}

			bool compare_exchange_weak(T &expected, T desired) volatile
			{
				return super::compare_exchange_weak(reinterpret_cast<uint8_t&>(expected), reinterpret_cast<uint8_t&>(desired));
			}
			
			T exchange(T replacement) volatile { uint8_t v = super::exchange(reinterpret_cast<uint8_t&>(replacement)); return reinterpret_cast<T&>(v); }
			
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile		{ store_with_cst(v); return v; }	
			using super::is_lock_free;
		};

		// 2바이트의 비정수..

		template<class T, unsigned short A>
		class atomic_base<T, 2, A> : private atomic_base_integral<uint16_t> 
		{
		public:
			typedef atomic_base_integral<uint16_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(to_integral(t)) {} 
	
			void store_with_cst(T t) volatile { super::store_with_cst(to_integral(t)); }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(to_integral(t)); }
			T load_with_cst() volatile const { return from_integral(super::load_with_cst()); }
			T load_with_relaxed() volatile const { return from_integral(super::load_with_relaxed()); }

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				uint16_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_strong(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}

			bool compare_exchange_weak(T &expected, T desired) volatile
			{
				uint16_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_weak(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}
	
			T exchange(T replacement) volatile { return from_integral(super::exchange(to_integral(replacement))); }
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile {store_with_cst(v); return v;}	
			
			using super::is_lock_free;
		protected:
			static inline uint16_t to_integral(T &t) { uint16_t tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
			static inline T from_integral(uint16_t t) { T tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
		};
		

		template<class T>
		class atomic_base<T, 2, 2> : private atomic_base_integral<uint16_t> 
		{
		public:
			typedef atomic_base_integral<uint16_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(reinterpret_cast<uint16_t&>(t)) {} 
	
			void store_with_cst(T t) volatile { super::store_with_cst(reinterpret_cast<uint16_t&>(t)); }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(reinterpret_cast<uint16_t&>(t)); }
			T load_with_cst() volatile const { uint16_t v = super::load_with_cst(); return reinterpret_cast<T&>(v); }
			T load_with_relaxed() volatile const { uint16_t v = super::load_with_relaxed(); return reinterpret_cast<T&>(v);  }

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				return super::compare_exchange_strong(reinterpret_cast<uint16_t&>(expected), reinterpret_cast<uint16_t&>(desired));
			}

			bool compare_exchange_weak(T &expected, T desired) volatile
			{
				return super::compare_exchange_weak(reinterpret_cast<uint16_t&>(expected), reinterpret_cast<uint16_t&>(desired));
			}
	
			T exchange(T replacement) volatile { uint16_t v = super::exchange(reinterpret_cast<uint16_t&>(replacement)); return reinterpret_cast<T&>(v); }
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile {store_with_cst(v); return v;}	
			
			using super::is_lock_free;
		};
		
				
		template<class T, unsigned short A>
		class atomic_base<T, 4, A> : private atomic_base_integral<uint32_t> 
		{
		public:
			typedef atomic_base_integral<uint32_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(to_integral(t)) {}
	
			void store_with_cst(T t) volatile { super::store_with_cst(to_integral(t)); }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(to_integral(t)); }
			T load_with_cst() volatile const { return from_integral(super::load_with_cst()); }
			T load_with_relaxed() volatile const { return from_integral(super::load_with_relaxed()); }

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				uint32_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_strong(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}

			bool compare_exchange_weak(T &expected,	T desired) volatile
			{
				uint32_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_weak(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}
	
			T exchange(T replacement) volatile { return from_integral(super::exchange(to_integral(replacement))); }
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile { store_with_cst(v); return v; }	
	
			using super::is_lock_free;

		protected:
			static inline uint32_t to_integral(T &t) { uint32_t tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
			static inline T from_integral(uint32_t t) { T tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
		};
		
		template<class T>
		class atomic_base<T, 4, 4> : private atomic_base_integral<uint32_t> 
		{
		public:
			typedef atomic_base_integral<uint32_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(reinterpret_cast<uint32_t&>(t)) {}
	
			void store_with_cst(T t) volatile { super::store_with_cst(reinterpret_cast<uint32_t&>(t)); }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(reinterpret_cast<uint32_t&>(t)); }
			T load_with_cst() volatile const { uint32_t v = super::load_with_cst();  return reinterpret_cast<T&>(v);  }
			T load_with_relaxed() volatile const { uint32_t v = super::load_with_relaxed(); return reinterpret_cast<T&>(v);  }

			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				return super::compare_exchange_strong(reinterpret_cast<uint32_t&>(expected), reinterpret_cast<uint32_t&>(desired));
			}

			bool compare_exchange_weak(T &expected,	T desired) volatile
			{
				return super::compare_exchange_weak(reinterpret_cast<uint32_t&>(expected), reinterpret_cast<uint32_t&>(desired));
			}
	
			T exchange(T replacement) volatile { uint32_t v = super::exchange(reinterpret_cast<uint32_t&>(replacement)); return reinterpret_cast<T&>(v); }
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile { store_with_cst(v); return v; }	
	
			using super::is_lock_free;
		};

				
		template<class T, unsigned short A>
		class atomic_base<T, 8, A> : private atomic_base_integral<uint64_t> 
		{
		public:
			typedef atomic_base_integral<uint64_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(to_integral(t)) {}
	
			void store_with_cst(T t) volatile {	super::store_with_cst(to_integral(t));  }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(to_integral(t)); }
			T load_with_cst() volatile const { return from_integral(super::load_with_cst()); }
			T load_with_relaxed() volatile const { return from_integral(super::load_with_relaxed()); }
			
			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				uint64_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_strong(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}

			bool compare_exchange_weak(T &expected, T desired) volatile
			{
				uint64_t _expected, _desired;
				_expected=to_integral(expected);
				_desired=to_integral(desired);
				bool success=super::compare_exchange_weak(_expected, _desired);
				expected=from_integral(_expected);
				return success;
			}
	
			T exchange(T replacement) volatile { return from_integral(super::exchange(to_integral(replacement))); }
	
			operator T(void) const volatile {return load_with_cst();}
			T operator=(T v) volatile {store_with_cst(v); return v;}	
			
			using super::is_lock_free;
		protected:
			static inline uint64_t to_integral(T &t) { uint64_t tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
			static inline T from_integral(uint64_t t) { T tmp; ::memcpy(&tmp, &t, sizeof(t)); return tmp; }
		};


		template<class T>
		class atomic_base<T, 8, 8> : private atomic_base_integral<uint64_t> 
		{
		public:
			typedef atomic_base_integral<uint64_t> super;
	
			atomic_base() {}
			explicit atomic_base(T t) : super(reinterpret_cast<uint64_t&>(t)) {}
	
			void store_with_cst(T t) volatile {	super::store_with_cst(reinterpret_cast<uint64_t&>(t));  }
			void store_with_relaxed(T t) volatile { super::store_with_relaxed(reinterpret_cast<uint64_t&>(t)); }
			T load_with_cst() volatile const { uint64_t v = super::load_with_cst(); return reinterpret_cast<T&>(v); }
			T load_with_relaxed() volatile const { uint64_t v = super::load_with_relaxed(); return reinterpret_cast<T&>(v); }
			
			bool compare_exchange_strong(T &expected, T desired) volatile
			{
				return super::compare_exchange_strong(reinterpret_cast<uint64_t&>(expected), reinterpret_cast<uint64_t&>(desired));
			}

			bool compare_exchange_weak(T &expected, T desired) volatile
			{
				return super::compare_exchange_weak(reinterpret_cast<uint64_t&>(expected), reinterpret_cast<uint64_t&>(desired));;
			}
	
			T exchange(T replacement) volatile { uint64_t v = super::exchange(reinterpret_cast<uint64_t&>(replacement)); return reinterpret_cast<T&>(v); }
	
			operator T(void) const volatile {return load_with_cst(); }
			T operator=(T v) volatile { store_with_cst(v); return v; }	
			
			using super::is_lock_free;
		};




	}
}