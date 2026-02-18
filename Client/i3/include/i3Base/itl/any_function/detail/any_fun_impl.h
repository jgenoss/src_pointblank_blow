#if !defined(pp_is_iterating)

#ifndef any_fun_impl_h
#define any_fun_impl_h

#include "any_fun_impl_base.h"
#include "../../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"
#include "../../preprocessor/repetition/pp_enum_shifted_params.h"

namespace i3
{
	namespace detail
	{
		struct any_function_ptr_tag;
		struct any_function_obj_ref_tag;
		struct any_function_mem_fun_ptr_tag;
		struct any_function_mem_fun_ref_tag;
		struct any_function_error_tag;
		
		template<class F, class FT, class Tag, std::size_t Arity>
		class any_fun_impl;
	}
	
}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "any_fun_impl.h") )
#include pp_iterate()



#endif // any_fun_impl_h


#else // pp_is_iterating

#define i_						pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class F, class FT>
		class any_fun_impl<F, FT, any_function_ptr_tag, i_> : public any_fun_impl_base<FT>
		{
		public:
			typedef any_fun_impl_base<FT>				base_type;
			typedef typename base_type::result_type		result_type;
			typedef result_type							R;
			
			pp_repeat_binary_params_with_a_cat(i_, typedef typename base_type::P, P, ;)
			
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) const
			{
				return m_fun( pp_enum_params(i_, p) );
			}
			
			any_fun_impl(typename make_parameter<F>::type f) : m_fun(f) {}
			any_fun_impl(const any_fun_impl& rhs ) : m_fun(rhs.m_fun) {}
			virtual any_fun*	clone() const {  return new any_fun_impl(*this);  }
		private:
			F		m_fun;
		};

		template<class F, class FT>
		class any_fun_impl<F, FT, any_function_obj_ref_tag, i_> : public any_fun_impl_base<FT>
		{
		public:
			typedef any_fun_impl_base<FT>				base_type;
			typedef typename base_type::result_type		result_type;
			typedef result_type							R;
			pp_repeat_binary_params_with_a_cat(i_, typedef typename base_type::P, P, ;)
			typedef typename unwrap_reference<F>::type	value_type;								
			typedef value_type*							pointer;
			
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) const
			{
				return (*m_fun)( pp_enum_params(i_, p) );
			}			

			any_fun_impl(F f) : m_fun( f.get_pointer() ) {}
			any_fun_impl(const any_fun_impl& rhs) : m_fun(rhs.m_fun) {}
			virtual any_fun*	clone() const {  return new any_fun_impl(*this);  }
			

		private:
			pointer		m_fun;
		};

		template<class MF, class FT>
		class any_fun_impl<MF, FT, any_function_mem_fun_ptr_tag, i_> : public any_fun_impl_base<FT>
		{
		public:
			typedef any_fun_impl_base<FT>				base_type;
			typedef typename base_type::result_type		result_type;
			typedef result_type							R;
			pp_repeat_binary_params_with_a_cat(i_, typedef typename base_type::P, P, ;)
			
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) const
			{
				return (p0->*m_fun)(pp_enum_shifted_params(i_, p));
			}		
			
			any_fun_impl(typename make_parameter<MF>::type f) : m_fun(f) {}
			any_fun_impl(const any_fun_impl& rhs) : m_fun(rhs.m_fun) {}
			virtual any_fun*	clone() const {  return new any_fun_impl(*this);  }
		private:
			MF			m_fun;
		};
		
		template<class MF, class FT>
		class any_fun_impl<MF, FT, any_function_mem_fun_ref_tag, i_> : public any_fun_impl_base<FT>
		{
		public:
			typedef any_fun_impl_base<FT>				base_type;
			typedef typename base_type::result_type		result_type;
			typedef result_type							R;
			pp_repeat_binary_params_with_a_cat(i_, typedef typename base_type::P, P, ;)

			virtual R operator()(pp_enum_binary_params(i_, P, p) ) const
			{
				return (p0.*m_fun)(pp_enum_shifted_params(i_, p));
			}		

			any_fun_impl(typename make_parameter<MF>::type f) : m_fun(f) {}
			any_fun_impl(const any_fun_impl& rhs) : m_fun(rhs.m_fun) {}
			virtual any_fun*	clone() const {  return new any_fun_impl(*this);  }
		private:
			MF			m_fun;
		};
	}
}

#undef i_

#endif // pp_is_iterating