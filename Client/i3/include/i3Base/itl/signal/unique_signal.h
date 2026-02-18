#if !defined(pp_is_iterating)

#ifndef unique_signal_h
#define unique_signal_h

#include "signal_common.h"
#include "../type_traits/function_traits.h"
#include "../function/function.h"
#include "../../smart_ptr/make_shared.h"
#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"
#include "../type_traits/is_template_base_of.h"

namespace i3
{
	template<class FunSig> 
	class unique_signal;	
		
	template<class>
	class unique_signal_base;

	template<class T>
	struct is_unque_signal : i3::is_template_base_of< i3::unique_signal_base<mpl_>, T> {};

	namespace detail
	{
				
		template<class Fun>
		struct unique_connection_holder : connection_holder_base
		{

			typedef i3::shared_ptr<unique_connection_holder<Fun> >			shared_ptr_type;

			unique_connection_holder() : m_pshared_ptr(nullptr) {}

			unique_connection_holder( Fun* f ) : m_fun(f), m_pshared_ptr(nullptr)
			{
			}
			
			~unique_connection_holder()
			{
				m_fun->clear();
			}

			virtual void disconnect() 
			{
				(*m_pshared_ptr).reset();			//  유효해야한다.	//  이 함수를 지나면 삭제될것이다.
			}

			virtual void disconnect_by_embedded_object() {}
		
			Fun*				m_fun;			// F는 i3::function이 젤 좋다..
			shared_ptr_type*	m_pshared_ptr;
		};

		template<class Fun>					// i3::function이어야함..
		struct unique_embedded_connection_holder : unique_connection_holder<Fun>
		{
			typedef unique_connection_holder<Fun>						base_type;
			typedef typename base_type::embedded_list_type				embedded_list_type;
			typedef typename embedded_list_type::iterator				embedded_list_iterator;

			unique_embedded_connection_holder(embedded_list_type& e) : m_embedded_list(e), m_embedded_iterator(e.end())
			{
			}
		
			unique_embedded_connection_holder(embedded_list_type& e, Fun* f) : m_embedded_list(e), m_embedded_iterator(e.end()), base_type(f)
			{
			}
			
			virtual ~unique_embedded_connection_holder() 
			{
				if (m_embedded_iterator != m_embedded_list.end())
					m_embedded_list.erase(m_embedded_iterator);		// 
			}

			virtual void disconnect() 
			{
				m_embedded_list.erase(m_embedded_iterator);		// 
				(*m_pshared_ptr).reset();						// delete *this 효과가 들어있다..
			}
			
			virtual void disconnect_by_embedded_object()
			{
				(*m_pshared_ptr).reset();						// delete *this 효과가 들어있다..
			}
			
			embedded_list_type&		m_embedded_list;		
			embedded_list_iterator	m_embedded_iterator;
		};
		
	}
	
	template<class FunSig>			
	class unique_signal_base 
	{
	public:
		typedef FunSig															signature_type;
		typedef typename i3::function_traits<FunSig>::result_type				signature_result_type;

		typedef signature_result_type											result_type; 
		typedef			i3::function<FunSig>									slot_function_type;
		typedef			detail::unique_connection_holder<slot_function_type>	holder_type;

		unique_signal_base() {}
		~unique_signal_base() 
		{
		
		}
		
		// 이 클래스는 유니크하기 때문에, connection은 이전 것에 대한 disconnect후에 수행된다.
		template<class Func> 
		typename i3::enable_if<	is_convertible<Func, slot_function_type>,
		connection>::type	connect(const Func& f) 
		{	
		
			m_holder = i3::make_shared<holder_type>( &m_fun );	// 이전것은 내부에서 자동 삭제되어야 한다.
			m_holder->m_pshared_ptr = &m_holder;				// 이렇게 처리함...

			m_fun = f;											// 나중에 넣는다..홀더소멸자에서 리셋이 된 뒤 들어온다.

			return connection(m_holder);
		}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<class B, class Func> 
		typename i3::enable_if< is_convertible<Func, slot_function_type>,
		connection>::type	connect_embedded(shared_disconnect<B>* obj, const Func& f) 
		{
		
			typedef detail::unique_embedded_connection_holder<slot_function_type> embeded_holder_type;
			shared_ptr<embeded_holder_type> p = i3::make_shared< embeded_holder_type >(i3::ref(obj->get_embedded_connection_list()), &m_fun );
			
			m_holder = p;
			p->m_pshared_ptr = &m_holder;
			p->m_embedded_iterator = p->m_embedded_list.insert(p->m_embedded_list.end(), p.get());

			m_fun = f;											// 나중에 넣는다..홀더소멸자에서 리셋이 된 뒤 들어온다.
			return connection(p);
		}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void			disconnect() 
		{
			if (m_holder)
				m_holder->disconnect();
		}

		bool			empty() const	{ return m_holder.empty(); }
	
	protected:	
		
		// m_holder는 최적화를 위해 deleter를 삽입하도록 처리한다..

		slot_function_type			m_fun;			// 이제 홀더에서 빼내오고 단독사용한다..(weak_ptr때문에 shared_ptr은 어쨋거나 필요하다)
		i3::shared_ptr<holder_type>	m_holder;				

		template<class> friend class unique_signal; 
	};

}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "unique_signal.h") )		// 0부터 반복임..
#include pp_iterate()


namespace i3
{
	template< class FunSig> inline
	void swap( unique_signal<FunSig>& lhs, unique_signal<FunSig>& rhs )
	{
		lhs.swap(rhs);
	}
}


#endif // unique_signal_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	
	template<class R pp_enum_trailing_params(i_, class A)> 
	class unique_signal< R (pp_enum_params(i_, A) )> : public unique_signal_base< R (pp_enum_params(i_, A) )>
	{
	public:

		typedef unique_signal< R (pp_enum_params(i_, A) )>				this_type;
		typedef unique_signal_base< R (pp_enum_params(i_, A) ) >		base_type;
		typedef typename unique_signal_base::signature_type				signature_type;
		typedef typename unique_signal_base::result_type				result_type;
		typedef typename unique_signal_base::slot_function_type			slot_function_type;
		
	private:
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<A, >::type P, ;)
	public:	
		// 함수형에 맞춰서..쭈욱..
		
		result_type		operator()( pp_enum_binary_params(i_, P, p) ) const
		{
			return m_fun( pp_enum_params(i_, p) );
		}
		
		void			swap( this_type& rhs) 
		{ 
			i3::swap( m_fun, rhs.m_fun);
			i3::swap( m_holder, rhs.m_holder);
		}
	};
	
}

#undef  i_

#endif // pp_is_iterating
