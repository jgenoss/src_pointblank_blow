#if !defined(pp_is_iterating)

#ifndef bind_list_h
#define bind_list_h

#include "bind_util.h"
#include "storage.h"

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

namespace i3
{
	namespace bi
	{
		class list0
		{
		public:
			list0() {}
			template<class T> T& operator[](value<T>& v) const { return v.get(); }
			template<class T> const T& operator[](const value<T>& v) const { return v.get(); }
			template<class T> T& operator[](const reference_wrapper<T>& v) const { return v.get(); }
			template<class R, class F, class L>
			typename result_traits<R,F>::type operator[](bind_t<R,F,L>& b) const { return b.eval(*this); }
			template<class R, class F, class L>
			typename result_traits<R,F>::type operator[](const bind_t<R,F,L>& b) const { return b.eval(*this); }
			
			template<class R, class F, class A>
			R operator()(type<R>, F& f, A&, long) { return unwrapper<F>::unwrap(f, 0)(); }
			
			template<class R, class F, class A>
			R operator()(type<R>, const F& f, A&, long) const { return unwrapper<const F>::unwrap(f, 0)(); }

			template<class F, class A>
			void operator()(type<void>, F& f, A&, int) { unwrapper<F>::unwrap(f, 0)(); }
			template<class F, class A>
			void operator()(type<void>, const F& f, A&, int) const { unwrapper<const F>::unwrap(f, 0)(); }

			template<class V> void accept(V&) const {}
			bool operator==(const list0&) const { return true; }
		};
		
		struct logical_and;
		struct logical_or;
	}
}

#pragma warning(push)
#pragma warning(disable:4100)		// 1~N까지 반복준비..

#define bind_flag_array_op			0x00
#define bind_flag_compare_op		0x01

#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "bind_list.h"))				// 1단계 파일반복..
#include	pp_iterate()

#undef  bind_flag_compare_op
#undef  bind_flag_array_op

#pragma warning(pop)



#pragma warning(default: 4512) 
#pragma warning(pop)


#endif // bind_list_h

#else // pp_is_iterating

#if				pp_iteration_depth() == 1

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace bi
	{
		template< pp_enum_params_1(i_, class A) >
		class pp_cat(list, i_) : private pp_cat(storage, i_)< pp_enum_params_1(i_, A) >
		{
		public:
			typedef pp_cat(storage, i_)< pp_enum_params_1(i_, A) >	base_type;

			explicit pp_cat(list, i_)( pp_enum_binary_params_1(i_,A,a) ) : base_type( pp_enum_params_1(i_, a) ) {}
			
			#define	 pp_iteration_params_2	(4, (1, i_, "bind_list.h", bind_flag_array_op))
			#include pp_iterate()
			
			template<class T> T&		operator[](value<T>& v) const { return v.get(); }
			template<class T> const T&	operator[](const value<T>& v) const { return v.get(); }
			template<class T> T& operator[](const reference_wrapper<T>& v) const { return v.get(); }
			template<class R, class F, class L>
			typename result_traits<R,F>::type operator[](bind_t<R,F,L>& b) const { return b.eval(*this); }
			template<class R, class F, class L>
			typename result_traits<R,F>::type operator[](const bind_t<R,F,L>& b) const { return b.eval(*this); }
		

			template<class R, class F, class A>
			R operator()(type<R>, F& f, A& a, long) 
			{ 
				return unwrapper<F>::unwrap(f, 0)( pp_enum_params_1_with_a_cat(i_, a[base_type::a, ]) ); 
			}
			
			template<class R, class F, class A>
			R operator()(type<R>, const F& f, A& a, long) const 
			{ 
				return unwrapper<const F>::unwrap(f, 0)(pp_enum_params_1_with_a_cat(i_, a[base_type::a, ]) ); 
			}

			template<class F, class A>
			void operator()(type<void>, F& f, A& a, int) 
			{ 
				unwrapper<F>::unwrap(f, 0)(pp_enum_params_1_with_a_cat(i_, a[base_type::a, ]) ); 
			}
			
			template<class F, class A>
			void operator()(type<void>, const F& f, A& a, int) const 
			{ 
				unwrapper<const F>::unwrap(f, 0)(pp_enum_params_1_with_a_cat(i_, a[base_type::a, ]) ); 
			}
			
#if i_ == 2
			template<class A> bool operator()( type<bool>, logical_and& , A & a, int )
			{
				return a[base_type::a1] && a[base_type::a2];
			}

			template<class A> bool operator()( type<bool>, const logical_and& /*f*/, A & a, int ) const
			{
				return a[base_type::a1] && a[base_type::a2];
			}

			template<class A> bool operator()( type<bool>, logical_or&, A & a, int )
			{
				return a[base_type::a1] || a[base_type::a2];
			}

			template<class A> bool operator()( type<bool>, const logical_or&, A & a, int ) const
			{
				return a[base_type::a1] || a[base_type::a2];
			}
#endif // i_ == 2

			template<class V> void accept(V& v) const { base_type::accept(v); }

			bool operator==(const pp_cat(list, i_)& rhs) const
			{
				#define	 pp_iteration_params_2	(4, (1, i_, "bind_list.h", bind_flag_compare_op))
				#include pp_iterate()
			}
						
		};


	}
}

#undef i_

#elif		pp_iteration_depth() == 2

#define j_		pp_frame_iteration(2)			// 1부터 반복되어야함.. 
	
#if			pp_iteration_flags() == bind_flag_array_op

			pp_cat(A, j_) operator[](arg<j_>) const 
			{ 
				return pp_cat(base_type::a, j_); 
			}
			
			pp_cat(A, j_) operator[]( arg<j_>(*)() ) const 
			{ 
				return pp_cat(base_type::a, j_); 
			}

#elif		pp_iteration_flags() == bind_flag_compare_op
			
			#if j_ == 1
			return 
			#endif
					ref_compare(pp_cat(base_type::a, j_), pp_cat(rhs.a, j_), 0) 
			#if j_ < i_ 
			&&
			#else
			;
			#endif 

#endif		// pp_iteration_flags

#undef  j_

#endif	// pp_iteration_depth() == 1 or 2


#endif	// pp_is_iterating	
