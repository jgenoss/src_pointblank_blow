//#include <math.h>

namespace i3
{

	namespace detail
	{
		template <int n> struct choose_initial_n 
		{
			enum { c = ( unsigned long(1)<<n<<n ) != 0  };
			static const int value = !c * n + choose_initial_n< 2*c*n >::value;
		};
		template <> struct choose_initial_n<0> {  static const int value = 0;  };
		
		const int n_zero = 16;
		const int initial_n = choose_initial_n<n_zero>::value;

		template <unsigned long x, int n = initial_n>
		struct static_log2_impl
		{
			enum { c = (x >> n) > 0 };
			static const int value = c*n + (static_log2_impl< (x>>c*n), n/2 >::value);
		};
		template <> struct static_log2_impl<1, 0> { static const int value = 0;  };
		
	}

	template < unsigned long x>
	struct static_log2
	{
		static const int value = detail::static_log2_impl<x>::value;
	};

	template<> struct static_log2<0>{};
	
	
	
}