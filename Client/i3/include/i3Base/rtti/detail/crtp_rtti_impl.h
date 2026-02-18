#pragma once

namespace i3
{
	template<class T>
	struct crtp_rtti_initializer
	{
		crtp_rtti_initializer()
		{
			T::create_explicit();
		}
	};
}
