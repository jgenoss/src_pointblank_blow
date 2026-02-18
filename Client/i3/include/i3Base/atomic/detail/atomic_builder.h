#pragma once

#pragma warning(push)
#pragma warning(disable:4146)		// unsigned정수계열에 의도적으로 -부호를 넣은 경우의 경고...

namespace i3
{
	
	namespace detail
	{
		template<class Base>
		class build_fetch_add : public Base 
		{
		public:
			typedef typename Base::integral_type integral_type;
			using Base::compare_exchange_weak;
			
			integral_type fetch_add(integral_type c) volatile
			{
				integral_type o = Base::load_with_relaxed(), n;
				do {n=o+c;} while(!compare_exchange_weak(o, n));
				return o;
			}
			
			build_fetch_add() {}
			explicit build_fetch_add(integral_type i) : Base(i) {}
		};

		template<class Base>
		class build_arithmeticops : public Base 
		{
		public:
			typedef typename Base::integral_type integral_type;
			using Base::fetch_add;
			integral_type fetch_sub(integral_type c) volatile {	return fetch_add(-c); }
			build_arithmeticops() {}
			explicit build_arithmeticops(integral_type i) : Base(i) {}
		};
	
		template<class Base>
		class build_logicops : public Base 
		{
		public:
			typedef typename Base::integral_type integral_type;
			
			using Base::compare_exchange_weak;
			using Base::load_with_relaxed;
			
			integral_type fetch_and(integral_type c) volatile
			{
				integral_type o= load_with_relaxed(), n;
				do {n=o&c;} while(!compare_exchange_weak(o, n));
				return o;
			}

			integral_type fetch_or(integral_type c) volatile
			{
				integral_type o=load_with_relaxed(), n;
				do {n=o|c;} while(!compare_exchange_weak(o, n));
				return o;
			}
			integral_type fetch_xor(integral_type c) volatile
			{
				integral_type o=load_with_relaxed(), n;
				do {n=o^c;} while(!compare_exchange_weak(o, n));
				return o;
			}
			build_logicops() {}
			build_logicops(integral_type i) : Base(i) {}
		};



		template<class Base>
		class build_atomic_from_add : public build_logicops< build_arithmeticops<Base> > 
		{
		public:
			typedef build_logicops< build_arithmeticops<Base> > super;
			typedef typename super::integral_type integral_type;
			build_atomic_from_add(void) {}
			build_atomic_from_add(integral_type i) : super(i) {}
		};

		template<class Base>
		class build_atomic_from_exchange : public build_logicops< build_arithmeticops< build_fetch_add<Base> > > 
		{
		public:
			typedef build_logicops< build_arithmeticops< build_fetch_add<Base> > > super;
			typedef typename super::integral_type integral_type;
			
			build_atomic_from_exchange(void) {}
			build_atomic_from_exchange(integral_type i) : super(i) {}
		};

		template<class Base>
		class build_exchange : public Base 
		{
		public:
			typedef typename Base::integral_type integral_type;
			
			using Base::load_with_relaxed;
			using Base::compare_exchange_weak;
			
			bool compare_exchange_strong(integral_type &expected, integral_type desired) volatile
			{
				integral_type expected_save=expected;
				while(true) 
				{
					if (compare_exchange_weak(expected, desired)) return true;
					if (expected_save!=expected) return false;
					expected=expected_save;
				}
			}
			
			integral_type exchange(integral_type replacement) volatile
			{
				integral_type o = load_with_relaxed();
				do {} while(!compare_exchange_weak(o, replacement));
				return o;
			}
			
			build_exchange() {}
			explicit build_exchange(integral_type i) : Base(i) {}
		};

		template<class Base>
		class build_atomic_from_minimal : public build_logicops< build_arithmeticops< build_fetch_add< build_exchange<Base> > > > 
		{
		public:
			typedef build_logicops< build_arithmeticops< build_fetch_add< build_exchange<Base> > > > super;
			typedef typename super::integral_type integral_type;
			
			build_atomic_from_minimal(void) {}
			build_atomic_from_minimal(typename super::integral_type i) : super(i) {}
		};


		template<class Base, class Type>
		class build_base_from_larger_type 
		{
		public:
			typedef Type integral_type;
	
			build_base_from_larger_type() {}
			build_base_from_larger_type(integral_type t) { store_with_relaxed(t);}
	
			integral_type load_with_cst() const volatile
			{
				larger_integral_type v = get_base().load_with_cst();
				return extract(v);
			}

			integral_type load_with_relaxed() const volatile
			{
				larger_integral_type v = get_base().load_with_relaxed();
				return extract(v);
			}
			
			bool compare_exchange_weak(integral_type &expected, integral_type desired) volatile
			{
				larger_integral_type expected_;
				larger_integral_type desired_;
		
				expected_=get_base().load_with_relaxed();
				expected_=insert(expected_, expected);
				desired_=insert(expected_, desired);
				
				bool success=get_base().compare_exchange_weak(expected_, desired_);
				expected=extract(expected_);
				return success;
			}

			void store_with_cst(integral_type v) volatile
			{
				larger_integral_type expected, desired;
				expected=get_base().load_with_relaxed();
				do 
				{
					desired=insert(expected, v);
				} while(!get_base().compare_exchange_weak(expected, desired));
			}

			void store_with_relaxed(integral_type v) volatile
			{
				larger_integral_type expected, desired;
				expected=get_base().load_with_relaxed();
				do 
				{
					desired=insert(expected, v);
				} while(!get_base().compare_exchange_weak(expected, desired));
			}
	
			bool is_lock_free(void) { return get_base().is_lock_free();	}
		private:
			typedef typename Base::integral_type larger_integral_type;
	
			const Base& get_base(void) const volatile
			{
				intptr_t address=(intptr_t)this;
				address&=~(sizeof(larger_integral_type)-1);
				return *reinterpret_cast<const Base *>(address);
			}

			Base& get_base(void) volatile
			{
				intptr_t address=(intptr_t)this;
				address&=~(sizeof(larger_integral_type)-1);
				return *reinterpret_cast<Base *>(address);
			}
			
			unsigned int get_offset(void) const volatile
			{
				intptr_t address=(intptr_t)this;
				address&=(sizeof(larger_integral_type)-1);
				return address;
			}
	
			unsigned int get_shift(void) const volatile { return get_offset()*8; }
	
			integral_type extract(larger_integral_type v) const volatile { return static_cast<integral_type>( v>>get_shift() ); }
	
			larger_integral_type insert(larger_integral_type target, integral_type source) const volatile
			{
				larger_integral_type tmp=source;
				larger_integral_type mask=(larger_integral_type)-1;
		
				mask=~(mask<<(8*sizeof(integral_type)));
		
				mask=mask<<get_shift();
				tmp=tmp<<get_shift();
		
				tmp=(tmp & mask) | (target & ~mask);
				return tmp;
			}
	
			integral_type i;
		};


		template<class Base, class Type>
		class build_atomic_from_larger_type : public build_atomic_from_minimal< build_base_from_larger_type<Base, Type> > 
		{
		public:
			typedef build_atomic_from_minimal< build_base_from_larger_type<Base, Type> > super;
			typedef Type integral_type;

			build_atomic_from_larger_type() {}
			build_atomic_from_larger_type(integral_type v) : super(v) {}
		};
		
		
		
		
	}
		
}

#pragma warning(pop) 

