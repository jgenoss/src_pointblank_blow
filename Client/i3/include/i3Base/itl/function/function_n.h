#if !defined(pp_is_iterating)


#ifndef function_n_h
#define function_n_h

// TODO : 비교연산자 보강 
// 일반적으로 Ctn<T> -> CtnN<t0,t1..> 으로 접근하지만, 이경우 반대로 접근 .. CtnN<t0,t1..> --> Ctn<T>
// ( 이유 ) : (1) 템플릿 형식인자의 수가 임의적이면, pp파일반복/pp_enum 특화가 자주 발생..--> (대안) 순차열 혹은 함수형을 통해, 템플릿형식인자의 수를 줄인다..
//			  (2) 클래스의 접미어가 있으면 일반함수에서 pp파일반복을 강제하는 경향..	  --> (대안) 접미어 없는 base클래스로 대신 처리
//

#include "function.h"

namespace i3
{
	template<class R = void>
	class function0;
}

#pragma warning(disable : 4717) // 제어경로 재귀 경고를 내는데, 실제로 그렇지 않음..
#define pp_iteration_params_1 (3, (0, ft_arity_limit, "function_n.h") )		// 0부터 반복임..
#include pp_iterate()
#pragma warning(default : 4717)

#endif // function_n_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)
 
namespace i3
{
	template< class R pp_enum_trailing_params(i_, class T)> 
	class pp_cat(function, i_) : public function< R (pp_enum_params(i_, T)) >
	{
	public:
		typedef pp_cat(function, i_)									my_type;
		typedef function< R (pp_enum_params(i_, T)) >				base_type;
		
		my_type() {}
		my_type(clear_type*) : base_type() {}
		my_type(const my_type& rhs) : base_type(rhs) {}
		my_type(const base_type& rhs) : base_type(rhs) {}
		template<class F> my_type( F f, typename disable_if< is_integral<F>, int>::type = 0) : base_type(f) {}

		template<class Obj, class F> my_type(Obj* obj, F f) : base_type(obj, f) {}
		template<class Obj, class F> my_type(Obj& obj, F f) : base_type(obj, f) {}
		
		my_type&	operator=(clear_type*) { clear(); return *this; }
		my_type&	operator=(const my_type& rhs) { assign_impl(rhs);	return *this; }
		my_type&	operator=(const base_type& rhs) { assign_impl(rhs); return *this; }

		template<class F> typename disable_if< is_integral<F>,
		my_type&>::type operator=(F f) { assign_impl(f);		return *this; }

		template<class Obj, class F> my_type& assign(Obj* obj, F f) { assign_impl(obj, f);	return *this; }
		template<class Obj, class F> my_type& assign(Obj& obj, F f) { assign_impl(obj, f);	return *this; }
	};

	template< class R pp_enum_trailing_params(i_, class T)> inline 
	void swap( pp_cat(function, i_)<R pp_enum_trailing_params(i_,T)>& lhs,  pp_cat(function, i_)<R pp_enum_trailing_params(i_,T)>& rhs)
	{
		lhs.swap(rhs);
	}

}

#undef i_

#endif // pp_is_iterating