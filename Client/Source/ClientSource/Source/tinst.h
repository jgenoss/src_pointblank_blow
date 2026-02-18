#ifndef TEMPLATE_INSTANCE_H
#define TEMPLATE_INSTANCE_H

// 상속홀더 대신 래퍼홀더로 작성..

// 
// 1. tinst<I>;	의 전방 선언이 필요할수 있음.
// 2. friend tinst_creator<I>;  필요..
//
//struct true_type { static const bool value = true; };
//struct false_type { static const bool value = false; };

template<class I> struct is_tinst_manual : std::tr1::false_type {};

template<class I>
struct tinst_creator
{
	I*		create()
	{
		I * p = new I;
#if defined( I3_DEBUG)
		NetworkDump_Malloc(p, sizeof(I),__FILE__,__LINE__);
#endif
		return p;
	}
	void	destroy(I* i) { delete i; }
};

//
namespace detail
{
	template<bool is_manual, class T, class F>
	struct if_ {	typedef T type; 	};
	template<class T, class F>
	struct if_<false, T, F> { typedef F type; };
	
	template<class I>
	struct tinst_creator_manual {	I* create() { return nullptr; }	void destroy(I*) {}	};
}

template<class I>
class tinst
{
public:
	operator I* () { return ms_i; }
	operator const I* () const { return ms_i; }
	I*				operator->() { return ms_i; }
	const I*		operator->() const { return ms_i ;}
	I&				operator*() { return *ms_i; }
	const I&		operator*() const { return *ms_i; }
	
	tinst<I>&		operator=(I* rhs) { if ( rhs != ms_i ) _destroy(); ms_i = rhs; return *this; }	// optional function..
	static void		destroy_explicit() { _destroy(); }		// optional function..
private:
	typedef typename detail::if_<	is_tinst_manual<I>::value, 
									detail::tinst_creator_manual<I>, 
									tinst_creator<I> >::type		creator;

	static I*		_create() { ::atexit(&tinst<I>::_destroy);  return creator().create();   }
	static void		_destroy() { creator().destroy(ms_i); ms_i = nullptr; }
		
	static I*		ms_i;
};

template<class I> I*			tinst<I>::ms_i = tinst<I>::_create();






#endif