#if !defined(pp_is_iterating)

#ifndef functor_fun_ref_h
#define functor_fun_ref_h

#include "../utility/ref.h"
//
// 함수객체의 레퍼런스를 사용...
// 따라서, 일반함수/멤버함수포인터의 경우 일찌감치 unwrap_reference를 걸어야한다..
//
namespace i3
{
	namespace detail
	{
		template<class F, class R, class List>
		class functor_fun_ref;
	}
}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "functor_fun_ref.h") )
#include pp_iterate()


#endif // functor_fun_ref_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)


namespace i3
{
	namespace detail
	{
		
		template<class F, class R pp_enum_trailing_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_fun_ref< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >
			: public class_common_pool<	functor_fun_ref< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
										functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >
		{
		public:
			typedef	functor_fun_ref													my_type;
			typedef	functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >	base_type;
			typedef typename base_type::root_type									root_type;
			typedef typename unwrap_reference<F>::type								value_type;								
			typedef value_type*														pointer;

			my_type(F f) : m_f( f.get_pointer() ) {}			// 레퍼런스래퍼라면 값으로 넘김....
			my_type(const my_type& rhs) : m_f(rhs.m_f) {}

			virtual my_type* clone() const { return new my_type(*this); }
			virtual void	 destroy_self() const { delete this; }				// 구현제거..

			virtual const std::type_info&	target_type() const { return typeid(value_type); }
			virtual const void*				target(const std::type_info& info) const { return (info == typeid(value_type)) ? m_f : 0; }

			virtual bool operator==( const root_type& rhs) const 
			{
				if (typeid(*this) != typeid(rhs) ) return false;
				return m_f == static_cast<const my_type&>(rhs).m_f;		// 포인터 비교로 마무리..(부스트 기준으로 이렇게 되있다..)
			}
			
			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const { return (*m_f)( pp_enum_params(i_, p) ); }
		private:
			pointer		m_f;		// 레퍼런스래퍼에서 포인터로..
		};
		

		
	}
}


#undef i_

#endif // pp_is_iterating