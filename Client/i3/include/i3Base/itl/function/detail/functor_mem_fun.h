#if !defined(pp_is_iterating)

#ifndef functor_mem_fun_h
#define functor_mem_fun_h



namespace i3
{
	namespace detail
	{
		template<class F, class R, class List>
		class functor_mem_fun;
		template<class F, class R, class List>
		class functor_mem_fun_ptr;
		template<class F, class R, class List>
		class functor_mem_fun_ref;

	}
}

#pragma warning(disable : 4717)	// 제어경로 재귀적이라는 경고인데, 실제로 그렇지 않음..

#define pp_iteration_params_1 (3, (1, ft_arity_limit, "functor_mem_fun.h") )		//항상, 첫인자가 존재하므로 1부터 반복.
#include pp_iterate()

#pragma warning(default : 4717)

#endif // functor_mem_fun_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		
		template<class F, class R, pp_enum_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_mem_fun< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_, P)> >
			: public functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_, P)> > 
		{
		public:
			typedef	 functor_mem_fun												my_type;
			typedef  functor_base<R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >  base_type;
			typedef  typename base_type::root_type									root_type;

			my_type( F f) : m_f(f) {}		
			my_type(const my_type& rhs) : m_f(rhs.m_f) {}

		
			virtual const std::type_info&	target_type() const { return typeid(F); }
			virtual const void*				target(const std::type_info& info) const { return (info == typeid(F)) ? &m_f : 0; }

			virtual bool operator==( const root_type& rhs) const 
			{
				if (typeid(*this) != typeid(rhs) ) return false;
				return m_f == static_cast<const my_type&>(rhs).m_f;
			}

		private:
			F		m_f;

			template<class, class, class > friend class functor_mem_fun_ptr;
			template<class, class, class > friend class functor_mem_fun_ref;
		};
	
		template<class F, class R, pp_enum_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_mem_fun_ptr<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > 
			: public class_common_pool<	functor_mem_fun_ptr< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
										functor_mem_fun<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >
		{
		public:	
			typedef  functor_mem_fun_ptr													my_type;
			typedef	 class_common_pool<	
						functor_mem_fun_ptr< F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
						functor_mem_fun<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >	base_type;

			my_type(F f) : base_type(f) {}		// 우선 놔둠..
			my_type(const my_type& rhs) : base_type(rhs) {}
			
			virtual my_type* clone() const { return new my_type(*this); }
			virtual void	 destroy_self() const { delete this; }				// 구현제거..

			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const 
			{ 
				return (p0->*m_f)( pp_enum_shifted_params(i_, p) ); 
			}
		};
		
		template<class F, class R, pp_enum_params(i_, class P) >		// P는 인자화된것으로 가정..
		class functor_mem_fun_ref<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > 
			: public class_common_pool<	functor_mem_fun_ref<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
										functor_mem_fun<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >
		{
		public:	
			typedef  functor_mem_fun_ref													my_type;
			typedef	 class_common_pool<	functor_mem_fun_ref<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> >,
										functor_mem_fun<F, R, pp_cat(mpl::list, i_)<pp_enum_params(i_,P)> > >	base_type;

			my_type(F f) : base_type(f) {}		// 우선 놔둠..
			my_type(const my_type& rhs) : base_type(rhs) {}
			
			virtual my_type* clone() const { return new my_type(*this); }
			virtual void	 destroy_self() const { delete this; }				// 구현제거..

			virtual R   operator()( pp_enum_binary_params(i_, P, p) ) const 
			{ 
				return (p0.*m_f)( pp_enum_shifted_params(i_, p) ); 
			}
		};

	}
}
#undef i_

#endif // pp_is_iterating