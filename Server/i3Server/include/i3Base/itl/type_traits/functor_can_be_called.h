#if !defined(pp_is_iterating)

#ifndef functor_can_be_called_h
#define functor_can_be_called_h

#include "../mpl/mpl_list.h"

// 고민끝에 함수객체 참조형에 대한 성공결과를 실패로 바꾸기로 했다... (type_traits의 값질의의 경우 대체로 참조형은 실패로 놓기 때문)

#define functor_can_be_called_limit 20

namespace i3
{
	namespace detail 
	{	
		template<class Fun, class Seq> struct functor_can_be_called_impl : false_type {};
	}
	
	// A1 ~ A20 (1부터 시작)
	template<class Functor, pp_enum_params_1_with_a_default(functor_can_be_called_limit, class A, mpl::na) >
	struct functor_can_be_called : mpl::and_< is_class_or_union<Functor>, 
			detail::functor_can_be_called_impl<Functor, typename mpl::list<pp_enum_params_1(functor_can_be_called_limit, A)>::type> > {};

	namespace detail
	{
		#define functor_can_be_called_unroll	pp_inc(functor_can_be_called_limit)

		struct private_type	{	const private_type&	operator,(int) const; };
		template<class T> 
		no_type		is_private_type(const T& );
		yes_type	is_private_type(const private_type& );
		
		struct dont_care { dont_care(...); };

		template<int num_arg, class Fun> struct funwrap;

		// 되도록 잘게 분해하는 것이 다량 인스턴스화를 줄이는데 도움이 된다. 
		// Fun 참조형을 허용하면 오류가 생기므로 사전에 false리턴으로 차단..
		template<int num_arg, class Fun>
		struct funwrap_var
		{
			typedef typename mpl::if_< is_const<Fun>, const funwrap<num_arg, Fun>, funwrap<num_arg, Fun> >::type funwrap_type;	// 이것만 활용..
			static funwrap_type& fun;	 
		};

		
		#define		pp_iteration_params_1	(3, (0, functor_can_be_called_unroll, "functor_can_be_called.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
		#include	pp_iterate()

			
		#undef functor_can_be_called_unroll
	}

}

#endif // functor_can_be_called_h
	
#else  // pp_is_iteraing		

		#define		i_			pp_frame_iteration(1)
				

		template<class Fun>
		struct funwrap<i_, Fun> : Fun
		{
			funwrap();
			typedef		const private_type& (*ptr_to_fun)(pp_enum_datas(i_, dont_care));
			operator	ptr_to_fun() const;
		};
		
		template<class Fun pp_enum_trailing_params_1(i_, class A) >
		struct functor_can_be_called_impl<Fun, pp_cat(mpl::list, i_)< pp_enum_params_1(i_, A) > >
		{
			#define declare_ref_var(z, n, d)	static typename add_reference<pp_cat(A, pp_inc(n))>::type pp_cat(a, pp_inc(n));
			pp_repeat(i_, declare_ref_var, ~)
			#undef declare_ref_var
			
			typedef funwrap_var< i_, Fun>	funwrap_var_type;
			
			static const bool value = ( sizeof(no_type) == sizeof(is_private_type( ( funwrap_var_type::fun( pp_enum_params_1(i_, a) ), 0) ) )  );
			typedef mpl::bool_<value> type; 
		};
		
		#undef i_
				
	

#endif // pp_is_iterating

	// 다음은 부스트에서 찾은 can_be_called의 원본 코드임..
	/*
	template<class Fun, class A, class B>
	struct can_be_called;

	struct dont_care { dont_care(...); };
	struct private_type	{	const private_type&	operator,(int) const; };
	template<class T>
	no_type		is_private_type(const T& );
	yes_type	is_private_type(const private_type& );
	template<class Fun>
	struct funwrap2 : Fun 
	{
	funwrap2();
	typedef const private_type& (*pointer_to_function)(dont_care,dont_care);
	operator pointer_to_function() const;
	};
	template<class Fun, class A, class B>
	struct can_be_called
	{
	static funwrap2<Fun>&	fun;
	static typename add_reference<A>::type			a;				// A& ---> add_reference<A>::type 변경..
	static typename add_reference<B>::type			b;				// B& ---> add_reference<B>::type 변경..
	enum { value = sizeof(no_type) == sizeof(is_private_type( ( fun(a, b), 0) ) ) };
	};
	*/	



