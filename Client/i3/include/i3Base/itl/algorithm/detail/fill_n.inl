#include <wchar.h>

namespace i3
{
	namespace detail
	{

		template<class It, class R>
		struct fill_n_disable_if_char : i3::disable_if<mpl::or_<	i3::is_same<It, char*>,
			i3::is_same<It, signed char*>,
			i3::is_same<It, unsigned char*>,
			i3::is_same<It, wchar_t*> >, R>
		{
		};

		template<class OutIt, class Diff,	class T> inline typename fill_n_disable_if_char<OutIt,
		void>::type  fill_n(OutIt f, Diff count, const T& val)
		{	
			for (; 0 < count; --count, ++f)
				*f = val;
		}

		inline void fill_n( char * f, size_t count, int val)	{	::memset(f, val, count);  }
		inline void fill_n( signed char * f, size_t count, int val)	{	::memset(f, val, count); }
		inline void fill_n( unsigned char * f, size_t count, int val){	::memset(f, val, count); }
		inline void fill_n( wchar_t * f, size_t count, wchar_t val){	::wmemset(f, val, count); }
	}

	template<class OutIt, class Diff, class T> inline
	void fill_n(OutIt f, Diff count, const T& val)
	{
		detail::fill_n(f, count, val);		
	}

	template<class OutIt, class T> inline
	void fill_1(OutIt f, const T& val)
	{
		*f = val; 
	}
}
