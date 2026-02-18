#if !defined(pp_is_iterating)

#ifndef mem_fn_h
#define mem_fn_h 

#ifndef mem_fn_enable_cc			// cc를 원한다면 번역단위 최초에서 인클루드 전에 1로 지정합니다...
#define mem_fn_enable_cc  0
#endif 

#include "../preprocessor/ext/pp_enum_trailing_binary_params_1.h"
#include "../preprocessor/ext/pp_enum_params_1_cat_params.h"

#define mem_fn_arity_limit 10		// 다소 작게 잡는 것으로 결정..

namespace i3
{
	namespace mfi
	{
		template<class T> class mf;						// 디폴트는 __thiscall일것이다..
		template<class T> class cmf;	
	}
}

#define mem_fn_cc
#define		pp_iteration_params_1	(3, (0, mem_fn_arity_limit, "mem_fn.h"))				// 1단계 파일반복..
#include	pp_iterate()
#undef  mem_fn_cc

#if		mem_fn_enable_cc > 0

namespace i3
{
	namespace mfi
	{
		template<class T> class mf__cdecl;
		template<class T> class mf__stdcall;
		template<class T> class mf__fastcall;

		template<class T> class cmf__cdecl;
		template<class T> class cmf__stdcall;
		template<class T> class cmf__fastcall;
	}
}

#define mem_fn_cc	__cdecl
#define		pp_iteration_params_1	(3, (0, mem_fn_arity_limit, "mem_fn.h"))				// 1단계 파일반복..
#include	pp_iterate()
#undef  mem_fn_cc 

#define mem_fn_cc	__stdcall
#define		pp_iteration_params_1	(3, (0, mem_fn_arity_limit, "mem_fn.h"))				// 1단계 파일반복..
#include	pp_iterate()
#undef  mem_fn_cc 

#define mem_fn_cc  __fastcall
#define		pp_iteration_params_1	(3, (0, mem_fn_arity_limit, "mem_fn.h"))				// 1단계 파일반복..
#include	pp_iterate()
#undef  mem_fn_cc  

#endif // mem_fn_enable_cc > 0

// 멤버포인터에 대한 호환성 버전이 추가된다...

namespace i3
{
	namespace mfi
	{
		template<class R, class T>
		class mp
		{
		public:
			typedef const R&	result_type;
			typedef const T*	argument_type;
			typedef mp			this_type;

			typedef R (T::*F);				// 멤버포인터 타입..

			explicit this_type(F f): m_f(f) {}

			R& operator()(T* p) const {	return (p->*m_f); }
			const R& operator()(const T* p) const { return (p->*m_f); }
			template<class U> const R& operator()(const U& u) const { return call(u, &u); }
			R& operator()(T& t) const {	return (t.*m_f); }
			const R& operator()(const T& t) const {	return (t.*m_f); }

			bool operator==(const this_type& rhs) const { return m_f == rhs.m_f; }
			bool operator!=(const this_type& rhs) const { return m_f != rhs.m_f; }
		private:
			template<class U> 
			const R& call(U& u, const T*) const {	return (u.*m_f);	}
			template<class U> 
			const R& call(U& u, const void*) const { return (get_pointer(u)->*m_f);	}
			
			F m_f;
		};
	}

	template<class R, class T> 
	mfi::mp<R, T> mem_fn(R T::*f) { return mfi::mp<R, T>(f);  }

}

#endif // mem_fn_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)


namespace i3
{
	namespace mfi 
	{	
		template<class R, class TPointer pp_enum_trailing_params_1(i_, class A)>
		class pp_cat(mf, mem_fn_cc)< R (TPointer pp_enum_trailing_params_1(i_, A) ) >
		{
		public:
			typedef R	result_type;

		#if		i_ == 0
			typedef TPointer	argument_type;
		#elif	i_ == 1
			typedef TPointer	first_argument_type;
			typedef A1	second_argument_type;
		#endif
			
			typedef typename i3::remove_pointer<TPointer>::type			T;

			typedef pp_cat(mf, mem_fn_cc)	this_type;
			typedef R (mem_fn_cc T::*F)(pp_enum_params_1(i_, A) );
			
			explicit this_type(F f) : m_f(f) {}

			R operator()(T* p pp_enum_trailing_binary_params_1(i_, A, a) ) const 
			{ return (p->*m_f)(pp_enum_params_1(i_, a));  }
			
			template<class U>
			R operator()(U& u pp_enum_trailing_binary_params_1(i_, A, a) ) const 
			{ const U* p = 0; return call(u, p pp_enum_trailing_params_1(i_, a)); }

			template<class U>
			R operator()(const U& u pp_enum_trailing_binary_params_1(i_, A, a) ) const 
			{ const U* p = 0; return call(u, p pp_enum_trailing_params_1(i_, a)); }

			R operator()(T& t pp_enum_trailing_binary_params_1(i_, A, a) ) const
			{ return (t.*m_f)(pp_enum_params_1(i_, a));  }

			bool operator==(const this_type& rhs) const { return m_f == rhs.m_f; }
			bool operator!=(const this_type& rhs) const { return m_f != rhs.m_f; }

		private:

			template<class U pp_enum_trailing_params_1(i_, class B)>
			R call(U& u, const T* pp_comma_if(i_) pp_enum_params_1_cat_params(i_, B,&, b) ) const
			{ return (u.*m_f)( pp_enum_params_1(i_, b) ); }
			
			template<class U pp_enum_trailing_params_1(i_, class B)>
			R call(U& u, const void* pp_comma_if(i_) pp_enum_params_1_cat_params(i_, B,&, b) ) const
			{ return (get_pointer(u)->*m_f)( pp_enum_params_1(i_, b) ); }

			F		m_f;
		};


		template<class R, class TPointer pp_enum_trailing_params_1(i_, class A)>
		class pp_cat(cmf, mem_fn_cc)< R (TPointer pp_enum_trailing_params_1(i_, A) ) >	// const T를 함수const로 간주..
		{
		public:
			typedef R	result_type;

		#if		i_ == 0
			typedef const TPointer	argument_type;
		#elif	i_ == 1
			typedef const TPointer	first_argument_type;
			typedef A1			second_argument_type;
		#endif
			
			typedef typename i3::remove_pointer<TPointer>::type		T;

			typedef pp_cat(cmf, mem_fn_cc)	this_type;
			typedef R (mem_fn_cc T::*F)(pp_enum_params_1(i_, A)) const;
			
			explicit this_type(F f) : m_f(f) {}

			template<class U>
			R operator()(const U& u pp_enum_trailing_binary_params_1(i_, A, a) ) const 
			{ const U* p = 0; return call(u, p pp_enum_trailing_params_1(i_, a)); }

			R operator()(const T& t pp_enum_trailing_binary_params_1(i_, A, a) ) const
			{ return (t.*m_f)(pp_enum_params_1(i_, a));  }

			bool operator==(const this_type& rhs) const { return m_f == rhs.m_f; }
			bool operator!=(const this_type& rhs) const { return m_f != rhs.m_f; }

		private:

			template<class U pp_enum_trailing_params_1(i_, class B)>
			R call(U& u, const T* pp_comma_if(i_) pp_enum_params_1_cat_params(i_, B,&, b) ) const
			{ return (u.*m_f)( pp_enum_params_1(i_, b) ); }
			
			template<class U pp_enum_trailing_params_1(i_, class B)>
			R call(U& u, const void* pp_comma_if(i_) pp_enum_params_1_cat_params(i_, B,&, b) ) const
			{ return (get_pointer(u)->*m_f)( pp_enum_params_1(i_, b) ); }

			F		m_f;
		};

	}

	template<class R, class T pp_enum_trailing_params_1(i_, class A)>
	pp_cat(mfi::mf, mem_fn_cc)< R (T pp_enum_trailing_params_1(i_, A) )> mem_fn( R (mem_fn_cc T::*f)(pp_enum_params_1(i_, A)) ) 
	{
		return pp_cat(mfi::mf, mem_fn_cc)< R (T* pp_enum_trailing_params_1(i_, A) )>(f);
	}
	
	template<class R, class T pp_enum_trailing_params_1(i_, class A)>
	pp_cat(mfi::cmf, mem_fn_cc)< R (T pp_enum_trailing_params_1(i_, A) )> mem_fn( R (mem_fn_cc T::*f)(pp_enum_params_1(i_, A)) const ) 
	{
		return pp_cat(mfi::cmf, mem_fn_cc)< R (T* pp_enum_trailing_params_1(i_, A) )>(f);
	}

}

#undef i_

#endif // pp_is_iterating
