#pragma once

namespace i3
{
	namespace bi
	{
		// 아래 구현은 ADL에 의해 호출될수 있음..
		template<class R, class F, class L>
		bool function_equal(const bind_t<R,F,L>& a, const bind_t<R,F,L>& b) { return a.compare(b); }
	}
}
