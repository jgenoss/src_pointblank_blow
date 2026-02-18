#pragma once

#include "../../itl/checked_delete.h"
#include "sp_counted_base.h"

#include "../../pool/static_size_pool.h"

#include "../type_enable_shared_mt.h"
#include "../../itl/type_traits/remove_pointer.h"

//
//  이 구현 중에 compressed_tuple의 필요성이 대두됨...(이것은 아직 부스트와 샌드박스에 소스코드가 존재하지 않으며, 포럼에 언급만 되어있다..)
//  현재는 간단이 compressed_pair의 중첩을 사용하여, 문제를 회피한다..  
//

namespace i3
{
	namespace detail
	{
		template<class> class sp_counted_impl_p;
		template<class, class> class sp_counted_impl_pd;
		template<class, class, class> class sp_counted_impl_pda;
	}
	
	struct sp_tag;

	// 아래는 size_pool을 T타입에 따라 멀티쓰레드할당을 켜고 끔...

	template<class T>
	struct is_multi_thread_static_size_pool<sp_tag, detail::sp_counted_impl_p<T> > : type_enable_shared_mt<T> { };
	
	template<class P, class D>
	struct is_multi_thread_static_size_pool<sp_tag, detail::sp_counted_impl_pd<P, D> > : type_enable_shared_mt< typename remove_pointer<P>::type > { };
	
	template<class T> 
	struct reserve_static_size_pool<sp_tag, detail::sp_counted_impl_p<T> > : integral_constant<std::size_t, 32> {};
	template<class P, class D>
	struct reserve_static_size_pool<sp_tag, detail::sp_counted_impl_pd<P, D> > : integral_constant<std::size_t, 32> {};
	
	// 아래는 dbg_util을 끄는 특화..
	template<class T> 
	struct enable_pool_dbg< detail::sp_counted_impl_p<T> > : false_type {};
	
	template<class P, class D> 
	struct enable_pool_dbg< detail::sp_counted_impl_pd<P, D> > : false_type {};
	
	template<class P, class D, class A> 
	struct enable_pool_dbg< detail::sp_counted_impl_pda<P, D, A> > : false_type {};
	

	namespace detail
	{
		template<bool is_mt>
		struct select_sp_count_base_with_bool
		{
			typedef typename mpl::if_c< is_mt, sp_counted_base_mt, sp_counted_base_st>::type	type;
		};

		template<class T>
		struct select_sp_count_base : select_sp_count_base_with_bool< type_enable_shared_mt<T>::value > {};


		// 아래의 빈 클래스는 외부에서 new로 생성하면 안된다...
		template<bool is_mt>
		class sp_counted_impl_empty : public shared_ginst<sp_counted_impl_empty<is_mt>, typename select_sp_count_base_with_bool<is_mt>::type >
		{
		public:
			virtual void		dispose() {}
			virtual void		destroy() {}						// 프로그램 종료를 제외하고는 파괴되어선 안된다..
			virtual void*		get_deleter(const std::type_info&) { return 0; }
			virtual long		use_count() const { return 0; }		// use_count의 가상함수변경은 if조건문을 제거함으로 trade_off될것으로 기대...
		};
		
		namespace
		{
			struct _init_sp_counted_impl_empty_ 
			{
				_init_sp_counted_impl_empty_()
				{
					sp_counted_impl_empty<true>::create_explicit();
					sp_counted_impl_empty<false>::create_explicit();
				}
			} _init_sp_counted_impl_empty_var_;
		}

		template<class T>
		class sp_counted_impl_p : public select_sp_count_base<T>::type  
		{
		public:	
			typedef sp_counted_impl_p<T> this_type;
			
			explicit this_type(T* px) : m_px(px) {}
			virtual void  dispose() { i3::checked_delete(m_px); }
			virtual void* get_deleter(const std::type_info&) { return 0; }

			void* operator new(std::size_t) { return static_size_pool_malloc(identity<sp_tag>(), identity<this_type>() ); }
			void  operator delete(void* p) { static_size_pool_free(identity<sp_tag>(), identity<this_type>(), p); }

		private:
			T*		m_px;
		};
		
		template<class P, class D> 
		class sp_counted_impl_pd : public select_sp_count_base< typename remove_pointer<P>::type >::type
		{
		public:
			typedef sp_counted_impl_pd<P, D> this_type;
			this_type(P p, D d) : m_cpair(p, d) {}
			virtual void	dispose() { del()( ptr() ); }
			virtual void*	get_deleter(const std::type_info& ti) { return ti == typeid(D) ? &reinterpret_cast<char&>( del() ) : 0; }
			
			void* operator new(std::size_t) { return static_size_pool_malloc(identity<sp_tag>(), identity<this_type>() ); }
			void  operator delete(void* p) { static_size_pool_free(identity<sp_tag>(), identity<this_type>(), p); }
			
		private:
			P&				ptr() { return m_cpair.first(); }
			const P&		ptr() const { return m_cpair.first(); }
			D&				del() { return m_cpair.second(); }
			const D&		del() const { return m_cpair.second(); }

			compressed_pair<P, D>	m_cpair;
		};
		
		// pda의 경우 얼로케이터가 따로 주어지고, 바깥에서 할당되고, 삭제는 내부에서 처리..
		template<class P, class D, class A>
		class sp_counted_impl_pda : public select_sp_count_base< typename remove_pointer<P>::type >::type
		{
		public:
			typedef sp_counted_impl_pda<P, D, A> this_type;
			typedef typename rebind_wrap<A, this_type>::type A2;

			this_type(P p, D d, A a) : m_cpair( compressed_pair(p, d), a) {}
				
			virtual void	dispose() { del()( ptr() ); }
			virtual void*	get_deleter(const std::type_info& ti) { return ti == typeid(D) ? &reinterpret_cast<char&>(del()) : 0; }
			
			virtual void	destroy()
			{
				this->~this_type();
				alloc().deallocate(this, 1);
			}

		private:
			P&				ptr() { return m_cpair.first().first(); }
			const P&		ptr() const { return m_cpair.first().first(); }
			D&				del() { return m_cpair.first().second(); }
			const D&		del() const { return m_cpair.first().second(); }
			A2&				alloc() { return m_cpair.second(); }
			const A2&		alloc() const { return m_cpair.second(); }
						
			compressed_pair< compressed_pair<P, D>, A2> m_cpair;			// ebo를 잘 살리려면, 중첩시 중첩의 위치가 대단이 중요하다.
		};
	}

	template<> struct use_delayed_atexit_ginst< detail::sp_counted_impl_empty<true>  > : true_type {};
	template<> struct use_delayed_atexit_ginst< detail::sp_counted_impl_empty<false>  > : true_type {};

}	

