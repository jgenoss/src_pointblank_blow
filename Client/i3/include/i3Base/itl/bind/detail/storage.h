#if !defined(pp_is_iterating)

#ifndef storage_h
#define storage_h

#include "../bind_arity_limit.h"

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

// 인자 1에 대해서는 직접 구현...
// 반복은 2부터 시작할것..


namespace i3
{
	namespace bi
	{
		template<class A1> 
		struct storage1
		{
			explicit storage1(A1 a1) : a1(a1) {}
			template<class V> void accept(V& v) const { visit_each(v, a1, 0); }
			A1	a1;
		};
		
		template<int I>
		struct storage1< arg<I> >
		{
			explicit storage1(arg<I>) {}
			template<class V> void accept(V&) const {}
			static arg<I> a1() { return arg<I>(); }
		};
		
		template<int I>
		struct storage1< arg<I>(*)() >
		{
			explicit storage1( arg<I>(*)() ) {}
			template<class V> void accept(V&) const {}
			static arg<I> a1() { return arg<I>(); }
		};
	}
}

#define		pp_iteration_params_1	(3, (2, bind_arity_limit, "storage.h"))				// 1단계 파일반복..
#include	pp_iterate()

#pragma warning(default: 4512) 
#pragma warning(pop)

#endif //storage_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)
#define dec_i	pp_dec(i_)

namespace i3
{
	namespace bi
	{
		template< pp_enum_params_1(i_, class A) > 
		struct pp_cat(storage, i_) : public pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) >
		{
			typedef pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) > inherited;
			pp_cat(storage, i_)( pp_enum_binary_params_1(i_, A, a) ) : 
				inherited( pp_enum_params_1(dec_i, a) ), pp_cat(a,i_)(pp_cat(a,i_)) {}

			template<class V> void accept(V& v) const { inherited::accept(v); visit_each(v, pp_cat(a,i_), 0); }
			
			pp_cat(A,i_)	pp_cat(a,i_);
		};
		
		template< pp_enum_params_1(dec_i, class A), int I>
		struct pp_cat(storage,i_)< pp_enum_params_1(dec_i,A), arg<I> > : public pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) >
		{
			typedef pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) > inherited;
			pp_cat(storage,i_)( pp_enum_binary_params_1(dec_i, A, a), arg<I> ) : inherited( pp_enum_params_1(dec_i, a) ) {}
			
			template<class V> void accept(V& v) const { inherited::accept(v); }
			
			static arg<I> pp_cat(a,i_)() { return  arg<I>(); }
		};

		template< pp_enum_params_1(dec_i, class A), int I>
		struct pp_cat(storage,i_)< pp_enum_params_1(dec_i,A), arg<I>(*)() > : public pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) >
		{
			typedef pp_cat(storage, dec_i)< pp_enum_params_1(dec_i, A) > inherited;
			pp_cat(storage,i_)( pp_enum_binary_params_1(dec_i, A, a), arg<I>(*)() ) : inherited( pp_enum_params_1(dec_i, a) ) {}
			
			template<class V> void accept(V& v) const { inherited::accept(v); }
			
			static arg<I> pp_cat(a,i_)() { return  arg<I>(); }
		};

		
	}
}

#undef dec_i
#undef i_


#endif // pp_is_iterating