#if !defined(pp_is_iterating)

#ifndef typed_in_place_factory_h
#define typed_in_place_factory_h

//
//  모듈 내부에서만 쓰는 템플릿 모듈이지만.. , inplace()함수는 바깥에서도 쓰일것이다..
//  boost와는 달리 apply함수 대신 함수호출 연산자를 쓰고, 매개변수 명기 강제가 되지 않도록 identity<>처리로 변경.
#include "../function_types/ft_arity_limit.h"
#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"
#include "../preprocessor/repetition/pp_enum_trailing_params.h"
#include "../mpl/mpl_identity.h"
#include "../tuple/tuple.h"

#pragma push_macro("new")

#undef new

namespace i3
{
	template< class CreationType, pp_enum_params_with_a_default(ft_arity_limit, class A, null_type) >		
	class typed_in_place_factory;

	// type_traits를 하나 만들어두어야한다..
	template<class T>
	struct is_typed_in_place_factory : i3::is_template_same< i3::typed_in_place_factory< pp_enum_datas(pp_inc(ft_arity_limit), mpl::_) >, T>
	{
	};

}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "typed_in_place_factory.h") )		// 0부터 반복임..
#include pp_iterate()


#pragma pop_macro("new")

#endif // typed_in_place_factory_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
#if i_ < ft_arity_limit
	template< class CreationType pp_enum_trailing_params(i_, class A)>
	class typed_in_place_factory< CreationType, pp_enum_params_spec(ft_arity_limit, i_, A, null_type) >  
#else
	template< class CreationType pp_enum_trailing_params(i_, class A)>
	class typed_in_place_factory
#endif
	{
	public:
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<A, >::type P, ;)
		typedef i3::tuple< pp_enum_params(i_, P) >							tuple_type;
		
		typed_in_place_factory( pp_enum_binary_params(i_, P, p) ) : m_args( pp_enum_params(i_, p) ) {}
		
		CreationType*		operator()( void* address ) const
		{
			return new (address) CreationType ( pp_enum_params_with_a_cat(i_, m_args.get<, >() ) );
		}
		
		CreationType*		operator()( void* address, std::size_t num_elem) const
		{
			CreationType* first = (*this)(address);
			for ( CreationType* next = first; num_elem > 1 ; --num_elem )
			{
				(*this)(++next);
			}
			return first;
		}

	private:
		tuple_type	m_args;
	};

	template<class C pp_enum_trailing_params(i_, class A) > inline 
	typed_in_place_factory<C pp_enum_trailing_params(i_, A)> in_place(i3::identity<C> pp_comma_if(i_) pp_enum_params_cat_params(i_, const A, &, a) )
	{
		return typed_in_place_factory<C pp_enum_trailing_params(i_, A)>( pp_enum_params(i_, a) );
	}

}

#undef  i_


#endif // pp_is_iterating
