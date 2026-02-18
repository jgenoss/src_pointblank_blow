#include <wchar.h>

namespace i3
{
	namespace detail
	{
		template<class It, class R>
		struct fill_disable_if_char : i3::disable_if<mpl::or_<	i3::is_same<It, char*>,
																i3::is_same<It, signed char*>,
																i3::is_same<It, unsigned char*>,
																i3::is_same<It, wchar_t*> >, R>
		{
		};

		template<class FwdIt, class T> inline typename fill_disable_if_char<FwdIt,
		void>::type  fill(FwdIt f, FwdIt l, const T& val)
		{	
			for (; f != l; ++f) *f = val;
		}	
		inline void  fill( char *f, char *l, int val){	::memset(f, val, l - f);	}
		inline void  fill( signed char *f, signed char *l, int val)	{	::memset(f, val, l - f);	}
		inline void  fill( unsigned char *f, unsigned char *l, int val)	{	::memset(f, val, l - f);	}
		inline void  fill( wchar_t *f, wchar_t *l, wchar_t val)	{	::wmemset(f, val, l - f);	}
	}
	
	template<class FwdIt, class T> inline
	void	fill(FwdIt f, FwdIt l, const T& val)
	{
		detail::fill(f, l, val);
	}
}
