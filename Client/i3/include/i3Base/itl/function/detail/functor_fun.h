#if !defined(pp_is_iterating)

#ifndef functor_fun_h
#define functor_fun_h

namespace i3
{
	namespace detail
	{
		template<class F, class R, class List>
		class functor_fun;
		
		template<class R, class List>
		class null_functor_fun;

		namespace detail_functor_fun
		{
			template<class F, bool equal_op> struct equal_func { static bool call(const F& a, const F& b) { return a == b; } };
			template<class F> struct equal_func<F, false> { static bool call(const F&, const F&) { return true; } };
		}

	}
}

#include "../../type_traits/is_equal_to_comparable.h"
#include "../../preprocessor/repetition/pp_enum_binary_params.h"
#include "../../type_traits/aligned_storage.h"

#pragma push_macro("new")
#undef new


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "functor_fun.h") )
#include pp_iterate()

#pragma pop_macro("new")

#endif // functor_fun_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
	
		template<class F, class R pp_enum_trailing_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_fun< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >
			: public class_common_pool<	functor_fun<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
										functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >
		{
		public:
			typedef	 functor_fun													my_type;
			typedef  functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >	base_type;
			typedef  typename base_type::root_type									root_type;

			my_type(typename make_parameter<F>::type f) : m_f(f) {}		// 함수객체라면 const ref, 포인터면 pointer const //
			my_type(const my_type& rhs) : m_f(rhs.m_f) {}

			virtual my_type* clone() const { return new my_type(*this); }
			virtual void	 destroy_self() const { delete this; }

			virtual const std::type_info&	target_type() const { return typeid(F); }
			virtual const void*				target(const std::type_info& info) const { return (info == typeid(F)) ? &m_f : 0; }

			virtual bool operator==( const root_type& rhs) const 
			{
				if (typeid(*this) != typeid(rhs) ) return false;
				
				return detail_functor_fun::equal_func<F, is_equal_to_comparable<F>::value >::call(m_f, static_cast<const my_type&>(rhs).m_f);
			}

			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const { return m_f( pp_enum_params(i_, p) ); }

		private:
			
			mutable F		m_f;
		};
	

		template<class R pp_enum_trailing_params(i_, class P) >		// P는 인자화된것으로 가정..
		class null_functor_fun< R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >
			: public functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > 
		{
		public:
			typedef	 null_functor_fun												my_type;
			typedef  functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >	base_type;
			typedef  typename base_type::root_type									root_type;

			my_type() {}		
			my_type(const my_type& rhs) {}

			virtual my_type* clone() const { return const_cast<my_type*>(this); }
			virtual void	 destroy_self() const { }				// 구현제거..

			virtual const std::type_info&	target_type() const { return typeid(my_type); }
			virtual const void*				target(const std::type_info& info) const { return nullptr; }

			virtual bool operator==( const root_type& rhs) const 
			{
				return (typeid(*this) == typeid(rhs) );
			}

			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const { return R(); }

		};


	}
}

#undef i_

#endif // pp_is_iterating