#pragma once

namespace i3
{

	template<class T>
	struct is_stack_basic_string : i3::is_template_same< 
		i3::stack_basic_string<mpl_, mpl_, mpl_, mpl_>, T> {};

}
