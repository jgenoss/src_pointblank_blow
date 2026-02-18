#if !defined(pp_is_iterating)

#ifndef any_function_h
#define any_function_h

#include "../function_types/ft_result_type.h"
#include "../function_types/ft_is_callable_builtin.h"
#include "../function_types/ft_is_function.h"
#include "../type_traits/call_traits.h"


#include "../../smart_ptr/scoped_ptr.h"   // 
#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"
#include "../preprocessor/ext/pp_enum_params_cat_params.h"
#include "detail/any_fun_impl.h"
#include "detail/detail_any_function.h"


#define pp_af_flag_any_function_impl		0x0000
#define pp_af_flag_create_any_function		0x0001

namespace i3
{
	class any_function_move_t : private i3::noncopyable
	{
	public:
		explicit any_function_move_t(detail::any_fun* p) : p(p) {}
	private:
		detail::any_fun* p;
		friend class any_function;
	};

	class any_function
	{
	public:
		any_function() {}
		any_function(const any_function& rhs) : m_fun( rhs.empty() ? nullptr : rhs.m_fun->clone() ) {}
		any_function(const any_function_move_t& m) : m_fun( m.p ) {}

		template<class F>
		any_function(F f, typename i3::enable_if<i3::ft::is_callable_builtin<F>, int>::type = 0) : 
			m_fun( detail::create_any_fun<F, typename detail_any_function::conv_fun<F>::type>(f) ) {}
		
		template<class FunctionType, class F>
		any_function(i3::identity<FunctionType>, const F& f, typename i3::enable_if<i3::ft::is_function<FunctionType>, int>::type = 0) : 
			m_fun( detail::create_any_fun<const F&, FunctionType>(f) ) {}


		any_function&			operator=(const any_function& rhs) { 	return assign(rhs);  }
		any_function&			operator=(const any_function_move_t& m) {	return assign(m);	}

		template<class F>  typename i3::enable_if<i3::ft::is_callable_builtin<F>,
		any_function&>::type	operator=(F f) 
		{
			typedef typename detail_any_function::conv_fun<F>::type		FT;
			m_fun.reset( detail::create_any_fun<F, FT>(f) );			return *this; 
		}

		any_function&			assign(const any_function& rhs) 
		{ 
			m_fun.reset( rhs.empty() ? nullptr : rhs.m_fun->clone());	return *this;
		}

		any_function&			assign(const any_function_move_t& m) {	m_fun.reset(m.p); return *this; }

		template<class F>  typename i3::enable_if<i3::ft::is_callable_builtin<F>,
		any_function&>::type	assign(F f) 
		{  
			typedef typename detail_any_function::conv_fun<F>::type		FT;
			m_fun.reset( detail::create_any_fun<F, FT>(f) );			return *this; 
		}

		template<class FunctionType, class F> typename i3::enable_if<i3::ft::is_function<FunctionType>,
		any_function&>::type	assign(const F& f) 
		{ 
			m_fun.reset( detail::create_any_fun<F, FunctionType>(f) );	return *this; 
		}
		
		void				swap(any_function& rhs) {	m_fun.swap(rhs.m_fun);	}
		void				move(any_function& rhs) { swap(rhs);	rhs.clear(); }
		void				move(const any_function_move_t& m) {  assign(m); }

		bool				empty() const {	return m_fun.get() == 0; }
		
		void				clear() { m_fun.reset(); }

		const std::type_info& function_typeid() const { return m_fun.get() ? m_fun->function_typeid() : typeid(void);  }
		
		
		template<class FT> typename i3::enable_if<i3::ft::is_function<FT>,
		bool>::type			is_callable() const { return function_typeid() == typeid(FT);  }
		
		bool				is_callable_void() const { return  function_typeid() == typeid(void (void) ); }
		
		template<class A0> typename i3::disable_if<i3::ft::is_function<A0>,
		bool>::type			is_callable_void() const { return function_typeid() == typeid(void (A0) ); }

		void				invoke_void() const 	{	m_fun->invoke<void>();	}

		template<class A0>  typename i3::disable_if<i3::ft::is_function<A0>,
		void>::type			invoke_void(typename i3::make_parameter<A0>::type p0) const {  m_fun->invoke<void, A0>(p0);  }

		template<class FT> typename i3::lazy_enable_if<i3::ft::is_function<FT>,
		i3::ft::result_type<FT> >::type invoke() const
		{ 
			typedef typename i3::ft::result_type<FT>::type    R;
			return m_fun->invoke<R>(); 
		} 
			
		template<class FT> typename i3::lazy_enable_if<i3::ft::is_function<FT>,
		i3::ft::result_type<FT> >::type invoke(typename detail_any_function::get_param<FT,0>::type p0) const
		{
			typedef typename i3::ft::result_type<FT>::type    R;
			typedef typename detail_any_function::get_elem<FT, 0>::type  A0;
			return m_fun->invoke<R, A0>(p0);
		}
		

		#define pp_iteration_params_1 ( 4, (2, ft_arity_limit, "any_function.h", pp_af_flag_any_function_impl) )
		#include pp_iterate()

	private:

		i3::scoped_ptr<detail::any_fun>		m_fun;
	};
	
	inline
	void	swap(i3::any_function& lhs, i3::any_function& rhs) { lhs.swap(rhs); }
	



	template<class F> __forceinline typename i3::enable_if<i3::ft::is_callable_builtin<F>, 
	any_function_move_t>::type create_any_function(F f)  
	{	 
		typedef typename detail_any_function::conv_fun<F>::type	FunctionType;
		return any_function_move_t( detail::create_any_fun<F, FunctionType>(f)); 
	}

	template<class FunctionType, class F> __forceinline typename i3::enable_if<i3::ft::is_function<FunctionType>,
	any_function_move_t>::type create_any_function(const F& f) 
	{  
		return any_function_move_t( detail::create_any_fun<F, FunctionType>(f)); 
	}
	
	template<class F> __forceinline
	any_function_move_t create_any_function_void(const F& f)
	{
		typedef  void (FunctionType)(void);
		return any_function_move_t( detail::create_any_fun<f, FunctionType>(f));
	}
	
	template<class A0, class F> __forceinline typename i3::disable_if<i3::ft::is_function<A0>,
	any_function_move_t>::type create_any_function_void(const F& f)
	{
		typedef  void (FunctionType)(A0);
		return any_function_move_t( detail::create_any_fun<f, FunctionType>(f));
	}
}

#define pp_iteration_params_1 ( 4, (2, ft_arity_limit, "any_function.h", pp_af_flag_create_any_function) )
#include pp_iterate()

#undef pp_af_flag_create_any_function
#undef pp_af_flag_any_function_impl


#endif

#else // pp_is_iterating


#define i_						pp_frame_iteration(1)


#if pp_iteration_flags() == pp_af_flag_any_function_impl

	template<class FT> typename i3::lazy_enable_if<i3::ft::is_function<FT>,
	i3::ft::result_type<FT> >::type invoke( 
	pp_enum_params_cat_params( i_, typename detail_any_function::get_param2<FT>::template apply<, >::type, p) ) const
	{
		typedef typename i3::ft::result_type<FT>::type    R;

		pp_repeat_binary_params_with_a_cat(i_, typedef typename detail_any_function::get_elem2<FT>::template apply<, >::type A, ;)
		
		return m_fun->invoke<R, pp_enum_params(i_, A)>( pp_enum_params(i_, p) );
	}

	template<pp_enum_params(i_, class A) > 
	void		invoke_void( pp_enum_binary_params(i_, typename i3::make_parameter<A, >::type p) ) const 
	{  
		m_fun->invoke<void, pp_enum_params(i_, A)>( pp_enum_params(i_, p) );  
	}

	template<pp_enum_params(i_, class A) > 
	bool		is_callable_void() const { return function_typeid() == typeid(void ( pp_enum_params(i_, A) ) ); }


#elif pp_iteration_flags() == pp_af_flag_create_any_function

namespace i3
{
	template<pp_enum_params(i_, class A), class F> __forceinline 
	any_function_move_t create_any_function_void(const F& f)
	{
		typedef  void (FunctionType)(pp_enum_params(i_, A));
		return any_function_move_t( detail::create_any_fun<f, FunctionType>(f));
	}
}

#endif //pp_iteration_flags()

#undef i_

#endif // pp_is_iterating