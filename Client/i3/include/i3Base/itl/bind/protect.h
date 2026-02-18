#if !defined(pp_is_iterating)

#ifndef bind_protect_h
#define bind_protect_h

namespace i3
{
	namespace bi
	{
		template<class F> 
		class protected_bind_t
		{
		public:
			typedef typename F::result_type result_type;
			explicit protected_bind_t(F f) : m_f(f) {}
			
			result_type operator()() { return m_f(); }
			result_type operator()() const { return m_f(); }
			template<class A1, class A2> result_type operator()(const A1& a1, A2 & a2) { return m_f(a1, a2); }
			template<class A1, class A2> result_type operator()(const A1& a1, A2 & a2) const {	return m_f(a1, a2);	}
			template<class A1, class A2> result_type operator()(A1& a1, const A2& a2) {	return m_f(a1, a2); }
			template<class A1, class A2> result_type operator()(A1& a1, const A2& a2) const { return m_f(a1, a2); }
			
			#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "protect.h"))				// 1단계 파일반복..
			#include	pp_iterate()

		private:
			F m_f;
		};

	}

	template<class F> __forceinline
	bi::protected_bind_t<F> protect(F f) {	return bi::protected_bind_t<F>(f); }

}


#endif // bind_protect_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

			template<pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_binary_params_1(i_, A,& a) ) { return m_f(pp_enum_params_1(i_, a)); }
			
			template<pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_binary_params_1(i_, A,& a) ) const { return m_f(pp_enum_params_1(i_, a)); }

			template<pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_binary_params_1(i_, const A,& a) ) { return m_f(pp_enum_params_1(i_, a)); }
			
			template<pp_enum_params_1(i_, class A)>
			result_type operator()( pp_enum_binary_params_1(i_, const A,& a) ) const { return m_f(pp_enum_params_1(i_, a)); }

#undef i_

#endif // pp_is_iterating
