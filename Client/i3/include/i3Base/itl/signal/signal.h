#if !defined(pp_is_iterating)

#ifndef signal_h
#define signal_h

//
// 부스트 구현이 너무 크고, vector이외에 내부에 map/list를 섞어쓴 부분이 핵심에 자리잡고 있어, 그쪽 구현은 참고하지 않는다..
// 그룹(인덱스순서관리)를 하려면 반드시 map컨테이너가 들어가야하며, 이런 구현을 원치 않으므로 우선적으로 생략한다..
// Combiner구현은 필요한듯 한데...컨테이너가 요구되서 이쪽 부분 구현을 좀 고민해야한다..
// Combiner에서 리턴값들을 순회하는 그 루틴안에 실제 함수 호출이 포함된다... (이것때문에 iterator_facade가 들어가게됨)
// 따라서, InputIterator가 함수호출이 될수 있도록 꾸며야한다..(이게 가능하려면 인수보관이 필요함-->레퍼런스 타입 튜플로 대체한다..)
//
#include "signal_common.h"

#include "last_value.h"

#include "../type_traits/function_traits.h"
#include "../function/function.h"
#include "../../smart_ptr/make_shared.h"

#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"

#include "signal_call_iterator.h"
#include "detail/call_bound.h"
#include "../type_traits/is_template_base_of.h"

namespace i3
{

	template< class FunSig, 
			  class Combiner = sig::last_value< typename i3::function_traits<FunSig>::result_type > > 
	class signal {};	// 선언으로는 에러가 나고, 구현이 되어야 한다.  (디폴트 매개변수에 의한 오류인듯 하다..)
	
	
	template<class, class>
	class signal_base;

	template<class T>
	struct is_signal : i3::is_template_base_of< i3::signal_base<mpl_,mpl_>, T> {};

	namespace detail
	{
	
		template<class Fun>					// i3::function이어야함..
		struct connection_holder : connection_holder_base
		{
			typedef i3::list< shared_ptr< connection_holder<Fun> > >	list_type;
			typedef	typename list_type::iterator						list_iterator;
			
			connection_holder( list_type& l) :m_list(l) 
			{
			}

			virtual ~connection_holder() {}

			template<class Fun2> 
			connection_holder( list_type& l, const Fun2& f, 
							typename i3::enable_if< is_convertible<Fun2, Fun>, int>::type = 0 ) : m_list(l), m_fun(f)
			{
			}

			virtual void disconnect() 
			{
				m_list.erase(m_iter);				// delete *this 효과가 들어있을수 있다.. 삭제후엔 connection클래스에서 이 함수호출이 막힘..
			}

			virtual void disconnect_by_embedded_object() {}
		
			list_type&			m_list;			// signal이 유효할때만 유효하다..(역시 weak_ptr로 막히기 때문에 유효성 점검은 필요없다.)   
			list_iterator		m_iter;			//									
			Fun					m_fun;			// F는 i3::function이 젤 좋다..
		};
		

		template<class Fun>					// i3::function이어야함..
		struct embedded_connection_holder : connection_holder<Fun>
		{
			typedef connection_holder<Fun>								base_type;
			typedef typename base_type::list_type						list_type;

			typedef typename base_type::embedded_list_type				embedded_list_type;

			typedef	typename list_type::iterator						list_iterator;
			typedef typename embedded_list_type::iterator				embedded_list_iterator;

			embedded_connection_holder(embedded_list_type& e, list_type& l) : m_embedded_list(e), m_embedded_iterator(e.end()), base_type(l)
			{
			}

			template<class Fun2> 
			embedded_connection_holder(embedded_list_type& e, list_type& l, const Fun2& f, 
						typename i3::enable_if< is_convertible<Fun2, Fun>, int>::type = 0 ) : m_embedded_list(e), m_embedded_iterator(e.end()), base_type(l, f)
			{
			}
			
			virtual ~embedded_connection_holder() 
			{
				if (m_embedded_iterator != m_embedded_list.end())
					m_embedded_list.erase(m_embedded_iterator);		// 
			}

			virtual void disconnect() 
			{
				m_embedded_list.erase(m_embedded_iterator);		// 
				m_embedded_iterator = m_embedded_list.end();
				m_list.erase(m_iter);						// delete *this 효과가 들어있을수 있다..그것때문에 it를 대신 썼다..
			}
			
			virtual void disconnect_by_embedded_object()
			{
				m_list.erase(m_iter);						// delete *this 효과가 들어있을수 있다..그것때문에 it를 대신 썼다..
			}
			
			embedded_list_type&		m_embedded_list;		
			embedded_list_iterator	m_embedded_iterator;
		};

		template<class T>
		struct get_result_type_impl
		{
			typedef typename T::result_type			type;
		};
		
	}

		
	template<class FunSig, class Combiner>			
	class signal_base : private i3::enable_ebo< Combiner >
	{
	public:

		typedef FunSig														signature_type;
		typedef typename i3::function_traits<FunSig>::result_type			signature_result_type;

		typedef typename mpl::eval_if<	has_result_type<Combiner>, 
								detail::get_result_type_impl<Combiner>, 
								i3::identity<signature_result_type> >::type	result_type; 
			
		typedef			i3::function<FunSig>								slot_function_type;
		
		typedef			Combiner											combiner_type;
		

		typedef			detail::connection_holder<slot_function_type>		holder_type;
		typedef			typename holder_type::list_type						list_type;
		typedef			i3::vector< weak_ptr< holder_type > >				weak_vector_type;
		
		typedef			typename list_type::iterator						list_iterator;
		
		signal_base() {}
		~signal_base() 
		{
		
		}
		
		template<class Func> 
		typename i3::enable_if<	is_convertible<Func, slot_function_type>,
		connection>::type	connect(const Func& f) 
		{
			shared_ptr<holder_type> p = i3::make_shared<holder_type>( i3::ref(m_list), f );
			p->m_iter = m_list.insert(m_list.end(), p);
			m_cache_weak_list.reserve(m_list.size());			// 미리 예약할 필요가 있어 보인다..
			return connection(p);
		}

		template<class Func> 
		typename i3::enable_if< is_convertible<Func, slot_function_type>,
		connection>::type	connect_front(const Func& f) 
		{
			shared_ptr<holder_type> p = i3::make_shared<holder_type>( i3::ref(m_list), f );
			p->m_iter = m_list.insert(m_list.begin(), p);
			m_cache_weak_list.reserve(m_list.size());			// 미리 예약할 필요가 있어 보인다..
			return connection(p);
		}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<class B, class Func> 
		typename i3::enable_if< is_convertible<Func, slot_function_type>,
		connection>::type	connect_embedded(shared_disconnect<B>* obj, const Func& f) 
		{
			typedef detail::embedded_connection_holder<slot_function_type> embeded_holder_type;
			shared_ptr<embeded_holder_type> p = i3::make_shared< embeded_holder_type >(i3::ref(obj->get_embedded_connection_list()), 
																						i3::ref(m_list), f );

			p->m_iter = m_list.insert(m_list.end(), p);
			p->m_embedded_iterator = p->m_embedded_list.insert(p->m_embedded_list.end(), p.get());
			m_cache_weak_list.reserve(m_list.size());			// 미리 예약할 필요가 있어 보인다..

			return connection(p);
		}

		template<class B, class Func> 
		typename i3::enable_if< is_convertible<Func, slot_function_type>,
		connection>::type	connect_embedded_front(shared_disconnect<B>* obj, const Func& f) 
		{
			typedef detail::embedded_connection_holder<slot_function_type> embeded_holder_type;
			shared_ptr<embeded_holder_type> p = i3::make_shared< embeded_holder_type >(i3::ref(obj->get_embedded_connection_list()), 
																						i3::ref(m_list), f );
			p->m_iter = m_list.insert(m_list.begin(), p);
			p->m_embedded_iterator = p->m_embedded_list.insert(p->m_embedded_list.end(), p.get());
			m_cache_weak_list.reserve(m_list.size());			// 미리 예약할 필요가 있어 보인다..

			return connection(p);
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void			disconnect_all()
		{
			m_list.clear();			
		}

		size_t			size() const	{ return m_list.size(); }
		bool			empty() const	{ return m_list.empty(); }
	
		typedef enable_ebo< Combiner >		ebo_base;
		combiner_type	combiner() const { return ebo_base::get(); }
		void			set_combiner(const combiner_type& c) { ebo_base::get() = c; }
		
	
	protected:	
		
		combiner_type&				combiner_ref() { return ebo_base::get(); }
		const combiner_type&		combiner_ref() const { return ebo_base::get(); }
		
		list_type					m_list;					// 홀더리스트..
		mutable weak_vector_type	m_cache_weak_list;		// 임시 목록 리스트.. (함수호출시에 임시로만 쓴다..)

		template<class, class> friend class signal; 
	};
	
	



}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "signal.h") )		// 0부터 반복임..
#include pp_iterate()


namespace i3
{
	template< class FunSig, class Combiner> inline
	void swap( signal<FunSig, Combiner>& lhs, signal<FunSig, Combiner>& rhs )
	{
		lhs.swap(rhs);
	}
}


#endif // signal_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	
	template<class R pp_enum_trailing_params(i_, class A), class Combiner> 
	class signal< R (pp_enum_params(i_, A) ), Combiner> : public signal_base< R (pp_enum_params(i_, A) ), Combiner >
	{
	public:

		typedef signal< R (pp_enum_params(i_, A) ), Combiner>			this_type;
		typedef signal_base< R (pp_enum_params(i_, A) ), Combiner >		base_type;
		typedef typename base_type::signature_type						signature_type;
		typedef typename base_type::result_type							result_type;
		typedef typename base_type::slot_function_type					slot_function_type;
		
	private:

		typedef			typename base_type::list_type					list_type;
		typedef			typename base_type::weak_vector_type			weak_vector_type;
		
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<A, >::type P, ;)
	public:	
		// 함수형에 맞춰서..쭈욱..
		
		result_type		operator()( pp_enum_binary_params(i_, P, p) ) const
		{
			typedef call_bound<signature_type>		call_bound_type;
			typedef signal_call_iterator_cache< call_bound_type, list_type, weak_vector_type>	cache_type;

#if i_ > 0
			call_bound_type	bound( pp_enum_params(i_, p) );
#else
			call_bound_type bound;
#endif
			cache_type cache(bound, m_list, m_cache_weak_list);

			return combiner_ref()( signal_call_iterator<call_bound_type, cache_type>( m_list.begin(), cache ), 
							signal_call_iterator<call_bound_type, cache_type>( m_list.end(), cache) );
		}
		
		void			swap( this_type& rhs) { i3::swap( m_list, rhs.m_list); }
	};
	
}

#undef  i_

#endif // pp_is_iterating
