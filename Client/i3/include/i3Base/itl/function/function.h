#if !defined(pp_is_iterating)

#ifndef function_h
#define function_h

#include "function_fwd.h"

#include <typeinfo>

#include "../../pool/class_common_pool.h"
#include "detail/functor_base.h"
#include "detail/functor_fun.h"
#include "detail/functor_fun_ref.h"
#include "detail/functor_mem_fun.h"
#include "detail/functor_obj_mem_fun.h"
#include "../enable_if.h"

#include "../mpl/mpl_multi_if.h"
#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"

#include "../../smart_ptr/scoped_ptr.h"

#include "../utility/create_global_with_atexit.h"

namespace i3
{
	
	template<class R, class Param>
	struct use_delayed_atexit_ginst< detail::null_functor_fun <R, Param> > : true_type {};

	namespace detail
	{
		struct function_ptr_tag{};	struct function_obj_tag{};
		struct member_ptr_tag {};	struct function_obj_ref_tag {};
		
		template<class F>
		struct get_function_tag
		{
			typedef typename mpl::multi_if< is_pointer<F>, function_ptr_tag,
											is_member_pointer<F>, member_ptr_tag,
											is_reference_wrapper<F>, function_obj_ref_tag,
											function_obj_tag>::type		type;
		};
		
		namespace function {	struct useless_clear_type{};	}
	}
}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "function.h") )		// 0부터 반복임..
#include pp_iterate()

namespace i3
{
	template<class T> inline
	void swap(function<T>& lhs, function<T>& rhs) {	lhs.swap(rhs);	}
}

#include "function_operators.h"

#endif // function_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{


	template<class R pp_enum_trailing_params(i_, class T)>
	class function< R (pp_enum_params(i_, T)) > 
	{
	public:
		typedef R																		result_type;
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<T, >::type P, ;)

		typedef function																my_type;
		typedef pp_cat(mpl::list,i_)<pp_enum_params(i_, P)>								parameter_types;
		typedef detail::functor_base<R, parameter_types	>								functor_base_type;
		
		typedef detail::null_functor_fun<R, parameter_types >							null_functor_fun_type;

		struct clear_type {};

		my_type() : m_functor( ms_pnull_functor_fun ) {}
		my_type(clear_type*) : m_functor( ms_pnull_functor_fun ) {}

		my_type(const my_type& rhs) : m_functor( rhs.m_functor->clone()) {}
		my_type&	operator=(const my_type& rhs) { assign_impl(rhs); return *this; } 
		
		template<class F>
		my_type( F f, typename disable_if< mpl::or_<is_integral<F>, is_base_and_derived<my_type, F> >, int>::type = 0 ) : m_functor( create_by_tag<F, typename detail::get_function_tag<F>::type >::call(f) ) {}
		
		template<class Obj, class F>
		my_type( Obj* obj, F f ) : m_functor(new detail::functor_obj_mem_fun<Obj*,F,R, parameter_types >(obj, f)) {}
		
		template<class Obj, class F>	// 이 경우 포인터레퍼런스 / 레퍼런스모두 해당됨..
		my_type( Obj& obj, F f) : m_functor(new detail::functor_obj_mem_fun<Obj*,F,R, parameter_types >(&obj, f)) {}
		
		template<class F> typename disable_if< mpl::or_<is_integral<F>, is_base_and_derived<my_type, F> >,
		my_type&>::type operator=(F f) { assign_impl(f);	return *this; }
		
		my_type& operator=(clear_type*) { clear(); return *this; }

		~function() 
		{
			m_functor->destroy_self();
		}

		template<class Obj, class F>
		my_type& assign(Obj* obj, F f) { assign_impl(obj, f); return *this; }
		template<class Obj, class F>
		my_type& assign(Obj& obj, F f) { assign_impl(obj, f); return *this; }
		
		
		bool		empty() const				{ return m_functor->target_type() == typeid(null_functor_fun_type); }
		operator	bool() const				{ return !empty(); }
		bool		operator!() const			{ return empty(); }

		void		clear()
		{ 
			m_functor->destroy_self();
			m_functor = ms_pnull_functor_fun;
		}

		void					swap(my_type& rhs) {	i3::swap(m_functor, rhs.m_functor);	}
		const std::type_info&	target_type() const { return m_functor->target_type(); }
		template<class Tgt>
		const Tgt*				target() const { return static_cast<const Tgt*>( m_functor->target(typeid(Tgt)) ); }
		template<class Tgt>
		Tgt*					target() { return static_cast<Tgt*>( const_cast<void*>( m_functor->target(typeid(Tgt)) ) ); }

		bool					operator==(const my_type& rhs) const {	return *m_functor == *rhs.m_functor;	}
		bool					operator!=(const my_type& rhs) const { return !(*this == rhs); }
		
		R	operator()( pp_enum_binary_params(i_, P, p) ) const { return (*m_functor)(pp_enum_params(i_, p) ); }
				
		static void				allow_global_variable()
		{
		//	ms_pnull_functor_fun = create_global_with_atexit<null_functor_fun_type,	my_type::ms_pnull_functor_fun, true>();
			ms_pnull_functor_fun = i3::ginst<null_functor_fun_type>::create_explicit();
		}
	protected:

		void assign_impl(const my_type& rhs) 
		{ 
			m_functor->destroy_self();	
			m_functor = rhs.m_functor->clone(); 
		}
		
		template<class F>
		void assign_impl(F f) 
		{ 
			m_functor->destroy_self();
			m_functor = create_by_tag<F, typename detail::get_function_tag<F>::type >::call(f); 
		}

		template<class Obj, class F>
		void assign_impl(Obj* obj, F f)
		{ 
			m_functor->destroy_self();
			m_functor = new detail::functor_obj_mem_fun<Obj*,F,R, parameter_types >(obj, f); 
		} 
		
		template<class Obj, class F>	// 이 경우 포인터레퍼런스 / 레퍼런스모두 해당됨..
		void assign_impl(Obj& obj, F f)
		{ 
			m_functor->destroy_self();
			m_functor = new detail::functor_obj_mem_fun<Obj*,F,R, parameter_types >(&obj, f); 
		}
		
	private:
		
		template<class F, class Tag> struct create_by_tag;

		template<class F> struct create_by_tag<F, detail::function_ptr_tag> 
		{ 
			static functor_base_type* call(F f) 
			{ 
				return new detail::functor_fun<F, R, parameter_types >(f); 
			} 
		};

		template<class F> struct create_by_tag<F, detail::function_obj_tag> 
		{ 
			static functor_base_type* call(F f) 
			{ 
				return new detail::functor_fun<F, R, parameter_types >(f); 
			} 
		};

		template<class F> struct create_by_tag<F, detail::function_obj_ref_tag> 
		{ 
			static functor_base_type* call(F f) 
			{ 
				return new detail::functor_fun_ref<F, R, parameter_types >(f); //래퍼는 내부에서 해소됨..
			} 
		};

#if i_ > 0		// 커버리티 컴파일 에러 때문에 function인수가 없는 경우 멤버함수포인터쪽 분기를 막는다..
		template<class F> struct create_by_tag<F, detail::member_ptr_tag> 
		{
			static const bool is_class_pointer = is_pointer<typename remove_reference<P0>::type >::value;

			static functor_base_type* call(F f) { return create_by_class_type<F, is_class_pointer>::call(f); }						
		};
#endif
		template<class F, bool is_class_pointer>
		struct create_by_class_type
		{   
			static functor_base_type* call(F f) 
			{ 
				return new detail::functor_mem_fun_ptr<F, R, parameter_types >(f); 
			} 
		};
		
		template<class F>
		struct create_by_class_type<F, false> 
		{ 
			static functor_base_type* call(F f) 
			{ 
				return new detail::functor_mem_fun_ref<F, R, parameter_types >(f); 
			} 
		};
						
		functor_base_type*	m_functor;		// 늘 그렇듯 널값 대신, 널함수포인터를 삽입하는게 더 좋음...

		static null_functor_fun_type*	ms_pnull_functor_fun;

	};
	
	template<class R pp_enum_trailing_params(i_, class T)>
	typename function< R (pp_enum_params(i_, T)) >::null_functor_fun_type*  function< R (pp_enum_params(i_, T)) >::ms_pnull_functor_fun = 

		i3::ginst<typename function< R(pp_enum_params(i_, T)) >::null_functor_fun_type>::create_explicit();

	//	create_global_with_atexit<typename function< R(pp_enum_params(i_, T)) >::null_functor_fun_type, 
	//								 function< R(pp_enum_params(i_, T)) >::ms_pnull_functor_fun, 
	//	                             true>();


}

#undef i_

#endif // pp_is_iterating


