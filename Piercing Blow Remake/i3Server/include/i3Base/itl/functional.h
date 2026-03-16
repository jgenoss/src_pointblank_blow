#pragma once 
#include <typeinfo>
namespace i3 
{
	template<class Arg, class R>
	struct unary_function 
	{	
		typedef Arg argument_type;	
		typedef R result_type; 
	};
	
	template<class Arg1, class Arg2, class R>
	struct binary_function
	{	
		typedef Arg1 first_argument_type;
		typedef Arg2 second_argument_type;
		typedef R result_type;
	};
	
	template<class T>
	struct less	: public binary_function<T, T, bool> {	bool operator()(const T& Left, const T& Right) const	{	return Left < Right; }  };
	
	template<class T>
	struct equal_to : public binary_function<T, T, bool> {	bool operator()(const T& Left, const T& Right) const	{	return Left == Right;	} };

	// 
	// type_info pointer에 대한 비교식 템플릿 오버로드..
	//
	template<>
	struct equal_to<const std::type_info*> : public binary_function<const std::type_info*, const std::type_info*, bool>
	{
		bool operator()(const std::type_info* Left, const std::type_info* Right) const{	return *Left == *Right;	} 
	};
	//

	// 이외에 무수이 많은 STL연산자들이 있음...(나중에 추가할것)

	// mem_fun

	template<class R, class T>
	class mem_fun_t : public unary_function<T*, R>
	{	
	public:
		explicit mem_fun_t(R (T::*Pm)()) : Pmemfun(Pm) {}
		R operator()(T* Pleft) const {	return (Pleft->*Pmemfun)();	}
	private:
		R (T::*Pmemfun)();	
	};

	template<class R, class T, class Arg>
	class mem_fun1_t : public binary_function<T *, Arg, R>
	{	
	public:
		explicit mem_fun1_t(R (T::*Pm)(Arg)) : Pmemfun(Pm) {}
		R operator()(T * Pleft, Arg Right) const {	 return (Pleft->*Pmemfun)(Right);	}
	private:
		R (T::*Pmemfun)(Arg);
	};
	
	template<class R, class T>
	class const_mem_fun_t : public unary_function<const T*, R>
	{	
	public:
		explicit const_mem_fun_t(R (T::*Pm)() const) : Pmemfun(Pm)	{ }
		R operator()(const T* Pleft) const	{	return (Pleft->*Pmemfun)();	}
	private:
		R (T::* Pmemfun)() const;	
	};
	
	template<class R, class T, class Arg>
	class const_mem_fun1_t : public binary_function<const T *, Arg, R>
	{	
	public:
		explicit const_mem_fun1_t(R (T::*Pm)(Arg) const) : Pmemfun(Pm) { }
		R operator()(const T * Pleft, Arg Right) const	{	return (Pleft->*Pmemfun)(Right); }
	private:
		R (T::*Pmemfun)(Arg) const;	
	};
		
	template<class R, class T> inline
	mem_fun_t<R, T> mem_fun(R (T::*Pm)()) {	  return i3::mem_fun_t<R, T>(Pm); }

	template<class R, class T, class Arg> inline
	mem_fun1_t<R, T, Arg> mem_fun(R (T::*Pm)(Arg)) {	return i3::mem_fun1_t<R, T, Arg>(Pm);	}

	template<class R, class T> inline
	const_mem_fun_t<R, T> mem_fun(R (T::*Pm)() const) {	 return i3::const_mem_fun_t<R, T>(Pm); }

	template<class R, class T, class Arg> inline
	const_mem_fun1_t<R, T, Arg> mem_fun(R (T::*Pm)(Arg) const) {	return i3::const_mem_fun1_t<R, T, Arg>(Pm);	}

	// bind1st & bind2nd

	template<class Fn2>
	class binder1st : public unary_function<typename Fn2::second_argument_type,	typename Fn2::result_type>
	{	
	public:
		typedef unary_function<typename Fn2::second_argument_type, typename Fn2::result_type> base_type;
		typedef typename base_type::argument_type	argument_type;
		typedef typename base_type::result_type		result_type;

		binder1st(const Fn2& Func, const typename Fn2::first_argument_type& Left) : op(Func), value(Left) {}
		result_type operator()(const argument_type& Right) const {	return op(value, Right); }
		result_type operator()(argument_type& Right) const {	return op(value, Right); }
	protected:
		Fn2 op;	
		typename Fn2::first_argument_type value;	
	};
	
	// 레퍼런스 인자를 위한 버전..
	template< template<class, class, class> class mf, class R, class T, class Arg>
	class binder1st< mf<R, T, Arg&> > : public unary_function< Arg&, R>
	{
	public:
		typedef mf<R, T, Arg&>							Fn2;
		typedef unary_function< Arg&, R>				base_type;
		typedef typename base_type::argument_type		argument_type;
		typedef typename base_type::result_type			result_type;
		
		binder1st(const Fn2& Func, const typename Fn2::first_argument_type& Left) : op(Func), value(Left) {}
		result_type operator()(argument_type Right) const {	return op(value, Right); }
	protected:
		Fn2 op;	
		typename Fn2::first_argument_type value;
	};



	template<class Fn2, class T> inline
	binder1st<Fn2> bind1st(const Fn2& Func, const T& Left)
	{	
		typename Fn2::first_argument_type Val(Left);
		return i3::binder1st<Fn2>(Func, Val);
	}

	template<class Fn2>
	class binder2nd : public unary_function<typename Fn2::first_argument_type, typename Fn2::result_type>
	{	
	public:
		typedef unary_function<typename Fn2::first_argument_type, typename Fn2::result_type> base_type;
		typedef typename base_type::argument_type	argument_type;
		typedef typename base_type::result_type		result_type;

		binder2nd(const Fn2& Func, const typename Fn2::second_argument_type& Right)	: op(Func), value(Right) {}

		result_type operator()(const argument_type& Left) const {	return op(Left, value); }
		result_type operator()(argument_type& Left) const {	return op(Left, value); }
	protected:
		Fn2 op;	
		typename Fn2::second_argument_type value;
	};


	// 레퍼런스 인자를 위한 버전..
	template< template<class, class, class> class mf, class R, class T, class Arg>
	class binder2nd< mf<R, T, Arg&> > : public unary_function< Arg&, R>
	{
	public:
		typedef mf<R, T, Arg&>							Fn2;
		typedef unary_function< Arg&, R>	base_type;
		typedef typename base_type::argument_type		argument_type;
		typedef typename base_type::result_type			result_type;

		binder2nd(const Fn2& Func, const typename Fn2::second_argument_type& Right) : op(Func), value(Right) {}
		result_type operator()(argument_type Left) const {	return op(Left, value); }
	protected:
		Fn2 op;	
		typename Fn2::second_argument_type value;
	};


	template<class Fn2, class T> inline
	binder2nd<Fn2> bind2nd(const Fn2& Func, const T& Right)
	{	
		typename Fn2::second_argument_type Val(Right);
		return i3::binder2nd<Fn2>(Func, Val);
	}
	
	// TEMPLATE CLASS unary_negate, not1	// ebo가 가능할듯도 싶은데..
	template<class Fn1>
	class unary_negate : public unary_function<typename Fn1::argument_type, bool>
	{	
	public:		explicit unary_negate(const Fn1& Func) : m_functor(Func) {}
				bool operator()(const typename Fn1::argument_type& Left) const { return !m_functor(Left); }
	protected:	Fn1 m_functor;	
	};
	template<class Fn1> inline unary_negate<Fn1> not1(const Fn1& Func) {	return i3::unary_negate<Fn1>(Func); }


	// TEMPLATE CLASS binary_negate, not2
	template<class Fn2>
	class binary_negate	: public binary_function<typename Fn2::first_argument_type, typename Fn2::second_argument_type, bool>
	{	
	public:		explicit binary_negate(const Fn2& Func) : m_functor(Func) {}
				bool operator()(const typename Fn2::first_argument_type& Left, const typename Fn2::second_argument_type& Right) const
				{	return !m_functor(Left, Right);	}
	protected:	Fn2 m_functor;	// the functor to apply
	};
	template<class Fn2> inline binary_negate<Fn2> not2(const Fn2& Func) {	return i3::binary_negate<Fn2>(Func);  }

	
	// TEMPLATE CLASS pointer_to_unary_function
	template<class arg, class result, class fun = result(*)(arg) >
	class pointer_to_unary_function : public unary_function<arg, result>
	{
	public: explicit pointer_to_unary_function(fun f) : m_f(f) {}
			result operator()(arg a) const { return m_f(a); }
	protected:		fun		m_f;
	};
	
	// TEMPLATE CLASS pointer_to_binary_function
	template<class arg1, class arg2, class result, class fun = result(*)(arg1, arg2) >
	class pointer_to_binary_function : public binary_function<arg1, arg2, result>
	{
	public:	explicit pointer_to_binary_function(fun f) : m_f(f) {}
			result operator()(arg1 a1, arg2 a2) const { return m_f(a1, a2); }
	protected:	fun m_f;
	};

	// TEMPLATE FUNCTION ptr_fun
	//template<class arg, class result> __forceinline
	//pointer_to_unary_function<arg, result, result (__cdecl*)(arg)> ptr_fun( result (__cdecl* f)(arg) )
	//{
	//	return i3::pointer_to_unary_function<arg, result, result (__cdecl*)(arg)>(f);
	//}
	//template<class arg, class result> __forceinline
	//pointer_to_unary_function<arg, result, result (__stdcall*)(arg)> ptr_fun( result (__stdcall* f)(arg) )
	//{
	//	return i3::pointer_to_unary_function<arg, result, result (__stdcall*)(arg)>(f);
	//}
	//	
	//template<class arg1, class arg2, class result> __forceinline
	//pointer_to_binary_function<arg1, arg2, result, result (__cdecl*)(arg1, arg2)> ptr_fun( result (__cdecl* f)(arg1, arg2) )
	//{
	//	return i3::pointer_to_binary_function<arg1, arg2, result, result (__cdecl*)(arg1, arg2)>(f);
	//}
	//template<class arg1, class arg2, class result> __forceinline
	//pointer_to_binary_function<arg1, arg2, result, result (__stdcall*)(arg1, arg2)> ptr_fun( result (__stdcall* f)(arg1, arg2) )
	//{
	//	return i3::pointer_to_binary_function<arg1, arg2, result, result (__stdcall*)(arg1, arg2)>(f);
	//}
	

}