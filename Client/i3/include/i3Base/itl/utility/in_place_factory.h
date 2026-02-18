#if !defined(pp_is_iterating)

#ifndef in_place_factory_h
#define in_place_factory_h

//
//  모듈 내부에서만 쓰는 템플릿 모듈이지만.. , inplace()함수는 바깥에서도 쓰일것이다..
//  boost와는 달리 apply함수 대신 함수호출 연산자를 쓰고, 매개변수 명기 강제가 되지 않도록 identity<>처리로 변경.
//
#include "../function_types/ft_arity_limit.h"
#include "../preprocessor/ext/pp_repeat_binary_params_with_a_cat.h"
#include "../mpl/mpl_identity.h"
#include "../tuple/tuple.h"

#pragma push_macro("new")

#undef new

namespace i3
{
	template< pp_enum_params_with_a_default(ft_arity_limit, class A, null_type) >		
	class in_place_factory;

	// type_traits를 하나 만들어두어야한다..
	template<class T>
	struct is_in_place_factory : i3::is_template_same< i3::in_place_factory< pp_enum_datas(ft_arity_limit, mpl::_) >, T>
	{
	};

}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "in_place_factory.h") )		// 0부터 반복임..
#include pp_iterate()

#pragma pop_macro("new")

#endif // in_place_factory_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{

#if i_ < ft_arity_limit
	template< pp_enum_params(i_, class A)>
	class in_place_factory< pp_enum_params_spec(ft_arity_limit, i_, A, null_type) >  
#else
	template< pp_enum_params(i_, class A)>								// 전체 매개변수가 다 들어가면 특화가 아님..
	class in_place_factory
#endif
	{
	public:
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<A, >::type P, ;)
		typedef i3::tuple< pp_enum_params(i_, P) >							tuple_type;
		
		in_place_factory( pp_enum_binary_params(i_, P, p) ) : m_args( pp_enum_params(i_, p) ) {}
		
		template<class CreationType> 
		CreationType*		operator()( i3::identity<CreationType>, void* address) const
		{
			return new (address) CreationType ( pp_enum_params_with_a_cat(i_, m_args.get<, >() ) );
		}
		
		template<class CreationType>
		CreationType*		operator()( i3::identity<CreationType> t, void* address, std::size_t num_elem) const
		{
			CreationType* first = (*this)(t, address);
			for ( CreationType* next = first; num_elem > 1 ; --num_elem )
			{
				(*this)(t, ++next);
			}
			return first;
		}

	private:
		tuple_type	m_args;
	};

#if i_ > 0
	template<pp_enum_params(i_, class A) > inline 
	in_place_factory<pp_enum_params(i_, A)>	in_place( pp_enum_params_cat_params(i_, const A, &, a) )
	{
		return in_place_factory<pp_enum_params(i_, A)>( pp_enum_params(i_, a) );
	}
#else
	inline in_place_factory<>	in_place()
	{
		return in_place_factory<>();
	}
	
#endif
}

#undef  i_

#endif // pp_is_iterating

