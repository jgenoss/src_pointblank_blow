#pragma once


namespace i3
{
	namespace precision_mode 
	{
		enum type {	underpoint,	significant_digit, };
	}
}

namespace precision_mode = i3::precision_mode;

#include "detail/num_to_str_detail.h"

namespace i3
{
	namespace str_conv_option
	{
		bool	is_enable_comma();
		void	set_enable_comma(bool);
	
		bool	is_fill_zero();
		void	set_fill_zero(bool);
		
		precision_mode::type get_precision_mode(); 
		void	set_precision_mode(precision_mode::type t);

		void	set_precision(int num);
		int		get_precision();

	}
	
	template<class ValType>
	struct get_max_digit_buffer_size
	{
		typedef typename mpl::if_< is_integral<ValType>, mpl::int_<std::numeric_limits<ValType>::digits10 
			* (comma_sep_interval+1) / comma_sep_interval + 2>,
			mpl::int_<_CVTBUFSIZE> >::type type;
		static const int value = type::value;
	};
	


	template<class Integral, class Range> typename i3::enable_if< is_integral<Integral>,
	void>::type		integral_to_str( Integral val, Range& out);

	template<class Real, class Range> typename i3::enable_if< is_floating_point<Real>,
	void>::type		real_to_str( Real val, Range& out);

	

}