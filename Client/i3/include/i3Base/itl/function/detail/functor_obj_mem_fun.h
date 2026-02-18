#if !defined(pp_is_iterating)

#ifndef functor_obj_mem_fun_h
#define functor_obj_mem_fun_h


namespace i3
{
	namespace detail
	{
		template<class ObjPtr, class F, class R, class List>
		class functor_obj_mem_fun;
	}
}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "functor_obj_mem_fun.h") )		// 0부터 반복임..
#include pp_iterate()


#endif // functor_obj_mem_fun_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class ObjPtrParam, class F, class R pp_enum_trailing_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_obj_mem_fun<ObjPtrParam, F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_, P)> >
			: public class_common_pool<	functor_obj_mem_fun<ObjPtrParam, F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_, P)> >,
										functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >
		{
		public:
			typedef	 functor_obj_mem_fun											my_type;
			typedef  functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >	base_type;
			typedef  typename base_type::root_type									root_type;

			my_type(typename make_parameter<ObjPtrParam>::type obj, F f) : m_obj(obj), m_f(f) {}	//obj는 ptr_ref는 원형/ptr은 ptr_const
			my_type(const my_type& rhs) : m_obj(rhs.m_obj), m_f(rhs.m_f) {}

			virtual my_type* clone() const { return new my_type(*this); }
			virtual void	 destroy_self() const { delete this; }				// 구현제거..

			virtual const std::type_info&	target_type() const { return typeid(F); }
			virtual const void*				target(const std::type_info& info) const { return (info == typeid(F)) ? &m_f : 0; }

			virtual bool operator==( const root_type& rhs) const 
			{
				if (typeid(*this) != typeid(rhs) ) return false;
				return	m_obj == static_cast<const my_type&>(rhs).m_obj && m_f == static_cast<const my_type&>(rhs).m_f;
			}

			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const 
			{ 
				return (m_obj->*m_f)( pp_enum_params(i_, p) ); 
			}

		private:
			ObjPtrParam		m_obj;		// 포인터 혹은 포인터레퍼런스..둘중 하나만...
			F				m_f;
		};
	
	}
}

#undef i_

#endif // pp_is_iterating