#if !defined(pp_is_iterating)

#ifndef tuple_h
#define tuple_h

// i3::tuple이 boost::tuple과 다른 더 좋은 점...
// 1. mpl::vector로 재구성 (컴파일실행시간단축)
// 2. 포함대신, 순차열/인덱스조합 베이스로 단일 상속을 써서 계층화..
// 3. get함수 접근이 직접적 ...( 최소한 디버그모드실행퍼포먼스는 더 나음)
// 4. 튜플 요소인자 초기화에서 인자가 모자라거나, 인자가 오버되도 잘 실행되게 변경..
// 5. 0사이즈를 포함, 튜플간 복사/대입시 요소갯수가 서로 다르더라도 잘 실행되게 변경...
// 6. 인덱스 이외에 타입으로도 값조회가 가능하게 처리..

//
// 단점 : tuple의 개선버전일 뿐이고, boost의 후속컨테이너 fusion에 비해서는 구현이 소박하다고 볼수 있다..
//

#include "../mpl/mpl_vector.h"
#include "../mpl/mpl_at.h"
#include "../mpl/mpl_size.h"
#include "../mpl/mpl_identity.h"
#include "../mpl/mpl_index_of.h"

#include "../type_traits/make_reference_result.h"
#include "../type_traits/make_const_reference_result.h"
#include "../type_traits/call_traits.h"
#include "../utility/swallow.h"

#include "../preprocessor/ext/pp_enum_params_cat_params.h"

#define tuple_limit 20

namespace i3
{

	template< pp_enum_params_with_a_default( tuple_limit, class T, mpl::na) >
	class tuple;
	
#define tuple_constructor_implementation			0x00
#define tuple0_constructor_implementation			0x01
	
	namespace detail 
	{
		template<class T>
		struct make_tuple_parameter : mpl::if_< is_same<T, mpl::na>, mpl::identity<const swallow_copy&>, make_parameter<T> >::type {};
	}	
	
	namespace detail {	template <class Seq, int Index> class tuple_impl; }
	
	template< pp_enum_params( tuple_limit, class T) >
	class tuple : public detail::tuple_impl<typename mpl::vector<pp_enum_params(tuple_limit, T)>::type, 
											mpl::size< typename mpl::vector<pp_enum_params(tuple_limit, T)>::type >::value - 1 >	// 1이상이 조건..
	{
	public:
		typedef typename mpl::vector<pp_enum_params(tuple_limit, T)>::type		sequence_type;
		static const int	sequence_size = mpl::size< sequence_type >::value;
		typedef detail::tuple_impl< sequence_type, sequence_size - 1 >			base_type;
		
		tuple() {}
		tuple(typename detail::make_tuple_parameter<T0>::type p0) : base_type(p0) {}
		tuple(typename detail::make_tuple_parameter<T0>::type p0, typename detail::make_tuple_parameter<T1>::type p1) : base_type(p0, p1) {}
		tuple(typename detail::make_tuple_parameter<T0>::type p0, typename detail::make_tuple_parameter<T1>::type p1, typename detail::make_tuple_parameter<T2>::type p2) : base_type(p0, p1, p2) {}
		
		#define		pp_iteration_params_1	(4, (4, tuple_limit, "tuple.h", tuple_constructor_implementation))				// 1단계 파일반복..
		#include	pp_iterate()
		
		tuple(const tuple& rhs) : base_type(rhs.base_type::get_this()) {}
		
		tuple(const tuple<>&) {}


		template<class Seq2, int Index2>		// Index2와 base_type의 인덱스를 비교해서..가장 작은 놈들로 피팅.. // 여기서는 rhs는 조정가능..
		tuple(const detail::tuple_impl<Seq2, Index2>& rhs ) : 
			base_type( rhs.detail::tuple_impl<Seq2, (Index2<sequence_size) ? Index2 : (sequence_size-1) >::get_this()) { }

		tuple&		operator=(const tuple& rhs) { base_type::assign_impl(rhs.base_type::get_this()); return *this; }
		tuple&		operator=(const tuple<>&) { return *this; }

		template<class Seq2, int Index2>
		tuple&		operator=(const detail::tuple_impl<Seq2, Index2>& rhs)	// 생성자보다는 처리가 쉬울 것임..모두 피팅..
		{	 
			static const int min_index = (Index2 < sequence_size) ? Index2 : (sequence_size - 1);
			detail::tuple_impl<sequence_type, min_index>::assign_impl( rhs.detail::tuple_impl<Seq2, min_index>::get_this() ); return *this; 
		}
		
		template<class U1, class U2>
		tuple&		operator=(const i3::pair<U1, U2>& rhs) 
		{
			static const int min_index = ( 1 < sequence_size ) ? 1 : (sequence_size - 1);
			detail::tuple_impl<sequence_type, min_index>::assign_impl(rhs);		return *this;
		}
		
		template<int N>
		struct element {	typedef typename mpl::at_c<sequence_type, N>::type	type; };
		

		template<int N>
		typename make_reference_result< typename element<N>::type>::type get() 
		{
			return detail::tuple_impl<sequence_type, N>::get_data();
		}

		template<int N>
		typename make_const_reference_result< typename element<N>::type>::type get() const 
		{
			return detail::tuple_impl<sequence_type, N>::get_data();
		}
		
		template<class Data>
		typename make_reference_result<Data>::type get() 
		{ 
			return detail::tuple_impl<sequence_type, mpl::index_of<sequence_type, Data>::type::value>::get_data(); 
		}

		template<class Data>
		typename make_const_reference_result<Data>::type	 get() const 
		{ 
			return detail::tuple_impl<sequence_type, mpl::index_of<sequence_type, Data>::type::value>::get_data(); 
		}
	};

	// 빈 튜플 특화 // 튜플 복사생성, 복사 대입의 경우 무시하게 처리...
	template<>
	class tuple<> 
	{ 
	public:
		static const int	sequence_size = 0;

		tuple() {}

		#define		pp_iteration_params_1	(4, (1, tuple_limit, "tuple.h", tuple0_constructor_implementation))				// 1단계 파일반복..
		#include	pp_iterate()

		tuple(const tuple&) {}
		tuple& operator=(const tuple&) { return *this; }
		
		template<class Seq2, int Index2>		
		tuple(const detail::tuple_impl<Seq2, Index2>& ) {}
		
		template<class Seq2, int Index2>
		tuple& operator=(const detail::tuple_impl<Seq2, Index2>& ) { return *this; }

		template<class U1, class U2>
		tuple&		operator=(const i3::pair<U1, U2>& rhs) { return *this; }
	};

	//
	
#undef tuple0_constructor_implementation
#undef tuple_constructor_implementation

}

#include "detail/tuple_impl.h"
#include "tuple_function.h"
#include "tuple_ext.h"
#include "tuple_comparison.h"

#endif // tuple_h

#else // pp_is_iterating

#if			pp_iteration_depth() == 1

#define		i_		pp_frame_iteration(1)

#if			pp_iteration_flags() == tuple_constructor_implementation

		tuple( pp_enum_params_cat_params(i_, typename detail::make_tuple_parameter<T, >::type, p) ) : base_type( pp_enum_params(i_, p) ) {}

#elif		pp_iteration_flags() == tuple0_constructor_implementation

		tuple( pp_enum_datas(i_, const swallow_copy&) ) {}

#endif		// pp_iteration_flags()

#undef		i_

#endif // pp_iteration_depth() == 1

#endif // pp_is_iterating

