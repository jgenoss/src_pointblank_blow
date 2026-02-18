#pragma once

namespace i3
{
	namespace mpl 
	{
		template<bool b> struct bool_;
		
		typedef bool_<true>				true_;
		typedef bool_<false>			false_;
	}
}