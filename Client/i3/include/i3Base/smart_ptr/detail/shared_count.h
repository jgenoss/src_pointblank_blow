#pragma once

#include "sp_counted_impl.h"
#include <memory>

namespace i3
{
	namespace detail
	{
		template<bool> class weak_count;

		template<bool is_mt>	// type_enable_shared_mt에 따라 결정..
		class shared_count		// 거의 일반 클래스임..
		{
		public:

			typedef typename	detail::select_sp_count_base_with_bool<is_mt>::type				sp_counted_base;
			typedef				detail::sp_counted_impl_empty<is_mt>							sp_counted_empty;

			shared_count() : pi(sp_counted_empty::i() ) { pi->add_ref_copy(); }

			template<class T> explicit
			shared_count(T* p) : pi( new sp_counted_impl_p<T>(p) ) {}			// 최적화를 위해 예외는 제거했다..
			
			template<class P, class D>
			shared_count(P p, D d) : pi( new sp_counted_impl_pd<P, D>(p, d) ) {}	// 이 경우 템플릿타입에 포인터가 내장될수 있다...
			
			template<class P, class D, class A>
			shared_count(P p, D d, A a) : pi( new (typename i3::rebind_wrap<A, sp_counted_impl_pda<P,D,A> >::type().allocate(1)) sp_counted_impl_pda<P,D,A>(p, d, a) ) { }
			
			explicit shared_count(sp_counted_base* cb) : pi(cb) {}		// 이런것을 추가...(sp_counted_base를 외부사용자가 쓰게 됨..)

			template<class U> explicit 
			shared_count(std::auto_ptr<U>& r ) : pi( new sp_counted_impl_p<U>( r.get() ) ) { r.release(); }
			
			~shared_count() { pi->release(); }
			shared_count(const shared_count& rhs) : pi(rhs.pi) { pi->add_ref_copy(); }
			
			explicit shared_count(const weak_count<is_mt>& rhs);
			
			shared_count& operator= (const shared_count& rhs)	// 이 구현은 순서와 널객체가 중요함..
			{
				rhs.pi->add_ref_copy();	pi->release(); pi = rhs.pi; 
				return *this;
			}
			
			void swap(shared_count& rhs) {  i3::swap(pi, rhs.pi); }
			long use_count() const { return pi->use_count(); }		// 조건문제거후, 가상함수변환상태..(소량의 호출 부하)
			bool unique() const { return use_count() == 1; }
			bool empty() const  { return pi == sp_counted_empty::i(); }

			void* get_deleter(const std::type_info& ti) const { return pi->get_deleter(ti); }

			friend inline bool operator==(const shared_count& l, const shared_count& r) { return l.pi == r.pi; }
			friend inline bool operator<(const shared_count& l, const shared_count& r) { return l.pi < r.pi; }

		private:
			
			sp_counted_base*	pi;
			
			template<bool> friend class weak_count;
		};
		
		template<bool is_mt>	// type_enable_shared_mt에 따라 결정..
		class weak_count
		{
		public:
			typedef typename	detail::select_sp_count_base_with_bool<is_mt>::type				sp_counted_base;
			typedef				detail::sp_counted_impl_empty<is_mt>							sp_counted_empty;
		
			weak_count() : pi(sp_counted_empty::i()) { pi->weak_add_ref(); }		

			weak_count(const shared_count<is_mt>& rhs) : pi(rhs.pi) { pi->weak_add_ref(); }
			weak_count(const weak_count& rhs) : pi(rhs.pi) { pi->weak_add_ref(); }
			~weak_count() { pi->weak_release(); }
			
			weak_count& operator=(const shared_count<is_mt>& rhs)	
			{ 
				rhs.pi->weak_add_ref();	pi->weak_release(); pi = rhs.pi;
				return *this; 
			}

			weak_count& operator=(const weak_count& rhs)	
			{ 
				rhs.pi->weak_add_ref();	pi->weak_release(); pi = rhs.pi;
				return *this; 
			}

			void swap(weak_count& rhs) {  i3::swap(pi, rhs.pi); }
			long use_count() const { return pi->use_count(); }
			bool empty() const { return pi == sp_counted_empty::i(); }


			friend inline bool operator==(const weak_count& l, const weak_count& r) { return l.pi == r.pi; }
			friend inline bool operator<(const weak_count& l, const weak_count& r) { return l.pi < r.pi; }
			

		private:
			sp_counted_base*	pi;

			template<bool> friend class shared_count;
		};

		template<bool is_mt>
		inline shared_count<is_mt>::shared_count( const weak_count<is_mt>& rhs ) : pi( rhs.pi ) 
		{
			if (pi == sp_counted_empty::i() )
			{
				pi->add_ref_copy();
			}
			else if ( !pi->add_ref_lock() ) 
			{
				pi = sp_counted_empty::i();	pi->add_ref_copy();
			}
		}

	}
}
