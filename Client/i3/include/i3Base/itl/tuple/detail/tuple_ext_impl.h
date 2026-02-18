#pragma once

#include "../../mpl/mpl_push_front.h"
#include "../../mpl/mpl_reverse_iter_fold.h"
#include "../../mpl/mpl_list.h"
#include "../../type_traits/functor_can_be_called.h"

namespace i3
{
	
	namespace detail
	{
		// 4개씩 루프를 풀어서 구현.. (퓨전쪽 구현이 이렇게 되있음)
		template<bool is_enable_call>
		struct tuple_call
		{
			template<int N, class Tuple, class Functor> __forceinline static void call(Tuple& t, Functor& f) { 	f(t.get<N>()); }
		};

		template<>
		struct tuple_call<false>
		{
			template<int N, class Tuple, class Functor> __forceinline static void call(Tuple& , Functor& ) {}
		};

		template<int N, class Tuple, class Functor> __forceinline
		void tuple_function_call( Tuple& t, Functor& f)  
		{
			static const bool is_enable_call =	i3::functor_can_be_called<Functor, typename tuple_element<N, Tuple>::type>::value;
			tuple_call<is_enable_call>::call<N>(t, f);
		}

		template<std::size_t Cnt>
		struct tuple_for_each
		{
			template<int N, class Tuple, class Functor> __forceinline			// const여부는 바깥 템플릿 함수에서 결정됨..
			static void visit( Tuple& t, Functor& f ) 
			{
				tuple_function_call<N>(t, f);
				tuple_function_call<N+1>(t, f);
				tuple_function_call<N+2>(t, f);
				tuple_function_call<N+3>(t, f);
				//f(t.get<N>());
				//f(t.get<N+1>());
				//f(t.get<N+2>());
				//f(t.get<N+3>());
				
				tuple_for_each<Cnt-4>::visit<N+4>(t, f);
			};
		};
		
		template<>
		struct tuple_for_each<3>
		{
			template<int N, class Tuple, class Functor> __forceinline			// const여부는 바깥 템플릿 함수에서 결정됨..
			static void visit( Tuple& t, Functor& f ) 
			{
				tuple_function_call<N>(t, f);
				tuple_function_call<N+1>(t, f);
				tuple_function_call<N+2>(t, f);
//				f(t.get<N>());
//				f(t.get<N+1>());
//				f(t.get<N+2>());
			};
		};

		template<>
		struct tuple_for_each<2>
		{
			template<int N, class Tuple, class Functor> __forceinline			// const여부는 바깥 템플릿 함수에서 결정됨..
			static void visit( Tuple& t, Functor& f ) 
			{
				tuple_function_call<N>(t, f);
				tuple_function_call<N+1>(t, f);
//				f(t.get<N>());
//				f(t.get<N+1>());
			};
		};
		
		template<>
		struct tuple_for_each<1>
		{
			template<int N, class Tuple, class Functor> __forceinline			// const여부는 바깥 템플릿 함수에서 결정됨..
			static void visit( Tuple& t, Functor& f ) 
			{
				tuple_function_call<N>(t, f);
//				f(t.get<N>());
			};
		};

		template<>
		struct tuple_for_each<0>
		{
			template<int N, class Tuple, class Functor> __forceinline			// const여부는 바깥 템플릿 함수에서 결정됨..
			static void visit(Tuple& , Functor& ) {}
		};

		template<class Tuple, class Functor> __forceinline
		void for_each_tuple(Tuple& t, Functor& f)
		{
			detail::tuple_for_each<tuple_size<Tuple>::value>::visit<0>(t, f);
		}

	}
	
	namespace detail
	{
		// list iterator를 활용하는게 더 좋을것 같다..	
		// count_if, replace_if류등을 보고 람다자리표를 변경하는 과정을 활용...
		
		template< class Pred>		// 최외곽 인자..
		struct tuple_element_if_pred
		{
			template< class SeqResult, class Iter>		// 내부에 쓰이는 인자로 고정됨..자리표 _1이 N(State), 자리표 _2는 시퀀스원소..
			struct apply : mpl::eval_if< typename mpl::apply1<Pred, typename mpl::deref<Iter>::type>::type, 
				mpl::push_front<SeqResult, Iter>, mpl::identity<SeqResult> > {};
		};
		
		template<class Begin, class End>
		struct tuple_for_each_if
		{
			template<class Tuple, class Functor> __forceinline 
			static void visit( Tuple& t, Functor& f ) 
			{
				typedef typename Tuple::sequence_type	sequence;
				typedef typename mpl::distance<mpl::begin<sequence>::type, mpl::deref<Begin>::type>::type   dist_type;
				
				tuple_function_call<dist_type::value>(t, f);
//				f(t.get<dist_type::value>());
				tuple_for_each_if< typename mpl::next<Begin>::type, End>::visit(t, f);
			}
		};

		template<class End>
		struct tuple_for_each_if<End, End>
		{
			template<class Tuple, class Functor> __forceinline 
			static void visit( Tuple& , Functor& ) {}
		};

		template<class MetaPred, class Tuple, class Functor> __forceinline
		void for_each_tuple_if(Tuple& t, Functor& f)
		{
			typedef typename Tuple::sequence_type	seq;
			typedef typename mpl::reverse_iter_fold< seq, mpl::list<>,  mpl::protect<detail::tuple_element_if_pred<MetaPred> > >::type iter_list;  
			detail::tuple_for_each_if<typename mpl::begin<iter_list>::type, typename mpl::end<iter_list>::type>::visit(t, f);
		}


	}
	
}