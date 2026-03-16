#if !defined(pp_is_iterating)

#ifndef mpl_bind_h
#define mpl_bind_h

#include "mpl_bind_fwd.h"
#include "mpl_placeholders.h"
#include "mpl_next_prior.h"
#include "mpl_protect.h"
#include "mpl_apply_wrap.h"
#include "mpl_limit.h"
#include "mpl_na.h"

#include "../preprocessor/ext/pp_enum_params_1_with_a_cat.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class T, class Arg>
			struct replace_unnamed_arg {	typedef Arg		next;	typedef T		type;	};

			template<class Arg>
			struct replace_unnamed_arg< mpl::arg< -1 >, Arg > {  typedef typename Arg::next next;   typedef Arg type;   };
			
			
			template<class T, pp_enum_params_1(mpl_arity_limit, class U) >
			struct resolve_bind_arg
			{
				typedef T type;
			};

			template<int N, pp_enum_params_1(mpl_arity_limit, class U) >
			struct resolve_bind_arg< arg<N>, pp_enum_params_1(mpl_arity_limit, U) >
			{
				typedef typename pp_cat(apply_wrap, mpl_arity_limit)<mpl::arg<N>, pp_enum_params_1(mpl_arity_limit, U) >::type type;
			};
			
			template<class F, pp_enum_params_1(mpl_arity_limit, class T), pp_enum_params_1(mpl_arity_limit, class U) >
			struct resolve_bind_arg< bind< F, pp_enum_params_1(mpl_arity_limit, T) >, pp_enum_params_1(mpl_arity_limit, U) >
			{
				typedef bind< F, pp_enum_params_1(mpl_arity_limit, T) > f_;
				typedef typename pp_cat(apply_wrap, mpl_arity_limit)< f_, pp_enum_params_1(mpl_arity_limit, U) >::type type;
			};
		}
	}

}

#define		pp_iteration_params_1	(3, (0, mpl_arity_limit, "mpl_bind.h"))				// 1단계 파일반복..
#include	pp_iterate()

namespace i3
{
	namespace mpl
	{
		// quote3라는 if_ / eval_if 버전...
		// if_/eval_if specializations

		template< template< class T1, class T2, class T3 > class F, class Tag >
		struct quote3;

		template< class T1, class T2, class T3 > struct if_;

		template< class Tag, class T1, class T2, class T3>
		struct bind3< quote3< if_,Tag >, T1, T2, T3>
		{
			template< pp_enum_params_1_with_a_default(mpl_arity_limit, class U, na) >
			struct apply
			{
			 private:
				typedef mpl::arg<1> n1;
				typedef aux::replace_unnamed_arg< T1,n1 > r1;
				typedef typename r1::type a1;
				typedef typename r1::next n2;
				typedef aux::resolve_bind_arg< a1, pp_enum_params_1(mpl_arity_limit, U) > t1;
				///
				typedef aux::replace_unnamed_arg< T2,n2 > r2;
				typedef typename r2::type a2;
				typedef typename r2::next n3;
				typedef aux::resolve_bind_arg< a2, pp_enum_params_1(mpl_arity_limit, U) > t2;
				///
				typedef aux::replace_unnamed_arg< T3,n3 > r3;
				typedef typename r3::type a3;
				typedef typename r3::next n4;
				typedef aux::resolve_bind_arg< a3, pp_enum_params_1(mpl_arity_limit, U) > t3;
				///
				typedef typename if_< typename t1::type, t2, t3 >::type f_;
			public:
				typedef typename f_::type type;
			};
		};


		template< template< class T1, class T2, class T3 > class F, class Tag>
		struct quote3;

		template< class T1, class T2, class T3 > struct eval_if;

		template<class Tag, class T1, class T2, class T3>
		struct bind3< quote3< eval_if,Tag >, T1, T2, T3>
		{
			template<pp_enum_params_1_with_a_default(mpl_arity_limit, class U, na)>
		    struct apply
			{
			 private:
				typedef mpl::arg<1> n1;
				typedef aux::replace_unnamed_arg< T1,n1 > r1;
				typedef typename r1::type a1;
				typedef typename r1::next n2;
				typedef aux::resolve_bind_arg< a1, pp_enum_params_1(mpl_arity_limit, U) > t1;
				///
				typedef aux::replace_unnamed_arg< T2,n2 > r2;
				typedef typename r2::type a2;
				typedef typename r2::next n3;
				typedef aux::resolve_bind_arg< a2, pp_enum_params_1(mpl_arity_limit, U) > t2;
				///
				typedef aux::replace_unnamed_arg< T3,n3 > r3;
				typedef typename r3::type a3;
				typedef typename r3::next n4;
				typedef aux::resolve_bind_arg< a3, pp_enum_params_1(mpl_arity_limit, U) > t3;
				///
				typedef typename eval_if< typename t1::type, t2, t3>::type f_;
			 public:
				typedef typename f_::type type;
			};
		};
	}
}

#endif

#else	// pp_is_itrating

#if			pp_iteration_depth() == 1

#define i_			pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		// resolve_bind_arg 파일반복..
		namespace aux
		{
			template< class F pp_enum_trailing_params_1(i_, class T), pp_enum_params_1(mpl_arity_limit, class U) >
			struct resolve_bind_arg< pp_cat(bind, i_)<F pp_enum_trailing_params_1(i_, T)>, pp_enum_params_1(mpl_arity_limit, U) >
			{
				typedef pp_cat(bind, i_)<F pp_enum_trailing_params_1(i_, T) > f_;
				typedef typename pp_cat(apply_wrap, mpl_arity_limit)< f_, pp_enum_params_1(mpl_arity_limit, U)>::type type;
			};
		}

		// bind_N객체 파일반복...

		template<class F pp_enum_trailing_params_1(i_, class T)>
		struct pp_cat(bind, i_)
		{
			template< pp_enum_params_1_with_a_default(mpl_arity_limit, class U, na) >
			struct apply
			{
			private:
				typedef aux::replace_unnamed_arg< F, mpl::arg<1> > r0;
				typedef typename r0::type a0;
				typedef typename r0::next n1;
				typedef typename aux::resolve_bind_arg< a0,	pp_enum_params_1(mpl_arity_limit, U) >::type f_;
			
			#if i_ > 0		// T1이 들어가는 경우부터..
				#define pp_iteration_params_2 (3, (1, i_, "mpl_bind.h"))
				#include pp_iterate()
			#endif

            public:
				typedef typename pp_cat(apply_wrap, i_)<f_ pp_comma_if(i_) pp_enum_params_1_with_a_cat(i_, typename t, ::type) >::type type;		// 헐..
		    };
		};

		#if i_ < mpl_arity_limit				// 특화인 경우..
			template<class F pp_enum_trailing_params_1(i_, class T)>
			struct bind< F, pp_enum_params_1_spec(mpl_arity_limit, i_, T, na) >
				: pp_cat(bind, i_)<F pp_enum_trailing_params_1(i_, T)>
			{
			};
		#else
			template<class F, pp_enum_params_1(i_, class T) >
			struct bind : pp_cat(bind, i_)< F, pp_enum_params_1(i_, T) >
			{
			};
		#endif

	}
}

#undef		i_

#elif		pp_iteration_depth() == 2

#define		j_			pp_frame_iteration(2)
	
	// 아래는 매 bind_N 객체 내부의 typedef의 반복내용이다..
	typedef		aux::replace_unnamed_arg<pp_cat(T, j_), pp_cat(n, j_)>	pp_cat(r, j_);						// j_는 1부터 시작해야함..
	typedef		typename			pp_cat(r, j_)::type pp_cat(a, j_);									//
	typedef		typename			pp_cat(r, j_)::next pp_cat(n, pp_inc(j_));							//
	typedef		aux::resolve_bind_arg<pp_cat(a, j_), pp_enum_params_1(mpl_arity_limit, U)> pp_cat(t, j_);	//

#undef		j_


#endif		// pp_iteration_depth()
#endif		// pp_is_itrating
