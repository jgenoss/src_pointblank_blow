#if !defined(pp_is_iterating)
#ifndef rtti_cast_ext_h
#define rtti_cast_ext_h

#define rtti_compare_arity_limit			10			// 2~10°³..

#define pp_iteration_flags_same_of			0
#define pp_iteration_flags_kind_of			1


#define pp_iteration_params_1 (3, (2, rtti_compare_arity_limit, "rtti_cast_ext.h") )
#include pp_iterate()


#undef pp_iteration_flags_kind_of
#undef pp_iteration_flags_same_of


#endif // rtti_cast_ext_h

#else // pp_is_iterating

#if			pp_iteration_depth() == 1

#define		i_			pp_frame_iteration(1)

namespace i3
{
	template< pp_enum_params_1(i_, class Ptr), class T>
	bool	same_of( const T* p ) 
	{  
		if (!p) return false;
		typedef detail::get_dynamic_rtti_info<T, typename get_rtti_tag<T>::type>	src_f;
		typename src_f::result_type	rtti_var = src_f::call(p);

		#define pp_iteration_params_2 (4, (1, i_, "rtti_cast_ext.h", pp_iteration_flags_same_of))
		#include pp_iterate()
		return false;
	}
	
	template< pp_enum_params_1(i_, class Ptr), class T>
	bool	kind_of( const T* p ) 
	{  
		if (!p) return false;
		typedef detail::get_dynamic_rtti_info<T, typename get_rtti_tag<T>::type>	src_f;
		typename src_f::result_type	rtti_var = src_f::call(p);

		#define pp_iteration_params_2 (4, (1, i_, "rtti_cast_ext.h", pp_iteration_flags_kind_of))
		#include pp_iterate()
		return false;
	}
	
}

#undef i_

#elif			pp_iteration_depth() == 2

#define	j_			pp_frame_iteration(2)

		typedef typename i3::remove_pointer<pp_cat(Ptr,j_)>::type	pp_cat(U,j_);	
		typedef detail::get_static_rtti_info<pp_cat(U,j_), typename get_rtti_tag<pp_cat(U,j_)>::type> pp_cat(compare_f,j_);
		
#if		pp_iteration_flags() == pp_iteration_flags_same_of
		if ( rtti_var->same_of( pp_cat(compare_f,j_)::call() ) ) return true;
#else	// pp_iteration_flags() == pp_iteration_flags_kind_of
		if ( rtti_var->kind_of( pp_cat(compare_f,j_)::call() ) ) return true;
#endif	

#undef	j_

#endif		//	pp_iteration_depth()

#endif // pp_is_iterating

