#pragma once

namespace i3
{

	template<class T>
	struct is_i3_basic_string : i3::is_template_same< 
		i3::basic_string<mpl_, mpl_, mpl_>, T> {};

}
