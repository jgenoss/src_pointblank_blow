#if !defined(pp_is_iterating)

#ifndef variant_detail_substitute_h
#define variant_detail_substitute_h

namespace i3
{
namespace detail 
{
	namespace variant 
	{
		
		template<class T, class Dest, class Source>
		struct substitute {		typedef T type; };

		template<class Dest, class Source>
		struct substitute< Source, Dest, Source> {	typedef Dest type;	};		// T에서 Src를 찾게되면 Dest로 변경..
		template<class Dest, class Source>
		struct substitute< const Source, Dest, Source> { typedef const Dest type;	};		// T에서 Src를 찾게되면 Dest로 변경..
		template<class Dest, class Source>
		struct substitute< volatile Source, Dest, Source> { typedef volatile Dest type;	};		// T에서 Src를 찾게되면 Dest로 변경..
		template<class Dest, class Source>
		struct substitute< const volatile Source, Dest, Source> { typedef const volatile Dest type;	};		// T에서 Src를 찾게되면 Dest로 변경..
		
		template<class T, class Dest, class Source>
		struct substitute< T*, Dest, Source> 
		{
			typedef typename substitute<T, Dest, Source>::type*		type;		// T*에서 Src*를 찾으면 Dest*로 변경...
		};
		template<class T, class Dest, class Source>
		struct substitute< T* const, Dest, Source> 
		{
			typedef typename substitute<T, Dest, Source>::type* const	type;		// T*에서 Src*를 찾으면 Dest*로 변경...
		};
		template<class T, class Dest, class Source>
		struct substitute< T* volatile, Dest, Source> 
		{
			typedef typename substitute<T, Dest, Source>::type* volatile	type;		// T*에서 Src*를 찾으면 Dest*로 변경...
		};
		template<class T, class Dest, class Source>
		struct substitute< T* const volatile, Dest, Source> 
		{
			typedef typename substitute<T, Dest, Source>::type* const volatile	type;		// T*에서 Src*를 찾으면 Dest*로 변경...
		};
		
		template<class T, class Dest, class Source>
		struct substitute< T&, Dest, Source> 
		{
			typedef typename substitute<T, Dest, Source>::type&		type;		// T&에서 Src&를 찾으면 Dest&로 변경...
		};
		
		// 함수포인터인 경우..인수와 리턴타입모두에 대해 재귀적으로 처리..
		template<class R, class Dest, class Source>
		struct substitute< R (*)(void), Dest, Source> 
		{
			typedef typename substitute< R, Dest, Source >::type	r;
			typedef r (*type)(void);
		};
		
	}
}

}


#define pp_iteration_params_1 (3, (1, mpl_arity_limit, "substitute.h") )
#include pp_iterate()

#endif // variant_detail_substitute_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
namespace detail
{
	namespace variant
	{
		// T가 템플릿 클래스인 경우의 처리.. 

		template< template< pp_enum_datas(i_, class) > class Template, pp_enum_params(i_, class U), class Dest, class Source>
		struct substitute< Template<pp_enum_params(i_, U) >, Dest, Source>
		{

#define repeat_substitute_params(z, n, d)		typedef typename substitute<pp_cat(U, n), Dest, Source>::type	pp_cat(u, n);
			pp_repeat(i_, repeat_substitute_params, ~)
#undef  repeat_substitute_params
			typedef Template< pp_enum_params(i_, u) > type;
		};
		
		template<class R, pp_enum_params(i_, class U), class Dest, class Source>
		struct substitute< R (*)( pp_enum_params(i_, U)), Dest, Source>
		{
#define repeat_substitute_params(z, n, d)		typedef typename substitute<pp_cat(U, n), Dest, Source>::type	pp_cat(u, n);
			pp_repeat(i_, repeat_substitute_params, ~)
#undef  repeat_substitute_params			
			typedef typename substitute< R, Dest, Source >::type	r;

			typedef r (*type)(pp_enum_params(i_, u) );			
		};

	}
}

}


#undef i_

#endif // pp_is_iterating