#if !defined(pp_is_iterating)

#ifndef factory_creator_h
#define factory_creator_h

#pragma push_macro("new")

#undef new

namespace i3
{
	namespace detail
	{
		template<class CreateFun>		// R (P...) 형태..
		struct factory_creator;
		template<class CreateFun, class D>
		struct factory_creator_derived;
		template<class CreateFun, class CustomFun>
		struct factory_creator_custom;
		template<class CreateFun>
		struct factory_creator_na;
			
	}
}

#define		pp_iteration_params_1	(3, (0, factory_arity_limit, "factory_creator.h"))		// 0 ~ 5까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

#pragma pop_macro("new")

#endif

#else	// pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class R pp_enum_trailing_params(i_, class P) >		// R (P...) 형태..
		struct factory_creator<R (pp_enum_params(i_, P))> 
		{
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) = 0;
#if defined(_DEBUG)
			virtual R operator()(pp_enum_binary_params(i_, P, p) pp_comma_if(i_) const char* file, int line) = 0;		// 디버깅용도로 꼭 필요함..
#endif
			virtual ~factory_creator() {}
		};

		template<class R pp_enum_trailing_params(i_, class P), class D>
		struct factory_creator_derived<R (pp_enum_params(i_, P)), D> : i3::class_common_pool<factory_creator_derived<R (pp_enum_params(i_, P)), D>, factory_creator<R (pp_enum_params(i_, P))> >
		{
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) { return new D(pp_enum_params(i_, p)); }				// default 구현...
#if defined(_DEBUG)
			virtual R operator()(pp_enum_binary_params(i_, P, p) pp_comma_if(i_) const char* file, int line) { 	return new (file, line) D(pp_enum_params(i_, p) ); }				// default 구현...
#endif
		};

		template<class R pp_enum_trailing_params(i_, class P), class CustomFun>
		struct factory_creator_custom<R (pp_enum_params(i_, P)), CustomFun> : i3::class_common_pool<factory_creator_custom<R (pp_enum_params(i_, P)), CustomFun>, factory_creator<R (pp_enum_params(i_, P))> >
		{
			factory_creator_custom(CustomFun f) : m_f(f) {}
			virtual R operator()(pp_enum_binary_params(i_, P, p)) { return m_f(pp_enum_params(i_, p)); }
#if defined(_DEBUG)
			virtual R operator()(pp_enum_binary_params(i_, P, p) pp_comma_if(i_) const char* file, int line) { return m_f(pp_enum_params(i_, p) pp_comma_if(i_) file, line);  }
#endif
			CustomFun m_f;
		};

		template<class R pp_enum_trailing_params(i_, class P)>
		struct factory_creator_na<R (pp_enum_params(i_, P))> : i3::class_common_pool<factory_creator_na<R (pp_enum_params(i_, P))>, factory_creator<R (pp_enum_params(i_, P))> >
		{
			virtual R operator()(pp_enum_binary_params(i_, P, p) ) { return nullptr; }				// 추상클래스의 경우 실행은 될수 있도록..
#if defined(_DEBUG)
			virtual R operator()(pp_enum_binary_params(i_, P, p) pp_comma_if(i_) const char* , int ) { 	return nullptr; }				// 추상클래스의 경우 실행은 될수 있도록..
#endif
		};

	}	
	
}

#undef i_

#endif