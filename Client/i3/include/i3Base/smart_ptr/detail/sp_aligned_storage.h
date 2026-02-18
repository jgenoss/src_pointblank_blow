#pragma once

#include "../../itl/type_traits/type_with_alignment.h"

namespace i3
{
	namespace detail
	{
		
		template<std::size_t Size, std::size_t Align> 
		struct sp_aligned_storage
		{
			union type
			{
				char data[Size];
				typename i3::type_with_alignment<Align>::type aligned_data;
			};
		};
	}
}

