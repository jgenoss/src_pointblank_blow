#pragma once

namespace i3
{
	namespace intrusive
	{
		namespace detail
		{

			template<class ConstReference>
			class equal_to_value
			{
				ConstReference t_;
			public:
				equal_to_value(ConstReference t) : t_(t) {}
				bool operator()(ConstReference t) const {  return t_ == t;   }
			};
		}
	}
}
