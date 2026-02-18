#if !defined(pp_is_iterating)

#ifndef bind_t_h
#define bind_t_h

#include "bind_list.h"

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

namespace i3
{
	namespace bi
	{
		template<class R, class F, class L>
		class bind_t
		{
		public:
			typedef bind_t								this_type;
			typedef typename result_traits<R, F>::type	result_type;

			bind_t(F f, const L& l) : m_f(f), m_l(l) {}
			
			result_type operator()() { list0 a; return m_l(type<result_type>(), m_f, a, 0); }
			result_type operator()() const { list0 a; return m_l(type<result_type>(), m_f, a, 0); }

			#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "bind_t.h"))				// 1단계 파일반복..
			#include	pp_iterate()
			
			// 2개인수의 경우 추가오버로딩이 4개 더 있음..
			template<class A1, class A2>
			result_type operator()(const A1& a1, A2& a2) { list2<const A1&,A2&> a(a1, a2); return m_l(type<result_type>(), m_f, a, 0); }
			template<class A1, class A2>
			result_type operator()(const A1& a1, A2& a2) const { list2<const A1&,A2&> a(a1, a2); return m_l(type<result_type>(), m_f, a, 0); }
			template<class A1, class A2>
			result_type operator()(A1& a1, const A2& a2) { list2<A1&,const A2&> a(a1, a2); return m_l(type<result_type>(), m_f, a, 0); }
			template<class A1, class A2>
			result_type operator()(A1& a1, const A2& a2) const { list2<A1&,const A2&> a(a1, a2); return m_l(type<result_type>(), m_f, a, 0); }
			
			template<class A> result_type eval(A& a) { return m_l(type<result_type>(), m_f, a, 0); }
			template<class A> result_type eval(A& a) const { return m_l(type<result_type>(), m_f, a, 0); }
			
			template<class V> void accept(V& v) const 
			{
				using i3::visit_each;	visit_each(v, m_f, 0);
				m_l.accept(v);
			}
			
			bool compare(const this_type& rhs) const { return ref_compare(m_f, rhs.m_f, 0) && m_l == rhs.m_l; }
		private:
			F	m_f;
			L	m_l;
		};
	}
}

#pragma warning(default: 4512) 
#pragma warning(pop)


#endif // bind_t_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

			template< pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_params_1_cat_params(i_,A,&, a) )
			{
				pp_cat(list,i_)< pp_enum_params_1_with_a_cat(i_,A,&)> a( pp_enum_params_1(i_, a) );
				return m_l(type<result_type>(), m_f, a, 0);
			}
			
			template< pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_params_1_cat_params(i_,A,&, a) ) const
			{
				pp_cat(list,i_)< pp_enum_params_1_with_a_cat(i_,A,&)> a( pp_enum_params_1(i_, a) );
				return m_l(type<result_type>(), m_f, a, 0);
			}

			template< pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_params_1_cat_params(i_,const A,&, a) )
			{
				pp_cat(list,i_)< pp_enum_params_1_with_a_cat(i_,const A,&)> a( pp_enum_params_1(i_, a) );
				return m_l(type<result_type>(), m_f, a, 0);
			}
			
			template< pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_params_1_cat_params(i_,const A,&, a) ) const
			{
				pp_cat(list,i_)< pp_enum_params_1_with_a_cat(i_,const A,&)> a( pp_enum_params_1(i_, a) );
				return m_l(type<result_type>(), m_f, a, 0);
			}


#undef  i_

#endif	// pp_is_iterating	
