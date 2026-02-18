#pragma once

namespace i3
{

	typedef		long	bits32;

	namespace mpl
	{
		template<i3::bits32 Val> struct bits32 : mpl::integral_c<i3::bits32, Val> {};
	}
		
	
}